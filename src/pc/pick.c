/*
 * pick.c - the in-game bug reporter (PC front-end; not part of the Xbox build).
 *
 * F8 (or Back/View + Start on a controller) freezes the game: no ticks, audio paused. The frame is grabbed,
 * drawn once more with flat per-draw-call ids (gfx_pick_pass: the draw sites tag what they draw, rt_pick.c),
 * and the player clicks broken things (or drags a box); right click undoes the last pick; typing fills the
 * note; Enter saves a report folder, Esc cancels. See docs/pc.md "Bug reporter".
 *
 * The report (reports/report_<date>/ next to the logs folder, never in the repository): screenshot.png,
 * annotated.png (highlights, click and box marks), idbuffer.png (false colours), report.json (note, marks,
 * the picked objects with ids / render states / positions, the game state, build, seed), log_tail.txt (the
 * last 300 log lines), clip.gif (the last ~10 s, 320x240) and input.txt (the pad state of every tick from the
 * start of the session, for a headless replay). Nothing leaves the machine.
 */
#include "pick.h"
#include "rt/rt.h"
#include "rt/rt_pick.h"
#include "rt/rt_log.h"
#include "fl/fl.h"
#include "pad/pad.h"
#include "audio/audio.h"

#include <SDL.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#ifdef MH1_WIN
#include <direct.h>
#endif

int viewer_write_png(const char *path, int w, int h, const uint8_t *rgb);
char *pick_game_json(void);
int rt_seed_get(void);
const char *rt_log_build(void);
void pick_json_str(char *out, int cap, const char *s);
void audio_pause(int on);

enum { PS_OFF, PS_ARMED, PS_PASS, PS_FROZEN };
static int state, W, H;
static int manual;                      /* started by a key (else by RT_PICK_AT) */
static uint8_t *shot, *disp;            /* the frozen frame; with highlights */
static uint32_t *idbuf;                 /* per pixel: entry id */
static float *depth;
static float view[16], proj[16];
static gfx_texture *dtex;
static Uint32 frozen_ms0;
static int dirty;

/* ------------------------------------------------------------ 5x7 font (ASCII 32..126), rows top to bottom, 5 bits */
static const unsigned char font5x7[95][7] = {
    {0,0,0,0,0,0,0},{4,4,4,4,4,0,4},{10,10,10,0,0,0,0},{10,10,31,10,31,10,10},{4,15,20,14,5,30,4},{24,25,2,4,8,19,3},{12,18,20,8,21,18,13},{12,4,8,0,0,0,0},
    {2,4,8,8,8,4,2},{8,4,2,2,2,4,8},{0,4,21,14,21,4,0},{0,4,4,31,4,4,0},{0,0,0,0,12,4,8},{0,0,0,31,0,0,0},{0,0,0,0,0,12,12},{0,1,2,4,8,16,0},
    {14,17,19,21,25,17,14},{4,12,4,4,4,4,14},{14,17,1,2,4,8,31},{31,2,4,2,1,17,14},{2,6,10,18,31,2,2},{31,16,30,1,1,17,14},{6,8,16,30,17,17,14},{31,1,2,4,8,8,8},
    {14,17,17,14,17,17,14},{14,17,17,15,1,2,12},{0,12,12,0,12,12,0},{0,12,12,0,12,4,8},{2,4,8,16,8,4,2},{0,0,31,0,31,0,0},{8,4,2,1,2,4,8},{14,17,1,2,4,0,4},
    {14,17,23,21,23,16,14},{14,17,17,31,17,17,17},{30,17,17,30,17,17,30},{14,17,16,16,16,17,14},{28,18,17,17,17,18,28},{31,16,16,30,16,16,31},{31,16,16,30,16,16,16},{14,17,16,23,17,17,15},
    {17,17,17,31,17,17,17},{14,4,4,4,4,4,14},{7,2,2,2,2,18,12},{17,18,20,24,20,18,17},{16,16,16,16,16,16,31},{17,27,21,21,17,17,17},{17,17,25,21,19,17,17},{14,17,17,17,17,17,14},
    {30,17,17,30,16,16,16},{14,17,17,17,21,18,13},{30,17,17,30,20,18,17},{15,16,16,14,1,1,30},{31,4,4,4,4,4,4},{17,17,17,17,17,17,14},{17,17,17,17,17,10,4},{17,17,17,21,21,21,10},
    {17,17,10,4,10,17,17},{17,17,17,10,4,4,4},{31,1,2,4,8,16,31},{14,8,8,8,8,8,14},{0,16,8,4,2,1,0},{14,2,2,2,2,2,14},{4,10,17,0,0,0,0},{0,0,0,0,0,0,31},
    {8,4,2,0,0,0,0},{0,0,14,1,15,17,15},{16,16,22,25,17,17,30},{0,0,14,16,16,17,14},{1,1,13,19,17,17,15},{0,0,14,17,31,16,14},{6,9,8,28,8,8,8},{0,15,17,17,15,1,14},
    {16,16,22,25,17,17,17},{4,0,12,4,4,4,14},{2,0,6,2,2,18,12},{16,16,18,20,24,20,18},{12,4,4,4,4,4,14},{0,0,26,21,21,17,17},{0,0,22,25,17,17,17},{0,0,14,17,17,17,14},
    {0,0,30,17,30,16,16},{0,0,13,19,15,1,1},{0,0,22,25,16,16,16},{0,0,15,16,14,1,30},{8,8,28,8,8,9,6},{0,0,17,17,17,19,13},{0,0,17,17,17,10,4},{0,0,17,17,21,21,10},
    {0,0,17,10,4,10,17},{0,0,17,17,15,1,14},{0,0,31,2,4,8,31},{2,4,4,8,4,4,2},{4,4,4,4,4,4,4},{8,4,4,2,4,4,8},{0,0,8,21,2,0,0},
};

static const unsigned char *glyph_rows(int c)
{
    static const unsigned char unknown[7] = { 31, 17, 17, 17, 17, 17, 31 };
    return c >= 32 && c <= 126 ? font5x7[c - 32] : unknown;
}

