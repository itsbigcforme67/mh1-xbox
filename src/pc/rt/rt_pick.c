/*
 * rt_pick.c - draw tags and the id table of the in-game bug reporter (F8, src/pc/pick.c).
 *
 * See rt_pick.h. Everything here only does something while gfx_pick_pass is set, i.e. while a frozen frame is
 * drawn once more for the id buffer. The game state snapshot (pick_game_json) is read once per report.
 */
#include "rt.h"
#include "types.h"
#include "rt_pick.h"
#include "../fl/fl.h"

#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *rt_host_symname(const void *addr, unsigned *off);   /* rt_symtab.c */
extern u8 game_w[];
extern u8 select_w[];
extern u8 em_work[];
extern u8 player_work[];
int rt_seed_get(void);
int rt_log_tick(void);

int gfx_pick_pass;
uint32_t (*gfx_pick_cb)(const gfx_pick_info *);

static pick_tag_t cur;
static pick_entry_t *ent;
static int nent, capent;
#define MAX_ENTRIES 0xFFFF00

static struct { unsigned char set, kind; short a, b, c; } handles[1024];

const char *pick_kind_name(int k)
{
    static const char *const n[PK_N] = { "other", "stage", "sky", "set", "monster", "hunter", "npc", "weapon", "effect",
                                         "shell", "prim", "hud", "text", "movie", "fade" };
    return k >= 0 && k < PK_N ? n[k] : "?";
}

void pick_tag_full(int kind, int a, int b, int c, int d, const void *fn, const void *obj, const float *pos)
{
    memset(&cur, 0, sizeof cur);
    cur.kind = kind;
    cur.a = a;
    cur.b = b;
    cur.c = c;
    cur.d = d;
    cur.fn = fn;
    cur.obj = obj;
    cur.clay_handle = -1;
    if (pos) {
        memcpy(cur.pos, pos, sizeof cur.pos);
        cur.has_pos = 1;
    }
}

void pick_note_handle(int h, int kind, int a, int b, int c)
{
    if (h < 0 || h >= 1024)
        return;
    handles[h].set = 1;
    handles[h].kind = (unsigned char)kind;
    handles[h].a = (short)a;
    handles[h].b = (short)b;
    handles[h].c = (short)c;
}

/* flExecuteClay(handle): the area / set model parts have their own tag, anything else keeps the enclosing one */
void pick_tag_handle(int h)
{
    if (h >= 0 && h < 1024 && handles[h].set)
        pick_tag_full(handles[h].kind, handles[h].a, handles[h].b, handles[h].c, 0, 0, 0, 0);
    cur.clay_handle = h;
}

static uint32_t reg(const gfx_pick_info *gi)
{
    pick_entry_t *e;
    if (nent >= MAX_ENTRIES)
        return 0;
    if (nent == capent) {
        capent = capent ? capent * 2 : 2048;
        ent = (pick_entry_t *)realloc(ent, (size_t)capent * sizeof *ent);
        if (!ent) {
            capent = nent = 0;
            return 0;
        }
    }
    e = &ent[nent++];
    e->tag = cur;
    e->gi = *gi;
    if (!e->tag.has_pos && !gi->is2d) {         /* the draw's world matrix translation */
        e->tag.pos[0] = gi->world[12];
        e->tag.pos[1] = gi->world[13];
        e->tag.pos[2] = gi->world[14];
    }
    if (cur.kind == PK_FADE)
        return 0;                               /* a full-screen fade would hide everything: not drawn in the pass */
    return (uint32_t)nent;
}

void pick_pass_begin(void)
{
    nent = 0;
    memset(&cur, 0, sizeof cur);
    cur.clay_handle = -1;
    gfx_pick_cb = reg;
}

int pick_count(void) { return nent; }
const pick_entry_t *pick_entry(uint32_t id) { return id >= 1 && id <= (uint32_t)nent ? &ent[id - 1] : NULL; }

