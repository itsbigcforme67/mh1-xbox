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
#include "../gfx/gfx_gl.h"           /* gfx_opt.widescreen (settings menu, gfx_opts.c) */

#include <hal/video.h>
#include <pbkit/pbkit.h>
#include <windows.h>
#include <xboxkrnl/xboxkrnl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "../rt/rt_prof.h"
#include "../rt/rt_log.h"

/* One kick per draw. pbkit's pb_end sends the commands at once (pb_start: an sfence, a write-combine flush that polls
 * a GPU register until done, then the PUT register write): one round of uncached register accesses per block, and
 * the backend wrote ~1300 small blocks a frame (each state change its own; 27 Oct 2026, G guild hall). In xemu every
 * such access is a slow exit to the emulator. Here pb_begin / pb_end only move a write pointer past pbkit's put
 * pointer (pbkit's pb_begin just returns it; its 128-dword block check is debug-only), and cmd_kick sends what was
 * written: after each draw, when a batch grows past CMD_BATCH dwords, and before every pbkit call that writes
 * commands itself or waits for the GPU (the macros below). Same commands in the same order, fewer PUT writes. */
#define CMD_BATCH 4096
static uint32_t *cmd_open, *cmd_start;
static void cmd_kick(void)
{
    if (!cmd_open)
        return;
    pb_end(cmd_open);
    cmd_open = NULL;
    rt_prof_count(RTPC_KICKS, 1);
}
static inline uint32_t *cmd_begin(void)
{
    if (!cmd_open)
        cmd_open = cmd_start = pb_begin();
    return cmd_open;
}
static inline void cmd_end(uint32_t *p)
{
    cmd_open = p;
    if (p - cmd_start > CMD_BATCH)
        cmd_kick();
}
#define pb_begin cmd_begin
#define pb_end cmd_end
#define pb_busy() (cmd_kick(), pb_busy())
#define pb_finished() (cmd_kick(), pb_finished())
#define pb_reset() (cmd_kick(), pb_reset())
#define pb_wait_for_vbl() (cmd_kick(), pb_wait_for_vbl())
#define pb_target_back_buffer() (cmd_kick(), pb_target_back_buffer())
#define pb_target_extra_buffer(...) (cmd_kick(), pb_target_extra_buffer(__VA_ARGS__))
#define pb_fill(...) (cmd_kick(), pb_fill(__VA_ARGS__))
#define pb_erase_depth_stencil_buffer(...) (cmd_kick(), pb_erase_depth_stencil_buffer(__VA_ARGS__))
#define pb_kill() (cmd_kick(), pb_kill())
#define pb_show_front_screen() (cmd_kick(), pb_show_front_screen())
#define MASK(mask, val) (((val) << (ffs(mask) - 1)) & (mask))
#define GPU_MAXRAM 0x03FFAFFF
#define RING_BYTES (6u << 20)
#define ZMAX 16777215.0f                 /* pbkit's default depth buffer is Z24S8 */

struct gfx_texture {
    int w, h, src, rect;
    int xrgb;                            /* a frame buffer (X8R8G8B8, linear): the PS2 output's frame (ps2_present) */
    uint8_t *mem;                        /* contiguous */
    uint32_t pitch;
    uint32_t *pal;                       /* palettised (P8): 256 x A8R8G8B8, contiguous; else NULL */
    size_t bytes;                        /* GPU memory taken (pixels + palette) */
};

typedef struct {
    float pos[3];
    uint8_t col[4];
    float st[2];
} vtx;

struct gfx_clay {
    int nvert, nindex, nbatch;
    float *pos, *st;
    uint8_t *col, *drawcol;
    uint16_t *index;
    gfx_batch *batch;
    int noscroll;               /* UV scroll (fl 0x19) does not apply */
    int fog;                    /* drawn with fog while GFX_RS_FOG_ENABLE is on (gfx_clay_desc.fog) */
    /* GPU skinning (gfx_clay_set_skin): the regrouped batches, their
     * vertices in contiguous memory (SKIN_STRIDE bytes each), this frame's
     * pose */
    vtx *vb;                    /* static clays: vertices in contiguous memory */
    float center[3], radius;    /* bounding sphere */
    gfx_skin_mesh *skin;
    uint8_t *svb;
    int nbone;
    float *pose;                /* nbone * 16, or NULL = bind pose */
    int posed;
    gfx_light light;
};
#define SKIN_STRIDE 80          /* pos 3f, nrm 3f, col 4ub, st 2f, w 4f, slot*3 4f, flag 2f */


#define PITCH64(n) (((n) + 63u) & ~63u)

static struct {
    int w, h;
    int anchor;                     /* GFX_RS_2D_ANCHOR */
    float view[16], proj[16], world[16], texmat[16], viewport[16];
    uint32_t fade;
    uint32_t batch_hide;            /* GFX_RS_BATCH_HIDE */
    gfx_texture *batch_tex;         /* GFX_RS_BATCH_TEX */
    gfx_texture *tex, *white;
    int filter_point, clamp;
    uint8_t *ring;
    uint32_t ring_used;
    int zwrite, ztest;
    int fog_on;
    float fog_start, fog_end, fog_col[4];
    /* the states as set, for the bug reporter (gfx_pick_info) and to restore blending after its id pass */
    int blend_on, bsrc, bdst, bop, afunc, aref, zfunc;
    int pick_was;                   /* the last frame was an id pass */
    int rw, rh;                     /* the back buffer (G.w / G.h are the 512 x 448 frame while ps2 is on) */
    int ps2, ps2_done, ps2_ok;      /* this frame is the PS2's frame (gfx_opt.ps2_output) / already shown / extra buffer there */
    gfx_texture ps2_tex;            /* pbkit's extra buffer 0, where that frame is drawn, as a texture */
    float pick_col[4];              /* the current draw's id colour (pick pass) */
    uint32_t shown[2];              /* the last two frames handed to pbkit's flip (pb_finished): their buffers */
} G;

