/*
 * dbgmenu.c - the testers' debug menu (docs/pc.md "Debug menu", docs/PLAYING.md "Debug menu for testers").
 *
 * Every platform and both games (PC and Xbox, MH1 and G): Back + R3 on the controller opens and closes it (F9 on the
 * PC). The game does not tick while it is open (the viewer asks dbgmenu_hold_ticks, as for the F10 settings menu) and
 * the last picture stays drawn under it. It is drawn with the game's own font (rt_font.c rt_font_dbg_text: Japanese item
 * and quest names show as the game writes them) in the game's 640 x 448 text frame, so it looks the same on the GL and
 * the nv2a backends.
 *
 * Controller: D-pad / left stick up/down select, left/right change a value (L1 / R1 in bigger steps, or a page in
 * lists), cross = enter, circle = back, square = clear (an inventory slot), start or Back + R3 = close. Keyboard (PC):
 * arrows, PgUp / PgDn, Enter, Backspace / Esc back, Delete clear, F9 close. Mouse (PC): wheel, click = enter, right
 * click = back.
 *
 * What the rows do is in rt_dbg.c (the game's own data) and in the viewer (quest start, village, title: the same paths
 * the game takes). Enabled: dbgmenu_init's flag (viewer.c: the build default, mh1pc.ini `debug_menu`, --debug-menu,
 * RT_DEBUG_MENU). Test aid RT_DBG_SHOTS=prefix: a PNG of the menu on every new page and after every action
 * (prefix_NN.png), for scripted runs that drive it with --input.
 */
#include "dbgmenu.h"
#include "gfx/gfx.h"
#include "rt/rt_dbg.h"
#include "rt/rt_log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef XBOX
#include <SDL.h>
#endif

float rt_font_dbg_text(float x, float y, float size, const char *s, uint32_t rgba, float max_w);
float rt_font_dbg_width(float size, const char *s);
int rt_font_dbg_ready(void);
void rt_2d_restore_texture(void);
int rt_ver_mhg(void);
int rt_flow_mode(void);
int rt_village_active(void);
int rt_boot_active(void);
void audio_pause(int on);

/* ------------------------------------------------------------ state */
enum { P_MAIN, P_QUEST, P_AREA, P_INV, P_ITEM, P_EQUIP, P_EQLIST, P_N };
typedef struct { int page, sel, top, arg; } level;

static struct {
    int enabled, open;
    dbgmenu_actions act;
    level st[6];
    int depth;
    int pad_prev, hold_dir, hold_rep, close_wait;
    int frame_step, step_pending, r3_held;
    int mask;                       /* buttons held when the menu closed: the game sees them only after a release */
    int freecam;
    /* the main page's values */
    int hp_pct_i, dmg_i;
    /* lists */
    int areas[64];
    float area_dest[64][4];
    int nareas;
    char msg[128];
    int msg_frames;
    /* scripted shots */
    const char *shot_prefix;
    int shot_n, shot_due;
    char shot_name[512];
    /* drawn layout (virtual 640 x 448), for the mouse */
    float row_y0, row_h;
    int nrows_vis;
} M;

static const int hp_pcts[] = { 100, 75, 50, 25, 10, 1 };
static const int dmg_muls[] = { 1, 2, 3, 5, 10, 20, 50 };
#define NHP ((int)(sizeof hp_pcts / sizeof hp_pcts[0]))
#define NDMG ((int)(sizeof dmg_muls / sizeof dmg_muls[0]))

void dbgmenu_init(int enabled, const dbgmenu_actions *a)
{
    M.enabled = enabled;
    if (a)
        M.act = *a;
    M.shot_prefix = getenv("RT_DBG_SHOTS");
    rt_log("debug menu: %s (Back + R3%s)", enabled ? "available" : "off", enabled ? ", F9 on the PC" : "");
}
int dbgmenu_enabled(void) { return M.enabled; }
void dbgmenu_set_enabled(int on)            /* the settings menu's "Debug menu" row */
{
    M.enabled = on;
    if (!on)
        M.frame_step = M.step_pending = M.freecam = 0;
    rt_log("debug menu: %s (settings menu)", on ? "available" : "off");
}
int dbgmenu_freecam(void) { return M.freecam; }

static level *cur(void) { return &M.st[M.depth]; }
static void note(const char *fmt, const char *s)
{
    snprintf(M.msg, sizeof M.msg, fmt, s);
    M.msg_frames = 150;
    rt_log("debug menu: %s", M.msg);
}
static void shot_soon(void) { if (M.shot_prefix) M.shot_due = 2; }

static void push(int page, int arg)
{
    if (M.depth + 1 >= (int)(sizeof M.st / sizeof M.st[0]))
        return;
    M.depth++;
    memset(cur(), 0, sizeof *cur());
    cur()->page = page;
    cur()->arg = arg;
    shot_soon();
}
static void pop(void)
{
    if (M.depth > 0)
        M.depth--;
    shot_soon();
}

