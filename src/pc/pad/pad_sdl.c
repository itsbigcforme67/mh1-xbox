/*
 * pad_sdl.c - SDL2 game controller + keyboard backend for pad.h, and the
 * scripted input used by offscreen tests. Xbox-style controllers map by
 * position: A cross, B circle, X square, Y triangle, LB L1, RB R1,
 * LT L2, RT R2, Back select, Start start, stick clicks L3/R3.
 */
#ifdef XBOX
#include "../xbox/kbm_xbox.h"
#endif
#include "pad.h"
#include "../rt/rt_log.h"

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static SDL_GameController *ctl;
static int text_mode;                   /* typing a name: the keyboard is not the pad */
static char text_buf[64];
static int text_n;

void pad_text_mode(int on)
{
    text_mode = on;
    text_n = 0;
    if (on)
        SDL_StartTextInput();
    else
        SDL_StopTextInput();
}

/* the viewer passes every SDL event here (typed text, Backspace, Enter) */
int pad_kb_wanted;                      /* the game's soft keyboard is up: Tab switches typing on/off */
void pad_event(const void *ev)
{
    const SDL_Event *e = ev;
    if (pad_kb_wanted && e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_TAB) {
        pad_text_mode(!text_mode);
        return;
    }
    if (!text_mode)
        return;
    if (e->type == SDL_TEXTINPUT) {
        const char *t = e->text.text;
        for (; *t && text_n < (int)sizeof text_buf; t++)
            if ((unsigned char)*t < 0x80)
                text_buf[text_n++] = *t;
    } else if (e->type == SDL_KEYDOWN && text_n < (int)sizeof text_buf) {
        if (e->key.keysym.sym == SDLK_BACKSPACE)
            text_buf[text_n++] = '\b';
        else if (e->key.keysym.sym == SDLK_RETURN || e->key.keysym.sym == SDLK_KP_ENTER)
            text_buf[text_n++] = '\n';
    }
}

int pad_text_take(char *out, int n)
{
    int k = text_n < n ? text_n : n;
    memcpy(out, text_buf, (size_t)k);
    text_n = 0;
    return k;
}

/* Scripted / hidden / screenshot / audio-dump runs (tests, agents) must never open a real controller: it would
 * read stray input and rumble in the owner's hands. They use the script and the keyboard map only. */
static int headless;
int pad_rumble_off;
void pad_set_headless(int on) { headless = on; }

void pad_init(void)
{
    int i;
    if (headless)
        return;
#ifdef XBOX
    kbm_init();                 /* a USB keyboard / mouse for the bug reporter: its HID driver before SDL starts the USB stack */
#endif
    if (!SDL_WasInit(SDL_INIT_GAMECONTROLLER))
        SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);
    for (i = 0; i < SDL_NumJoysticks() && !ctl; i++)
        if (SDL_IsGameController(i) && (ctl = SDL_GameControllerOpen(i)) != NULL)
            {
                fprintf(stderr, "pad: controller \"%s\"\n", SDL_GameControllerName(ctl));
                rt_log("controller: \"%s\"", SDL_GameControllerName(ctl));
            }
    {
        static int told;        /* pad_init runs again each frame while none is plugged in */
        if (!ctl && !told++)
            rt_log("controller: none found (keyboard only; %d joystick(s) seen); a pad plugged in later is picked up", SDL_NumJoysticks());
    }
}

static int axis(SDL_GameControllerAxis a)
{
    int v = SDL_GameControllerGetAxis(ctl, a) / 256;   /* -128..127 */
    return v < -127 ? -127 : v;
}

