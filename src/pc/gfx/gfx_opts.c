/*
 * gfx_opts.c - the PC display options: defaults, the settings file mh1pc.ini, the command-line flags, the
 * frame-rate cap. The options themselves live in gfx_opt (gfx_gl.h); the GL backend reads them at start-up.
 *
 * The file sits next to the save folder (rt_mc_root()'s parent: ~/.local/share/mh1pc/mh1pc.ini, Windows
 * %APPDATA%\mh1pc\mh1pc.ini): "key = value" lines, '#' or ';' comments. Order of precedence: built-in
 * defaults, then the file, then the command-line flags. A normal exit writes the file back (not --shot runs),
 * so the window size and position, and a fullscreen toggled with Alt+Enter, are remembered.
 */
#include "gfx_gl.h"
#include "../rt/rt_log.h"

#include <SDL.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

const char *rt_mc_root(void);

static char ini_path[1024];
static int ini_off;

static void default_path(void)
{
    char *s;
    if (ini_path[0])
        return;
    snprintf(ini_path, sizeof ini_path, "%s", rt_mc_root());
    s = strrchr(ini_path, '/');
    if (!s)
        s = strrchr(ini_path, '\\');
    if (s && s[1])
        snprintf(s, sizeof ini_path - (size_t)(s - ini_path), "/mh1pc.ini");
    else
        snprintf(ini_path, sizeof ini_path, "mh1pc.ini");
}

static int truth(const char *v)
{
    return !(!strcmp(v, "0") || !strcasecmp(v, "off") || !strcasecmp(v, "no") || !strcasecmp(v, "false"));
}

static int clampi(int v, int lo, int hi) { return v < lo ? lo : v > hi ? hi : v; }

static int snap_msaa(int v) { return v >= 4 ? 4 : v >= 2 ? 2 : 0; }

static void set_key(const char *k, const char *v)
{
    if (!strcmp(k, "fullscreen")) gfx_opt.fullscreen = truth(v);
    else if (!strcmp(k, "vsync")) gfx_opt.vsync = truth(v);
    else if (!strcmp(k, "fps_cap")) gfx_opt.fps_cap = clampi(atoi(v), 0, 1000);
    else if (!strcmp(k, "msaa")) gfx_opt.msaa = snap_msaa(atoi(v));
    else if (!strcmp(k, "aniso")) gfx_opt.aniso = clampi(atoi(v), 1, 16);
    else if (!strcmp(k, "widescreen")) gfx_opt.widescreen = truth(v);
    else if (!strcmp(k, "filter2d")) gfx_opt.nearest2d = !strcasecmp(v, "nearest");
    else if (!strcmp(k, "window_x")) gfx_opt.win_x = atoi(v);
    else if (!strcmp(k, "window_y")) gfx_opt.win_y = atoi(v);
    else if (!strcmp(k, "window_w")) gfx_opt.win_w = clampi(atoi(v), 0, 16384);
    else if (!strcmp(k, "window_h")) gfx_opt.win_h = clampi(atoi(v), 0, 16384);
}

void gfx_opts_load(int argc, char **argv)
{
    FILE *f;
    char line[256];
    int i;
    for (i = 1; i < argc; i++) {                /* --ini FILE / --no-ini decide which file, before the flags run */
        if (!strcmp(argv[i], "--ini") && i + 1 < argc)
            snprintf(ini_path, sizeof ini_path, "%s", argv[i + 1]);
        else if (!strcmp(argv[i], "--no-ini"))
            ini_off = 1;
    }
    if (ini_off)
        return;
    default_path();
    if (!(f = fopen(ini_path, "r")))
        return;
    while (fgets(line, sizeof line, f)) {
        char *k = line, *v, *e;
        while (isspace((unsigned char)*k))
            k++;
        if (!*k || *k == '#' || *k == ';' || *k == '[')
            continue;
        if (!(v = strchr(k, '=')))
            continue;
        *v++ = 0;
        while (isspace((unsigned char)*v))
            v++;
        for (e = k + strlen(k); e > k && isspace((unsigned char)e[-1]); )
            *--e = 0;
        for (e = v + strlen(v); e > v && isspace((unsigned char)e[-1]); )
            *--e = 0;
        set_key(k, v);
    }
    fclose(f);
    rt_log("settings: %s read", ini_path);
}

