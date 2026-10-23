/*
 * menu.c - the settings menu (docs/pc.md, "Settings menu"; Xbox: docs/xbox.md, "Settings menu on the Xbox"). F10 opens it
 * at any time (also on the title screen, which shows a hint); on a controller Back + L3 does (the Xbox has only that).
 * The Xbox shows the rows that mean something on the console (language, aspect, HUD, button layout, volumes, rumble, debug
 * menu) and draws it in a 320 x 240 frame on the 4:3 picture (2x the 5x7 font on a 640 x 480 screen). While it is open the game does not tick (the viewer's tick loop asks
 * menu_hold_ticks), the music is paused, and the last picture stays drawn underneath so the changes that apply live
 * (aspect ratio, 2D filter, anisotropy, vsync, fullscreen) can be seen at once.
 *
 * It is a small overlay of its own: a 5x7 pixel font (the bug reporter's table, pick.c) drawn from an atlas texture, in
 * window pixels, on the gfx.h 2D path. Keyboard: Up / Down select, Left / Right change, Enter or Space next value, Esc or F10
 * close. Controller: D-pad or left stick, cross / circle change, start / back / select (on release) close. Mouse: click a
 * row (left: next, right: previous), wheel selects.
 *
 * The settings are the ones in gfx_opt / pc_opt (gfx_gl.h) and are written to mh1pc.ini when the menu closes. The two
 * volumes are the game's own options (option_w[1] music, option_w[2] effects, which the game's OPTION screen edits and the
 * memory card keeps), changed the way that screen does it: not a second copy.
 *
 * Test aids: RT_MENU_AT=tick opens it at that game tick; RT_MENU_KEYS="down,down,right,enter,esc" feeds one key per two
 * frames once it is open (up down left right enter esc); RT_MENU_SAVE=1 lets a --shot run write mh1pc.ini.
 */
#include "menu.h"
#include "font5x7.h"
#include "dbgmenu.h"
#include "gfx/gfx.h"
#include "gfx/gfx_gl.h"
#include "pad/pad.h"
#include "rt/rt_log.h"

#ifndef XBOX
#include <SDL.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int rt_pad_swap_confirm;
extern int rt_boot_title(void);
extern signed char option_w[];
extern unsigned char system_w[];
void str_master_vol(int);
void rt_2d_restore_texture(void);
void audio_pause(int on);
int flPADShockSet(int port, int strength, int frames);

enum { R_DISPLAY, R_ASPECT, R_VSYNC, R_FPS, R_MSAA, R_ANISO, R_FILTER, R_LANG, R_PAD, R_HUD, R_MUSIC, R_EFFECTS, R_RUMBLE, R_DEBUG,
       R_SKIP, R_DEFAULTS, R_CLOSE, R_N };
#ifdef XBOX     /* the rows shown, in order */
static const int rows[] = { R_LANG, R_ASPECT, R_HUD, R_PAD, R_MUSIC, R_EFFECTS, R_RUMBLE, R_DEBUG, R_SKIP, R_CLOSE };
#define XB(xbox, pc) (xbox)
#else
static const int rows[] = { R_DISPLAY, R_ASPECT, R_VSYNC, R_FPS, R_MSAA, R_ANISO, R_FILTER, R_LANG, R_PAD, R_HUD, R_MUSIC, R_EFFECTS,
                            R_RUMBLE, R_DEBUG, R_SKIP, R_DEFAULTS, R_CLOSE };
#define XB(xbox, pc) (pc)
#endif
#define NROWS ((int)(sizeof rows / sizeof rows[0]))

static struct {
    int inited, open, sel, shot;
    int msaa0, lang0;               /* what this run started with: changes that need a restart say so */
    int have_table;                 /* a translation table exists (English offered) */
    int rel_mouse;
    int pad_prev, pad_hold, pad_rep, pad_close;
    int pad_now, tick_prev, mask;            /* the game's pad: the open combo's edge; buttons held when the menu closed (kept from the game until released) */
    /* scripted test */
    int at, done;
    const char *keys;
    int key_wait;
    /* drawn layout (window pixels), for the mouse */
    int row_y[NROWS], row_h, px0, px1, ly0;
    int hover;
} M;