static void do_open(void)
{
    if (M.open || !M.enabled)
        return;
    M.open = 1;
    M.depth = 0;
    memset(M.st, 0, sizeof M.st);
    M.pad_prev = 0xFFFF;                /* the buttons held to open it are not presses */
    M.hold_dir = M.close_wait = 0;
    audio_pause(1);
    rt_font_dbg_ready();
    rt_log("debug menu: opened (game mode %d%s)", rt_flow_mode(), rt_boot_active() ? ", boot screens" : rt_village_active() ? ", village" : "");
    shot_soon();
}
static void do_close(void)
{
    if (!M.open)
        return;
    M.open = 0;
    M.mask = 0xFFFF;
    audio_pause(0);
    rt_log("debug menu: closed");
}

/* ------------------------------------------------------------ the main page */
enum { R_QUEST, R_AREA, R_VILLAGE, R_VILLAGE_INTRO, R_TITLE, R_POUCH, R_BOX, R_MONEY, R_EQUIP, R_HR, R_CLEAR,
       R_GOD, R_ONEHIT, R_DMG, R_HP, R_KILL, R_WARP, R_TIMER, R_HITBOX, R_STEP, R_FREECAM, R_CLOSE, R_N };
static int main_rows[R_N], n_main;

static int in_game(void) { return !rt_boot_active(); }
static int in_quest(void) { return in_game() && rt_flow_mode() == 2 && !rt_village_active(); }
/* rows shown greyed out and refused: the village jumps need a hunter (from the title screen G crashed there,
 * owner's Xbox, 16 Oct 2026; docs/RELEASE_G.md GB5 bug 7) */
static int row_off(int r) { return (r == R_VILLAGE || r == R_VILLAGE_INTRO) && !in_game(); }

static void main_build(void)
{
    int r;
    n_main = 0;
    for (r = 0; r < R_N; r++) {
        if ((r == R_AREA || r == R_KILL || r == R_WARP || r == R_HP || r == R_TIMER) && !in_quest())
            continue;
        if ((r == R_POUCH || r == R_BOX || r == R_MONEY || r == R_EQUIP || r == R_HR || r == R_CLEAR) && !in_game())
            continue;
#ifdef XBOX
        if (r == R_FREECAM)
            continue;
#endif
        main_rows[n_main++] = r;
    }
}

static const char *onoff(int v) { return v ? "on" : "off"; }

static void main_label(int r, char *l, size_t ln, char *v, size_t vn)
{
    *v = 0;
    switch (r) {
    case R_QUEST: snprintf(l, ln, "Start a quest  >"); break;
    case R_AREA: snprintf(l, ln, "Area warp (the areas this quest file links)  >"); break;
    case R_VILLAGE: snprintf(l, ln, "Go to the village (no intro)"); break;
    case R_VILLAGE_INTRO: snprintf(l, ln, "Go to the village (with the first-visit intro)"); break;
    case R_TITLE: snprintf(l, ln, "Title screen (soft reset)"); break;
    case R_POUCH: snprintf(l, ln, "Item pouch%s  >", rt_dbg_in_quest() ? " (this quest)" : ""); break;
    case R_BOX: snprintf(l, ln, "Item box  >"); break;
    case R_MONEY: snprintf(l, ln, "Money"); snprintf(v, vn, "%d z", rt_dbg_money()); break;
    case R_EQUIP: snprintf(l, ln, "Equipment  >"); break;
    case R_HR: snprintf(l, ln, "Hunter rank"); snprintf(v, vn, "%d", rt_dbg_hr()); break;
    case R_CLEAR: snprintf(l, ln, "Mark all offline quests cleared"); break;
    case R_GOD: snprintf(l, ln, "Can't faint"); snprintf(v, vn, "%s", onoff(rt_dbg.god)); break;
    case R_ONEHIT: snprintf(l, ln, "One-hit kills"); snprintf(v, vn, "%s", onoff(rt_dbg.one_hit)); break;
    case R_DMG: snprintf(l, ln, "Damage to monsters"); snprintf(v, vn, "x%d", rt_dbg.dmg_mul); break;
    case R_HP: snprintf(l, ln, "Set monster HP (this area; Enter)"); snprintf(v, vn, "%d%%", hp_pcts[M.hp_pct_i]); break;
    case R_KILL: snprintf(l, ln, "Kill the monsters in this area"); break;
    case R_WARP: snprintf(l, ln, "Warp to the monster"); break;
    case R_TIMER: snprintf(l, ln, "Quest timer"); snprintf(v, vn, "%s", rt_dbg.timer_stop ? "stopped" : "running"); break;
    case R_HITBOX: snprintf(l, ln, "Show hit volumes"); snprintf(v, vn, "%s", onoff(rt_dbg.hitboxes)); break;
    case R_STEP: snprintf(l, ln, "Frame step (R3 / F7: one tick)"); snprintf(v, vn, "%s", onoff(M.frame_step)); break;
    case R_FREECAM: snprintf(l, ln, "Free camera (WASD, mouse; Space/C up/down)"); snprintf(v, vn, "%s", onoff(M.freecam)); break;
    case R_CLOSE: snprintf(l, ln, "Close"); break;
    }
}

