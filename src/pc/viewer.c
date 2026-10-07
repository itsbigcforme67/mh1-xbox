/*
 * viewer.c - first piece of the PC port: a real-time viewer that loads
 * MH1 data straight from the user's disc files and shows stage 4 with the
 * Rathian (em01) and a hunter, both animating, under a free-fly camera.
 *
 *   build/pc/mhview DISC_DIR [--shot FILE.png] [--frames N] [--size WxH]
 *                            [--cam x,y,z,yaw,pitch] [--time SECONDS]
 *
 * DISC_DIR holds AFS_DATA.AFS and SLPM_654.95 (e.g. disc/mh1). Nothing is
 * extracted to disk. Controls: WASD move, mouse look, Space/C up/down,
 * Shift fast, Esc quit. Docs: docs/pc.md.
 */
#include "fl/fl.h"
#include "rt/rt.h"
#include "pad/pad.h"
#include "audio/audio.h"

#include <SDL.h>
#include <stdio.h>
#include "rt/rt_memstat.h"
#include <stdlib.h>
#include <string.h>
#ifdef XBOX
#include <nxdk/mount.h>
#endif

/* ------------------------------------------------------------ data */
static fmt_afs afs;

static fmt_blob load(const char *name, uint8_t **keep)
{
    fmt_blob b = { NULL, 0 };
    size_t n;
    const char *o = rt_ms_push("files kept for host models/stage (viewer)");
    *keep = fmt_afs_load(&afs, name, &n);
    rt_ms_pop(o);
    if (!*keep) {
        fprintf(stderr, "missing or bad AFS entry %s\n", name);
        return b;
    }
    b.p = *keep;
    b.n = n;
    if (getenv("RT_MEM_FILES"))
        fprintf(stderr, "memstat: file %s %zu\n", name, n);
    return b;
}

/* A file whose contents fl_model_create has copied (geometry, skeleton,
 * textures): not needed any more (memory: docs/xbox.md). Motion tables
 * (*_tbl.bin) stay: the motion players read their keys in place. */
static void drop(uint8_t **keep)
{
    free(*keep);
    *keep = NULL;
}

/* File of a stage from one of the per-stage AFS index tables in main
 * (stage.md 1: 88 x s32, -1 = none), Meltw-decompressed. */
static fmt_blob load_stage_file(uint32_t table, int stage, uint8_t **keep)
{
    fmt_blob none = { NULL, 0 };
    const uint8_t *p = rt_addr(table + 4 * (uint32_t)stage, 4);
    int32_t idx;
    *keep = NULL;
    if (!p || stage < 0 || stage >= 88)
        return none;
    memcpy(&idx, p, 4);
    if (idx < 0 || (uint32_t)idx >= afs.count)
        return none;
    return load(afs.name[idx], keep);
}

/* PS2 addresses (ELF / overlay) are looked up through the runtime. */
static const uint8_t *elf_addr(uint32_t va)
{
    return rt_addr(va, 4);
}

/* CLAY+0x88 word of part k (Attribute_from_amo), 0 if it has no 0xF0000 chunk */
static uint32_t part_attr(const fl_model *m, int k)
{
    const amo_part *p = &m->amo.part[k];
    return p->has_attr ? rt_clay_attr_word(p->attr) : 0;
}

/* fl_model_draw with each part's own blend/filter/clamp (clay_attr_set) */
static void draw_model_attr(fl_model *m, int sky)
{
    int i;
    for (i = 0; i < m->npart; i++)
        if (sky < 0 || sky == m->part[i].is_sky) {
            rt_clay_attr_set(part_attr(m, i));
            gfx_execute_clay(m->part[i].clay);
            rt_clay_attr_reset();
        }
}

/* The effect models (eft_mdlw, load_eft / load_shadow at 0x111110): AFS
 * entries from main's effect_model_data / EFT_TEX tables (5 x s32 each:
 * ef_00, kage04-06, ef_01), handed to the game C. */
static fl_model eft_models[5];
static uint8_t *eft_keep[10];
static void eft_skin(int k, const float *mats, int n)
{
    static fl_light none;
    static flmat id;
    (void)n;
    fl_model_pose(&eft_models[k], (const flmat *)mats, &none);
    /* the node matrices are in world space (the PS2 skin program uses
     * them directly): draw the skinned clay with an identity world */
    flmat_identity(id);
    gfx_set_render_state(GFX_RS_WORLD, (uintptr_t)id);
}
static void load_eft_models(void)
{
    int k;
    for (k = 0; k < 5; k++) {
        fmt_blob link = load_stage_file(0x2ECEE0, k, &eft_keep[2 * k]);    /* effect_model_data */
        fmt_blob tex = load_stage_file(0x2EF2A0, k, &eft_keep[2 * k + 1]); /* EFT_TEX */
        gfx_clay **c;
        uint32_t *at;
        int i;
        if (!link.p || fl_model_create(&eft_models[k], fmt_link_entry(link, 0, FMT_LE),
                                       fmt_link_entry(link, 1, FMT_LE), tex, 0, FMT_LE) != 0) {
            fprintf(stderr, "effect model %d: load failed\n", k);
            continue;
        }
        c = calloc((size_t)eft_models[k].npart + 1, sizeof *c);
        at = calloc((size_t)eft_models[k].npart + 1, sizeof *at);
        for (i = 0; i < eft_models[k].npart; i++) {
            c[i] = eft_models[k].part[i].clay;
            at[i] = part_attr(&eft_models[k], i);
        }
        rt_bind_eft_model(k, c, at, eft_models[k].npart);
        drop(&eft_keep[2 * k]);
        drop(&eft_keep[2 * k + 1]);
        if (eft_models[k].skel.nbone > 0)
            rt_bind_eft_skin(k, eft_models[k].skel.nbone, eft_skin);
        free(c);
        free(at);
    }
}

/* load_file_mdl for the game C: AFS entry by index, Meltw-decompressed */
static uint8_t *afs_entry(int idx, size_t *n)
{
    if (idx < 0 || (uint32_t)idx >= afs.count)
        return NULL;
    {
        const char *o = rt_ms_push("files for the game's loaders (load_file_mdl, kept copies)");
        uint8_t *p = fmt_afs_load(&afs, afs.name[idx], n);
        rt_ms_pop(o);
        return p;
    }
}

static uint32_t crc_table[256];

static uint32_t crc32_update(uint32_t c, const uint8_t *p, size_t n)
{
    size_t i;
    if (!crc_table[1]) {
        uint32_t k, j;
        for (k = 0; k < 256; k++) {
            uint32_t v = k;
            for (j = 0; j < 8; j++)
                v = v & 1 ? 0xEDB88320u ^ (v >> 1) : v >> 1;
            crc_table[k] = v;
        }
    }
    c = ~c;
    for (i = 0; i < n; i++)
        c = crc_table[(c ^ p[i]) & 255] ^ (c >> 8);
    return ~c;
}

static void put32(FILE *f, uint32_t v)
{
    uint8_t b[4] = { (uint8_t)(v >> 24), (uint8_t)(v >> 16), (uint8_t)(v >> 8), (uint8_t)v };
    fwrite(b, 1, 4, f);
}

static void chunk(FILE *f, const char *type, const uint8_t *d, size_t n)
{
    uint32_t c = crc32_update(0, (const uint8_t *)type, 4);
    c = crc32_update(c, d, n);
    put32(f, (uint32_t)n);
    fwrite(type, 1, 4, f);
    fwrite(d, 1, n, f);
    put32(f, c);
}

/* PNG with stored (uncompressed) deflate blocks: no zlib needed. */
static int write_png(const char *path, int w, int h, const uint8_t *rgb)
{
    FILE *f = fopen(path, "wb");
    size_t raw_n = (size_t)h * (w * 3 + 1), i, nblk, pos = 0, o = 0;
    uint8_t *raw, *z, ihdr[13];
    uint32_t a = 1, b = 0;
    int y;
    if (!f)
        return -1;
    raw = malloc(raw_n);
    for (y = 0; y < h; y++) {
        raw[y * (w * 3 + 1)] = 0;
        memcpy(raw + y * (w * 3 + 1) + 1, rgb + (size_t)y * w * 3, (size_t)w * 3);
    }
    nblk = (raw_n + 65534) / 65535;
    z = malloc(2 + raw_n + nblk * 5 + 4);
    z[o++] = 0x78;
    z[o++] = 0x01;
    for (i = 0; i < nblk; i++) {
        size_t len = raw_n - pos > 65535 ? 65535 : raw_n - pos;
        z[o++] = (uint8_t)(i == nblk - 1);
        z[o++] = (uint8_t)len;
        z[o++] = (uint8_t)(len >> 8);
        z[o++] = (uint8_t)~len;
        z[o++] = (uint8_t)(~len >> 8);
        memcpy(z + o, raw + pos, len);
        o += len;
        pos += len;
    }
    for (i = 0; i < raw_n; i++) {
        a = (a + raw[i]) % 65521;
        b = (b + a) % 65521;
    }
    z[o++] = (uint8_t)(b >> 8); z[o++] = (uint8_t)b;
    z[o++] = (uint8_t)(a >> 8); z[o++] = (uint8_t)a;
    fwrite("\x89PNG\r\n\x1a\n", 1, 8, f);
    ihdr[0] = (uint8_t)(w >> 24); ihdr[1] = (uint8_t)(w >> 16); ihdr[2] = (uint8_t)(w >> 8); ihdr[3] = (uint8_t)w;
    ihdr[4] = (uint8_t)(h >> 24); ihdr[5] = (uint8_t)(h >> 16); ihdr[6] = (uint8_t)(h >> 8); ihdr[7] = (uint8_t)h;
    ihdr[8] = 8; ihdr[9] = 2; ihdr[10] = ihdr[11] = ihdr[12] = 0;
    chunk(f, "IHDR", ihdr, 13);
    chunk(f, "IDAT", z, o);
    chunk(f, "IEND", NULL, 0);
    fclose(f);
    free(raw);
    free(z);
    return 0;
}

