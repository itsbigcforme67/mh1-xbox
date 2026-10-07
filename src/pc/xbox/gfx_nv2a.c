/*
 * gfx_nv2a.c - gfx.h on the original Xbox GPU (NV2A) through nxdk's pbkit.
 *
 * First version (agent A, 7 Oct 2026), written without hardware or xemu:
 * it compiles and links, nothing here has been seen on a screen yet.
 * Structure follows gfx_gl.c and nxdk's samples/mesh:
 *  - one vertex program (shaders/vs.vs.cg): position through one matrix
 *    (world * view * projection * viewport, row vectors as fl and gfx_gl.c
 *    use them), pre-lit colour, texture coordinates through the texture
 *    matrix; one pixel shader (shaders/ps.ps.cg): texture x colour.
 *    Untextured draws bind a 1x1 white texture.
 *  - vertices are copied per draw into a ring of contiguous (GPU-visible)
 *    memory, 24 bytes each (pos 3f, colour RGBA8, st 2f), reset per frame.
 *  - textures: power-of-two sizes are swizzled A8B8G8R8 (repeat works);
 *    other sizes are linear A8B8G8R8 "rect" textures, whose coordinates are
 *    in texels, so the texture matrix gets a w x h scale for them.
 *  - fl's fade colour (0x67) is multiplied on the CPU as in gfx_gl.c.
 * Missing: fog (needs a fog output from the vertex program), clipping of
 * triangles crossing the camera plane (the divide by w is done in the
 * vertex program as in nxdk's samples), gfx_read_pixels only reads the back
 * buffer memory as is (format assumed X8R8G8B8, untested).
 */
#include "../gfx/gfx.h"

#include <hal/video.h>
#include <pbkit/pbkit.h>
#include <windows.h>
#include <xboxkrnl/xboxkrnl.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define MASK(mask, val) (((val) << (ffs(mask) - 1)) & (mask))
#define GPU_MAXRAM 0x03FFAFFF
#define RING_BYTES (6u << 20)
#define ZMAX 16777215.0f                 /* pbkit's default depth buffer is Z24S8 */

struct gfx_texture {
    int w, h, src, rect;
    uint8_t *mem;                        /* contiguous */
    uint32_t pitch;
    uint32_t *pal;                       /* palettised (P8): 256 x A8R8G8B8, contiguous; else NULL */
    size_t bytes;                        /* GPU memory taken (pixels + palette) */
};

struct gfx_clay {
    int nvert, nindex, nbatch;
    float *pos, *st;
    uint8_t *col, *drawcol;
    uint16_t *index;
    gfx_batch *batch;
    /* GPU skinning (gfx_clay_set_skin): the regrouped batches, their
     * vertices in contiguous memory (SKIN_STRIDE bytes each), this frame's
     * pose */
    gfx_skin_mesh *skin;
    uint8_t *svb;
    int nbone;
    float *pose;                /* nbone * 16, or NULL = bind pose */
    int posed;
    gfx_light light;
};
#define SKIN_STRIDE 80          /* pos 3f, nrm 3f, col 4ub, st 2f, w 4f, slot*3 4f, flag 2f */

typedef struct {
    float pos[3];
    uint8_t col[4];
    float st[2];
} vtx;

static struct {
    int w, h;
    float view[16], proj[16], world[16], texmat[16], viewport[16];
    uint32_t fade;
    gfx_texture *tex, *white;
    int filter_point, clamp;
    uint8_t *ring;
    uint32_t ring_used;
    int zwrite, ztest;
    int fog_on;
    float fog_start, fog_end, fog_col[4];
} G;

