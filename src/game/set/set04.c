/* set04 - game.bin 0x0061F2C0-0x0061F7D8. A flipbook-animated model with a
 * looping sound on stages 14 (models 2-17, swaying) and 28 (models 4-19,
 * hidden while game_w.flag1B3 bit 0 is set). */
#include "set.h"
#include "game.h"
#include "prim.h"
#include "clay.h"
#include "fl.h"

#define ANG2DEG(a)  (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d)  (2.0f * (3.1415927f * ((d) / 360.0f)))

typedef struct SET_MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x2F];
    CLAY *clay;         /* 0x30 */
} SET_MDLW;

extern SET_MDLW *set_mdlw;

void se_req2(int, int, int, f32 *, int, int);
void flvecCopy(void *, void *);

static void set04_move(SETW *sw);
static void set04_i(SETW *sw);
static void set04_m(SETW *sw);
static void set04_d(SETW *sw);
static void set04_e(SETW *sw);
static void set04_trans(PRIM *pr);

void Set04_set(void) {
    SETW *sw;

    if (game_w.stage == 0x1C && (game_w.flag1B3 & 1)) {
        return;
    }
    sw = pull_set_work(0);
    if (sw != 0) {
        sw->type = 4;
        sw->move = set04_move;
    }
}

static void set04_move(SETW *sw) {
    switch (sw->mode) {
    case 0:
        set04_i(sw);
        break;
    case 1:
        set04_m(sw);
        break;
    case 2:
        set04_d(sw);
        break;
    case 3:
        set04_e(sw);
        break;
    }
}

/* G: original bytes (build/mhg/raw/set04_i.inc, config/mhg/c_rawfuncs.txt); the C below is the MH1 version */
#if defined(VERSION_MHG) && defined(__MWERKS__)
static asm void set04_i(SETW *sw)
{
#include "set04_i.inc"
}
#elif defined(VERSION_MHG)
/* G set04_i (game 0x64FBB0), PC C from the asm (not match-checked: G's set04 is in config/mhg/c_skip.txt).
 * G adds stages 0x59-0x5B, sends any other stage straight back (push_set_work) and runs set04_m once at once. */
static void set04_i(SETW *sw) {
    sw->mode++;
    sw->be_flag = 1;
    sw->work14 = 0;
    sw->timer = 0;
    sw->cnt = 0;
    switch (game_w.stage) {
    case 0xE:
        sw->pos[0] = 12680.0f;
        sw->pos[1] = 696.0f;
        sw->pos[2] = 12756.0f;
        break;
    case 0x1C:
        sw->pos[0] = 10400.0f;
        sw->pos[1] = 1300.0f;
        sw->pos[2] = 20570.0f;
        break;
    case 0x59:
        sw->pos[0] = 10007.0f;
        sw->pos[1] = 200.0f;
        sw->pos[2] = 9377.0f;
        break;
    case 0x5A:
        sw->pos[0] = 5129.0f;
        sw->pos[1] = 1513.0f;
        sw->pos[2] = 10935.0f;
        break;
    case 0x5B:
        sw->pos[0] = 7980.0f;
        sw->pos[1] = 1101.0f;
        sw->pos[2] = 10006.0f;
        break;
    default:
        push_set_work(sw);
        return;
    }
    se_req2(7, 0x21, 0, sw->pos, 9, 0);
    sw->prim = get_prim_ptr(get_prim());
    if (sw->prim != 0) {
        sw->prim->owner = sw;
        sw->prim->trans = set04_trans;
        flvecCopy(sw->prim->pos, sw->pos);
        set04_m(sw);
        return;
    }
    push_set_work(sw);
}
#else
static void set04_i(SETW *sw) {
    sw->mode++;
    sw->be_flag = 1;
    sw->work14 = 0;
    sw->timer = 0;
    sw->cnt = 0;
    switch (game_w.stage) {
    case 0xE:
        sw->pos[0] = 12680.0f;
        sw->pos[1] = 696.0f;
        sw->pos[2] = 12756.0f;
        break;
    case 0x1C:
        sw->pos[0] = 10400.0f;
        sw->pos[1] = 1300.0f;
        sw->pos[2] = 20570.0f;
        break;
    }
    se_req2(7, 0x21, 0, sw->pos, 9, 0);
    sw->prim = get_prim_ptr(get_prim());
    if (sw->prim != 0) {
        sw->prim->owner = sw;
        sw->prim->trans = set04_trans;
        flvecCopy(sw->prim->pos, sw->pos);
        return;
    }
    push_set_work(sw);
}
#endif

/* G: original bytes (build/mhg/raw/set04_m.inc, config/mhg/c_rawfuncs.txt); the C below is the MH1 version */
#if defined(VERSION_MHG) && defined(__MWERKS__)
static asm void set04_m(SETW *sw)
{
#include "set04_m.inc"
}
#elif defined(VERSION_MHG)
/* G set04_m (game 0x64FD90), PC C from the asm: stages 0x59 / 0x5B flip models 1-16, 0x5A models 6-21, all three
 * sway like stage 14 (cnt); any other stage does nothing. */
