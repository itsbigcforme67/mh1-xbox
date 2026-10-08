/*
 * gfx_gl.c - gfx.h on OpenGL 1.x fixed function (SDL2 window + context).
 *
 * Only GL 1.1-era calls are used (client vertex arrays, glTexImage2D,
 * glAlphaFunc, glFog) so the same structure maps onto the Xbox NV2A /
 * Direct3D 8 later. Matrices are fl-style row-vector 4x4 (v' = v * M),
 * which is exactly OpenGL's column-major memory layout, so they load as-is.
 */
#include "gfx.h"
#include "../rt/rt_prof.h"
#include "../rt/rt_log.h"

#include <SDL.h>
#include <GL/gl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct gfx_texture { GLuint id; int w, h, src; long xbox; };

struct gfx_clay {
    int nvert, nindex, nbatch;
    float *pos, *st;
    uint8_t *col, *drawcol;
    uint16_t *index;
    gfx_batch *batch;
    int noscroll;               /* UV scroll (fl 0x19) does not apply */
};

static struct {
    SDL_Window *win;
    SDL_GLContext ctx;
    int w, h;
    float view[16], proj[16], world[16], texmat[16];
    unsigned afunc;              /* GL alpha func (fl 0x5F) */
    float aref;
    uint32_t fade;               /* 0xAARRGGBB, 0xFFFFFFFF = none */
    uint32_t batch_hide;         /* GFX_RS_BATCH_HIDE */
    gfx_texture *tex;
    GLint filter, wrap;          /* fl 0x63 / 0x64, applied when a texture is bound */
    void (APIENTRY *blend_eq)(GLenum);   /* glBlendEquation (GL 1.4), may be NULL */
    /* the states as set, for the bug reporter's pick info (gfx_pick_info) */
    int blend_on, bsrc, bdst, bop, ztest, zwrite, zfunc, fog;
    GLuint white;
} G;


#ifndef GL_FUNC_ADD
#define GL_FUNC_ADD 0x8006
#define GL_FUNC_SUBTRACT 0x800A
#define GL_FUNC_REVERSE_SUBTRACT 0x800B
#endif

#ifndef GL_CLAMP_TO_EDGE        /* GL 1.2: not in the Windows opengl32 header (GL 1.1) */
#define GL_CLAMP_TO_EDGE 0x812F
#endif

/* fl blend factor codes (GFX_BF_*) */
static const GLenum blend_factor[6] = {
    GL_ZERO, GL_ONE, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA
};

static const float ident[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };

int gfx_init(int width, int height, const char *title, int hidden)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL_Init: %s", SDL_GetError());
        return -1;
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    G.win = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                             width, height, SDL_WINDOW_OPENGL | (hidden ? SDL_WINDOW_HIDDEN : 0));
    if (!G.win) {
        SDL_Log("SDL_CreateWindow: %s", SDL_GetError());
        return -1;
    }
    G.ctx = SDL_GL_CreateContext(G.win);
    if (!G.ctx) {
        SDL_Log("SDL_GL_CreateContext: %s", SDL_GetError());
        return -1;
    }
    SDL_GL_SetSwapInterval(1);
    {
        const char *v = (const char *)glGetString(GL_VENDOR), *r = (const char *)glGetString(GL_RENDERER),
                   *ver = (const char *)glGetString(GL_VERSION);
        SDL_version cv, rv;
        SDL_VERSION(&cv);
        SDL_GetVersion(&rv);
        rt_log("GPU: %s / %s, OpenGL %s", v ? v : "?", r ? r : "?", ver ? ver : "?");
        rt_log("SDL %d.%d.%d (built with %d.%d.%d), video driver %s", rv.major, rv.minor, rv.patch, cv.major, cv.minor, cv.patch,
               SDL_GetCurrentVideoDriver() ? SDL_GetCurrentVideoDriver() : "?");
    }
    G.w = width;
    G.h = height;
    memcpy(G.view, ident, sizeof ident);
    memcpy(G.proj, ident, sizeof ident);
    memcpy(G.world, ident, sizeof ident);
    memcpy(G.texmat, ident, sizeof ident);
    G.fade = 0xFFFFFFFFu;

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);           /* strip winding is not consistent */
    glEnable(GL_ALPHA_TEST);
    G.afunc = GL_GREATER;
    G.aref = 0.0f;
    glAlphaFunc(G.afunc, G.aref);
    glDisable(GL_LIGHTING);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    G.filter = GL_LINEAR;
    G.wrap = GL_REPEAT;
    G.ztest = 1;
    G.zwrite = 1;
    G.zfunc = 3;
    G.bsrc = GFX_BF_SRC_ALPHA;
    G.bdst = GFX_BF_INV_SRC_ALPHA;
    G.afunc = 0x204;
    G.blend_eq = (void (APIENTRY *)(GLenum))SDL_GL_GetProcAddress("glBlendEquation");
    return 0;
}