/* ------------------------------------------------------------ CPU drawing into an RGB image */
static void px(uint8_t *img, int x, int y, int r, int g, int b)
{
    if (x < 0 || y < 0 || x >= W || y >= H)
        return;
    img[((size_t)y * W + x) * 3] = (uint8_t)r;
    img[((size_t)y * W + x) * 3 + 1] = (uint8_t)g;
    img[((size_t)y * W + x) * 3 + 2] = (uint8_t)b;
}

static void cpu_rect(uint8_t *img, int x0, int y0, int x1, int y1, int r, int g, int b)
{
    int x, y;
    for (y = y0; y <= y1; y++)
        for (x = x0; x <= x1; x++)
            px(img, x, y, r, g, b);
}

static void cpu_box(uint8_t *img, int x0, int y0, int x1, int y1, int r, int g, int b)
{
    cpu_rect(img, x0, y0, x1, y0 + 1, r, g, b);
    cpu_rect(img, x0, y1 - 1, x1, y1, r, g, b);
    cpu_rect(img, x0, y0, x0 + 1, y1, r, g, b);
    cpu_rect(img, x1 - 1, y0, x1, y1, r, g, b);
}

static void cpu_text(uint8_t *img, int x, int y, const char *s, int sc, int r, int g, int b)
{
    for (; *s; s++, x += 6 * sc) {
        const unsigned char *rows = glyph_rows((unsigned char)*s);
        int ry, rx;
        for (ry = 0; ry < 7; ry++)
            for (rx = 0; rx < 5; rx++)
                if (rows[ry] & (16 >> rx))
                    cpu_rect(img, x + rx * sc, y + ry * sc, x + rx * sc + sc - 1, y + ry * sc + sc - 1, r, g, b);
    }
}

/* ------------------------------------------------------------ GL overlay (quads into one draw call) */
static float *uv_pos;
static uint8_t *uv_col;
static int uv_n, uv_cap;

static void ui_quad(float x0, float y0, float x1, float y1, unsigned rgba)
{
    float p[12] = { x0, y0, x1, y0, x0, y1, x1, y0, x1, y1, x0, y1 };
    int i;
    if (uv_n + 6 > uv_cap) {
        uv_cap = uv_cap ? uv_cap * 2 : 1 << 16;
        uv_pos = (float *)realloc(uv_pos, (size_t)uv_cap * 2 * sizeof(float));
        uv_col = (uint8_t *)realloc(uv_col, (size_t)uv_cap * 4);
    }
    memcpy(uv_pos + 2 * uv_n, p, sizeof p);
    for (i = 0; i < 6; i++) {
        uv_col[4 * (uv_n + i)] = (uint8_t)(rgba >> 24);
        uv_col[4 * (uv_n + i) + 1] = (uint8_t)(rgba >> 16);
        uv_col[4 * (uv_n + i) + 2] = (uint8_t)(rgba >> 8);
        uv_col[4 * (uv_n + i) + 3] = (uint8_t)rgba;
    }
    uv_n += 6;
}

static void ui_text(int x, int y, const char *s, int sc, unsigned rgba)
{
    for (; *s; s++, x += 6 * sc) {
        const unsigned char *rows = glyph_rows((unsigned char)*s);
        int ry, rx;
        for (ry = 0; ry < 7; ry++)
            for (rx = 0; rx < 5; rx++)
                if (rows[ry] & (16 >> rx))
                    ui_quad((float)(x + rx * sc), (float)(y + ry * sc), (float)(x + rx * sc + sc), (float)(y + ry * sc + sc), rgba);
    }
}

static void ui_flush(void)
{
    if (uv_n)
        gfx_draw_2d(W, H, uv_n, uv_pos, NULL, uv_col);
    uv_n = 0;
}

/* ------------------------------------------------------------ selection */
#define MAXSEL 40
#define MAXACT 64
typedef struct {
    uint32_t id;                /* representative entry */
    int act;                    /* which mark picked it */
    int hx, hy;                 /* click pixel (-1 for a box pick) */
} sel_t;
typedef struct { int type, x0, y0, x1, y1, first, count; } mark_t;
static sel_t sel[MAXSEL];
static int nsel;
static mark_t marks[MAXACT];
static int nmarks;
static signed char *grp;        /* per entry id: selection index or -1 */
static int grp_n;
static char note[512];
static int drag, dx0, dy0, mx, my;

static const unsigned pal[6][3] = { { 255, 220, 0 }, { 0, 220, 255 }, { 255, 80, 200 }, { 120, 255, 80 }, { 255, 140, 0 }, { 170, 130, 255 } };

static void rebuild_groups(void)
{
    int i, k, n = pick_count();
    free(grp);
    grp = (signed char *)malloc((size_t)n + 2);
    grp_n = n;
    for (i = 1; i <= n; i++) {
        grp[i] = -1;
        for (k = 0; k < nsel; k++)
            if (pick_same(pick_entry((uint32_t)i), pick_entry(sel[k].id))) {
                grp[i] = (signed char)k;
                break;
            }
    }
}

static void build_disp(uint8_t *out, int draw_marks)
{
    size_t i;
    int x, y;
    memcpy(out, shot, (size_t)W * H * 3);
    if (nsel) {
        for (y = 0; y < H; y++)
            for (x = 0; x < W; x++) {
                uint32_t id = idbuf[(size_t)y * W + x];
                int g = id && id <= (uint32_t)grp_n ? grp[id] : -1;
                if (g < 0)
                    continue;
                {
                    int edge = 0, k;
                    static const int dxs[4] = { 1, -1, 0, 0 }, dys[4] = { 0, 0, 1, -1 };
                    const unsigned *c = pal[g % 6];
                    uint8_t *o = out + ((size_t)y * W + x) * 3;
                    for (k = 0; k < 4; k++) {
                        int nx = x + dxs[k] * 2, ny = y + dys[k] * 2;
                        if (nx < 0 || ny < 0 || nx >= W || ny >= H) { edge = 1; break; }
                        {
                            uint32_t nid = idbuf[(size_t)ny * W + nx];
                            int ng = nid && nid <= (uint32_t)grp_n ? grp[nid] : -1;
                            if (ng != g) { edge = 1; break; }
                        }
                    }
                    if (edge) {
                        o[0] = (uint8_t)c[0]; o[1] = (uint8_t)c[1]; o[2] = (uint8_t)c[2];
                    } else {
                        o[0] = (uint8_t)((o[0] * 5 + c[0] * 3) / 8);
                        o[1] = (uint8_t)((o[1] * 5 + c[1] * 3) / 8);
                        o[2] = (uint8_t)((o[2] * 5 + c[2] * 3) / 8);
                    }
                }
            }
    }
    if (draw_marks) {
        int m;
        for (m = 0; m < nmarks; m++) {
            char lab[8];
            snprintf(lab, sizeof lab, "%d", m + 1);
            if (marks[m].type == 0) {
                cpu_rect(out, marks[m].x0 - 9, marks[m].y0 - 1, marks[m].x0 + 9, marks[m].y0 + 1, 255, 0, 0);
                cpu_rect(out, marks[m].x0 - 1, marks[m].y0 - 9, marks[m].x0 + 1, marks[m].y0 + 9, 255, 0, 0);
                cpu_text(out, marks[m].x0 + 6, marks[m].y0 + 4, lab, 2, 255, 255, 255);
            } else {
                cpu_box(out, marks[m].x0, marks[m].y0, marks[m].x1, marks[m].y1, 255, 0, 0);
                cpu_text(out, marks[m].x0 + 4, marks[m].y0 + 4, lab, 2, 255, 255, 255);
            }
        }
    }
    (void)i;
}