static const char *label[R_N] = {
    "Display", "Aspect ratio", "VSync", "Frame rate cap", "Anti-aliasing (MSAA)", "Anisotropic filter", "2D art filter",
    "Text language", "Button layout", "HUD", "Music volume", "Effects volume", "Rumble", "Debug menu (testers)",
    "Skip intros", "Restore display defaults", "Close"
};
static const int fps_list[] = { 0, 30, 60, 72, 90, 120, 144, 165, 240 };
#define NFPS ((int)(sizeof fps_list / sizeof fps_list[0]))

static void init(void)
{
    const char *e;
    if (M.inited)
        return;
    M.inited = 1;
    M.msaa0 = gfx_opt.msaa;
    M.lang0 = pc_opt.lang_en;
    M.have_table = gfx_text_table_find() != NULL;
    M.at = -1;
    if ((e = getenv("RT_MENU_AT")) != NULL)
        M.at = atoi(e);
    M.keys = getenv("RT_MENU_KEYS");
    M.hover = -1;
}

/* ------------------------------------------------------------ the settings */
static int clampi(int v, int lo, int hi) { return v < lo ? lo : v > hi ? hi : v; }

#ifndef XBOX
static int fps_index(void)
{
    int i, best = 0;
    for (i = 0; i < NFPS; i++)
        if (fps_list[i] == gfx_opt.fps_cap)
            return i;
        else if (gfx_opt.fps_cap > fps_list[i])
            best = i;
    return best;
}

static int aniso_steps(void)       /* 1, 2, 4, 8, 16 up to the driver's limit: how many */
{
    int n = 1, v = 2, mx = gfx_aniso_max();
    while (v <= mx && v <= 16) {
        n++;
        v *= 2;
    }
    return n;
}

static int aniso_index(void)
{
    int i = 0, v = 1;
    while (v < gfx_opt.aniso && i < aniso_steps() - 1) {
        v *= 2;
        i++;
    }
    return i;
}

#endif

static void vol_change(int idx, int d)       /* the game's option screen: the value, system_w, the stream master volume */
{
    int v = clampi(option_w[idx] + d, 0, 7);
    option_w[idx] = (signed char)v;
    system_w[0x36] = (unsigned char)option_w[1];
    system_w[0x37] = (unsigned char)option_w[2];
    str_master_vol(1);
}

#ifndef XBOX
static void defaults(void)
{
    gfx_opt.widescreen = 0;
    gfx_set_vsync(1);
    gfx_opt.fps_cap = 0;
    gfx_opt.msaa = 0;
    gfx_opt.aniso = 1;
    gfx_opt.filter2d = F2D_LINEAR;
    pc_opt.western_pad = 0;
    rt_pad_swap_confirm = 0;
    rt_hud_style = 0;
}
#endif

