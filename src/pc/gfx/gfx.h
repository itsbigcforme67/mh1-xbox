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
extern int gfx_tex_src_hint;   /* memory report only: the next texture's size as stored on the disc (4/8-bit + CLUT) */
void gfx_release_texture(gfx_texture *t);
/* Video frames (the movies): planar 4:2:0 YUV, BT.601 studio range. A
 * backend that converts on the GPU (gfx_yuv_capable() 1: gfx_nv2a.c, a
 * YUY2 texture the NV2A samples as RGB) keeps one texture and updates it
 * per frame; the others get RGBA from the caller (gfx_create_texture). */
int  gfx_yuv_capable(void);
gfx_texture *gfx_create_texture_yuv(int w, int h);
void gfx_update_texture_yuv(gfx_texture *t, const uint8_t *y, const uint8_t *u, const uint8_t *v);
/* gfx_pal.c: <= 256 colours -> palette (RGBA words) + indices (idx may be
 * NULL); returns the colour count or -1. And the bytes the Xbox backend
 * keeps for such a texture (P8 + palette when it can, else RGBA8). */
int  gfx_to_indexed(const uint32_t *src, int n, uint32_t *pal, uint8_t *idx);
long gfx_xbox_texture_bytes(int w, int h, const uint8_t *rgba);
int  gfx_clip_tri(const float w[3], float cw, int a[4], int b[4], float t[4]);

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
    GFX_RS_ALPHA_FUNC   = 0x5F,  /* value: the game's alpha-test compare 0-7 (flrs07): 0 never, 1 less, 2 equal, 3 lequal, 4 greater (normal), 5 notequal, 6 gequal, 7 always */
    GFX_RS_ALPHA_REF    = 0x60,  /* value: 0-255, alpha test GREATER ref */
    GFX_RS_FILTER       = 0x63,  /* value: 0 bilinear, 0x10000 point (fl/GS TEX1) */
    GFX_RS_TEX_CLAMP    = 0x64,  /* value: 0 repeat, else clamp (fl/GS CLAMP: 0x20000, 0x40000) */
    GFX_RS_FADE_COLOR   = 0x67,  /* value: 0xAARRGGBB multiplied into every vertex */
    GFX_RS_ZWRITE       = 0x6C,  /* value: 0/1 */
    GFX_RS_ZFUNC        = 0x6D,  /* value: the game's GS ZTST: 1 greater (= GL less), 3 gequal (normal, = GL lequal), 7 always; others never */
    /* port-only states */
    GFX_RS_PROJECTION   = 0x100, /* value: const float[16] */
    GFX_RS_BLEND        = 0x101, /* value: 0 off, 1 src-alpha/inv-src-alpha */
    GFX_RS_ZTEST        = 0x102, /* value: 0/1 */
    GFX_RS_BATCH_HIDE   = 0x103, /* value: bit b set = batch b (< 32) of the next clays is not drawn; 0 = all drawn.
                                    The PS2 hides a material by writing 0 to its alpha (em_material_sub 0x10CEA0) */
    GFX_RS_BATCH_TEX    = 0x104  /* value: gfx_texture* drawn instead of every batch's own texture (a material's texture
                                    swapped, em_material_sub kind 2); 0 = the batches' own */
};
/* Blend factor codes of fl state 0x5E, read from flPS2SendRenderState_ALPHA
 * (graphics.md 5a). The GS can only blend Cs and Cd with As, Ad or a fixed
 * value, so 6-9 (colour factors) have no GS form and are drawn as 0/1. */
enum { GFX_BF_ZERO, GFX_BF_ONE, GFX_BF_SRC_ALPHA, GFX_BF_INV_SRC_ALPHA,
       GFX_BF_DST_ALPHA, GFX_BF_INV_DST_ALPHA };
void gfx_set_render_state(int state, uintptr_t value);
/* convenience for float-valued states */
void gfx_set_render_state_f(int state, float value);

/* ------------------------------------------------------------ picking (in-game bug reporter)
 * While gfx_pick_pass is set (the frozen frame is drawn once more, rt_pick.c / pick.c) the backend does not
 * draw colours: every clay / 2D draw asks gfx_pick_cb for an id (it records the draw's render states from
 * this struct and the current tag) and draws flat in that id's colour, so a click can be turned back into
 * the draw call under it. Only the GL backend does this; other backends ignore it. */
typedef struct {
    int is2d, nvert;
    unsigned tex;                 /* GL texture name (0 = untextured), tex_w x tex_h */
    int tex_w, tex_h;
    int blend_on, bsrc, bdst, bop;  /* GFX_BF_* codes, fl operation */
    int ztest, zwrite, zfunc, afunc; /* afunc: GL compare - 0x200 (0 never .. 7 always) */
    float aref;
    int nearest, clamp, fog, noscroll;
    uint32_t fade;
    float texmat[16], world[16];
    float bbox2d[4];              /* 2D draws: x0 y0 x1 y1 in the virtual screen (w x h below) */
    int sw, sh;
    const void *clay;
} gfx_pick_info;
extern int gfx_pick_pass;
extern uint32_t (*gfx_pick_cb)(const gfx_pick_info *);
void gfx_pick_matrices(float view[16], float proj[16]);
int gfx_read_depth(float *d);                              /* GL backend: the depth buffer, top-down, window size */   /* GL backend: the view / projection of the last draw */

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
    int noscroll;              /* the part's attribute has no UV scroll (+0x1C = 0): fl state 0x19 does not move its texture */
} gfx_clay_desc;

