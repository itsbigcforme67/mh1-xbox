/*
 * gfx_opts.c - the settings: defaults, the settings file, the command-line flags, the frame-rate cap. The options
 * themselves live in gfx_opt / pc_opt (gfx_gl.h); the backends read them.
 *
 * PC: the file mh1pc.ini sits next to the save folder (rt_mc_root()'s parent: ~/.local/share/mh1pc/mh1pc.ini, Windows
 * %APPDATA%\mh1pc\mh1pc.ini; G: mhgpc.ini in ~/.local/share/mhgpc or %APPDATA%\mhgpc): "key = value" lines, '#' or ';' comments. Order of precedence: built-in defaults, then
 * the file, then the command-line flags. A normal exit writes the file back (not --shot runs), so the window size and
 * position, and a fullscreen toggled with Alt+Enter, are remembered.
 * Xbox (docs/xbox.md "Settings menu on the Xbox"): settings.ini in the game folder (E:\Games\MH1, E:\Games\MHG), the
 * same format with the keys that mean something on the console; written when the settings menu closes. Without a
 * widescreen line the dashboard's 16:9 setting decides.
 */
#include "gfx_gl.h"
#include "../rt/rt_log.h"
#include "../pad/pad.h"
#include "../dbgmenu.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef XBOX
#include <hal/video.h>
#include "../xbox/xbox_title.h"
void xbox_env_set(const char *name, const char *value);
#else
#include <SDL.h>
const char *rt_mc_root(void);
#endif

pc_options pc_opt;
#ifdef XBOX
gfx_options gfx_opt = { 0, 1 };     /* the PC defines it in gfx_gl.c; vsync is always on here */
#endif
/* the settings file next to the save folder: MH1 mh1pc.ini, G mhgpc.ini (each game in its own folder) */
#ifdef VERSION_MHG
#define INI_NAME "mhgpc.ini"
#else
#define INI_NAME "mh1pc.ini"
#endif
static char ini_path[1024];
static int ini_off;

static int ieq(const char *a, const char *b)       /* strcasecmp == 0 (pdclib has no strings.h) */
{
    while (*a && tolower((unsigned char)*a) == tolower((unsigned char)*b))
        a++, b++;
    return tolower((unsigned char)*a) == tolower((unsigned char)*b);
}

static void default_path(void)
{
#ifdef XBOX
    if (!ini_path[0])
        snprintf(ini_path, sizeof ini_path, "%s", XBOX_GAME_DIR "\\settings.ini");
#else
    char *s;
    if (ini_path[0])
        return;
    snprintf(ini_path, sizeof ini_path, "%s", rt_mc_root());
    s = strrchr(ini_path, '/');
    if (!s)
        s = strrchr(ini_path, '\\');
    if (s && s[1])
        snprintf(s, sizeof ini_path - (size_t)(s - ini_path), "/" INI_NAME);
    else
        snprintf(ini_path, sizeof ini_path, INI_NAME);
#ifdef VERSION_MHG
    {   /* G's settings were called mh1pc.ini (in G's own folder) before 20 Oct 2026: take that file over once */
        char old[1024];
        FILE *f = fopen(ini_path, "r");
        if (f)
            fclose(f);
        else if (s && s[1] && (size_t)(s - ini_path) < sizeof old - 16) {
            snprintf(old, sizeof old, "%.*s/mh1pc.ini", (int)(s - ini_path), ini_path);
            rename(old, ini_path);
        }
    }
#endif
#endif
}

