/*
 * gfx_gl.h - the PC display options (GL backend + settings file, gfx_opts.c). PC only: the Xbox build does not
 * include this. See docs/pc.md, "Display options".
 */
#ifndef MH_GFX_GL_H
#define MH_GFX_GL_H

typedef struct {
    int fullscreen;      /* desktop fullscreen */
    int vsync;           /* swap interval 1 (default) or 0 */
    int fps_cap;         /* drawn frames per second at most, 0 = no cap (the game logic always ticks at 30 Hz) */
    int msaa;            /* 0, 2 or 4 samples */
    int aniso;           /* anisotropic filtering level for the 3D textures, 1 = off */
    int widescreen;      /* 1: the 3D fills a wide window (Hor+), HUD at the edges, menus 4:3 centred; 0: 4:3 */
    int nearest2d;       /* 1: nearest filter for the 2D art (HUD, menus), 0 bilinear */
    int win_x, win_y;    /* the remembered windowed position (-1: centred) and size (0: default) */
    int win_w, win_h;
} gfx_options;
extern gfx_options gfx_opt;

float gfx_scene_aspect(void);         /* width / height of the 3D scene rectangle (4/3 unless widescreen) */
int  gfx_toggle_fullscreen(void);     /* Alt+Enter; returns the new state */
void gfx_window_geometry(void);       /* note the windowed position / size into gfx_opt */
void gfx_pick_viewport(int r[4]);     /* the scene rectangle in window pixels, top-down: x y w h */

/* gfx_opts.c */
void gfx_opts_load(int argc, char **argv);           /* defaults < settings file (--ini FILE, --no-ini); the flags come after */
int  gfx_opts_arg(int argc, char **argv, int *i);    /* a display flag at argv[*i]: consumed, 1 */
void gfx_opts_finish(int size_given, int *w, int *h); /* after the flags: default / remembered window size */
void gfx_opts_save(void);
void gfx_frame_pace(void);                           /* after the swap: the frame-rate cap */

#endif