static const float ident[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
static const uint32_t blend_factor[6] = { 0, 1, 0x302, 0x303, 0x304, 0x305 };  /* GL enums, as NV2A takes them */

int gfx_tex_src_hint;

static void mat_mul(float *out, const float *a, const float *b)       /* out = a * b */
{
    float r[16];
    int i, j;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            r[4 * i + j] = a[4 * i] * b[j] + a[4 * i + 1] * b[4 + j] + a[4 * i + 2] * b[8 + j] + a[4 * i + 3] * b[12 + j];
    memcpy(out, r, sizeof r);
}

static void push1(uint32_t reg, uint32_t v)
{
    uint32_t *p = pb_begin();
    p = pb_push1(p, reg, v);
    pb_end(p);
}

/* two vertex programs in the 136-slot program memory: vs.vs.cg at 0, the
 * skinning one (skin.vs.cg) after it */
static const uint32_t vs_program[] = {
#include "vs.inl"
};
static const uint32_t skin_program[] = {
#include "skin.inl"
};
#define SKIN_START (sizeof vs_program / 16)
typedef char vp_fit[(sizeof vs_program + sizeof skin_program) / 16 <= 136 ? 1 : -1];
static int cur_prog;

static void use_program(int skin)
{
    if (cur_prog != skin) {
        push1(NV097_SET_TRANSFORM_PROGRAM_START, skin ? (uint32_t)SKIN_START : 0);
        cur_prog = skin;
    }
}

static void load_shaders(void)
{
    uint32_t *p;
    unsigned i;

    p = pb_begin();
    p = pb_push1(p, NV097_SET_TRANSFORM_PROGRAM_START, 0);
    p = pb_push1(p, NV097_SET_TRANSFORM_EXECUTION_MODE,
                 MASK(NV097_SET_TRANSFORM_EXECUTION_MODE_MODE, NV097_SET_TRANSFORM_EXECUTION_MODE_MODE_PROGRAM)
                 | MASK(NV097_SET_TRANSFORM_EXECUTION_MODE_RANGE_MODE, NV097_SET_TRANSFORM_EXECUTION_MODE_RANGE_MODE_PRIV));
    p = pb_push1(p, NV097_SET_TRANSFORM_PROGRAM_CXT_WRITE_EN, 0);
    p = pb_push1(p, NV097_SET_TRANSFORM_PROGRAM_LOAD, 0);
    pb_end(p);
    for (i = 0; i < sizeof vs_program / 16; i++) {
        p = pb_begin();
        pb_push(p++, NV097_SET_TRANSFORM_PROGRAM, 4);
        memcpy(p, &vs_program[i * 4], 16);
        p += 4;
        pb_end(p);
    }
    for (i = 0; i < sizeof skin_program / 16; i++) {     /* loads on after the first one */
        p = pb_begin();
        pb_push(p++, NV097_SET_TRANSFORM_PROGRAM, 4);
        memcpy(p, &skin_program[i * 4], 16);
        p += 4;
        pb_end(p);
    }
    p = pb_begin();
#include "ps.inl"
    pb_end(p);
}

static void fixed_state(void)
{
    uint32_t *p = pb_begin();
    p = pb_push1(p, NV097_SET_CULL_FACE_ENABLE, 0);        /* strip winding is not consistent */
    p = pb_push1(p, NV097_SET_DEPTH_FUNC, 0x203);           /* GL_LEQUAL */
    p = pb_push1(p, NV097_SET_ALPHA_TEST_ENABLE, 1);
    p = pb_push1(p, NV097_SET_ALPHA_FUNC, 0x204);           /* GL_GREATER */
    p = pb_push1(p, NV097_SET_ALPHA_REF, 0);
    p = pb_push1(p, NV097_SET_BLEND_ENABLE, 0);
    p = pb_push1(p, NV097_SET_BLEND_EQUATION, NV097_SET_BLEND_EQUATION_V_FUNC_ADD);
    p = pb_push1(p, NV097_SET_FOG_ENABLE, 0);     /* fog is done in the shaders (COLOR1) */
    p = pb_push1(p, NV097_SET_SPECULAR_ENABLE, 1);  /* let COLOR1 through [guess: needed with vertex programs] */
    pb_end(p);
    /* texture stages 1-3 off */
    p = pb_begin();
    p = pb_push1(p, NV097_SET_TEXTURE_CONTROL0 + 64 * 1, 0x0003ffc0);
    p = pb_push1(p, NV097_SET_TEXTURE_CONTROL0 + 64 * 2, 0x0003ffc0);
    p = pb_push1(p, NV097_SET_TEXTURE_CONTROL0 + 64 * 3, 0x0003ffc0);
    pb_end(p);
    G.ztest = G.zwrite = 1;
    push1(NV097_SET_DEPTH_TEST_ENABLE, 1);
    push1(NV097_SET_DEPTH_MASK, 1);
}

int gfx_init(int width, int height, const char *title, int hidden)
{
    static const uint8_t white[4] = { 255, 255, 255, 255 };
    (void)width; (void)height; (void)title; (void)hidden;

    XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);
    if (pb_init() != 0)
        return -1;
    pb_show_front_screen();
    G.w = pb_back_buffer_width();
    G.h = pb_back_buffer_height();
    memcpy(G.view, ident, sizeof ident);
    memcpy(G.proj, ident, sizeof ident);
    memcpy(G.world, ident, sizeof ident);
    memcpy(G.texmat, ident, sizeof ident);
    /* clip space -> window: x (-1..1) -> 0..w, y (1..-1) -> 0..h, z (-1..1) -> 0..ZMAX */
    memset(G.viewport, 0, sizeof G.viewport);
    G.viewport[0] = G.w / 2.0f;
    G.viewport[5] = -G.h / 2.0f;
    G.viewport[10] = ZMAX / 2.0f;
    G.viewport[12] = G.w / 2.0f;
    G.viewport[13] = G.h / 2.0f;
    G.viewport[14] = ZMAX / 2.0f;
    G.viewport[15] = 1;
    G.fade = 0xFFFFFFFFu;
    G.ring = MmAllocateContiguousMemoryEx(RING_BYTES, 0, GPU_MAXRAM, 0, PAGE_READWRITE | PAGE_WRITECOMBINE);
    if (!G.ring)
        return -1;
    load_shaders();
    fixed_state();
    G.white = gfx_create_texture(1, 1, white);
    return 0;
}

void gfx_shutdown(void)
{
    pb_kill();
    if (G.ring)
        MmFreeContiguousMemory(G.ring);
    G.ring = NULL;
}

void gfx_size(int *w, int *h)
{
    *w = G.w;
    *h = G.h;
}

static void wait_idle(void)
{
    while (pb_busy())
        ;
}

void gfx_begin_frame(uint32_t c)
{
    pb_wait_for_vbl();
    pb_reset();
    pb_target_back_buffer();
    wait_idle();
    G.ring_used = 0;
    push1(NV097_SET_DEPTH_MASK, 1);
    pb_erase_depth_stencil_buffer(0, 0, G.w, G.h);
    pb_fill(0, 0, G.w, G.h, 0xFF000000u | (c & 0xFFFFFF));
    push1(NV097_SET_DEPTH_MASK, G.zwrite);
}

void gfx_end_frame(void)
{
    wait_idle();
    while (pb_finished())
        ;
}

int gfx_read_pixels(uint8_t *rgb)
{
    const uint8_t *fb = (const uint8_t *)pb_back_buffer();
    int i;
    if (!fb)
        return -1;
    wait_idle();
    for (i = 0; i < G.w * G.h; i++) {                       /* X8R8G8B8: B G R X in memory */
        rgb[3 * i] = fb[4 * i + 2];
        rgb[3 * i + 1] = fb[4 * i + 1];
        rgb[3 * i + 2] = fb[4 * i];
    }
    return 0;
}

/* ------------------------------------------------------------ textures */
static int is_pow2(int v) { return v > 0 && (v & (v - 1)) == 0; }
static int log2i(int v) { int n = 0; while ((1 << n) < v) n++; return n; }