static void upload_disp(void)
{
    size_t n = (size_t)W * H, i;
    uint8_t *rgba = (uint8_t *)malloc(n * 4);
    build_disp(disp, 0);
    for (i = 0; i < n; i++) {
        rgba[4 * i] = disp[3 * i];
        rgba[4 * i + 1] = disp[3 * i + 1];
        rgba[4 * i + 2] = disp[3 * i + 2];
        rgba[4 * i + 3] = 255;
    }
    if (dtex)
        gfx_release_texture(dtex);
    dtex = gfx_create_texture(W, H, rgba);
    free(rgba);
    dirty = 0;
}

static int sel_has(uint32_t id)
{
    int k;
    for (k = 0; k < nsel; k++)
        if (pick_same(pick_entry(id), pick_entry(sel[k].id)))
            return 1;
    return 0;
}

static int add_click(int x, int y)
{
    uint32_t id;
    mark_t *m;
    if (x < 0 || y < 0 || x >= W || y >= H || nmarks >= MAXACT)
        return 0;
    id = idbuf[(size_t)y * W + x];
    m = &marks[nmarks];
    m->type = 0;
    m->x0 = x;
    m->y0 = y;
    m->first = nsel;
    m->count = 0;
    if (id && id <= (uint32_t)pick_count() && !sel_has(id) && nsel < MAXSEL) {
        sel[nsel].id = id;
        sel[nsel].act = nmarks;
        sel[nsel].hx = x;
        sel[nsel].hy = y;
        nsel++;
        m->count = 1;
    }
    nmarks++;
    rebuild_groups();
    dirty = 1;
    return 1;
}

static int add_box(int x0, int y0, int x1, int y1)
{
    int x, y, t;
    mark_t *m;
    if (nmarks >= MAXACT)
        return 0;
    if (x0 > x1) { t = x0; x0 = x1; x1 = t; }
    if (y0 > y1) { t = y0; y0 = y1; y1 = t; }
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 >= W) x1 = W - 1;
    if (y1 >= H) y1 = H - 1;
    m = &marks[nmarks];
    m->type = 1;
    m->x0 = x0; m->y0 = y0; m->x1 = x1; m->y1 = y1;
    m->first = nsel;
    m->count = 0;
    for (y = y0; y <= y1; y++)
        for (x = x0; x <= x1; x++) {
            uint32_t id = idbuf[(size_t)y * W + x];
            if (id && id <= (uint32_t)pick_count() && nsel < MAXSEL && !sel_has(id)) {
                sel[nsel].id = id;
                sel[nsel].act = nmarks;
                sel[nsel].hx = sel[nsel].hy = -1;
                nsel++;
                m->count++;
            }
        }
    nmarks++;
    rebuild_groups();
    dirty = 1;
    return 1;
}

static void undo(void)
{
    if (!nmarks)
        return;
    nmarks--;
    nsel = marks[nmarks].first;
    rebuild_groups();
    dirty = 1;
}

/* ------------------------------------------------------------ math for the hit position */
static void mat_mul(float *o, const float *a, const float *b)         /* o = a * b, column-major (GL) */
{
    float t[16];
    int r, c, k;
    for (c = 0; c < 4; c++)
        for (r = 0; r < 4; r++) {
            t[c * 4 + r] = 0;
            for (k = 0; k < 4; k++)
                t[c * 4 + r] += a[k * 4 + r] * b[c * 4 + k];
        }
    memcpy(o, t, sizeof t);
}

static int mat_inv(float *o, const float *m)
{
    double a[4][8];
    int i, j, k;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++) {
            a[i][j] = m[j * 4 + i];
            a[i][4 + j] = i == j;
        }
    for (i = 0; i < 4; i++) {
        int p = i;
        double d;
        for (k = i + 1; k < 4; k++)
            if (fabs(a[k][i]) > fabs(a[p][i]))
                p = k;
        if (fabs(a[p][i]) < 1e-20)
            return -1;
        if (p != i)
            for (j = 0; j < 8; j++) { d = a[i][j]; a[i][j] = a[p][j]; a[p][j] = d; }
        d = a[i][i];
        for (j = 0; j < 8; j++)
            a[i][j] /= d;
        for (k = 0; k < 4; k++)
            if (k != i) {
                d = a[k][i];
                for (j = 0; j < 8; j++)
                    a[k][j] -= d * a[i][j];
            }
    }
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            o[j * 4 + i] = (float)a[i][4 + j];
    return 0;
}

/* window pixel + depth -> world position (the game's world: the view / projection of the id pass) */
static int unproject(int x, int y, float out[3])
{
    float pv[16], inv[16], v[4], w;
    float d = depth ? depth[(size_t)y * W + x] : 1.0f;
    if (d >= 1.0f)
        return 0;
    mat_mul(pv, proj, view);
    if (mat_inv(inv, pv))
        return 0;
    v[0] = (x + 0.5f) / W * 2 - 1;
    v[1] = 1 - (y + 0.5f) / H * 2;
    v[2] = d * 2 - 1;
    v[3] = 1;
    {
        float r[4];
        int i;
        for (i = 0; i < 4; i++)
            r[i] = inv[i] * v[0] + inv[4 + i] * v[1] + inv[8 + i] * v[2] + inv[12 + i] * v[3];
        w = r[3];
        if (fabsf(w) < 1e-12f)
            return 0;
        out[0] = r[0] / w;
        out[1] = r[1] / w;
        out[2] = r[2] / w;
    }
    return 1;
}

