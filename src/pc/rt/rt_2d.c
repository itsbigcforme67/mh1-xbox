/*
 * rt_2d.c - the fl library's screen primitives and textures for the game's
 * 2D code (HUD "pit", menus, info banner, result screens), on src/pc/gfx.
 *
 * Read from the asm (main f_flps0 0x175290-0x176470, flPS2Conv*):
 *   flps0002 line      { s16 x0, y0, x1, y1; u32 col }
 *   flps0004 rect      { s16 x0, y0, x1, y1; u32 col }          (corners)
 *   flps0005 gradient  { s16 x0, y0, x1, y1; u32 c[4] }  TL, TR, BL, BR
 *   flps0008 sprite    { s16 x, y, w, h; u32 col; s16 u0, v0, u1, v1 }
 *   flps0009 triangle  { s16 x0, y0, x1, y1, x2, y2; u32 col }
 *   flps000C tex. tri  { s16 x0, y0, x1, y1, x2, y2; u32 col; s16 uv[6] }
 * Colours are 0xAARRGGBB with 0xFF = 1.0 (flPS2ConvColor halves to the
 * GS 0x80 scale). UVs are texels of the current texture (flSetRenderState
 * 4), clamped to its size (flPS2CheckUV). Screen coordinates are the PS2
 * frame, 512 x 448 (menu code scales its 640-wide layout by 0.8).
 *
 * Textures: flCreateTextureFromApx_mem returns a host handle (index into
 * a table here) stored in mem_tex[] like the PS2 handle; the game's
 * SetTextureStage(n) passes mem_tex[n] to flSetRenderState(4, ...).
 */
#include "rt.h"
#include "types.h"
#include "../fmt/fmt.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SCR_W 512
#define SCR_H 448

/* ------------------------------------------------------------ textures */
#define TEX_MAX 1024
static struct { gfx_texture *t; int w, h; } tex[TEX_MAX];
static int ntex = 1;
static int cur_tex;

/* flCreateTextureFromApx_mem (0x16FA00): APX in memory -> handle */
void *flCreateTextureFromApx_mem(void *p, int type)
{
    apx_image img;
    fmt_blob b;
    (void)type;
    b.p = p;
    b.n = *(u32 *)p;            /* +0: total size */
    if (ntex >= TEX_MAX || fmt_apx_decode(&img, b, FMT_LE) != 0) {
        if (getenv("RT_TRACE"))
            fprintf(stderr, "rt_2d: APX not decoded (size %u)\n", (unsigned)b.n);
        return 0;
    }
    if (getenv("RT_TRACE"))
        fprintf(stderr, "rt_2d: texture %d: %dx%d\n", ntex, img.w, img.h);
    if (getenv("RT_TEX_DUMP")) {        /* raw RGBA of each texture, for checking */
        char fn[512];
        FILE *f;
        snprintf(fn, sizeof fn, "%s/tex%03d_%dx%d.rgba", getenv("RT_TEX_DUMP"), ntex, img.w, img.h);
        if ((f = fopen(fn, "wb"))) {
            fwrite(img.rgba, 4, (size_t)img.w * img.h, f);
            fclose(f);
        }
    }
    tex[ntex].t = gfx_create_texture(img.w, img.h, img.rgba);
    tex[ntex].w = img.w;
    tex[ntex].h = img.h;
    free(img.rgba);
    return (void *)(uintptr_t)ntex++;
}

/* flSetRenderState(4, handle): the current texture (rt_fl.c routes it) */
void rt_2d_set_texture(u32 h)
{
    if (getenv("RT_TEX_TRACE"))
        fprintf(stderr, "rt_2d: texture state %X\n", h);
    cur_tex = (h & 0xFFFF) < (u32)ntex ? (int)(h & 0xFFFF) : 0;
    gfx_set_render_state(GFX_RS_TEXTURE, (uintptr_t)tex[cur_tex].t);
}

/* back to the current fl texture after host draws that bound their own
 * (font glyphs): the game caches the texture stage (SetTextureStage only
 * calls flSetRenderState on a change) */
void rt_2d_restore_texture(void)
{
    gfx_set_render_state(GFX_RS_TEXTURE, (uintptr_t)tex[cur_tex].t);
}

/* reload_tex: textures stay resident on the PC */
void flReloadTexture(int n, void *p) { (void)n; (void)p; }

/* ------------------------------------------------------------ the loads
 * data_load_ptr (rt_motion.c) is where the PS2 loads files before
 * converting them; a host buffer here. */
extern u8 *data_load_ptr;
void rt_2d_init(void)
{
    if (!data_load_ptr)
        data_load_ptr = malloc(4u << 20);
}