/* NV2A swizzle: the bits of x and y interleaved, x first, while both last */
static void swizzle_masks(int w, int h, uint32_t *mx, uint32_t *my)
{
    uint32_t bit = 1, m = 1;
    int more;
    *mx = *my = 0;
    do {
        more = 0;
        if ((int)bit < w) { *mx |= m; m <<= 1; more = 1; }
        if ((int)bit < h) { *my |= m; m <<= 1; more = 1; }
        bit <<= 1;
    } while (more);
}

static uint32_t deposit(uint32_t v, uint32_t mask)
{
    uint32_t r = 0, b;
    for (b = 1; mask; b <<= 1) {
        uint32_t low = mask & -mask;
        if (v & b)
            r |= low;
        mask &= mask - 1;
    }
    return r;
}

gfx_texture *gfx_create_texture(int w, int h, const uint8_t *rgba)
{
    gfx_texture *t = calloc(1, sizeof *t);
    size_t size;
    if (!t)
        return NULL;
    t->w = w;
    t->h = h;
    t->src = gfx_tex_src_hint;
    gfx_tex_src_hint = 0;
    t->rect = !(is_pow2(w) && is_pow2(h));
    if (!t->rect && w * h >= 256) {
        /* palettised when the image has <= 256 colours: 1 byte per texel
         * instead of 4 (NV2A SZ_I8_A8R8G8B8; swizzled, power-of-two only) */
        static uint32_t pal[256];
        uint8_t *idx = malloc((size_t)w * h);
        int np = idx ? gfx_to_indexed((const uint32_t *)rgba, w * h, pal, idx) : -1;
        if (np >= 0) {
            t->pal = MmAllocateContiguousMemoryEx(1024, 0, GPU_MAXRAM, 0, PAGE_READWRITE | PAGE_WRITECOMBINE);
            t->mem = t->pal ? MmAllocateContiguousMemoryEx((size_t)w * h, 0, GPU_MAXRAM, 0,
                                                            PAGE_READWRITE | PAGE_WRITECOMBINE) : NULL;
        }
        if (np >= 0 && t->mem) {
            uint32_t mx, my;
            int x, y, i;
            for (i = 0; i < 256; i++) {          /* RGBA bytes -> A8R8G8B8 word */
                uint32_t c = i < np ? pal[i] : 0;
                t->pal[i] = (c & 0xFF00FF00u) | (c & 0xFF) << 16 | (c >> 16 & 0xFF);
            }
            swizzle_masks(w, h, &mx, &my);
            for (y = 0; y < h; y++) {
                uint32_t oy = deposit((uint32_t)y, my);
                for (x = 0; x < w; x++)
                    t->mem[oy | deposit((uint32_t)x, mx)] = idx[y * w + x];
            }
            free(idx);
            t->pitch = (uint32_t)w;
            t->bytes = (size_t)w * h + 1024;
            rt_ms_add("textures in GPU memory (P8 or RGBA8, GPU)", (long)t->bytes);
            rt_ms_add("textures as on disc (4/8-bit+CLUT, GPU)", t->src);
            return t;
        }
        free(idx);
        if (t->pal)
            MmFreeContiguousMemory(t->pal);
        t->pal = NULL;
    }
    t->pitch = (uint32_t)w * 4;
    size = (size_t)t->pitch * h;
    t->mem = MmAllocateContiguousMemoryEx(size, 0, GPU_MAXRAM, 0, PAGE_READWRITE | PAGE_WRITECOMBINE);
    if (!t->mem) {
        free(t);
        return NULL;
    }
    t->bytes = size;
    rt_ms_add("textures in GPU memory (P8 or RGBA8, GPU)", (long)size);
    rt_ms_add("textures as on disc (4/8-bit+CLUT, GPU)", t->src);
    if (t->rect) {
        memcpy(t->mem, rgba, size);
    } else {
        uint32_t mx, my, *dst = (uint32_t *)t->mem;
        const uint32_t *src = (const uint32_t *)rgba;
        int x, y;
        swizzle_masks(w, h, &mx, &my);
        for (y = 0; y < h; y++) {
            uint32_t oy = deposit((uint32_t)y, my);
            for (x = 0; x < w; x++)
                dst[oy | deposit((uint32_t)x, mx)] = src[y * w + x];
        }
    }
    return t;
}

void gfx_release_texture(gfx_texture *t)
{
    if (!t)
        return;
    wait_idle();                       /* the GPU may still read it this frame */
    rt_ms_add("textures in GPU memory (P8 or RGBA8, GPU)", -(long)t->bytes);
    rt_ms_add("textures as on disc (4/8-bit+CLUT, GPU)", -(long)t->src);
    MmFreeContiguousMemory(t->mem);
    if (t->pal)
        MmFreeContiguousMemory(t->pal);
    if (G.tex == t)
        G.tex = NULL;
    free(t);
}