static void main_change(int r, int d, int big)
{
    switch (r) {
    case R_MONEY: rt_dbg_set_money(rt_dbg_money() + d * (big ? 100000 : 1000)); break;
    case R_HR: rt_dbg_set_hr(rt_dbg_hr() + d * (big ? 10 : 1)); break;
    case R_GOD: rt_dbg.god = !rt_dbg.god; break;
    case R_ONEHIT: rt_dbg.one_hit = !rt_dbg.one_hit; break;
    case R_DMG: {
        int i = 0;
        while (i < NDMG - 1 && dmg_muls[i] != rt_dbg.dmg_mul)
            i++;
        rt_dbg.dmg_mul = dmg_muls[(i + d + NDMG) % NDMG];
        break;
    }
    case R_HP: M.hp_pct_i = (M.hp_pct_i + d + NHP) % NHP; break;
    case R_TIMER: rt_dbg.timer_stop = !rt_dbg.timer_stop; break;
    case R_HITBOX: rt_dbg.hitboxes = !rt_dbg.hitboxes; break;
    case R_STEP: M.frame_step = !M.frame_step; M.step_pending = 0; break;
    case R_FREECAM: M.freecam = !M.freecam; break;
    default: return;
    }
    rt_log("debug menu: changed row %d", r);
}

static void main_enter(int r)
{
    switch (r) {
    case R_QUEST: push(P_QUEST, 0); break;
    case R_AREA:
        M.nareas = rt_dbg_area_list(M.areas, M.area_dest, 64);
        push(P_AREA, 0);
        break;
    case R_VILLAGE: case R_VILLAGE_INTRO:
        if (row_off(r)) {
            note("%s", "Load or make a hunter first (CONTINUE / NEW GAME)");
            break;
        }
        do_close();
        if (M.act.village)
            M.act.village(r == R_VILLAGE_INTRO);
        shot_soon();
        break;
    case R_TITLE:
        do_close();
        if (M.act.title)
            M.act.title();
        break;
    case R_POUCH: push(P_INV, DBG_POUCH); break;
    case R_BOX: push(P_INV, DBG_BOX); break;
    case R_EQUIP: push(P_EQUIP, 0); break;
    case R_CLEAR: {
        char b[32];
        snprintf(b, sizeof b, "%d", rt_dbg_clear_all_quests());
        note("%s offline quests marked cleared (the Elder's board fills at the next village visit)", b);
        break;
    }
    case R_HP:
        rt_dbg_monsters_hp(hp_pcts[M.hp_pct_i]);
        note("monster HP set to %s", "the chosen %");
        break;
    case R_KILL:
        rt_dbg.kill_req = 1;
        do_close();
        break;
    case R_WARP: {
        int s = rt_dbg_monster_slot();
        if (s < 0) {
            note("%s", "no living monster in this area");
            break;
        }
        rt_dbg.warp_em_slot = s;
        do_close();
        break;
    }
    case R_CLOSE: do_close(); break;
    default: main_change(r, 1, 0); break;
    }
    shot_soon();
}

/* ------------------------------------------------------------ lists */
static int list_count(const level *l)
{
    switch (l->page) {
    case P_MAIN: return n_main;
    case P_QUEST: return rt_dbg_quest_count();
    case P_AREA: return M.nareas;
    case P_INV: return rt_dbg_inv_slots(l->arg);
    case P_ITEM: return rt_dbg_item_count();
    case P_EQUIP: return DBG_EQ_N;
    case P_EQLIST: return rt_dbg_equip_count(l->arg);
    }
    return 0;
}

static const char *eq_kind_name[DBG_EQ_N] = { "Weapon (blade)", "Weapon (bowgun)", "Head", "Body", "Arms", "Waist", "Legs" };

static void list_enter(void)
{
    level *l = cur();
    if (list_count(l) <= 0)
        return;
    switch (l->page) {
    case P_MAIN: main_enter(main_rows[l->sel]); return;
    case P_QUEST: {
        int no = rt_dbg_quest_no(l->sel);
        do_close();
        if (M.act.start_quest)
            M.act.start_quest(no);
        shot_soon();
        return;
    }
    case P_AREA:
        rt_dbg_area_warp(M.areas[l->sel], M.area_dest[l->sel]);
        do_close();
        return;
    case P_INV: {
        int cnt, id = rt_dbg_inv_get(l->arg, l->sel, &cnt);
        push(P_ITEM, l->sel);
        cur()->sel = id > 0 ? id : 1;
        cur()->top = cur()->sel - 4 > 0 ? cur()->sel - 4 : 0;
        return;
    }
    case P_ITEM: {      /* the item for the slot of the inventory page below */
        level *inv = &M.st[M.depth - 1];
        int cnt, id = rt_dbg_inv_get(inv->arg, inv->sel, &cnt), max = rt_dbg_item_max(l->sel);
        (void)id;
        if (cnt <= 0)
            cnt = 1;
        rt_dbg_inv_set(inv->arg, inv->sel, l->sel, cnt > max ? max : cnt);
        pop();
        return;
    }
    case P_EQUIP: {
        int id = rt_dbg_equip_get(l->sel);
        push(P_EQLIST, l->sel);
        cur()->sel = id > 0 ? id : 0;
        cur()->top = cur()->sel - 4 > 0 ? cur()->sel - 4 : 0;
        return;
    }
    case P_EQLIST:
        if (rt_dbg_equip_set(l->arg, l->sel) == 0)
            note("%s", "equipped (the hunter carries it from the next quest or village visit)");
        else
            note("%s", "the equipment box is full");
        pop();
        return;
    }
}

