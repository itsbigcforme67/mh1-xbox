/*
 * gfx_gl.h - the PC display options (GL backend + settings file, gfx_opts.c). PC only: the Xbox build does not
 * include this. See docs/pc.md, "Display options".
 */
#ifndef MH_GFX_GL_H
#define MH_GFX_GL_H

enum { F2D_LINEAR, F2D_NEAREST, F2D_SHARP };
typedef struct {
    int fullscreen;      /* desktop fullscreen */
    int vsync;           /* swap interval 1 (default) or 0 */
    int fps_cap;         /* drawn frames per second at most, 0 = no cap (the game logic always ticks at 30 Hz) */
    int msaa;            /* 0, 2 or 4 samples */
    int aniso;           /* anisotropic filtering level for the 3D textures, 1 = off */
    int widescreen;      /* 1: the 3D fills a wide window (Hor+), HUD at the edges, menus 4:3 centred; 0: 4:3 */
    int filter2d;        /* the 2D art (HUD, menus, text): F2D_LINEAR bilinear, F2D_NEAREST, F2D_SHARP nearest at an integer scale + bilinear for the rest */
    int win_x, win_y;    /* the remembered windowed position (-1: centred) and size (0: default) */
    int win_w, win_h;
} gfx_options;
extern gfx_options gfx_opt;

/* the PC settings that are not about the picture (gfx_opts.c; the in-game settings menu, src/pc/menu.c, edits them) */
typedef struct {
    int lang_en;         /* 1: English text from the translation table (text/en.txt, docs/english.md); read at start-up */
    int western_pad;     /* 1: cross confirms and circle cancels (swapped), 0: the Japanese circle-confirm layout */
    char text_table[512];/* path of the translation table from the settings file ("" = look in the default places) */
    char online_server[128];  /* the online town (ONLINE=1, docs/network.md 3.5): lobby server "host:port" ("" = 127.0.0.1:10200) */
    char online_login[16];    /* the 8-digit login (the MMBB id the KDDI portal gave the PS2) */
    char online_password[24]; /* its password (16 characters) */
} pc_options;
extern pc_options pc_opt;
const char *gfx_text_table_find(void);                /* the translation table file that exists, or NULL */

float gfx_scene_aspect(void);         /* width / height of the 3D scene rectangle (4/3 unless widescreen) */
int  gfx_toggle_fullscreen(void);     /* Alt+Enter; returns the new state */
void gfx_set_fullscreen(int on);      /* the settings menu: switch now (and gfx_opt.fullscreen follows) */
void gfx_set_vsync(int on);           /* swap interval now */
int  gfx_aniso_max(void);             /* the driver's anisotropy limit (0: no support) */
int  gfx_apply_aniso(void);           /* after gfx_opt.aniso changed: mip levels for the textures made before; 0 if the driver cannot */
int  gfx_sharp_ok(void);              /* the "sharp" 2D filter needs shaders (GL 2.0): 1 if they work */
void gfx_window_geometry(void);       /* note the windowed position / size into gfx_opt */
void gfx_pick_viewport(int r[4]);     /* the scene rectangle in window pixels, top-down: x y w h */

/* gfx_opts.c */
void gfx_opts_load(int argc, char **argv);           /* defaults < settings file (--ini FILE, --no-ini); the flags come after */
int  gfx_opts_arg(int argc, char **argv, int *i);    /* a display flag at argv[*i]: consumed, 1 */
void gfx_opts_finish(int size_given, int *w, int *h); /* after the flags: default / remembered window size */
void gfx_opts_save(void);
void gfx_opts_apply_language(void);                  /* before the data is loaded: language = en -> RT_TEXT_TABLE */
void gfx_frame_pace(void);                           /* after the swap: the frame-rate cap */

#endif