static void bind_texture(gfx_texture *t)
{
    uint32_t fmt, *p, wrap, filt;
    if (!t)
        t = G.white;
    if (t->rect)
        fmt = MASK(NV097_SET_TEXTURE_FORMAT_COLOR, NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8B8G8R8);
    else
        fmt = MASK(NV097_SET_TEXTURE_FORMAT_COLOR, t->pal ? NV097_SET_TEXTURE_FORMAT_COLOR_SZ_I8_A8R8G8B8
                                                          : NV097_SET_TEXTURE_FORMAT_COLOR_SZ_A8B8G8R8)
            | MASK(NV097_SET_TEXTURE_FORMAT_BASE_SIZE_U, log2i(t->w))
            | MASK(NV097_SET_TEXTURE_FORMAT_BASE_SIZE_V, log2i(t->h))
            | MASK(NV097_SET_TEXTURE_FORMAT_BASE_SIZE_P, 0);
    fmt |= MASK(NV097_SET_TEXTURE_FORMAT_CONTEXT_DMA, 2) | NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE
        | MASK(NV097_SET_TEXTURE_FORMAT_DIMENSIONALITY, 2) | MASK(NV097_SET_TEXTURE_FORMAT_MIPMAP_LEVELS, 1);
    /* address modes: 1 wrap, 3 clamp (rect textures can only clamp) */
    wrap = (G.clamp || t->rect) ? 0x00030303 : 0x00030101;
    filt = G.filter_point ? 0x01012000 : 0x02022000;     /* mag | min, nearest 1 / linear 2 */
    p = pb_begin();
    p = pb_push2(p, NV097_SET_TEXTURE_OFFSET, (uint32_t)(uintptr_t)t->mem & 0x03ffffff, fmt);
    p = pb_push1(p, NV097_SET_TEXTURE_CONTROL1, t->pitch << 16);
    p = pb_push1(p, NV097_SET_TEXTURE_IMAGE_RECT, ((uint32_t)t->w << 16) | (uint32_t)t->h);
    p = pb_push1(p, NV097_SET_TEXTURE_ADDRESS, wrap);
    p = pb_push1(p, NV097_SET_TEXTURE_CONTROL0, 0x4003ffc0);
    p = pb_push1(p, NV097_SET_TEXTURE_FILTER, filt);
    if (t->pal)     /* 256 entries; DMA B as the texture (CONTEXT_DMA 2 above) [guess: untested] */
        p = pb_push1(p, NV097_SET_TEXTURE_PALETTE, ((uint32_t)(uintptr_t)t->pal & 0x03ffffc0)
                     | MASK(NV097_SET_TEXTURE_PALETTE_LENGTH, NV097_SET_TEXTURE_PALETTE_LENGTH_256)
                     | NV097_SET_TEXTURE_PALETTE_CONTEXT_DMA);
    pb_end(p);
}

/* ------------------------------------------------------------ state */
void gfx_set_render_state(int state, uintptr_t v)
{
    if (gfx_rec_state(state, v))
        return;
    switch (state) {
    case GFX_RS_TEXTURE:
        G.tex = (gfx_texture *)v;
        break;
    case GFX_RS_FOG_COLOR:
        G.fog_col[0] = ((v >> 16) & 255) / 255.0f;
        G.fog_col[1] = ((v >> 8) & 255) / 255.0f;
        G.fog_col[2] = (v & 255) / 255.0f;
        break;
    case GFX_RS_FOG_START:
        G.fog_start = *(const float *)v;
        break;
    case GFX_RS_FOG_END:
        G.fog_end = *(const float *)v;
        break;
    case GFX_RS_FOG_ENABLE:
        G.fog_on = v != 0;
        break;
    case GFX_RS_VIEW:
        memcpy(G.view, (const float *)v, sizeof G.view);
        break;
    case GFX_RS_TEXMAT:
        memcpy(G.texmat, v ? (const float *)v : ident, sizeof G.texmat);
        break;
    case GFX_RS_WORLD:
        memcpy(G.world, (const float *)v, sizeof G.world);
        break;
    case GFX_RS_PROJECTION:
        memcpy(G.proj, (const float *)v, sizeof G.proj);
        break;
    case GFX_RS_ALPHA_REF:
        push1(NV097_SET_ALPHA_REF, (uint32_t)(v & 255));
        break;
    case GFX_RS_FADE_COLOR:
        G.fade = (uint32_t)v;
        break;
    case GFX_RS_ZWRITE:
        G.zwrite = v != 0;
        push1(NV097_SET_DEPTH_MASK, G.zwrite);
        break;
    case GFX_RS_ZTEST:
        G.ztest = v != 0;
        push1(NV097_SET_DEPTH_TEST_ENABLE, G.ztest);
        break;
    case GFX_RS_BLEND_FUNC: {
        unsigned src = v & 15, dst = (v >> 4) & 15;
        if (src < 6 && dst < 6) {
            uint32_t *p = pb_begin();
            p = pb_push1(p, NV097_SET_BLEND_ENABLE, 1);
            p = pb_push1(p, NV097_SET_BLEND_FUNC_SFACTOR, blend_factor[src]);
            p = pb_push1(p, NV097_SET_BLEND_FUNC_DFACTOR, blend_factor[dst]);
            pb_end(p);
        }
        break;
    }
    case GFX_RS_BLEND_OP:
        push1(NV097_SET_BLEND_EQUATION, (v & 0xC00) == 0x400 ? NV097_SET_BLEND_EQUATION_V_FUNC_SUBTRACT
              : (v & 0xC00) == 0x800 ? NV097_SET_BLEND_EQUATION_V_FUNC_REVERSE_SUBTRACT
              : NV097_SET_BLEND_EQUATION_V_FUNC_ADD);
        break;
    case GFX_RS_FILTER:
        G.filter_point = (v & 0x10000) != 0;
        break;
    case GFX_RS_TEX_CLAMP:
        G.clamp = v != 0;
        break;
    case GFX_RS_BLEND: {
        uint32_t *p = pb_begin();
        p = pb_push1(p, NV097_SET_BLEND_ENABLE, v != 0);
        if (v) {
            p = pb_push1(p, NV097_SET_BLEND_FUNC_SFACTOR, 0x302);
            p = pb_push1(p, NV097_SET_BLEND_FUNC_DFACTOR, 0x303);
        }
        pb_end(p);
        break;
    }
    }
}

void gfx_set_render_state_f(int state, float value)
{
    gfx_set_render_state(state, (uintptr_t)&value);
}

/* ------------------------------------------------------------ drawing */
/* vertex program constants (cgc's allocation, see shaders/vs.vs.cg):
 * c[0..3] the combined matrix, c[4..7] the texture matrix, c[8] fog
 * (k, b): factor = saturate(w * k + b), c[9] fog colour, c[10] = (0, 1)
 * cgc's own constant. fog: 0 for 2D draws. */