static void change(int row, int d)
{
    switch (row) {
#ifndef XBOX
    case R_DISPLAY:
        gfx_set_fullscreen(!gfx_opt.fullscreen);
        break;
    case R_VSYNC:
        gfx_set_vsync(!gfx_opt.vsync);
        break;
    case R_FPS:
        gfx_opt.fps_cap = fps_list[(fps_index() + d + NFPS) % NFPS];
        break;
    case R_MSAA: {
        int i = gfx_opt.msaa == 0 ? 0 : gfx_opt.msaa == 2 ? 1 : 2;
        i = (i + d + 3) % 3;
        gfx_opt.msaa = i == 0 ? 0 : i == 1 ? 2 : 4;
        break;
    }
    case R_ANISO: {
        int n = aniso_steps(), i;
        if (n <= 1)
            break;
        i = (aniso_index() + d + n) % n;
        gfx_opt.aniso = 1 << i;
        gfx_apply_aniso();
        break;
    }
    case R_FILTER: {
        int m = gfx_opt.filter2d, k;
        for (k = 0; k < 3; k++) {          /* linear -> sharp -> nearest (sharp skipped without shaders) */
            static const int order[3] = { F2D_LINEAR, F2D_SHARP, F2D_NEAREST };
            int i = 0;
            while (order[i] != m)
                i++;
            m = order[(i + d + 3) % 3];
            if (m != F2D_SHARP || gfx_sharp_ok())
                break;
        }
        gfx_opt.filter2d = m;
        break;
    }
    case R_DEFAULTS:
        defaults();
        break;
#endif
    case R_ASPECT:
        gfx_opt.widescreen = !gfx_opt.widescreen;
        break;
    case R_LANG:
        if (M.have_table)
            pc_opt.lang_en = !pc_opt.lang_en;
        break;
    case R_PAD:
        pc_opt.western_pad = !pc_opt.western_pad;
        rt_pad_swap_confirm = pc_opt.western_pad;
        break;
    case R_HUD:
        rt_hud_style = !rt_hud_style;
        break;
    case R_MUSIC:
        vol_change(1, d);
        break;
    case R_EFFECTS:
        vol_change(2, d);
        break;
    case R_RUMBLE:
        pad_rumble_off = !pad_rumble_off;
        if (pad_rumble_off)
            flPADShockSet(0, 0, 0);       /* stop one that is running */
        break;
    case R_DEBUG:
        dbgmenu_set_enabled(!dbgmenu_enabled());
        pc_opt.debug_menu = dbgmenu_enabled() ? 1 : 2;
        break;
    case R_SKIP:
        pc_opt.skip_intros = !pc_opt.skip_intros;
        break;
    }
}

static const char *value(int row, char *buf, size_t n, const char **note)
{
    *note = "";
    switch (row) {
#ifndef XBOX
    case R_DISPLAY:
        *note = "Alt+Enter or F11 switches too";
        return gfx_opt.fullscreen ? "Fullscreen" : "Windowed";
    case R_VSYNC:
        return gfx_opt.vsync ? "On" : "Off";
    case R_FPS:
        *note = "drawn frames only; the game logic stays at 30 Hz";
        if (gfx_opt.fps_cap <= 0)
            return "Off";
        snprintf(buf, n, "%d", gfx_opt.fps_cap);
        return buf;
    case R_MSAA:
        if (gfx_opt.msaa != M.msaa0)
            *note = "applies at the next start";
        snprintf(buf, n, gfx_opt.msaa ? "%dx" : "Off", gfx_opt.msaa);
        return buf;
    case R_ANISO:
        if (gfx_aniso_max() <= 1)
            return "Not supported";
        snprintf(buf, n, gfx_opt.aniso > 1 ? "%dx" : "Off", gfx_opt.aniso);
        return buf;
    case R_FILTER:
        *note = gfx_opt.filter2d == F2D_SHARP ? "nearest at whole-number scales, smooth between" : "";
        return gfx_opt.filter2d == F2D_NEAREST ? "Nearest (hard pixels)" : gfx_opt.filter2d == F2D_SHARP ? "Sharp (integer scale)" : "Smooth (bilinear)";
#endif
    case R_ASPECT:
        *note = gfx_opt.widescreen ? XB("set the TV to 16:9; menus stay 4:3", "wider view; menus stay 4:3, the HUD hugs the edges")
                                   : "the original 4:3 picture";
        return gfx_opt.widescreen ? XB("16:9 (anamorphic)", "16:9 (wide)") : "4:3 (original)";
    case R_LANG:
        if (!M.have_table) {
            *note = XB("English needs text\\en.txt in the game folder", "English needs text/en.txt (docs/english.md)");
            return "Japanese";
        }
        if (pc_opt.lang_en != M.lang0)
            *note = "applies at the next start";
        return pc_opt.lang_en ? "English" : "Japanese";
    case R_PAD:
        *note = XB("the game's button icons keep the PS2 shapes", "swaps circle and cross everywhere; icons stay as is");
        return pc_opt.western_pad ? XB("A confirms (West)", "Cross confirms (West)") : XB("B confirms (Japan)", "Circle confirms (Japan)");
    case R_HUD:
        *note = rt_hud_style ? XB("bowgun users: the unused early HUD, rebuilt", "bowgun users: the unused early HUD mock-up, rebuilt") : "";
        return rt_hud_style ? "Prototype gunner" : "Original";
    case R_MUSIC:
    case R_EFFECTS: {
        int v = clampi(option_w[row == R_MUSIC ? 1 : 2], 0, 7), i;
        *note = "the game's own option, kept in the save";
        for (i = 0; i < 7; i++)
            buf[i] = i < v ? '#' : '-';
        snprintf(buf + 7, n - 7, " %d", v);
        return buf;
    }
    case R_RUMBLE:
        return pad_rumble_off ? "Off" : "On";
    case R_DEBUG:
        *note = XB("Back + R3 opens it", "Back + R3 (F9 on the PC) opens it");
        return dbgmenu_enabled() ? "On" : "Off";
    case R_SKIP:
        *note = "power-on goes straight to the title screen (from the next start)";
        return pc_opt.skip_intros ? "On" : "Off";
    }
    return "";
}

