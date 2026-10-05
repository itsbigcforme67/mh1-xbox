/*
 * gfx.h - the port's thin graphics interface.
 *
 * Deliberately small and fixed-function friendly: the original Xbox GPU
 * (NV2A) is roughly DirectX 8 class, so everything here maps onto plain
 * vertex arrays, one texture stage, alpha test/blend, z test/write and
 * fog. Lighting and skinning are done on the CPU by the fl layer
 * (src/pc/fl/), as the PS2 did them on VU1, so a clay only carries
 * position, a pre-lit colour and texture coordinates.
 *
 * Names follow Capcom's PS2 "fl" library (docs/formats/graphics.md):
 * a "clay" is one converted model part (flCreateClayHandle /
 * flExecuteClay), and gfx_set_render_state() takes flSetRenderState's
 * state numbers where they are known, so the game's own calls can later
 * be routed here. States without an fl equivalent use numbers >= 0x100.
 */
#ifndef MH_GFX_H
#define MH_GFX_H

#include <stdint.h>

typedef struct gfx_texture gfx_texture;
typedef struct gfx_clay gfx_clay;

/* ------------------------------------------------------------ device */
int  gfx_init(int width, int height, const char *title, int hidden);
void gfx_shutdown(void);
void gfx_size(int *w, int *h);
void gfx_begin_frame(uint32_t clear_rgb);       /* 0xRRGGBB */
void gfx_end_frame(void);                        /* present */
/* Read the back buffer as top-down RGB (w*h*3 bytes). */
int  gfx_read_pixels(uint8_t *rgb);

/* ------------------------------------------------------------ textures */
gfx_texture *gfx_create_texture(int w, int h, const uint8_t *rgba);
void gfx_release_texture(gfx_texture *t);

/* ------------------------------------------------------------ render state */
enum {
    /* fl numbers (flSetRenderState 0x177720, graphics.md section 5) */
    GFX_RS_TEXTURE      = 0x04,  /* value: gfx_texture* (NULL = untextured) */
    GFX_RS_BLEND_OP     = 0x0D,  /* value: fl RenderOperation 0xC00 bits: 0 add (src + dst),
                                    0x400 subtract (src - dst), 0x800 reverse (dst - src) [guess] */
    GFX_RS_FOG_COLOR    = 0x0F,  /* value: 0xRRGGBB */
    GFX_RS_FOG_START    = 0x10,  /* value: float* */
    GFX_RS_FOG_END      = 0x11,  /* value: float* */
    GFX_RS_FOG_ENABLE   = 0x12,  /* value: 0 off, else on (fl: fog type) */
    GFX_RS_VIEW         = 0x17,  /* value: const float[16], row vectors */
    GFX_RS_TEXMAT       = 0x19,  /* value: const float[16] applied to (s,t,0,1); NULL = identity */
    GFX_RS_WORLD        = 0x1A,  /* value: const float[16] (flMATRIX[0]) */
    GFX_RS_BLEND_FUNC   = 0x5E,  /* value: fl blend factors, src | dst << 4, each
                                    GFX_BF_*; also turns blending on */
    GFX_RS_ALPHA_REF    = 0x60,  /* value: 0-255, alpha test GREATER ref */
    GFX_RS_FILTER       = 0x63,  /* value: 0 bilinear, 0x10000 point (fl/GS TEX1) */
    GFX_RS_TEX_CLAMP    = 0x64,  /* value: 0 repeat, else clamp (fl/GS CLAMP: 0x20000, 0x40000) */
    GFX_RS_FADE_COLOR   = 0x67,  /* value: 0xAARRGGBB multiplied into every vertex */
    GFX_RS_ZWRITE       = 0x6C,  /* value: 0/1 */
    /* port-only states */
    GFX_RS_PROJECTION   = 0x100, /* value: const float[16] */
    GFX_RS_BLEND        = 0x101, /* value: 0 off, 1 src-alpha/inv-src-alpha */
    GFX_RS_ZTEST        = 0x102  /* value: 0/1 */
};
/* Blend factor codes of fl state 0x5E, read from flPS2SendRenderState_ALPHA
 * (graphics.md 5a). The GS can only blend Cs and Cd with As, Ad or a fixed
 * value, so 6-9 (colour factors) have no GS form and are drawn as 0/1. */
enum { GFX_BF_ZERO, GFX_BF_ONE, GFX_BF_SRC_ALPHA, GFX_BF_INV_SRC_ALPHA,
       GFX_BF_DST_ALPHA, GFX_BF_INV_DST_ALPHA };
void gfx_set_render_state(int state, uintptr_t value);
/* convenience for float-valued states */
void gfx_set_render_state_f(int state, float value);

/* ------------------------------------------------------------ clays */
typedef struct {
    int first, count;          /* range in the index list (triangles) */
    gfx_texture *tex;          /* texture for this range (material) */
} gfx_batch;

typedef struct {
    int nvert;
    const float *pos;          /* 3 floats per vertex */
    const float *st;           /* 2 floats per vertex, or NULL */
    const uint8_t *col;        /* RGBA8 per vertex (pre-lit), or NULL = white */
    int nindex;
    const uint16_t *index;     /* triangle list */
    int nbatch;
    const gfx_batch *batch;
    int dynamic;               /* positions/colours change every frame */
} gfx_clay_desc;

/* Immediate 2D triangle list (screen prims of the game's menus / HUD):
 * nvert vertices, pos 2 floats in a virtual screen of w x h (origin top
 * left; the PS2 frame is 512 x 448) stretched over the window, st 2 floats
 * in 0..1 (or NULL: untextured, else the current GFX_RS_TEXTURE), col RGBA8
 * (or NULL = white). No depth test or write; blend/filter/clamp/alpha
 * states apply. Matrices are left as they were. */
void gfx_draw_2d(int w, int h, int nvert, const float *pos, const float *st, const uint8_t *col);

gfx_clay *gfx_create_clay(const gfx_clay_desc *d);
/* Replace positions and/or colours (either may be NULL). */
void gfx_update_clay(gfx_clay *c, const float *pos, const uint8_t *col);
/* Draw with the current WORLD/VIEW/PROJECTION and render states. */
void gfx_execute_clay(gfx_clay *c);
void gfx_release_clay(gfx_clay *c);

#endif