/* ------------------------------------------------------------ actors */
typedef struct {
    fl_model model;
    fl_skel skel;            /* drives model (same AHI) */
    flmat world;
    fmt_blob tbl;            /* *_tbl.bin */
    int game;                /* 1: posed by the game's motion code (em_work[0]) */
    uint8_t *mem[3];
} monster;

#define HUNTER_PARTS 6
static const char *pl_slot[HUNTER_PARTS] = { "reg", "face", "hair", "body", "arm", "wst" };

typedef struct {
    fl_skel master;                  /* legs AHI: the animated skeleton */
    fl_model part[HUNTER_PARTS];
    const uint8_t *ptmat[HUNTER_PARTS];  /* s16 tables from ptmat_tbl (0x3018F0) */
    flmat *pw[HUNTER_PARTS];         /* per part bone world matrices */
    flmat world;
    fmt_blob tbl;                    /* plcom_tbl.bin */
    int game;                        /* 1: posed by the game's motion code (player_work[no]) */
    int no;                          /* player_work index */
    int look[HUNTER_PARTS], sex, look_gen;  /* the parts loaded (hunter_relook) */
    uint8_t *mem[HUNTER_PARTS * 2 + 1];
} hunter;

/* camera world matrix looking from eye to target (up = +Y, roll ignored) */
static void lookat_world(flmat camw, const float *eye, const float *tar)
{
    float b[3], r[3], u[3], len;
    int k;
    for (k = 0; k < 3; k++)
        b[k] = eye[k] - tar[k];
    len = sqrtf(b[0] * b[0] + b[1] * b[1] + b[2] * b[2]);
    if (len < 1e-3f) { b[0] = 0; b[1] = 0; b[2] = 1; len = 1; }
    for (k = 0; k < 3; k++)
        b[k] /= len;
    r[0] = b[2]; r[1] = 0; r[2] = -b[0];          /* up (0,1,0) x back */
    len = sqrtf(r[0] * r[0] + r[2] * r[2]);
    if (len < 1e-3f) { r[0] = 1; r[2] = 0; len = 1; }
    r[0] /= len; r[2] /= len;
    u[0] = b[1] * r[2] - b[2] * r[1];               /* back x right */
    u[1] = b[2] * r[0] - b[0] * r[2];
    u[2] = b[0] * r[1] - b[1] * r[0];
    memset(camw, 0, sizeof(flmat));
    for (k = 0; k < 3; k++) {
        camw[k] = r[k];
        camw[4 + k] = u[k];
        camw[8 + k] = b[k];
        camw[12 + k] = eye[k];
    }
    camw[15] = 1;
}

static monster weapon;              /* the hunter's weapon (--play with the game's player code) */

/* weapon bones: hierarchy roots from rt_player_weapon (weapon_trans's
 * placement), the rest from their bind pose under the parent */
static void weapon_pose(const fl_light *L)
{
    float r0[16], r1[16];
    int i, roots = 0;
    ahi_skel *k = &weapon.skel.skel;
    if (rt_player_weapon(0, r0, r1) < 0)
        return;
    for (i = 0; i < k->nbone; i++) {
        const ahi_bone *b = &k->bone[i];
        if (b->parent < 0 || b->parent >= i) {
            memcpy(weapon.skel.world[i], roots++ == 0 ? r0 : r1, sizeof(flmat));
        } else {
            flmat loc;
            flmat_srt(loc, b->s, b->r, b->t);
            flmat_mul(weapon.skel.world[i], loc, weapon.skel.world[b->parent]);
        }
    }
    fl_model_pose(&weapon.model, (const flmat *)weapon.skel.world, L);
}

static float min_y_of(const fl_model *m)
{
    float y = 1e30f;
    int i, v;
    for (i = 0; i < m->npart; i++) {
        const float *p = m->part[i].skinpos ? m->part[i].skinpos : m->amo.part[i].pos;
        for (v = 0; v < m->amo.part[i].nvert; v++)
            if (p[3 * v + 1] < y)
                y = p[3 * v + 1];
    }
    return y;
}

static int monster_load(monster *e, const char *amh, const char *tex, const char *tbl, int slot)
{
    fmt_blob link = load(amh, &e->mem[0]), tx = load(tex, &e->mem[1]), tb = load(tbl, &e->mem[2]);
    fmt_blob amo, ahi;
    int g;
    if (!link.p)
        return -1;
    e->tbl = tb;
    e->game = 0;
    amo = fmt_link_entry(link, 0, FMT_LE);
    ahi = fmt_link_entry(link, 1, FMT_LE);
    if (fl_model_create(&e->model, amo, ahi, tx, 1, FMT_LE) != 0 || fl_skel_create(&e->skel, ahi, FMT_LE) != 0)
        return -1;
    drop(&e->mem[0]);
    drop(&e->mem[1]);
    /* em tables: one bank per group, bank 2g (motion.md 3) */
    for (g = 0; g < 3; g++)
        if (tb.p)
            fl_skel_set_motion(&e->skel, g, tb, 200 * g + slot, FMT_LE);
    flmat_identity(e->world);
    return 0;
}

static void hunter_pose(hunter *h, float frame, const fl_light *L)
{
    int s, i;
    if (h->game)
        rt_player_pose(h->no, &h->master);   /* frame_move's motion player */
    else
        fl_skel_update(&h->master, frame);
    for (s = 0; s < HUNTER_PARTS; s++) {
        fl_model *m = &h->part[s];
        int nb = m->skel.nbone;
        char *ok = calloc(nb + 1, 1);
        int done = 0, guard = 0;
        if (s == 0) {
            fl_model_pose(m, (const flmat *)h->master.world, L);
            free(ok);
            continue;
        }
        /* SetPartsTrans (0x163E40): part bone -> master bone via ptmat_tbl */
        while (done < nb && guard++ < nb + 2) {
            for (i = 0; i < nb; i++) {
                int v = h->ptmat[s] ? fmt_s16(h->ptmat[s] + 2 * i, FMT_LE) : -1;
                int par = m->skel.bone[i].parent;
                if (ok[i])
                    continue;
                if (v > 0 && v < 64 && v < h->master.skel.nbone) {
                    memcpy(h->pw[s][i], h->master.world[v], sizeof(flmat));
                } else {
                    flmat loc;
                    const ahi_bone *b = &m->skel.bone[i];
                    if (par >= 0 && par < nb && !ok[par])
                        continue;
                    flmat_srt(loc, b->s, b->r, b->t);
                    if (par >= 0 && par < nb)
                        flmat_mul(h->pw[s][i], loc, h->pw[s][par]);
                    else
                        memcpy(h->pw[s][i], loc, sizeof(flmat));
                }
                ok[i] = 1;
                done++;
            }
        }
        free(ok);
        if (s == 2 && h->game) {        /* player_trans: hair colour (PLW+0x5FC) on the head part's first material */
            unsigned c = rt_player_hair_col(h->no);
            m->has_tint = c != 0;
            m->tint[0] = ((c >> 16) & 0xFF) / 255.0f;
            m->tint[1] = ((c >> 8) & 0xFF) / 255.0f;
            m->tint[2] = (c & 0xFF) / 255.0f;
        }
        fl_model_pose(m, (const flmat *)h->pw[s], L);
    }
}

static int hunter_load(hunter *h, const int *num, int legs_id, int upper_id)
{
    int s, k = 0;
    fmt_blob tb;
    const uint8_t *tbl = elf_addr(0x3018F0);     /* ptmat_tbl: 6 pointers */
    for (s = 0; s < HUNTER_PARTS; s++) {
        char name[64], tname[64];
        fmt_blob link, tex, ahi;
        snprintf(name, sizeof name, "m_%s%03d_amh.bin", pl_slot[s], num[s]);
        snprintf(tname, sizeof tname, "m_%s%03d.apx", pl_slot[s], num[s]);
        link = load(name, &h->mem[k++]);
        tex = load(tname, &h->mem[k++]);
        if (!link.p)
            return -1;
        ahi = fmt_link_entry(link, 1, FMT_LE);
        if (fl_model_create(&h->part[s], fmt_link_entry(link, 0, FMT_LE), ahi, tex, 1, FMT_LE) != 0)
            return -1;
        if (s == 0 && fl_skel_create(&h->master, ahi, FMT_LE) != 0)
            return -1;
        drop(&h->mem[k - 2]);
        drop(&h->mem[k - 1]);
        h->pw[s] = calloc(h->part[s].skel.nbone + 1, sizeof(flmat));
        h->ptmat[s] = tbl ? rt_ptr_at(0x3018F0 + 4 * (uint32_t)s) : NULL;   /* relocated by rt_import_data */
    }
    if (!tbl)
        fprintf(stderr, "warning: no SLPM_654.95, armour parts will not follow the skeleton\n");
    tb = load("plcom_tbl.bin", &h->mem[k++]);
    h->tbl = tb;
    h->game = 0;
    if (tb.p) {
        fl_skel_set_motion(&h->master, 0, tb, legs_id, FMT_LE);   /* char0: legs */
        fl_skel_set_motion(&h->master, 1, tb, upper_id, FMT_LE);  /* char1: upper body */
    }
    flmat_identity(h->world);
    return 0;
}

/* The hunter the game asked for (armor_create_model, rt_pl.c: sex and the
 * six part numbers from the save's character and armour): reload the
 * parts that differ. A part file that cannot be loaded keeps the old one. */