/* ------------------------------------------------------------ open / close */
static void do_open(void)
{
    init();
    if (M.open)
        return;
    M.open = 1;
    M.sel = 0;
    M.pad_prev = 0xFFFF;                 /* the buttons held to open it are not presses */
    M.pad_hold = M.pad_rep = M.pad_close = 0;
    M.hover = -1;
    audio_pause(1);
#ifndef XBOX
    M.rel_mouse = SDL_GetRelativeMouseMode();
    SDL_SetRelativeMouseMode(SDL_FALSE);
    SDL_ShowCursor(SDL_ENABLE);
#endif
    rt_log("settings menu: opened");
}

static void do_close(void)
{
    if (!M.open)
        return;
    M.open = 0;
    M.mask = M.pad_now;                   /* buttons still held (cross on Close): the game sees them only after a release */
    audio_pause(0);
#ifndef XBOX
    if (M.rel_mouse)
        SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_ShowCursor(M.rel_mouse ? SDL_DISABLE : SDL_ENABLE);
#endif
    if (!M.shot || getenv("RT_MENU_SAVE"))
        gfx_opts_save();
    rt_log("settings menu: closed (aspect %s, vsync %d, fps cap %d, msaa %d, aniso %d, 2D filter %d, language %s, confirm %s, hud %d, rumble %d, debug menu %d)",
           gfx_opt.widescreen ? "16:9" : "4:3", gfx_opt.vsync, gfx_opt.fps_cap, gfx_opt.msaa, gfx_opt.aniso, gfx_opt.filter2d,
           pc_opt.lang_en ? "en" : "ja", pc_opt.western_pad ? "cross" : "circle", rt_hud_style, !pad_rumble_off, dbgmenu_enabled());
}

/* a key, from the keyboard, the controller or the test script */
enum { K_UP, K_DOWN, K_LEFT, K_RIGHT, K_ENTER, K_ESC };
static void key(int k)
{
    switch (k) {
    case K_UP:
        M.sel = (M.sel + NROWS - 1) % NROWS;
        break;
    case K_DOWN:
        M.sel = (M.sel + 1) % NROWS;
        break;
    case K_LEFT:
        change(rows[M.sel], -1);
        break;
    case K_RIGHT:
        change(rows[M.sel], 1);
        break;
    case K_ENTER:
        if (rows[M.sel] == R_CLOSE)
            do_close();
        else
            change(rows[M.sel], 1);
        break;
    case K_ESC:
        do_close();
        break;
    }
}

#ifdef XBOX
int menu_event(const void *p) { (void)p; return 0; }
#else
static int row_at(int x, int y)
{
    int i;
    if (x < M.px0 || x >= M.px1)
        return -1;
    for (i = 0; i < NROWS; i++)
        if (y >= M.row_y[i] && y < M.row_y[i] + M.row_h)
            return i;
    return -1;
}