void gfx_shutdown(void)
{
    if (G.ctx)
        SDL_GL_DeleteContext(G.ctx);
    if (G.win)
        SDL_DestroyWindow(G.win);
    SDL_Quit();
    memset(&G, 0, sizeof G);
}

void gfx_size(int *w, int *h)
{
    *w = G.w;
    *h = G.h;
}

static void gfx_begin_frame_gl(uint32_t c)
{
    glDepthMask(GL_TRUE);
    if (gfx_pick_pass)
        c = 0;                  /* the id pass: id 0 = nothing */
    glClearColor(((c >> 16) & 255) / 255.0f, ((c >> 8) & 255) / 255.0f, (c & 255) / 255.0f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void gfx_begin_frame(uint32_t c)
{
    rt_prof_begin(RTP_GFX);
    gfx_begin_frame_gl(c);
    rt_prof_end(RTP_GFX);
}

void gfx_end_frame(void)
{
    SDL_GL_SwapWindow(G.win);
}

int gfx_read_pixels(uint8_t *rgb)
{
    int y;
    size_t row = (size_t)G.w * 3;
    uint8_t *tmp = malloc(row);
    glFinish();
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadBuffer(GL_BACK);
    glReadPixels(0, 0, G.w, G.h, GL_RGB, GL_UNSIGNED_BYTE, rgb);
    for (y = 0; y < G.h / 2; y++) {          /* GL is bottom-up */
        memcpy(tmp, rgb + y * row, row);
        memcpy(rgb + y * row, rgb + (G.h - 1 - y) * row, row);
        memcpy(rgb + (G.h - 1 - y) * row, tmp, row);
    }
    free(tmp);
    return 0;
}

int gfx_read_depth(float *d)
{
    int y;
    size_t row = (size_t)G.w;
    float *tmp = malloc(row * sizeof(float));
    glFinish();
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadBuffer(GL_BACK);
    glReadPixels(0, 0, G.w, G.h, GL_DEPTH_COMPONENT, GL_FLOAT, d);
    for (y = 0; y < G.h / 2; y++) {
        memcpy(tmp, d + y * row, row * sizeof(float));
        memcpy(d + y * row, d + (G.h - 1 - y) * row, row * sizeof(float));
        memcpy(d + (G.h - 1 - y) * row, tmp, row * sizeof(float));
    }
    free(tmp);
    return 0;
}

int gfx_tex_src_hint;
gfx_texture *gfx_create_texture(int w, int h, const uint8_t *rgba)
{
    gfx_texture *t = calloc(1, sizeof *t);
    t->w = w;
    t->h = h;
    t->src = gfx_tex_src_hint;
    gfx_tex_src_hint = 0;
    rt_ms_add("textures in GPU memory (RGBA8, GPU)", (long)w * h * 4);
    rt_ms_add("textures as on disc (4/8-bit+CLUT, GPU)", t->src);
    {   /* the Xbox estimate costs a colour count per texture: only for RT_MEM */
        static int mem = -1;
        if (mem < 0)
            mem = getenv("RT_MEM") != NULL;
        t->xbox = mem ? gfx_xbox_texture_bytes(w, h, rgba) : 0;
    }
    rt_ms_add("textures as the Xbox keeps them (P8/RGBA8, GPU)", t->xbox);
    glGenTextures(1, &t->id);
    glBindTexture(GL_TEXTURE_2D, t->id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
    return t;
}

void gfx_release_texture(gfx_texture *t)
{
    if (!t)
        return;
    glDeleteTextures(1, &t->id);
    rt_ms_add("textures in GPU memory (RGBA8, GPU)", -(long)t->w * t->h * 4);
    rt_ms_add("textures as on disc (4/8-bit+CLUT, GPU)", -(long)t->src);
    rt_ms_add("textures as the Xbox keeps them (P8/RGBA8, GPU)", -t->xbox);
    free(t);
}

void gfx_set_render_state(int state, uintptr_t v)
{
    if (gfx_rec_state(state, v))
        return;
    switch (state) {
    case GFX_RS_TEXTURE:
        G.tex = (gfx_texture *)v;
        break;
    case GFX_RS_FOG_COLOR: {
        float c[4] = { ((v >> 16) & 255) / 255.0f, ((v >> 8) & 255) / 255.0f, (v & 255) / 255.0f, 1 };
        glFogfv(GL_FOG_COLOR, c);
        break;
    }
    case GFX_RS_FOG_START:
        glFogf(GL_FOG_START, *(const float *)v);
        break;
    case GFX_RS_FOG_END:
        glFogf(GL_FOG_END, *(const float *)v);
        break;
    case GFX_RS_FOG_ENABLE:
        G.fog = v != 0;
        if (v) glEnable(GL_FOG); else glDisable(GL_FOG);
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
        G.aref = (float)(v & 255) / 255.0f;
        glAlphaFunc(G.afunc, G.aref);
        break;
    case GFX_RS_ALPHA_FUNC:
        G.afunc = 0x200 + (v & 7);      /* the game's 0-7 are GL_NEVER .. GL_ALWAYS in order */
        glAlphaFunc(G.afunc, G.aref);
        break;
    case GFX_RS_ZFUNC:
        G.zfunc = (int)v;
        glDepthFunc(v == 1 ? GL_LESS : v == 3 ? GL_LEQUAL : v == 7 ? GL_ALWAYS : GL_NEVER);
        break;
    case GFX_RS_FADE_COLOR:
        G.fade = (uint32_t)v;
        break;
    case GFX_RS_BATCH_HIDE:
        G.batch_hide = (uint32_t)v;
        break;
    case GFX_RS_ZWRITE:
        G.zwrite = v != 0;
        glDepthMask(v ? GL_TRUE : GL_FALSE);
        break;
    case GFX_RS_ZTEST:
        G.ztest = v != 0;
        if (v) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
        break;
    case GFX_RS_BLEND_FUNC: {
        unsigned src = v & 15, dst = (v >> 4) & 15;
        if (src < 6 && dst < 6) {        /* others have no GS form: ignored like fl does */
            G.blend_on = 1;
            G.bsrc = (int)src;
            G.bdst = (int)dst;
            glEnable(GL_BLEND);
            glBlendFunc(blend_factor[src], blend_factor[dst]);
        }
        break;
    }
    case GFX_RS_BLEND_OP:
        G.bop = (int)v;
        if (G.blend_eq)
            G.blend_eq((v & 0xC00) == 0x400 ? GL_FUNC_SUBTRACT
                       : (v & 0xC00) == 0x800 ? GL_FUNC_REVERSE_SUBTRACT : GL_FUNC_ADD);
        break;
    case GFX_RS_FILTER:
        G.filter = (v & 0x10000) ? GL_NEAREST : GL_LINEAR;
        break;
    case GFX_RS_TEX_CLAMP:
        G.wrap = v ? GL_CLAMP_TO_EDGE : GL_REPEAT;
        break;
    case GFX_RS_BLEND:
        G.blend_on = v != 0;
        G.bsrc = GFX_BF_SRC_ALPHA;
        G.bdst = GFX_BF_INV_SRC_ALPHA;
        if (v) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        } else {
            glDisable(GL_BLEND);
        }
        break;
    }
}

void gfx_set_render_state_f(int state, float value)
{
    gfx_set_render_state(state, (uintptr_t)&value);
}

gfx_clay *gfx_create_clay(const gfx_clay_desc *d)
{
    gfx_clay *c = calloc(1, sizeof *c);
    c->nvert = d->nvert;
    c->nindex = d->nindex;
    c->nbatch = d->nbatch;
    c->noscroll = d->noscroll;
    c->pos = malloc(sizeof(float) * 3 * (d->nvert + 1));
    memcpy(c->pos, d->pos, sizeof(float) * 3 * d->nvert);
    if (d->st) {
        c->st = malloc(sizeof(float) * 2 * (d->nvert + 1));
        memcpy(c->st, d->st, sizeof(float) * 2 * d->nvert);
    }
    c->col = malloc(4 * (size_t)(d->nvert + 1));
    c->drawcol = malloc(4 * (size_t)(d->nvert + 1));
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


/* ------------------------------------------------------------ id pass (bug reporter, pick.c)
 * Flat id colours: no blending / fog / dither, textures keep their alpha (alpha test, vertex alpha) but the
 * colour is the id (texture environment: combine, constant colour). */
#ifndef GL_COMBINE
#define GL_COMBINE 0x8570
#define GL_COMBINE_RGB 0x8571
#define GL_COMBINE_ALPHA 0x8572
#define GL_SOURCE0_RGB 0x8580
#define GL_SOURCE1_RGB 0x8581
#define GL_SOURCE0_ALPHA 0x8588
#define GL_SOURCE1_ALPHA 0x8589
#define GL_OPERAND0_RGB 0x8590
#define GL_OPERAND0_ALPHA 0x8598
#define GL_OPERAND1_ALPHA 0x8599
#define GL_CONSTANT 0x8576
#define GL_PRIMARY_COLOR 0x8577
#endif

static void pick_gl_on(uint32_t id)
{
    float c[4] = { (id & 255) / 255.0f, ((id >> 8) & 255) / 255.0f, ((id >> 16) & 255) / 255.0f, 1.0f };
    if (!G.white) {
        static const uint8_t w[4] = { 255, 255, 255, 255 };
        glGenTextures(1, &G.white);
        glBindTexture(GL_TEXTURE_2D, G.white);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, w);
    }
    glDisable(GL_FOG);
    glDisable(GL_BLEND);
    glDisable(GL_DITHER);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_CONSTANT);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_TEXTURE);
    glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA, GL_PRIMARY_COLOR);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
    glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA);
    glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, c);
}

