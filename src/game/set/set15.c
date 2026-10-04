/* set15 - game.bin 0x00624050-0x0062511C. Three small swimming creatures
 * on a 500-unit grid around (5100, 5000). Each picks a move at random
 * (forward, turn, or head back to its home cell), and when a player comes
 * within 200 units in front of it, it leaps and dives, then reappears at a
 * new random cell. */
#include "set.h"
#include "game.h"
#include "pl.h"
#include "prim.h"
#include "clay.h"
#include "fl.h"

typedef struct SET_MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x2F];
    CLAY *clay;         /* 0x30 */
} SET_MDLW;

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))
#define HOME_X(sw) (5100.0f + 500.0f * (sw)->se1)
#define HOME_Z(sw) (5000.0f + 500.0f * (sw)->se0)

extern SET_MDLW *set_mdlw;

u32 ran_suu(int);
f32 flSqrt(f32);
f32 flAbs(f32);
f32 flArcTan2(f32, f32);
void flmatInit(FLMAT *);
void flmatRotX33(FLMAT *, f32);
void flvecApplyMat33_2(f32 *, FLMAT *);

static void set15_move(SETW *sw);
static void set15_i(SETW *sw);
static void set15_m(SETW *sw);
static void set15_move_select(SETW *sw);
static u8 set15_move_select2(SETW *sw);
static void set15_move_fb(SETW *sw, f32 spd);
static void set15_turn(SETW *sw, u8 dir);
static void set15_sub_init(SETW *sw);
static void set15_d(SETW *sw);
static void set15_e(SETW *sw);
static void set15_trans(PRIM *pr);

void set15_set(void) {
    SETW *sw;
    s16 i;

    for (i = 0; i < 3; i++) {
        if ((sw = pull_set_work(0)) != 0) {
            sw->type = 15;
            sw->arg = 0;
            sw->se0 = (u16)ran_suu(1) & 7;
            sw->se1 = (u16)ran_suu(1) & 3;
            sw->move = set15_move;
        }
    }
}

static void set15_move(SETW *sw) {
    switch (sw->mode) {
    case 0:
        set15_i(sw);
        break;
    case 1:
        set15_m(sw);
        break;
    case 2:
        set15_d(sw);
        break;
    case 3:
        set15_e(sw);
        break;
    }
}

static void set15_i(SETW *sw) {
    sw->mode++;
    sw->be_flag = 1;
    sw->work14 = 0;
    sw->cnt = 0;
    sw->pos[0] = HOME_X(sw);
    sw->pos[1] = 4.0f;
    sw->pos[2] = HOME_Z(sw);
    sw->timer = 0;
    sw->x1E = 0;
    sw->prim = get_prim_ptr(get_prim());
    if (sw->prim != 0) {
        sw->prim->owner = sw;
        sw->prim->trans = set15_trans;
    }
}

static void set15_m(SETW *sw) {
    PLW *pl = player_work;
    s16 i;
    f32 dx, dz, d;
    u16 a, diff;

    if (sw->arg != 4 && sw->arg != 5) {
        for (i = 0; i < 4; i++, pl++) {
            if (pl->be_flag != 0 && pl->x01 != 0) {
                dx = pl->pos[0] - sw->pos[0];
                dz = pl->pos[2] - sw->pos[2];
                d = flSqrt(dx * dx + dz * dz);
                if (d < 200.0f) {
                    if (dx != 0.0f || dz != 0.0f) {
                        dx /= d;
                        dz /= d;
                        a = (s32)(0.5f + 65536.0f * flArcTan2(dx, dz) / 6.2831855f);
                    } else {
                        a = 0;
                    }
                    diff = a - (u16)sw->cnt;
                    if (diff >= 0x9555 || diff < 0x6AAC) {
                        if (sw->mode2 == 0) {
                            sw->mode2++;
                        }
                        sw->timer = 6;
                        sw->speed = -50.0f;
                        sw->arg = 4;
                    }
                }
            }
        }
    }
    switch (sw->mode2) {
    case 0:
        if (flAbs(sw->pos[0] - HOME_X(sw)) < 300.0f && flAbs(sw->pos[2] - HOME_Z(sw)) < 300.0f) {
            set15_move_select(sw);
        } else {
            sw->arg = set15_move_select2(sw);
        }
        sw->mode2++;
    case 1:
        switch (sw->arg) {
        case 0:
            set15_move_fb(sw, 0.75f);
            break;
        case 1:
            set15_move_fb(sw, 0.75f);
            set15_turn(sw, 0);
            break;
        case 2:
            set15_move_fb(sw, 0.75f);
            set15_turn(sw, 1);
            break;
        case 3:
            break;
        case 4:
            set15_move_fb(sw, sw->speed);
            sw->speed += 2.0f;
            break;
        case 5:
            set15_move_fb(sw, sw->speed);
            sw->speed += 2.0f;
            break;
        }
        sw->timer--;
        if (sw->timer <= 0) {
            if (sw->arg == 4) {
                sw->pos[1] = 4.0f;
                sw->arg = 5;
                sw->timer = 15;
            } else if (sw->arg == 5) {
                if (sw->pos[0] > 7400.0f || sw->pos[0] < 4600.0f || sw->pos[2] > 9500.0f || sw->pos[2] < 4500.0f) {
                    sw->mode2++;
                    sw->timer = 0;
                } else {
                    sw->mode2--;
                }
            } else {
                sw->mode2--;
            }
        }
        break;
    case 2:
        sw->timer -= 0x222;
        set15_move_fb(sw, -60.0f);
        if (sw->pos[1] <= -80.0f) {
            sw->mode2++;
            sw->be_flag = 0;
            sw->timer = 300;
            sw->pos[0] = 0.0f;
            sw->pos[2] = 0.0f;
        }
        break;
    case 3:
        sw->timer--;
        if (sw->timer <= 0) {
            set15_sub_init(sw);
        }
        break;
    case 4:
        sw->timer += 0x222;
        sw->speed -= 1.0f;
        set15_move_fb(sw, sw->speed);
        if ((u16)sw->timer >= 0xFE00) {
            sw->pos[1] = 4.0f;
            sw->mode2 = 1;
            sw->timer = 0;
        }
        break;
    }
    sw->prim->pos[0] = sw->pos[0];
    sw->prim->pos[1] = sw->pos[1];
    sw->prim->pos[2] = sw->pos[2];
    add_prim(ot1, sw->prim, 0x20, 1);
}