static void upload_constants(const float *mvp, const float *tex, int fog)
{
    static const float c10[4] = { 0, 1, 0, 0 };
    float kb[4] = { 0, 0, 0, 0 };
    uint32_t *p = pb_begin();
    if (fog && G.fog_on && G.fog_end != G.fog_start) {     /* GL linear fog, w as the eye distance */
        kb[0] = 1.0f / (G.fog_end - G.fog_start);
        kb[1] = -G.fog_start / (G.fog_end - G.fog_start);
    }
    p = pb_push1(p, NV097_SET_TRANSFORM_CONSTANT_LOAD, 96);
    pb_push(p++, NV097_SET_TRANSFORM_CONSTANT, 16);
    memcpy(p, mvp, 64);
    p += 16;
    pb_push(p++, NV097_SET_TRANSFORM_CONSTANT, 16);
    memcpy(p, tex, 64);
    p += 16;
    pb_push(p++, NV097_SET_TRANSFORM_CONSTANT, 4);
    memcpy(p, kb, 16);
    p += 4;
    pb_push(p++, NV097_SET_TRANSFORM_CONSTANT, 4);
    memcpy(p, G.fog_col, 16);
    p += 4;
    pb_push(p++, NV097_SET_TRANSFORM_CONSTANT, 4);
    memcpy(p, c10, 16);
    p += 4;
    pb_end(p);
}

/* the texture matrix for T: rect textures take texel coordinates */
static void tex_matrix(const gfx_texture *t, float *out)
{
    memcpy(out, G.texmat, 64);
    if (t && t->rect) {
        int i;
        for (i = 0; i < 4; i++) {
            out[4 * i] *= (float)t->w;
            out[4 * i + 1] *= (float)t->h;
        }
    }
}

static vtx *ring_alloc(int n)
{
    uint32_t need = (uint32_t)n * sizeof(vtx);
    vtx *v;
    if (need > RING_BYTES)
        return NULL;
    if (G.ring_used + need > RING_BYTES) {     /* full: let the GPU finish, start over */
        wait_idle();
        G.ring_used = 0;
    }
    v = (vtx *)(G.ring + G.ring_used);
    G.ring_used += (need + 15) & ~15u;
    return v;
}

static void set_arrays(const vtx *v)
{
    uint32_t base = (uint32_t)(uintptr_t)v & 0x03ffffff, *p;
    int i;
    p = pb_begin();
    pb_push(p++, NV097_SET_VERTEX_DATA_ARRAY_FORMAT, 16);
    for (i = 0; i < 16; i++)
        *p++ = NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F;       /* size 0: unused */
    p = pb_push1(p, NV097_SET_VERTEX_DATA_ARRAY_FORMAT + 0 * 4,
                 MASK(NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F)
                 | MASK(NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE, 3) | MASK(NV097_SET_VERTEX_DATA_ARRAY_FORMAT_STRIDE, sizeof(vtx)));
    p = pb_push1(p, NV097_SET_VERTEX_DATA_ARRAY_OFFSET + 0 * 4, base);
    p = pb_push1(p, NV097_SET_VERTEX_DATA_ARRAY_FORMAT + 3 * 4,
                 MASK(NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_UB_OGL)
                 | MASK(NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE, 4) | MASK(NV097_SET_VERTEX_DATA_ARRAY_FORMAT_STRIDE, sizeof(vtx)));
    p = pb_push1(p, NV097_SET_VERTEX_DATA_ARRAY_OFFSET + 3 * 4, base + 12);
    p = pb_push1(p, NV097_SET_VERTEX_DATA_ARRAY_FORMAT + 9 * 4,
                 MASK(NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F)
                 | MASK(NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE, 2) | MASK(NV097_SET_VERTEX_DATA_ARRAY_FORMAT_STRIDE, sizeof(vtx)));
    p = pb_push1(p, NV097_SET_VERTEX_DATA_ARRAY_OFFSET + 9 * 4, base + 16);
    pb_end(p);
}

/* triangles from 16-bit indices, in blocks that stay under pbkit's
 * 128-dword limit per pb_begin/pb_end */
static void draw_indexed(const uint16_t *idx, int n)
{
    uint32_t *p;
    int i = 0;
    p = pb_begin();
    p = pb_push1(p, NV097_SET_BEGIN_END, NV097_SET_BEGIN_END_OP_TRIANGLES);
    pb_end(p);
    while (n - i >= 2) {
        int pairs = (n - i) / 2, k;
        if (pairs > 120)
            pairs = 120;
        p = pb_begin();
        pb_push(p++, 0x40000000 | NV097_ARRAY_ELEMENT16, pairs);
        for (k = 0; k < pairs; k++, i += 2)
            *p++ = (uint32_t)idx[i] | ((uint32_t)idx[i + 1] << 16);
        pb_end(p);
    }
    p = pb_begin();
    if (i < n)
        p = pb_push1(p, NV097_ARRAY_ELEMENT32, idx[i]);
    p = pb_push1(p, NV097_SET_BEGIN_END, NV097_SET_BEGIN_END_OP_END);
    pb_end(p);
}

static void draw_arrays(int n)
{
    uint32_t *p;
    int i = 0;
    p = pb_begin();
    p = pb_push1(p, NV097_SET_BEGIN_END, NV097_SET_BEGIN_END_OP_TRIANGLES);
    while (i < n) {
        int c = n - i > 255 ? 255 : n - i;
        /* DRAW_ARRAYS: start in the low 24 bits, count-1 in the top 8 */
        p = pb_push1(p, NV097_DRAW_ARRAYS, MASK(NV097_DRAW_ARRAYS_COUNT, c - 1) | MASK(NV097_DRAW_ARRAYS_START_INDEX, i));
        i += c;
    }
    p = pb_push1(p, NV097_SET_BEGIN_END, NV097_SET_BEGIN_END_OP_END);
    pb_end(p);
}