static void pick_gl_off(void)
{
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_DITHER);
    if (G.fog) glEnable(GL_FOG);
    if (G.blend_on) glEnable(GL_BLEND);
}

static void pick_fill(gfx_pick_info *gi, int is2d, int nvert, gfx_texture *t)
{
    memset(gi, 0, sizeof *gi);
    gi->is2d = is2d;
    gi->nvert = nvert;
    if (t) {
        gi->tex = t->id;
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
    gi->afunc = G.afunc - 0x200;
    gi->aref = G.aref;
    gi->nearest = G.filter == GL_NEAREST;
    gi->clamp = G.wrap != GL_REPEAT;
    gi->fog = G.fog;
    gi->fade = G.fade;
    memcpy(gi->texmat, G.texmat, sizeof gi->texmat);
    memcpy(gi->world, G.world, sizeof gi->world);
}

/* the view / projection of the id pass for unprojecting a click (pick.c) */
void gfx_pick_matrices(float view[16], float proj[16])
{
    memcpy(view, G.view, sizeof G.view);
    memcpy(proj, G.proj, sizeof G.proj);
}

static void gfx_execute_clay_gl(gfx_clay *c)
{
    if (gfx_rec_clay(c))
        return;
    int b;
    uint32_t pid = 0;
    if (gfx_pick_pass) {
        gfx_pick_info gi;
        pick_fill(&gi, 0, c->nvert, c->nbatch ? (c->batch[0].tex ? c->batch[0].tex : G.tex) : G.tex);
        gi.noscroll = c->noscroll;
        gi.clay = c;
        if (!gfx_pick_cb || !(pid = gfx_pick_cb(&gi)))
            return;
    }
    rt_prof_count(RTPC_DRAW_VERTS, c->nvert);
    for (b = 0; b < c->nbatch; b++)
        rt_prof_count(RTPC_DRAW_TRIS, c->batch[b].count / 3);
    rt_prof_count(RTPC_DRAWS, c->nbatch);
    const uint8_t *col = c->col;

    if (pid)
        pick_gl_on(pid);
    if (G.fade != 0xFFFFFFFFu && !pid) {            /* fl state 0x67: per-draw multiply */
        unsigned f[4] = { (G.fade >> 16) & 255, (G.fade >> 8) & 255, G.fade & 255, G.fade >> 24 };
        int i, k;
        for (i = 0; i < c->nvert; i++)
            for (k = 0; k < 4; k++)
                c->drawcol[4 * i + k] = (uint8_t)(c->col[4 * i + k] * f[k] / 255);
        col = c->drawcol;
    }
    if (getenv("RT_UV_TRACE") && !c->noscroll && memcmp(G.texmat, ident, sizeof ident)) {
        static const void *seen[64]; static int ns; int k;
        for (k = 0; k < ns && seen[k] != c; k++) ;
        if (k == ns && ns < 64) {
            seen[ns++] = c;
            fprintf(stderr, "uv scroll applied to clay %p: %d verts, %d batches, tex %p, matrix t=(%.3f %.3f)\n", (const void *)c, c->nvert, c->nbatch,
                    (void *)(c->nbatch ? c->batch[0].tex : 0), G.texmat[12], G.texmat[13]);
        }
    }
    glMatrixMode(GL_TEXTURE);
    glLoadMatrixf(c->noscroll ? ident : G.texmat);       /* fl 0x19: UV scroll (set14), only for parts whose attribute asks for it */
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(G.proj);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(G.view);
    glMultMatrixf(G.world);        /* GL: M = view * world; row-vector world*view */

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, c->pos);
    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, col);
    if (c->st) {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 0, c->st);
    } else {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
    for (b = 0; b < c->nbatch; b++) {
        gfx_texture *t = c->batch[b].tex ? c->batch[b].tex : G.tex;
        if (b < 32 && (G.batch_hide >> b & 1))
            continue;
        if (t && c->st) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, t->id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, G.filter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, G.filter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, G.wrap);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, G.wrap);
        } else if (pid) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, G.white);
        } else {
            glDisable(GL_TEXTURE_2D);
        }
        glDrawElements(GL_TRIANGLES, c->batch[b].count, GL_UNSIGNED_SHORT, c->index + c->batch[b].first);
    }
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    if (pid)
        pick_gl_off();
}