static const float ident[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
static const uint32_t blend_factor[6] = { 0, 1, 0x302, 0x303, 0x304, 0x305 };  /* GL enums, as NV2A takes them */

int gfx_tex_src_hint;
int gfx_tex_rgba_hint;

static void mat_mul(float *out, const float *a, const float *b)       /* out = a * b */
{
    float r[16];
    int i, j;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            r[4 * i + j] = a[4 * i] * b[j] + a[4 * i + 1] * b[4 + j] + a[4 * i + 2] * b[8 + j] + a[4 * i + 3] * b[12 + j];
    memcpy(out, r, sizeof r);
}

/* Widescreen (gfx_opt.widescreen, docs/xbox.md "Settings menu on the Xbox") is 16:9 anamorphic: the 640 x 480 frame is
 * stretched to 16:9 by the TV. The 3D fills it with a 16:9 projection (scene_aspect in viewer.c: the vertical field of
 * view stays, the horizontal one grows, as on the PC); the game's 4:3 2D sits on the middle 3/4 of the frame (x 80..560),
 * so it looks 4:3 again on the TV; HUD widgets anchored left / right (rt_2d.c) slide to the frame's edges, fades and
 * window-sized draws cover all of it. 4:3: the 2D covers the whole frame. */
float gfx_scene_aspect(void)
{
    return gfx_opt.widescreen && !G.ps2 ? 16.0f / 9.0f : 4.0f / 3.0f;
}

static void rect43(float *x, float *w)     /* the 4:3 picture in the frame: x offset and width */
{
    *w = gfx_opt.widescreen && !G.ps2 ? G.w * 0.75f : (float)G.w;
    *x = (G.w - *w) * 0.5f;
}

/* gfx.h: the game's 512 x 448 frame is the 4:3 picture (the whole frame, or its middle in widescreen) */
int gfx_project(const float in[3], float out[4])
{
    float a[4] = { in[0], in[1], in[2], 1.0f }, b[4], c[4];
    int j;
    for (j = 0; j < 4; j++)
        b[j] = a[0] * G.world[j] + a[1] * G.world[4 + j] + a[2] * G.world[8 + j] + a[3] * G.world[12 + j];
    for (j = 0; j < 4; j++)
        a[j] = b[0] * G.view[j] + b[1] * G.view[4 + j] + b[2] * G.view[8 + j] + b[3] * G.view[12 + j];
    for (j = 0; j < 4; j++)
        c[j] = a[0] * G.proj[j] + a[1] * G.proj[4 + j] + a[2] * G.proj[8 + j] + a[3] * G.proj[12 + j];
    out[3] = c[3];
    if (c[3] <= 1e-6f) {
        out[0] = out[1] = 0;
        out[2] = -1;
        return 0;
    }
    {
        float rx, rw;
        rect43(&rx, &rw);
        out[0] = ((c[0] / c[3] * 0.5f + 0.5f) * G.w - rx) * 512.0f / rw;
    }
    out[1] = (0.5f - c[1] / c[3] * 0.5f) * 448.0f;
    out[2] = c[2] / c[3] * 0.5f + 0.5f;
    return 1;
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
    /* COLOR1's alpha is the fog factor (vs.vs.cg, skin.vs.cg). The NV2A forces the specular alpha to 1 unless
     * LIGHT_CONTROL has ALPHA_FROM_MATERIAL_SPECULAR, also with vertex programs (xemu models it so from hardware
     * tests: CSV0_C bit 17, xemu commit 7a34eedd6f); with alpha 1 every fogged clay is drawn in the fog colour.
     * Nothing set it before 25 Oct 2026, so the fog depended on the register's power-on value. RT_NV2A_LC=hex
     * overrides the value (test aid). */
    {
        const char *lc = getenv("RT_NV2A_LC");
        p = pb_push1(p, NV097_SET_LIGHT_CONTROL, lc ? (uint32_t)strtoul(lc, NULL, 16)
                     : NV097_SET_LIGHT_CONTROL_V_ALPHA_FROM_MATERIAL_SPECULAR | NV097_SET_LIGHT_CONTROL_V_SEPARATE_SPECULAR);
    }
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

/* clip space -> window: x (-1..1) -> 0..w, y (1..-1) -> 0..h, z (-1..1) -> 0..ZMAX */
static void set_viewport(void)
{
    memset(G.viewport, 0, sizeof G.viewport);
    G.viewport[0] = G.w / 2.0f;
    G.viewport[5] = -G.h / 2.0f;
    G.viewport[10] = ZMAX / 2.0f;
    G.viewport[12] = G.w / 2.0f;
    G.viewport[13] = G.h / 2.0f;
    G.viewport[14] = ZMAX / 2.0f;
    G.viewport[15] = 1;
}

int gfx_init(int width, int height, const char *title, int hidden)
{
    static const uint8_t white[4] = { 255, 255, 255, 255 };
    (void)width; (void)height; (void)title; (void)hidden;

    XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);
    if (gfx_opt.ps2_output)
        pb_extra_buffers(1);                    /* the PS2 output's 512 x 448 frame is drawn there (ps2_present) */
    if (pb_init() != 0)
        return -1;
    pb_show_front_screen();
    G.w = G.rw = pb_back_buffer_width();
    G.h = G.rh = pb_back_buffer_height();
    if (gfx_opt.ps2_output && pb_extra_buffer(0) && G.w >= 512 && G.h >= 448) {
        G.ps2_ok = 1;
        G.ps2_tex.w = 512;
        G.ps2_tex.h = 448;
        G.ps2_tex.rect = 1;
        G.ps2_tex.xrgb = 1;
        G.ps2_tex.mem = (uint8_t *)pb_extra_buffer(0);
        G.ps2_tex.pitch = pb_back_buffer_pitch();
    }
    memcpy(G.view, ident, sizeof ident);
    memcpy(G.proj, ident, sizeof ident);
    memcpy(G.world, ident, sizeof ident);
    memcpy(G.texmat, ident, sizeof ident);
    set_viewport();
    G.fade = 0xFFFFFFFFu;
    G.ring = MmAllocateContiguousMemoryEx(RING_BYTES, 0, GPU_MAXRAM, 0, PAGE_READWRITE | PAGE_WRITECOMBINE);
    if (!G.ring)
        return -1;
    load_shaders();
    fixed_state();
    G.white = gfx_create_texture(1, 1, white);
    return 0;
}

static void gpu_free_pending(int all);

void gfx_shutdown(void)
{
    gpu_free_pending(1);
    pb_kill();
    if (G.ring)
        MmFreeContiguousMemory(G.ring);
    G.ring = NULL;
}

void gfx_size(int *w, int *h)
{
    *w = G.rw;          /* the screen, also while the PS2 output's frame is drawn */
    *h = G.rh;
}

/* The waits for the GPU give the CPU away (SwitchToThread): spinning here starved
 * the SDL audio thread on the real Xbox and in xemu (12 Oct 2026: no sound at all, the
 * movie fell back to the game tick, and closing the audio device on exit hung). It
 * played once the game also wrote every log line to the disk, which paused this thread. */
static void wait_idle(void)
{
    if (!pb_busy())
        return;
    rt_prof_begin(RTP_GPU_WAIT);
    rt_prof_count(RTPC_GPU_WAITS, 1);
    while (pb_busy())
        SwitchToThread();
    rt_prof_end(RTP_GPU_WAIT);
}

/* GPU memory given back late (textures, palettes, vertex buffers): NV2A caches texels and the palette by address and
 * does not notice the CPU writing a new texture where a freed one was (the vertex cache has a method for that,
 * NV097_BREAK_VERTEX_BUFFER_CACHE; no texture cache method is known here). Freed at once, a texture's memory was
 * often reused by the next screen's texture in the same tick, which could then show stale texels or palette colours
 * for its first frames on hardware (owner report, 12 Oct 2026: "screens flash as they fade in"; xemu re-reads memory
 * and showed every fade frame like the PC). Kept for DEFER_FRAMES drawn frames, then freed; when an allocation
 * fails, everything pending is freed first (the GPU waited for) and the allocation tried again. Also spares the
 * wait for the GPU at every release (a stage change releases hundreds). */
#define DEFER_FRAMES 3
static struct { void *p; uint32_t frame; } *deferred;
static int ndeferred, capdeferred;
static uint32_t frame_no;

static void gpu_free_pending(int all)
{
    int i, k = 0;
    if (!ndeferred)
        return;
    if (all)
        wait_idle();
    for (i = 0; i < ndeferred; i++)
        if (all || frame_no - deferred[i].frame >= DEFER_FRAMES)
            MmFreeContiguousMemory(deferred[i].p);
        else
            deferred[k++] = deferred[i];
    ndeferred = k;
}

static void gpu_free_later(void *p)
{
    if (!p)
        return;
    if (ndeferred == capdeferred) {
        int cap = capdeferred ? 2 * capdeferred : 256;
        void *n = realloc(deferred, sizeof *deferred * (size_t)cap);
        if (!n) {                           /* no room to remember it: the old way */
            wait_idle();
            MmFreeContiguousMemory(p);
            return;
        }
        deferred = n;
        capdeferred = cap;
    }
    deferred[ndeferred].p = p;
    deferred[ndeferred].frame = frame_no;
    ndeferred++;
}

static void *gpu_alloc(size_t n)
{
    void *p = MmAllocateContiguousMemoryEx(n, 0, GPU_MAXRAM, 0, PAGE_READWRITE | PAGE_WRITECOMBINE);
    if (!p && ndeferred) {
        gpu_free_pending(1);
        p = MmAllocateContiguousMemoryEx(n, 0, GPU_MAXRAM, 0, PAGE_READWRITE | PAGE_WRITECOMBINE);
    }
    return p;
}

void gfx_begin_frame(uint32_t c)
{
    /* Triple buffering: pbkit flips to a finished frame at the next vblank and rotates over three back buffers, so
     * the next one is free to draw while at most one finished frame still waits for its vblank (the screen shows the
     * last or the one before). Only with two waiting would it be the one on screen: then wait for a vblank. Until
     * 27 Oct 2026 every frame waited for a vblank here, which also held the CPU until the last frame was on screen
     * (half a 60 Hz refresh on average, 8 ms, in every frame slower than 60 fps). */
    {
        uint32_t front = *(volatile uint32_t *)(0xFD000000u + 0x00600800u) & 0x03FFFFFFu;     /* PCRTC_START */
        if (front != G.shown[0] && front != G.shown[1]) {
            rt_prof_begin(RTP_GPU_WAIT);
            rt_prof_count(RTPC_GPU_WAITS, 1);
            pb_wait_for_vbl();
            rt_prof_end(RTP_GPU_WAIT);
        }
    }
    pb_reset();
    /* the PS2 output (gfx_opt.ps2_output): the frame is drawn at the PS2's 512 x 448 in the extra buffer and shown by
     * ps2_present (not for the bug reporter's id pass, which works in screen pixels) */
    G.ps2 = G.ps2_ok && gfx_opt.ps2_output && !gfx_pick_pass;
    G.ps2_done = 0;
    G.w = G.ps2 ? 512 : G.rw;
    G.h = G.ps2 ? 448 : G.rh;
    set_viewport();
    if (G.ps2)
        pb_target_extra_buffer(0);
    else
        pb_target_back_buffer();
    wait_idle();
    frame_no++;
    gpu_free_pending(0);                       /* the GPU is idle: what was released DEFER_FRAMES frames ago goes */
    G.ring_used = 0;
    push1(NV097_SET_DEPTH_MASK, 1);
    pb_erase_depth_stencil_buffer(0, 0, G.w, G.h);
    if (gfx_pick_pass)
        c = 0;                                  /* the id pass: id 0 = nothing */
    pb_fill(0, 0, G.w, G.h, 0xFF000000u | (c & 0xFFFFFF));
    push1(NV097_SET_DEPTH_MASK, G.zwrite);
    if (gfx_pick_pass || G.pick_was) {          /* blending off for the id pass, back as the game set it after it */
        push1(NV097_SET_BLEND_ENABLE, G.blend_on && !gfx_pick_pass);
        G.pick_was = gfx_pick_pass;
    }
}

/* The PS2's picture (gfx_opt.ps2_output, as gfx_gl.c): the 512 x 448 frame in the extra buffer is drawn over the screen
 * twice, the second time one frame line lower at 50 %: the PCRTC merge the game sets up (flPS2SwapDBuff, video mode 1:
 * PMODE 0x7F27, read circuit 1 one line below circuit 2, ALP 0x7F); bilinear, 512 -> 640 as the TV stretches it to
 * 4:3. Then the screen size is back (gfx_read_pixels, the next frame). */
static void ps2_present(void)
{
    static const float st[12] = { 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1 };     /* (rect textures: scaled to texels by gfx_draw_2d) */
    float pos[12];
    uint8_t col[24];
    gfx_texture *tex = G.tex;
    int pass, i, anchor = G.anchor, fp = G.filter_point, clamp = G.clamp;
    uint32_t *p;
    if (!G.ps2 || G.ps2_done)
        return;
    G.ps2_done = 1;
    wait_idle();                                /* the frame is finished before it is read */
    pb_target_back_buffer();
    G.w = G.rw;
    G.h = G.rh;
    set_viewport();
    p = pb_begin();
    p = pb_push1(p, NV097_SET_ALPHA_FUNC, 0x207);                   /* always */
    p = pb_push1(p, NV097_SET_BLEND_EQUATION, NV097_SET_BLEND_EQUATION_V_FUNC_ADD);
    p = pb_push1(p, NV097_SET_BLEND_FUNC_SFACTOR, 0x302);
    p = pb_push1(p, NV097_SET_BLEND_FUNC_DFACTOR, 0x303);
    pb_end(p);
    G.tex = &G.ps2_tex;
    G.anchor = GFX_A_STRETCH;
    G.filter_point = 0;
    G.clamp = 1;
    for (pass = 0; pass < 2; pass++) {
        float y0 = pass ? G.h / 448.0f : 0.0f, y1 = y0 + G.h;
        float q[12] = { 0, y0, (float)G.w, y0, (float)G.w, y1, 0, y0, (float)G.w, y1, 0, y1 };
        memcpy(pos, q, sizeof pos);
        for (i = 0; i < 6; i++) {
            col[4 * i] = col[4 * i + 1] = col[4 * i + 2] = 255;
            col[4 * i + 3] = pass ? 128 : 255;
        }
        push1(NV097_SET_BLEND_ENABLE, pass);
        gfx_draw_2d(G.w, G.h, 6, pos, st, col);
    }
    G.tex = tex;
    G.anchor = anchor;
    G.filter_point = fp;
    G.clamp = clamp;
    p = pb_begin();                             /* the game's states as they were */
    p = pb_push1(p, NV097_SET_ALPHA_FUNC, 0x200 + (G.afunc & 7));
    p = pb_push1(p, NV097_SET_BLEND_ENABLE, G.blend_on && !gfx_pick_pass);
    p = pb_push1(p, NV097_SET_BLEND_FUNC_SFACTOR, blend_factor[G.bsrc < 6 ? G.bsrc : 2]);
    p = pb_push1(p, NV097_SET_BLEND_FUNC_DFACTOR, blend_factor[G.bdst < 6 ? G.bdst : 3]);
    p = pb_push1(p, NV097_SET_BLEND_EQUATION, (G.bop & 0xC00) == 0x400 ? NV097_SET_BLEND_EQUATION_V_FUNC_SUBTRACT
                 : (G.bop & 0xC00) == 0x800 ? NV097_SET_BLEND_EQUATION_V_FUNC_REVERSE_SUBTRACT
                 : NV097_SET_BLEND_EQUATION_V_FUNC_ADD);
    pb_end(p);
}

void gfx_end_frame(void)
{
    ps2_present();
    wait_idle();
    G.shown[1] = G.shown[0];
    G.shown[0] = (uint32_t)(uintptr_t)pb_back_buffer() & 0x03FFFFFFu;
    rt_prof_begin(RTP_GPU_WAIT);
    while (pb_finished())
        SwitchToThread();               /* see wait_idle */
    rt_prof_end(RTP_GPU_WAIT);
}

int gfx_read_pixels(uint8_t *rgb)
{
    const uint8_t *fb;
    int i;
    ps2_present();          /* the PS2 output: the screen as it will be shown */
    fb = (const uint8_t *)pb_back_buffer();
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

/* src (w x h texels of bpp bytes, rows packed) in NV2A swizzled order into dst (w * h * bpp
 * bytes). The swizzled x/y offsets step with (v - mask) & mask (the next value inside the
 * mask's bits). Callers write into cached memory and copy the result to the GPU's
 * write-combined memory in order: scattered single-texel writes to it were slow. */
static void swizzle(void *dst, const void *src, int w, int h, int bpp)
{
    uint32_t mx, my, sx, sy = 0;
    int x, y;
    swizzle_masks(w, h, &mx, &my);
    for (y = 0; y < h; y++) {
        sx = 0;
        if (bpp == 1) {
            const uint8_t *s = (const uint8_t *)src + (size_t)y * w;
            uint8_t *d = (uint8_t *)dst + sy;
            for (x = 0; x < w; x++) {
                d[sx] = s[x];
                sx = (sx - mx) & mx;
            }
        } else {
            const uint32_t *s = (const uint32_t *)src + (size_t)y * w;
            uint32_t *d = (uint32_t *)dst + sy;
            for (x = 0; x < w; x++) {
                d[sx] = s[x];
                sx = (sx - mx) & mx;
            }
        }
        sy = (sy - my) & my;
    }
}

static gfx_texture *gfx_create_texture_lt(int w, int h, const uint8_t *rgba)
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
    if (gfx_tex_rgba_hint) {            /* a movie frame: RGBA8, updated in place later (gfx_update_texture) */
        gfx_tex_rgba_hint = 0;
    } else if (!t->rect && w * h >= 256) {
        /* palettised when the image has <= 256 colours: 1 byte per texel
         * instead of 4 (NV2A SZ_I8_A8R8G8B8; swizzled, power-of-two only) */
        static uint32_t pal[256];
        uint8_t *idx = malloc((size_t)w * h);
        int np = idx ? gfx_to_indexed((const uint32_t *)rgba, w * h, pal, idx) : -1;
        if (np >= 0) {
            t->pal = gpu_alloc(1024);
            t->mem = t->pal ? gpu_alloc((size_t)w * h) : NULL;
        }
        uint8_t *sw = np >= 0 ? malloc((size_t)w * h) : NULL;
        if (np >= 0 && t->mem && sw) {
            uint32_t p[256];
            int i;
            for (i = 0; i < 256; i++) {          /* RGBA bytes -> A8R8G8B8 word */
                uint32_t c = i < np ? pal[i] : 0;
                p[i] = (c & 0xFF00FF00u) | (c & 0xFF) << 16 | (c >> 16 & 0xFF);
            }
            memcpy(t->pal, p, sizeof p);
            swizzle(sw, idx, w, h, 1);
            memcpy(t->mem, sw, (size_t)w * h);
            free(sw);
            free(idx);
            t->pitch = PITCH64((uint32_t)w);   /* unused when swizzled, but checked */
            t->bytes = (size_t)w * h + 1024;
            rt_ms_add("textures in GPU memory (P8 or RGBA8, GPU)", (long)t->bytes);
            rt_ms_add("textures as on disc (4/8-bit+CLUT, GPU)", t->src);
            return t;
        }
        free(sw);
        free(idx);
        if (t->mem)
            MmFreeContiguousMemory(t->mem);
        t->mem = NULL;
        if (t->pal)
            MmFreeContiguousMemory(t->pal);
        t->pal = NULL;
    }
    /* NV2A: a texture pitch must be a multiple of 64 bytes, or SET_TEXTURE_CONTROL1
     * stops the GPU ("invalid data", seen on the real Xbox on 11 Oct 2026 for
     * the 1x1 white texture: pitch 4). Linear rows are padded to it. */
    t->pitch = PITCH64((uint32_t)w * 4);
    size = (size_t)t->pitch * h;
    t->mem = gpu_alloc(size);
    if (!t->mem) {
        free(t);
        return NULL;
    }
    t->bytes = size;
    rt_ms_add("textures in GPU memory (P8 or RGBA8, GPU)", (long)size);
    rt_ms_add("textures as on disc (4/8-bit+CLUT, GPU)", t->src);
    if (t->rect) {
        int y;
        for (y = 0; y < h; y++)
            memcpy(t->mem + (size_t)y * t->pitch, rgba + (size_t)y * w * 4, (size_t)w * 4);
    } else {
        uint32_t *sw = malloc(size);
        if (sw) {
            swizzle(sw, rgba, w, h, 4);
            memcpy(t->mem, sw, size);
            free(sw);
        } else
            memset(t->mem, 0, size);
    }
    return t;
}

/* new texels in place for an RGBA8 texture of the same size (movie frames). The GPU is idle at each gfx_begin_frame
 * (wait_idle) and rt_movie.c rewrites the oldest of its three textures, last drawn two frames before. */
int gfx_update_texture(gfx_texture *t, const uint8_t *rgba)
{
    static uint32_t *scratch;
    static size_t scratch_n;
    size_t size;
    int y;
    if (!t || !t->mem || t->pal)
        return -1;
    rt_prof_count(RTPC_TEX_NEW, 1);
    rt_prof_count(RTPC_TEX_KB, (long)t->w * t->h * 4 / 1024);
    rt_lt_begin(RTL_TEXUP);
    size = (size_t)t->pitch * t->h;
    if (t->rect) {
        for (y = 0; y < t->h; y++)
            memcpy(t->mem + (size_t)y * t->pitch, rgba + (size_t)y * t->w * 4, (size_t)t->w * 4);
    } else {
        if (scratch_n < size) {
            free(scratch);
            scratch = malloc(size);
            scratch_n = scratch ? size : 0;
        }
        if (!scratch) {
            rt_lt_end(RTL_TEXUP);
            return -1;
        }
        swizzle(scratch, rgba, t->w, t->h, 4);   /* (scattered writes into write-combined memory would be slow) */
        memcpy(t->mem, scratch, size);
    }
    rt_lt_end(RTL_TEXUP);
    return 0;
}

gfx_texture * gfx_create_texture(int w, int h, const uint8_t *rgba)
{
    gfx_texture * r;
    rt_prof_count(RTPC_TEX_NEW, 1);
    rt_prof_count(RTPC_TEX_KB, (long)w * h * 4 / 1024);
    rt_lt_begin(RTL_TEXUP);
    r = gfx_create_texture_lt(w, h, rgba);
    rt_lt_end(RTL_TEXUP);
    return r;
}

void gfx_release_texture(gfx_texture *t)
{
    if (!t)
        return;
    rt_ms_add("textures in GPU memory (P8 or RGBA8, GPU)", -(long)t->bytes);
    rt_ms_add("textures as on disc (4/8-bit+CLUT, GPU)", -(long)t->src);
    gpu_free_later(t->mem);             /* not reused while the GPU may still read it or cache it */
    gpu_free_later(t->pal);
    if (G.tex == t)
        G.tex = NULL;
    free(t);
}

static void bind_texture(gfx_texture *t)
{
    uint32_t fmt, *p, wrap, filt;
    if (!t)
        t = G.white;
    if (t->xrgb)
        fmt = MASK(NV097_SET_TEXTURE_FORMAT_COLOR, NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X8R8G8B8);
    else if (t->rect)
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
        if (getenv("RT_FOG_TRACE"))     /* what the backend got (the PC's gfx_gl.c prints the same) */
            rt_log("nv2a fog: colour %02X%02X%02X start %g end %g", (int)(G.fog_col[0] * 255.0f + 0.5f),
                   (int)(G.fog_col[1] * 255.0f + 0.5f), (int)(G.fog_col[2] * 255.0f + 0.5f), G.fog_start, G.fog_end);
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
        G.aref = (int)(v & 255);
        push1(NV097_SET_ALPHA_REF, (uint32_t)(v & 255));
        break;
    case GFX_RS_ALPHA_FUNC:
        G.afunc = (int)(v & 7);
        push1(NV097_SET_ALPHA_FUNC, 0x200 + (v & 7));      /* the game's 0-7 are the GL compare enums in order */
        break;
    case GFX_RS_ZFUNC:
        G.zfunc = (int)v;
        push1(NV097_SET_DEPTH_FUNC, v == 1 ? 0x201 : v == 3 ? 0x203 : v == 7 ? 0x207 : 0x200);
        break;
    case GFX_RS_FADE_COLOR:
        G.fade = (uint32_t)v;
        break;
    case GFX_RS_BATCH_HIDE:
        G.batch_hide = (uint32_t)v;
        break;
    case GFX_RS_BATCH_TEX:
        G.batch_tex = (gfx_texture *)v;
        break;
    case GFX_RS_2D_ANCHOR:
        G.anchor = (int)v;
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
            G.blend_on = 1;
            G.bsrc = (int)src;
            G.bdst = (int)dst;
            p = pb_push1(p, NV097_SET_BLEND_ENABLE, !gfx_pick_pass);     /* the id pass draws without blending */
            p = pb_push1(p, NV097_SET_BLEND_FUNC_SFACTOR, blend_factor[src]);
            p = pb_push1(p, NV097_SET_BLEND_FUNC_DFACTOR, blend_factor[dst]);
            pb_end(p);
        }
        break;
    }
    case GFX_RS_BLEND_OP:
        G.bop = (int)v;
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
        G.blend_on = v != 0;
        G.bsrc = GFX_BF_SRC_ALPHA;
        G.bdst = GFX_BF_INV_SRC_ALPHA;
        p = pb_push1(p, NV097_SET_BLEND_ENABLE, v != 0 && !gfx_pick_pass);
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
static void fade_rgba(float out[4])
{
    int i;
    for (i = 0; i < 4; i++)
        out[i] = G.fade == 0xFFFFFFFFu ? 1.0f : ((G.fade >> (i == 3 ? 24 : 16 - 8 * i)) & 255) / 255.0f;
}

