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

pc_options pc_opt;
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
    else if (!strcmp(k, "filter2d"))
        gfx_opt.filter2d = !strcasecmp(v, "nearest") ? F2D_NEAREST : !strcasecmp(v, "sharp") ? F2D_SHARP : F2D_LINEAR;
    else if (!strcmp(k, "language")) pc_opt.lang_en = !strcasecmp(v, "en") || !strcasecmp(v, "english");
    else if (!strcmp(k, "confirm")) pc_opt.western_pad = !strcasecmp(v, "cross");
    else if (!strcmp(k, "text_table")) snprintf(pc_opt.text_table, sizeof pc_opt.text_table, "%s", v);
    else if (!strcmp(k, "online_server")) snprintf(pc_opt.online_server, sizeof pc_opt.online_server, "%s", v);
    else if (!strcmp(k, "online_login")) snprintf(pc_opt.online_login, sizeof pc_opt.online_login, "%s", v);
    else if (!strcmp(k, "online_password")) snprintf(pc_opt.online_password, sizeof pc_opt.online_password, "%s", v);
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
    else if (!strcmp(a, "--english")) pc_opt.lang_en = 1;
    else if (!strcmp(a, "--japanese")) pc_opt.lang_en = 0;
    else if (!strcmp(a, "--confirm") && *i + 1 < argc) set_key("confirm", argv[++*i]);
    else if (!strcmp(a, "--text-table") && *i + 1 < argc) set_key("text_table", argv[++*i]);
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
    fprintf(f, "# filter2d: linear, nearest, or sharp (nearest at an integer scale, bilinear for the rest) for the HUD and menu art.\n");
    fprintf(f, "filter2d = %s\n", gfx_opt.filter2d == F2D_NEAREST ? "nearest" : gfx_opt.filter2d == F2D_SHARP ? "sharp" : "linear");
    fprintf(f, "# language: ja, or en when a translation table is found (text_table, else text/en.txt; docs/english.md). Read at start-up.\n");
    fprintf(f, "language = %s\n", pc_opt.lang_en ? "en" : "ja");
    if (pc_opt.text_table[0])
        fprintf(f, "text_table = %s\n", pc_opt.text_table);
    fprintf(f, "# confirm: circle (Japanese layout) or cross (Western: circle and cross swapped, menus and play alike)\n");
    fprintf(f, "confirm = %s\n", pc_opt.western_pad ? "cross" : "circle");
    if (pc_opt.online_server[0] || pc_opt.online_login[0] || pc_opt.online_password[0]) {
        fprintf(f, "# online (the ONLINE=1 build): lobby server host:port (a public address set here is allowed; MH Oldschool\n"
                   "# is always refused), the 8-digit login and its password\n");
        fprintf(f, "online_server = %s\nonline_login = %s\nonline_password = %s\n", pc_opt.online_server, pc_opt.online_login,
                pc_opt.online_password);
    }
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

/* the translation table to use: the settings file's text_table, RT_TEXT_TABLE, text/en.txt next to the settings file, in the
 * working folder or next to the program. NULL when there is none (the English option is then not offered). */
const char *gfx_text_table_find(void)
{
    static char found[1100];
    char cand[5][1100];
    int n = 0, k;
    const char *e = getenv("RT_TEXT_TABLE");
    char *base;
    if (pc_opt.text_table[0])
        snprintf(cand[n++], sizeof cand[0], "%s", pc_opt.text_table);
    if (e && *e)
        snprintf(cand[n++], sizeof cand[0], "%s", e);
    default_path();
    snprintf(cand[n], sizeof cand[0], "%s", ini_path);
    {
        char *sl = strrchr(cand[n], '/');
        if (!sl)
            sl = strrchr(cand[n], '\\');
        if (sl)
            snprintf(sl, sizeof cand[0] - (size_t)(sl - cand[n]), "/text/en.txt");
        else
            snprintf(cand[n], sizeof cand[0], "text/en.txt");
        n++;
    }
    snprintf(cand[n++], sizeof cand[0], "text/en.txt");
    base = SDL_GetBasePath();
    if (base) {
        snprintf(cand[n++], sizeof cand[0], "%stext/en.txt", base);
        SDL_free(base);
    }
    for (k = 0; k < n; k++) {
        FILE *f = fopen(cand[k], "rb");
        if (f) {
            fclose(f);
            snprintf(found, sizeof found, "%s", cand[k]);
            return found;
        }
    }
    return NULL;
}

/* start-up: English text asked for and a table found: hand it to the text layer (rt_text.c reads RT_TEXT_TABLE) */
void gfx_opts_apply_language(void)
{
    const char *t;
    if (!pc_opt.lang_en || getenv("RT_TEXT_TABLE"))
        return;
    if ((t = gfx_text_table_find()) != NULL) {
        SDL_setenv("RT_TEXT_TABLE", t, 0);
        rt_log("settings: English text from %s", t);
    } else {
        rt_log("settings: language = en, but no translation table was found (text/en.txt): Japanese");
    }
}