extern u8 game_w[];
extern u32 mem_tex[];
extern s32 PIT_TEX[];
void mkmapTexture(int a, int stage, int idx, int type);
int load_file_mdl(void *dst, int id);
extern int f_type[2];
extern s16 filedef_sys[];

/* load_texlist / mkTexture (main g_load_texlist, src/main/load/loadf_nm.c;
 * the same here, without that file's PS2 file loader) */
int load_texlist(int id, int base, int type)
{
    u8 *p = data_load_ptr;
    int i, n;
    load_file_mdl(p, id);
    n = *(int *)p;
    if (n > 0x32)
        n = 0x32;
    for (i = 0; i < n; i++)
        mem_tex[base + i] = (u32)(uintptr_t)flCreateTextureFromApx_mem(p + *(int *)(p + i * 8 + 4), f_type[type]);
    return n;
}

void mkTexture(int file, int idx, int type)
{
    load_file_mdl(data_load_ptr, filedef_sys[file]);
    mem_tex[idx] = (u32)(uintptr_t)flCreateTextureFromApx_mem(data_load_ptr, f_type[type]);
}
/* load_pit (0x274E10): the cockpit (HUD) textures: PIT_TEX[1] list at
 * mem_tex 0x118.., the stage map at 0x119, filedef_sys 2/8/4 at
 * 0x11A-0x11C (online: 7 at 0x119, 5 at 0x11A) */
void load_pit(void)
{
    static int boot;
    rt_2d_init();
    if (!boot) {        /* loaded once at boot on the PS2: the select overlay's
                         * Init_task (PIT_TEX[0] list at mem_tex 2..) and
                         * all_reset (filedef_sys 5 / 6 at 0x157 / 0x156) */
        boot = 1;
        load_texlist(PIT_TEX[0], 2, 0);
        mkTexture(5, 0x157, 0);
        mkTexture(6, 0x156, 0);
    }
    load_texlist(PIT_TEX[1], 0x118, 0);
    if (game_w[0x1DC] == 0) {
        mkmapTexture(game_w[0x2E], *(u16 *)(game_w + 0x2C), 0x119, 0);
        mkTexture(2, 0x11A, 0);
    } else {
        mkTexture(7, 0x119, 0);
        mkTexture(5, 0x11A, 0);
    }
    mkTexture(8, 0x11B, 0);
    mkTexture(4, 0x11C, 0);
}

/* ------------------------------------------------------------ prims */
static void rgba(u8 *o, u32 c)
{
    o[0] = (u8)(c >> 16);
    o[1] = (u8)(c >> 8);
    o[2] = (u8)c;
    o[3] = (u8)(c >> 24);
}

static void quad(float x0, float y0, float x1, float y1, const u32 c[4], const float *uv)
{
    float pos[12] = { x0, y0, x1, y0, x0, y1, x1, y0, x1, y1, x0, y1 };
    float st[12];
    u8 col[24];
    static const int vi[6] = { 0, 1, 2, 1, 3, 2 };   /* corners TL TR BL BR */
    int i;
    for (i = 0; i < 6; i++)
        rgba(col + 4 * i, c[vi[i]]);
    if (uv) {
        float u0 = uv[0], v0 = uv[1], u1 = uv[2], v1 = uv[3];
        float s[12] = { u0, v0, u1, v0, u0, v1, u1, v0, u1, v1, u0, v1 };
        memcpy(st, s, sizeof st);
    }
    gfx_draw_2d(SCR_W, SCR_H, 6, pos, uv ? st : NULL, col);
}

static float tu(int u) { return tex[cur_tex].w ? (float)(u > tex[cur_tex].w ? tex[cur_tex].w : u) / tex[cur_tex].w : 0; }
static float tv(int v) { return tex[cur_tex].h ? (float)(v > tex[cur_tex].h ? tex[cur_tex].h : v) / tex[cur_tex].h : 0; }

void flps0002(s16 *q)
{
    u32 c = *(u32 *)(q + 4), cc[4] = { c, c, c, c };
    float dx = q[2] - q[0], dy = q[3] - q[1], l = sqrtf(dx * dx + dy * dy);
    float nx, ny;
    float pos[12];
    u8 col[24];
    int i;
    if (l < 0.01f) {
        quad(q[0], q[1], q[0] + 1, q[1] + 1, cc, NULL);
        return;
    }
    nx = -dy / l * 0.5f;
    ny = dx / l * 0.5f;
    {
        float p[12] = { q[0] + nx, q[1] + ny, q[2] + nx, q[3] + ny, q[0] - nx, q[1] - ny,
                        q[2] + nx, q[3] + ny, q[2] - nx, q[3] - ny, q[0] - nx, q[1] - ny };
        memcpy(pos, p, sizeof pos);
    }
    for (i = 0; i < 6; i++)
        rgba(col + 4 * i, c);
    gfx_draw_2d(SCR_W, SCR_H, 6, pos, NULL, col);
}

