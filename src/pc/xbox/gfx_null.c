/*
 * gfx_null.c - a gfx.h backend that draws nothing (first Xbox link, and
 * headless tests). The real Xbox backend will be on pbkit (docs/xbox.md).
 */
#include "../gfx/gfx.h"
#include <stdlib.h>
#include <string.h>

struct gfx_texture { int w, h; };
struct gfx_clay { int nvert; };
int gfx_tex_src_hint;
static int W = 640, H = 480;

int gfx_init(int width, int height, const char *title, int hidden)
{
    (void)title; (void)hidden;
    W = width;
    H = height;
    return 0;
}
void gfx_shutdown(void) {}
void gfx_size(int *w, int *h) { *w = W; *h = H; }
void gfx_begin_frame(uint32_t clear_rgb) { (void)clear_rgb; }
void gfx_end_frame(void) {}
int gfx_read_pixels(uint8_t *rgb) { memset(rgb, 0, (size_t)W * H * 3); return 0; }
gfx_texture *gfx_create_texture(int w, int h, const uint8_t *rgba)
{
    gfx_texture *t = calloc(1, sizeof *t);
    (void)rgba;
    gfx_tex_src_hint = 0;
    if (t) {
        t->w = w;
        t->h = h;
    }
    return t;
}
void gfx_release_texture(gfx_texture *t) { free(t); }
void gfx_set_render_state(int state, uintptr_t value) { (void)state; (void)value; }
void gfx_set_render_state_f(int state, float value) { (void)state; (void)value; }
void gfx_draw_2d(int w, int h, int nvert, const float *pos, const float *st, const uint8_t *col)
{
    (void)w; (void)h; (void)nvert; (void)pos; (void)st; (void)col;
}
gfx_clay *gfx_create_clay(const gfx_clay_desc *d)
{
    gfx_clay *c = calloc(1, sizeof *c);
    if (c)
        c->nvert = d->nvert;
    return c;
}
void gfx_update_clay(gfx_clay *c, const float *pos, const uint8_t *col) { (void)c; (void)pos; (void)col; }
void gfx_execute_clay(gfx_clay *c) { (void)c; }
void gfx_release_clay(gfx_clay *c) { free(c); }

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