/* vs.vs.cg: c0 mvp, c4 texture matrix, c8 fog (k, b), c9 fog colour,
 * c10 fade colour, c11 (0, 1) cgc's own constant. fog / fade: 0 for 2D. */
static void upload_constants(const float *mvp, const float *tex, int fog)
{
    static const float c11[4] = { 0, 1, 0, 0 };
    float kb[4] = { 0, 0, 0, 0 }, fd[4] = { 1, 1, 1, 1 };
    uint32_t *p = pb_begin();
    if (fog && G.fog_on && G.fog_end != G.fog_start) {     /* GL linear fog, w as the eye distance */
        kb[0] = 1.0f / (G.fog_end - G.fog_start);
        kb[1] = -G.fog_start / (G.fog_end - G.fog_start);
    }
    if (fog)
        fade_rgba(fd);
    if (gfx_pick_pass) {        /* the id pass: the fog path paints the id colour (factor 1), no fade */
        kb[0] = 0;
        kb[1] = 1;
        fd[0] = fd[1] = fd[2] = fd[3] = 1;
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
    memcpy(p, gfx_pick_pass ? G.pick_col : G.fog_col, 16);
    p += 4;
    pb_push(p++, NV097_SET_TRANSFORM_CONSTANT, 4);
    memcpy(p, fd, 16);
    p += 4;
    pb_push(p++, NV097_SET_TRANSFORM_CONSTANT, 4);
    memcpy(p, c11, 16);
    p += 4;
    pb_end(p);
}

/* the texture matrix for T: rect textures take texel coordinates */
static void tex_matrix(const gfx_texture *t, float *out, const gfx_clay *c)
{
    memcpy(out, c->noscroll ? ident : G.texmat, 64);
    if (t && t->rect) {
        int i;
        for (i = 0; i < 4; i++) {
            out[4 * i] *= (float)t->w;
            out[4 * i + 1] *= (float)t->h;
        }
    }
}

/* ------------------------------------------------------------ the bug reporter's id pass (pick.c, rt_pick.c)
 * As gfx_gl.c: while gfx_pick_pass is set, every clay / 2D draw asks gfx_pick_cb for an id and is drawn flat in
 * that id's colour (red = bits 0-7, green 8-15, blue 16-23). The shaders are not changed: the fog path paints it
 * (fog colour = the id colour, fog factor 1: rgb = COLOR1.rgb exactly), the alpha stays texture alpha x vertex alpha
 * so the alpha test still cuts out leaves and fences; blending is off for the pass (gfx_set_render_state does not
 * switch it on, gfx_begin_frame switches it off and back). The game's z test and write stay. */
static void pick_fill(gfx_pick_info *gi, int is2d, int nvert, gfx_texture *t)
{
    memset(gi, 0, sizeof *gi);
    gi->is2d = is2d;
    gi->nvert = nvert;
    if (t) {
        gi->tex = (unsigned)(uintptr_t)t;       /* no GL texture names here: the texture's address */
        gi->tex_w = t->w;
        gi->tex_h = t->h;
    }
    gi->blend_on = G.blend_on;
    gi->bsrc = G.bsrc;
    gi->bdst = G.bdst;
    gi->bop = G.bop;
    gi->ztest = is2d ? 0 : G.ztest;
    gi->zwrite = G.zwrite;
    gi->zfunc = G.zfunc;
    gi->afunc = G.afunc;
    gi->aref = (float)G.aref;
    gi->nearest = G.filter_point;
    gi->clamp = G.clamp;
    gi->fog = G.fog_on;
    gi->fade = G.fade;
    memcpy(gi->texmat, G.texmat, sizeof gi->texmat);
    memcpy(gi->world, G.world, sizeof gi->world);
}

static int pick_use(const gfx_pick_info *gi)
{
    uint32_t id;
    if (!gfx_pick_cb || !(id = gfx_pick_cb(gi)))
        return 0;
    G.pick_col[0] = (id & 255) / 255.0f;
    G.pick_col[1] = ((id >> 8) & 255) / 255.0f;
    G.pick_col[2] = ((id >> 16) & 255) / 255.0f;
    G.pick_col[3] = 1;
    return 1;
}

/* the view / projection of the id pass, for turning a click back into a world position (pick.c) */
void gfx_pick_matrices(float view[16], float proj[16])
{
    memcpy(view, G.view, sizeof G.view);
    memcpy(proj, G.proj, sizeof G.proj);
}

/* pbkit keeps its depth buffer's address to itself (and NV2A may store it tiled / compressed): no depth here, so an
 * Xbox report has no hit position or nearest bone (pick.c leaves them out) */
int gfx_read_depth(float *d)
{
    (void)d;
    return -1;
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

/* NV2A keeps fetched vertices in a cache keyed by memory address and does not
 * notice the CPU rewriting that memory. The vertex ring starts at offset 0
 * every frame, so without this a draw reads the vertices an earlier frame
 * left at the same address: on the real Xbox (11 Oct 2026) the CAPCOM logo's
 * first quad came out with the ACCESS screen's full-screen quad (texture
 * spread over the screen) and its second quad with the fade's vertices
 * (confirmed on hardware the same day: one centred logo after this fix). Broken before every
 * draw's arrays: one method, and static or reused buffers are covered too. */
static uint32_t *break_vertex_cache(uint32_t *p)
{
    return pb_push1(p, NV097_BREAK_VERTEX_BUFFER_CACHE, 0);
}

static void set_arrays(const vtx *v)
{
    uint32_t base = (uint32_t)(uintptr_t)v & 0x03ffffff, *p;
    int i;
    p = pb_begin();
    p = break_vertex_cache(p);
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
    rt_prof_count(RTPC_DRAWS, 1);
    rt_prof_count(RTPC_DRAW_TRIS, n / 3);
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
    cmd_kick();                 /* the GPU starts on the draw while the CPU sets up the next */
}

static void draw_arrays(int n)
{
    uint32_t *p;
    int i = 0;
    rt_prof_count(RTPC_DRAWS, 1);
    rt_prof_count(RTPC_DRAW_TRIS, n / 3);
    rt_prof_count(RTPC_DRAW_VERTS, n);
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
    cmd_kick();                 /* the GPU starts on the draw while the CPU sets up the next */
}

gfx_clay *gfx_create_clay(const gfx_clay_desc *d)
{
    gfx_clay *c = calloc(1, sizeof *c);
    c->nvert = d->nvert;
    c->nindex = d->nindex;
    c->nbatch = d->nbatch;
    c->noscroll = d->noscroll;
    c->fog = d->fog;
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
    {   /* bounding sphere (for the near-plane check) */
        float lo[3] = { 1e30f, 1e30f, 1e30f }, hi[3] = { -1e30f, -1e30f, -1e30f }, r2 = 0;
        int i, k;
        for (i = 0; i < d->nvert; i++)
            for (k = 0; k < 3; k++) {
                if (d->pos[3 * i + k] < lo[k]) lo[k] = d->pos[3 * i + k];
                if (d->pos[3 * i + k] > hi[k]) hi[k] = d->pos[3 * i + k];
            }
        for (k = 0; k < 3; k++)
            c->center[k] = d->nvert ? (lo[k] + hi[k]) * 0.5f : 0;
        for (i = 0; i < d->nvert; i++) {
            float dx = d->pos[3 * i] - c->center[0], dy = d->pos[3 * i + 1] - c->center[1], dz = d->pos[3 * i + 2] - c->center[2];
            if (dx * dx + dy * dy + dz * dz > r2)
                r2 = dx * dx + dy * dy + dz * dz;
        }
        c->radius = sqrtf(r2);
    }
    if (!d->dynamic && d->nvert > 0) {      /* static geometry (the stage): its vertices stay in GPU-visible memory */
        c->vb = gpu_alloc(sizeof(vtx) * (size_t)d->nvert);
        if (c->vb) {
            int i;
            rt_ms_add("static vertex buffers (GPU)", (long)(sizeof(vtx) * (size_t)d->nvert));
            for (i = 0; i < d->nvert; i++) {
                memcpy(c->vb[i].pos, c->pos + 3 * i, 12);
                memcpy(c->vb[i].col, c->col + 4 * i, 4);
                if (c->st)
                    memcpy(c->vb[i].st, c->st + 2 * i, 8);
                else
                    c->vb[i].st[0] = c->vb[i].st[1] = 0;
            }
        }
    }
    return c;
}

void gfx_update_clay(gfx_clay *c, const float *pos, const uint8_t *col)
{
    if (pos)
        memcpy(c->pos, pos, sizeof(float) * 3 * c->nvert);
    if (col)
        memcpy(c->col, col, 4 * (size_t)c->nvert);
    if (c->vb) {                            /* a static clay changed after all: refresh its buffer */
        int i;
        wait_idle();
        for (i = 0; i < c->nvert; i++) {
            memcpy(c->vb[i].pos, c->pos + 3 * i, 12);
            memcpy(c->vb[i].col, c->col + 4 * i, 4);
        }
    }
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

/* the clay's bounding sphere entirely at w >= CLIP_W? (w = p . column 3 of mvp) */
static int sphere_in_front(const gfx_clay *c, const float *mvp)
{
    float wc = clip_w_of(mvp, c->center);
    float len = sqrtf(mvp[3] * mvp[3] + mvp[7] * mvp[7] + mvp[11] * mvp[11]);
    return wc - c->radius * len >= CLIP_W;
}

static void execute_cpu(gfx_clay *c)
{
    float mvp[16], tm[16];
    int i, k, b, ncross = 0, nv;
    float *w = NULL;
    const uint16_t *clip_idx = NULL;
    vtx *v;

    if (!c->nvert)
        return;
    mat_mul(mvp, G.world, G.view);          /* row vectors: v * world * view * proj * viewport */
    mat_mul(mvp, mvp, G.proj);
    mat_mul(mvp, mvp, G.viewport);
    /* which triangles cross the near plane (w per vertex: one dot product);
     * a clay whose bounding sphere is in front of it is not checked */
    if (c->nbatch <= 256 && !sphere_in_front(c, mvp) && (w = malloc(sizeof(float) * c->nvert)) != NULL) {
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
    if (!ncross && c->vb) {                 /* static geometry: drawn from its own buffer */
        set_arrays(c->vb);
        for (b = 0; b < c->nbatch; b++) {
            gfx_texture *t = G.batch_tex ? G.batch_tex : c->batch[b].tex ? c->batch[b].tex : G.tex;
            if (b < 32 && (G.batch_hide >> b & 1))
                continue;
            if (!c->st)
                t = NULL;
            bind_texture(t);
            tex_matrix(t, tm, c);
            upload_constants(mvp, tm, 1);
            draw_indexed(c->index + c->batch[b].first, c->batch[b].count);
        }
        return;
    }
    nv = c->nvert + 2 * ncross;
    if (!(v = ring_alloc(nv))) {
        free(w);
        return;
    }
    for (i = 0; i < c->nvert; i++) {
        memcpy(v[i].pos, c->pos + 3 * i, 12);
        for (k = 0; k < 4; k++)
            v[i].col[k] = c->col[4 * i + k];       /* the fade colour is a shader constant */
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
        gfx_texture *t = G.batch_tex ? G.batch_tex : c->batch[b].tex ? c->batch[b].tex : G.tex;
        if (b < 32 && (G.batch_hide >> b & 1))
            continue;
        if (!c->st)
            t = NULL;
        bind_texture(t);
        tex_matrix(t, tm, c);
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
    rt_prof_count(RTPC_DRAWS_2D, 1);
    if (gfx_pick_pass) {
        gfx_pick_info gi;
        pick_fill(&gi, 1, nvert, t);
        gi.sw = w;
        gi.sh = h;
        gi.bbox2d[0] = gi.bbox2d[1] = 1e30f;
        gi.bbox2d[2] = gi.bbox2d[3] = -1e30f;
        for (i = 0; i < nvert; i++) {
            if (pos[2 * i] < gi.bbox2d[0]) gi.bbox2d[0] = pos[2 * i];
            if (pos[2 * i] > gi.bbox2d[2]) gi.bbox2d[2] = pos[2 * i];
            if (pos[2 * i + 1] < gi.bbox2d[1]) gi.bbox2d[1] = pos[2 * i + 1];
            if (pos[2 * i + 1] > gi.bbox2d[3]) gi.bbox2d[3] = pos[2 * i + 1];
        }
        if (!pick_use(&gi))
            return;
    }
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
    /* virtual w x h screen (origin top left) to frame pixels: on the 4:3 picture; anchored HUD widgets at its edges,
     * stretched draws (fades, a frame-sized virtual screen) over the whole frame (see gfx_scene_aspect) */
    memcpy(m, ident, sizeof m);
    m[5] = (float)G.h / h;
    if (G.anchor == GFX_A_STRETCH || (w == G.w && h == G.h) || !gfx_opt.widescreen || G.ps2) {
        m[0] = (float)G.w / w;
    } else {
        float rx, rw;
        rect43(&rx, &rw);
        m[0] = rw / w;
        m[12] = G.anchor == GFX_A_LEFT ? 0.0f : G.anchor == GFX_A_RIGHT ? G.w - rw : rx;
    }
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
    gpu_free_later(c->vb);
    if (c->skin) {
        gfx_skin_free(c->skin);
        free(c->skin);
        gpu_free_later(c->svb);
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
    c->svb = gpu_alloc((size_t)m->nv * SKIN_STRIDE);
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
    p = break_vertex_cache(p);     /* see set_arrays */
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
    if (gfx_pick_pass) {                    /* the id pass (see upload_constants) */
        k[32] = 0;
        k[33] = 1;
        memcpy(k + 36, G.pick_col, 12);
        k[76] = k[77] = k[78] = k[79] = 1;
    }
    use_program(1);
    push_consts(92, c91, 1);
    for (b = 0; b < m->nbatch; b++) {
        const gfx_skin_batch *bt = &m->batch[b];
        gfx_texture *t = G.batch_tex ? G.batch_tex : bt->tex ? bt->tex : G.tex;
        if (bt->src < 32 && (G.batch_hide >> bt->src & 1))
            continue;
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
        tex_matrix(t, tm, c);
        memcpy(k + 16, tm, 64);
        push_consts(0, k, 20);
        if (bt->nbone)
            push_consts(20, rows, 3 * bt->nbone);
        set_skin_arrays(c->svb + (size_t)bt->vfirst * SKIN_STRIDE);
        draw_indexed(m->index + bt->first, bt->count);
    }
    use_program(0);
}

static void execute_clay(gfx_clay *c);
void gfx_execute_clay(gfx_clay *c)
{
    int on = G.fog_on;
    G.fog_on = on && c->fog;            /* the scene's fog, for the clays whose attribute asks for it */
    execute_clay(c);
    G.fog_on = on;
}

static void execute_clay(gfx_clay *c)
{
    float mvp[16];
    if (gfx_rec_clay(c))
        return;
    if (gfx_pick_pass) {                    /* the bug reporter's id pass: one id per clay (as gfx_gl.c) */
        gfx_pick_info gi;
        pick_fill(&gi, 0, c->nvert, c->nbatch ? (c->batch[0].tex ? c->batch[0].tex : G.tex) : G.tex);
        gi.noscroll = c->noscroll;
        gi.clay = c;
        if (!pick_use(&gi))
            return;
    }
    if (!c->skin || !c->posed) {
        execute_cpu(c);
        return;
    }
    /* skinned and lit by skin.vs.cg */
    mat_mul(mvp, G.world, G.view);
    mat_mul(mvp, mvp, G.proj);
    mat_mul(mvp, mvp, G.viewport);
    draw_skinned(c, mvp);
}

/* ------------------------------------------------------------ video */