/* ------------------------------------------------------------ saving */
static void mkdirs(const char *path)
{
    char tmp[900], *s;
    snprintf(tmp, sizeof tmp, "%s", path);
    for (s = tmp + 1; *s; s++)
        if (*s == '/' || *s == '\\') {
            char c = *s;
            *s = 0;
            mkdir(tmp, 0755);
            *s = c;
        }
    mkdir(tmp, 0755);
}

static void write_text(const char *dir, const char *name, const char *text)
{
    char p[1000];
    FILE *f;
    snprintf(p, sizeof p, "%s/%s", dir, name);
    f = fopen(p, "wb");
    if (f) {
        fwrite(text, 1, strlen(text), f);
        fclose(f);
    }
}

static void idbuffer_png(const char *dir)
{
    char p[1000];
    size_t n = (size_t)W * H, i;
    uint8_t *img = (uint8_t *)malloc(n * 3);
    for (i = 0; i < n; i++) {
        uint32_t id = idbuf[i], h = id * 2654435761u;
        img[3 * i] = id ? (uint8_t)(64 + (h >> 24) % 192) : 0;
        img[3 * i + 1] = id ? (uint8_t)(64 + (h >> 16) % 192) : 0;
        img[3 * i + 2] = id ? (uint8_t)(64 + (h >> 8) % 192) : 0;
    }
    snprintf(p, sizeof p, "%s/idbuffer.png", dir);
    viewer_write_png(p, W, H, img);
    free(img);
}

/* --- the clip: the last seconds, small, as an animated GIF */
#define RW 320
#define RH 240
#define RING_N 60
#define RING_MS 170
static uint8_t *ring;
static int rhead, rcount;
static Uint32 last_cap;

void pick_ring_frame(void)
{
    static uint8_t *full;
    Uint32 now;
    int x, y;
    uint8_t *o;
    if (state != PS_OFF)
        return;
    {
        static int ms = -1;
        if (ms < 0)
            ms = getenv("RT_PICK_RING_MS") ? atoi(getenv("RT_PICK_RING_MS")) : RING_MS;     /* 0: every frame (headless tests) */
        now = SDL_GetTicks();
        if (now - last_cap < (Uint32)ms)
            return;
    }
    last_cap = now;
    if (!ring) {
        ring = (uint8_t *)malloc((size_t)RING_N * RW * RH * 3);
        if (!ring)
            return;
    }
    gfx_size(&W, &H);
    full = (uint8_t *)realloc(full, (size_t)W * H * 3);
    if (!full || gfx_read_pixels(full))
        return;
    o = ring + (size_t)rhead * RW * RH * 3;
    for (y = 0; y < RH; y++)
        for (x = 0; x < RW; x++)
            memcpy(o + ((size_t)y * RW + x) * 3, full + ((size_t)(y * H / RH) * W + (size_t)(x * W / RW)) * 3, 3);
    rhead = (rhead + 1) % RING_N;
    if (rcount < RING_N)
        rcount++;
}

typedef struct { FILE *f; unsigned acc; int nb; unsigned char blk[255]; int bn; } bw_t;
static void bw_byte(bw_t *b, int v)
{
    b->blk[b->bn++] = (unsigned char)v;
    if (b->bn == 255) {
        fputc(255, b->f);
        fwrite(b->blk, 1, 255, b->f);
        b->bn = 0;
    }
}
static void bw_code(bw_t *b, int code, int size)
{
    b->acc |= (unsigned)code << b->nb;
    b->nb += size;
    while (b->nb >= 8) {
        bw_byte(b, (int)(b->acc & 255));
        b->acc >>= 8;
        b->nb -= 8;
    }
}

static void gif_frame(FILE *f, const uint8_t *rgb)
{
    static int *tab;               /* hash: key (prefix << 8 | byte) + 1 -> code */
    static int *keys;
    int i, next = 258, size = 9, prefix;
    bw_t b;
    const int HS = 16384;
    if (!tab) {
        tab = (int *)malloc(sizeof(int) * HS);
        keys = (int *)malloc(sizeof(int) * HS);
    }
    fputc(0x21, f); fputc(0xF9, f); fputc(4, f); fputc(0, f); fputc(17, f); fputc(0, f); fputc(0, f); fputc(0, f);   /* 170 ms */
    fputc(0x2C, f);
    fputc(0, f); fputc(0, f); fputc(0, f); fputc(0, f);
    fputc(RW & 255, f); fputc(RW >> 8, f); fputc(RH & 255, f); fputc(RH >> 8, f);
    fputc(0, f);
    fputc(8, f);
    memset(&b, 0, sizeof b);
    b.f = f;
    for (i = 0; i < HS; i++) keys[i] = 0;
    bw_code(&b, 256, size);
    {
        int n = RW * RH, p;
        prefix = (rgb[0] >> 5 << 5) | (rgb[1] >> 5 << 2) | (rgb[2] >> 6);
        for (p = 1; p < n; p++) {
            int c = (rgb[3 * p] >> 5 << 5) | (rgb[3 * p + 1] >> 5 << 2) | (rgb[3 * p + 2] >> 6);
            int key = (prefix << 8 | c) + 1, h = (int)((unsigned)key * 2654435761u >> 18) & (HS - 1);
            int code = -1;
            while (keys[h]) {
                if (keys[h] == key) { code = tab[h]; break; }
                h = (h + 1) & (HS - 1);
            }
            if (code >= 0) {
                prefix = code;
                continue;
            }
            bw_code(&b, prefix, size);
            keys[h] = key;
            tab[h] = next++;
            if (next - 1 >= (1 << size) && size < 12)
                size++;
            if (next - 1 == 4095) {
                bw_code(&b, 256, size);
                for (i = 0; i < HS; i++) keys[i] = 0;
                next = 258;
                size = 9;
            }
            prefix = c;
        }
        bw_code(&b, prefix, size);
    }
    bw_code(&b, 257, size);
    if (b.nb)
        bw_byte(&b, (int)(b.acc & 255));
    if (b.bn) {
        fputc(b.bn, f);
        fwrite(b.blk, 1, (size_t)b.bn, f);
    }
    fputc(0, f);
}