static void list_change(int d, int big)
{
    level *l = cur();
    int n = list_count(l);
    if (l->page == P_MAIN) {
        if (n > 0)
            main_change(main_rows[l->sel], d, big);
        return;
    }
    if (l->page == P_INV) {         /* the count of that slot */
        int cnt, id = rt_dbg_inv_get(l->arg, l->sel, &cnt), max;
        if (id <= 0)
            return;
        max = l->arg == DBG_BOX ? 99 : rt_dbg_item_max(id);
        cnt += d * (big ? 10 : 1);
        rt_dbg_inv_set(l->arg, l->sel, id, cnt < 1 ? 1 : cnt > max ? max : cnt);
        return;
    }
    if (n <= 0)                     /* other lists: left / right move a page (10), L1 / R1 by 100 */
        return;
    l->sel += d * (big ? 100 : 10);
    if (l->sel < 0) l->sel = 0;
    if (l->sel >= n) l->sel = n - 1;
}

static void list_clear(void)
{
    level *l = cur();
    if (l->page == P_INV)
        rt_dbg_inv_set(l->arg, l->sel, 0, 0);
}

static void move(int d)
{
    level *l = cur();
    int n = list_count(l);
    if (n <= 0)
        return;
    l->sel = (l->sel + d + n) % n;
}

/* ------------------------------------------------------------ input */
enum { K_UP, K_DOWN, K_LEFT, K_RIGHT, K_PGUP, K_PGDN, K_ENTER, K_BACK, K_CLEAR, K_CLOSE };
static void key(int k)
{
    if (!M.open)
        return;
    if (cur()->page == P_MAIN)
        main_build();
    switch (k) {
    case K_UP: move(-1); break;
    case K_DOWN: move(1); break;
    case K_LEFT: list_change(-1, 0); break;
    case K_RIGHT: list_change(1, 0); break;
    case K_PGUP: list_change(-1, 1); break;
    case K_PGDN: list_change(1, 1); break;
    case K_ENTER: list_enter(); break;
    case K_BACK: if (M.depth > 0) pop(); else do_close(); break;
    case K_CLEAR: list_clear(); break;
    case K_CLOSE: do_close(); break;
    }
}

#define COMBO (PAD_SELECT | PAD_R3)
void dbgmenu_tick_pad(pad_state *ps)
{
    int b = ps->bits;
    static int prev;
    if (M.step_pending > 0)
        M.step_pending--;
    ps->bits &= (uint16_t)~M.mask;          /* the press that closed the menu (cross, circle, Enter = start) stays ours */
    M.mask &= b;
    if (M.enabled && (b & COMBO) == COMBO && (prev & COMBO) != COMBO) {
        prev = b;
        ps->bits &= (uint16_t)~COMBO;       /* the game does not see the combo */
        do_open();
        return;
    }
    prev = b;
}

int dbgmenu_hold_ticks(void)
{
    return M.open || (M.frame_step && M.step_pending <= 0);
}
int dbgmenu_wants_pad(void) { return M.open || M.frame_step; }
int dbgmenu_busy(void) { return M.open || M.shot_due > 0; }