int pick_same(const pick_entry_t *x, const pick_entry_t *y)
{
    return x->tag.kind == y->tag.kind && x->tag.a == y->tag.a && x->tag.b == y->tag.b && x->tag.c == y->tag.c
        && x->tag.fn == y->tag.fn && x->tag.d == y->tag.d && x->tag.clay_handle == y->tag.clay_handle
        && x->gi.is2d == y->gi.is2d && (!x->gi.is2d || x->gi.tex == y->gi.tex);
}

/* ------------------------------------------------------------ text helpers */
static const char *sym(const void *fn, char *buf, size_t n)
{
    unsigned off = 0;
    const char *nm = fn ? rt_host_symname(fn, &off) : NULL;
    if (!nm) {
        if (fn)
            snprintf(buf, n, "%p", fn);
        else
            buf[0] = 0;
        return buf;
    }
    if (off)
        snprintf(buf, n, "%s+0x%x", nm, off);
    else
        snprintf(buf, n, "%s", nm);
    return buf;
}

static const char *const bf_name[] = { "ZERO", "ONE", "SRC_ALPHA", "INV_SRC_ALPHA", "DST_ALPHA", "INV_DST_ALPHA" };
static const char *const af_name[] = { "NEVER", "LESS", "EQUAL", "LEQUAL", "GREATER", "NOTEQUAL", "GEQUAL", "ALWAYS" };

void pick_describe(const pick_entry_t *e, char *out, int cap)
{
    const pick_tag_t *t = &e->tag;
    char fs[96];
    sym(t->fn, fs, sizeof fs);
    switch (t->kind) {
    case PK_STAGE: snprintf(out, (size_t)cap, "stage %d area part %d", t->a, t->b); break;
    case PK_SKY: snprintf(out, (size_t)cap, "stage %d sky part %d", t->a, t->b); break;
    case PK_SET: t->c == 1 ? snprintf(out, (size_t)cap, "stage %d set-model part %d", t->a, t->b)
                           : snprintf(out, (size_t)cap, "set object work %d type %d arg %d (%s)", t->a, t->b, t->c, fs); break;
    case PK_MONSTER: snprintf(out, (size_t)cap, "monster kind %d slot %d part %d", t->b, t->a, t->c); break;
    case PK_NPC: snprintf(out, (size_t)cap, "npc kind %d slot %d part %d", t->b, t->a, t->c); break;
    case PK_HUNTER: {
        static const char *const sl[6] = { "legs", "face", "hair", "body", "arms", "waist" };
        snprintf(out, (size_t)cap, "hunter %d %s model part %d", t->a, t->b >= 0 && t->b < 6 ? sl[t->b] : "?", t->c);
        break;
    }
    case PK_WEAPON: snprintf(out, (size_t)cap, "weapon of hunter %d", t->a); break;
    case PK_EFT: snprintf(out, (size_t)cap, "effect work %d type %d arg %d (%s)", t->a, t->b, t->c, fs); break;
    case PK_SHELL: snprintf(out, (size_t)cap, "shell work %d type %d arg %d (%s)", t->a, t->b, t->c, fs); break;
    case PK_PRIM: snprintf(out, (size_t)cap, "prim on ot%d (%s)", t->a, fs); break;
    case PK_HUD: snprintf(out, (size_t)cap, "HUD/2D element tex %u %dx%d at %.0f,%.0f %s", e->gi.tex, e->gi.tex_w, e->gi.tex_h,
                          e->gi.bbox2d[0], e->gi.bbox2d[1], fs); break;
    case PK_TEXT: snprintf(out, (size_t)cap, "text glyph code %d (palette %d) at %.0f,%.0f", t->b, t->c, e->gi.bbox2d[0], e->gi.bbox2d[1]); break;
    case PK_MOVIE: snprintf(out, (size_t)cap, "movie frame"); break;
    default: snprintf(out, (size_t)cap, "%s draw (clay handle %d, tag %d %d %d) %s", e->gi.is2d ? "2D" : "3D", t->clay_handle, t->a, t->b, t->c, fs); break;
    }
}