static int write_gif(const char *path)
{
    FILE *f;
    int i, k;
    if (!ring || rcount < 2)
        return -1;
    f = fopen(path, "wb");
    if (!f)
        return -1;
    fwrite("GIF89a", 1, 6, f);
    fputc(RW & 255, f); fputc(RW >> 8, f); fputc(RH & 255, f); fputc(RH >> 8, f);
    fputc(0xF7, f); fputc(0, f); fputc(0, f);
    for (i = 0; i < 256; i++) {
        fputc((i >> 5) * 255 / 7, f);
        fputc(((i >> 2) & 7) * 255 / 7, f);
        fputc((i & 3) * 255 / 3, f);
    }
    fwrite("\x21\xFF\x0BNETSCAPE2.0\x03\x01\x00\x00\x00", 1, 19, f);
    for (k = 0; k < rcount; k++) {
        int idx = (rhead - rcount + k + RING_N * 2) % RING_N;
        gif_frame(f, ring + (size_t)idx * RW * RH * 3);
    }
    fputc(0x3B, f);
    fclose(f);
    return 0;
}

static void jappend(char **buf, int *n, int *cap, const char *fmt, ...)
{
    va_list ap;
    int k;
    if (*n + 4096 > *cap) {
        *cap *= 2;
        *buf = (char *)realloc(*buf, (size_t)*cap);
    }
    va_start(ap, fmt);
    k = vsnprintf(*buf + *n, (size_t)(*cap - *n), fmt, ap);
    va_end(ap);
    if (k > 0)
        *n += k < *cap - *n ? k : *cap - *n - 1;
}

/* nearest bone of the object's skeleton to a world position */
static int nearest_bone(const pick_entry_t *e, const float hit[3], float *dist)
{
    const fl_skel *sk = (const fl_skel *)e->tag.obj;
    flmat inv;
    float lp[3];
    int b, best = -1;
    float bd = 1e30f;
    if (!sk || !sk->world || sk->skel.nbone <= 0)
        return -1;
    flmat_invert_affine(inv, e->gi.world);
    flmat_apply(lp, hit, inv);
    for (b = 0; b < sk->skel.nbone; b++) {
        float dx = sk->world[b][12] - lp[0], dy = sk->world[b][13] - lp[1], dz = sk->world[b][14] - lp[2];
        float d = dx * dx + dy * dy + dz * dz;
        if (d < bd) { bd = d; best = b; }
    }
    if (dist)
        *dist = sqrtf(bd);
    return best;
}

static char *report_json(const char *name)
{
    int cap = 1 << 16, n = 0, k;
    char *o = (char *)malloc((size_t)cap), *g, nt[1400];
    int nticks = 0;
    char *inp = rt_pick_input_script(&nticks);
    char tbuf[64];
    time_t t = time(NULL);
    unsigned seed = (unsigned)rt_seed_get();
    if (!o) return NULL;
    strftime(tbuf, sizeof tbuf, "%Y-%m-%dT%H:%M:%S", localtime(&t));
    pick_json_str(nt, sizeof nt, note);
    jappend(&o, &n, &cap, "{\n \"report\": \"%s\",\n \"created\": \"%s\",\n \"build\": \"%s\",\n \"note\": %s,\n", name, tbuf, rt_log_build(), nt);
    jappend(&o, &n, &cap, " \"window\": [%d,%d],\n \"random_seed\": %u,\n", W, H, seed);
    {
        char as[1400];
        pick_json_str(as, sizeof as, rt_log_path(rt_pick_args()));
        jappend(&o, &n, &cap, " \"arguments\": %s,\n", as);
    }
    jappend(&o, &n, &cap, " \"replay\": {\"input_file\": \"input.txt\", \"ticks\": %d, \"how\": \"RT_SEED=%u mhview <disc> [--boot or the same --quest/--stage] --input @input.txt --shot out.png --time <ticks/30> "
            "(the pad state of every game tick from the start of the session; typed text of the name entry: RT_NAME)\"},\n", nticks, seed);
    jappend(&o, &n, &cap, " \"marks\": [");
    for (k = 0; k < nmarks; k++) {
        mark_t *m = &marks[k];
        if (m->type == 0)
            jappend(&o, &n, &cap, "%s{\"n\":%d,\"type\":\"click\",\"x\":%d,\"y\":%d,\"picked\":%d}", k ? "," : "", k + 1, m->x0, m->y0, m->count);
        else
            jappend(&o, &n, &cap, "%s{\"n\":%d,\"type\":\"box\",\"x0\":%d,\"y0\":%d,\"x1\":%d,\"y1\":%d,\"picked\":%d}", k ? "," : "", k + 1, m->x0, m->y0, m->x1, m->y1, m->count);
    }
    jappend(&o, &n, &cap, "],\n \"objects\": [\n");
    for (k = 0; k < nsel; k++) {
        const pick_entry_t *e = pick_entry(sel[k].id);
        char *j = (char *)malloc(8192);
        int jl, j2 = 0;
        float hit[3];
        if (!e || !j) { free(j); continue; }
        jl = pick_json(e, j, 8192);
        (void)j2;
        if (jl > 0 && j[jl - 1] == '}') {
            char extra[600];
            int en = snprintf(extra, sizeof extra, ",\"picked_by_mark\":%d", sel[k].act + 1);
            if (sel[k].hx >= 0) {
                en += snprintf(extra + en, sizeof extra - (size_t)en, ",\"click_pixel\":[%d,%d]", sel[k].hx, sel[k].hy);
                if (unproject(sel[k].hx, sel[k].hy, hit)) {
                    int bone;
                    float bd = 0;
                    en += snprintf(extra + en, sizeof extra - (size_t)en, ",\"hit_world\":[%.1f,%.1f,%.1f]", hit[0], hit[1], hit[2]);
                    bone = nearest_bone(e, hit, &bd);
                    if (bone >= 0)
                        en += snprintf(extra + en, sizeof extra - (size_t)en, ",\"nearest_bone\":{\"index\":%d,\"distance\":%.1f}", bone, bd);
                }
            }
            jl--;
            memcpy(j + jl, extra, (size_t)en);
            j[jl + en] = '}';
            j[jl + en + 1] = 0;
        }
        jappend(&o, &n, &cap, "%s  %s", k ? ",\n" : "", j);
        free(j);
    }
    g = pick_game_json();
    jappend(&o, &n, &cap, "\n ],\n \"game\": %s,\n", g ? g : "null");
    jappend(&o, &n, &cap, " \"files\": [\"screenshot.png\",\"annotated.png\",\"idbuffer.png\",\"log_tail.txt\",\"clip.gif\",\"input.txt\"]\n}\n");
    free(g);
    free(inp);
    return o;
}