void dbgmenu_pad(const pad_state *p)
{
    int b = p->bits, dx = 0, dy = 0, now;
    if (!M.open) {              /* frame step: R3 one tick, held: run; Back + R3 opens the menu */
        if ((b & COMBO) == COMBO && (M.pad_prev & COMBO) != COMBO)
            do_open();
        else if ((b & PAD_R3) && !(b & PAD_SELECT)) {
            if (!(M.pad_prev & PAD_R3) || ++M.r3_held > 15)
                M.step_pending = 1;
        } else
            M.r3_held = 0;
        M.pad_prev = b;
        return;
    }
    if ((b & PAD_UP) || p->ly < -70) dy = -1;
    else if ((b & PAD_DOWN) || p->ly > 70) dy = 1;
    if ((b & PAD_LEFT) || p->lx < -70) dx = -1;
    else if ((b & PAD_RIGHT) || p->lx > 70) dx = 1;
    now = dy * 3 + dx;              /* a direction held: at once, then repeating */
    if (now == 0)
        M.hold_dir = M.hold_rep = 0;
    else if (now != M.hold_dir || --M.hold_rep <= 0) {
        M.hold_rep = now != M.hold_dir ? 12 : 3;
        M.hold_dir = now;
        if (dy) key(dy < 0 ? K_UP : K_DOWN);
        else key(dx < 0 ? K_LEFT : K_RIGHT);
    }
#define PRESS(m) ((b & (m)) && !(M.pad_prev & (m)))
    if (PRESS(PAD_CROSS)) key(K_ENTER);
    if (PRESS(PAD_SQUARE)) key(K_CLEAR);
    if (PRESS(PAD_L1)) key(K_PGUP);
    if (PRESS(PAD_R1)) key(K_PGDN);
    if (PRESS(PAD_CIRCLE)) key(K_BACK);
    /* close on release (the game must not see the press): start, or Back + R3 */
    if ((b & PAD_START) || (b & COMBO) == COMBO) {
        if (!(M.pad_prev & PAD_START) && (M.pad_prev & COMBO) != COMBO)
            M.close_wait = 1;
    } else if (M.close_wait && !(b & (PAD_START | PAD_SELECT | PAD_R3))) {
        M.close_wait = 0;
        key(K_CLOSE);
    }
#undef PRESS
    M.pad_prev = b;
}

#ifndef XBOX
/* the window's 4:3 rectangle, where the 640 x 448 frame is drawn (GFX_A_CENTER) */
static void to_virtual(int mx, int my, float *x, float *y)
{
    int W, H;
    float rw, rh, rx, ry;
    gfx_size(&W, &H);
    if (W * 3 > H * 4) {
        rh = (float)H;
        rw = H * 4.0f / 3;
    } else {
        rw = (float)W;
        rh = W * 3.0f / 4;
    }
    rx = (W - rw) / 2;
    ry = (H - rh) / 2;
    *x = (mx - rx) * 640.0f / rw;
    *y = (my - ry) * 448.0f / rh;
}

static int row_at(int mx, int my)
{
    float x, y;
    int i;
    to_virtual(mx, my, &x, &y);
    if (x < 20 || x > 620 || y < M.row_y0 || M.row_h <= 0)
        return -1;
    i = (int)((y - M.row_y0) / M.row_h);
    return i < M.nrows_vis ? cur()->top + i : -1;
}

int dbgmenu_event(const void *p)
{
    const SDL_Event *e = (const SDL_Event *)p;
    if (!M.enabled)
        return 0;
    if (!M.open) {
        if (e->type == SDL_KEYDOWN && !e->key.repeat && e->key.keysym.sym == SDLK_F9) {
            do_open();
            return 1;
        }
        if (M.frame_step && e->type == SDL_KEYDOWN && (e->key.keysym.sym == SDLK_F7 || e->key.keysym.sym == SDLK_PERIOD)) {
            M.step_pending = 1;     /* (key repeat: held, it runs) */
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
        case SDLK_PAGEUP: key(K_PGUP); break;
        case SDLK_PAGEDOWN: key(K_PGDN); break;
        case SDLK_DELETE: key(K_CLEAR); break;
        case SDLK_RETURN: case SDLK_KP_ENTER: case SDLK_SPACE: if (!e->key.repeat) key(K_ENTER); break;
        case SDLK_ESCAPE: case SDLK_BACKSPACE: if (!e->key.repeat) key(K_BACK); break;
        case SDLK_F9: if (!e->key.repeat) key(K_CLOSE); break;
        default: break;
        }
        return 1;
    case SDL_KEYUP: case SDL_TEXTINPUT: case SDL_TEXTEDITING: case SDL_MOUSEBUTTONUP:
        return 1;
    case SDL_MOUSEMOTION: {
        int r = row_at(e->motion.x, e->motion.y);
        if (r >= 0 && r < list_count(cur()))
            cur()->sel = r;
        return 1;
    }
    case SDL_MOUSEBUTTONDOWN: {
        int r = row_at(e->button.x, e->button.y);
        if (e->button.button == SDL_BUTTON_RIGHT)
            key(K_BACK);
        else if (r >= 0 && r < list_count(cur())) {
            cur()->sel = r;
            key(K_ENTER);
        }
        return 1;
    }
    case SDL_MOUSEWHEEL:
        key(e->wheel.y > 0 ? K_UP : K_DOWN);
        return 1;
    default:
        return 0;
    }
}
#else
int dbgmenu_event(const void *p) { (void)p; return 0; }
#endif

/* ------------------------------------------------------------ drawing (640 x 448 frame) */
static void quad(float x0, float y0, float x1, float y1, uint32_t rgba)
{
    float p[12] = { x0, y0, x1, y0, x0, y1, x1, y0, x1, y1, x0, y1 };
    uint8_t c[24];
    int i;
    for (i = 0; i < 6; i++) {
        c[4 * i] = (uint8_t)(rgba >> 24);
        c[4 * i + 1] = (uint8_t)(rgba >> 16);
        c[4 * i + 2] = (uint8_t)(rgba >> 8);
        c[4 * i + 3] = (uint8_t)rgba;
    }
    gfx_set_render_state(GFX_RS_TEXTURE, 0);
    gfx_draw_2d(640, 448, 6, p, NULL, c);
}