/* ------------------------------------------------------------ json */
typedef struct { char *p; int n, cap; } sb;
static void put(sb *s, const char *fmt, ...)
{
    va_list ap;
    int k;
    if (s->n >= s->cap - 1)
        return;
    va_start(ap, fmt);
    k = vsnprintf(s->p + s->n, (size_t)(s->cap - s->n), fmt, ap);
    va_end(ap);
    if (k > 0)
        s->n += k < s->cap - s->n ? k : s->cap - s->n - 1;
}

void pick_json_str(char *out, int cap, const char *s)      /* a JSON string literal (with quotes) */
{
    int n = 0;
    if (cap < 3) return;
    out[n++] = '"';
    for (; *s && n < cap - 8; s++) {
        unsigned char c = (unsigned char)*s;
        if (c == '"' || c == '\\') { out[n++] = '\\'; out[n++] = (char)c; }
        else if (c == '\n') { out[n++] = '\\'; out[n++] = 'n'; }
        else if (c == '\r') { }
        else if (c == '\t') { out[n++] = '\\'; out[n++] = 't'; }
        else if (c < 32) { n += snprintf(out + n, 8, "\\u%04x", c); }
        else out[n++] = (char)c;
    }
    out[n++] = '"';
    out[n] = 0;
}

static void jstr(sb *s, const char *v)
{
    char b[1200];
    pick_json_str(b, sizeof b, v);
    put(s, "%s", b);
}

