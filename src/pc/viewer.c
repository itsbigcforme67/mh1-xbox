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

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------ data */
static fmt_afs afs;

static fmt_blob load(const char *name, uint8_t **keep)
{
    fmt_blob b = { NULL, 0 };
    size_t n;
    *keep = fmt_afs_load(&afs, name, &n);
    if (!*keep) {
        fprintf(stderr, "missing or bad AFS entry %s\n", name);
        return b;
    }
    b.p = *keep;
    b.n = n;
    return b;
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
        free(c);
        free(at);
    }
}

static fmt_blob ground_hit;
static int ground_y(float x, float z, float ymax, float *y)
{
    return fmt_hits_ground_y(ground_hit, x, z, ymax, y, FMT_LE);
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
    uint8_t *mem[HUNTER_PARTS * 2 + 1];
} hunter;

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
    amo = fmt_link_entry(link, 0, FMT_LE);
    ahi = fmt_link_entry(link, 1, FMT_LE);
    if (fl_model_create(&e->model, amo, ahi, tx, 1, FMT_LE) != 0 || fl_skel_create(&e->skel, ahi, FMT_LE) != 0)
        return -1;
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
        h->pw[s] = calloc(h->part[s].skel.nbone + 1, sizeof(flmat));
        h->ptmat[s] = tbl ? rt_ptr_at(0x3018F0 + 4 * (uint32_t)s) : NULL;   /* relocated by rt_import_data */
    }
    if (!tbl)
        fprintf(stderr, "warning: no SLPM_654.95, armour parts will not follow the skeleton\n");
    tb = load("plcom_tbl.bin", &h->mem[k++]);
    if (tb.p) {
        fl_skel_set_motion(&h->master, 0, tb, legs_id, FMT_LE);   /* char0: legs */
        fl_skel_set_motion(&h->master, 1, tb, upper_id, FMT_LE);  /* char1: upper body */
    }
    flmat_identity(h->world);
    return 0;
}

static void place(flmat w, float x, float y, float z, float yaw)
{
    float s[3] = { 1, 1, 1 }, r[3] = { 0, 0, 0 }, t[3];
    r[1] = yaw;
    t[0] = x; t[1] = y; t[2] = z;
    flmat_srt(w, s, r, t);
}