static void set04_m(SETW *sw) {
    s16 first, last;

    switch (game_w.stage) {
    case 0xE:
        first = 2;
        last = 0x11;
        sw->cnt++;
        break;
    case 0x1C:
        first = 4;
        if (game_w.flag1B3 & 1) {
            return;
        }
        last = 0x13;
        break;
    case 0x59:
    case 0x5B:
        first = 1;
        last = 0x10;
        sw->cnt++;
        break;
    case 0x5A:
        first = 6;
        last = 0x15;
        sw->cnt++;
        break;
    default:
        return;
    }
    if (++sw->timer >= last - first + 1) {
        sw->timer = 0;
    }
    if ((sw->timer & 3) == 0) {
        se_req2(7, 0x21, 0, sw->pos, 9, 1);
    }
    sw->prim->pos[0] = sw->pos[0];
    sw->prim->pos[1] = sw->pos[1];
    sw->prim->pos[2] = sw->pos[2];
    add_prim(ot1, sw->prim, 0x20, 0);
}
#else
static void set04_m(SETW *sw) {
    s16 first, last;

    switch (game_w.stage) {
    case 0xE:
        first = 2;
        last = 0x11;
        sw->cnt++;
        break;
    case 0x1C:
        first = 4;
        if (game_w.flag1B3 & 1) {
            return;
        }
        last = 0x13;
        break;
    }
    if (++sw->timer >= last - first + 1) {
        sw->timer = 0;
    }
    if ((sw->timer & 3) == 0) {
        se_req2(7, 0x21, 0, sw->pos, 9, 1);
    }
    sw->prim->pos[0] = sw->pos[0];
    sw->prim->pos[1] = sw->pos[1];
    sw->prim->pos[2] = sw->pos[2];
    add_prim(ot1, sw->prim, 0x20, 0);
}
#endif

static void set04_d(SETW *sw) {
    sw->mode++;
    sw->be_flag = 0;
}

static void set04_e(SETW *sw) {
    push_set_work(sw);
}

/* G: original bytes (build/mhg/raw/set04_trans.inc, config/mhg/c_rawfuncs.txt); the C below is the MH1 version */
#if defined(VERSION_MHG) && defined(__MWERKS__)
static asm void set04_trans(PRIM *pr)
{
#include "set04_trans.inc"
}
#elif defined(VERSION_MHG)
/* G set04_trans (game 0x64FF60), PC C from the asm: stages 0x59-0x5B sway like stage 14 (first model 1 / 6 / 1);
 * any other stage draws nothing. */
static void set04_trans(PRIM *pr) {
    FLMAT mat;
    SET_MDLW *mw = set_mdlw;
    SETW *sw = pr->owner;
    CLAY *cl;
    s16 first;

    if (mw != 0 && mw->flag != 0) {
        flmatMakeTrans(&mat, pr->pos[0], pr->pos[1], pr->pos[2]);
        switch (game_w.stage) {
        case 0xE:
            flmatRotY33(&mat, DEG2RAD(30.0f * flSin(DEG2RAD(ANG2DEG(sw->cnt << 8)))));
            first = 2;
            break;
        case 0x1C:
            first = 4;
            break;
        case 0x59:
            flmatRotY33(&mat, DEG2RAD(30.0f * flSin(DEG2RAD(ANG2DEG(sw->cnt << 8)))));
            first = 1;
            break;
        case 0x5A:
            flmatRotY33(&mat, DEG2RAD(30.0f * flSin(DEG2RAD(ANG2DEG(sw->cnt << 8)))));
            first = 6;
            break;
        case 0x5B:
            flmatRotY33(&mat, DEG2RAD(30.0f * flSin(DEG2RAD(ANG2DEG(sw->cnt << 8)))));
            first = 1;
            break;
        default:
            return;
        }
        cl = &mw->clay[first] + sw->timer;
        flSetRenderState(0x1A, (u32)&mat);
        if (cl != 0 && cl->handle != -1) {
            clay_attr_set(cl->attr);
            flExecuteClay(cl->handle, 0);
        }
        clay_attr_reset();
    }
}
#else
static void set04_trans(PRIM *pr) {
    FLMAT mat;
    SET_MDLW *mw = set_mdlw;
    SETW *sw = pr->owner;
    CLAY *cl;
    s16 first;

    if (mw != 0 && mw->flag != 0) {
        flmatMakeTrans(&mat, pr->pos[0], pr->pos[1], pr->pos[2]);
        switch (game_w.stage) {
        case 0xE:
            flmatRotY33(&mat, DEG2RAD(30.0f * flSin(DEG2RAD(ANG2DEG(sw->cnt << 8)))));
            first = 2;
            break;
        case 0x1C:
            first = 4;
            break;
        }
        cl = &mw->clay[first] + sw->timer;
        flSetRenderState(0x1A, (u32)&mat);
        if (cl != 0 && cl->handle != -1) {
            clay_attr_set(cl->attr);
            flExecuteClay(cl->handle, 0);
        }
        clay_attr_reset();
    }
}
#endif