static void set15_move_select(SETW *sw) {
    sw->timer = ((u16)ran_suu(1) & 0x3F) + 10;
    sw->arg = (u16)ran_suu(1) % 6;
    if (sw->arg > 3) {
        sw->arg = 3;
    }
}

static u8 set15_move_select2(SETW *sw) {
    u16 d;

    sw->timer = (u16)ran_suu(1) % 60 + 10;
    d = (u16)(s32)(0.5f + 65536.0f * flArcTan2(HOME_X(sw) - sw->pos[0], HOME_Z(sw) - sw->pos[2]) / 6.2831855f) - (u16)sw->cnt;
    if (d < 0x8000 && d > 0x2000) {
        return 2;
    }
    if (d >= 0x8000 && d < 0xE000) {
        return 1;
    }
    return (u16)ran_suu(1) & 3;
}

static void set15_move_fb(SETW *sw, f32 spd) {
    FLMAT mat;
    f32 v[3];

    v[0] = 0.0f;
    v[1] = 0.0f;
    v[2] = spd;
    flmatInit(&mat);
    if (sw->mode2 == 2 || sw->mode2 == 4) {
        flmatRotX33(&mat, DEG2RAD(ANG2DEG((u16)sw->timer)));
    } else if (sw->arg == 4) {
        v[1] = 15.0f * sw->timer / 3.0f - 15.0f;
    }
    flmatRotY33(&mat, DEG2RAD(ANG2DEG((u16)sw->cnt)));
    flvecApplyMat33_2(v, &mat);
    sw->pos[0] += v[0];
    sw->pos[1] += v[1];
    sw->pos[2] += v[2];
}

static void set15_turn(SETW *sw, u8 dir) {
    if (dir == 0) {
        sw->cnt -= 0xB6;
    } else {
        sw->cnt += 0xB6;
    }
}

static void set15_sub_init(SETW *sw) {
    sw->be_flag = 1;
    sw->work14 = 0;
    sw->cnt = 0;
    sw->se0 = (u16)ran_suu(1) & 7;
    sw->se1 = (u16)ran_suu(1) & 3;
    sw->pos[0] = HOME_X(sw);
    sw->pos[1] = -46.0f;
    sw->pos[2] = HOME_Z(sw);
    sw->timer = -0x2000;
    sw->speed = 15.0f;
    sw->mode2++;
}

static void set15_d(SETW *sw) {
    sw->mode++;
    sw->be_flag = 0;
}

static void set15_e(SETW *sw) {
    push_set_work(sw);
}

static void set15_trans(PRIM *pr) {
    FLMAT mat;
    SETW *sw = pr->owner;
    SET_MDLW *mw = set_mdlw;
    CLAY *cl;

    if (mw != 0 && mw->flag != 0) {
        cl = &mw->clay[1];
        flmatMakeScale(&mat, 0.85f, 0.85f, 0.85f);
        if (sw->mode2 == 2 || sw->mode2 == 4) {
            flmatRotX33(&mat, DEG2RAD(ANG2DEG((u16)sw->timer)));
        } else if (sw->arg == 4) {
            flmatRotX33(&mat, 0.3926991f * flSin(DEG2RAD((f32)(180 * (sw->timer + 15)) / 21.0f)));
        } else if (sw->arg == 5) {
            flmatRotX33(&mat, 0.3926991f * flSin(DEG2RAD((f32)(180 * sw->timer) / 21.0f)));
        }
        flmatRotY33(&mat, DEG2RAD(ANG2DEG((u16)sw->cnt)));
        flmatSetTrans(&mat, sw->pos[0], sw->pos[1], sw->pos[2]);
        flSetRenderState(0x1A, (u32)&mat);
        if (cl->handle != -1) {
            clay_attr_set(cl->attr);
            flExecuteClay(cl->handle, 0);
        }
        flSetRenderState(0x60, 0);
        clay_attr_reset();
    }
}