static void save_report(void)
{
    char base[900], dir[1000], stamp[40], name[80];
    time_t t = time(NULL);
    char *json, *tail, *inp;
    const char *ld = rt_log_dir();
    size_t l;
    uint8_t *ann;
    char p[1100];
    strftime(stamp, sizeof stamp, "%Y%m%d_%H%M%S", localtime(&t));
    snprintf(base, sizeof base, "%s", ld);
    l = strlen(base);
    while (l > 1 && (base[l - 1] == '/' || base[l - 1] == '\\'))
        base[--l] = 0;
    while (l > 0 && base[l - 1] != '/' && base[l - 1] != '\\')
        l--;
    if (l == 0)
        snprintf(base, sizeof base, "reports");
    else
        snprintf(base + l, sizeof base - l, "reports");
    snprintf(name, sizeof name, "report_%s", stamp);
    snprintf(dir, sizeof dir, "%s/%s", base, name);
    {
        int k;
        for (k = 2; k < 100; k++) {
            FILE *x = fopen(dir, "rb");        /* (a directory: fopen fails on it on Windows, succeeds on Linux) */
            struct stat st;
            if (x) fclose(x);
            if (stat(dir, &st) != 0)
                break;
            snprintf(name, sizeof name, "report_%s_%d", stamp, k);
            snprintf(dir, sizeof dir, "%s/%s", base, name);
        }
    }
    mkdirs(dir);
    snprintf(p, sizeof p, "%s/screenshot.png", dir);
    viewer_write_png(p, W, H, shot);
    ann = (uint8_t *)malloc((size_t)W * H * 3);
    build_disp(ann, 1);
    snprintf(p, sizeof p, "%s/annotated.png", dir);
    viewer_write_png(p, W, H, ann);
    free(ann);
    idbuffer_png(dir);
    json = report_json(name);
    if (json) write_text(dir, "report.json", json);
    free(json);
    tail = rt_log_tail(300);
    if (tail) write_text(dir, "log_tail.txt", tail);
    free(tail);
    inp = rt_pick_input_script(NULL);
    if (inp) write_text(dir, "input.txt", inp);
    free(inp);
    snprintf(p, sizeof p, "%s/clip.gif", dir);
    write_gif(p);
    rt_log("bug report saved: %s (note: %d characters, %d objects picked)", rt_log_path(dir), (int)strlen(note), nsel);
    fprintf(stderr, "bug report saved: %s\n", dir);
}

/* ------------------------------------------------------------ state machine */
static const char *sc_clicks, *sc_note;
static int sc_at = -1, sc_done;
static void ui_script_start(void);
static void ui_script_step(void);

static void sc_init(void)
{
    static int inited;
    const char *e;
    if (inited)
        return;
    inited = 1;
    if ((e = getenv("RT_PICK_AT")) != NULL)
        sc_at = atoi(e);
    sc_clicks = getenv("RT_PICK_CLICKS");
    sc_note = getenv("RT_PICK_NOTE");
}

int pick_frozen(void) { return state == PS_FROZEN; }
int pick_busy(void) { return state != PS_OFF; }

void pick_arm(void)
{
    if (state != PS_OFF)
        return;
    state = PS_ARMED;
    audio_pause(1);
    rt_log("bug report: game frozen (F8)");
}

static void end_session(void)
{
    state = PS_OFF;
    audio_pause(0);
    SDL_StopTextInput();
    SDL_ShowCursor(SDL_DISABLE);
    gfx_set_render_state(GFX_RS_ALPHA_FUNC, 4);
    gfx_set_render_state(GFX_RS_BLEND, 1);
    gfx_set_render_state(GFX_RS_TEX_CLAMP, 0);
    if (dtex) { gfx_release_texture(dtex); dtex = NULL; }
    free(shot); shot = NULL;
    free(disp); disp = NULL;
    free(idbuf); idbuf = NULL;
    free(depth); depth = NULL;
    free(grp); grp = NULL;
    nsel = nmarks = 0;
    note[0] = 0;
}

int pick_hold_ticks(int ticks)
{
    sc_init();
    if (state == PS_OFF && sc_at >= 0 && !sc_done && ticks >= sc_at) {
        sc_done = 1;
        manual = 0;
        pick_arm();
    }
    return state != PS_OFF;
}

/* a click / box from the RT_PICK_CLICKS script: "x,y;x0,y0,x1,y1;..." */
static void run_script(void)
{
    const char *s = sc_clicks;
    while (s && *s) {
        int a, b, c, d, n = 0;
        if (sscanf(s, "%d,%d,%d,%d%n", &a, &b, &c, &d, &n) == 4)
            add_box(a, b, c, d);
        else if (sscanf(s, "%d,%d%n", &a, &b, &n) == 2)
            add_click(a, b);
        else
            break;
        s += n;
        if (*s == ';')
            s++;
    }
    snprintf(note, sizeof note, "%s", sc_note ? sc_note : "scripted test report");
}

