/*
 * pad_sdl.c - SDL2 game controller + keyboard backend for pad.h, and the
 * scripted input used by offscreen tests. Xbox-style controllers map by
 * position: A cross, B circle, X square, Y triangle, LB L1, RB R1,
 * LT L2, RT R2, Back select, Start start, stick clicks L3/R3.
 */
#include "pad.h"

#include <SDL.h>
#include <stdlib.h>
#include <string.h>

static SDL_GameController *ctl;

void pad_init(void)
{
    int i;
    if (!SDL_WasInit(SDL_INIT_GAMECONTROLLER))
        SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);
    for (i = 0; i < SDL_NumJoysticks() && !ctl; i++)
        if (SDL_IsGameController(i))
            ctl = SDL_GameControllerOpen(i);
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
    if (keyboard) {
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
#define MAX_STEPS 256
static pad_state steps[MAX_STEPS];
static int ticks[MAX_STEPS], nsteps, cur, left;

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

int pad_script_set(const char *s)
{
    nsteps = cur = left = 0;
    while (*s && nsteps < MAX_STEPS) {
        pad_state p;
        const char *e = s;
        memset(&p, 0, sizeof p);
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

int pad_script_next(pad_state *p)
{
    while (cur < nsteps && left <= 0) {
        cur++;
        left = cur < nsteps ? ticks[cur] : 0;
    }
    if (cur >= nsteps) {
        memset(p, 0, sizeof *p);
        return 0;
    }
    *p = steps[cur];
    left--;
    return 1;
}