int menu_event(const void *p)
{
    const SDL_Event *e = (const SDL_Event *)p;
    init();
    if (!M.open) {
        if (e->type == SDL_KEYDOWN && !e->key.repeat && e->key.keysym.sym == SDLK_F10) {
            do_open();
            return 1;
        }
        return 0;
    }
    switch (e->type) {
    case SDL_KEYDOWN:
        switch (e->key.keysym.sym) {
        case SDLK_UP: key(K_UP); break;
        case SDLK_DOWN: key(K_DOWN); break;
        case SDLK_LEFT: key(K_LEFT); break;
        case SDLK_RIGHT: key(K_RIGHT); break;
        case SDLK_RETURN: case SDLK_KP_ENTER: case SDLK_SPACE:
            if (!e->key.repeat)
                key(K_ENTER);
            break;
        case SDLK_ESCAPE: case SDLK_F10:
            if (!e->key.repeat)
                key(K_ESC);
            break;
        default: break;
        }
        return 1;
    case SDL_KEYUP: case SDL_TEXTINPUT: case SDL_TEXTEDITING:
        return 1;
    case SDL_MOUSEMOTION:
        M.hover = row_at(e->motion.x, e->motion.y);
        if (M.hover >= 0)
            M.sel = M.hover;
        return 1;
    case SDL_MOUSEBUTTONDOWN: {
        int r = row_at(e->button.x, e->button.y);
        if (r >= 0) {
            M.sel = r;
            if (e->button.button == SDL_BUTTON_RIGHT)
                key(K_LEFT);
            else
                key(K_ENTER);
        }
        return 1;
    }
    case SDL_MOUSEBUTTONUP:
        return 1;
    case SDL_MOUSEWHEEL:
        key(e->wheel.y > 0 ? K_UP : K_DOWN);
        return 1;
    default:
        return 0;
    }
}

#endif

/* the pad while the menu is open (the controller once a drawn frame, or the --input script: viewer.c) */
void menu_pad(const pad_state *p)
{
    int b = p->bits, dir_x = 0, dir_y = 0, now;
    M.pad_now = b;
    if (!M.open) {
        M.pad_prev = b;
        return;
    }
    if ((b & PAD_UP) || p->ly < -70) dir_y = -1;
    else if ((b & PAD_DOWN) || p->ly > 70) dir_y = 1;
    if ((b & PAD_LEFT) || p->lx < -70) dir_x = -1;
    else if ((b & PAD_RIGHT) || p->lx > 70) dir_x = 1;
    now = dir_y * 3 + dir_x;             /* a direction held: first press at once, then repeating */
    if (now == 0) {
        M.pad_hold = M.pad_rep = 0;
    } else if (now != M.pad_hold) {
        M.pad_hold = now;
        M.pad_rep = 18;
        if (dir_y) key(dir_y < 0 ? K_UP : K_DOWN);
        else key(dir_x < 0 ? K_LEFT : K_RIGHT);
    } else if (--M.pad_rep <= 0) {
        M.pad_rep = 5;
        if (dir_y) key(dir_y < 0 ? K_UP : K_DOWN);
        else key(dir_x < 0 ? K_LEFT : K_RIGHT);
    }
    if ((b & PAD_CROSS) && !(M.pad_prev & PAD_CROSS))
        key(K_ENTER);
    if (b & (PAD_CIRCLE | PAD_START | PAD_SELECT)) {
        if (!(M.pad_prev & (PAD_CIRCLE | PAD_START | PAD_SELECT)) || M.pad_close)
            M.pad_close = 1;              /* closes once released: the game must not see the press afterwards */
    } else if (M.pad_close) {
        M.pad_close = 0;
        key(K_ESC);
    }
    M.pad_prev = b;
}

/* the controller, once a drawn frame: Back + L3 opens the menu (also while the game is held, e.g. on a frozen frame), and
 * the navigation while it is open */