void flps0004(s16 *q)
{
    u32 c = *(u32 *)(q + 4), cc[4] = { c, c, c, c };
    quad(q[0], q[1], q[2], q[3], cc, NULL);
}

void flps0005(s16 *q)
{
    quad(q[0], q[1], q[2], q[3], (u32 *)(q + 4), NULL);
}

void flps0008(s16 *q)
{
    u32 c = *(u32 *)(q + 4), cc[4] = { c, c, c, c };
    float uv[4];
    uv[0] = tu(q[6]);
    uv[1] = tv(q[7]);
    uv[2] = tu(q[8]);
    uv[3] = tv(q[9]);
    quad(q[0], q[1], q[0] + q[2], q[1] + q[3], cc, cur_tex ? uv : NULL);
}

void flps0009(s16 *q)
{
    u32 c = *(u32 *)(q + 6);
    float pos[6] = { q[0], q[1], q[2], q[3], q[4], q[5] };
    u8 col[12];
    int i;
    for (i = 0; i < 3; i++)
        rgba(col + 4 * i, c);
    gfx_draw_2d(SCR_W, SCR_H, 3, pos, NULL, col);
}

void flps000C(s16 *q)
{
    u32 c = *(u32 *)(q + 6);
    s16 *uv = q + 8;
    float pos[6] = { q[0], q[1], q[2], q[3], q[4], q[5] };
    float st[6] = { tu(uv[0]), tv(uv[1]), tu(uv[2]), tv(uv[3]), tu(uv[4]), tv(uv[5]) };
    u8 col[12];
    int i;
    for (i = 0; i < 3; i++)
        rgba(col + 4 * i, c);
    gfx_draw_2d(SCR_W, SCR_H, 3, pos, cur_tex ? st : NULL, col);
}

/* flps0D00 (0x176470): screen sprite of the sprite list (SpritePut kind 0,
 * trans_spr_sub) { f32 x0, y0, z, w; f32 x1, y1, z, w; u32 col }: a GS
 * SPRITE between the two corners in frame coordinates (flPS2ConvScreenFX/
 * FY: minus the screen offset, y halved in field mode), one colour */
void flps0D00(f32 *q)
{
    u32 c = *(u32 *)(q + 8), cc[4] = { c, c, c, c };
    quad(q[0], q[1], q[4], q[5], cc, NULL);
}

/* other primitive kinds (sprite list types 0x0D, 0x0F, 0x13, 0x14, 0x16):
 * not used by the HUD; reported once */
#define PRIM_TODO(n) void n(void *q) { static int o; (void)q; if (!o++ && getenv("RT_TRACE")) fprintf(stderr, "rt_2d: %s not ported\n", #n); }
PRIM_TODO(flps0F00) PRIM_TODO(flps1300) PRIM_TODO(flps1400) PRIM_TODO(flps1600)

/* ------------------------------------------------------------ prim lists */
typedef struct { u8 d[0x20]; } PIT_PRIM_H;
extern PIT_PRIM_H pit_prim[4];
/* pit_prim_init (0x1692F0) */
void pit_prim_init(void) { memset(pit_prim, 0, 4 * sizeof pit_prim[0]); }

/* ------------------------------------------------------------ maths */
/* flSinCos (0x173620) */
void flSinCos(f32 a, f32 *s, f32 *c)
{
    *s = sinf(a);
    *c = cosf(a);
}
/* flmatSetZYX33 (0x172310): rotation part of m from three angles [order a
 * guess; the game's calls only turn about Z] */
void flmatRotX33(void *m, f32 a);
void flmatRotY33(void *m, f32 a);
void flmatRotZ33(void *m, f32 a);
void flmatSetZYX33(f32 x, f32 y, f32 z, f32 *m)
{
    int i;
    for (i = 0; i < 16; i++)
        m[i] = (i % 5 == 0) ? 1.0f : 0.0f;
    flmatRotZ33(m, z);
    flmatRotY33(m, y);
    flmatRotX33(m, x);
}

/* flvecrRotTransPers (0x1734D0): world point -> screen (map markers);
 * flmatrStore (0x173450). Not ported yet. */
void flvecrRotTransPers(f32 *out, f32 *in) { (void)in; out[0] = out[1] = out[2] = 0; }
void flmatrStore(void *m) { (void)m; }