int pick_frame_hook(void)
{
    switch (state) {
    case PS_ARMED: {
        gfx_size(&W, &H);
        free(shot);
        shot = (uint8_t *)malloc((size_t)W * H * 3);
        if (!shot || gfx_read_pixels(shot)) {
            end_session();
            return 0;
        }
        pick_pass_begin();
        gfx_pick_pass = 1;
        state = PS_PASS;
        return 1;                       /* draw the frame again for the id buffer */
    }
    case PS_PASS: {
        uint8_t *rgb = (uint8_t *)malloc((size_t)W * H * 3);
        size_t i, n = (size_t)W * H;
        gfx_pick_pass = 0;
        gfx_pick_cb = NULL;
        if (!rgb || gfx_read_pixels(rgb)) {
            free(rgb);
            end_session();
            return 0;
        }
        free(idbuf);
        idbuf = (uint32_t *)malloc(n * sizeof *idbuf);
        for (i = 0; i < n; i++)
            idbuf[i] = rgb[3 * i] | (uint32_t)rgb[3 * i + 1] << 8 | (uint32_t)rgb[3 * i + 2] << 16;
        free(rgb);
        free(depth);
        depth = (float *)malloc(n * sizeof *depth);
        gfx_read_depth(depth);
        gfx_pick_matrices(view, proj);
        disp = (uint8_t *)malloc(n * 3);
        nsel = nmarks = 0;
        note[0] = 0;
        rebuild_groups();
        rt_log("bug report: id pass done (%d draw calls tagged)", pick_count());
        if (!manual && !getenv("RT_PICK_UI")) {   /* scripted (RT_PICK_*): picks, note, save, resume */
            run_script();
            save_report();
            end_session();
            return 0;
        }
        manual = 1;
        state = PS_FROZEN;
        ui_script_start();
        SDL_ShowCursor(SDL_ENABLE);
        SDL_StartTextInput();
        frozen_ms0 = SDL_GetTicks();
        dirty = 1;
        pick_frozen_frame();            /* the back buffer holds the id pass: show the frozen frame */
        return 0;
    }
    default:
        return 0;
    }
}

/* RT_PICK_UI=1 (with RT_PICK_AT / _CLICKS / _NOTE): the same picks, but as SDL events pushed one per frozen frame, so
 * the real UI path (event handling, highlight, panel, saving on Enter) runs headless. RT_PICK_UI_SHOT=file.png writes the
 * frozen frame with the panel just before Enter. */
static SDL_Event q[64];
static int qn, qi, qwait;
static void qpush_mouse(int type, int x, int y, int btn)
{
    SDL_Event e;
    memset(&e, 0, sizeof e);
    e.type = (Uint32)type;
    if (type == SDL_MOUSEMOTION) { e.motion.x = x; e.motion.y = y; }
    else { e.button.x = x; e.button.y = y; e.button.button = (Uint8)btn; }
    if (qn < 64) q[qn++] = e;
}
static void ui_script_start(void)
{
    const char *s = sc_clicks;
    SDL_Event e;
    qn = qi = 0;
    qwait = 2;
    if (!getenv("RT_PICK_UI"))
        return;
    while (s && *s) {
        int a, b, c, d, n = 0;
        if (sscanf(s, "%d,%d,%d,%d%n", &a, &b, &c, &d, &n) == 4) {
            qpush_mouse(SDL_MOUSEBUTTONDOWN, a, b, SDL_BUTTON_LEFT);
            qpush_mouse(SDL_MOUSEMOTION, (a + c) / 2, (b + d) / 2, 0);
            qpush_mouse(SDL_MOUSEBUTTONUP, c, d, SDL_BUTTON_LEFT);
        } else if (sscanf(s, "%d,%d%n", &a, &b, &n) == 2) {
            qpush_mouse(SDL_MOUSEBUTTONDOWN, a, b, SDL_BUTTON_LEFT);
            qpush_mouse(SDL_MOUSEBUTTONUP, a, b, SDL_BUTTON_LEFT);
        } else
            break;
        s += n;
        if (*s == ';')
            s++;
    }
    memset(&e, 0, sizeof e);
    e.type = SDL_TEXTINPUT;
    snprintf(e.text.text, sizeof e.text.text, "%.30s", sc_note ? sc_note : "ui test");
    if (qn < 64) q[qn++] = e;
}
static void ui_script_step(void)
{
    if (!qn || qi > qn)
        return;
    if (qwait-- > 0)
        return;
    qwait = 1;
    if (qi < qn) {
        pick_event(&q[qi++]);
    } else {
        SDL_Event e;
        const char *shotp = getenv("RT_PICK_UI_SHOT");
        if (shotp) {
            uint8_t *rgb = (uint8_t *)malloc((size_t)W * H * 3);
            if (rgb && !gfx_read_pixels(rgb))
                viewer_write_png(shotp, W, H, rgb);
            free(rgb);
        }
        memset(&e, 0, sizeof e);
        e.type = SDL_KEYDOWN;
        e.key.keysym.sym = SDLK_RETURN;
        qi++;
        pick_event(&e);
    }
}

unsigned pick_frozen_ms(void) { return frozen_ms0 ? SDL_GetTicks() - frozen_ms0 : 0; }