void gfx_execute_clay(gfx_clay *c)
{
    rt_prof_begin(RTP_GFX);
    gfx_execute_clay_gl(c);
    rt_prof_end(RTP_GFX);
}

static void gfx_draw_2d_gl(int w, int h, int nvert, const float *pos, const float *st, const uint8_t *col)
{
    if (gfx_rec_2d(w, h, nvert, pos, st, col))
        return;
    rt_prof_count(RTPC_DRAW_VERTS, nvert);
    rt_prof_count(RTPC_DRAW_TRIS, nvert / 3);
    rt_prof_count(RTPC_DRAWS, 1);
    gfx_texture *t = st ? G.tex : NULL;
    uint32_t pid = 0;
    if (gfx_pick_pass) {
        gfx_pick_info gi;
        int k;
        pick_fill(&gi, 1, nvert, t);
        gi.sw = w;
        gi.sh = h;
        gi.bbox2d[0] = gi.bbox2d[1] = 1e30f;
        gi.bbox2d[2] = gi.bbox2d[3] = -1e30f;
        for (k = 0; k < nvert; k++) {
            if (pos[2 * k] < gi.bbox2d[0]) gi.bbox2d[0] = pos[2 * k];
            if (pos[2 * k] > gi.bbox2d[2]) gi.bbox2d[2] = pos[2 * k];
            if (pos[2 * k + 1] < gi.bbox2d[1]) gi.bbox2d[1] = pos[2 * k + 1];
            if (pos[2 * k + 1] > gi.bbox2d[3]) gi.bbox2d[3] = pos[2 * k + 1];
        }
        if (!gfx_pick_cb || !(pid = gfx_pick_cb(&gi)))
            return;
    }
    GLboolean dt = glIsEnabled(GL_DEPTH_TEST);
    GLboolean dm;
    glGetBooleanv(GL_DEPTH_WRITEMASK, &dm);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, h, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    if (pid)
        pick_gl_on(pid);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, pos);
    if (col) {
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(4, GL_UNSIGNED_BYTE, 0, col);
    } else {
        glColor4ub(255, 255, 255, 255);
    }
    if (t) {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 0, st);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, t->id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, G.filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, G.filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, G.wrap);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, G.wrap);
    } else if (pid) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, G.white);
    } else {
        glDisable(GL_TEXTURE_2D);
    }
    glDrawArrays(GL_TRIANGLES, 0, nvert);
    if (pid)
        pick_gl_off();
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    if (dt)
        glEnable(GL_DEPTH_TEST);
    glDepthMask(dm);
}