static int truth(const char *v)
{
    return !(!strcmp(v, "0") || ieq(v, "off") || ieq(v, "no") || ieq(v, "false"));
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
        gfx_opt.filter2d = ieq(v, "nearest") ? F2D_NEAREST : ieq(v, "sharp") ? F2D_SHARP : F2D_LINEAR;
    else if (!strcmp(k, "language")) pc_opt.lang_en = ieq(v, "en") || ieq(v, "english");
    else if (!strcmp(k, "confirm")) pc_opt.western_pad = ieq(v, "cross");
    else if (!strcmp(k, "hud")) rt_hud_style = strstr(v, "proto") || strstr(v, "gunner");
    else if (!strcmp(k, "text_table")) snprintf(pc_opt.text_table, sizeof pc_opt.text_table, "%s", v);
    else if (!strcmp(k, "online_server")) snprintf(pc_opt.online_server, sizeof pc_opt.online_server, "%s", v);
    else if (!strcmp(k, "online_login")) snprintf(pc_opt.online_login, sizeof pc_opt.online_login, "%s", v);
    else if (!strcmp(k, "online_password")) snprintf(pc_opt.online_password, sizeof pc_opt.online_password, "%s", v);
    else if (!strcmp(k, "debug_menu")) pc_opt.debug_menu = truth(v) ? 1 : 2;
    else if (!strcmp(k, "rumble")) pad_rumble_off = !truth(v);
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
#ifdef XBOX
    gfx_opt.widescreen = (XVideoGetEncoderSettings() & VIDEO_WIDESCREEN) != 0;     /* the dashboard's 16:9; the file decides over it */
    rt_log("settings: dashboard widescreen %s", gfx_opt.widescreen ? "on" : "off");
#endif
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
    else if (!strcmp(a, "--hud") && *i + 1 < argc) set_key("hud", argv[++*i]);
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

static void note(FILE *f, const char *text)     /* a comment line in the file */
{
    fprintf(f, "# %s\n", text);
}

void gfx_opts_save(void)
{
    FILE *f;
    if (ini_off)
        return;
    default_path();
#ifdef XBOX
    if (!(f = fopen(ini_path, "wb"))) {     /* pdclib: "w+" never opens (docs/xbox.md); plain "wb" does */
        rt_log("settings: cannot write %s", ini_path);
        return;
    }
    note(f, "Monster Hunter settings (Xbox). The settings menu (Back + L3) writes this file.");
    note(f, "widescreen: 0 = the original 4:3, 1 = 16:9 anamorphic (set the TV to 16:9). Without this line the dashboard decides.");
    fprintf(f, "widescreen = %d\n", gfx_opt.widescreen);
#else
    gfx_window_geometry();
    if (!(f = fopen(ini_path, "w")))
        return;
    note(f, "MH1 PC display settings (docs/pc.md, \"Display options\"). Command-line flags override these.");
    fprintf(f, "fullscreen = %d\n", gfx_opt.fullscreen);
    fprintf(f, "vsync = %d\n", gfx_opt.vsync);
    note(f, "fps_cap: most drawn frames per second, 0 = no cap. The game logic always runs at 30 per second.");
    fprintf(f, "fps_cap = %d\n", gfx_opt.fps_cap);
    note(f, "msaa: 0, 2 or 4. aniso: 1 (off) to 16.");
    fprintf(f, "msaa = %d\n", gfx_opt.msaa);
    fprintf(f, "aniso = %d\n", gfx_opt.aniso);
    note(f, "widescreen: 0 = the original 4:3, 1 = 16:9 and wider (wider field of view, HUD at the screen edges).");
    fprintf(f, "widescreen = %d\n", gfx_opt.widescreen);
    note(f, "filter2d: linear, nearest, or sharp (nearest at an integer scale, bilinear for the rest) for the HUD and menu art.");
    fprintf(f, "filter2d = %s\n", gfx_opt.filter2d == F2D_NEAREST ? "nearest" : gfx_opt.filter2d == F2D_SHARP ? "sharp" : "linear");
#endif
    note(f, "language: ja, or en when a translation table is found (text_table, else text/en.txt; docs/english.md). Read at start-up.");
    fprintf(f, "language = %s\n", pc_opt.lang_en ? "en" : "ja");
    if (pc_opt.text_table[0])
        fprintf(f, "text_table = %s\n", pc_opt.text_table);
    note(f, "confirm: circle (Japanese layout) or cross (Western: circle and cross swapped, menus and play alike)");
    fprintf(f, "confirm = %s\n", pc_opt.western_pad ? "cross" : "circle");
    note(f, "rumble: 1 = the controller vibrates when the game asks, 0 = never");
    fprintf(f, "rumble = %d\n", !pad_rumble_off);
    if (pc_opt.debug_menu)
        fprintf(f, "# debug_menu: 1 = the testers' debug menu (F9, Back + R3 on a controller; docs/PLAYING.md), 0 = off\ndebug_menu = %d\n",
                pc_opt.debug_menu == 1);
    note(f, "hud: original, or prototype gunner (the unused early HUD mock-up cockpit2, for bowgun users; docs/pc.md)");
    fprintf(f, "hud = %s\n", rt_hud_style ? "prototype gunner" : "original");
    /* (the Xbox too: the settings menu rewrites settings.ini, and the player's online lines must survive that) */
    if (pc_opt.online_server[0] || pc_opt.online_login[0] || pc_opt.online_password[0]) {
        fprintf(f, "# online (the ONLINE=1 build): lobby server host:port (a public address set here is allowed; MH Oldschool\n"
                   "# is always refused), the 8-digit login and its password\n");
        fprintf(f, "online_server = %s\nonline_login = %s\nonline_password = %s\n", pc_opt.online_server, pc_opt.online_login,
                pc_opt.online_password);
    }
#ifndef XBOX
    note(f, "the windowed position and size, kept on exit");
    if (gfx_opt.win_w > 0) {
        fprintf(f, "window_x = %d\nwindow_y = %d\nwindow_w = %d\nwindow_h = %d\n", gfx_opt.win_x, gfx_opt.win_y, gfx_opt.win_w,
                gfx_opt.win_h);
    }
#endif
    fclose(f);
#ifdef XBOX
    rt_log("settings: %s written", ini_path);
#endif
}

#ifndef XBOX
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

#endif

/* the translation table to use: the settings file's text_table, RT_TEXT_TABLE, text/en.txt next to the settings file, in the
 * working folder or next to the program (Xbox: E:\Games\MH1\text\en.txt, else in the data folders). NULL when there is none
 * (the English option is then not offered). */
const char *gfx_text_table_find(void)
{
    static char found[1100];
    char cand[5][1100];
    int n = 0, k;
    const char *e = getenv("RT_TEXT_TABLE");
#ifndef XBOX
    char *base;
#endif
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
#ifdef XBOX
    snprintf(cand[n++], sizeof cand[0], "%s", XBOX_GAME_DIR "\\data\\text\\en.txt");
    snprintf(cand[n++], sizeof cand[0], "%s", "D:\\data\\text\\en.txt");
#else
    snprintf(cand[n++], sizeof cand[0], "text/en.txt");
    base = SDL_GetBasePath();
    if (base) {
        snprintf(cand[n++], sizeof cand[0], "%stext/en.txt", base);
        SDL_free(base);
    }
#endif
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
#ifdef XBOX
        xbox_env_set("RT_TEXT_TABLE", t);
#else
        SDL_setenv("RT_TEXT_TABLE", t, 0);
#endif
        rt_log("settings: English text from %s", t);
    } else {
        rt_log("settings: language = en, but no translation table was found (text/en.txt): Japanese");
    }
}