void pad_read(pad_state *p, int keyboard)
{
    static const struct { SDL_GameControllerButton b; uint16_t bit; } bmap[] = {
        { SDL_CONTROLLER_BUTTON_A, PAD_CROSS }, { SDL_CONTROLLER_BUTTON_B, PAD_CIRCLE },
        { SDL_CONTROLLER_BUTTON_X, PAD_SQUARE }, { SDL_CONTROLLER_BUTTON_Y, PAD_TRIANGLE },
        { SDL_CONTROLLER_BUTTON_LEFTSHOULDER, PAD_L1 }, { SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, PAD_R1 },
        { SDL_CONTROLLER_BUTTON_BACK, PAD_SELECT }, { SDL_CONTROLLER_BUTTON_START, PAD_START },
        { SDL_CONTROLLER_BUTTON_LEFTSTICK, PAD_L3 }, { SDL_CONTROLLER_BUTTON_RIGHTSTICK, PAD_R3 },
        { SDL_CONTROLLER_BUTTON_DPAD_UP, PAD_UP }, { SDL_CONTROLLER_BUTTON_DPAD_DOWN, PAD_DOWN },
        { SDL_CONTROLLER_BUTTON_DPAD_LEFT, PAD_LEFT }, { SDL_CONTROLLER_BUTTON_DPAD_RIGHT, PAD_RIGHT },
    };
    static const struct { SDL_Scancode k; uint16_t bit; } kmap[] = {
        { SDL_SCANCODE_K, PAD_CROSS }, { SDL_SCANCODE_L, PAD_CIRCLE },
        { SDL_SCANCODE_J, PAD_SQUARE }, { SDL_SCANCODE_I, PAD_TRIANGLE },
        { SDL_SCANCODE_Q, PAD_L1 }, { SDL_SCANCODE_E, PAD_R1 },
        { SDL_SCANCODE_Z, PAD_L2 }, { SDL_SCANCODE_C, PAD_R2 },
        { SDL_SCANCODE_RETURN, PAD_START }, { SDL_SCANCODE_BACKSPACE, PAD_SELECT },
        { SDL_SCANCODE_T, PAD_UP }, { SDL_SCANCODE_G, PAD_DOWN },
        { SDL_SCANCODE_F, PAD_LEFT }, { SDL_SCANCODE_H, PAD_RIGHT },
    };
    size_t i;
    memset(p, 0, sizeof *p);
    if (ctl && !SDL_GameControllerGetAttached(ctl)) {
        rt_log("controller: unplugged");
        SDL_GameControllerClose(ctl);
        ctl = NULL;
    }
    if (!ctl)
        pad_init();
    if (ctl) {
        for (i = 0; i < sizeof bmap / sizeof bmap[0]; i++)
            if (SDL_GameControllerGetButton(ctl, bmap[i].b))
                p->bits |= bmap[i].bit;
        if (SDL_GameControllerGetAxis(ctl, SDL_CONTROLLER_AXIS_TRIGGERLEFT) > 16000)
            p->bits |= PAD_L2;
        if (SDL_GameControllerGetAxis(ctl, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) > 16000)
            p->bits |= PAD_R2;
        p->lx = axis(SDL_CONTROLLER_AXIS_LEFTX);
        p->ly = axis(SDL_CONTROLLER_AXIS_LEFTY);
        p->rx = axis(SDL_CONTROLLER_AXIS_RIGHTX);
        p->ry = axis(SDL_CONTROLLER_AXIS_RIGHTY);
    }
    if (keyboard && !text_mode) {
        const Uint8 *k = SDL_GetKeyboardState(NULL);
        for (i = 0; i < sizeof kmap / sizeof kmap[0]; i++)
            if (k[kmap[i].k])
                p->bits |= kmap[i].bit;
        if (k[SDL_SCANCODE_W]) p->ly = -127;
        if (k[SDL_SCANCODE_S]) p->ly = 127;
        if (k[SDL_SCANCODE_A]) p->lx = -127;
        if (k[SDL_SCANCODE_D]) p->lx = 127;
        if (k[SDL_SCANCODE_UP]) p->ry = -127;
        if (k[SDL_SCANCODE_DOWN]) p->ry = 127;
        if (k[SDL_SCANCODE_LEFT]) p->rx = -127;
        if (k[SDL_SCANCODE_RIGHT]) p->rx = 127;
    }
}

/* ------------------------------------------------------------ script */
/* steps grow as needed (a recorded session of the bug reporter has thousands) */
static pad_state *steps;
static int *ticks, nsteps, capsteps, cur, left;
static int *waits;                      /* per step: 0, or (event + 1) << 16 | n for "@villageN" / "@questN" */
static int ev_count[PAD_EV_N];          /* village entries / quest starts so far (pad_script_event) */

static int name_to(const char *n, size_t len, pad_state *p)
{
    static const struct { const char *n; uint16_t bit; int lx, ly, rx, ry; } names[] = {
        { "up", 0, 0, -127, 0, 0 }, { "down", 0, 0, 127, 0, 0 },
        { "left", 0, -127, 0, 0, 0 }, { "right", 0, 127, 0, 0, 0 },
        { "cam_l", 0, 0, 0, -127, 0 }, { "cam_r", 0, 0, 0, 127, 0 },
        { "cam_u", 0, 0, 0, 0, -127 }, { "cam_d", 0, 0, 0, 0, 127 },
        { "cross", PAD_CROSS }, { "circle", PAD_CIRCLE }, { "square", PAD_SQUARE },
        { "triangle", PAD_TRIANGLE }, { "l1", PAD_L1 }, { "r1", PAD_R1 }, { "l2", PAD_L2 },
        { "r2", PAD_R2 }, { "l3", PAD_L3 }, { "r3", PAD_R3 }, { "start", PAD_START },
        { "select", PAD_SELECT }, { "dup", PAD_UP }, { "ddown", PAD_DOWN },
        { "dleft", PAD_LEFT }, { "dright", PAD_RIGHT }, { "idle", 0 },
    };
    size_t i;
    for (i = 0; i < sizeof names / sizeof names[0]; i++) {
        if (strlen(names[i].n) != len || strncmp(names[i].n, n, len))
            continue;
        p->bits |= names[i].bit;
        if (names[i].lx) p->lx = names[i].lx;
        if (names[i].ly) p->ly = names[i].ly;
        if (names[i].rx) p->rx = names[i].rx;
        if (names[i].ry) p->ry = names[i].ry;
        return 1;
    }
    return 0;
}