int pick_json(const pick_entry_t *e, char *out, int cap)
{
    const pick_tag_t *t = &e->tag;
    const gfx_pick_info *g = &e->gi;
    sb s = { out, 0, cap };
    char d[400], fs[96];
    int i, ident = 1;
    static const float id16[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
    pick_describe(e, d, sizeof d);
    sym(t->fn, fs, sizeof fs);
    put(&s, "{\"kind\":\"%s\",\"description\":", pick_kind_name(t->kind));
    jstr(&s, d);
    put(&s, ",\"ids\":{\"a\":%d,\"b\":%d,\"c\":%d,\"d\":%d,\"clay_handle\":%d}", t->a, t->b, t->c, t->d, t->clay_handle);
    if (fs[0]) {
        put(&s, ",\"draw_function\":");
        jstr(&s, fs);
    }
    switch (t->kind) {
    case PK_STAGE: case PK_SKY: put(&s, ",\"stage\":%d,\"part\":%d", t->a, t->b); break;
    case PK_SET: if (t->c == 1) put(&s, ",\"stage\":%d,\"set_model_part\":%d", t->a, t->b);
                 else put(&s, ",\"work\":%d,\"type\":%d,\"arg\":%d", t->a, t->b, t->c); break;
    case PK_MONSTER: case PK_NPC: {
        const u8 *em = em_work + 0xA10 * (t->a & 31);
        put(&s, ",\"slot\":%d,\"model_kind\":%d,\"model_part\":%d,\"motion_id\":%u,\"motion_frame\":%u,\"mode\":%u,\"step\":%u,\"hp\":%d",
            t->a, t->b, t->c, (unsigned)*(const u16 *)(em + 0x2DC), (unsigned)*(const u16 *)(em + 0x2E4), em[0x14], em[0x05], (int)*(const s16 *)(em + 0x302));
        break;
    }
    case PK_HUNTER: case PK_WEAPON: {
        const u8 *pl = player_work + 0xA00 * (t->a & 7);
        put(&s, ",\"player\":%d,\"hunter_slot\":%d,\"model_part\":%d,\"motion_id\":%u,\"motion_frame\":%u,\"mode\":%u,\"step\":%u,\"hp\":%d",
            t->a, t->b, t->c, (unsigned)*(const u16 *)(pl + 0x2DC), (unsigned)*(const u16 *)(pl + 0x2E4), pl[0x14], pl[0x05], (int)*(const s16 *)(pl + 0x302));
        break;
    }
    case PK_EFT: case PK_SHELL: put(&s, ",\"work\":%d,\"type\":%d,\"arg\":%d", t->a, t->b, t->c); break;
    default: break;
    }
    if (t->obj) {
        const fl_skel *sk = (const fl_skel *)t->obj;
        put(&s, ",\"skeleton\":{\"bones\":%d,\"motion_frame\":%.1f,\"motion_end\":%.1f}", sk->skel.nbone, sk->frame, sk->end);
    }
    put(&s, ",\"world_position\":[%.1f,%.1f,%.1f]", t->pos[0], t->pos[1], t->pos[2]);
    put(&s, ",\"render\":{\"is_2d\":%s,\"vertices\":%d", g->is2d ? "true" : "false", g->nvert);
    if (g->tex)
        put(&s, ",\"texture\":{\"gl_name\":%u,\"width\":%d,\"height\":%d}", g->tex, g->tex_w, g->tex_h);
    else
        put(&s, ",\"texture\":null");
    put(&s, ",\"blend\":{\"on\":%s,\"src\":\"%s\",\"dst\":\"%s\",\"operation\":%d}", g->blend_on ? "true" : "false",
        g->bsrc >= 0 && g->bsrc < 6 ? bf_name[g->bsrc] : "?", g->bdst >= 0 && g->bdst < 6 ? bf_name[g->bdst] : "?", g->bop);
    put(&s, ",\"z_test\":%s,\"z_write\":%s,\"z_func\":%d,\"alpha_func\":\"%s\",\"alpha_ref\":%.3f", g->ztest ? "true" : "false",
        g->zwrite ? "true" : "false", g->zfunc, g->afunc >= 0 && g->afunc < 8 ? af_name[g->afunc] : "?", g->aref);
    put(&s, ",\"filter\":\"%s\",\"clamp\":%s,\"fog\":%s,\"fade_color\":\"0x%08X\"", g->nearest ? "point" : "linear",
        g->clamp ? "true" : "false", g->fog ? "true" : "false", (unsigned)g->fade);
    for (i = 0; i < 16; i++)
        if (g->texmat[i] != id16[i])
            ident = 0;
    if (g->noscroll)
        put(&s, ",\"scroll_matrix\":\"none (part has no UV scroll)\"");
    else if (ident)
        put(&s, ",\"scroll_matrix\":\"identity\"");
    else {
        put(&s, ",\"scroll_matrix\":[");
        for (i = 0; i < 16; i++)
            put(&s, "%s%.5g", i ? "," : "", g->texmat[i]);
        put(&s, "]");
    }
    if (g->is2d)
        put(&s, ",\"screen_box\":[%.0f,%.0f,%.0f,%.0f],\"screen\":[%d,%d]", g->bbox2d[0], g->bbox2d[1], g->bbox2d[2], g->bbox2d[3], g->sw, g->sh);
    put(&s, "}}");
    return s.n;
}

/* ------------------------------------------------------------ game state */
void rt_player_get(int no, float pos[3], int *ang_y);
void rt_monster_get(int no, float pos[3], int *ang_y);
void rt_cam_view(float eye[3], float tar[3], float *roll, float *fov);

char *pick_game_json(void)
{
    int cap = 24000, i;
    char *o = (char *)malloc((size_t)cap);
    sb s = { o, 0, cap };
    float p[3] = { 0, 0, 0 }, eye[3] = { 0, 0, 0 }, tar[3] = { 0, 0, 0 }, roll = 0, fov = 0;
    int ang = 0, any = 0;
    if (!o) return NULL;
    put(&s, "{\"tick\":%d,\"mode\":%u,\"step\":%u,\"stage\":%u,\"quest\":%u,\"map_areas\":[%u,%u,%u,%u],\"random_seed\":%d",
        rt_log_tick(), game_w[0], game_w[1], game_w[0x14], select_w[0xAC], game_w[0x28], game_w[0x29], game_w[0x2A], game_w[0x2B], rt_seed_get());
    rt_cam_view(eye, tar, &roll, &fov);
    put(&s, ",\"camera\":{\"eye\":[%.1f,%.1f,%.1f],\"target\":[%.1f,%.1f,%.1f],\"roll\":%.3f,\"fov\":%.3f}", eye[0], eye[1], eye[2], tar[0], tar[1], tar[2], roll, fov);
    {
        int m = 0;                                  /* game_w.master: the player slot of this machine */
        const u8 *pl = player_work + 0xA00 * m;
        rt_player_get(m, p, &ang);
        put(&s, ",\"hunter\":{\"position\":[%.1f,%.1f,%.1f],\"angle\":%d,\"motion_id\":%u,\"motion_frame\":%u,\"mode\":%u,\"step\":%u,\"hp\":%d}",
            p[0], p[1], p[2], ang & 0xFFFF, (unsigned)*(const u16 *)(pl + 0x2DC), (unsigned)*(const u16 *)(pl + 0x2E4), pl[0x14], pl[0x05],
            (int)*(const s16 *)(pl + 0x302));
    }
    put(&s, ",\"monsters_in_area\":[");
    for (i = 0; i < 20; i++) {
        const u8 *em = em_work + 0xA10 * i;
        float q[3];
        if (!em[0] || em[0x736] != game_w[0x14])
            continue;
        memcpy(q, em + 0xAC, sizeof q);
        put(&s, "%s{\"slot\":%d,\"kind\":%u,\"npc\":%s,\"position\":[%.1f,%.1f,%.1f],\"motion_id\":%u,\"motion_frame\":%u,\"mode\":%u,\"step\":%u,\"hp\":%d}",
            any++ ? "," : "", i, em[2], em[0x1E] ? "true" : "false", q[0], q[1], q[2], (unsigned)*(const u16 *)(em + 0x2DC),
            (unsigned)*(const u16 *)(em + 0x2E4), em[0x14], em[0x05], (int)*(const s16 *)(em + 0x302));
    }
    put(&s, "]}");
    return o;
}

/* ------------------------------------------------------------ session recording */
typedef struct { unsigned short bits; signed char lx, ly, rx, ry; unsigned n; } run_t;
static run_t *runs;
static int nruns, caprun;
static unsigned total_ticks;
#define MAX_RUNS 400000

void rt_pick_record_pad(unsigned bits, int lx, int ly, int rx, int ry)
{
    run_t *r = nruns ? &runs[nruns - 1] : NULL;
    total_ticks++;
    if (r && r->bits == (unsigned short)bits && r->lx == lx && r->ly == ly && r->rx == rx && r->ry == ry) {
        r->n++;
        return;
    }
    if (nruns >= MAX_RUNS)
        return;
    if (nruns == caprun) {
        caprun = caprun ? caprun * 2 : 1024;
        runs = (run_t *)realloc(runs, (size_t)caprun * sizeof *runs);
        if (!runs) { nruns = caprun = 0; return; }
    }
    r = &runs[nruns++];
    r->bits = (unsigned short)bits;
    r->lx = (signed char)lx; r->ly = (signed char)ly; r->rx = (signed char)rx; r->ry = (signed char)ry;
    r->n = 1;
}

char *rt_pick_input_script(int *nticks)
{
    size_t cap = (size_t)nruns * 28 + 16, len = 0;
    char *o = (char *)malloc(cap);
    int i;
    if (!o) return NULL;
    o[0] = 0;
    for (i = 0; i < nruns; i++)
        len += (size_t)snprintf(o + len, cap - len, "%sx%04X:%d:%d:%d:%d*%u", i ? "," : "", runs[i].bits, runs[i].lx, runs[i].ly,
                                runs[i].rx, runs[i].ry, runs[i].n);
    if (nticks) *nticks = (int)total_ticks;
    return o;
}

static char args_s[1200];
void rt_pick_set_args(int argc, char **argv)
{
    int i;
    args_s[0] = 0;
    for (i = 1; i < argc && strlen(args_s) < sizeof args_s - 200; i++)
        snprintf(args_s + strlen(args_s), sizeof args_s - strlen(args_s), "%s%s", i > 1 ? " " : "", argv[i]);
}
const char *rt_pick_args(void) { return args_s; }