void menu_poll_pad(void)
{
    pad_state p;
    int b;
    init();
    pad_read(&p, 0);                     /* the controller alone: the keyboard has its own keys here */
    b = p.bits;
    if (!M.open) {
        if ((b & MENU_COMBO) == MENU_COMBO && (M.pad_prev & MENU_COMBO) != MENU_COMBO)
            do_open();
        M.pad_prev = b;
        return;
    }
    menu_pad(&p);
}

/* each game tick's pad (the script or the controller): Back + L3 opens the menu and the game does not see it; buttons
 * held when the menu closed stay hidden from the game until released */
void menu_tick_pad(pad_state *ps)
{
    int b = ps->bits;
    init();
    ps->bits &= (uint16_t)~M.mask;
    M.mask &= b;
    if ((b & MENU_COMBO) == MENU_COMBO && (M.tick_prev & MENU_COMBO) != MENU_COMBO) {
        ps->bits &= (uint16_t)~MENU_COMBO;
        do_open();
    }
    M.tick_prev = b;
}

static void script_keys(void)          /* RT_MENU_KEYS: one key per two drawn frames while open */
{
    const char *s;
    int n = 0;
    if (!M.open || !M.keys || !*M.keys || M.key_wait-- > 0)
        return;
    s = M.keys;
    while (s[n] && s[n] != ',')
        n++;
    if (n == 2 && !strncmp(s, "up", 2)) key(K_UP);
    else if (n == 4 && !strncmp(s, "down", 4)) key(K_DOWN);
    else if (n == 4 && !strncmp(s, "left", 4)) key(K_LEFT);
    else if (n == 5 && !strncmp(s, "right", 5)) key(K_RIGHT);
    else if (n == 5 && !strncmp(s, "enter", 5)) key(K_ENTER);
    else if (n == 3 && !strncmp(s, "esc", 3)) key(K_ESC);
    M.keys = s[n] ? s + n + 1 : s + n;
    M.key_wait = 1;
}

int menu_hold_ticks(int ticks)
{
    init();
    if (!M.open && M.at >= 0 && !M.done && ticks >= M.at) {
        M.done = 1;
        do_open();
    }
    return M.open;
}

int menu_open(void) { return M.open; }
int menu_busy(void) { return M.open && M.keys && *M.keys; }
void menu_set_shot(int s) { M.shot = s; }

/* ------------------------------------------------------------ drawing */
static gfx_texture *atlas;
#define AT_W 128
#define AT_H 64

static void atlas_make(void)
{
    static unsigned char px[AT_W * AT_H * 4];
    int c, x, y;
    memset(px, 0, sizeof px);
    for (c = 32; c < 128; c++) {
        const unsigned char *rows = font5x7_rows(c);
        int cx = ((c - 32) % 16) * 8, cy = ((c - 32) / 16) * 8;
        for (y = 0; y < 7; y++)
            for (x = 0; x < 5; x++)
                if (rows[y] & (16 >> x)) {
                    unsigned char *o = px + ((cy + y) * AT_W + cx + x) * 4;
                    o[0] = o[1] = o[2] = o[3] = 255;
                }
    }
    atlas = gfx_create_texture(AT_W, AT_H, px);
}

typedef struct { float *pos, *st; unsigned char *col; int n, cap; } batch;
static batch bq, bt;                      /* untextured quads, text quads */

static void bgrow(batch *b, int textured)
{
    if (b->n + 6 <= b->cap)
        return;
    b->cap = b->cap ? b->cap * 2 : 4096;
    b->pos = (float *)realloc(b->pos, (size_t)b->cap * 2 * sizeof(float));
    b->col = (unsigned char *)realloc(b->col, (size_t)b->cap * 4);
    if (textured)
        b->st = (float *)realloc(b->st, (size_t)b->cap * 2 * sizeof(float));
}