void pick_frozen_frame(void)
{
    char line[260];
    int sc = W >= 900 ? 2 : 1, cw = 6 * sc, lh = 9 * sc, y, k, px0 = 8, pw, maxc, rows;
    if (state != PS_FROZEN)
        return;
    if (dirty)
        upload_disp();
    gfx_begin_frame(0);
    gfx_set_render_state(GFX_RS_ALPHA_FUNC, 7);
    gfx_set_render_state(GFX_RS_FOG_ENABLE, 0);
    gfx_set_render_state(GFX_RS_FADE_COLOR, 0xFFFFFFFFu);
    gfx_set_render_state(GFX_RS_BLEND, 0);
    gfx_set_render_state(GFX_RS_FILTER, 0);
    gfx_set_render_state(GFX_RS_TEX_CLAMP, 1);
    gfx_set_render_state(GFX_RS_TEXTURE, (uintptr_t)dtex);
    {
        float pos[12] = { 0, 0, (float)W, 0, 0, (float)H, (float)W, 0, (float)W, (float)H, 0, (float)H };
        float st[12] = { 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1 };
        gfx_draw_2d(W, H, 6, pos, st, NULL);
    }
    gfx_set_render_state(GFX_RS_TEXTURE, 0);
    gfx_set_render_state(GFX_RS_BLEND, 1);
    /* marks */
    for (k = 0; k < nmarks; k++) {
        mark_t *m = &marks[k];
        if (m->type == 0) {
            ui_quad((float)(m->x0 - 9), (float)(m->y0 - 1), (float)(m->x0 + 10), (float)(m->y0 + 2), 0xFF0000FFu);
            ui_quad((float)(m->x0 - 1), (float)(m->y0 - 9), (float)(m->x0 + 2), (float)(m->y0 + 10), 0xFF0000FFu);
        } else {
            ui_quad((float)m->x0, (float)m->y0, (float)m->x1, (float)(m->y0 + 2), 0xFF0000FFu);
            ui_quad((float)m->x0, (float)(m->y1 - 1), (float)m->x1, (float)(m->y1 + 1), 0xFF0000FFu);
            ui_quad((float)m->x0, (float)m->y0, (float)(m->x0 + 2), (float)m->y1, 0xFF0000FFu);
            ui_quad((float)(m->x1 - 1), (float)m->y0, (float)(m->x1 + 1), (float)m->y1, 0xFF0000FFu);
        }
        snprintf(line, sizeof line, "%d", k + 1);
        ui_text(m->x0 + 6, m->y0 + 4, line, sc, 0xFFFFFFFFu);
    }
    if (drag)
        ui_quad((float)(dx0 < mx ? dx0 : mx), (float)(dy0 < my ? dy0 : my), (float)(dx0 < mx ? mx : dx0) + 1, (float)(dy0 < my ? my : dy0) + 1, 0xFFFF0060u);
    /* panel */
    pw = W - 2 * px0 > 100 * cw ? 100 * cw : W - 2 * px0;
    maxc = pw / cw - 2;
    rows = 4 + (nsel < 12 ? nsel : 12) + 3;
    ui_quad((float)px0, 6, (float)(px0 + pw), (float)(6 + rows * lh + 8), 0x000000C8u);
    y = 10;
    ui_text(px0 + 4, y, "BUG REPORT: click = pick, drag = box, right-click = undo", sc, 0xFFFF80FFu);
    y += lh;
    ui_text(px0 + 4, y, "type a note, Enter = save the report, Esc or F8 = cancel", sc, 0xFFFF80FFu);
    y += lh;
    snprintf(line, sizeof line, "%d object(s) picked, %d draw calls on screen", nsel, pick_count());
    ui_text(px0 + 4, y, line, sc, 0xC0C0C0FFu);
    y += lh;
    for (k = 0; k < nsel && k < 12; k++) {
        char d[220];
        const unsigned *c = pal[k % 6];
        pick_describe(pick_entry(sel[k].id), d, sizeof d);
        snprintf(line, sizeof line, "%d. %.*s", k + 1, maxc - 4, d);
        ui_text(px0 + 4, y, line, sc, (c[0] << 24) | (c[1] << 16) | (c[2] << 8) | 0xFF);
        y += lh;
    }
    if (nsel > 12) {
        snprintf(line, sizeof line, "... and %d more (see report.json)", nsel - 12);
        ui_text(px0 + 4, y, line, sc, 0xC0C0C0FFu);
        y += lh;
    }
    y += lh / 2;
    {
        int len = (int)strlen(note), from = len > maxc - 8 ? len - (maxc - 8) : 0;
        char tmp[300];
        snprintf(tmp, sizeof tmp, "Note: %.*s%s", maxc - 7, note + from, (SDL_GetTicks() / 400) & 1 ? "_" : " ");
        ui_text(px0 + 4, y, tmp, sc, 0xFFFFFFFFu);
    }
    ui_flush();
    ui_script_step();
}

static void cancel(void)
{
    rt_log("bug report: cancelled");
    end_session();
}

static void utf8_pop(char *s)
{
    size_t l = strlen(s);
    if (!l)
        return;
    l--;
    while (l > 0 && ((unsigned char)s[l] & 0xC0) == 0x80)
        l--;
    s[l] = 0;
}

/* one SDL event: returns 1 when the reporter consumed it */
int pick_event(const void *vev)
{
    const SDL_Event *ev = (const SDL_Event *)vev;
    if (ev->type == SDL_KEYDOWN && ev->key.keysym.sym == SDLK_F8 && !ev->key.repeat) {
        if (state == PS_OFF) {
            manual = 1;
            pick_arm();
        } else if (state == PS_FROZEN)
            cancel();
        return 1;
    }
    if (state == PS_OFF)
        return 0;
    if (ev->type == SDL_QUIT)
        return 0;
    if (state != PS_FROZEN)
        return 1;
    switch (ev->type) {
    case SDL_KEYDOWN:
        if (ev->key.keysym.sym == SDLK_ESCAPE)
            cancel();
        else if (ev->key.keysym.sym == SDLK_RETURN || ev->key.keysym.sym == SDLK_KP_ENTER) {
            save_report();
            end_session();
        } else if (ev->key.keysym.sym == SDLK_BACKSPACE)
            utf8_pop(note);
        else if (ev->key.keysym.sym == SDLK_v && (ev->key.keysym.mod & KMOD_CTRL)) {
            char *c = SDL_GetClipboardText();
            if (c) {
                size_t i;
                for (i = 0; c[i]; i++)
                    if (c[i] == '\n' || c[i] == '\r' || c[i] == '\t')
                        c[i] = ' ';
                strncat(note, c, sizeof note - strlen(note) - 1);
                SDL_free(c);
            }
        }
        break;
    case SDL_TEXTINPUT:
        if (strlen(note) + strlen(ev->text.text) < sizeof note - 1)
            strcat(note, ev->text.text);
        break;
    case SDL_MOUSEBUTTONDOWN:
        if (ev->button.button == SDL_BUTTON_LEFT) {
            drag = 1;
            dx0 = mx = ev->button.x;
            dy0 = my = ev->button.y;
        } else if (ev->button.button == SDL_BUTTON_RIGHT)
            undo();
        break;
    case SDL_MOUSEMOTION:
        mx = ev->motion.x;
        my = ev->motion.y;
        break;
    case SDL_MOUSEBUTTONUP:
        if (ev->button.button == SDL_BUTTON_LEFT && drag) {
            drag = 0;
            mx = ev->button.x;
            my = ev->button.y;
            if (abs(mx - dx0) < 5 && abs(my - dy0) < 5)
                add_click(dx0, dy0);
            else
                add_box(dx0, dy0, mx, my);
        }
        break;
    }
    return 1;
}

/* the controller combo, polled once per drawn frame */
void pick_poll_pad(void)
{
    static int was;
    int now = pad_combo_held();
    if (now && !was && state == PS_OFF) {
        manual = 1;
        pick_arm();
    }
    was = now;
}