/* Immediate 2D triangle list (screen prims of the game's menus / HUD):
 * nvert vertices, pos 2 floats in a virtual screen of w x h (origin top
 * left; the PS2 frame is 512 x 448) stretched over the window, st 2 floats
 * in 0..1 (or NULL: untextured, else the current GFX_RS_TEXTURE), col RGBA8
 * (or NULL = white). No depth test or write; blend/filter/clamp/alpha
 * states apply. Matrices are left as they were. */
void gfx_draw_2d(int w, int h, int nvert, const float *pos, const float *st, const uint8_t *col);

/* Recording (gfx_rec.c): between gfx_rec_begin and gfx_rec_end the draw
 * calls below are recorded instead of drawn; gfx_rec_replay draws the last
 * finished recording (game code that draws while its tick runs, e.g. the
 * title screen). gfx_rec_call records a host draw callback in order. */
void gfx_rec_begin(void);
void gfx_rec_end(void);
void gfx_rec_clear(void);
int  gfx_rec_have(void);
void gfx_rec_replay(void);
void gfx_rec_call(void (*fn)(void *), void *arg);
/* backend hooks: 1 = recorded, the backend returns at once */
int  gfx_rec_state(int state, uintptr_t v);
int  gfx_rec_2d(int w, int h, int n, const float *pos, const float *st, const uint8_t *col);
int  gfx_rec_clay(gfx_clay *c);

/* ------------------------------------------------------------ GPU skinning
 * Backends that skin and light on the GPU (gfx_nv2a.c: a vertex program
 * with a 24-bone palette) get the bind-pose data once and a pose per
 * frame instead of gfx_update_clay. The GL backend does not
 * (gfx_skin_capable() 0): fl_model keeps skinning on the CPU there.
 * The math is fl_model_pose's: pos = rigid * bind + sum w_k * (bind * M_k)
 * (row vectors, weights as stored, not normalised), the normal likewise,
 * normalised; colour = base * min(ambient + sum max(0, -n.dir) col, 1),
 * times the tint where the tint mask is set; alpha = base alpha. */
#define GFX_SKIN_INFL 4
#define GFX_SKIN_PALETTE 24
typedef struct {
    int nvert;
    const float *nrm;             /* 3 per vertex, or NULL (then the normal is (0,1,0)) */
    const uint8_t *infl_n;        /* per vertex, or NULL = all rigid */
    const int16_t *infl_bone;     /* nvert * GFX_SKIN_INFL */
    const float *infl_w;
    int nbone;                    /* bones the matrices of gfx_clay_pose cover */
    int skinned;                  /* 0: rigid part (lighting only) */
    const uint8_t *tint_mask;     /* per vertex 1 = tinted, or NULL */
} gfx_skin_desc;
typedef struct {
    int lit;                      /* 0: colour = base (times fade) */
    float dir[3][3], col[3][3], ambient[3];
    int tint;                     /* tint the masked vertices */
    float tint_rgb[3];
} gfx_light;
int  gfx_skin_capable(void);
/* after gfx_create_clay (whose desc gave positions, st, base colours,
 * indices); 0 = the backend skins this clay from now on */
int  gfx_clay_set_skin(gfx_clay *c, const gfx_skin_desc *s);
/* this frame's pose: skin[b] = inverse bind * bone world (row vectors), or
 * NULL = bind pose; L as above */
void gfx_clay_pose(gfx_clay *c, const float (*skin)[16], const gfx_light *L);

/* gfx_skin.c: the GPU skinning data, shared (and checked on the PC):
 * triangles regrouped into batches that use at most GFX_SKIN_PALETTE
 * bones, vertices copied per batch with palette-local bone slots. */
typedef struct {
    int first, count;             /* into gfx_skin_mesh.index */
    int vfirst, nv;               /* this batch's vertices */
    int nbone;
    int16_t bone[GFX_SKIN_PALETTE];   /* palette slot -> model bone */
    gfx_texture *tex;
    int src;                      /* the clay's batch (material) it came from (GFX_RS_BATCH_HIDE) */
} gfx_skin_batch;
typedef struct {
    int nv;
    float *pos, *nrm, *st, *w;    /* 3, 3, 2, 4 per vertex */
    uint8_t *slot;                /* 4 per vertex: palette slots */
    uint8_t *col;                 /* 4 per vertex, base RGBA */
    float *flag;                  /* 2 per vertex: rigid weight, tinted */
    int *src;                     /* per vertex: the clay's vertex */
    uint16_t *index;
    int nindex, nbatch;
    gfx_skin_batch *batch;
} gfx_skin_mesh;
int  gfx_skin_build(gfx_skin_mesh *m, const gfx_clay_desc *d, const gfx_skin_desc *s);
void gfx_skin_free(gfx_skin_mesh *m);
/* what the vertex program computes for vertex v (a C model of it) */
void gfx_skin_eval(const gfx_skin_mesh *m, int b, int v, const float (*skin)[16], const gfx_light *L,
                   float pos[3], float rgba[4]);

gfx_clay *gfx_create_clay(const gfx_clay_desc *d);
/* Replace positions and/or colours (either may be NULL). */
void gfx_update_clay(gfx_clay *c, const float *pos, const uint8_t *col);
/* Draw with the current WORLD/VIEW/PROJECTION and render states. */
void gfx_execute_clay(gfx_clay *c);
void gfx_release_clay(gfx_clay *c);

#endif