void gfx_draw_2d(int w, int h, int nvert, const float *pos, const float *st, const uint8_t *col)
{
    rt_prof_begin(RTP_GFX);
    gfx_draw_2d_gl(w, h, nvert, pos, st, col);
    rt_prof_end(RTP_GFX);
}

void gfx_release_clay(gfx_clay *c)
{
    if (!c)
        return;
    free(c->pos);
    free(c->st);
    free(c->col);
    free(c->drawcol);
    free(c->index);
    free(c->batch);
    free(c);
}

/* no GPU skinning here: fl_model skins on the CPU (gfx.h) */
int gfx_skin_capable(void) { return 0; }
int gfx_clay_set_skin(gfx_clay *c, const gfx_skin_desc *s) { (void)c; (void)s; return -1; }
void gfx_clay_pose(gfx_clay *c, const float (*skin)[16], const gfx_light *L) { (void)c; (void)skin; (void)L; }

/* no GPU YUV here: the movie converts to RGBA itself (gfx.h) */
int gfx_yuv_capable(void) { return 0; }
gfx_texture *gfx_create_texture_yuv(int w, int h) { (void)w; (void)h; return NULL; }
void gfx_update_texture_yuv(gfx_texture *t, const uint8_t *y, const uint8_t *u, const uint8_t *v)
{
    (void)t; (void)y; (void)u; (void)v;
}