static void hunter_relook(hunter *h, int sex, const int *num)
{
    int s;
    for (s = 0; s < HUNTER_PARTS; s++) {
        char name[64], tname[64];
        uint8_t *m0, *m1;
        fmt_blob link, tex, ahi;
        fl_model nm;
        if (num[s] == h->look[s] && sex == h->sex)
            continue;
        snprintf(name, sizeof name, "%c_%s%03d_amh.bin", sex ? 'f' : 'm', pl_slot[s], num[s]);
        snprintf(tname, sizeof tname, "%c_%s%03d.apx", sex ? 'f' : 'm', pl_slot[s], num[s]);
        link = load(name, &m0);
        tex = load(tname, &m1);
        if (!link.p) {
            free(m1);
            continue;
        }
        ahi = fmt_link_entry(link, 1, FMT_LE);
        if (fl_model_create(&nm, fmt_link_entry(link, 0, FMT_LE), ahi, tex, 1, FMT_LE) != 0) {
            free(m0);
            free(m1);
            continue;
        }
        if (s == 0) {           /* the legs carry the master skeleton the motions pose */
            fl_skel ns;
            if (fl_skel_create(&ns, ahi, FMT_LE) != 0) {
                fl_model_release(&nm);
                free(m0);
                free(m1);
                continue;
            }
            ns.root_lock = h->master.root_lock;
            fl_skel_release(&h->master);
            h->master = ns;
        }
        fl_model_release(&h->part[s]);
        free(h->mem[2 * s]);
        free(h->mem[2 * s + 1]);
        h->mem[2 * s] = NULL;      /* copied by fl_model_create */
        h->mem[2 * s + 1] = NULL;
        free(m0);
        free(m1);
        h->part[s] = nm;
        free(h->pw[s]);
        h->pw[s] = calloc(h->part[s].skel.nbone + 1, sizeof(flmat));
    }
    for (s = 0; s < HUNTER_PARTS; s++)
        h->look[s] = num[s];
    h->sex = sex;
}

static void place(flmat w, float x, float y, float z, float yaw)
{
    float s[3] = { 1, 1, 1 }, r[3] = { 0, 0, 0 }, t[3];
    r[1] = yaw;
    t[0] = x; t[1] = y; t[2] = z;
    flmat_srt(w, s, r, t);
}

/* Joint world matrices of the hunter (player_work[0]) and the Rathian
 * (em_work[0]) for the game C: parts, get_joint_pos, hit_data_expand.
 * On the PS2 they come from the draw (trans) that runs between move()
 * and hit_check(), so this runs once per game tick, before hit_check. */
/* em_work[0] is the host's Rathian object (em01 model) only while it is a
 * Rathian (kind 1; 0 in free hunts); any other kind in slot 0 (Kut-Ku,
 * Rathalos, ...) is posed, drawn and given joints by monsters_sync */
static int slot0_rathian(void)
{
    extern uint8_t em_work[];
    return em_work[2] <= 1;
}
static void sync_joints(hunter *h, float hyoff, monster *e, float eyoff)
{
    static flmat jw[128], ew[128];
    float p[3];
    int a, nb, j;
    if (h->game) {
        rt_player_pose(0, &h->master);
        rt_player_get(0, p, &a);
        place(h->world, p[0], p[1] + hyoff, p[2], (float)(a & 0xFFFF) * (6.2831853f / 65536.0f));
        nb = h->master.skel.nbone < 128 ? h->master.skel.nbone : 128;
        for (j = 0; j < nb; j++)
            flmat_mul(jw[j], h->master.world[j], h->world);
        rt_player_parts(0, &jw[0][0], nb);
    }
    if (e->game && e->skel.root_lock && slot0_rathian()) {
        rt_monster_get(0, p, &a);
        place(e->world, p[0], p[1] + eyoff, p[2], (float)(a & 0xFFFF) * (6.2831853f / 65536.0f));
        rt_monster_pose(0, &e->skel);
        nb = e->skel.skel.nbone < 128 ? e->skel.skel.nbone : 128;
        for (j = 0; j < nb; j++)
            flmat_mul(ew[j], e->skel.world[j], e->world);
        rt_monster_joints(0, &ew[0][0], nb);
    }
}


/* ------------------------------------------------------------ main */
/* 48 kHz stereo s16 wav (--audio-dump) */
static void write_wav(const char *path, const int16_t *pcm, size_t frames)
{
    FILE *f = fopen(path, "wb");
    uint32_t data = (uint32_t)(frames * 4), v;
    uint16_t h;
    if (!f)
        return;
    fwrite("RIFF", 1, 4, f); v = 36 + data; fwrite(&v, 4, 1, f);
    fwrite("WAVEfmt ", 1, 8, f); v = 16; fwrite(&v, 4, 1, f);
    h = 1; fwrite(&h, 2, 1, f); h = 2; fwrite(&h, 2, 1, f);
    v = 48000; fwrite(&v, 4, 1, f); v = 48000 * 4; fwrite(&v, 4, 1, f);
    h = 4; fwrite(&h, 2, 1, f); h = 16; fwrite(&h, 2, 1, f);
    fwrite("data", 1, 4, f); fwrite(&data, 4, 1, f);
    fwrite(pcm, 4, frames, f);
    fclose(f);
}

/* main()'s state (file scope so the game tick can run as game_core) */
static const char *disc = NULL, *shot = NULL;
static int frames = 1, W = 1280, H = 720, i, running = 1, frame_no = 0;
static float cam[5] = { 11900, 700, 8900, 0.75f, -0.2f };   /* x y z yaw pitch */
static float fixed_time = -1;
static const char *shot_list;     /* RT_SHOTS */
static int shot_next;
static int tick_trace;
static char path[1024];
static size_t n;
static fmt_blob stage_link, stage_tex, set_link, set_tex;
static uint8_t *keep[8];
static fl_model stage, set;
static monster rathian;
static hunter pl;
static fl_light light;
extern unsigned char light_work[];

/* The game's lights for the host's CPU lighting (docs/pc.md "Lighting"). light_work set 1 (light_work + 0x140: the
 * hunter / monster / NPC set that Pl_light_set hands to flSetRenderState(0x5A..0x5C)) holds three light blocks of
 * 0x68 bytes from +0x158: +0x04 colour rgb, +0x24 the ambient part of that light (the PS2 shader adds each light's
 * ambient row), +0x34 direction the light travels (the shader negates it). light_init fills them from
 * pl_light_tbl[stage], light_change_normal re-reads the stage rows, flash_move (thunder) blends the colours.
 * Returns 0 when light_work is still empty (no stage lights yet): the caller keeps its default. */