gfx_clay *gfx_create_clay(const gfx_clay_desc *d)
{
    gfx_clay *c = calloc(1, sizeof *c);
    c->nvert = d->nvert;
    c->nindex = d->nindex;
    c->nbatch = d->nbatch;
    c->pos = malloc(sizeof(float) * 3 * (d->nvert + 1));
    memcpy(c->pos, d->pos, sizeof(float) * 3 * d->nvert);
    if (d->st) {
        c->st = malloc(sizeof(float) * 2 * (d->nvert + 1));
        memcpy(c->st, d->st, sizeof(float) * 2 * d->nvert);
    }
    c->col = malloc(4 * (size_t)(d->nvert + 1));
    if (d->col)
        memcpy(c->col, d->col, 4 * (size_t)d->nvert);
    else
        memset(c->col, 255, 4 * (size_t)d->nvert);
    c->index = malloc(sizeof(uint16_t) * (d->nindex + 1));
    memcpy(c->index, d->index, sizeof(uint16_t) * d->nindex);
    c->batch = malloc(sizeof(gfx_batch) * (d->nbatch + 1));
    memcpy(c->batch, d->batch, sizeof(gfx_batch) * d->nbatch);
    return c;
}

void gfx_update_clay(gfx_clay *c, const float *pos, const uint8_t *col)
{
    if (pos)
        memcpy(c->pos, pos, sizeof(float) * 3 * c->nvert);
    if (col)
        memcpy(c->col, col, 4 * (size_t)c->nvert);
}

/* ------------------------------------------------------------ near clipping
 * The vertex program divides by w; a triangle with a vertex at or behind
 * the eye would come out wrapped across the screen. Triangles that cross
 * the plane w = CLIP_W are cut here on the CPU, in object space (a clip
 * space plane is a plane there too, and the attributes interpolate the
 * same way); triangles wholly behind it are dropped. Only clays with such
 * triangles pay for it. */
#define CLIP_W 1.0f
static float clip_w_of(const float *mvp, const float *p)
{
    return p[0] * mvp[3] + p[1] * mvp[7] + p[2] * mvp[11] + mvp[15];
}

static void vtx_lerp(vtx *o, const vtx *a, const vtx *b, float t)
{
    int k;
    for (k = 0; k < 3; k++)
        o->pos[k] = a->pos[k] + (b->pos[k] - a->pos[k]) * t;
    for (k = 0; k < 4; k++)
        o->col[k] = (uint8_t)(a->col[k] + (b->col[k] - a->col[k]) * t + 0.5f);
    for (k = 0; k < 2; k++)
        o->st[k] = a->st[k] + (b->st[k] - a->st[k]) * t;
}

static uint16_t *clip_buf;          /* the batches' index lists after clipping (grows) */
static int clip_cap, clip_first[256], clip_count[256];

static void execute_cpu(gfx_clay *c)
{
    float mvp[16], tm[16];
    unsigned f[4] = { 255, 255, 255, 255 };
    int i, k, b, fade = G.fade != 0xFFFFFFFFu, ncross = 0, nv;
    float *w = NULL;
    const uint16_t *clip_idx = NULL;
    vtx *v;

    if (gfx_rec_clay(c))
        return;
    if (!c->nvert)
        return;
    mat_mul(mvp, G.world, G.view);          /* row vectors: v * world * view * proj * viewport */
    mat_mul(mvp, mvp, G.proj);
    mat_mul(mvp, mvp, G.viewport);
    /* which triangles cross the near plane (w per vertex: one dot product) */
    if (c->nbatch <= 256 && (w = malloc(sizeof(float) * c->nvert)) != NULL) {
        int any_out = 0;
        for (i = 0; i < c->nvert; i++)
            if ((w[i] = clip_w_of(mvp, c->pos + 3 * i)) < CLIP_W)
                any_out = 1;
        if (any_out)
            for (b = 0; b < c->nbatch; b++)
                for (i = 0; i + 2 < c->batch[b].count; i += 3) {
                    const uint16_t *t = c->index + c->batch[b].first + i;
                    int in = (w[t[0]] >= CLIP_W) + (w[t[1]] >= CLIP_W) + (w[t[2]] >= CLIP_W);
                    if (in < 3)
                        ncross++;
                }
        if (!ncross || c->nvert + 2 * ncross > 65535) {
            free(w);
            w = NULL;
            ncross = 0;
        }
    }
    nv = c->nvert + 2 * ncross;
    if (!(v = ring_alloc(nv))) {
        free(w);
        return;
    }
    if (fade) {
        f[0] = (G.fade >> 16) & 255;
        f[1] = (G.fade >> 8) & 255;
        f[2] = G.fade & 255;
        f[3] = G.fade >> 24;
    }
    for (i = 0; i < c->nvert; i++) {
        memcpy(v[i].pos, c->pos + 3 * i, 12);
        for (k = 0; k < 4; k++)
            v[i].col[k] = fade ? (uint8_t)(c->col[4 * i + k] * f[k] / 255) : c->col[4 * i + k];
        if (c->st)
            memcpy(v[i].st, c->st + 2 * i, 8);
        else
            v[i].st[0] = v[i].st[1] = 0;
    }
    if (ncross) {
        /* new index lists: kept triangles as they are, crossing ones cut
         * (1 vertex in: 1 triangle; 2 in: a quad = 2 triangles) */
        int nidx = 0, nnew = c->nvert;
        for (b = 0; b < c->nbatch; b++)
            nidx += c->batch[b].count + 3 * ncross;
        if (nidx > clip_cap) {
            free(clip_buf);
            clip_buf = malloc(sizeof *clip_buf * (size_t)nidx);
            clip_cap = clip_buf ? nidx : 0;
        }
        if (!clip_buf) {
            free(w);
            return;
        }
        clip_idx = clip_buf;
        nidx = 0;
        for (b = 0; b < c->nbatch; b++) {
            clip_first[b] = nidx;
            for (i = 0; i + 2 < c->batch[b].count; i += 3) {
                const uint16_t *t = c->index + c->batch[b].first + i;
                uint16_t poly[4];
                float tw[3] = { w[t[0]], w[t[1]], w[t[2]] }, tt[4];
                int np, e, ea[4], eb[4];
                np = gfx_clip_tri(tw, CLIP_W, ea, eb, tt);
                for (e = 0; e < np; e++)
                    if (tt[e] == 0.0f) {
                        poly[e] = t[ea[e]];
                    } else {
                        vtx_lerp(&v[nnew], &v[t[ea[e]]], &v[t[eb[e]]], tt[e]);
                        poly[e] = (uint16_t)nnew++;
                    }
                for (e = 1; e + 1 < np; e++) {
                    clip_buf[nidx++] = poly[0];
                    clip_buf[nidx++] = poly[e];
                    clip_buf[nidx++] = poly[e + 1];
                }
            }
            clip_count[b] = nidx - clip_first[b];
        }
        free(w);
    }
    set_arrays(v);
    for (b = 0; b < c->nbatch; b++) {
        gfx_texture *t = c->batch[b].tex ? c->batch[b].tex : G.tex;
        if (!c->st)
            t = NULL;
        bind_texture(t);
        tex_matrix(t, tm);
        upload_constants(mvp, tm, 1);
        if (clip_idx)
            draw_indexed(clip_idx + clip_first[b], clip_count[b]);
        else
            draw_indexed(c->index + c->batch[b].first, c->batch[b].count);
    }
}