/* Back / View + Start held together on the controller: the bug reporter's combo (pick.c) */
int pad_combo_held(void)
{
    return ctl && SDL_GameControllerGetAttached(ctl) && SDL_GameControllerGetButton(ctl, SDL_CONTROLLER_BUTTON_BACK)
        && SDL_GameControllerGetButton(ctl, SDL_CONTROLLER_BUTTON_START);
}

int pad_script_set(const char *s)
{
    nsteps = cur = left = 0;
    while (*s) {
        pad_state p;
        const char *e = s;
        memset(&p, 0, sizeof p);
        if (nsteps == capsteps) {
            capsteps = capsteps ? capsteps * 2 : 512;
            steps = (pad_state *)realloc(steps, (size_t)capsteps * sizeof *steps);
            ticks = (int *)realloc(ticks, (size_t)capsteps * sizeof *ticks);
            waits = (int *)realloc(waits, (size_t)capsteps * sizeof *waits);
        }
        waits[nsteps] = 0;
        if (*e == '@') {            /* "@villageN" / "@questN": idle until the N-th village entry / quest start of the run */
            static const char *const ev[PAD_EV_N] = { "village", "quest" };
            int k, n = 0, used = 0;
            for (k = 0; k < PAD_EV_N; k++)
                if (!strncmp(e + 1, ev[k], strlen(ev[k])))
                    break;
            if (k == PAD_EV_N || sscanf(e + 1 + strlen(ev[k]), "%d%n", &n, &used) != 1 || n <= 0)
                return 0;
            waits[nsteps] = (k + 1) << 16 | n;
            e += 1 + strlen(ev[k]) + used;
        } else
        if (*e == 'x') {            /* a recorded state: xBITS:lx:ly:rx:ry (hex bits), see rt_pick.c */
            unsigned b;
            int n = 0;
            if (sscanf(e + 1, "%x:%d:%d:%d:%d%n", &b, &p.lx, &p.ly, &p.rx, &p.ry, &n) != 5)
                return 0;
            p.bits = (uint16_t)b;
            e += 1 + n;
        } else
        for (;;) {
            size_t len = strcspn(e, "+*,");
            if (!name_to(e, len, &p))
                return 0;
            e += len;
            if (*e != '+')
                break;
            e++;
        }
        ticks[nsteps] = 1;
        if (*e == '*') {
            ticks[nsteps] = atoi(e + 1);
            e += 1 + strspn(e + 1, "0123456789");
        }
        steps[nsteps++] = p;
        if (*e == ',')
            e++;
        else if (*e)
            return 0;
        s = e;
    }
    left = nsteps ? ticks[0] : 0;
    return 1;
}

void pad_script_event(int ev)
{
    if (ev >= 0 && ev < PAD_EV_N)
        ev_count[ev]++;
}

static uint16_t script_bits;            /* the buttons of the step played last (pad_script_quit_held) */

/* a pad script holds select + start (Back + Start): the Xbox build's quit combo in scripted test runs, which have no
 * real controller pressing it (tools/xemu_relaunch_test.sh) */
int pad_script_quit_held(void)
{
    return (script_bits & (PAD_SELECT | PAD_START)) == (PAD_SELECT | PAD_START);
}

int pad_script_next(pad_state *p)
{
    while (cur < nsteps && (left <= 0 || waits[cur])) {
        if (waits[cur] && ev_count[(waits[cur] >> 16) - 1] < (waits[cur] & 0xFFFF)) {
            memset(p, 0, sizeof *p);        /* waiting for the event */
            script_bits = 0;
            return 1;
        }
        if (waits[cur]) {
            waits[cur] = 0;                 /* reached: go on with the next step */
            left = 0;
            continue;
        }
        cur++;
        left = cur < nsteps ? ticks[cur] : 0;
    }
    if (cur >= nsteps) {
        memset(p, 0, sizeof *p);
        script_bits = 0;
        return 0;
    }
    *p = steps[cur];
    script_bits = p->bits;
    left--;
    return 1;
}

/* flPADShockSet (the game's vib_set / vib_set_pl: vib_tbl rows {strength 1-7, frames}): the controller's rumble */
int flPADShockSet(int port, int strength, int frames)
{
    if (strength > 0)
        rt_log("rumble: port %d strength %d frames %d%s", port, strength, frames, headless ? " (headless: not sent)" :
               pad_rumble_off ? " (rumble off in the settings)" : ctl ? "" : " (no controller)");
    if (headless || port != 0 || !ctl)
        return 0;
    if (strength <= 0 || pad_rumble_off) {                /* vib_stop / vib_stop_all: strength 0 */
        SDL_GameControllerRumble(ctl, 0, 0, 0);
        return 0;
    }
    if (strength > 7)
        strength = 7;
    SDL_GameControllerRumble(ctl, (Uint16)(strength * 9362), (Uint16)(strength * 9362), (Uint32)(frames > 0 ? frames * 1000 / 30 : 100));
    return 0;
}
