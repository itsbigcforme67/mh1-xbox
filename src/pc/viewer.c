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
#include "../../include/gwx.h"
#include "fl/fl.h"
#include "rt/rt.h"
#include "pad/pad.h"
#include "audio/audio.h"

#include <SDL.h>
#include "pick.h"
#include "menu.h"
#include "dbgmenu.h"
#ifdef MH1_WIN
#include <fcntl.h>
#include <io.h>
#endif
#include "gfx/gfx_gl.h"
#ifndef XBOX
#include "install.h"
#include "rt/rt_save.h"
#endif
#include <stdio.h>
#include "rt/rt_memstat.h"
#include "rt/rt_prof.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#define EM_KINDS 64     /* monster kinds the host model cache holds (MH1 35, G 52) */
#ifdef XBOX
#include <nxdk/mount.h>
#include "xbox/xbox_title.h"
void xbox_log_start(void);
void xbox_log_note(const char *msg);
void xbox_log_frame(int frame, unsigned ms);
void xbox_log_quit(void);
void xbox_env_load(void);
void xbox_env_note(void);
#endif

/* ------------------------------------------------------------ data */
static fmt_afs afs;

/* the AFS_DATA entry's file name, NULL past the end (the debug menu's quest list, rt_dbg.c) */
const char *rt_afs_name(int idx)
{
    return idx >= 0 && (uint32_t)idx < afs.count ? afs.name[idx] : NULL;
}

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
    const uint8_t *p = RTVA(table) ? rt_addr(RTVA(table) + 4 * (uint32_t)stage, 4) : NULL;    /* MH1 address; G: by name */
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
    return RTVA(va) ? rt_addr(RTVA(va), 4) : NULL;     /* MH1 addresses; G: through names */
}

/* CLAY+0x88 word of part k (Attribute_from_amo), 0 if it has no 0xF0000 chunk */
static uint32_t part_attr(const fl_model *m, int k)
{
    const amo_part *p = &m->amo.part[k];
    return p->has_attr ? rt_clay_attr_word(p->attr) : 0;
}

/* the bug reporter's tag for the model drawn next (what it is: kind, a, b of the PICK tag; skeleton for the nearest bone) */
static struct { int kind, a, b; const void *skel; } powner;
static void pick_owner(int kind, int a, int b, const void *skel)
{
    powner.kind = kind;
    powner.a = a;
    powner.b = b;
    powner.skel = skel;
}

/* fl_model_draw with each part's own blend/filter/clamp (clay_attr_set) */
/* em: a monster's em_work entry; its clays and materials are drawn as
 * enemy_trans does (rt_em_materials: clays switched off, materials hidden,
 * faded, re-coloured or re-textured); NULL = everything plainly.
 * light_col: the colour override was put into the light already (all
 * materials alike, monsters_sync), not applied per draw here. */
static void em_mat_trace(int kind, int part, int drawn, uint32_t hidden)
{
    static uint32_t seen[EM_KINDS][16][8];
    uint32_t key = drawn ? hidden | 0x80000000u : 0x40000000u;
    int k;
    if (part >= 16)
        return;
    for (k = 0; k < 8 && seen[kind % 40][part][k] && seen[kind % 40][part][k] != key; k++)
        ;
    if (k == 8 || seen[kind % 40][part][k])
        return;
    seen[kind % 40][part][k] = key;
    if (drawn)
        fprintf(stderr, "em-mat: kind %d part %d hides materials 0x%03x\n", kind, part, (unsigned)hidden);
    else
        fprintf(stderr, "em-mat: kind %d part %d not drawn\n", kind, part);
}

/* 1 when every visible material of every drawn clay of monster em has the
 * same diffuse override (Cephadrome, kind 8): f = override / the file's
 * diffuse. The VU1 programs multiply the directional light colours by the
 * material diffuse (graphics.md 4.2, MATERIAL), so the host scales its
 * light by f for this model instead of tinting per draw. */
static int em_model_col(fl_model *m, const void *em, float f[3])
{
    int i, k, any = 0;
    for (i = 0; i < m->npart; i++) {
        const amo_part *ap = &m->amo.part[i];
        rt_em_mat st[32];
        int n = ap->nmatlist < 32 ? ap->nmatlist : 32;
        if (!rt_em_materials(em, i, n, st))
            continue;
        for (k = 0; k < n; k++) {
            const float *cb;
            float g[3];
            int c;
            if (st[k].alpha <= 0.0f)
                continue;
            if (!st[k].has_col || ap->matlist[k] >= (uint32_t)m->amo.nmat)
                return 0;
            cb = m->amo.mat[ap->matlist[k]].col_b;
            for (c = 0; c < 3; c++)
                g[c] = cb[c] > 0.0f ? st[k].col[c] / cb[c] : st[k].col[c];
            if (!any)
                memcpy(f, g, sizeof g);
            else if (fabsf(f[0] - g[0]) > 0.01f || fabsf(f[1] - g[1]) > 0.01f || fabsf(f[2] - g[2]) > 0.01f)
                return 0;
            any = 1;
        }
    }
    return any;
}

static void draw_model_attr_em(fl_model *m, int sky, const void *em, int light_col, int only)
{
    int i;
    for (i = 0; i < m->npart; i++) {
        const amo_part *ap = &m->amo.part[i];
        fl_part *fp = &m->part[i];
        rt_em_mat st[32];
        int bst[32], g, b, n = ap->nmatlist < 32 ? ap->nmatlist : 32;
        uint32_t done;
        if (!(sky < 0 || sky == fp->is_sky) || (only >= 0 && i != only))
            continue;
        if (!em || getenv("RT_EM_ALL_MATS")) {     /* test aid: every clay and material as in the file */
            PICK_FN(powner.kind, powner.a, powner.b, i, 0, 0, powner.skel, 0);
            rt_clay_attr_set(part_attr(m, i));
            gfx_execute_clay(fp->clay);
            rt_clay_attr_reset();
            continue;
        }
        {
            int drawn = rt_em_materials(em, i, n, st) || only == i;     /* only: the cut tail, drawn apart */
            if (getenv("RT_EM_MAT_TRACE")) {       /* test aid (test_activities em_materials) */
                uint32_t h = 0;
                int k;
                for (k = 0; k < n; k++)
                    if (st[k].alpha <= 0.0f && ap->matlist[k] < 32)
                        h |= 1u << ap->matlist[k];
                em_mat_trace(((const uint8_t *)em)[2], i, drawn, h);
            }
            if (!drawn)
                continue;
        }
        /* each batch (one per material number) takes the state of its material; batches
         * with the same state are drawn together, the others hidden (GFX_RS_BATCH_HIDE) */
        for (b = 0; b < 32; b++) {
            int k;
            bst[b] = fp->batch_mat[b] == -2 ? -2 : -1;
            if (fp->batch_mat[b] < 0)
                continue;
            for (k = 0; k < n; k++)
                if (ap->matlist[k] == (uint32_t)fp->batch_mat[b])
                    break;
            bst[b] = k < n ? k : -2;        /* -2: not in the list, drawn plainly */
        }
        PICK_FN(powner.kind, powner.a, powner.b, i, 0, 0, powner.skel, 0);
        rt_clay_attr_set(part_attr(m, i));
        done = 0;
        for (g = 0; g < 33; g++) {
            /* pass g: the first batch not drawn yet picks the state; the batches with the same state go with it */
            uint32_t grp = 0, all = 0, hide, fade = 0xFFFFFFFFu;
            const rt_em_mat *s0 = NULL;
            int first = -1;
            for (b = 0; b < 32; b++) {
                const rt_em_mat *sb = bst[b] >= 0 ? &st[bst[b]] : NULL;
                if (bst[b] == -1)
                    continue;
                all |= 1u << b;
                if ((done >> b & 1) || (sb && sb->alpha <= 0.0f))     /* drawn, or hidden */
                    continue;
                if (first < 0) {
                    first = b;
                    s0 = sb;
                }
                if (b != first && ((sb == NULL) != (s0 == NULL)
                    || (sb && (sb->alpha != s0->alpha || sb->tex != s0->tex
                               || (!light_col && (sb->has_col != s0->has_col
                                                  || (sb->has_col && memcmp(sb->col, s0->col, sizeof sb->col))))))))
                    continue;
                grp |= 1u << b;
            }
            if (first < 0)
                break;
            done |= grp;
            hide = all & ~grp;
            if (s0) {
                float c[4] = { 1, 1, 1, s0->alpha < 1.0f ? s0->alpha : 1.0f };
                int k;
                if (s0->has_col && !light_col) {         /* diffuse override against the file's diffuse */
                    const float *cb = m->amo.mat[ap->matlist[bst[first]]].col_b;
                    for (k = 0; k < 3; k++)
                        c[k] = cb[k] > 0.0f ? s0->col[k] / cb[k] : s0->col[k];
                }
                if (c[0] != 1 || c[1] != 1 || c[2] != 1 || c[3] != 1) {
                    unsigned u[4];
                    for (k = 0; k < 4; k++)
                        u[k] = (unsigned)(c[k] <= 0 ? 0 : c[k] >= 1 ? 255 : c[k] * 255.0f + 0.5f);
                    fade = u[3] << 24 | u[0] << 16 | u[1] << 8 | u[2];
                }
                if (s0->tex >= 0 && s0->tex < m->ntex)
                    gfx_set_render_state(GFX_RS_BATCH_TEX, (uintptr_t)m->tex[s0->tex]);
                if (s0->alpha < 1.0f)
                    gfx_set_render_state(GFX_RS_ALPHA_REF, 0);  /* enemy_trans: alpha reference 0 while fading */
            }
            if (fade != 0xFFFFFFFFu)
                gfx_set_render_state(GFX_RS_FADE_COLOR, fade);
            if (hide)
                gfx_set_render_state(GFX_RS_BATCH_HIDE, hide);
            gfx_execute_clay(fp->clay);
            if (hide)
                gfx_set_render_state(GFX_RS_BATCH_HIDE, 0);
            if (fade != 0xFFFFFFFFu)
                gfx_set_render_state(GFX_RS_FADE_COLOR, 0xFFFFFFFFu);
            if (s0 && s0->tex >= 0)
                gfx_set_render_state(GFX_RS_BATCH_TEX, 0);
            if (s0 && s0->alpha < 1.0f)
                gfx_set_render_state(GFX_RS_ALPHA_REF, 0x40);
        }
        rt_clay_attr_reset();
    }
}

static void draw_model_attr(fl_model *m, int sky)
{
    draw_model_attr_em(m, sky, NULL, 0, -1);
}

/* The cut-off tail of a monster (eft09_t, rt_em_cut_tail): clay 1 alone, its tail bones in their
 * bind pose under Scale(EMW+0xB8) * RotY * Trans at the place it was cut, in world space. */