void gfx_draw_2d(int w, int h, int nvert, const float *pos, const float *st, const uint8_t *col)
{
    gfx_texture *t = st ? G.tex : NULL;
    float m[16], tm[16];
    vtx *v;
    int i;

    if (gfx_rec_2d(w, h, nvert, pos, st, col))
        return;
    if (nvert <= 0 || !(v = ring_alloc(nvert)))
        return;
    for (i = 0; i < nvert; i++) {
        v[i].pos[0] = pos[2 * i];
        v[i].pos[1] = pos[2 * i + 1];
        v[i].pos[2] = 0;
        if (col)
            memcpy(v[i].col, col + 4 * i, 4);
        else
            memset(v[i].col, 255, 4);
        v[i].st[0] = st ? st[2 * i] : 0;
        v[i].st[1] = st ? st[2 * i + 1] : 0;
    }
    /* virtual w x h screen (origin top left) straight to window pixels */
    memcpy(m, ident, sizeof m);
    m[0] = (float)G.w / w;
    m[5] = (float)G.h / h;
    memcpy(tm, ident, sizeof tm);
    if (t && t->rect) {
        tm[0] = (float)t->w;
        tm[5] = (float)t->h;
    }
    push1(NV097_SET_DEPTH_TEST_ENABLE, 0);
    push1(NV097_SET_DEPTH_MASK, 0);
    set_arrays(v);
    bind_texture(t);
    upload_constants(m, tm, 0);
    draw_arrays(nvert);
    push1(NV097_SET_DEPTH_TEST_ENABLE, G.ztest);
    push1(NV097_SET_DEPTH_MASK, G.zwrite);
}

void gfx_release_clay(gfx_clay *c)
{
    if (!c)
        return;
    if (c->skin) {
        wait_idle();
        gfx_skin_free(c->skin);
        free(c->skin);
        if (c->svb)
            MmFreeContiguousMemory(c->svb);
        free(c->pose);
    }
    free(c->pos);
    free(c->st);
    free(c->col);
    free(c->drawcol);
    free(c->index);
    free(c->batch);
    free(c);
}

/* ------------------------------------------------------------ GPU skinning */
int gfx_skin_capable(void) { return 1; }

int gfx_clay_set_skin(gfx_clay *c, const gfx_skin_desc *s)
{
    gfx_clay_desc d;
    gfx_skin_mesh *m = calloc(1, sizeof *m);
    int i, k;
    if (!m)
        return -1;
    memset(&d, 0, sizeof d);
    d.nvert = c->nvert;
    d.pos = c->pos;
    d.st = c->st;
    d.col = c->col;
    d.nindex = c->nindex;
    d.index = c->index;
    d.nbatch = c->nbatch;
    d.batch = c->batch;
    if (gfx_skin_build(m, &d, s) != 0) {
        free(m);
        return -1;              /* stays a CPU clay (fl_model skins it) */
    }
    c->svb = MmAllocateContiguousMemoryEx((size_t)m->nv * SKIN_STRIDE, 0, GPU_MAXRAM, 0, PAGE_READWRITE | PAGE_WRITECOMBINE);
    if (!c->svb) {
        gfx_skin_free(m);
        free(m);
        return -1;
    }
    rt_ms_add("skinned vertex buffers (GPU)", (long)m->nv * SKIN_STRIDE);
    for (i = 0; i < m->nv; i++) {
        uint8_t *v = c->svb + (size_t)i * SKIN_STRIDE;
        float f[4];
        memcpy(v, m->pos + 3 * i, 12);
        memcpy(v + 12, m->nrm + 3 * i, 12);
        memcpy(v + 24, m->col + 4 * i, 4);
        memcpy(v + 28, m->st + 2 * i, 8);
        memcpy(v + 36, m->w + 4 * i, 16);
        for (k = 0; k < 4; k++)
            f[k] = 3.0f * m->slot[4 * i + k];
        memcpy(v + 52, f, 16);
        memcpy(v + 68, m->flag + 2 * i, 8);
    }
    /* the mesh keeps only what drawing needs (batches, indices) */
    free(m->pos); free(m->nrm); free(m->st); free(m->w); free(m->slot); free(m->col); free(m->flag); free(m->src);
    m->pos = m->nrm = m->st = m->w = m->flag = NULL;
    m->slot = m->col = NULL;
    m->src = NULL;
    c->skin = m;
    c->nbone = s->nbone;
    return 0;
}