static void quad(float x0, float y0, float x1, float y1, unsigned rgba)
{
    float p[12] = { x0, y0, x1, y0, x0, y1, x1, y0, x1, y1, x0, y1 };
    int i;
    bgrow(&bq, 0);
    memcpy(bq.pos + 2 * bq.n, p, sizeof p);
    for (i = 0; i < 6; i++) {
        unsigned char *c = bq.col + 4 * (bq.n + i);
        c[0] = (unsigned char)(rgba >> 24);
        c[1] = (unsigned char)(rgba >> 16);
        c[2] = (unsigned char)(rgba >> 8);
        c[3] = (unsigned char)rgba;
    }
    bq.n += 6;
}

static void glyph(float x, float y, int sc, int ch, unsigned rgba)
{
    int cx, cy, i;
    float u0, v0, u1, v1, x1 = x + 6 * sc, y1 = y + 7 * sc;
    float p[12] = { x, y, x1, y, x, y1, x1, y, x1, y1, x, y1 };
    float s[12];
    if (ch < 32 || ch > 127)
        ch = '?';
    cx = ((ch - 32) % 16) * 8;
    cy = ((ch - 32) / 16) * 8;
    u0 = (float)cx / AT_W;
    u1 = (float)(cx + 6) / AT_W;
    v0 = (float)cy / AT_H;
    v1 = (float)(cy + 7) / AT_H;
    {
        float t[12] = { u0, v0, u1, v0, u0, v1, u1, v0, u1, v1, u0, v1 };
        memcpy(s, t, sizeof s);
    }
    bgrow(&bt, 1);
    memcpy(bt.pos + 2 * bt.n, p, sizeof p);
    memcpy(bt.st + 2 * bt.n, s, sizeof s);
    for (i = 0; i < 6; i++) {
        unsigned char *c = bt.col + 4 * (bt.n + i);
        c[0] = (unsigned char)(rgba >> 24);
        c[1] = (unsigned char)(rgba >> 16);
        c[2] = (unsigned char)(rgba >> 8);
        c[3] = (unsigned char)rgba;
    }
    bt.n += 6;
}

static void text(int x, int y, int sc, const char *s, unsigned rgba)
{
    for (; *s; s++, x += 6 * sc) {
        if (*s == ' ')
            continue;
        glyph((float)(x + sc), (float)(y + sc), sc, *s, 0x000000A0u);      /* shadow */
        glyph((float)x, (float)y, sc, *s, rgba);
    }
}

static int text_w(int sc, const char *s) { return (int)strlen(s) * 6 * sc; }

static void flush(int W, int H)
{
    if (bq.n) {
        gfx_set_render_state(GFX_RS_TEXTURE, 0);
        gfx_draw_2d(W, H, bq.n, bq.pos, NULL, bq.col);
    }
    if (bt.n) {
        gfx_set_render_state(GFX_RS_TEXTURE, (uintptr_t)atlas);
        gfx_draw_2d(W, H, bt.n, bt.pos, bt.st, bt.col);
    }
    bq.n = bt.n = 0;
}

static void states_on(void)
{
    gfx_set_2d_anchor(GFX_A_CENTER);
    gfx_set_render_state(GFX_RS_ALPHA_FUNC, 7);
    gfx_set_render_state(GFX_RS_FOG_ENABLE, 0);
    gfx_set_render_state(GFX_RS_BLEND, 1);
    gfx_set_render_state(GFX_RS_FILTER, 0x10000);
    gfx_set_render_state(GFX_RS_TEX_CLAMP, 1);
}

static void states_off(void)
{
    gfx_set_render_state(GFX_RS_ALPHA_FUNC, 4);
    gfx_set_render_state(GFX_RS_BLEND, 1);
    gfx_set_render_state(GFX_RS_FILTER, 0);
    gfx_set_render_state(GFX_RS_TEX_CLAMP, 0);
    rt_2d_restore_texture();
}