/* ------------------------------------------------------------ main */
int main(int argc, char **argv)
{
    const char *disc = NULL, *shot = NULL;
    int frames = 1, W = 1280, H = 720, i, running = 1, frame_no = 0;
    float cam[5] = { 11900, 700, 8900, 0.75f, -0.2f };   /* x y z yaw pitch */
    float fixed_time = -1;
    char path[1024];
    size_t n;
    fmt_blob stage_link, stage_tex, set_link, set_tex, hit;
    uint8_t *keep[8];
    fl_model stage, set;
    monster rathian;
    hunter pl;
    fl_light light;
    static const int parts[HUNTER_PARTS] = { 1, 0, 1, 1, 1, 1 };
    float hx = 10900, hz = 7700, rx = 10000, rz = 6700, gy;
    Uint32 t0;
    int set_h0 = -1, ticks = 0, stage_no = 4, cam_given = 0;

    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--shot") && i + 1 < argc) shot = argv[++i];
        else if (!strcmp(argv[i], "--frames") && i + 1 < argc) frames = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--time") && i + 1 < argc) fixed_time = (float)atof(argv[++i]);
        else if (!strcmp(argv[i], "--size") && i + 1 < argc) sscanf(argv[++i], "%dx%d", &W, &H);
        else if (!strcmp(argv[i], "--cam") && i + 1 < argc)
            cam_given = sscanf(argv[++i], "%f,%f,%f,%f,%f", &cam[0], &cam[1], &cam[2], &cam[3], &cam[4]) > 0;
        else if (!strcmp(argv[i], "--stage") && i + 1 < argc) stage_no = (int)strtol(argv[++i], NULL, 0);
        else if (argv[i][0] != '-') disc = argv[i];
    }
    if (!disc) {
        fprintf(stderr, "usage: %s DISC_DIR [--shot out.png] [--frames N] [--time S] "
                "[--size WxH] [--cam x,y,z,yaw,pitch] [--stage N]\n", argv[0]);
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
        uint8_t *ovl = fmt_afs_read(&afs, fmt_afs_find(&afs, "game.bin"), &n);   /* stored raw */
        rt_set_overlay(ovl, ovl ? n : 0);
    }
    if (rt_import_data() != 0)
        fprintf(stderr, "some game data tables are missing\n");
    if (gfx_init(W, H, "MH1 PC viewer", shot != NULL) != 0)
        return 1;

    /* the stage's area model + set model (stage.md 1), ground collision,
     * found through main's per-stage tables (stage 4 = st04, st04_1, lg004) */
    stage_link = load_stage_file(0x2EC950, stage_no, &keep[0]);   /* stage_model_data */
    stage_tex = load_stage_file(0x2EDB40, stage_no, &keep[1]);    /* STAGE_TEX */
    set_link = load_stage_file(0x2ECD70, stage_no, &keep[2]);     /* set_model_data */
    set_tex = load_stage_file(0x2EF130, stage_no, &keep[3]);      /* SET_TEX */
    hit = load_stage_file(0x2ECAB0, stage_no, &keep[4]);          /* stage_hit_data_f */
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
                    if (fmt_hits_ground_y(hit, x, z, 1e6f, &y, FMT_LE)) {
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
            fmt_hits_ground_y(hit, sx, sz + 2500, 1e6f, &y, FMT_LE);
            cam[0] = sx;
            cam[1] = y + 600;
            cam[2] = sz + 2500;
            cam[3] = 0;
            cam[4] = -0.15f;
        }
    }
    if (!stage_link.p || fl_model_create(&stage, fmt_link_entry(stage_link, 0, FMT_LE),
                                         fmt_link_entry(stage_link, 1, FMT_LE), stage_tex, 0, FMT_LE) != 0) {
        fprintf(stderr, "stage load failed\n");
        return 1;
    }
    memset(&set, 0, sizeof set);
    if (set_link.p)
        fl_model_create(&set, fmt_link_entry(set_link, 0, FMT_LE), fmt_link_entry(set_link, 1, FMT_LE),
                        set_tex, 0, FMT_LE);
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
    load_eft_models();
    ground_hit = hit;
    rt_set_ground(ground_y);
    rt_game_init(stage_no);

    if (monster_load(&rathian, "em01_amh.bin", "em01_tex.bin", "em01_tbl.bin", 3) != 0)
        fprintf(stderr, "em01 load failed\n");
    if (hunter_load(&pl, parts, 1, 101) != 0)
        fprintf(stderr, "hunter load failed\n");

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
    fl_model_pose(&rathian.model, (const flmat *)rathian.skel.world, &light);
    gy = 0;
    fmt_hits_ground_y(hit, rx, rz, 1e6f, &gy, FMT_LE);
    place(rathian.world, rx, gy - min_y_of(&rathian.model), rz, 0.6f);
    hunter_pose(&pl, 0, &light);
    {
        float lo = 1e30f;
        int s;
        for (s = 0; s < HUNTER_PARTS; s++) {
            float y = min_y_of(&pl.part[s]);
            if (y < lo)
                lo = y;
        }
        gy = 0;
        fmt_hits_ground_y(hit, hx, hz, 1e6f, &gy, FMT_LE);
        place(pl.world, hx, gy - lo, hz, 2.6f);
        {   /* the hunter is the master player (player_work[0]) for the game C */
            float p[3] = { hx, gy, hz };
            rt_set_player(0, p);
        }
    }

    if (!shot)
        SDL_SetRelativeMouseMode(SDL_TRUE);
    t0 = SDL_GetTicks();
    while (running) {
        SDL_Event ev;
        float t = fixed_time >= 0 ? fixed_time : (SDL_GetTicks() - t0) / 1000.0f;
        float fr = t * 30.0f;                      /* game motions run at 30 fps */
        flmat proj, camw, view;
        const Uint8 *keys;
        float spd = 40;

        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT || (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE))
                running = 0;
            else if (ev.type == SDL_MOUSEMOTION && !shot) {
                cam[3] -= ev.motion.xrel * 0.003f;
                cam[4] -= ev.motion.yrel * 0.003f;
                if (cam[4] > 1.5f) cam[4] = 1.5f;
                if (cam[4] < -1.5f) cam[4] = -1.5f;
            }
        }
        {   /* camera: rotate pitch then yaw, looking down -Z like GL */
            float s[3] = { 1, 1, 1 }, r[3], tr[3];
            r[0] = cam[4]; r[1] = cam[3]; r[2] = 0;
            tr[0] = cam[0]; tr[1] = cam[1]; tr[2] = cam[2];
            flmat_srt(camw, s, r, tr);
        }
        keys = SDL_GetKeyboardState(NULL);
        if (!shot) {
            if (keys[SDL_SCANCODE_LSHIFT]) spd *= 6;
            if (keys[SDL_SCANCODE_W]) { cam[0] -= camw[8] * spd; cam[1] -= camw[9] * spd; cam[2] -= camw[10] * spd; }
            if (keys[SDL_SCANCODE_S]) { cam[0] += camw[8] * spd; cam[1] += camw[9] * spd; cam[2] += camw[10] * spd; }
            if (keys[SDL_SCANCODE_A]) { cam[0] -= camw[0] * spd; cam[2] -= camw[2] * spd; }
            if (keys[SDL_SCANCODE_D]) { cam[0] += camw[0] * spd; cam[2] += camw[2] * spd; }
            if (keys[SDL_SCANCODE_SPACE]) cam[1] += spd;
            if (keys[SDL_SCANCODE_C]) cam[1] -= spd;
        }
        flmat_invert_affine(view, camw);
        rt_set_camera(camw);            /* rview_mat / rview_matY for game billboards */
        flmat_perspective(proj, 1.0f, (float)W / H, 10.0f, 80000.0f);

        /* game logic ticks at 30 per second (at least 2, so set objects
         * have run their init and queued their prims) */
        while (ticks < 2 + (int)fr) {
            rt_game_move();
            ticks++;
        }
        fl_skel_update(&rathian.skel, fr);
        fl_model_pose(&rathian.model, (const flmat *)rathian.skel.world, &light);
        hunter_pose(&pl, fr, &light);

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
        gfx_set_render_state(GFX_RS_WORLD, (uintptr_t)rathian.world);
        draw_model_attr(&rathian.model, -1);
        gfx_set_render_state(GFX_RS_WORLD, (uintptr_t)pl.world);
        {
            int s;
            for (s = 0; s < HUNTER_PARTS; s++)
                draw_model_attr(&pl.part[s], -1);
        }

        frame_no++;
        if (shot && frame_no >= frames) {
            uint8_t *rgb = malloc((size_t)W * H * 3);
            gfx_read_pixels(rgb);
            write_png(shot, W, H, rgb);
            printf("wrote %s (%dx%d, motion frame %.1f)\n", shot, W, H, fr);
            free(rgb);
            running = 0;
        }
        gfx_end_frame();
    }
    (void)n;
    gfx_shutdown();
    fmt_afs_close(&afs);
    return 0;
}