static void draw_cut_tail(fl_model *m, const fl_skel *sk, const uint8_t *em, const fl_light *L, int light_col)
{
    float pos[3], sc[3], r[3] = { 0, 0, 0 };
    flmat root, id, *wm;
    int k, skip[64];
    if (m->npart < 2 || m->npart > 64 || getenv("RT_EM_ALL_MATS") || !rt_em_cut_tail(em, pos, &r[1]))
        return;
    memcpy(sc, em + 0xB8, sizeof sc);
    if (sc[0] == 0.0f)
        sc[0] = sc[1] = sc[2] = 1.0f;
    flmat_srt(root, sc, r, pos);
    wm = malloc(sizeof(flmat) * (sk->skel.nbone + 1));
    if (wm && fl_skel_cut_tail(sk, root, wm)) {
        for (k = 0; k < m->npart; k++) {
            skip[k] = m->part[k].skip;
            m->part[k].skip = k != 1;
        }
        fl_model_pose(m, (const flmat *)wm, L);
        for (k = 0; k < m->npart; k++)
            m->part[k].skip = skip[k];
        flmat_identity(id);
        gfx_set_render_state(GFX_RS_WORLD, (uintptr_t)id);
        draw_model_attr_em(m, -1, em, light_col, 1);
    }
    free(wm);
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
        rt_clay_mats *mats;
        float (*dif)[4];
        int i;
        if (!link.p || fl_model_create(&eft_models[k], fmt_link_entry(link, 0, FMT_LE),
                                       fmt_link_entry(link, 1, FMT_LE), tex, 0, FMT_LE) != 0) {
            fprintf(stderr, "effect model %d: load failed\n", k);
            continue;
        }
        c = calloc((size_t)eft_models[k].npart + 1, sizeof *c);
        at = calloc((size_t)eft_models[k].npart + 1, sizeof *at);
        mats = calloc((size_t)eft_models[k].npart + 1, sizeof *mats);
        dif = calloc((size_t)eft_models[k].amo.nmat + 1, sizeof *dif);
        for (i = 0; i < eft_models[k].npart; i++) {
            const amo_part *ap = &eft_models[k].amo.part[i];
            const fl_part *fp = &eft_models[k].part[i];
            int j, b;
            c[i] = eft_models[k].part[i].clay;
            at[i] = part_attr(&eft_models[k], i);
            mats[i].n = ap->nmatlist < 32 ? ap->nmatlist : 32;     /* the materials the game can colour (rt_clay_mat_draw) */
            for (j = 0; j < mats[i].n; j++)
                mats[i].no[j] = ap->matlist[j];
            for (b = 0; b < 32; b++) {
                mats[i].bk[b] = -1;
                for (j = 0; j < mats[i].n && fp->batch_mat[b] >= 0; j++)
                    if (ap->matlist[j] == (uint32_t)fp->batch_mat[b]) {
                        mats[i].bk[b] = (int8_t)j;
                        break;
                    }
            }
        }
        for (i = 0; i < eft_models[k].amo.nmat; i++)
            memcpy(dif[i], eft_models[k].amo.mat[i].col_b, sizeof dif[i]);
        rt_bind_eft_model(k, c, at, eft_models[k].npart, mats, (const float (*)[4])dif, eft_models[k].amo.nmat);
        free(mats);
        free(dif);
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
    if (idx < 0 || (uint32_t)idx >= afs.count) {
        rt_warn_once("afs-idx", "file request for AFS_DATA entry %d: no such entry", idx);
        return NULL;
    }
    {
        const char *o = rt_ms_push("files for the game's loaders (load_file_mdl, kept copies)");
        uint8_t *p = fmt_afs_load(&afs, afs.name[idx], n);
        rt_ms_pop(o);
        if (!p)
            rt_warn("file %s (AFS_DATA entry %d) could not be read / decompressed", afs.name[idx], idx);
        else if (getenv("RT_MEM_FILES"))
            fprintf(stderr, "memstat: loader file %s %zu\n", afs.name[idx], *n);
        if (p && rt_ver_mhg() && rt_text_active() && strlen(afs.name[idx]) == 10 && afs.name[idx][0] == 'm' &&
            !strcmp(afs.name[idx] + 6, ".mib")) {   /* G mission file mNNNNN.mib: English quest text */
            size_t cap = *n + 0x4000;
            uint8_t *q = realloc(p, cap);
            if (q) {
                p = q;
                *n = rt_text_quest_g(atoi(afs.name[idx] + 1), p, *n, cap);
            }
        }
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

static uint8_t *put32(uint8_t *o, uint32_t v)
{
    o[0] = (uint8_t)(v >> 24); o[1] = (uint8_t)(v >> 16); o[2] = (uint8_t)(v >> 8); o[3] = (uint8_t)v;
    return o + 4;
}

/* a PNG chunk at o (length, type, data already at o + 8, CRC); returns the end */
static uint8_t *chunk(uint8_t *o, const char *type, size_t n)
{
    uint32_t c;
    put32(o, (uint32_t)n);
    memcpy(o + 4, type, 4);
    c = crc32_update(0, o + 4, 4 + n);
    return put32(o + 8 + n, c);
}

/* one whole file in one write. The Xbox's C library (pdclib) copies fwrite data byte by byte into a 1 KB buffer and
 * writes each kilobyte to the disk on its own: a bug report's three 900 KB pictures took over a minute on the owner's
 * Xbox (RELEASE_G GB5 bug 12); there the file goes to the disk in one WriteFile (xbox_write_file). */
#ifdef XBOX
int xbox_write_file(const char *path, const void *data, size_t n);
#endif
int viewer_write_file(const char *path, const void *data, size_t n)
{
#ifdef XBOX
    return xbox_write_file(path, data, n);
#else
    FILE *f = fopen(path, "wb");
    size_t w;
    if (!f)
        return -1;
    w = fwrite(data, 1, n, f);
    return fclose(f) == 0 && w == n ? 0 : -1;
#endif
}

/* PNG with stored (uncompressed) deflate blocks: no zlib needed. Built in memory, written in one go. */
static int write_png(const char *path, int w, int h, const uint8_t *rgb)
{
    size_t raw_n = (size_t)h * (w * 3 + 1), nblk = (raw_n + 65534) / 65535, pos = 0, i;
    size_t zn = 2 + raw_n + nblk * 5 + 4, total = 8 + 12 + 13 + 12 + zn + 12;
    uint8_t *png = malloc(total), *o, *z;
    uint32_t a = 1, b = 0;
    int y, r;
    if (!png)
        return -1;
    memcpy(png, "\x89PNG\r\n\x1a\n", 8);
    o = png + 8;
    put32(o + 8, (uint32_t)w);
    put32(o + 12, (uint32_t)h);
    o[16] = 8; o[17] = 2; o[18] = o[19] = o[20] = 0;
    o = chunk(o, "IHDR", 13);
    z = o + 8;                          /* the IDAT data: zlib header, stored blocks of the filtered rows, Adler-32 */
    *z++ = 0x78;
    *z++ = 0x01;
    for (i = 0; i < nblk; i++) {
        size_t len = raw_n - pos > 65535 ? 65535 : raw_n - pos, k = 0;
        z[0] = (uint8_t)(i == nblk - 1);
        z[1] = (uint8_t)len;
        z[2] = (uint8_t)(len >> 8);
        z[3] = (uint8_t)~len;
        z[4] = (uint8_t)(~len >> 8);
        z += 5;
        while (k < len) {               /* row bytes: filter 0 at each row start */
            size_t rp = (pos + k) % (size_t)(w * 3 + 1), take;
            y = (int)((pos + k) / (size_t)(w * 3 + 1));
            if (rp == 0) {
                z[k++] = 0;
                continue;
            }
            take = (size_t)(w * 3 + 1) - rp;
            if (take > len - k)
                take = len - k;
            memcpy(z + k, rgb + (size_t)y * w * 3 + (rp - 1), take);
            k += take;
        }
        for (k = 0; k < len; ) {        /* Adler-32, the modulo once per 5552 bytes */
            size_t end = k + 5552 < len ? k + 5552 : len;
            for (; k < end; k++) {
                a += z[k];
                b += a;
            }
            a %= 65521;
            b %= 65521;
        }
        z += len;
        pos += len;
    }
    z = put32(z, b << 16 | a);
    o = chunk(o, "IDAT", zn);
    o = chunk(o, "IEND", 0);
    r = viewer_write_file(path, png, (size_t)(o - png));
    free(png);
    return r;
}

int viewer_write_png(const char *path, int w, int h, const uint8_t *rgb) { return write_png(path, w, h, rgb); }

/* ------------------------------------------------------------ actors */
typedef struct {
    fl_model model;
    fl_skel skel;            /* drives model (same AHI) */
    flmat world;
    fmt_blob tbl;            /* *_tbl.bin */
    int game;                /* the weapon model (weapon_load): loaded */
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

static int lp = 0;                    /* this machine's player slot (game_w.master); 0 offline, co-op: rt_np.c */
static monster weapon;              /* the hunter's weapon (--play with the game's player code) */

/* weapon bones: hierarchy roots from rt_player_weapon (weapon_trans's
 * placement), the rest from their bind pose under the parent */
static void weapon_pose_of(monster *weapon_m, int no, const fl_light *L)
{
#define weapon (*weapon_m)
    float r0[16], r1[16];
    int i, roots = 0;
    ahi_skel *k = &weapon.skel.skel;
    if (rt_player_weapon(no, r0, r1) < 0)
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
#undef weapon
}
static void weapon_pose(const fl_light *L) { weapon_pose_of(&weapon, lp, L); }

static int monster_load(monster *e, const char *amh, const char *tex, const char *tbl)
{
    fmt_blob link = load(amh, &e->mem[0]), tx = load(tex, &e->mem[1]), tb = load(tbl, &e->mem[2]);
    fmt_blob amo, ahi;
    if (!link.p)
        return -1;
    e->tbl = tb;
    amo = fmt_link_entry(link, 0, FMT_LE);
    ahi = fmt_link_entry(link, 1, FMT_LE);
    if (fl_model_create(&e->model, amo, ahi, tx, 1, FMT_LE) != 0 || fl_skel_create(&e->skel, ahi, FMT_LE) != 0)
        return -1;
    drop(&e->mem[0]);
    drop(&e->mem[1]);
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
        h->ptmat[s] = tbl ? rt_ptr_at(RTVA(0x3018F0) + 4 * (uint32_t)s) : NULL;   /* relocated by rt_import_data */
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

/* Joint world matrices of the hunter (player_work[0]) for the game C: parts, get_joint_pos, hit_data_expand
 * (the monsters' are monsters_sync's). On the PS2 they come from the draw (trans) that runs between move()
 * and hit_check(), so this runs once per game tick, before hit_check. */
static void sync_joints(hunter *h, float hyoff)
{
    static flmat jw[128];
    flmat wg;
    float p[3];
    int a, nb, j;
    if (h->game) {
        rt_player_pose(lp, &h->master);
        rt_player_get(lp, p, &a);
        place(h->world, p[0], p[1] + hyoff, p[2], (float)(a & 0xFFFF) * (6.2831853f / 65536.0f));
        /* the game's joints (body_hit, the hit check, effects) at the hunter's own height: hyoff only lowers the drawn
         * model onto its feet. They had it until 20 Oct 2026, 0.3 units below the PS2's (the referee's REF_NODES) */
        place(wg, p[0], p[1], p[2], (float)(a & 0xFFFF) * (6.2831853f / 65536.0f));
        nb = h->master.skel.nbone < 128 ? h->master.skel.nbone : 128;
        for (j = 0; j < nb; j++)
            flmat_mul(jw[j], h->master.world[j], wg);
        rt_player_parts(lp, &jw[0][0], nb);
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
void rt_font_set_draw(int on);      /* rt_font.c: text drawn by the game, not the host (Windows build: no implicit declarations) */
static const char *disc = NULL, *shot = NULL, *install_iso = NULL, *install_dir = NULL;
#ifndef XBOX
/* PS2 saves given on the command line / dropped on the window (rt_save.c) */
static const char *drop_save[8];
static int ndrop_save;

/* tell the player what an import did: stderr, plus a message box unless RT_NO_GUI (tests) is set */
static void save_note(int ok, const char *msg)
{
    fprintf(ok ? stdout : stderr, "%s\n", msg);
    if (!getenv("RT_NO_GUI"))
        SDL_ShowSimpleMessageBox(ok ? SDL_MESSAGEBOX_INFORMATION : SDL_MESSAGEBOX_ERROR, ok ? "Save imported" : "Save not imported", msg, NULL);
}
#endif
static int install_only;
static int size_given, frames = 1, W = 1280, H = 720, i, running = 1, frame_no = 0;
/* the 3D scene's aspect: the window's (Xbox: 16:9 anamorphic), in widescreen mode (the vertical fov stays as the game
 * sets it, so the horizontal one grows: Hor+), else the original 4:3 (gfx_gl.c, gfx_nv2a.c) */
static float scene_aspect(void)
{
    return gfx_scene_aspect();
}

static float cam[5] = { 11900, 700, 8900, 0.75f, -0.2f };   /* x y z yaw pitch */
static float view_fov = 1.0f;   /* free camera's vertical angle of view (--view's 7th value; 1.0 rad before) */
static float fixed_time = -1;
static const char *shot_list;     /* RT_SHOTS */
static int shot_next;
static int tick_trace;
#define CATCH_UP 4                  /* ticks a drawn frame may run to catch up with real time */
static char path[1024];
static size_t n;
static fmt_blob stage_link, stage_tex, set_link, set_tex;
static uint8_t *keep[8];
static fl_model stage, set;
static hunter pl;
static fl_light light;
extern unsigned char light_work[];

/* The game's lights for the host's CPU lighting (docs/pc.md "Lighting"). light_work set 1 (light_work + 0x140: the
 * hunter / monster / NPC set that Pl_light_set hands to flSetRenderState(0x5A..0x5C)) holds three light blocks of
 * 0x68 bytes from +0x158: +0x04 colour rgb, +0x24 the ambient part of that light (the PS2 shader adds each light's
 * ambient row), +0x34 direction the light travels (the shader negates it). light_init fills them from
 * pl_light_tbl[stage], light_change_normal re-reads the stage rows, flash_move (thunder) blends the colours.
 * rt_light_get (rt_light.c) runs the game's own per-actor step (pl_light_change + Pl_light_set) for hunters and NPCs and gives
 * the stage rows to monsters and sets. Without RT_LIGHT_GAME, or while light_work is still empty (no stage lights yet): the
 * fixed default above. */
int rt_light_get(void *actor, int hunter, float dir[3][3], float col[3][3], float amb[3]);
void rt_light_tick(void);
void light_move(void);
void *rt_player_ptr(int no);
static fl_light light_game;          /* the game's lights for the actor asked about, else the fixed default above */
static const fl_light *light_for(void *actor, int hunter)
{
    static int on = -1;
    if (on < 0)
        on = getenv("RT_LIGHT_GAME") != NULL;
    if (on && rt_light_get(actor, hunter, light_game.dir, light_game.col, light_game.ambient)) {
        static int shown;
        if (getenv("RT_LIGHT_TRACE") && shown++ % 600 == 0) {
            int i;
            for (i = 0; i < 3; i++)
                fprintf(stderr, "light %d dir %.2f %.2f %.2f col %.2f %.2f %.2f\n", i, light_game.dir[i][0], light_game.dir[i][1],
                        light_game.dir[i][2], light_game.col[i][0], light_game.col[i][1], light_game.col[i][2]);
            fprintf(stderr, "light ambient %.2f %.2f %.2f%s\n", light_game.ambient[0], light_game.ambient[1], light_game.ambient[2],
                    actor ? (hunter ? " (hunter)" : " (npc)") : "");
        }
        return &light_game;
    }
    return &light;
}
static const fl_light *light_cur(void) { return light_for(NULL, 0); }
static const fl_light *light_hunter(int no) { return light_for(rt_player_ptr(no), 1); }
static const int parts[HUNTER_PARTS] = { 1, 0, 1, 1, 1, 1 };
static float hx = 10900, hz = 7700, rx = 10000, rz = 6700, gy;
static Uint32 t0;
static int set_h0 = -1, ticks = 0, stage_no = 4, cam_given = 0, stage_given = 0, quest_no = 0;
static float follow[3] = { 900.0f, 450.0f, -0.3f };
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
    rt_prof_begin(RTP_MIX);
    audio_mix(dump_pcm + dump_n, 1600);
    rt_prof_end(RTP_MIX);
    dump_n += 1600 * 2;
}

static float gc_eye[3] = { 0 }, gc_tar[3] = { 0 }, gc_roll = 0, gc_fov = 1.0f;   /* --play camera: distance, height, pitch */
static int play = 0, sw_trace = 0;          /* --play: the pad drives the hunter */
#ifdef MH1_ONLINE
int rt_net_test(const char *scenario);
static const char *nettest;               /* --nettest SCENARIO: headless online test, no window */
#endif
static int boot = 0, booting = 0;           /* --boot: from power-on (rt_boot.c) */
static int dbg_arg = 0;                     /* --debug-menu 1 / --no-debug-menu 2 */
static const char *script = NULL;
static int script_live = 1;                 /* the --input script still has steps */

/* each game tick's pad: the script or the controller (+ keyboard, unless the debug menu's free camera has it); the
 * debug menu takes its open combo out (dbgmenu_tick_pad) */
int rt_ref_pad(pad_state *ps);
static void read_pad(pad_state *ps)
{
    if (rt_ref_pad(ps))         /* the lockstep referee's pad (rt_ref.c, tools/referee.py) */
        return;
    if (script)
        script_live = pad_script_next(ps);
    else
        pad_read(ps, !dbgmenu_freecam());
    dbgmenu_tick_pad(ps);
    menu_tick_pad(ps);
    pick_tick_pad(ps);          /* the bug reporter's combo on the Xbox (Back + Black) */
}

/* a --input script kept in a file (--input @file; on the Xbox D:\data\input.txt):
 * the whole file, trailing blanks and line ends cut. NULL when it cannot be read. */
static char *script_file_read(const char *path)
{
    FILE *sf = fopen(path, "rb");
    size_t n, cap = 1 << 16, len = 0;
    char *b;
    if (!sf)
        return NULL;
    b = malloc(cap);
    while ((n = fread(b + len, 1, cap - len - 1, sf)) > 0) {
        len += n;
        if (len + 2 >= cap)
            b = realloc(b, cap *= 2);
    }
    b[len] = 0;
    while (len && (b[len - 1] == '\n' || b[len - 1] == '\r' || b[len - 1] == ' '))
        b[--len] = 0;
    fclose(sf);
    return b;
}
static float hunter_yoff = 0;

/* test aid RT_HOLD="t1,t2,...": the game clock stops at each listed game tick
 * for RT_HOLD_SECS seconds (default 20) while drawing goes on, with a log line,
 * so a screenshot taken from outside (xemu: tools/run_xemu.sh --after) shows
 * exactly that tick. Ascending ticks. */
static int test_hold_ticks(int ticks)
{
    static const char *list;
    static int inited, at = -1;
    static Uint32 until;
    if (!inited) {
        inited = 1;
        list = getenv("RT_HOLD");
        if (list && *list)
            at = (int)strtol(list, (char **)&list, 10);
    }
    if (at < 0 || ticks < at)
        return 0;
    if (!until) {
        const char *e = getenv("RT_HOLD_SECS");
        int secs = e ? atoi(e) : 20;
        until = SDL_GetTicks() + (Uint32)secs * 1000u;
        rt_log("hold: game tick %d held for %d s (RT_HOLD)", at, secs);
    }
    if (SDL_GetTicks() < until)
        return 1;
    until = 0;
    at = -1;
    if (list && *list == ',')
        at = (int)strtol(list + 1, (char **)&list, 10);
    return 0;
}

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
    hunter_pose(h, 0, light_hunter(no));
    rt_cam_view(eye, tar, &roll, &fov);
    lookat_world(camw, eye, tar);
    flmat_invert_affine(view, camw);
    flmat_perspective(proj, fov > 0.01f ? fov : 1.0f, scene_aspect(), 10.0f, 80000.0f);
    gfx_set_render_state(GFX_RS_PROJECTION, (uintptr_t)proj);
    gfx_set_render_state(GFX_RS_VIEW, (uintptr_t)view);
    gfx_set_render_state(GFX_RS_ALPHA_REF, 0x40);
    gfx_set_render_state(GFX_RS_BLEND, 1);
    gfx_set_render_state(GFX_RS_ZWRITE, 1);
    gfx_set_render_state(GFX_RS_WORLD, (uintptr_t)h->world);
    for (s = 0; s < HUNTER_PARTS; s++) {
        pick_owner(PK_HUNTER, no, s, &h->master);
        draw_model_attr(&h->part[s], -1);
    }
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
static void host_models_gc(void);
static int load_stage_models(int st)
{
    fl_model old_stage = stage, old_set = set;
    int k, reload = stage.npart > 0;
    rt_lt_begin(RTL_GC);
    host_models_gc();
    rt_lt_end(RTL_GC);
    for (k = 0; k < 4; k++)
        if (reload) {
            free(keep[k]);
            keep[k] = NULL;
        }
    stage_link = load_stage_file(0x2EC950, st, &keep[0]);   /* stage_model_data */
    stage_tex = load_stage_file(0x2EDB40, st, &keep[1]);    /* STAGE_TEX */
    set_link = load_stage_file(0x2ECD70, st, &keep[2]);     /* set_model_data */
    set_tex = load_stage_file(0x2EF130, st, &keep[3]);      /* SET_TEX */
#ifdef VERSION_MHG
    {   /* G: the stage file's environment block (link entry 2: fog, lights, sun), as load_stage_model's
         * func_0010FD10 reads it (rt_game.c rt_stage_env) */
        void rt_stage_env(const unsigned char *p, size_t n);
        if (stage_link.p && fmt_link_count(stage_link, FMT_LE) >= 3) {
            fmt_blob e = fmt_link_entry(stage_link, 2, FMT_LE);
            rt_stage_env(e.p, e.n);
        } else {
            rt_stage_env(NULL, 0);
        }
    }
#endif
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
    if (getenv("RT_LIGHT_TRACE") && atoi(getenv("RT_LIGHT_TRACE")) >= 2) {     /* lighting type (attr +0x14) of the area and set parts */
        int q;
        for (q = 0; q < stage.amo.npart; q++)
            fprintf(stderr, "stage %d area part %d (%d verts) lighting type %d (attr +0x04 family %d) uvscroll %d has_attr %d\n", st, q, stage.amo.part[q].nvert, stage.amo.part[q].attr[5], stage.amo.part[q].attr[1], stage.amo.part[q].attr[7], stage.amo.part[q].has_attr);
        for (q = 0; q < set.amo.npart; q++)
            fprintf(stderr, "stage %d set part %d (%d verts) lighting type %d (attr +0x04 family %d) uvscroll %d has_attr %d\n", st, q, set.amo.part[q].nvert, set.amo.part[q].attr[5], set.amo.part[q].attr[1], set.amo.part[q].attr[7], set.amo.part[q].has_attr);
    }
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
        {
            extern unsigned char rt_stage_sky_flags[64];
            extern int rt_stage_model_no;
            rt_stage_model_no = st;
            for (k = 0; k < nc; k++)
                rt_stage_sky_flags[k] = (unsigned char)stage.part[k].is_sky;
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
    } else {                    /* no set model here: the previous stage's set clays are released below, so set_mdlw
                                 * must not keep them (G stage 24 after an area change drew freed clays) */
        set_h0 = rt_bind_set_model(NULL, NULL, 0);
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

/* the weapon model of player no: weapon_model_data / WEAPON_TEX[PLW+0x34C] (AFS indices) */
static void weapon_load(monster *w, int no)
{
    int mi = rt_weapon_afs(rt_player_weapon_model(no), 0), ti = rt_weapon_afs(rt_player_weapon_model(no), 1);
    if (mi > 0 && mi < (int)afs.count && ti > 0 && ti < (int)afs.count) {
        fmt_blob link = load(afs.name[mi], &w->mem[0]), tx = load(afs.name[ti], &w->mem[1]);
        if (link.p && fl_model_create(&w->model, fmt_link_entry(link, 0, FMT_LE),
                                      fmt_link_entry(link, 1, FMT_LE), tx, 1, FMT_LE) == 0
            && fl_skel_create(&w->skel, fmt_link_entry(link, 1, FMT_LE), FMT_LE) == 0)
            w->game = 1;
        drop(&w->mem[0]);
        drop(&w->mem[1]);
    }
}

/* a hunter's weapon model again when the game changed it (player_work.work34C): the online town's hunters (their mini
 * data names the weapon; a hunter can change it in the town), the local hunter after a purchase. *have = the model
 * loaded, -1 = none yet. */
static int rt_online_town(void);
static int weapon_have = -1;         /* the local hunter's weapon model loaded */
__attribute__((unused)) static void weapon_follow(monster *w, int no, int *have)
{
    int m = rt_player_weapon_model(no);
    if (m == *have || rt_weapon_afs(m, 0) <= 0)
        return;
    if (w->game) {
        fl_model_release(&w->model);
        fl_skel_release(&w->skel);
        w->game = 0;
    }
    weapon_load(w, no);
    if (getenv("RT_ONLINE_TRACE") || getenv("RT_QUEST_TRACE"))
        fprintf(stderr, "%s: slot %d weapon model %d (was %d)%s\n", rt_online_town() ? "online" : "village", no, m, *have,
                w->game ? "" : ", not loaded");
    *have = m;
}

/* The local hunter's weapon class motions (ids >= 1000): wNN_tbl.bin, NN = job (PLW+2), through
 * create_pl_motion, at the start and at every quest start as the PS2's quest set-up does
 * (pl_motion_load): the village's lbcom motions run past the common part of the motion table
 * into the hunter's (entries 500-521) and replaced some of them. The file stays while its
 * sets live; another class's file replaces it (a bought weapon). */
int rt_motion_refs(const void *p, size_t n);
static void weapon_motions(void)
{
    static uint8_t *wmem, *old;
    static size_t wn, oldn;
    static int job = -1;
    char wname[32];
    fmt_blob wt;
    if (old && !rt_motion_refs(old, oldn)) {
        free(old);
        old = NULL;
    }
    if (!wmem || rt_player_job(lp) != job) {
        if (wmem) {
            free(old);              /* (a third class within one stage: not seen) */
            old = wmem;
            oldn = wn;
            wmem = NULL;
        }
        job = rt_player_job(lp);
        snprintf(wname, sizeof wname, "w%02d_tbl.bin", job);
        wt = load(wname, &wmem);
        wn = wt.n;
        if (!wt.p) {
            fprintf(stderr, "no %s: weapon motions missing\n", wname);
            return;
        }
    }
    rt_motion_load_pl(lp, wmem);
    if (getenv("RT_QUEST_TRACE"))
        fprintf(stderr, "viewer: weapon motions w%02d_tbl created\n", job);
}

/* ------------------------------------------------------------ co-op hunters
 * The other players' hunters (ONLINE=1, rt_np.c): their player works are moved by the
 * game's own code from the packets (net_receive_pl -> Pl_act_set, Pl_adj_calc); the host
 * draws them like the local hunter, with default parts until the game builds their look. */
#ifdef MH1_ONLINE
int rt_np_arg(int argc, char **argv, int *i);
int rt_np_wanted(void);
int rt_np_setup(int quest_no);
int rt_np_slot(void);
int rt_np_players(void);
int rt_np_visible(int slot);
void rt_np_tick(void);
void rt_np_close(void);
int rt_np_session_end(void);
static hunter rh[8];                /* 4 in a co-op quest, up to 8 in the online town */
static monster rw[8];               /* their weapons */
static int rw_model[8] = { -1, -1, -1, -1, -1, -1, -1, -1 };  /* the weapon model each holds (weapon_follow) */
static int rh_ok[8];
static uint8_t *rh_wmem[8];
int rt_online_active(void);
int rt_online_visible(int slot);
static void remote_motion_start(void)
{
    int s;
    for (s = 0; s < 4 && s < rt_np_players(); s++)
        if (s != lp && pl.game)     /* (plcom is loaded by then: pl.tbl may be gone, host_models_gc) */
            rt_player_motion_start(s, pl.tbl.p, 1, 101);
}
static void remote_weapons(void)
{
    int s;
    for (s = 0; s < 4 && s < rt_np_players(); s++) {
        char wname[32];
        fmt_blob wt;
        if (s == lp)
            continue;
        snprintf(wname, sizeof wname, "w%02d_tbl.bin", rt_player_job(s));
        free(rh_wmem[s]);
        rh_wmem[s] = NULL;
        wt = load(wname, &rh_wmem[s]);
        if (wt.p)
            rt_motion_load_pl(s, wt.p);
        if (!rw[s].game) {
            weapon_load(&rw[s], s);
            rw_model[s] = rt_player_weapon_model(s);
        }
    }
}
static void remote_hunters(int draw, const fl_light *L)
{
    static flmat jw[128];
    int s;
    for (s = 0; s < 8; s++) {
        hunter *h = &rh[s];
        float p[3];
        int a, nb, j, sx, ids[HUNTER_PARTS], g;
        if (s == lp || !(rt_online_active() ? rt_online_visible(s) : rt_np_visible(s)))
            continue;
        if (!rh_ok[s]) {
            rh_ok[s] = hunter_load(h, parts, 1, 101) == 0 ? 1 : -1;
            memcpy(h->look, parts, sizeof h->look);
            h->game = 1;
            h->no = s;
            h->master.root_lock = 1;
        }
        if (rh_ok[s] < 0)
            continue;
        g = rt_player_look(s, &sx, ids);
        if (g && g != h->look_gen) {
            h->look_gen = g;
            hunter_relook(h, sx, ids);
        }
        if (rt_online_active())     /* the town: each hunter's own weapon (co-op quests load them at the start) */
            weapon_follow(&rw[s], s, &rw_model[s]);
        rt_player_get(s, p, &a);
        place(h->world, p[0], p[1] + hunter_yoff, p[2], (float)(a & 0xFFFF) * (6.2831853f / 65536.0f));
        if (!draw) {        /* the joint matrices the game C reads (hit volumes of the parts) */
            rt_player_pose(s, &h->master);
            nb = h->master.skel.nbone < 128 ? h->master.skel.nbone : 128;
            for (j = 0; j < nb; j++)
                flmat_mul(jw[j], h->master.world[j], h->world);
            rt_player_parts(s, &jw[0][0], nb);
            continue;
        }
        L = light_hunter(s);
        hunter_pose(h, 0, L);
        gfx_set_render_state(GFX_RS_WORLD, (uintptr_t)h->world);
        for (j = 0; j < HUNTER_PARTS; j++) {
            pick_owner(PK_HUNTER, s, j, &h->master);
            draw_model_attr(&h->part[j], -1);
        }
        if (rw[s].game) {
            pick_owner(PK_WEAPON, s, 0, NULL);
            static flmat wid;
            weapon_pose_of(&rw[s], s, L);
            flmat_identity(wid);
            gfx_set_render_state(GFX_RS_WORLD, (uintptr_t)wid);
            draw_model_attr(&rw[s].model, -1);
        }
    }
}
#else
static void remote_motion_start(void) {}
static void remote_weapons(void) {}
static void remote_hunters(int draw, const fl_light *L) { (void)draw; (void)L; }
#endif
#ifdef MH1_ONLINE
static int rt_online_town(void) { return rt_online_active(); }
#else
static int rt_online_town(void) { return 0; }
#endif

/* ------------------------------------------------------------ one game tick
 * What game_core does on the PS2 (swset, move, trans, hit_check), done by
 * the host pieces in the PS2 order. With --quest it runs inside the game's
 * own mode loop (game2 -> game_core, src/main/game/f_game.c; rt_flow.c). */
static void monsters_sync(int draw, const fl_light *L);
int rt_monster_motion_ready(int no);
static void sim_tick(void)
{
    if (pl.game && play && ticks >= 2) {
        pad_state ps;
        read_pad(&ps);
        rt_pad_set(ps.bits, ps.lx, ps.ly, ps.rx, ps.ry);
#ifdef MH1_ONLINE
        rt_np_tick();           /* co-op: the other players' packets (net_receive_pl) */
#endif
        rt_player_tick(lp);
        /* right stick turns the follow camera */
        cam[3] -= ps.rx * (0.04f / 127.0f);
        if (sw_trace) {
            int now, ang, pw;
            float p[3];
            int a;
            rt_player_sw(lp, &now, &ang, &pw);
            rt_player_get(lp, p, &a);
            printf("tick %d: sw %04X stick ang %04X pow %d -> pos %.0f %.0f %.0f ang %04X\n",
                   ticks, now, ang, pw, p[0], p[1], p[2], a & 0xFFFF);
        }
    } else if (pl.game) {
        rt_player_motion_tick(lp);
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
    /* game_core's order (f_frame_nm.c move(), then trans() and hit_check()): the player, the monsters (slots 0..19), body_hit,
     * ..., the effects / shells / sets, the area exit check, the HUD; the joints of this tick's poses and hit_check last.
     * Until 18 Oct 2026 hit_check ran before the monsters moved and body_hit after the effects: a monster's attack hit
     * the hunter one tick later than on the PS2 (the lockstep referee, docs/referee.md) */
    if (ticks >= 2) {
        int i;
        for (i = 0; i < 20; i++)        /* move()'s monster loop, slots 0..19 as on the PS2 (f_frame_nm.c). em_work[0] ran last
                                         * until 18 Oct 2026: the monsters then drew their random numbers in another order and
                                         * the small ones of an area started elsewhere (the lockstep referee, docs/referee.md) */
            rt_monster_tick(i);
        if (sw_trace && rt_monster_shown(0)) {
            float p[3];
            int a;
            rt_monster_get(0, p, &a);
            printf("tick %d: em0 pos %.0f %.0f %.0f ang %04X\n", ticks, p[0], p[1], p[2], a & 0xFFFF);
        }
    }
    if (ticks >= 2 && !(getenv("RT_BODY_HIT") && getenv("RT_BODY_HIT")[0] == '0')) {
        extern unsigned char game_w[];
        void body_hit(void);
        if (game_w[GWX(0x21F)] == 0)         /* move(): item_check / body_hit only while info_stop == 0 (hunters and monsters pushed apart) */
            body_hit();
    }
    if (ticks >= 2) {
        /* move()'s wall pass after body_hit (f_frame_nm.c): every hunter (in an online game, or with PLW+0x7EC set) and
         * every monster with +0x7EC set is pushed out of the walls again with keep = 1. The PC lacked it until 20 Oct
         * 2026: a hunter pushed into a wall by a monster stayed there for a tick and the next tick's push started from
         * there (the lockstep referee, Kut-Ku fight) */
        extern unsigned char game_w[], player_work[], em_work[];
        void HitWallPlayer(void *w, int keep);
        int k;
        for (k = 0; k < 8; k++) {
            unsigned char *w = player_work + 0xA00 * k;
            if (w[0] && (game_w[0x2E] == 1 || w[0x7EC]))
                HitWallPlayer(w, 1);
        }
        for (k = 0; k < 20; k++) {
            unsigned char *w = em_work + 0xA10 * k;
            if (w[0] && w[0x7EC])
                HitWallPlayer(w, 1);
        }
    }
    if (pl.game && play && ticks >= 2) {
        /* player_mk (move(), after body_hit): the hunters' joint matrices of this tick, before the effects / sets read them
         * (the monsters' come from enemy_mk, right after each one moved: rt_monster_tick -> em_pose_now). Until 18 Oct
         * 2026 all joints were made at the end of the tick, so body_hit and the effects saw the last tick's */
        rt_prof_begin(RTP_JOINTS);
        sync_joints(&pl, hunter_yoff);
        remote_hunters(0, light_cur());
        rt_prof_end(RTP_JOINTS);
    }
    if (pl.game && play && ticks >= 2 && game_cam) {
        /* CameraMove: in move() after body_hit, the wall pass and player_mk (f_frame_nm.c), so the camera follows where
         * the hunter ended up this tick. It ran right after the player until 20 Oct 2026: after a monster pushed the
         * hunter the camera (and with it the stick's direction for the next tick) was a tick behind (the referee, Kut-Ku) */
        flmat cw;
        rt_cam_tick();      /* CameraMove (src/main/cam) */
        /* rview_mat follows the game camera every tick (sound
         * distances, billboards), also when several ticks run
         * in one drawn frame */
        rt_cam_view(gc_eye, gc_tar, &gc_roll, &gc_fov);
        lookat_world(cw, gc_eye, gc_tar);
        rt_set_camera(cw);
    }
    rt_game_move();             /* move_set / move_eft / move_shell: after the player and CameraMove as in game_core (f_frame_nm.c), so
                                 * camera-attached sets (set13's fog veils, 50 units in front of the eye) use this tick's camera */
    light_move();               /* game_core's step after CameraMove (f_frame_nm.c): turns light 2 of set 1 with the view, runs the
                                 * flash effect. The host's sim_tick stands in for game_core, so it has to call it (only
                                 * the lights of RT_LIGHT_GAME read what it does) */
    if (quest_no && pl.game && play && ticks >= 2) {
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
    if (snd == 0)
        rt_snd_stage_tick();            /* move_stage's river / waterfall loops (Snd_server runs per tick below) */
    if (pl.game && play && ticks >= 2) {
        rt_prof_begin(RTP_JOINTS);
        rt_actor_nodes_fill();  /* every live actor's node array as after trans() (rt_eft.c): the joints made in move()
                                 * (player_mk above, enemy_mk per monster); trans() makes none, so hit_check sees those */
        rt_prof_end(RTP_JOINTS);
        rt_hit_check();         /* hit_check (src/main/hit/hit_nm.c), as game_core does after trans */
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
    remote_motion_start();
    rt_player_game_init(lp);
    remote_weapons();
    weapon_motions();
    if (game_cam)
        rt_cam_init(stage_no);
    {   /* game13's last step: the hunt fades in */
        void fade_set(int n);
        fade_set(2);
    }
}

/* Monster models (the game's em_create_model -> em_model_load): the model of kind `kind` (cached by
 * kind) and the motions of model slot `slot` (create_em_motion from em<kind>_tbl.bin, as the PS2 loads
 * them with the model). Every monster in em_work is posed, drawn and given joints by monsters_sync. */
static monster em_mdl[EM_KINDS];
static int em_have[EM_KINDS];
static monster *em_kind_model(int kind)
{
    char a[32], t[32], b[32];
    monster *e;
    if (kind <= 0 || kind >= EM_KINDS)
        return NULL;
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
            const uint8_t *q = RTVA(tbl_va[k]) ? rt_addr(RTVA(tbl_va[k]) + 4 * (uint32_t)kind, 4) : NULL;
            uint32_t idx = q ? fmt_u32(q, FMT_LE) : 0;
            if (idx > 0 && idx < afs.count)
                snprintf(nm[k], 32, "%s", afs.name[idx]);
        }
        if (getenv("RT_EM_MODEL")) {    /* test aid "kind:emNN[:emMM]": that kind drawn with emNN's model and
                                         * textures (and emMM's motion file); G's unused kind 40 has no model of
                                         * its own (docs/FINDS.md): 40:em09 gives it the Felyne's body */
            int kk, n1 = -1, n2 = -1;
            if (sscanf(getenv("RT_EM_MODEL"), "%d:em%d:em%d", &kk, &n1, &n2) >= 2 && kk == kind) {
                snprintf(a, sizeof a, "em%02d_amh.bin", n1);
                snprintf(t, sizeof t, "em%02d_tex.bin", n1);
                if (n2 >= 0)
                    snprintf(b, sizeof b, "em%02d_tbl.bin", n2);
            }
        }
        if (monster_load(e, a, t, b) != 0) {
            fprintf(stderr, "monster kind %d: model %s not loaded\n", kind, a);
            return NULL;
        }
        /* the game moves the monster by its root motion (frame_move -> pl_velocity_sub) and the PS2 then clears the
         * root bone's translation (FRSKL.vel = 0) before drawing: keep that bone's X/Z at its bind value, as for the
         * Rathian and the hunter. Without it the walk loops were drawn moving forward on top of the game's movement
         * (up to 390 units ahead of the collision position, so the body went into rocks) and snapped back ~300 units
         * at every loop wrap: owner report 8 Oct 2026, "keeps rewind moving and walking into walls" (Aptonoth, stage 39) */
        e->skel.root_lock = !getenv("RT_EM_ROOT_FREE");
        em_have[kind] = 1;
    }
    return e;
}
static void em_model_load(int slot, int kind)
{
    monster *e = em_kind_model(kind);
    if (e && e->tbl.p)
        rt_em_motion_create(slot, kind, e->tbl.p);
}

/* every monster in use on this stage, posed by the game's motion player; also their joint matrices for
 * the game's hit checks */
int rt_log_tick(void);
static flmat em_jw[20][128];        /* per slot: rt_monster_joints keeps the pointer */
/* em_work[i]: posed by the game's motion player, its joint matrices handed to the game (and drawn when draw) */
static void em_slot_sync(int i, int draw, const fl_light *L)
{
    extern uint8_t em_work[];
    flmat (*jw)[128] = em_jw;
    int j, nb;
    uint8_t *em = em_work + 0xA10 * i;
    monster *m;
    flmat w;
    float s[3], r[3], t[3];
    int kind = em[2];
    if (!em[0] || em[0x1E] || kind <= 0 || kind >= EM_KINDS || !em_have[kind])
        return;
    if (em[0x736] != (uint8_t)rt_game_stage())
        return;
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
    if (!draw && getenv("RT_EM_DRAW_TRACE") && rt_monster_motion_ready(i) && nb > 1) {
        /* test aid: where the drawn body is (the root motion bone, AAN bone 1 of group 0, in world space) next to
         * the game position; printed twice per tick, by the joint sync before hit_check and the one after the tick (tools/test_activities.py herbivore_rewind) */
        int b, k = 0;
        for (b = 0; b < nb; b++)
            if (m->skel.skel.bone[b].group == 0 && k++ == 1)
                break;
        if (b < nb)
            printf("emdraw t%d slot %d kind %d mot %d frame %.1f pos %.1f %.1f body %.1f %.1f %.1f\n", rt_log_tick(), i,
                   kind, ((uint16_t *)(em + 0x2DC))[0], *(float *)(em + 0x19C), t[0], t[2], jw[i][b][12],
                   jw[i][b][13], jw[i][b][14]);
    }
    static float (*hmax)[1200], (*lmin)[1200];     /* RT_POSE_CHECK, per kind and motion: allocated for the test aid only (375 KB) */
    if (getenv("RT_POSE_CHECK") && rt_monster_motion_ready(i) && (hmax || (hmax = calloc(80, sizeof *hmax)))) {
        lmin = hmax + 40;
        /* test aid: highest joint above the monster's feet per kind (tools/test_activities.py herbivore_pose) */
        int mo = ((uint16_t *)(em + 0x2DC))[0] % 1200;
        float hh = 0;
        for (j = 0; j < nb; j++)
            if (jw[i][j][13] - t[1] > hh)
                hh = jw[i][j][13] - t[1];
        {   /* lowest joint: a body sunk into the ground (hit reactions, death) */
            float lo = 0;
            for (j = 0; j < nb; j++)
                if (jw[i][j][13] - t[1] < lo)
                    lo = jw[i][j][13] - t[1];
            if (lo < lmin[kind][mo] - 20.0f) {
                lmin[kind][mo] = lo;
                fprintf(stderr, "pose-check: kind %d slot %d joints down to %.0f below the feet (motion %d)\n", kind, i, lo, mo);
            }
        }
        if (hh > hmax[kind][mo] + 20.0f) {
            hmax[kind][mo] = hh;
            fprintf(stderr, "pose-check: kind %d slot %d joints up to %.0f above the feet (motion %d)\n", kind, i,
                    hh, mo);
        }
    }
    if (draw && em[1] && rt_monster_motion_ready(i)) {   /* em+1 (x01) 0: not drawn (em_move: no prim without it) */
        fl_light lc;
        float f[3];
        int light_col = L && !getenv("RT_EM_ALL_MATS") && em_model_col(&m->model, em, f);
        if (light_col) {            /* the diffuse override scales the directional lights (em_model_col) */
            int l, c;
            if (getenv("RT_EM_MAT_TRACE")) {
                static char said[EM_KINDS];
                if (!said[kind]++)
                    fprintf(stderr, "em-mat: kind %d light colour %.3f %.3f %.3f\n", kind, f[0], f[1], f[2]);
            }
            lc = *L;
            for (l = 0; l < 3; l++)
                for (c = 0; c < 3; c++)
                    lc.col[l][c] *= f[c];
        }
        fl_model_pose(&m->model, (const flmat *)m->skel.world, light_col ? &lc : L);
        gfx_set_render_state(GFX_RS_WORLD, (uintptr_t)w);
        pick_owner(PK_MONSTER, i, kind, &m->skel);
        draw_model_attr_em(&m->model, -1, em, light_col, -1);
        draw_cut_tail(&m->model, &m->skel, em, light_col ? &lc : L, light_col);
    }
}
static void monsters_sync(int draw, const fl_light *L)
{
    int i;
    for (i = 0; i < 20; i++)
        em_slot_sync(i, draw, L);
}
/* enemy_mk's part of the tick (move(): enemy_mv -> enemy_mk -> em_ride_sub): the new pose's joints, before body_hit
 * and em_ride_sub (the Lao-Shan Lung carrying the hunters) read them (rt_em.c) */
static void em_pose_now(int i)
{
    void rt_actor_nodes_fill_one(void *work);
    extern uint8_t em_work[];
    em_slot_sync(i, 0, NULL);
    rt_actor_nodes_fill_one(em_work + 0xA10 * i);
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
    /* NPCs move by their root motion too (the village's walkers): keep the root bone's X/Z at its bind value as for
     * the monsters (em_kind_model). Without it they were drawn walking ahead of their position and jumping back at
     * every walk loop (owner report from the Xbox, 12 Oct 2026; the PC had it too) */
    e->skel.root_lock = !getenv("RT_EM_ROOT_FREE");
    npc_have[slot] = 1;
    drop(&e->mem[0]);
    drop(&e->mem[1]);
    if (getenv("RT_QUEST_TRACE"))
        fprintf(stderr, "village: npc model %d = %s, %d parts, %d bones\n", slot, afs.name[amh], e->model.npart, e->skel.skel.nbone);
}

/* Monster and NPC models nobody uses any more, freed (at each stage load, load_stage_models;
 * em_create_model / npc_create_model load them again when a stage asks for them, as the PS2
 * does). They were cached per kind for the whole run, ~0.5-2 MB a kind with its motion file
 * (docs/xbox.md memory). A monster kind stays while an em_work entry of that kind lives (also
 * on another area: large monsters roam) or a motion set still reads its motion file (the
 * game's motion table keeps a model slot's sets until another kind is created there); the
 * NPC models stay while the village or the town is up or an NPC uses them. */
int rt_motion_refs(const void *p, size_t n);
static void host_models_gc(void)
{
    extern uint8_t em_work[];
    int k, i;
    for (k = 1; k < EM_KINDS; k++) {
        monster *e = &em_mdl[k];
        if (!em_have[k])
            continue;
        for (i = 0; i < 20; i++) {
            const uint8_t *em = em_work + 0xA10 * i;
            if (em[0] && !em[0x1E] && em[2] == k)
                break;
        }
        if (i < 20 || (e->mem[2] && rt_motion_refs(e->mem[2], e->tbl.n)))
            continue;
        if (getenv("RT_QUEST_TRACE"))
            fprintf(stderr, "viewer: monster kind %d model freed\n", k);
        fl_model_release(&e->model);
        fl_skel_release(&e->skel);
        free(e->mem[2]);
        memset(e, 0, sizeof *e);
        em_have[k] = 0;
    }
    if (pl.game && pl.tbl.p && !rt_motion_refs(pl.tbl.p, pl.tbl.n)) {
        /* the hunter's copy of plcom_tbl (hunter_load): in game mode it only gave the first common motion
         * sets (rt_player_motion_start); com_motion_load keeps its own copy (1 MB) */
        if (getenv("RT_QUEST_TRACE"))
            fprintf(stderr, "viewer: the hunter's plcom_tbl copy freed\n");
        fl_skel_drop_motions(&pl.master);
        free(pl.mem[2 * HUNTER_PARTS]);
        pl.mem[2 * HUNTER_PARTS] = NULL;
        memset(&pl.tbl, 0, sizeof pl.tbl);
    }
    if (rt_village_active() || rt_online_town())
        return;
    for (k = 0; k < 4; k++) {
        if (!npc_have[k])
            continue;
        for (i = 0; i < 20; i++) {
            const uint8_t *em = em_work + 0xA10 * i;
            if (em[0] && em[0x1E] && em[0x34F] == k)
                break;
        }
        if (i < 20)
            continue;
        if (getenv("RT_QUEST_TRACE"))
            fprintf(stderr, "viewer: npc model %d freed\n", k);
        fl_model_release(&npc_mdl[k].model);
        fl_skel_release(&npc_mdl[k].skel);
        memset(&npc_mdl[k], 0, sizeof npc_mdl[k]);
        npc_have[k] = 0;
    }
}

/* The NPCs on this stage (em_work slots with +0x1E): their model, posed by
 * the game's motion player, placed and scaled like Lb_npc_mk; villagers
 * (kind 0) show only their own parts (+0x4E6 per part, lb_npc_trans). */
static void npc_draw(const fl_light *L)
{
    extern uint8_t em_work[];
    int i, k;
    (void)L;                    /* each NPC gets its own (light_for) */
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
        if (getenv("RT_EM_DRAW_TRACE") && m->skel.skel.nbone > 1) {
            /* test aid: the NPC's root motion bone in world space next to its game position (as emdraw for monsters) */
            int b, n = 0;
            for (b = 0; b < m->skel.skel.nbone; b++)
                if (m->skel.skel.bone[b].group == 0 && n++ == 1)
                    break;
            if (b < m->skel.skel.nbone) {
                float bp[3] = { m->skel.world[b][12], m->skel.world[b][13], m->skel.world[b][14] };
                printf("npcdraw t%d slot %d kind %d mot %d pos %.1f %.1f local %.1f %.1f %.1f\n", rt_log_tick(), i, kind,
                       ((uint16_t *)(em + 0x2DC))[0], *(float *)(em + 0xAC), *(float *)(em + 0xB4), bp[0], bp[1], bp[2]);
            }
        }
        for (k = 0; k < m->model.npart; k++)    /* villagers: only their own parts are drawn, so only those are skinned */
            m->model.part[k].skip = kind == 0 && (k >= 0x20 || !em[0x4E6 + k]) && !getenv("RT_POSE_ALL");
        fl_model_pose(&m->model, (const flmat *)m->skel.world, light_for(em, 0));
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
            PICK_FN(PK_NPC, i, kind, k, 0, 0, &m->skel, 0);
            rt_clay_attr_set(part_attr(&m->model, k));
            gfx_execute_clay(m->model.part[k].clay);
            rt_clay_attr_reset();
        }
    }
}

/* ------------------------------------------------------------ flFlip inside a tick
 * The PS2 game draws a loading screen and swaps buffers itself (flFlip) before a load
 * that blocks the rest of the tick: game2 step 2 (an area change) the whole-area map
 * (DispWholeMap), game11 (a quest start) Disp_load_start with the quest title, the
 * village Disp_NowLoading before Lb_stage_load. Here the draws since the previous flip
 * of the tick are recorded (gfx_rec.c) and each flip shows them as a frame at once, so
 * the screen holds that picture while the load runs. Empty segments are not shown (the
 * PS2 showed a cleared buffer: after the fade-out the same black). Boot ticks record
 * their draws already (rt_boot.c); a flip there shows the part drawn so far.
 * Headless --shot runs draw no extra frames, unless RT_FLIP_SHOT=prefix is set: then
 * each flip frame is also written to prefix_<tick>_<n>.png (test aid). */
static int in_tick, flip_rec, flip_ticks;
static int quest_dark_tick = -1;    /* the tick a quest started from the village (quest_from_village) */
extern int gfx_rec_on;           /* gfx_rec.c */
static void game_flip(void)
{
    static int n, last_tick = -1;
    const char *fs = getenv("RT_FLIP_SHOT");
    int boot_rec = gfx_rec_on && !flip_rec;
    if (!in_tick || (shot && !fs))
        return;
    if (gfx_rec_on) {
        gfx_rec_end();
        if (gfx_rec_draws() > 0) {
            rt_lt_begin(RTL_FLIP);
            gfx_begin_frame(0);
            gfx_rec_replay();
            if (fs) {
                char name[512];
                uint8_t *rgb = malloc((size_t)W * H * 3);
                if (flip_ticks != last_tick)
                    n = 0;
                last_tick = flip_ticks;
                gfx_read_pixels(rgb);
                snprintf(name, sizeof name, "%s_%d_%d.png", fs, flip_ticks, n++);
                write_png(name, W, H, rgb);
                free(rgb);
            }
            gfx_end_frame();
            rt_lt_end(RTL_FLIP);
        }
    } else
        rt_font_set_draw(1);
    gfx_rec_begin();
    if (!boot_rec)
        flip_rec = 1;
}

#ifdef VERSION_MHG
/* G's loading screen (disp1_nm.c): a request the game sets before a load; the PS2 draws it on each vsync
 * the load waits for (load_bin). Here after each disc read, at most 60 times a second; the first read
 * after the request draws at once. Headless runs show nothing extra (game_flip). */
int Disp_load_req(void);
void Disp_load_req_end(void), Disp_load_screen(void);
static void load_wait(void)
{
    static Uint32 last;
    Uint32 now;
    if (!in_tick || !Disp_load_req()) {
        last = 0;
        return;
    }
    now = SDL_GetTicks();
    if (last && now - last < 17)
        return;
    last = now ? now : 1;
    Disp_load_screen();
}
#endif

/* the end of a tick: what was drawn after the last flip is not shown (the frame
 * the viewer draws next replaces it) */
static void flip_tick_end(void)
{
    in_tick = 0;
#ifdef VERSION_MHG
    Disp_load_req_end();        /* G clears the request after each frame (main 0x12DB20) */
#endif
    if (flip_rec) {
        gfx_rec_end();
        gfx_rec_clear();
        rt_font_set_draw(0);
        flip_rec = 0;
    }
}

/* The quest accepted in the village (game mode 0 -> game1/10/11/12/13 on
 * the PS2): Quest_init + Quest_start (rt_quest_load), the hunt starts on
 * the quest's own start stage (the base camp, game_w.stage), the hunter at
 * its start position (pl_init), the stage's monsters (Quest_em_init_set). */
int rt_tick_count(void);
static void quest_from_village(void)
{
    int st;
    float p[3] = { rx, 0, rz };
    rt_lt_span_begin("quest start");
    rt_monster_clear_all();
    if (rt_quest_load(quest_no) != 0) {
        rt_lt_span_end();
        fprintf(stderr, "quest %d: no mission file\n", quest_no);
        rt_warn("quest %d: no mission file", quest_no);
        return;
    }
    {   /* game11: the loading screen with the quest's title (quest_title, set by Quest_start
         * in rt_quest_load) while the stage loads */
        extern uint8_t game_w[];
#ifdef VERSION_MHG
        void Disp_load_start(int arg, int mode);
        Disp_load_start(*(uint16_t *)(game_w + 0x2C), 0);     /* G: the request (drawn by the loads, load_wait) */
#else
        void Disp_load_start(int arg);
        Disp_load_start(*(uint16_t *)(game_w + 0x2C));
#endif
    }
    quest_dark_tick = flip_ticks;
    st = rt_game_stage();
    if (st != stage_no)
        load_stage_models(st);
    stage_no = st;
    rt_game_init(stage_no);
    rt_hud_init();
    remote_motion_start();
    rt_player_game_init(lp);
    remote_weapons();
    weapon_motions();
    rt_monster_spawn(1, p, 0);
    pad_script_event(PAD_EV_QUEST);
    if (game_cam)
        rt_cam_init(stage_no);
    {   /* game13's last step: the hunt fades in */
        void fade_set(int n);
        fade_set(2);
    }
    rt_lt_span_end();
    if (getenv("RT_QUEST_TRACE"))
        fprintf(stderr, "village: quest %d starts on stage %d (player tick %d)\n", quest_no, stage_no, rt_tick_count());
}

/* Local_main's -1: the village was left another way than for a quest (the bed's "quit the game"). Game_task (main
 * f_game.c, its case -1) clears the world and runs Select_task: the game menu, then NEW GAME / CONTINUE lead back to
 * Game_task and the village (rt_boot.c). Was quest_back (the quest restarted: "quit the game" dropped the hunter at the
 * base camp). */
void rt_world_reset(void);
void rt_monster_clear_all(void);
void all_reset(void);
int rt_import_select(void);
void rt_boot_to_select(void);
static void village_quit(void)
{
    if (getenv("RT_QUEST_TRACE"))
        fprintf(stderr, "village: left without a quest (Local_main -1): to the game menu\n");
    rt_world_reset();
    rt_monster_clear_all();
    rt_import_select();                 /* Load_overlay(1, 1): select.bin's data as on the disc */
    rt_boot_to_select();
    booting = 1;
    rt_hunter_draw_hook = ed_hunter_hook;
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
#ifdef MH1_ONLINE
    if (rt_np_session_end() >= 0)   /* co-op over: the hunter saved, single player again */
        lp = 0;
#endif
#ifdef MH1_ONLINE
    {   /* the network mode (title menu, or --online): the online town (rt_online.c) instead of the village */
        int rt_online_wanted(void), rt_online_active(void), rt_online_tick(void);
        void rt_online_enter(void);
        if (rt_online_wanted()) {
            if (!rt_online_active())
                rt_online_enter();
            q = rt_online_tick();
            {   /* the local hunter is the town's player_work[game_w.master] (the lobby member list's order) */
                extern uint8_t game_w[];
                lp = game_w[GWX(0xD1)] < 8 ? game_w[GWX(0xD1)] : 0;
            }
            if (q > 0) {        /* a matched room: its quest as a co-op session (rt_np.c), the leader hosting */
                int q2 = rt_np_setup(q);
                if (q2 > 0) {
                    lp = rt_np_slot();
                    quest_no = q2;
                    rt_log("online: co-op quest %d, %d players, slot %d", quest_no, rt_np_players(), lp);
                    {   /* the quest's game.bin replaces the lobby overlay: "in the lobby" off (the PS2's game_init
                         * clears game_w). Left on, body_hit read the town's body table (D_610370) for the quest's
                         * monsters and crashed on the first area change (kind 12 on stage 39), and the menus took
                         * their lobby variants. */
                        extern uint8_t game_w[];
                        void com_motion_load(int n), rt_motion_em_release(void);
                        game_w[GWX(0x1DC)] = 0;
                        rt_motion_em_release();     /* the town NPCs' motions go (as rt_village_leave) */
                        com_motion_load(0);         /* and the quest's common motions (plcom_tbl) come back: the town
                                                     * had loaded its own (lbcom_tbl), which lack the carve / gather
                                                     * motions; a carve then ended at once, nothing was carved */
                    }
                    quest_from_village();
                    return;
                }
                rt_log("online: the co-op session did not start");
                lp = 0;
            }
            if (q < 0)
                quest_back();
            return;
        }
    }
#endif
    if (!rt_village_active()) {
        rt_lt_span_begin("village enter");
        rt_village_enter();
        pad_script_event(PAD_EV_VILLAGE);
        q = rt_village_tick();
        rt_lt_span_end();
    } else
        q = rt_village_tick();
    if (q > 0) {
        if (getenv("RT_QUEST_TRACE"))
            fprintf(stderr, "village: quest %d accepted, leaving the village\n", q);
        quest_no = q;
        quest_from_village();
    } else if (q < 0) {
        village_quit();
    }
}

/* ------------------------------------------------------------ the debug menu's actions (dbgmenu.c), between ticks */
void rt_village_leave(void);
void rt_world_reset(void);
void rt_boot_abort(void);
void all_reset(void);
int rt_import_select(void);
void Event_flag_set(int);
void Event_flag_clear(int);
static void dbg_leave_boot(void)    /* from the title / boot screens: the boot's tasks end as when Game_task starts */
{
    if (!booting)
        return;
    rt_boot_abort();
    booting = 0;
}
/* what Local_main does when the hunter leaves the village (src/lobby/f/lb_village_nm.c, its return 1 / -1): the pause
 * menu's prims and state back to the quest's (Pit_reset) and the lobby's three screen prims cleared. The debug menu's
 * jumps out of the village skipped both: the village's menu prims went on into the quest (owner, G on the Xbox, 16 Oct
 * 2026: a half-screen tint and wrong draw order after "start a quest" from the village, RELEASE_G GB5 bug 1) */
void Pit_reset(void);
void Lbc_set_prim();
static void dbg_leave_village(void)
{
    if (!rt_village_active())
        return;
    Pit_reset();
    Lbc_set_prim(0, 0, 0);
    rt_village_leave();
}
static void dbg_start_quest(int no)     /* as from the village counter: Quest_init / Quest_start, the start area */
{
    dbg_leave_boot();
    dbg_leave_village();
    rt_world_reset();
    all_reset();
    rt_log("debug menu: start quest %d", no);
    quest_no = no;
    quest_from_village();
}
static void dbg_village(int intro)      /* game mode 6: the village is entered on the next tick (rt_village_enter) */
{
    dbg_leave_boot();
    dbg_leave_village();
    all_reset();
    if (intro)
        Event_flag_clear(2);            /* the first-visit event not seen yet (RT_VILLAGE_SKIP_INTRO sets it) */
    else
        Event_flag_set(2);
    rt_log("debug menu: to the village (%s the intro)", intro ? "with" : "without");
    rt_flow_set_mode(6);
}
static void dbg_title(void)             /* the power-on sequence again (logos, title), as a reset does */
{
    dbg_leave_village();
    rt_world_reset();
    rt_monster_clear_all();
    all_reset();
    rt_import_select();                 /* select.bin's data as on the disc */
    {   /* a power-on starts with the sound quiet: the village's music (stream 0) and demo stream 1 kept playing over
         * the logos and the title (owner, G on the Xbox, 15 Oct 2026) */
        void str_stop_all(void);
        void se_stop_all(void);
        str_stop_all();
        se_stop_all();
    }
    rt_log("debug menu: back to the title (power-on sequence)");
    rt_boot_init();
    booting = 1;
    rt_hunter_draw_hook = ed_hunter_hook;
}
static void dbg_shot(const char *png)
{
    uint8_t *rgb = malloc((size_t)W * H * 3);
    gfx_read_pixels(rgb);
    write_png(png, W, H, rgb);
    free(rgb);
}

/* RT_MEM="t1,t2,...": the memory report (rt_memstat.c) at those host ticks */
void rt_motion_report(void);
static void mem_tick(int t)
{
    const char *m = getenv("RT_MEM");
    char where[32];
    while (m && *m) {
        if (atoi(m) == t) {
            snprintf(where, sizeof where, "tick %d", t);
            rt_ms_report(where);
            rt_motion_report();
            rt_area_report();
            rt_stack_report(where);
        }
        while (*m && *m != ',')
            m++;
        if (*m)
            m++;
    }
}

static void usage(FILE *f, const char *exe)
{
    fprintf(f, "usage: %s DISC_DIR [--shot out.png] [--frames N] [--time S] "
            "[--size WxH] [--cam x,y,z,yaw,pitch] [--view ex,ey,ez,tx,ty,tz[,fov]] [--stage N] [--quest N] [--play] [--boot] "
            "[--input SCRIPT] [--audio-dump FILE.wav] [--install FILE.iso]\n"
            "  (docs/pc.md lists the RT_* switches; MH1_HIDDEN=1: no window, no install dialog)\n", exe);
}

int main(int argc, char **argv)
{
#ifdef MH1_WIN       /* text-mode stdout / stderr would write CRLF into redirected logs (the tests grep them) */
    _setmode(_fileno(stdout), _O_BINARY);
    _setmode(_fileno(stderr), _O_BINARY);
    {   /* the command line as UTF-8 (the ANSI argv mangles non-ASCII paths, e.g. a dropped ISO) */
        int wn, i8;
        wchar_t **wa = CommandLineToArgvW(GetCommandLineW(), &wn);
        if (wa && wn == argc) {
            char **na = (char **)calloc((size_t)wn + 1, sizeof *na);
            for (i8 = 0; i8 < wn; i8++) {
                int len = WideCharToMultiByte(CP_UTF8, 0, wa[i8], -1, NULL, 0, NULL, NULL);
                na[i8] = (char *)malloc((size_t)len);
                WideCharToMultiByte(CP_UTF8, 0, wa[i8], -1, na[i8], len, NULL, NULL);
            }
            argv = na;
        }
    }
#endif
#ifdef XBOX
    nxMountDrive('E', "\\Device\\Harddisk0\\Partition1\\");
    xbox_env_load();        /* before any switch is read (xbox_env.c) */
    if (getenv("RT_ARGS")) {
        /* test aid (xemu, docs/xbox.md): RT_ARGS="--quest 131 --follow 250,150,-0.35 ..." in env.txt is the command
         * line the Xbox does not have; the run then starts as the PC's (straight into the quest, no power-on boot) */
        static char buf[512];
        static char *av[32];
        char *t;
        int n = 0;
        av[n++] = argv[0];
        snprintf(buf, sizeof buf, "%s", getenv("RT_ARGS"));
        for (t = strtok(buf, " "); t && n < 31; t = strtok(NULL, " "))
            av[n++] = t;
        av[n] = NULL;
        argc = n;
        argv = av;
    }
#endif
    rt_stack_paint();
    for (i = 1; i < argc; i++)      /* --help: the usage on stdout, no SDL, no window, no install dialog */
        if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")) {
            usage(stdout, argv[0]);
            return 0;
        }
    gfx_opts_load(argc, argv);      /* mh1pc.ini (Xbox: settings.ini in the game folder); the display flags below override it */
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--shot") && i + 1 < argc) shot = argv[++i];
        else if (!strcmp(argv[i], "--frames") && i + 1 < argc) frames = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--time") && i + 1 < argc) fixed_time = (float)atof(argv[++i]);
        else if (!strcmp(argv[i], "--size") && i + 1 < argc) { sscanf(argv[++i], "%dx%d", &W, &H); size_given = 1; }
        else if (gfx_opts_arg(argc, argv, &i)) ;       /* --fullscreen, --vsync, --widescreen, ... (docs/pc.md) */
        else if (!strcmp(argv[i], "--cam") && i + 1 < argc)
            cam_given = sscanf(argv[++i], "%f,%f,%f,%f,%f", &cam[0], &cam[1], &cam[2], &cam[3], &cam[4]) > 0;
        else if (!strcmp(argv[i], "--view") && i + 1 < argc) {
            /* --view ex,ey,ez,tx,ty,tz[,fov]: the free camera from an eye, a target and an angle of view (rad), e.g. a
             * PS2 game camera's lpView read in PCSX2 (tools/g_ref_shot.sh) */
            float v[7] = { 0, 0, 0, 0, 0, -1, 1.0f }, d[3];
            if (sscanf(argv[++i], "%f,%f,%f,%f,%f,%f,%f", &v[0], &v[1], &v[2], &v[3], &v[4], &v[5], &v[6]) >= 6) {
                d[0] = v[3] - v[0]; d[1] = v[4] - v[1]; d[2] = v[5] - v[2];
                cam[0] = v[0]; cam[1] = v[1]; cam[2] = v[2];
                cam[3] = atan2f(-d[0], -d[2]);
                cam[4] = atan2f(d[1], sqrtf(d[0] * d[0] + d[2] * d[2]));
                view_fov = v[6];
                cam_given = 1;
            }
        }
        else if (!strcmp(argv[i], "--stage") && i + 1 < argc) { stage_no = (int)strtol(argv[++i], NULL, 0); stage_given = 1; }
        else if (!strcmp(argv[i], "--quest") && i + 1 < argc) quest_no = (int)strtol(argv[++i], NULL, 0);
        else if (!strcmp(argv[i], "--play")) play = 1;
        else if (!strcmp(argv[i], "--debug-menu")) dbg_arg = 1;          /* the testers' debug menu on / off (dbgmenu.c) */
        else if (!strcmp(argv[i], "--no-debug-menu")) dbg_arg = 2;
        else if (!strcmp(argv[i], "--boot")) { boot = 1; play = 1; }
        else if (!strcmp(argv[i], "--input") && i + 1 < argc) {
            script = argv[++i];
            play = 1;
            if (script[0] == '@') {         /* --input @file: the script is in a file (the bug reporter's input.txt) */
                char *b = script_file_read(script + 1);
                if (b)
                    script = b;
                else
                    fprintf(stderr, "cannot read the input script %s\n", script + 1);
            }
        }
        else if (!strcmp(argv[i], "--sw-trace")) sw_trace = 1;
#ifdef MH1_ONLINE
        else if (!strcmp(argv[i], "--nettest") && i + 1 < argc) nettest = argv[++i];
        else if (!strcmp(argv[i], "--online")) { void rt_online_request(void); rt_online_request(); }   /* the online town */
        else if (rt_np_arg(argc, argv, &i)) ;      /* co-op: --host [IP] / --join IP, --port, --players */
#endif
        else if (!strcmp(argv[i], "--audio-dump") && i + 1 < argc) audio_dump = argv[++i];
        else if (!strcmp(argv[i], "--mute")) mute = 1;
        else if (!strcmp(argv[i], "--follow") && i + 1 < argc)
            follow_given = sscanf(argv[++i], "%f,%f,%f", &follow[0], &follow[1], &follow[2]) > 0;
        else if (!strcmp(argv[i], "--install") && i + 1 < argc) { install_iso = argv[++i]; install_only = 1; }
        else if (!strcmp(argv[i], "--install-dir") && i + 1 < argc) install_dir = argv[++i];
#ifndef XBOX
        else if ((!strcmp(argv[i], "--import-save") || !strcmp(argv[i], "--export-save")) && i + 1 < argc) i++;   /* rt_save_cli below */
        else if (argv[i][0] != '-' && ndrop_save < 8 && rt_save_looks_like(argv[i])) drop_save[ndrop_save++] = argv[i];   /* a PS2 save dropped on the exe */
#endif
        else if (argv[i][0] != '-') disc = argv[i];
    }
#if defined(VERSION_MHG) && !defined(XBOX)
    /* G (docs/mhg.md section 5) is a work in progress run by agents: every run is hidden (no window, no real
     * controller, no install dialog) unless MH1_SHOW=1 is set by hand; the screenshot ends the run */
    if (!getenv("MH1_SHOW") && !shot) {
        fprintf(stderr, "G runs are hidden: give --shot FILE --time S (with --input SCRIPT for pad input); MH1_SHOW=1 shows a window\n");
        return 1;
    }
#endif
#ifdef XBOX
    /* Xbox (tools/build_xbox.py): no command line. Boot the game from
     * power-on with the player's own disc files next to the XBE (D:\data)
     * or on the hard disk (E:\Games\MH1\data, docs/xbox.md). */
    if (!disc) {
        /* the version's disc files (xbox_title.h): MH1 in E:\Games\MH1\data, G in E:\Games\MHG\data. D:\data
         * (next to the XBE: xemu's data ISO, or the network loader's E:\Games\MH1) only when it holds this
         * version's program file, so a G build started by the MH1 loader does not take MH1's files. */
        static const char *dirs[] = { "D:\\data", XBOX_GAME_DIR "\\data" };
#ifdef VERSION_MHG
        static const struct { const char *n; long sz; } need[] = {      /* SLPM-65869 */
            { "AFS_DATA.AFS", 150759424L }, { "AFS00.AFS", 926939136L },
            { "AFS01.AFS", 25186304L }, { "SLPM_658.69", 2029336L } };
        static const char disc_name[] = "Monster Hunter G";
#else
        static const struct { const char *n; long sz; } need[] = {
            { "AFS_DATA.AFS", 121866240L }, { "AFS00.AFS", 775729152L },
            { "AFS01.AFS", 20463616L }, { "SLPM_654.95", 5648908L } };
        static const char disc_name[] = "Monster Hunter";
#endif
        FILE *t;
        int d;
        for (d = 0; d < 2 && !disc; d++) {
            snprintf(path, sizeof path, "%s\\%s", dirs[d], need[3].n);
            if ((t = fopen(path, "rb")) != NULL) {
                fclose(t);
                disc = dirs[d];
            }
        }
        xbox_log_start();
        {
            char m[160];
            snprintf(m, sizeof m, "%s", disc ? disc : "");
            if (!disc)
                snprintf(m, sizeof m, "no %s in D:\\data or %s\\data", need[3].n, XBOX_GAME_DIR);
            xbox_log_note(m);
        }
        {   /* the player's disc files must be complete: a missing or cut-short copy (FTP stopped early) gets a
             * plain message instead of a crash later (sizes of the Japanese disc's files, docs/PLAYING.md) */
            int k, bad = 0;
            for (k = 0; k < 4 && disc; k++) {
                long sz = -1;
                FILE *q;
                snprintf(path, sizeof path, "%s\\%s", disc, need[k].n);
                if ((q = fopen(path, "rb")) != NULL) {
                    if (fseek(q, 0, SEEK_END) == 0)
                        sz = ftell(q);
                    fclose(q);
                }
                if (sz != need[k].sz) {
                    char m[160];
                    snprintf(m, sizeof m, "DATA PROBLEM: %s is %s", need[k].n, sz < 0 ? "missing" : "the wrong size (copy not finished?)");
                    xbox_log_note(m);
                    bad = 1;
                }
            }
            if (!disc || bad) {
                char m[160];
                snprintf(m, sizeof m, "The game needs the files of YOUR Japanese %s PS2 disc in", disc_name);
                xbox_log_note(m);
                snprintf(m, sizeof m, "%s\\data : %s %s %s %s", XBOX_GAME_DIR, need[0].n, need[1].n, need[2].n, need[3].n);
                xbox_log_note(m);
                xbox_log_note("(see docs/PLAYING.md). Copy them with FTP, then start again.");
                exit(1);
            }
        }
        if (!getenv("RT_ARGS"))
            boot = 1;       /* RT_ARGS: as on the PC, --boot only when given */
        play = 1;
        /* test runs (xemu, tools/run_xemu.sh --input/--env): a pad script in the data folder,
         * the same format as the PC's --input; env.txt (loaded at the start of main) set RT_* test aids */
        if (disc) {
            xbox_env_note();
            snprintf(path, sizeof path, "%s\\input.txt", disc);
            script = script_file_read(path);
            if (!script && strcmp(disc, "D:\\data"))      /* the loader's test script (E:\Games\MH1\data) */
                script = script_file_read("D:\\data\\input.txt");
            if (script)
                xbox_log_note("input.txt: scripted pad input");
        }
    }
#endif
#ifndef XBOX
    /* PS2 save import / export (docs/pc.md "Importing a PS2 save"): --import-save F, --export-save F, or save files
     * dropped on the exe. No game data is needed for these. */
    {
        int rc = rt_save_cli(rt_mc_root(), argc, argv), k;
        if (rc >= 0)
            return rc;
        for (k = 0; k < ndrop_save; k++) {
            char msg[2000];
            rc = rt_save_import(rt_mc_root(), drop_save[k], msg, sizeof msg);
            save_note(rc == 0, msg);
            if (rc != 0)
                return 1;
        }
        if (ndrop_save && !disc && !install_iso && !script && !quest_no && !play && !shot) {   /* a save dropped on the exe: start the game */
            boot = 1;
            play = 1;
        }
    }
    /* The game's files come from the player's own ISO, installed once into a data folder:
     * an .iso given as the disc (drag and drop onto the exe, play.bat) or --install FILE.iso, or no
     * data found anywhere (then ask for the ISO). docs/pc.md "Installing from an ISO". */
    {
        static char found[1024], dest[1024];
        const char *iso = install_iso;
        int asked = 0;
        if (!iso && disc && install_is_iso_name(disc))
            iso = disc;
        if (!iso && (!disc || (disc && !install_has_data(disc))) && !shot) {
            if (install_find_data(found, sizeof found))
                disc = found;
            else if (!disc && getenv("MH1_HIDDEN") && !getenv("MH1_SHOW")) {
                /* a hidden run (agents, tests) never opens the install dialog on the desktop */
                fprintf(stderr, "no game data found and no DISC_DIR given (MH1_HIDDEN is set: no install dialog)\n");
                return 1;
            } else if (!disc) {
                char got[1024];
                if (install_prompt(got, sizeof got) == 0) {
                    snprintf(found, sizeof found, "%s", got);
                    iso = found;
                    asked = 1;
                }
            }
        }
        rt_log_init(disc ? disc : iso, argc, argv);
        if (iso) {
            static char final[1024];
            if (install_dir)
                snprintf(dest, sizeof dest, "%s", install_dir);
            else
                install_default_dir(dest, sizeof dest);
            if (install_from_iso(iso, dest, !getenv("RT_NO_GUI") && !(getenv("MH1_HIDDEN") && !getenv("MH1_SHOW"))) != 0)
                return 1;
            snprintf(final, sizeof final, "%s", dest);
            disc = final;
            if (install_only)
                return 0;
            if (!script && !quest_no && !play && !shot) {   /* dropped on the exe: start the game */
                boot = 1;
                play = 1;
            }
            (void)asked;
        }
    }
#endif
    rt_pick_set_args(argc, argv);
    {   /* the settings that are read once: the translation table (before the game data is imported) and the button layout */
        extern int rt_pad_swap_confirm;
        gfx_opts_apply_language();
        rt_pad_swap_confirm = pc_opt.western_pad;
        menu_set_shot(shot != NULL);
    }
    if (!disc) {
        usage(stderr, argv[0]);
        return 1;
    }
    if (stage_given && (stage_no < 0 || stage_no >= 88)) {
        /* both games have stages 0-87 (stage_start_pos[88], the per-stage file tables); G's
         * stage_hit_data_w holds 0 past 87, which loaded file 0 as a collision link and crashed */
        fprintf(stderr, "--stage %d: no such stage (0-87)\n", stage_no);
        return 1;
    }
    if (!rt_log_started())
        rt_log_init(disc, argc, argv);
    snprintf(path, sizeof path, "%s/AFS_DATA.AFS", disc);
    if (fmt_afs_open(&afs, path) != 0) {
        fprintf(stderr, "cannot open %s\n", path);
        rt_warn("cannot open %s: wrong disc directory? (it must hold AFS_DATA.AFS and SLPM_654.95)", rt_log_path(path));
        return 1;
    }
    rt_log("boot: AFS_DATA.AFS opened (%u entries)", (unsigned)afs.count);
    snprintf(path, sizeof path, "%s/%s", disc, rt_ver_elf());     /* SLPM_654.95 (G: SLPM_658.69) */
    if (rt_load_elf(path) != 0) {
        fprintf(stderr, "cannot read %s: no game data tables\n", path);
        rt_warn("cannot read %s: no game data tables", rt_log_path(path));
    } else
        rt_log("boot: %s loaded", rt_ver_elf());
    {
        const char *mso = rt_ms_push("overlay binaries (game/lobby/select.bin)");
        /* stored raw in MH1, packed in G (fmt_mwo_unpack) */
        uint8_t *ovl = fmt_afs_read(&afs, fmt_afs_find(&afs, "game.bin"), &n);
        ovl = fmt_mwo_unpack(ovl, ovl ? n : 0, &n);
        rt_set_overlay(ovl, ovl ? n : 0);
        ovl = fmt_afs_read(&afs, fmt_afs_find(&afs, "lobby.bin"), &n);           /* the village overlay */
        ovl = fmt_mwo_unpack(ovl, ovl ? n : 0, &n);
        rt_set_lobby(ovl, ovl ? n : 0);
        ovl = fmt_afs_read(&afs, fmt_afs_find(&afs, "select.bin"), &n);          /* the boot overlay (title, new hunter, load) */
        ovl = fmt_mwo_unpack(ovl, ovl ? n : 0, &n);
        rt_set_select(ovl, ovl ? n : 0);
        if (rt_ver_mhg()) {     /* G: sub_main.bin, resident after main's .bss */
            ovl = fmt_afs_read(&afs, fmt_afs_find(&afs, "sub_main.bin"), &n);
            ovl = fmt_mwo_unpack(ovl, ovl ? n : 0, &n);
            rt_set_sub_main(ovl, ovl ? n : 0);
        }
        rt_ms_pop(mso);
    }
    if (rt_import_data() != 0) {
        fprintf(stderr, "some game data tables are missing\n");
        rt_warn("some game data tables are missing (main / game.bin)");
    }
    if (rt_import_lobby() != 0) {
        fprintf(stderr, "some lobby data tables are missing\n");
        rt_warn("some lobby data tables are missing (lobby.bin)");
    }
    if (rt_import_select() != 0) {
        fprintf(stderr, "select.bin is missing: no title screen\n");
        rt_warn("select.bin is missing: no title screen");
    }
    rt_text_finish();       /* translation table (RT_TEXT_TABLE), if any */
    rt_log("boot: game data tables imported");
#ifdef MH1_ONLINE
    if (nettest)        /* headless online test (src/pc/rt/rt_net.c, docs/network.md) */
        return rt_net_test(nettest);
    if (rt_np_wanted()) {   /* co-op (rt_np.c): the host waits for the others and names the quest */
        int q = rt_np_setup(quest_no);
        if (q <= 0)
            return 1;
        quest_no = q;
        play = 1;
        lp = rt_np_slot();
        rt_log("co-op: quest %d, %d players, slot %d", quest_no, rt_np_players(), lp);
    }
#endif
    if (!getenv("RT_NO_TRIM"))
        rt_mem_trim();
    if (boot && !quest_no)
        quest_no = 10;      /* the set-up below as for a quest; the boot ends in the village (game mode 6) */
#ifndef XBOX
    if (!shot)
        gfx_opts_finish(size_given, &W, &H);        /* the remembered window, else 960x720 (4:3) / 1280x720 (widescreen) */
#endif
    /* scripted runs (--input: tests, bug-report replays) never show a window on the desktop;
     * MH1_SHOW=1 shows one anyway for debugging by hand */
    int hide = shot != NULL || ((script != NULL || getenv("MH1_HIDDEN")) && !getenv("MH1_SHOW"));
#if defined(VERSION_MHG) && !defined(XBOX)
    if (!getenv("MH1_SHOW"))
        hide = 1;       /* G on the PC: always hidden (above). Not on the Xbox: the console is played by
                         * hand, and hide also turned off the controller there (owner, 14 Oct) */
#endif
    if (gfx_init(W, H, "MH1 PC viewer", hide) != 0) {
        rt_warn("graphics start-up failed (no window / OpenGL context)");
        return 1;
    }
    gfx_size(&W, &H);
    rt_log("boot: window %dx%d%s", W, H, shot ? " (hidden, screenshot run)" : hide ? " (hidden, scripted run)" : "");
    { extern void rt_seed_random(int); rt_seed_random(script != NULL); }
    { void rt_ref_init(void); rt_ref_init(); }     /* the lockstep referee (RT_REF_*, rt_ref.c) */
    if (script && !pad_script_set(script)) {
        fprintf(stderr, "bad --input script\n");
        rt_warn("bad --input script");
        return 1;
    }
    pad_set_headless(hide || script != NULL || audio_dump != NULL);
    if (play)
        pad_init();

    rt_set_file_loader(afs_entry);
    rt_set_em_model_loader(em_model_load);  /* before the quest's em_create_model calls */
    rt_set_em_pose_fn(em_pose_now);         /* enemy_mk's joints for em_ride_sub (rt_em.c) */
    if (quest_no) {
        /* --quest N: Quest_init + Quest_start as game11 does. The hunt
         * starts where the game starts it: the quest's start stage
         * (game_w.stage, the base camp), as a quest accepted in the
         * village does. Test aid RT_QUEST_STAGE=1: start on the stage of
         * the quest's own monster instead (the old scripted-test set-up);
         * --stage N overrides both. */
        int k = -1, st;
        { void rt_ref_user_data(void); rt_ref_user_data(); }   /* RT_REF_UD: the PS2's User_data */
        if (rt_ver_mhg() && boot) {
            /* G stand-in (docs/mhg.md): the boot's set-up quest is not loaded; G's Quest_start
             * (sub_main 0x4B3AA0, edited, twice MH1's size) has no G C yet */
            rt_log_standin("G boot: set-up quest 10 (Quest_start)");
            rt_quest_free_hunt();
        } else if (rt_quest_load(quest_no) != 0)
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
    if (!boot) {                        /* --quest / free play: the options' defaults, as the boot's
                                         * InitSystemData sets them (sound volumes come from system_w) */
        void option_default_set(void);
        option_default_set();
    }
    if (!mute)
        snd = rt_snd_init(disc, audio_dump == NULL && shot == NULL);

    {   /* the free-play Rathian (kind 1, em_work[0], model slot 0; spawned below): its model, and the motions
         * create_em_motion builds, started on the walk loop */
        monster *e = em_kind_model(1);
        static const int ids[3] = { 1003, 1203, 1403 };   /* slot 3 of banks 0/2/4 */
        if (!e || !e->tbl.p)
            fprintf(stderr, "em01 load failed\n");
        else
            rt_monster_motion_start(0, 0, e->tbl.p, 1, ids, 3);
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

    if (getenv("RT_EM_POS"))            /* test placement of the Rathian: "x,z" */
        sscanf(getenv("RT_EM_POS"), "%f,%f", &rx, &rz);
    gy = 0;
    rt_ground_y(rx, rz, 1e6f, &gy);
    {   /* em_work[0] on the stage (a quest: its own monsters): the game's monster code (enemy_mv, rt_em.c) moves it
         * by its root motion, em_move's wall/ground collision keeps it on the ground and inside the walls */
        float p[3] = { rx, gy, rz };
        if (getenv("RT_EM_KIND"))       /* test aid: another kind, with its own model */
            em_model_load(0, atoi(getenv("RT_EM_KIND")));
        if (!(rt_ver_mhg() && boot))    /* G: no set-up quest at power-on (see above) */
            rt_monster_spawn(1, p, (int)(0.6f * 65536.0f / 6.2831853f));
    }
    /* the hunter on the ground: posed at frame 0, the lowest vertex on the collision floor */
    hunter_pose(&pl, 0, light_cur());
    {
        float lo = 1e30f;
        int s;
        for (s = 0; s < HUNTER_PARTS; s++) {
            float y = fl_model_min_y(&pl.part[s], s ? (const flmat *)pl.pw[s] : (const flmat *)pl.master.world);
            if (y < lo)
                lo = y;
        }
        gy = 0;
        rt_ground_y(hx, hz, 1e6f, &gy);
        place(pl.world, hx, gy - lo, hz, 2.6f);
        {   /* the hunter is the master player (player_work[0]) for the game C */
            float p[3] = { hx, gy, hz };
            rt_set_player(lp, p);
            rt_debug_spawn(p);          /* RT_SPAWN test effects at the hunter */
            if (pl.tbl.p) {     /* animate with the game's frame_init/frame_move */
                rt_player_motion_start(lp, pl.tbl.p, 1, 101);
                pl.game = 1;
                pl.no = lp;
                pl.master.root_lock = 1;    /* the game moves the actor by the root motion */
                rt_player_set_ang(lp, (int)(2.6f * 65536.0f / 6.2831853f));
                if (play) {
                    remote_motion_start();
                    rt_player_game_init(lp);    /* the game's pl_init: start position, idle */
                    remote_weapons();
                    weapon_motions();
                    weapon_load(&weapon, lp);
                    weapon_have = rt_player_weapon_model(lp);
                }
            }
            hunter_yoff = -lo;
            if (play && pl.game) {
                rt_cam_init(stage_no);  /* the game camera follows player_work[0] (game2 moves it on stage changes) */
                game_cam = !follow_given && !getenv("RT_HOST_CAM");
            }
        }
    }

    /* packs + stage stream; the Rathian (kind 1) is the stage's monster. Not with --boot: the quest set-up above is only
     * the host's scaffolding there (stage 21, the Forest and Hills camp), and its stream (M2_CAMP1) played under the
     * boot logos. On the PS2 the logos are silent and the opening movie brings its own sound; the title and name entry
     * load their packs themselves (Menu_snd_load, edit_se_load) and the village's stage loads bring the rest */
    if (snd == 0 && !boot) {
        static const int em_kinds[1] = { 1 };
        rt_snd_stage(stage_no, em_kinds, 1);
    }
#ifndef XBOX                            /* no mouse on the Xbox: nxdk's SDL asserts in relative mode */
    if (!shot)
        SDL_SetRelativeMouseMode(SDL_TRUE);
#endif
    rt_flow_set_core(sim_tick);
    rt_set_stage_loader(load_stage_models);
    rt_flow_set_back(quest_back);
    rt_flow_set_village(village_step);
    {
        void rt_set_flip(void (*fn)(void));
        rt_set_flip(game_flip);
#ifdef VERSION_MHG
        rt_set_load_wait(load_wait);
#endif
    }
    rt_set_npc_model_loader(npc_model_load);
    {
        void rt_set_text_input(void (*begin)(int), int (*take)(char *, int));
        rt_set_text_input(pad_text_mode, pad_text_take);
    }
    {   /* the testers' debug menu (dbgmenu.c): on in development builds, off in a release build (MH1_RELEASE); the ini key
         * debug_menu, --debug-menu / --no-debug-menu or RT_DEBUG_MENU=1/0 decide over that */
        static const dbgmenu_actions acts = { dbg_start_quest, dbg_village, dbg_title, dbg_shot };
#ifdef MH1_RELEASE
        int on = 0;
#else
        int on = 1;
#endif
        if (pc_opt.debug_menu)          /* the settings file (the settings menu's row) */
            on = pc_opt.debug_menu == 1;
        if (dbg_arg)
            on = dbg_arg == 1;
        if (getenv("RT_DEBUG_MENU"))
            on = atoi(getenv("RT_DEBUG_MENU")) != 0;
        dbgmenu_init(on && play, &acts);
    }
    if (quest_no && getenv("RT_VILLAGE_START"))   /* test aid: straight to the village (game mode 6) */
        rt_flow_set_mode(6);
    if (boot) {         /* power-on: the game's boot tasks until Game_task (rt_boot.c) */
        rt_boot_init();
        booting = 1;
        rt_log("boot: running the game from power-on (--boot)");
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
    rt_lt_init();
    while (running) {
        SDL_Event ev;
        /* RT_STEP=1 with --time S: one game tick per drawn frame up to S
         * (a frame per tick, as at 30 fps: for RT_PROF's draw numbers) */
        static int step = -1;
        float t;
        if (step < 0)
            step = getenv("RT_STEP") != NULL && fixed_time >= 0;
        t = fixed_time >= 0 ? fixed_time : (SDL_GetTicks() - t0) / 1000.0f;
        if (step && frame_no / 30.0f < fixed_time)
            t = frame_no / 30.0f;
        float fr = t * 30.0f;                      /* game motions run at 30 fps */
        flmat proj, camw, view;
        if (!shot)
            gfx_size(&W, &H);                       /* the window may have been resized / gone fullscreen */
        const Uint8 *keys;
        float spd = 40;

        while (SDL_PollEvent(&ev)) {
#ifndef XBOX
            if (ev.type == SDL_KEYDOWN && !ev.key.repeat &&
                ((ev.key.keysym.sym == SDLK_RETURN && (ev.key.keysym.mod & KMOD_ALT)) || ev.key.keysym.sym == SDLK_F11)) {
                gfx_toggle_fullscreen();                    /* Alt+Enter / F11: never reaches the game's pad */
                continue;
            }
#endif
            if (menu_event(&ev))        /* F10: the settings menu (open: it takes the keys and the mouse) */
                continue;
            if (dbgmenu_event(&ev))     /* F9: the debug menu (open: it takes the keys and the mouse) */
                continue;
            if (pick_event(&ev))        /* F8: the bug reporter (frozen: it takes all input) */
                continue;
            pad_event(&ev);     /* typed text (the name entry) */
            if (ev.type == SDL_QUIT || (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE))
                running = 0;
#ifndef XBOX
            else if (ev.type == SDL_DROPFILE) {     /* a PS2 save dropped on the window: import it (the game reads the card at CONTINUE) */
                if (rt_save_looks_like(ev.drop.file)) {
                    char msg[2200];
                    Uint32 b = SDL_GetTicks();
                    int rc = rt_save_import(rt_mc_root(), ev.drop.file, msg, sizeof msg);
                    if (rc == 0)
                        strcat(msg, "\n\nChoose CONTINUE on the title screen to load it. If you are already in the game, quit and start it again first: saving now would replace the imported save.");
                    save_note(rc == 0, msg);
                    t0 += SDL_GetTicks() - b;       /* the message box stopped the clock */
                }
                SDL_free(ev.drop.file);
            }
#endif
            else if (ev.type == SDL_MOUSEMOTION && !shot) {
                cam[3] -= ev.motion.xrel * 0.003f;
                cam[4] -= ev.motion.yrel * 0.003f;
                if (cam[4] > 1.5f) cam[4] = 1.5f;
                if (cam[4] < -1.5f) cam[4] = -1.5f;
            }
        }
        if (play)
            pick_poll_pad();            /* Back/View + Start on the controller */
        if (script && menu_open()) {   /* the settings menu open in a scripted run: the script drives it, a step a frame */
            pad_state mp;
            script_live = pad_script_next(&mp);
            menu_pad(&mp);
        } else if (!shot)
            menu_poll_pad();            /* Back + L3 opens the settings menu; its controller navigation */
        if (play && dbgmenu_wants_pad()) {      /* the debug menu open (or frame step): it reads the pad once a frame, the script too */
            pad_state dp;
            if (script)
                script_live = pad_script_next(&dp);
            else
                pad_read(&dp, 0);
            dbgmenu_pad(&dp);
        }
        if (play && pick_wants_pad()) {         /* the bug reporter frozen with the controller: pointer, A, note, the script too */
            pad_state pp;
            if (script)
                script_live = pad_script_next(&pp);
            else
                pad_read(&pp, 0);
            pick_pad(&pp);
        }
        {   /* the bug reporter stops the game clock while it is frozen */
            static int holding;
            static Uint32 hold_t0;
            int hnow = pick_hold_ticks(ticks) | menu_hold_ticks(ticks) | dbgmenu_hold_ticks() | test_hold_ticks(ticks);
            if (hnow && !holding)
                hold_t0 = SDL_GetTicks();
            if (!hnow && holding && fixed_time < 0) {
                t0 += SDL_GetTicks() - hold_t0;
                t = (SDL_GetTicks() - t0) / 1000.0f;
                fr = t * 30.0f;
            }
            holding = hnow;
        }
        {   /* the debug menu's free camera (PC): starts at the game camera's eye, looking where it looked */
            static int fc_prev;
            if (dbgmenu_freecam() && !fc_prev && have_view) {
                float d[3] = { gc_tar[0] - gc_eye[0], gc_tar[1] - gc_eye[1], gc_tar[2] - gc_eye[2] };
                cam[0] = gc_eye[0];
                cam[1] = gc_eye[1];
                cam[2] = gc_eye[2];
                cam[3] = atan2f(-d[0], -d[2]);
                cam[4] = atan2f(d[1], sqrtf(d[0] * d[0] + d[2] * d[2]));
            }
            fc_prev = dbgmenu_freecam();
        }
        if (game_cam && have_view && !dbgmenu_freecam()) {    /* look-at from the game camera's eye/target (roll ignored) */
            lookat_world(camw, gc_eye, gc_tar);
        } else {   /* camera: rotate pitch then yaw, looking down -Z like GL */
            float s[3] = { 1, 1, 1 }, r[3], tr[3];
            r[0] = cam[4]; r[1] = cam[3]; r[2] = 0;
            tr[0] = cam[0]; tr[1] = cam[1]; tr[2] = cam[2];
            flmat_srt(camw, s, r, tr);
        }
        keys = SDL_GetKeyboardState(NULL);
        if (!shot && (!play || dbgmenu_freecam())) {
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
        flmat_perspective(proj, game_cam && have_view ? gc_fov : view_fov, scene_aspect(), 10.0f, 80000.0f);

        /* game logic ticks at 30 per second (at least 2, so set objects
         * have run their init and queued their prims) */
        if (shot && shot_next > 2 + (int)(fixed_time >= 0 ? fixed_time * 30.0f : fr))
            shot_next = 0;              /* RT_SHOTS past --time: dropped (with RT_STEP, fr is still behind --time) */
        /* real time: at most CATCH_UP ticks behind the clock. When ticks cost more than their
         * 33 ms (xemu on a busy host: the opening movie's decode), catching up every lost tick
         * means a frame only every few seconds and never recovering; the game slows down
         * instead, as on the PS2. Load pauses no longer come back as a burst of ticks either. */
        if (fixed_time < 0 && 2 + (int)fr - ticks > CATCH_UP
            && !pick_hold_ticks(ticks) && !menu_hold_ticks(ticks) && !dbgmenu_hold_ticks() && !test_hold_ticks(ticks)) {     /* (a hold stops the clock itself) */
            static Uint32 told, lost_sum;
            Uint32 lost = (Uint32)((2 + (int)fr - ticks - CATCH_UP) * 1000 / 30);
            t0 += lost;
            t = (SDL_GetTicks() - t0) / 1000.0f;
            fr = t * 30.0f;
            lost_sum += lost;
            if (lost_sum > 1000 && SDL_GetTicks() - told > 10000) {
                told = SDL_GetTicks();
                rt_log("slow: game time fell %u ms behind real time by tick %d (the game slowed down)", (unsigned)lost_sum, ticks);
                lost_sum = 0;
            }
        }
        /* RT_PROF=1: CPU time per subsystem (rt_prof.c), per game tick and
         * per drawn frame, every 300 ticks */
        while (!pick_hold_ticks(ticks) && !menu_hold_ticks(ticks) && !dbgmenu_hold_ticks() && !test_hold_ticks(ticks) && ticks < 2 + (int)fr && !(shot_next > 0 && ticks >= shot_next)) {
            rt_prof_begin(RTP_LOGIC);
            in_tick = 1;                /* flFlip shows frames from here (game_flip) */
            flip_ticks = ticks;
            rt_actor_nodes_fill();      /* every actor's node array as the last tick left it (boot screens, village, quests) */
            if (booting) {      /* ACRMain: pad, then the task scheduler */
                pad_state ps;
                read_pad(&ps);
                rt_pad_set(ps.bits, ps.lx, ps.ly, ps.rx, ps.ry);
                rt_pad_tick();
                if (rt_boot_tick()) {
                    booting = 0;
                    rt_log("boot: finished after %d ticks; game mode 6 (village)", ticks);
                    rt_flow_set_mode(6);    /* Game_task offline: the village */
                }
                if (snd == 0) {
                    rt_snd_tick();
                    audio_dump_tick();
                }
                ticks++;
                mem_tick(ticks);
                flip_tick_end();
                rt_prof_end(RTP_LOGIC);
                rt_prof_tick();
                continue;
            }
            if (quest_no) {
                { void rt_ref_tick(int); rt_ref_tick(ticks); }      /* the referee's record of the tick's start */
                /* outside game2 the host tick (sim_tick) does not run: the
                 * pad is still read every tick (result / reward screens) */
                extern unsigned char game_w[];
                if (rt_flow_mode() != 2 && play && !(game_w[0] == 1 && game_w[1] == 3)) {    /* (not while game13 waits: its tick
                                                                                             * reads the pad in sim_tick) */
                    pad_state ps;
                    read_pad(&ps);
                    rt_pad_set(ps.bits, ps.lx, ps.ly, ps.rx, ps.ry);
                    if (rt_flow_mode() == 6)
                        rt_pad_read();      /* the village's Lb_pl_move runs swset */
                    else
                        rt_pad_tick();
                }
                { void rt_sys_frame_start(void); rt_sys_frame_start(); }  /* the Scheduler's ran_suu(1) (rt_boot.c) */
                rt_flow_tick();         /* game2 / game3 / game5 (f_game.c): game_core = sim_tick */
                rt_sys_tick();          /* Fade_task (the scheduler's system tasks) */
            }
            else
                sim_tick();
            if (snd == 0) {             /* Snd_server: ACRMain runs it every frame, in every game mode
                                         * (the village had no music on the PC without it) */
                rt_snd_tick();
                audio_dump_tick();
            }
            ticks++;
            rt_light_tick();
                mem_tick(ticks);
            /* the joint matrices the next tick reads are those of the state
             * this tick left, whether or not a frame is drawn in between
             * (windowed and --shot runs stay tick-for-tick the same) */
            if (pl.game && play && ticks >= 2) {
                rt_prof_begin(RTP_JOINTS);
                sync_joints(&pl, hunter_yoff);
                if (!rt_village_active() || rt_online_town())
                    remote_hunters(0, light_cur());
                if (!rt_village_active())
                    monsters_sync(0, light_cur());
                rt_actor_nodes_fill();
                rt_prof_end(RTP_JOINTS);
            }
            if (tick_trace) {           /* RT_TICK_TRACE=1: compare windowed and headless runs */
                extern uint8_t em_work[];
                float p[3], es = 0;
                int a, k;
                rt_player_get(lp, p, &a);
                for (k = 0; k < 20; k++)
                    es += ((float *)(em_work + 0xA10 * k + 0xAC))[0] + ((float *)(em_work + 0xA10 * k + 0xAC))[2];
                fprintf(stderr, "T %d m%d st%d pl %.2f %.2f %.2f %04X em %.2f\n", ticks, rt_flow_mode(),
                        rt_game_stage(), p[0], p[1], p[2], a & 0xFFFF, es);
            }
            flip_tick_end();
            rt_prof_end(RTP_LOGIC);
            rt_prof_tick();
        }
        rt_prof_begin(RTP_DRAW);
        if (pick_frozen()) {            /* the bug reporter: the frozen frame, no scene */
            pick_frozen_frame();
            goto frame_done;
        }
    redraw:                             /* (the id pass draws the frame once more) */
        if (booting) {          /* the boot screens: the last tick's picture */
            gfx_begin_frame(0);
            rt_boot_draw();
            goto frame_done;
        }
        if (quest_no && rt_flow_mode() == 2) {
            /* game2 steps 3-6 (an area change, after the map and st_model_load): the PS2 game
             * returns before game_core, so nothing is drawn and flFlip shows a cleared (black)
             * buffer until step 6 (pl_init, fade_set(2)) fades the new area in. Drawing the
             * scene here showed the new stage from the old position, without fade, for a frame
             * (a long one on the Xbox, whose sound loads span ticks). */
            extern uint8_t game_w[];
            if (game_w[1] >= 3 && game_w[1] <= 6) {
                gfx_begin_frame(0);
                goto frame_done;
            }
        }
        if (rt_flow_mode() == 6 && !rt_village_active() && !rt_online_town()) {
            /* the quest is over (game5's last step, a retire, the debug menu's village row) and Game_task's mode 6 has
             * not entered the village yet: the quest stage is gone (all_reset) and the fade was reset, so the scene
             * showed one frame of sky with the hunter where the village should be (owner, G on the Xbox, 16 Oct 2026,
             * RELEASE_G GB5 bug 10). The PS2 draws nothing between the result screen and Local_main's first fade */
            gfx_begin_frame(0);
            goto frame_done;
        }
        if (quest_dark_tick >= 0 && quest_dark_tick == flip_ticks) {
            /* the quest start (game11-13 on the PS2, one tick here): the screen stays dark (MH1) or on the
             * loading screen (G) until game13's fade_set(2) fades the base camp in on the next tick. Drawing
             * the scene here showed the new stage from a camera not set yet (and in G the title text) */
            gfx_begin_frame(0);
            goto frame_done;
        }
        if (game_cam) {
            rt_cam_view(gc_eye, gc_tar, &gc_roll, &gc_fov);
            have_view = 1;
        }
        if (pl.game && play) {          /* hunter from player_work[0]; camera follows */
            float p[3];
            int a;
            rt_player_get(lp, p, &a);
            place(pl.world, p[0], p[1] + hunter_yoff, p[2], (float)(a & 0xFFFF) * (6.2831853f / 65536.0f));
            if (!game_cam) {
            cam[0] = p[0] + sinf(cam[3]) * follow[0];
            cam[1] = p[1] + follow[1];
            cam[2] = p[2] + cosf(cam[3]) * follow[0];
            cam[4] = follow[2];
            }
        }
        {   /* the hunter the game built last (character, armour) */
            int sx, ids[HUNTER_PARTS], g = rt_player_look(lp, &sx, ids);
            if (g && g != pl.look_gen) {
                pl.look_gen = g;
                hunter_relook(&pl, sx, ids);
            }
        }
        hunter_pose(&pl, fr, light_hunter(lp));
        if (pl.game && play)            /* joint world matrices for the game C (parts, get_joint_pos) */
            sync_joints(&pl, hunter_yoff);
        if ((rt_online_town() || rt_village_active()) && pl.game && play)
            /* the town and the offline village: a weapon bought or equipped there (shop, item box). The PS2's
             * lobby reloads the hunter on equip (armor_set_myArmor: Lb_player_release + Set_userdata +
             * Lb_player_load, whose pl_create_model makes the weapon model); the host's weapon model follows
             * PLW+0x34C here. Before 17 Oct 2026 only the online town did: a weapon bought in the village kept
             * the old model on the back until the next quest (RELEASE_G GB5 bug 13, MH1 too) */
            weapon_follow(&weapon, lp, &weapon_have);
        if (weapon.game && pl.game && play)
            weapon_pose(light_hunter(lp));

        if (getenv("RT_CAM_EM")) {      /* test aid "slot,dist,height,yaw" (or "kKIND,...", "tKIND,..." its cut tail): free camera on monster slot, optional 5th value pitch */
            float p[3], d = 1500, hh = 600, yw = 0, pit = 99;
            int a, sl = 0;
            const char *ce = getenv("RT_CAM_EM");
            int on_tail = ce[0] == 't';             /* "tN,...": that monster's cut-off tail */
            if (ce[0] == 'k' || on_tail) {    /* "kN,...": the first live monster of kind N on this stage */
                extern uint8_t em_work[];
                int kd = 0, j;
                sscanf(ce + 1, "%d,%f,%f,%f,%f", &kd, &d, &hh, &yw, &pit);
                for (j = 0; j < 20; j++)
                    if (em_work[0xA10 * j] && em_work[0xA10 * j + 2] == kd && em_work[0xA10 * j + 0x736] == (uint8_t)rt_game_stage()) {
                        sl = j;
                        break;
                    }
            } else {
                sscanf(getenv("RT_CAM_EM"), "%d,%f,%f,%f,%f", &sl, &d, &hh, &yw, &pit);
            }
            rt_monster_get(sl, p, &a);
            if (on_tail) {
                extern uint8_t em_work[];
                float tp[3], ty;
                if (rt_em_cut_tail(em_work + 0xA10 * sl, tp, &ty))
                    memcpy(p, tp, sizeof tp);
            }
            cam[0] = p[0] + sinf(yw) * d;
            cam[1] = p[1] + hh;
            cam[2] = p[2] + cosf(yw) * d;
            cam[3] = yw;
            cam[4] = pit < 90 ? pit : -atan2f(hh - 250.0f, d);   /* optional 5th value: pitch (rad, negative = down) */
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
        flmat_perspective(proj, game_cam && have_view ? gc_fov : view_fov, scene_aspect(), 10.0f, 80000.0f);

        gfx_begin_frame(0);            /* black, as the game's setBGcolor(0) (G stage 24: black below the jungle edge on the PS2) */
        gfx_set_render_state(GFX_RS_PROJECTION, (uintptr_t)proj);
        gfx_set_render_state(GFX_RS_VIEW, (uintptr_t)view);
        gfx_set_render_state(GFX_RS_ALPHA_REF, 0x40);
        rt_fog_scene(1);                /* the stage's fog (stage_fog_set) for the clays that ask for it */
        gfx_set_render_state(GFX_RS_BLEND, 1);
        {
            flmat id;
            flmat_identity(id);
            gfx_set_render_state(GFX_RS_WORLD, (uintptr_t)id);
            gfx_set_render_state(GFX_RS_ZWRITE, 1);
            rt_prof_begin(RTP_STAGE_DRAW);
            rt_stage_draw();            /* trans_stage: area model + placed set parts */
            rt_prof_end(RTP_STAGE_DRAW);
        }
        gfx_set_render_state(GFX_RS_WORLD, (uintptr_t)pl.world);
        {
            int s;
            for (s = 0; s < HUNTER_PARTS; s++) {
                pick_owner(PK_HUNTER, lp, s, &pl.master);
                draw_model_attr(&pl.part[s], -1);
            }
        }
        if (!rt_village_active() || rt_online_town())
            remote_hunters(1, light_cur());     /* co-op / the online town: the other players' hunters */
        if (rt_village_active())
            npc_draw(light_cur());
        else
            monsters_sync(1, light_cur());
        if (weapon.game && pl.game && play) {
            static flmat wid;
            flmat_identity(wid);
            gfx_set_render_state(GFX_RS_WORLD, (uintptr_t)wid);
            pick_owner(PK_WEAPON, lp, 0, NULL);
            draw_model_attr(&weapon.model, -1);
        }
        if (pl.game && play && !rt_village_active())
            rt_player_item_draw(lp);    /* the item in the hand (BBQ spit, pickaxe, net, ...) */
        /* The game's own order (trans(), weapon/trans.c): stage, then the actors (GameTrans), then the shells, prims, set
         * objects and effects. The translucent ones (smoke, dust, sparks, breath) blend over the monsters and the hunter, and the
         * depth they write does not hide the actors behind them (they used to be drawn first, which cut holes in monsters). */
        rt_prof_begin(RTP_PRIMS);
        rt_game_draw();                 /* game C prims (set14 waterfalls, effects) */
        rt_prof_end(RTP_PRIMS);
        rt_cap_frame();                 /* G: the quest clear's copy of the scene for the reward list (rt_2d.c) */
        rt_fog_scene(0);
        rt_prof_begin(RTP_2D);
        rt_game_draw_2d();              /* screen layers: HUD, info banner, text (after the 3D scene) */
        rt_fade_draw();                 /* fade_draw: the screen fade (Fade_task) */
        rt_prof_end(RTP_2D);
    frame_done:
        if (pick_frame_hook())
            goto redraw;
        menu_draw();                    /* the settings menu over the last picture (and the title screen's hint) */
        dbgmenu_draw();                 /* the debug menu, hit volumes, frame-step line */
        {
            const char *ds = dbgmenu_take_shot();       /* RT_DBG_SHOTS: the menu as it is now */
            if (ds) {
                uint8_t *rgb = malloc((size_t)W * H * 3);
                gfx_read_pixels(rgb);
                write_png(ds, W, H, rgb);
                printf("wrote %s\n", ds);
                free(rgb);
            }
        }

        frame_no++;
        rt_log_frame();
#ifdef XBOX
        {   /* bench aids (docs/xbox.md "Network loader"): a live frame counter on the text screen,
             * and Back + Start held 2 s quits to the loader (the Xbox has no in-game reset) */
            static int held;
            held = pad_combo_held() ? held + 1 : 0;
            if (held == 60) {
                fprintf(stderr, "Back + Start held: exit at frame %d\n", frame_no);
                xbox_log_quit();        /* not the normal shutdown: it hung on hardware (12 Oct) */
            }
            xbox_log_frame(frame_no, SDL_GetTicks());
        }
#endif
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
        if (shot && frame_no >= frames && shot_next <= 0 && !(pick_busy() && (script_live || !pick_wants_pad())) && !menu_busy() && !(dbgmenu_busy() && script_live) && (!shot_list || ticks >= 2 + (int)fr)
            && (!step || frame_no / 30.0f >= fixed_time)) {
            uint8_t *rgb = malloc((size_t)W * H * 3);
            gfx_read_pixels(rgb);
            write_png(shot, W, H, rgb);
            printf("wrote %s (%dx%d, motion frame %.1f)\n", shot, W, H, fr);
            free(rgb);
            running = 0;
        }
        pick_ring_frame();              /* the bug report's clip of the last seconds */
        rt_prof_begin(RTP_GFX);
        gfx_end_frame();
        rt_prof_end(RTP_GFX);
#ifndef XBOX
        if (!shot)
            gfx_frame_pace();           /* --fps-cap: drawing only; the game ticks at 30 Hz from the clock above */
#endif
        rt_prof_end(RTP_DRAW);
        rt_prof_frame();
        rt_lt_frame(ticks, rt_game_stage());    /* "load:" lines for pauses (rt_prof.c) */
    }
    (void)n;
    if (audio_dump && dump_pcm) {
        write_wav(audio_dump, dump_pcm, dump_n / 2);
        printf("wrote %s (%.2fs)\n", audio_dump, dump_n / 2 / 48000.0);
    }
#ifdef MH1_ONLINE
    rt_np_close();
#endif
    if (snd == 0)
        rt_snd_shutdown();
#ifndef XBOX
    if (!shot)
        gfx_opts_save();            /* mh1pc.ini: window size / position, fullscreen */
#endif
    gfx_shutdown();
    fmt_afs_close(&afs);
    rt_stack_report("at exit");
    return 0;
}