int gfx_opts_arg(int argc, char **argv, int *i)
{
    const char *a = argv[*i];
    if (!strcmp(a, "--fullscreen")) gfx_opt.fullscreen = 1;
    else if (!strcmp(a, "--windowed")) gfx_opt.fullscreen = 0;
    else if (!strcmp(a, "--vsync")) gfx_opt.vsync = 1;
    else if (!strcmp(a, "--no-vsync")) gfx_opt.vsync = 0;
    else if (!strcmp(a, "--fps-cap") && *i + 1 < argc) set_key("fps_cap", argv[++*i]);
    else if (!strcmp(a, "--msaa") && *i + 1 < argc) set_key("msaa", argv[++*i]);
    else if (!strcmp(a, "--aniso") && *i + 1 < argc) set_key("aniso", argv[++*i]);
    else if (!strcmp(a, "--widescreen")) gfx_opt.widescreen = 1;
    else if (!strcmp(a, "--aspect") && *i + 1 < argc) {
        const char *v = argv[++*i];
        gfx_opt.widescreen = strcmp(v, "4:3") != 0;     /* 16:9 (or any other value): widescreen */
    }
    else if (!strcmp(a, "--filter2d") && *i + 1 < argc) set_key("filter2d", argv[++*i]);
    else if (!strcmp(a, "--ini") && *i + 1 < argc) { snprintf(ini_path, sizeof ini_path, "%s", argv[++*i]); }
    else if (!strcmp(a, "--no-ini")) ini_off = 1;
    else return 0;
    return 1;
}

/* the window size nobody asked for: the remembered one, else 960x720 (4:3) / 1280x720 (widescreen) */
void gfx_opts_finish(int size_given, int *w, int *h)
{
    if (size_given)
        return;
    if (gfx_opt.win_w >= 320 && gfx_opt.win_h >= 240) {
        *w = gfx_opt.win_w;
        *h = gfx_opt.win_h;
    } else {
        *w = gfx_opt.widescreen ? 1280 : 960;
        *h = 720;
    }
}

void gfx_opts_save(void)
{
    FILE *f;
    if (ini_off)
        return;
    default_path();
    gfx_window_geometry();
    if (!(f = fopen(ini_path, "w")))
        return;
    fprintf(f, "# MH1 PC display settings (docs/pc.md, \"Display options\"). Command-line flags override these.\n");
    fprintf(f, "fullscreen = %d\n", gfx_opt.fullscreen);
    fprintf(f, "vsync = %d\n", gfx_opt.vsync);
    fprintf(f, "# fps_cap: most drawn frames per second, 0 = no cap. The game logic always runs at 30 per second.\n");
    fprintf(f, "fps_cap = %d\n", gfx_opt.fps_cap);
    fprintf(f, "# msaa: 0, 2 or 4. aniso: 1 (off) to 16.\n");
    fprintf(f, "msaa = %d\n", gfx_opt.msaa);
    fprintf(f, "aniso = %d\n", gfx_opt.aniso);
    fprintf(f, "# widescreen: 0 = the original 4:3, 1 = 16:9 and wider (wider field of view, HUD at the screen edges).\n");
    fprintf(f, "widescreen = %d\n", gfx_opt.widescreen);
    fprintf(f, "# filter2d: linear or nearest (HUD and menu art).\n");
    fprintf(f, "filter2d = %s\n", gfx_opt.nearest2d ? "nearest" : "linear");
    fprintf(f, "# the windowed position and size, kept on exit\n");
    if (gfx_opt.win_w > 0) {
        fprintf(f, "window_x = %d\nwindow_y = %d\nwindow_w = %d\nwindow_h = %d\n", gfx_opt.win_x, gfx_opt.win_y, gfx_opt.win_w,
                gfx_opt.win_h);
    }
    fclose(f);
}

/* the frame-rate cap: sleep (then spin for the last 1.5 ms) to the next frame slot */
void gfx_frame_pace(void)
{
    static Uint64 next;
    Uint64 now = SDL_GetPerformanceCounter(), f = SDL_GetPerformanceFrequency();
    Uint64 step;
    if (gfx_opt.fps_cap <= 0) {
        next = 0;
        return;
    }
    step = f / (Uint64)gfx_opt.fps_cap;
    if (!next || now > next + step * 4)
        next = now;                         /* first frame, or far behind: do not try to catch up */
    next += step;
    if (now < next) {
        Uint64 ms = (next - now) * 1000 / f;
        if (ms > 2)
            SDL_Delay((Uint32)(ms - 2));
        while (SDL_GetPerformanceCounter() < next)
            ;
    }
}