static void line2d(float x0, float y0, float x1, float y1, uint32_t rgba)     /* in the 512 x 448 frame (gfx_project) */
{
    float dx = x1 - x0, dy = y1 - y0, l = dx * dx + dy * dy, nx, ny;
    uint8_t c[24];
    int i;
    if (l < 1e-6f)
        return;
    {
        float s = 0.9f / (float)__builtin_sqrtf(l);
        nx = -dy * s;
        ny = dx * s;
    }
    {
        float p[12] = { x0 + nx, y0 + ny, x1 + nx, y1 + ny, x0 - nx, y0 - ny, x1 + nx, y1 + ny, x1 - nx, y1 - ny, x0 - nx, y0 - ny };
        for (i = 0; i < 6; i++) {
            c[4 * i] = (uint8_t)(rgba >> 24);
            c[4 * i + 1] = (uint8_t)(rgba >> 16);
            c[4 * i + 2] = (uint8_t)(rgba >> 8);
            c[4 * i + 3] = (uint8_t)rgba;
        }
        gfx_set_render_state(GFX_RS_TEXTURE, 0);
        gfx_draw_2d(512, 448, 6, p, NULL, c);
    }
}

static float text(float x, float y, float size, const char *s, uint32_t rgba, float max_w)
{
    return rt_font_dbg_text(x, y, size, s ? s : "", rgba, max_w);
}

static void states_on(void)
{
    gfx_set_2d_anchor(GFX_A_CENTER);
    gfx_set_render_state(GFX_RS_ALPHA_FUNC, 7);
    gfx_set_render_state(GFX_RS_FOG_ENABLE, 0);
    gfx_set_render_state(GFX_RS_BLEND, 1);
    gfx_set_render_state(GFX_RS_FILTER, 0);
    gfx_set_render_state(GFX_RS_TEX_CLAMP, 1);
    gfx_set_render_state(GFX_RS_ZTEST, 0);
    gfx_set_render_state(GFX_RS_FADE_COLOR, 0x80808080u);
}

static void states_off(void)
{
    gfx_set_render_state(GFX_RS_ALPHA_FUNC, 4);
    gfx_set_render_state(GFX_RS_BLEND, 1);
    gfx_set_render_state(GFX_RS_TEX_CLAMP, 0);
    /* the depth test and the fade colour back as the scene uses them: nothing else switches the depth test on again, so
     * after the menu had been open once every later frame drew without it (owner, G on the Xbox, 16 Oct 2026: wrong
     * draw order after the debug menu's quest start, "until restart"; RELEASE_G GB5 bug 1) */
    gfx_set_render_state(GFX_RS_ZTEST, 1);
    gfx_set_render_state(GFX_RS_FADE_COLOR, 0xFFFFFFFFu);
    rt_2d_restore_texture();
}