static int rt_light_from_game(fl_light *L)
{
    int i, k, any = 0;
    float amb[3] = { 0, 0, 0 };
    for (i = 0; i < 3; i++) {
        const unsigned char *b = light_work + 0x158 + 0x68 * i;
        float d[3], len;
        memcpy(d, b + 0x34, sizeof d);
        len = sqrtf(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
        for (k = 0; k < 3; k++) {
            float c, a;
            memcpy(&c, b + 4 + 4 * k, 4);
            memcpy(&a, b + 0x24 + 4 * k, 4);
            L->dir[i][k] = len > 1e-6f ? d[k] / len : 0.0f;
            L->col[i][k] = len > 1e-6f ? c : 0.0f;      /* an unused light has no direction row */
            amb[k] += a;
            any |= c != 0.0f || a != 0.0f;
        }
    }
    if (!any)
        return 0;
    for (k = 0; k < 3; k++)
        L->ambient[k] = amb[k];
    return 1;
}
static fl_light light_game;          /* the game's stage lights (light_work), else the fixed default above */
static const fl_light *light_cur(void)
{
    if (!getenv("RT_LIGHT_FIXED") && rt_light_from_game(&light_game)) {
        static int shown;
        if (getenv("RT_LIGHT_TRACE") && shown++ % 600 == 0) {
            int i;
            for (i = 0; i < 3; i++)
                fprintf(stderr, "light %d dir %.2f %.2f %.2f col %.2f %.2f %.2f\n", i, light_game.dir[i][0], light_game.dir[i][1],
                        light_game.dir[i][2], light_game.col[i][0], light_game.col[i][1], light_game.col[i][2]);
            fprintf(stderr, "light ambient %.2f %.2f %.2f\n", light_game.ambient[0], light_game.ambient[1], light_game.ambient[2]);
        }
        return &light_game;
    }
    return &light;
}
static const int parts[HUNTER_PARTS] = { 1, 0, 1, 1, 1, 1 };
static float hx = 10900, hz = 7700, rx = 10000, rz = 6700, gy;
static Uint32 t0;
static int set_h0 = -1, ticks = 0, stage_no = 4, cam_given = 0, stage_given = 0, quest_no = 0;
static float follow[3] = { 900.0f, 450.0f, -0.3f };
static float rathian_yoff = 0;
static int follow_given = 0, game_cam = 0, have_view = 0;
static const char *audio_dump = NULL;      /* --audio-dump out.wav: mix each game tick into a wav */
static int mute = 0, snd = -1;
static int16_t *dump_pcm = NULL;
static size_t dump_n = 0, dump_cap = 0;   /* game_cam: the game's CameraMove drives the view */
/* --audio-dump: 1/30 s of mixer output per game tick */
static void audio_dump_tick(void)
{
    if (!audio_dump)
        return;
    audio_set_driven(1);        /* the movie's clock follows what is mixed */
    if (dump_n + 1600 * 2 > dump_cap) {
        dump_cap = dump_cap ? dump_cap * 2 : 1 << 20;
        dump_pcm = realloc(dump_pcm, dump_cap * sizeof *dump_pcm);
    }
    audio_mix(dump_pcm + dump_n, 1600);
    dump_n += 1600 * 2;
}

static float gc_eye[3] = { 0 }, gc_tar[3] = { 0 }, gc_roll = 0, gc_fov = 1.0f;   /* --play camera: distance, height, pitch */
static int play = 0, sw_trace = 0;          /* --play: the pad drives the hunter */
static int boot = 0, booting = 0;           /* --boot: from power-on (rt_boot.c) */
static const char *script = NULL;
static float hunter_yoff = 0;

/* ------------------------------------------------------------ boot hunters
 * The character creation and continue screens draw their hunter from a
 * prim (trans_pl_sub -> player_trans) inside the task's trans(): rt_pl.c
 * calls rt_hunter_draw_hook(no) there, which records a host draw at that
 * point of the tick's gfx list (gfx_rec_call); each replay poses
 * player_work[no] with the game's motion and draws it with the game's view
 * (lpView, as in the village). One host hunter per player_work slot. */
static hunter ed_h[3];
static int ed_loaded[3];
static void ed_hunter_draw(void *arg)
{
    int no = (int)(intptr_t)arg, sx, ids[HUNTER_PARTS], g, s, a;
    hunter *h;
    float eye[3], tar[3], roll, fov, p[3];
    flmat camw, view, proj;
    if (no < 0 || no >= 3)
        return;
    h = &ed_h[no];
    if (!ed_loaded[no]) {
        static const int dflt[HUNTER_PARTS] = { 1, 0, 1, 1, 1, 1 };
        ed_loaded[no] = hunter_load(h, dflt, 1, 101) == 0 ? 1 : -1;
        memcpy(h->look, dflt, sizeof h->look);
        h->no = no;
        h->game = 1;
    }
    if (ed_loaded[no] < 0)
        return;
    g = rt_player_look(no, &sx, ids);      /* continue screen: armor_create_model */
    if (!g)
        g = rt_player_edit_look(no, &sx, ids);     /* character creation */
    if (g && g != h->look_gen) {
        h->look_gen = g;
        hunter_relook(h, sx, ids);
    }
    rt_player_get(no, p, &a);
    place(h->world, p[0], p[1], p[2], (float)(a & 0xFFFF) * (6.2831853f / 65536.0f));
    hunter_pose(h, 0, light_cur());
    rt_cam_view(eye, tar, &roll, &fov);
    lookat_world(camw, eye, tar);
    flmat_invert_affine(view, camw);
    flmat_perspective(proj, fov > 0.01f ? fov : 1.0f, (float)W / H, 10.0f, 80000.0f);
    gfx_set_render_state(GFX_RS_PROJECTION, (uintptr_t)proj);
    gfx_set_render_state(GFX_RS_VIEW, (uintptr_t)view);
    gfx_set_render_state(GFX_RS_ALPHA_REF, 0x40);
    gfx_set_render_state(GFX_RS_BLEND, 1);
    gfx_set_render_state(GFX_RS_ZWRITE, 1);
    gfx_set_render_state(GFX_RS_WORLD, (uintptr_t)h->world);
    for (s = 0; s < HUNTER_PARTS; s++)
        draw_model_attr(&h->part[s], -1);
}
static void ed_hunter_hook(int no)
{
    gfx_rec_call(ed_hunter_draw, (void *)(intptr_t)no);
}


/* ------------------------------------------------------------ stage files
 * The stage's area model + set model (stage.md 1) and collision, found
 * through main's per-stage tables (stage 4 = st04, st04_1, lg004). Used at
 * start-up and, as st_model_load, when the game changes stage (game2 steps
 * 2-6: area exits, the cart back to camp). */
static int load_stage_models(int st)
{
    fl_model old_stage = stage, old_set = set;
    int k, reload = stage.npart > 0;
    for (k = 0; k < 4; k++)
        if (reload) {
            free(keep[k]);
            keep[k] = NULL;
        }
    stage_link = load_stage_file(0x2EC950, st, &keep[0]);   /* stage_model_data */
    stage_tex = load_stage_file(0x2EDB40, st, &keep[1]);    /* STAGE_TEX */
    set_link = load_stage_file(0x2ECD70, st, &keep[2]);     /* set_model_data */
    set_tex = load_stage_file(0x2EF130, st, &keep[3]);      /* SET_TEX */
    /* collision: the game's load_stage_hit (wall + ground HITS files) */
    rt_set_file_loader(afs_entry);
    if (rt_load_stage_hit(st) != 0)
        fprintf(stderr, "stage %d: no ground collision\n", st);
    if (!stage_link.p || fl_model_create(&stage, fmt_link_entry(stage_link, 0, FMT_LE),
                                         fmt_link_entry(stage_link, 1, FMT_LE), stage_tex, 0, FMT_LE) != 0) {
        fprintf(stderr, "stage %d: load failed\n", st);
        return -1;
    }
    memset(&set, 0, sizeof set);
    if (set_link.p)
        fl_model_create(&set, fmt_link_entry(set_link, 0, FMT_LE), fmt_link_entry(set_link, 1, FMT_LE),
                        set_tex, 0, FMT_LE);
    for (k = 0; k < 4; k++)
        drop(&keep[k]);
    {                           /* the area model to the game C (stage_work.mdl) */
        gfx_clay *c[64];
        uint32_t at[64];
        int k, nc = stage.npart < 64 ? stage.npart : 64;
        for (k = 0; k < nc; k++) {
            c[k] = stage.part[k].clay;
            at[k] = part_attr(&stage, k);
        }
        rt_bind_stage_model(c, at, nc);
    }
    if (set.npart) {            /* hand the set model to the game C (set_mdlw) */
        gfx_clay *c[64];
        uint32_t at[64];
        int k, nc = set.npart < 64 ? set.npart : 64;
        for (k = 0; k < nc; k++) {
            c[k] = set.part[k].clay;
            at[k] = part_attr(&set, k);
        }
        set_h0 = rt_bind_set_model(c, at, nc);
    }
    {                           /* the stage's light rows into light_work (init_light_work in the game's stage change) */
        extern unsigned char game_w[];
        extern void light_init(void);
        unsigned char old = game_w[0x14];
        game_w[0x14] = (unsigned char)st;
        light_init();
        game_w[0x14] = old;
    }
    if (reload) {
        fl_model_release(&old_stage);
        if (old_set.npart)
            fl_model_release(&old_set);
        stage_no = st;
        if (game_cam)
            rt_cam_init(st);            /* the stage's camera file */
        if (snd == 0) {
            static const int em_kinds[1] = { 1 };
            rt_snd_stage(st, em_kinds, 1);
        }
    }
    return 0;
}

/* ------------------------------------------------------------ one game tick
 * What game_core does on the PS2 (swset, move, trans, hit_check), done by
 * the host pieces in the PS2 order. With --quest it runs inside the game's
 * own mode loop (game2 -> game_core, src/main/game/f_game.c; rt_flow.c). */
static void monsters_sync(int draw, const fl_light *L);
static void sim_tick(void)
{
    rt_game_move();
    if (pl.game && play && ticks >= 2) {
        pad_state ps;
        if (script)
            pad_script_next(&ps);
        else
            pad_read(&ps, 1);
        rt_pad_set(ps.bits, ps.lx, ps.ly, ps.rx, ps.ry);
        rt_player_tick(0);
        if (game_cam) {
            flmat cw;
            rt_cam_tick();      /* CameraMove (src/main/cam) */
            /* rview_mat follows the game camera every tick (sound
             * distances, billboards), also when several ticks run
             * in one drawn frame */
            rt_cam_view(gc_eye, gc_tar, &gc_roll, &gc_fov);
            lookat_world(cw, gc_eye, gc_tar);
            rt_set_camera(cw);
        }
        /* right stick turns the follow camera */
        cam[3] -= ps.rx * (0.04f / 127.0f);
        if (sw_trace) {
            int now, ang, pw;
            float p[3];
            int a;
            rt_player_sw(0, &now, &ang, &pw);
            rt_player_get(0, p, &a);
            printf("tick %d: sw %04X stick ang %04X pow %d -> pos %.0f %.0f %.0f ang %04X\n",
                   ticks, now, ang, pw, p[0], p[1], p[2], a & 0xFFFF);
        }
    } else if (pl.game) {
        rt_player_motion_tick(0);
    }
    if (pl.game && play && ticks < 2 && game_cam) {
        /* the camera also runs on the first ticks: a quest's event demo
         * (game2 -> EvDemoMove) can request its demo camera on tick 0 and
         * ends at once when CameraMove has not taken the request */
        flmat cw;
        rt_cam_tick();
        rt_cam_view(gc_eye, gc_tar, &gc_roll, &gc_fov);
        lookat_world(cw, gc_eye, gc_tar);
        rt_set_camera(cw);
    }
    if (pl.game && play && ticks >= 2 && rt_player_uses_game()) {
        sync_joints(&pl, hunter_yoff, &rathian, rathian_yoff);
        monsters_sync(0, light_cur());
        rt_hit_check();         /* hit_check (src/main/hit/hit_nm.c), as game_core does after trans */
    }
    if (ticks >= 2 && !getenv("RT_EM_STANDIN")) {
        int i;
        for (i = 1; i < 20; i++)        /* move()'s monster loop: the others (em_work[0] below) */
            rt_monster_tick(i);
    }
    if (rathian.game && ticks >= 2) {
        if (getenv("RT_EM_STANDIN"))
            rt_monster_motion_tick(0);
        else
            rt_monster_tick(0);
        if (sw_trace && rathian.skel.root_lock) {
            float p[3];
            int a;
            rt_monster_get(0, p, &a);
            printf("tick %d: em0 pos %.0f %.0f %.0f ang %04X\n", ticks, p[0], p[1], p[2], a & 0xFFFF);
        }
    }
    if (quest_no && pl.game && play && ticks >= 2 && rt_player_uses_game()) {
        void stage_mv_ck(void);
        stage_mv_ck();                  /* move_stage -> stage_m's area-exit check (f_stage.c):
                                         * pl+0x738 = 1 -> game2 steps 2-6 load the next area */
    }
    if (quest_no && ticks >= 2) {
        void bgm_server(void);
        bgm_server();                   /* move(): fight music, quest clear / fail jingles (bgm_nm.c) */
    }
    if (quest_no || play)
        rt_hud_tick();                  /* Pit_mv: HUD layers (last step of move()) */
    if (snd == 0) {
        rt_snd_tick();
        audio_dump_tick();
    }
}

/* After the reward screen (game mode 6) the PS2 goes back to the village,
 * which is not ported: start the same quest again (stand-in). */
static void quest_back(void)
{
    int k, st;
    float p[3] = { rx, 0, rz };
    rt_monster_clear_all();
    if (rt_quest_load(quest_no) != 0)
        return;
    st = rt_quest_monster_stage(&k);
    if (!getenv("RT_QUEST_STAGE") || st < 0)
        st = rt_game_stage();           /* the quest's start stage (base camp) */
    if (st != stage_no)
        load_stage_models(st);
    stage_no = st;
    rt_game_init(stage_no);
    rt_hud_init();
    rt_monster_spawn(1, p, (int)(0.6f * 65536.0f / 6.2831853f));
    rt_player_game_init(0);
    if (game_cam)
        rt_cam_init(stage_no);
    {   /* game13's last step: the hunt fades in */
        void fade_set(int n);
        fade_set(2);
    }
}

/* Quest monsters other than the Rathian of the host's own set-up (the
 * game's em_create_model -> here): the model of kind `kind` (cached by
 * kind) and the motions of model slot `slot` (create_em_motion from
 * em<kind>_tbl.bin, as the PS2 loads them with the model). */
static monster em_mdl[40];
static int em_have[40];
static void em_model_load(int slot, int kind)
{
    char a[32], t[32], b[32];
    monster *e;
    if (kind <= 0 || kind >= 40)
        return;
    e = &em_mdl[kind];
    if (!em_have[kind]) {
        /* the game's per-kind AFS entries: model (load_enemy_model,
         * main 0x2EC7A0), textures (0x2EEE20) and motions (load_em_motion,
         * 0x2EC830); several kinds share files (the dromes use the
         * Velociprey / Genprey / Ioprey models and the em16 motions) */
        static const uint32_t tbl_va[3] = { 0x2EC7A0, 0x2EEE20, 0x2EC830 };
        char *nm[3] = { a, t, b };
        int k;
        snprintf(a, sizeof a, "em%02d_amh.bin", kind);
        snprintf(t, sizeof t, "em%02d_tex.bin", kind);
        snprintf(b, sizeof b, "em%02d_tbl.bin", kind);
        for (k = 0; k < 3; k++) {
            const uint8_t *q = elf_addr(tbl_va[k] + 4 * (uint32_t)kind);
            uint32_t idx = q ? fmt_u32(q, FMT_LE) : 0;
            if (idx > 0 && idx < afs.count)
                snprintf(nm[k], 32, "%s", afs.name[idx]);
        }
        if (monster_load(e, a, t, b, 0) != 0) {
            fprintf(stderr, "monster kind %d: model %s not loaded\n", kind, a);
            return;
        }
        em_have[kind] = 1;
    }
    if (e->tbl.p)
        rt_em_motion_create(slot, kind, e->tbl.p);
}

/* every monster in use on this stage but the host's Rathian (em_work[0]
 * with the em01 set-up above), posed by the game's motion player; also
 * their joint matrices for the game's hit checks */
static void monsters_sync(int draw, const fl_light *L)
{
    extern uint8_t em_work[];
    static flmat jw[20][128];       /* per slot: rt_monster_joints keeps the pointer */
    int i, j, nb;
    for (i = 0; i < 20; i++) {
        uint8_t *em = em_work + 0xA10 * i;
        monster *m;
        flmat w;
        float s[3], r[3], t[3];
        int kind = em[2];
        if (!em[0] || em[0x1E] || (i == 0 && rathian.game && kind <= 1) || kind <= 0 || kind >= 40 || !em_have[kind])
            continue;
        if (em[0x736] != (uint8_t)rt_game_stage())
            continue;
        m = &em_mdl[kind];
        rt_monster_pose(i, &m->skel);
        memcpy(s, em + 0xB8, sizeof s);
        if (s[0] == 0.0f) s[0] = s[1] = s[2] = 1.0f;
        r[0] = 0;
        r[1] = (float)(*(int32_t *)(em + 0xA4) & 0xFFFF) * (6.2831853f / 65536.0f);
        r[2] = 0;
        memcpy(t, em + 0xAC, sizeof t);
        flmat_srt(w, s, r, t);
        nb = m->skel.skel.nbone < 128 ? m->skel.skel.nbone : 128;
        for (j = 0; j < nb; j++)
            flmat_mul(jw[i][j], m->skel.world[j], w);
        rt_monster_joints(i, &jw[i][0][0], nb);
        if (draw) {
            fl_model_pose(&m->model, (const flmat *)m->skel.world, L);
            gfx_set_render_state(GFX_RS_WORLD, (uintptr_t)w);
            draw_model_attr(&m->model, -1);
        }
    }
}

/* Village NPC models (npc_create_model -> here): slot = NPC kind. */
static monster npc_mdl[4];
static int npc_have[4];
static void npc_model_load(int slot, int amh, int tex)
{
    monster *e = &npc_mdl[slot];
    fmt_blob link, tx, amo, ahi;
    if (npc_have[slot] || amh < 0 || (uint32_t)amh >= afs.count || tex < 0 || (uint32_t)tex >= afs.count)
        return;
    link = load(afs.name[amh], &e->mem[0]);
    tx = load(afs.name[tex], &e->mem[1]);
    if (!link.p)
        return;
    amo = fmt_link_entry(link, 0, FMT_LE);
    ahi = fmt_link_entry(link, 1, FMT_LE);
    if (fl_model_create(&e->model, amo, ahi, tx, 1, FMT_LE) != 0 || fl_skel_create(&e->skel, ahi, FMT_LE) != 0) {
        fprintf(stderr, "npc model %d (%s): load failed\n", slot, afs.name[amh]);
        return;
    }
    npc_have[slot] = 1;
    drop(&e->mem[0]);
    drop(&e->mem[1]);
    if (getenv("RT_QUEST_TRACE"))
        fprintf(stderr, "village: npc model %d = %s, %d parts, %d bones\n", slot, afs.name[amh], e->model.npart, e->skel.skel.nbone);
}

/* The NPCs on this stage (em_work slots with +0x1E): their model, posed by
 * the game's motion player, placed and scaled like Lb_npc_mk; villagers
 * (kind 0) show only their own parts (+0x4E6 per part, lb_npc_trans). */
static void npc_draw(const fl_light *L)
{
    extern uint8_t em_work[];
    int i, k;
    for (i = 0; i < 20; i++) {
        uint8_t *em = em_work + 0xA10 * i;
        monster *m;
        flmat w;
        float s[3], r[3], t[3];
        int kind = em[0x34F];
        if (!em[0] || !em[0x1E] || !em[1] || em[0x736] != (uint8_t)rt_game_stage() || kind > 3 || !npc_have[kind])
            continue;
        m = &npc_mdl[kind];
        rt_monster_pose(i, &m->skel);
        fl_model_pose(&m->model, (const flmat *)m->skel.world, L);
        memcpy(s, em + 0xB8, sizeof s);
        r[0] = 0;
        r[1] = (float)(*(int32_t *)(em + 0xA4) & 0xFFFF) * (6.2831853f / 65536.0f);
        r[2] = 0;
        memcpy(t, em + 0xAC, sizeof t);
        flmat_srt(w, s, r, t);
        gfx_set_render_state(GFX_RS_WORLD, (uintptr_t)w);
        for (k = 0; k < m->model.npart; k++) {
            if (kind == 0 && (k >= 0x20 || !em[0x4E6 + k]))
                continue;
            rt_clay_attr_set(part_attr(&m->model, k));
            gfx_execute_clay(m->model.part[k].clay);
            rt_clay_attr_reset();
        }
    }
}

/* The quest accepted in the village (game mode 0 -> game1/10/11/12/13 on
 * the PS2): Quest_init + Quest_start (rt_quest_load), the hunt starts on
 * the quest's own start stage (the base camp, game_w.stage), the hunter at
 * its start position (pl_init), the stage's monsters (Quest_em_init_set). */
static void quest_from_village(void)
{
    int st;
    float p[3] = { rx, 0, rz };
    rt_monster_clear_all();
    if (rt_quest_load(quest_no) != 0) {
        fprintf(stderr, "quest %d: no mission file\n", quest_no);
        return;
    }
    st = rt_game_stage();
    if (st != stage_no)
        load_stage_models(st);
    stage_no = st;
    rt_game_init(stage_no);
    rt_hud_init();
    rt_player_game_init(0);
    rt_monster_spawn(1, p, 0);
    if (game_cam)
        rt_cam_init(stage_no);
    {   /* game13's last step: the hunt fades in */
        void fade_set(int n);
        fade_set(2);
    }
    if (getenv("RT_QUEST_TRACE"))
        fprintf(stderr, "village: quest %d starts on stage %d\n", quest_no, stage_no);
}

/* After the reward screen (game mode 6): the village, as on the PS2
 * (rt_village.c runs lobby.bin's Local_main); a quest accepted at the
 * counter starts when the hunter leaves through the gate. RT_NO_VILLAGE=1
 * restarts the same quest instead (the old stand-in). */
static void village_step(void)
{
    int q;
    if (getenv("RT_NO_VILLAGE")) {
        quest_back();
        return;
    }
    if (!rt_village_active())
        rt_village_enter();
    q = rt_village_tick();
    if (q > 0) {
        if (getenv("RT_QUEST_TRACE"))
            fprintf(stderr, "village: quest %d accepted, leaving the village\n", q);
        quest_no = q;
        quest_from_village();
    } else if (q < 0) {
        quest_back();
    }
}

/* RT_MEM="t1,t2,...": the memory report (rt_memstat.c) at those host ticks */
static void mem_tick(int t)
{
    const char *m = getenv("RT_MEM");
    char where[32];
    while (m && *m) {
        if (atoi(m) == t) {
            snprintf(where, sizeof where, "tick %d", t);
            rt_ms_report(where);
            rt_area_report();
            rt_stack_report(where);
        }
        while (*m && *m != ',')
            m++;
        if (*m)
            m++;
    }
}

int main(int argc, char **argv)
{
    rt_stack_paint();
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--shot") && i + 1 < argc) shot = argv[++i];
        else if (!strcmp(argv[i], "--frames") && i + 1 < argc) frames = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--time") && i + 1 < argc) fixed_time = (float)atof(argv[++i]);
        else if (!strcmp(argv[i], "--size") && i + 1 < argc) sscanf(argv[++i], "%dx%d", &W, &H);
        else if (!strcmp(argv[i], "--cam") && i + 1 < argc)
            cam_given = sscanf(argv[++i], "%f,%f,%f,%f,%f", &cam[0], &cam[1], &cam[2], &cam[3], &cam[4]) > 0;
        else if (!strcmp(argv[i], "--stage") && i + 1 < argc) { stage_no = (int)strtol(argv[++i], NULL, 0); stage_given = 1; }
        else if (!strcmp(argv[i], "--quest") && i + 1 < argc) quest_no = (int)strtol(argv[++i], NULL, 0);
        else if (!strcmp(argv[i], "--play")) play = 1;
        else if (!strcmp(argv[i], "--boot")) { boot = 1; play = 1; }
        else if (!strcmp(argv[i], "--input") && i + 1 < argc) { script = argv[++i]; play = 1; }
        else if (!strcmp(argv[i], "--sw-trace")) sw_trace = 1;
        else if (!strcmp(argv[i], "--audio-dump") && i + 1 < argc) audio_dump = argv[++i];
        else if (!strcmp(argv[i], "--mute")) mute = 1;
        else if (!strcmp(argv[i], "--follow") && i + 1 < argc)
            follow_given = sscanf(argv[++i], "%f,%f,%f", &follow[0], &follow[1], &follow[2]) > 0;
        else if (argv[i][0] != '-') disc = argv[i];
    }
#ifdef XBOX
    /* Xbox (tools/build_xbox.py): no command line. Boot the game from
     * power-on with the player's own disc files next to the XBE (D:\data)
     * or on the hard disk (E:\Games\MH1\data, docs/xbox.md). */
    if (!disc) {
        static const char *dirs[] = { "D:\\data", "E:\\Games\\MH1\\data" };
        FILE *t;
        int d;
        nxMountDrive('E', "\\Device\\Harddisk0\\Partition1\\");
        for (d = 0; d < 2 && !disc; d++) {
            snprintf(path, sizeof path, "%s\\AFS_DATA.AFS", dirs[d]);
            if ((t = fopen(path, "rb")) != NULL) {
                fclose(t);
                disc = dirs[d];
            }
        }
        boot = 1;
        play = 1;
    }
#endif
    if (!disc) {
        fprintf(stderr, "usage:%s DISC_DIR [--shot out.png] [--frames N] [--time S] "
                "[--size WxH] [--cam x,y,z,yaw,pitch] [--stage N] [--play] [--input SCRIPT]\n", argv[0]);
        return 1;
    }
    snprintf(path, sizeof path, "%s/AFS_DATA.AFS", disc);
    if (fmt_afs_open(&afs, path) != 0) {
        fprintf(stderr, "cannot open %s\n", path);
        return 1;
    }
    snprintf(path, sizeof path, "%s/SLPM_654.95", disc);
    if (rt_load_elf(path) != 0)
        fprintf(stderr, "cannot read %s: no game data tables\n", path);
    {
        const char *mso = rt_ms_push("overlay binaries (game/lobby/select.bin)");
        uint8_t *ovl = fmt_afs_read(&afs, fmt_afs_find(&afs, "game.bin"), &n);   /* stored raw */
        rt_set_overlay(ovl, ovl ? n : 0);
        ovl = fmt_afs_read(&afs, fmt_afs_find(&afs, "lobby.bin"), &n);           /* the village overlay */
        rt_set_lobby(ovl, ovl ? n : 0);
        ovl = fmt_afs_read(&afs, fmt_afs_find(&afs, "select.bin"), &n);          /* the boot overlay (title, new hunter, load) */
        rt_set_select(ovl, ovl ? n : 0);
        rt_ms_pop(mso);
    }
    if (rt_import_data() != 0)
        fprintf(stderr, "some game data tables are missing\n");
    if (rt_import_lobby() != 0)
        fprintf(stderr, "some lobby data tables are missing\n");
    if (rt_import_select() != 0)
        fprintf(stderr, "select.bin is missing: no title screen\n");
    if (!getenv("RT_NO_TRIM"))
        rt_mem_trim();
    if (boot && !quest_no)
        quest_no = 10;      /* the set-up below as for a quest; the boot ends in the village (game mode 6) */
    if (gfx_init(W, H, "MH1 PC viewer", shot != NULL) != 0)
        return 1;
    if (script && !pad_script_set(script)) {
        fprintf(stderr, "bad --input script\n");
        return 1;
    }
    if (play)
        pad_init();

    rt_set_file_loader(afs_entry);
    rt_set_em_model_loader(em_model_load);  /* before the quest's em_create_model calls */
    if (quest_no) {
        /* --quest N: Quest_init + Quest_start as game11 does. The hunt
         * starts where the game starts it: the quest's start stage
         * (game_w.stage, the base camp), as a quest accepted in the
         * village does. Test aid RT_QUEST_STAGE=1: start on the stage of
         * the quest's own monster instead (the old scripted-test set-up);
         * --stage N overrides both. */
        int k = -1, st;
        if (rt_quest_load(quest_no) != 0)
            fprintf(stderr, "quest %d: no mission file\n", quest_no);
        else {
            st = rt_quest_monster_stage(&k);
            fprintf(stderr, "quest %d: monster kind %d on stage %d, start stage %d\n",
                    quest_no, k, st, rt_game_stage());
            if (!stage_given) {
                if (getenv("RT_QUEST_STAGE") && st >= 0)
                    stage_no = st;
                else
                    stage_no = rt_game_stage();
            }
        }
    } else if (play)
        rt_quest_free_hunt();           /* Quest_init: the free-hunt tables (HUD clock etc.) */
    /* the stage's area model + set model (stage.md 1), ground collision,
     * found through main's per-stage tables (stage 4 = st04, st04_1, lg004) */
    if (load_stage_models(stage_no) != 0)
        return 1;
    if (stage_no != 4) {
        /* no hand-picked spots: the hunter at the stage's start position
         * (stage_start_pos, main 0x2F2620, also used by set09/em19), else
         * at the middle of the walkable ground */
        extern float stage_start_pos[88][3];
        float sx = stage_start_pos[stage_no][0], sz = stage_start_pos[stage_no][2], x, z, y;
        int cnt = 0;
        if (sx == 0.0f && sz == 0.0f) {
            for (x = -30000; x <= 30000; x += 500)
                for (z = -30000; z <= 30000; z += 500)
                    if (rt_ground_y(x, z, 1e6f, &y)) {
                        sx += x;
                        sz += z;
                        cnt++;
                    }
            if (cnt) {
                sx /= cnt;
                sz /= cnt;
            }
        }
        hx = sx;
        hz = sz;
        rx = sx - 900;
        rz = sz - 1000;
        if (!cam_given) {
            y = 0;
            rt_ground_y(sx, sz + 2500, 1e6f, &y);
            cam[0] = sx;
            cam[1] = y + 600;
            cam[2] = sz + 2500;
            cam[3] = 0;
            cam[4] = -0.15f;
        }
    }
    load_eft_models();
    rt_game_init(stage_no);
    if (quest_no || play)
        rt_hud_init();                  /* load_pit, Pit_init, info banner */
    if (!mute)
        snd = rt_snd_init(disc, audio_dump == NULL && shot == NULL);

    if (monster_load(&rathian, "em01_amh.bin", "em01_tex.bin", "em01_tbl.bin", 3) != 0)
        fprintf(stderr, "em01 load failed\n");
    else if (rathian.tbl.p && !getenv("RT_HOST_MOTION")) {       /* animate with the game's create_em_motion/frame_move */
        static const int ids[3] = { 1003, 1203, 1403 };   /* slot 3 of banks 0/2/4 */
        rt_monster_motion_start(0, 0, rathian.tbl.p, 1, ids, 3);
        rathian.game = 1;
    }
    if (hunter_load(&pl, parts, 1, 101) != 0)
        fprintf(stderr, "hunter load failed\n");
    memcpy(pl.look, parts, sizeof pl.look);

    /* lighting: the VU1 model, 3 directional + ambient */
    memset(&light, 0, sizeof light);
    {
        float d[3][3] = { { -0.4f, -0.8f, -0.45f }, { 0.6f, -0.3f, 0.7f }, { 0, 1, 0 } };
        float c[3][3] = { { 0.75f, 0.72f, 0.65f }, { 0.25f, 0.27f, 0.32f }, { 0.08f, 0.08f, 0.08f } };
        int l;
        for (l = 0; l < 3; l++) {
            float len = sqrtf(d[l][0] * d[l][0] + d[l][1] * d[l][1] + d[l][2] * d[l][2]);
            int k;
            for (k = 0; k < 3; k++) {
                light.dir[l][k] = d[l][k] / len;
                light.col[l][k] = c[l][k];
            }
        }
        light.ambient[0] = light.ambient[1] = light.ambient[2] = 0.38f;
    }

    /* stand both on the ground: pose at frame 0, put the lowest vertex on
     * the collision floor */
    fl_skel_update(&rathian.skel, 0);
    fl_model_pose(&rathian.model, (const flmat *)rathian.skel.world, light_cur());
    if (getenv("RT_EM_POS"))            /* test placement of the Rathian: "x,z" */
        sscanf(getenv("RT_EM_POS"), "%f,%f", &rx, &rz);
    gy = 0;
    rt_ground_y(rx, rz, 1e6f, &gy);
    place(rathian.world, rx, gy - min_y_of(&rathian.model), rz, 0.6f);
    rathian_yoff = -min_y_of(&rathian.model);
    if (rathian.game && !getenv("RT_EM_FIXED")) {
        /* em_work[0] on the stage: the game moves it by its root motion
         * (walk loop 1003) and em_move's wall/ground collision keeps it on
         * the ground and inside the walls */
        float p[3] = { rx, gy, rz };
        if (getenv("RT_EM_STANDIN"))     /* old host stand-in: root motion and collision only */
            rt_monster_place(0, 1, p, (int)(0.6f * 65536.0f / 6.2831853f));
        else {                          /* the game's monster code: enemy_mv / em01 (rt_em.c) */
            if (getenv("RT_EM_KIND"))   /* test aid: another kind, with its own model */
                em_model_load(0, atoi(getenv("RT_EM_KIND")));
            rt_monster_spawn(1, p, (int)(0.6f * 65536.0f / 6.2831853f));
        }
        rathian.skel.root_lock = 1;
    }
    hunter_pose(&pl, 0, light_cur());
    {
        float lo = 1e30f;
        int s;
        for (s = 0; s < HUNTER_PARTS; s++) {
            float y = min_y_of(&pl.part[s]);
            if (y < lo)
                lo = y;
        }
        gy = 0;
        rt_ground_y(hx, hz, 1e6f, &gy);
        place(pl.world, hx, gy - lo, hz, 2.6f);
        {   /* the hunter is the master player (player_work[0]) for the game C */
            float p[3] = { hx, gy, hz };
            rt_set_player(0, p);
            rt_debug_spawn(p);          /* RT_SPAWN test effects at the hunter */
            if (pl.tbl.p && !getenv("RT_HOST_MOTION")) {   /* animate with the game's frame_init/frame_move */
                rt_player_motion_start(0, pl.tbl.p, 1, 101);
                pl.game = 1;
                pl.master.root_lock = 1;    /* the game moves the actor by the root motion */
                rt_player_set_ang(0, (int)(2.6f * 65536.0f / 6.2831853f));
                if (play && rt_player_uses_game()) {
                    /* the weapon class's own motions (ids >= 1000): wNN_tbl.bin,
                     * NN = job (PLW+2), like create_pl_motion's table */
                    static uint8_t *wmem;
                    char wname[32];
                    fmt_blob wt;
                    rt_player_game_init(0);     /* the game's pl_init: start position, idle */
                    snprintf(wname, sizeof wname, "w%02d_tbl.bin", rt_player_job(0));
                    wt = load(wname, &wmem);
                    if (wt.p)
                        rt_motion_load_pl(0, wt.p);
                    else
                        fprintf(stderr, "no %s: weapon motions missing\n", wname);
                    {   /* the weapon model: weapon_model_data / WEAPON_TEX[PLW+0x34C] (AFS indices) */
                        int mi = rt_weapon_afs(rt_player_weapon_model(0), 0), ti = rt_weapon_afs(rt_player_weapon_model(0), 1);
                        if (mi > 0 && mi < (int)afs.count && ti > 0 && ti < (int)afs.count) {
                            fmt_blob link = load(afs.name[mi], &weapon.mem[0]), tx = load(afs.name[ti], &weapon.mem[1]);
                            if (link.p && fl_model_create(&weapon.model, fmt_link_entry(link, 0, FMT_LE),
                                                          fmt_link_entry(link, 1, FMT_LE), tx, 1, FMT_LE) == 0
                                && fl_skel_create(&weapon.skel, fmt_link_entry(link, 1, FMT_LE), FMT_LE) == 0)
                                weapon.game = 1;
                            drop(&weapon.mem[0]);
                            drop(&weapon.mem[1]);
                        }
                    }
                }
            }
            hunter_yoff = -lo;
            if (play && pl.game) {
                rt_cam_init(stage_no);  /* the game camera follows player_work[0] (game2 moves it on stage changes) */
                game_cam = !follow_given && !getenv("RT_HOST_CAM");
            }
        }
    }

    if (snd == 0) {                     /* packs + stage stream; the Rathian (kind 1) is the stage's monster */
        static const int em_kinds[1] = { 1 };
        rt_snd_stage(stage_no, em_kinds, 1);
    }
    if (!shot)
        SDL_SetRelativeMouseMode(SDL_TRUE);
    rt_flow_set_core(sim_tick);
    rt_set_stage_loader(load_stage_models);
    rt_flow_set_back(quest_back);
    rt_flow_set_village(village_step);
    rt_set_npc_model_loader(npc_model_load);
    {
        void rt_set_text_input(void (*begin)(int), int (*take)(char *, int));
        rt_set_text_input(pad_text_mode, pad_text_take);
    }
    if (quest_no && getenv("RT_VILLAGE_START"))   /* test aid: straight to the village (game mode 6) */
        rt_flow_set_mode(6);
    if (boot) {         /* power-on: the game's boot tasks until Game_task (rt_boot.c) */
        rt_boot_init();
        booting = 1;
        rt_hunter_draw_hook = ed_hunter_hook;
    } else
        rt_sys_init();  /* the system tasks the boot would have started (Fade_task) */
    tick_trace = getenv("RT_TICK_TRACE") != NULL;
    if (shot && getenv("RT_SHOTS")) {   /* test aid: "t1,t2,...": with --shot X.png also X_<tick>.png at those game ticks (ascending) */
        shot_list = getenv("RT_SHOTS");
        shot_next = (int)strtol(shot_list, (char **)&shot_list, 10);
        if (*shot_list == ',')
            shot_list++;
    }
    t0 = SDL_GetTicks();
    while (running) {
        SDL_Event ev;
        float t = fixed_time >= 0 ? fixed_time : (SDL_GetTicks() - t0) / 1000.0f;
        float fr = t * 30.0f;                      /* game motions run at 30 fps */
        flmat proj, camw, view;
        const Uint8 *keys;
        float spd = 40;

        while (SDL_PollEvent(&ev)) {
            pad_event(&ev);     /* typed text (the name entry) */
            if (ev.type == SDL_QUIT || (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE))
                running = 0;
            else if (ev.type == SDL_MOUSEMOTION && !shot) {
                cam[3] -= ev.motion.xrel * 0.003f;
                cam[4] -= ev.motion.yrel * 0.003f;
                if (cam[4] > 1.5f) cam[4] = 1.5f;
                if (cam[4] < -1.5f) cam[4] = -1.5f;
            }
        }
        if (game_cam && have_view) {    /* look-at from the game camera's eye/target (roll ignored) */
            lookat_world(camw, gc_eye, gc_tar);
        } else {   /* camera: rotate pitch then yaw, looking down -Z like GL */
            float s[3] = { 1, 1, 1 }, r[3], tr[3];
            r[0] = cam[4]; r[1] = cam[3]; r[2] = 0;
            tr[0] = cam[0]; tr[1] = cam[1]; tr[2] = cam[2];
            flmat_srt(camw, s, r, tr);
        }
        keys = SDL_GetKeyboardState(NULL);
        if (!shot && !play) {
            if (keys[SDL_SCANCODE_LSHIFT]) spd *= 6;
            if (keys[SDL_SCANCODE_W]) { cam[0] -= camw[8] * spd; cam[1] -= camw[9] * spd; cam[2] -= camw[10] * spd; }
            if (keys[SDL_SCANCODE_S]) { cam[0] += camw[8] * spd; cam[1] += camw[9] * spd; cam[2] += camw[10] * spd; }
            if (keys[SDL_SCANCODE_A]) { cam[0] -= camw[0] * spd; cam[2] -= camw[2] * spd; }
            if (keys[SDL_SCANCODE_D]) { cam[0] += camw[0] * spd; cam[2] += camw[2] * spd; }
            if (keys[SDL_SCANCODE_SPACE]) cam[1] += spd;
            if (keys[SDL_SCANCODE_C]) cam[1] -= spd;
        }
        if (getenv("RT_CAM_DEBUG"))
            fprintf(stderr, "frame %d: eye %.0f %.0f %.0f fwd %.2f %.2f %.2f game_cam %d have_view %d\n", frame_no,
                    camw[12], camw[13], camw[14], -camw[8], -camw[9], -camw[10], game_cam, have_view);
        flmat_invert_affine(view, camw);
        rt_set_camera(camw);            /* rview_mat / rview_matY for game billboards */
        /* the game's angle of view taken as the vertical fov [guess] */
        flmat_perspective(proj, game_cam && have_view ? gc_fov : 1.0f, (float)W / H, 10.0f, 80000.0f);

        /* game logic ticks at 30 per second (at least 2, so set objects
         * have run their init and queued their prims) */
        if (shot && shot_next > 2 + (int)fr)
            shot_next = 0;              /* RT_SHOTS past --time: dropped */
        /* RT_PROF=1: host time per game tick (logic) and per drawn frame
         * (CPU side of the draw: posing, skinning, GL calls), every 300 ticks */
        static int prof = -1, prof_ticks0, prof_n, prof_fr;
        static double prof_logic, prof_draw;
        static Uint64 prof_t;
        if (prof < 0)
            prof = getenv("RT_PROF") != NULL;
        if (prof) {
            prof_t = SDL_GetPerformanceCounter();
            prof_ticks0 = ticks;
        }
        while (ticks < 2 + (int)fr && !(shot_next > 0 && ticks >= shot_next)) {
            if (booting) {      /* ACRMain: pad, then the task scheduler */
                pad_state ps;
                if (script)
                    pad_script_next(&ps);
                else
                    pad_read(&ps, 1);
                rt_pad_set(ps.bits, ps.lx, ps.ly, ps.rx, ps.ry);
                rt_pad_tick();
                if (rt_boot_tick()) {
                    booting = 0;
                    rt_flow_set_mode(6);    /* Game_task offline: the village */
                }
                if (snd == 0) {
                    rt_snd_tick();
                    audio_dump_tick();
                }
                ticks++;
                mem_tick(ticks);
                continue;
            }
            if (quest_no) {
                /* outside game2 the host tick (sim_tick) does not run: the
                 * pad is still read every tick (result / reward screens) */
                if (rt_flow_mode() != 2 && play) {
                    pad_state ps;
                    if (script)
                        pad_script_next(&ps);
                    else
                        pad_read(&ps, 1);
                    rt_pad_set(ps.bits, ps.lx, ps.ly, ps.rx, ps.ry);
                    if (rt_flow_mode() == 6)
                        rt_pad_read();      /* the village's Lb_pl_move runs swset */
                    else
                        rt_pad_tick();
                }
                rt_flow_tick();         /* game2 / game3 / game5 (f_game.c): game_core = sim_tick */
                rt_sys_tick();          /* Fade_task (the scheduler's system tasks) */
            }
            else
                sim_tick();
            ticks++;
                mem_tick(ticks);
            /* the joint matrices the next tick reads are those of the state
             * this tick left, whether or not a frame is drawn in between
             * (windowed and --shot runs stay tick-for-tick the same) */
            if (pl.game && play && ticks >= 2) {
                sync_joints(&pl, hunter_yoff, &rathian, rathian_yoff);
                if (!rt_village_active())
                    monsters_sync(0, light_cur());
            }
            if (tick_trace) {           /* RT_TICK_TRACE=1: compare windowed and headless runs */
                extern uint8_t em_work[];
                float p[3], es = 0;
                int a, k;
                rt_player_get(0, p, &a);
                for (k = 0; k < 20; k++)
                    es += ((float *)(em_work + 0xA10 * k + 0xAC))[0] + ((float *)(em_work + 0xA10 * k + 0xAC))[2];
                fprintf(stderr, "T %d m%d st%d pl %.2f %.2f %.2f %04X em %.2f\n", ticks, rt_flow_mode(),
                        rt_game_stage(), p[0], p[1], p[2], a & 0xFFFF, es);
            }
        }
        if (prof) {
            Uint64 t1 = SDL_GetPerformanceCounter();
            prof_logic += (double)(t1 - prof_t) * 1000.0 / (double)SDL_GetPerformanceFrequency();
            prof_n += ticks - prof_ticks0;
            prof_t = t1;
        }
        if (booting) {          /* the boot screens: the last tick's picture */
            gfx_begin_frame(0);
            rt_boot_draw();
            goto frame_done;
        }
        if (game_cam) {
            rt_cam_view(gc_eye, gc_tar, &gc_roll, &gc_fov);
            have_view = 1;
        }
        if (pl.game && play) {          /* hunter from player_work[0]; camera follows */
            float p[3];
            int a;
            rt_player_get(0, p, &a);
            place(pl.world, p[0], p[1] + hunter_yoff, p[2], (float)(a & 0xFFFF) * (6.2831853f / 65536.0f));
            if (!game_cam) {
            cam[0] = p[0] + sinf(cam[3]) * follow[0];
            cam[1] = p[1] + follow[1];
            cam[2] = p[2] + cosf(cam[3]) * follow[0];
            cam[4] = follow[2];
            }
        }
        if (rathian.game && rathian.skel.root_lock) {   /* drawn where the game has it */
            float p[3];
            int a;
            rt_monster_get(0, p, &a);
            place(rathian.world, p[0], p[1] + rathian_yoff, p[2], (float)(a & 0xFFFF) * (6.2831853f / 65536.0f));
        }
        {   /* the hunter the game built last (character, armour) */
            int sx, ids[HUNTER_PARTS], g = rt_player_look(0, &sx, ids);
            if (g && g != pl.look_gen) {
                pl.look_gen = g;
                hunter_relook(&pl, sx, ids);
            }
        }
        if (rathian.game)
            rt_monster_pose(0, &rathian.skel);
        else
            fl_skel_update(&rathian.skel, fr);
        fl_model_pose(&rathian.model, (const flmat *)rathian.skel.world, light_cur());
        hunter_pose(&pl, fr, light_cur());
        if (pl.game && play)            /* joint world matrices for the game C (parts, get_joint_pos) */
            sync_joints(&pl, hunter_yoff, &rathian, rathian_yoff);
        if (weapon.game && pl.game && play)
            weapon_pose(light_cur());

        if (getenv("RT_CAM_EM")) {      /* test aid "slot,dist,height,yaw": free camera on monster slot */
            float p[3], d = 1500, hh = 600, yw = 0;
            int a, sl = 0;
            sscanf(getenv("RT_CAM_EM"), "%d,%f,%f,%f", &sl, &d, &hh, &yw);
            rt_monster_get(sl, p, &a);
            cam[0] = p[0] + sinf(yw) * d;
            cam[1] = p[1] + hh;
            cam[2] = p[2] + cosf(yw) * d;
            cam[3] = yw;
            cam[4] = -atan2f(hh - 250.0f, d);
            game_cam = 0;
        }
        /* the view of this frame, from the camera the ticks above left
         * (the game camera or the follow camera moved with the hunter) */
        if (game_cam && have_view) {
            lookat_world(camw, gc_eye, gc_tar);
        } else {
            float s3[3] = { 1, 1, 1 }, r3[3], t3[3];
            r3[0] = cam[4]; r3[1] = cam[3]; r3[2] = 0;
            t3[0] = cam[0]; t3[1] = cam[1]; t3[2] = cam[2];
            flmat_srt(camw, s3, r3, t3);
        }
        flmat_invert_affine(view, camw);
        rt_set_camera(camw);
        flmat_perspective(proj, game_cam && have_view ? gc_fov : 1.0f, (float)W / H, 10.0f, 80000.0f);

        gfx_begin_frame(0x8098B8);
        gfx_set_render_state(GFX_RS_PROJECTION, (uintptr_t)proj);
        gfx_set_render_state(GFX_RS_VIEW, (uintptr_t)view);
        gfx_set_render_state(GFX_RS_ALPHA_REF, 0x40);
        gfx_set_render_state(GFX_RS_BLEND, 1);
        {
            flmat id;
            flmat_identity(id);
            gfx_set_render_state(GFX_RS_WORLD, (uintptr_t)id);
            gfx_set_render_state(GFX_RS_ZWRITE, 1);
            rt_stage_draw();            /* trans_stage: area model + placed set parts */
        }
        rt_game_draw();                 /* game C prims (set14 waterfalls) */
        if (rt_monster_shown(0) && slot0_rathian()) {     /* in use and on this stage */
            gfx_set_render_state(GFX_RS_WORLD, (uintptr_t)rathian.world);
            draw_model_attr(&rathian.model, -1);
        }
        gfx_set_render_state(GFX_RS_WORLD, (uintptr_t)pl.world);
        {
            int s;
            for (s = 0; s < HUNTER_PARTS; s++)
                draw_model_attr(&pl.part[s], -1);
        }
        if (rt_village_active())
            npc_draw(light_cur());
        else
            monsters_sync(1, light_cur());
        if (weapon.game && pl.game && play) {
            static flmat wid;
            flmat_identity(wid);
            gfx_set_render_state(GFX_RS_WORLD, (uintptr_t)wid);
            draw_model_attr(&weapon.model, -1);
        }
        rt_game_draw_2d();              /* screen layers: HUD, info banner, text (after the 3D scene) */
        rt_fade_draw();                 /* fade_draw: the screen fade (Fade_task) */
    frame_done:

        frame_no++;
        if (getenv("RT_FPS")) {         /* drawn frames per second (the game ticks at 30 regardless) */
            static Uint32 fps_t0; static int fps_n;
            Uint32 now = SDL_GetTicks();
            if (!fps_t0) fps_t0 = now;
            if (++fps_n, now - fps_t0 >= 1000) {
                fprintf(stderr, "fps %.1f\n", fps_n * 1000.0 / (now - fps_t0));
                fps_t0 = now; fps_n = 0;
            }
        }
        if (shot && shot_next > 0 && ticks >= shot_next) {     /* RT_SHOTS: a picture at each listed tick */
            char name[512];
            uint8_t *rgb = malloc((size_t)W * H * 3);
            gfx_read_pixels(rgb);
            snprintf(name, sizeof name, "%.*s_%d.png", (int)(strlen(shot) > 4 ? strlen(shot) - 4 : strlen(shot)), shot, shot_next);
            write_png(name, W, H, rgb);
            printf("wrote %s\n", name);
            free(rgb);
            shot_next = shot_list && *shot_list ? (int)strtol(shot_list, (char **)&shot_list, 10) : 0;
            if (shot_list && *shot_list == ',')
                shot_list++;
        }
        if (shot && frame_no >= frames && shot_next <= 0 && (!shot_list || ticks >= 2 + (int)fr)) {
            uint8_t *rgb = malloc((size_t)W * H * 3);
            gfx_read_pixels(rgb);
            write_png(shot, W, H, rgb);
            printf("wrote %s (%dx%d, motion frame %.1f)\n", shot, W, H, fr);
            free(rgb);
            running = 0;
        }
        if (prof) {
            prof_draw += (double)(SDL_GetPerformanceCounter() - prof_t) * 1000.0 / (double)SDL_GetPerformanceFrequency();
            prof_fr++;
            if (prof_n >= 300 || prof_fr >= 300) {
                fprintf(stderr, "prof: logic %.2f ms/tick (%d ticks), draw %.2f ms/frame CPU (%d frames)\n",
                        prof_n ? prof_logic / prof_n : 0.0, prof_n, prof_draw / prof_fr, prof_fr);
                prof_logic = prof_draw = 0;
                prof_n = prof_fr = 0;
            }
        }
        gfx_end_frame();
    }
    (void)n;
    if (audio_dump && dump_pcm) {
        write_wav(audio_dump, dump_pcm, dump_n / 2);
        printf("wrote %s (%.2fs)\n", audio_dump, dump_n / 2 / 48000.0);
    }
    if (snd == 0)
        rt_snd_shutdown();
    gfx_shutdown();
    fmt_afs_close(&afs);
    rt_stack_report("at exit");
    return 0;
}