void gfx_clay_pose(gfx_clay *c, const float (*skin)[16], const gfx_light *L)
{
    if (!c->skin)
        return;
    if (skin && c->nbone > 0) {
        if (!c->pose)
            c->pose = malloc(sizeof(float) * 16 * (size_t)c->nbone);
        if (c->pose)
            memcpy(c->pose, skin, sizeof(float) * 16 * (size_t)c->nbone);
    } else {
        free(c->pose);          /* bind pose: identity bones (gfx.h) */
        c->pose = NULL;
    }
    c->light = *L;
    c->posed = 1;
}

static void set_skin_arrays(const uint8_t *v)
{
    uint32_t base = (uint32_t)(uintptr_t)v & 0x03ffffff, *p;
    static const struct { int attr, type, size, off; } a[7] = {
        { 0, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F, 3, 0 },     /* pos */
        { 2, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F, 3, 12 },    /* normal */
        { 3, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_UB_OGL, 4, 24 },  /* base colour */
        { 9, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F, 2, 28 },    /* TEX0: st */
        { 10, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F, 4, 36 },   /* TEX1: weights */
        { 11, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F, 4, 52 },   /* TEX2: 3 * palette slot */
        { 12, NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F, 2, 68 },   /* TEX3: rigid, tinted */
    };
    int i;
    p = pb_begin();
    pb_push(p++, NV097_SET_VERTEX_DATA_ARRAY_FORMAT, 16);
    for (i = 0; i < 16; i++)
        *p++ = NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F;
    for (i = 0; i < 7; i++) {
        p = pb_push1(p, NV097_SET_VERTEX_DATA_ARRAY_FORMAT + a[i].attr * 4,
                     MASK(NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE, a[i].type)
                     | MASK(NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE, a[i].size)
                     | MASK(NV097_SET_VERTEX_DATA_ARRAY_FORMAT_STRIDE, SKIN_STRIDE));
        p = pb_push1(p, NV097_SET_VERTEX_DATA_ARRAY_OFFSET + a[i].attr * 4, base + (uint32_t)a[i].off);
    }
    pb_end(p);
}

static void push_consts(uint32_t first, const float *v, int n4)
{
    uint32_t *p = pb_begin();
    p = pb_push1(p, NV097_SET_TRANSFORM_CONSTANT_LOAD, 96 + first);
    while (n4 > 0) {
        int k = n4 > 8 ? 8 : n4;            /* 32 floats per push */
        pb_push(p++, NV097_SET_TRANSFORM_CONSTANT, 4 * k);
        memcpy(p, v, 16 * (size_t)k);
        p += 4 * k;
        v += 4 * k;
        n4 -= k;
    }
    pb_end(p);
}

/* skin.vs.cg's constants (tools/build_xbox.py checks cgc's layout):
 * c0 mvp, c4 texture matrix, c8 fog (k, b), c9 fog colour, c10-12 light
 * directions, c13-15 light colours, c16 ambient, c17 tint, c18 mode
 * (x: lit), c19 fade colour, c20.. bone rows (3 per palette slot), c92 (1, 0, 0.5) */
static void draw_skinned(gfx_clay *c, const float *mvp)
{
    static const float id[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
    gfx_skin_mesh *m = c->skin;
    float k[20 * 4], tm[16], rows[GFX_SKIN_PALETTE * 3 * 4], c91[4] = { 1, 0, 0.5f, 0 };
    int b, i, j, l;
    memset(k, 0, sizeof k);
    memcpy(k, mvp, 64);
    if (G.fog_on && G.fog_end != G.fog_start) {
        k[32] = 1.0f / (G.fog_end - G.fog_start);
        k[33] = -G.fog_start / (G.fog_end - G.fog_start);
    }
    memcpy(k + 36, G.fog_col, 12);
    for (l = 0; l < 3; l++) {
        memcpy(k + 40 + 4 * l, c->light.dir[l], 12);
        memcpy(k + 52 + 4 * l, c->light.col[l], 12);
    }
    memcpy(k + 64, c->light.ambient, 12);
    if (c->light.tint)
        memcpy(k + 68, c->light.tint_rgb, 12);
    else
        k[68] = k[69] = k[70] = 1;
    k[72] = c->light.lit ? 1.0f : 0.0f;
    for (i = 0; i < 4; i++)                  /* fl state 0x67, as the CPU path multiplies it in */
        k[76 + i] = G.fade == 0xFFFFFFFFu ? 1.0f : ((G.fade >> (i == 3 ? 24 : 16 - 8 * i)) & 255) / 255.0f;
    use_program(1);
    push_consts(92, c91, 1);
    for (b = 0; b < m->nbatch; b++) {
        const gfx_skin_batch *bt = &m->batch[b];
        gfx_texture *t = bt->tex ? bt->tex : G.tex;
        if (!c->st)
            t = NULL;
        for (i = 0; i < bt->nbone; i++) {
            const float *M = c->pose ? c->pose + 16 * bt->bone[i] : id;
            for (j = 0; j < 3; j++) {           /* row j = column j of the row-vector matrix */
                rows[12 * i + 4 * j] = M[j];
                rows[12 * i + 4 * j + 1] = M[4 + j];
                rows[12 * i + 4 * j + 2] = M[8 + j];
                rows[12 * i + 4 * j + 3] = M[12 + j];
            }
        }
        bind_texture(t);
        tex_matrix(t, tm);
        memcpy(k + 16, tm, 64);
        push_consts(0, k, 20);
        if (bt->nbone)
            push_consts(20, rows, 3 * bt->nbone);
        set_skin_arrays(c->svb + (size_t)bt->vfirst * SKIN_STRIDE);
        draw_indexed(m->index + bt->first, bt->count);
    }
    use_program(0);
}

void gfx_execute_clay(gfx_clay *c)
{
    float mvp[16];
    if (!c->skin || !c->posed) {
        execute_cpu(c);
        return;
    }
    if (gfx_rec_clay(c))                    /* skinned and lit by skin.vs.cg */
        return;
    mat_mul(mvp, G.world, G.view);
    mat_mul(mvp, mvp, G.proj);
    mat_mul(mvp, mvp, G.viewport);
    draw_skinned(c, mvp);
}