void menu_draw(void)
{
    int W, H, sc, lh, pw, ph, x0, y0, i, pad, vx;
    char buf[64];
    const char *note = "";
    init();
    script_keys();
#ifdef XBOX
    W = 320;                            /* a 320 x 240 frame on the 4:3 picture: the 5x7 font at 2x on 640 x 480 */
    H = 240;
    sc = 1;
#else
    gfx_size(&W, &H);
    sc = H >= 1000 ? 3 : H >= 480 ? 2 : 1;
#endif
    if (!atlas)
        atlas_make();
    if (!M.open) {
        if (rt_boot_title()) {          /* the hint on the title screen */
            const char *h = XB("Back + L3: SETTINGS", M.shot ? "F10: PC SETTINGS" : "F10 or Back+L3: PC SETTINGS");
            int hs = H >= 700 ? 2 : 1;
            states_on();
            text(XB(24, 8 * hs), H - XB(24, 12 * hs), hs, h, 0xFFFFFFC0u);     /* Xbox: inside the TV's overscan */
            flush(W, H);
            states_off();
        }
        return;
    }
    while (sc > 1 && 58 * 6 * sc > W)
        sc--;
    lh = 12 * sc;
    pad = 8 * sc;
    pw = XB(288, 58 * 6 * sc);           /* Xbox: 32 px in from each side of the screen (TV overscan) */
    ph = pad * 2 + lh * (NROWS + 4);
    x0 = (W - pw) / 2;
    y0 = (H - ph) / 2;
    if (y0 < 0)
        y0 = 0;
    states_on();
    gfx_set_2d_anchor(GFX_A_STRETCH);   /* the dimming covers the whole screen (also beside a 4:3 frame on a wide one) */
    quad(0, 0, (float)W, (float)H, 0x000000A8u);
    flush(W, H);
    gfx_set_2d_anchor(GFX_A_CENTER);
    quad((float)x0, (float)y0, (float)(x0 + pw), (float)(y0 + ph), 0x10141CF0u);
    quad((float)x0, (float)y0, (float)(x0 + pw), (float)(y0 + 2 * sc), 0xE0A040FFu);
    quad((float)x0, (float)(y0 + ph - 2 * sc), (float)(x0 + pw), (float)(y0 + ph), 0xE0A040FFu);
    text(x0 + pad, y0 + pad, sc, XB("MONSTER HUNTER SETTINGS", "MONSTER HUNTER PC SETTINGS"), 0xFFD060FFu);
    M.px0 = x0;
    M.px1 = x0 + pw;
    M.row_h = lh;
    vx = x0 + pad + XB(22, 26) * 6 * sc;
    for (i = 0; i < NROWS; i++) {
        int r = rows[i], y = y0 + pad + lh * (i + 2);
        const char *v, *n;
        unsigned col = i == M.sel ? 0xFFFFFFFFu : 0xB8C0D0FFu;
        M.row_y[i] = y - sc * 2;
        if (i == M.sel)
            quad((float)(x0 + sc * 3), (float)(y - sc * 2), (float)(x0 + pw - sc * 3), (float)(y + lh - sc * 3), 0x2F5F9FE0u);
        text(x0 + pad, y, sc, label[r], col);
        v = value(r, buf, sizeof buf, &n);
        if (r == R_CLOSE)
            text(vx, y, sc, XB("(B / Start)", "(Esc / F10)"), 0x8890A0FFu);
        else if (r == R_DEFAULTS)
            text(vx, y, sc, "(Enter)", 0x8890A0FFu);
        else {
            if (i == M.sel) {
                text(vx - 8 * sc, y, sc, "<", 0xFFD060FFu);
                text(vx + text_w(sc, v) + 2 * sc, y, sc, ">", 0xFFD060FFu);
            }
            text(vx, y, sc, v, i == M.sel ? 0xFFFF90FFu : 0xE0E8F0FFu);
        }
        if (i == M.sel)
            note = n;
    }
    text(x0 + pad, y0 + ph - pad - lh * 2 + sc * 2, sc, note, 0x90D090FFu);
    text(x0 + pad, y0 + ph - pad - lh + sc, sc, XB("D-pad select, Left/Right change, B close",
                                                   "Up/Down select  Left/Right change  Enter next  Esc close"), 0x8890A0FFu);
    flush(W, H);
    states_off();
}