/* the monsters' body volumes (red) and the live attack shells (yellow), projected (rt_dbg_volumes) */
static void draw_volumes(void)
{
    static rt_dbg_vol v[256];
    static const float ident[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
    int n = rt_dbg_volumes(v, 256), i, k;
    gfx_set_render_state(GFX_RS_WORLD, (uintptr_t)ident);
    for (i = 0; i < n; i++) {
        uint32_t col = v[i].kind ? 0xFFE040E0u : 0xFF4040C0u;
        float pa[4], pb[4], q[4], ra, rb;
        float t[3];
        int ends;
        if (!gfx_project(v[i].a, pa) || !gfx_project(v[i].b, pb))
            continue;
        t[0] = v[i].a[0]; t[1] = v[i].a[1] + v[i].r; t[2] = v[i].a[2];      /* screen radius: the point r above the centre */
        ra = gfx_project(t, q) ? (float)__builtin_sqrtf((q[0] - pa[0]) * (q[0] - pa[0]) + (q[1] - pa[1]) * (q[1] - pa[1])) : 0;
        t[0] = v[i].b[0]; t[1] = v[i].b[1] + v[i].r; t[2] = v[i].b[2];
        rb = gfx_project(t, q) ? (float)__builtin_sqrtf((q[0] - pb[0]) * (q[0] - pb[0]) + (q[1] - pb[1]) * (q[1] - pb[1])) : 0;
        if (pa[3] < 30 || pb[3] < 30 || ra > 300 || rb > 300)
            continue;           /* at or behind the eye (a warp right next to a monster): no sensible outline */
        ends = memcmp(v[i].a, v[i].b, sizeof v[i].a) ? 2 : 1;
        for (k = 0; k < 16; k++) {
            float a0 = k * 6.2831853f / 16, a1 = (k + 1) * 6.2831853f / 16;
            float c0 = __builtin_cosf(a0), s0 = __builtin_sinf(a0), c1 = __builtin_cosf(a1), s1 = __builtin_sinf(a1);
            line2d(pa[0] + c0 * ra, pa[1] + s0 * ra, pa[0] + c1 * ra, pa[1] + s1 * ra, col);
            if (ends == 2)
                line2d(pb[0] + c0 * rb, pb[1] + s0 * rb, pb[0] + c1 * rb, pb[1] + s1 * rb, col);
        }
        if (ends == 2) {    /* the capsule's sides */
            float dx = pb[0] - pa[0], dy = pb[1] - pa[1], l = (float)__builtin_sqrtf(dx * dx + dy * dy);
            if (l > 0.5f) {
                float nx = -dy / l, ny = dx / l;
                line2d(pa[0] + nx * ra, pa[1] + ny * ra, pb[0] + nx * rb, pb[1] + ny * rb, col);
                line2d(pa[0] - nx * ra, pa[1] - ny * ra, pb[0] - nx * rb, pb[1] - ny * rb, col);
            }
        }
    }
}

static const char *page_title(const level *l, char *b, size_t n)
{
    switch (l->page) {
    case P_MAIN: snprintf(b, n, "DEBUG MENU  (%s)", rt_ver_mhg() ? "Monster Hunter G" : "Monster Hunter"); break;
    case P_QUEST: snprintf(b, n, "START A QUEST  (%d)", rt_dbg_quest_count()); break;
    case P_AREA: snprintf(b, n, "AREA WARP"); break;
    case P_INV: snprintf(b, n, l->arg == DBG_POUCH ? "ITEM POUCH%s" : "ITEM BOX", rt_dbg_in_quest() ? " (this quest)" : ""); break;
    case P_ITEM: snprintf(b, n, "CHOOSE AN ITEM  (%d)", rt_dbg_item_count()); break;
    case P_EQUIP: snprintf(b, n, "EQUIPMENT"); break;
    case P_EQLIST: snprintf(b, n, "%s  (%d)", eq_kind_name[l->arg], rt_dbg_equip_count(l->arg)); break;
    }
    return b;
}

static const char *help_line(const level *l)
{
    switch (l->page) {
    case P_MAIN: return "Up/Down select  Left/Right change  X/Enter do  O/Esc back  Start/Back+R3/F9 close";
    case P_INV: return "X/Enter choose the item  Left/Right count (L1/R1 10)  Square/Del clear  O/Esc back";
    case P_QUEST: case P_ITEM: case P_EQLIST: return "Up/Down  Left/Right 10  L1/R1 100  X/Enter choose  O/Esc back";
    default: return "Up/Down select  X/Enter choose  O/Esc back";
    }
}

/* one row: x, y, the selected flag; returns the row's height in lines (1 or 2) */
static void draw_row(const level *l, int i, float x, float y, float w, int seln)
{
    char a[160], b[64];
    uint32_t c = seln ? 0xFFFFFFFFu : 0xB8C0D0FFu, cv = seln ? 0xFFFF90FFu : 0xE0E8F0FFu;
    switch (l->page) {
    case P_MAIN: {
        int r = main_rows[i];
        main_label(r, a, sizeof a, b, sizeof b);
        if (row_off(r)) {
            strncat(a, "  (load a hunter first)", sizeof a - strlen(a) - 1);
            c = seln ? 0x9098A8FFu : 0x606878FFu;
        }
        text(x, y, 16, a, c, 0);
        if (*b) {
            if (seln) {
                text(x + 360, y, 16, "<", 0xFFD060FFu, 0);
                text(x + 378 + rt_font_dbg_width(16, b), y, 16, ">", 0xFFD060FFu, 0);
            }
            text(x + 370, y, 16, b, cv, 0);
        }
        break;
    }
    case P_QUEST: {
        int rank;
        const char *t, *g;
        int no = rt_dbg_quest_info(i, &rank, &t, &g);
        if (rt_ver_mhg())
            snprintf(a, sizeof a, "%5d  %s", no, rank == 0 ? "village" : rank < 4 ? "guild" : rank == 4 ? "training" : "other");
        else if (rank)
            snprintf(a, sizeof a, "%3d  %d*", no, rank);
        else
            snprintf(a, sizeof a, "%3d  guild", no);
        text(x, y, 16, a, c, 0);
        text(x + (rt_ver_mhg() ? 130 : 90), y, 16, t ? t : "(no text)", cv, 230);
        text(x + 330, y + 1, 14, g ? g : "", 0x90D090FFu, w - 330);
        break;
    }
    case P_AREA:
        snprintf(a, sizeof a, "Area / stage %d", M.areas[i]);
        text(x, y, 16, a, c, 0);
        break;
    case P_INV: {
        int cnt, id = rt_dbg_inv_get(l->arg, i, &cnt);
        snprintf(a, sizeof a, "%3d", i + 1);
        text(x, y, 16, a, c, 0);
        if (id > 0) {
            snprintf(b, sizeof b, "#%d", id);
            text(x + 40, y, 16, b, 0x8890A0FFu, 0);
            text(x + 100, y, 16, rt_dbg_item_name(id), cv, 360);
            snprintf(b, sizeof b, "x %d", cnt);
            text(x + 480, y, 16, b, cv, 0);
        } else
            text(x + 40, y, 16, "(empty)", 0x707888FFu, 0);
        break;
    }
    case P_ITEM: {
        const char *nm = rt_dbg_item_name(i);
        snprintf(a, sizeof a, "#%d", i);
        text(x, y, 16, a, c, 0);
        text(x + 60, y, 16, nm && *nm ? nm : "-", cv, 360);
        snprintf(b, sizeof b, "max %d", rt_dbg_item_max(i));
        text(x + 460, y, 16, b, 0x8890A0FFu, 0);
        break;
    }
    case P_EQUIP: {
        int id = rt_dbg_equip_get(i);
        text(x, y, 16, eq_kind_name[i], c, 0);
        if (id >= 0) {
            snprintf(b, sizeof b, "#%d", id);
            text(x + 170, y, 16, b, 0x8890A0FFu, 0);
            text(x + 230, y, 16, rt_dbg_equip_name(i, id), cv, 330);
        } else
            text(x + 170, y, 16, "(not equipped)", 0x707888FFu, 0);
        break;
    }
    case P_EQLIST: {
        const char *nm = rt_dbg_equip_name(l->arg, i);
        snprintf(a, sizeof a, "#%d", i);
        text(x, y, 16, a, c, 0);
        text(x + 60, y, 16, nm && *nm ? nm : "-", cv, 480);
        break;
    }
    }
}

void dbgmenu_draw(void)
{
    level *l;
    int n, i, vis;
    float x0 = 20, y0 = 14, x1 = 620, y1 = 434, rh;
    char b[96];
    if (!M.enabled)
        return;
    if (!M.open && !rt_dbg.hitboxes && !M.frame_step && M.msg_frames <= 0 && M.shot_due <= 0)
        return;
    if (!rt_font_dbg_ready())
        return;
    states_on();
    if (rt_dbg.hitboxes)
        draw_volumes();
    if (!M.open) {
        if (M.frame_step)
            text(8, 4, 14, "FRAME STEP: R3 / F7 one tick, held: run", 0xFFD060FFu, 0);
        if (M.msg_frames > 0) {
            M.msg_frames--;
            text(8, 428, 14, M.msg, 0x90FF90FFu, 0);
        }
        states_off();
        goto shot;
    }
    l = cur();
    if (l->page == P_MAIN)
        main_build();
    n = list_count(l);
    if (l->sel >= n)
        l->sel = n > 0 ? n - 1 : 0;
    rh = l->page == P_QUEST ? 20 : 18;
    vis = (int)((y1 - y0 - 70) / rh);
    if (l->sel < l->top)
        l->top = l->sel;
    if (l->sel >= l->top + vis)
        l->top = l->sel - vis + 1;
    M.row_y0 = y0 + 34;
    M.row_h = rh;
    M.nrows_vis = vis;
    quad(0, 0, 640, 448, 0x00000090u);
    quad(x0, y0, x1, y1, 0x10141CE8u);
    quad(x0, y0, x1, y0 + 2, 0x40A0E0FFu);
    quad(x0, y1 - 2, x1, y1, 0x40A0E0FFu);
    text(x0 + 10, y0 + 8, 18, page_title(l, b, sizeof b), 0x80D0FFFFu, 0);
    if (n > vis) {
        snprintf(b, sizeof b, "%d / %d", l->sel + 1, n);
        text(x1 - 10 - rt_font_dbg_width(14, b), y0 + 10, 14, b, 0x8890A0FFu, 0);
    }
    if (n == 0)
        text(x0 + 14, M.row_y0, 16, l->page == P_AREA ? "(no exits found in this quest's map)" : "(nothing here)", 0x8890A0FFu, 0);
    for (i = 0; i < vis && l->top + i < n; i++) {
        float y = M.row_y0 + rh * i;
        int s = l->top + i == l->sel;
        if (s)
            quad(x0 + 4, y - 1, x1 - 4, y + rh - 1, 0x2F5F9FE0u);
        draw_row(l, l->top + i, x0 + 14, y, x1 - x0 - 28, s);
    }
    if (M.msg_frames > 0) {
        M.msg_frames--;
        text(x0 + 10, y1 - 40, 14, M.msg, 0x90FF90FFu, x1 - x0 - 20);
    }
    text(x0 + 10, y1 - 20, 13, help_line(l), 0x8890A0FFu, x1 - x0 - 20);
    states_off();
shot:
    if (M.shot_due > 0 && --M.shot_due == 0 && M.shot_prefix) {
        snprintf(M.shot_name, sizeof M.shot_name, "%s_%02d.png", M.shot_prefix, ++M.shot_n);
    }
}

const char *dbgmenu_take_shot(void)
{
    if (!M.shot_name[0])
        return NULL;
    {
        static char name[512];
        memcpy(name, M.shot_name, sizeof name);
        M.shot_name[0] = 0;
        return name;
    }
}
