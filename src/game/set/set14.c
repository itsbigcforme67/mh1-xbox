/* set14 - game.bin 0x006229B0-0x0062404C, one translation unit, every function matches
 * (set14_trans needed the order of the u/v assignments in the uv scroll cases swapped,
 * found by a hill-climb over adjacent statement swaps). A stage overlay with scrolling textures: on stages
 * 0 and 26 it follows the master player; on stages 51-53 two of five
 * masked layers fade in and out at random intervals (mode2/se0 pick the
 * layers, timer/cnt count up to se1/x1E). */
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

typedef struct STAGE_WORK {
    u8 _pad00[8];
    s16 timer;          /* 0x08 */
    u8 _pad0A[0x3C - 0x0A];
    SET_MDLW *mdl;      /* 0x3C stage model set */
    u8 _pad40[0x64 - 0x40];
} STAGE_WORK;

extern SET_MDLW *set_mdlw;
extern STAGE_WORK stage_work;
extern PLW player_work[];
extern u8 ot3[];
extern s16 set14_st00_mask_tbl[4];
extern s16 set14_st01_mask_tbl[4];
extern s16 set14_st03_mask_tbl[4];
extern s16 set14_st04_mask_tbl[4];
extern s16 set14_st42_mask_tbl[4];
extern s16 set14_st51_mask_tbl[4];
extern s16 set14_st52_mask_tbl[4];
extern s16 set14_st53_mask_tbl[4];
extern s16 st00_mdl_tbl[4];
extern s16 st01_mdl_tbl[4];
extern s16 st04_mdl_tbl[4];
extern f32 set14_st42_pos_tbl[4][3];
extern f32 set14_st51_pos_tbl[5][3];
extern f32 set14_st52_pos_tbl[5][3];
extern f32 set14_st53_pos_tbl[5][3];
extern f32 uv_pos00_00678370[16][2];

u32 ran_suu(int);
f32 flFloor(f32);
void SetFilterMode(int);
void flExecuteClay(s32, int);

static void set14_move(SETW *sw);
static void set14_i(SETW *sw);
static void set14_m(SETW *sw);
static void set14_d(SETW *sw);
static void set14_e(SETW *sw);
static void set14_trans(PRIM *pr);

void set14_set(void) {
    SETW *sw = pull_set_work(0);

    if (sw != 0) {
        sw->type = 0xE;
        sw->arg = 0;
        sw->move = set14_move;
    }
}

static void set14_move(SETW *sw) {
    switch (sw->mode) {
    case 0:
        set14_i(sw);
        break;
    case 1:
        set14_m(sw);
        break;
    case 2:
        set14_d(sw);
        break;
    case 3:
        set14_e(sw);
        break;
    }
}

static void set14_i(SETW *sw) {
    sw->mode++;
    sw->be_flag = 1;
    sw->cnt = 0;
    sw->timer = 0;
    switch (game_w.stage) {
    case 0:
    case 1:
    case 3:
    case 0x1A:
        sw->pos[0] = 0.0f;
        sw->pos[1] = 0.0f;
        sw->pos[2] = 0.0f;
        break;
    case 4:
        sw->pos[0] = 11060.0f;
        sw->pos[1] = 0.0f;
        sw->pos[2] = 1566.0f;
        break;
    case 0x2A:
        sw->pos[0] = 7227.0f;
        sw->pos[1] = -34.0f;
        sw->pos[2] = 5795.0f;
        break;
    case 0x33:
        sw->pos[0] = 9400.0f;
        sw->pos[1] = 18.0f;
        sw->pos[2] = 11400.0f;
        sw->mode2 = (u16)ran_suu(1) % 5;
        sw->timer = (u16)ran_suu(1) & 0xF;
        sw->se1 = (u16)ran_suu(1) % 10 + 0x37;
        sw->se0 = (u16)ran_suu(1) % 4 + 1;
        sw->se0 += sw->mode2;
        if (sw->se0 >= 5) {
            sw->se0 -= 5;
        }
        sw->cnt = (u16)ran_suu(1) & 0xF;
        sw->x1E = (u16)ran_suu(1) % 10 + 0x37;
        break;
    case 0x34:
        sw->pos[0] = 9800.0f;
        sw->pos[1] = 70.0f;
        sw->pos[2] = 12600.0f;
        sw->mode2 = (u16)ran_suu(1) % 5;
        sw->timer = (u16)ran_suu(1) & 0xF;
        sw->se1 = (u16)ran_suu(1) % 10 + 0x37;
        sw->se0 = (u16)ran_suu(1) % 4 + 1;
        sw->se0 += sw->mode2;
        if (sw->se0 >= 5) {
            sw->se0 -= 5;
        }
        sw->cnt = (u16)ran_suu(1) & 0xF;
        sw->x1E = (u16)ran_suu(1) % 10 + 0x37;
        break;
    case 0x35:
        sw->pos[0] = 8800.0f;
        sw->pos[1] = 13.0f;
        sw->pos[2] = 10400.0f;
        sw->mode2 = (u16)ran_suu(1) % 5;
        sw->timer = (u16)ran_suu(1) & 0xF;
        sw->se1 = (u16)ran_suu(1) % 10 + 0x37;
        sw->se0 = (u16)ran_suu(1) % 4 + 1;
        sw->se0 += sw->mode2;
        if (sw->se0 >= 5) {
            sw->se0 -= 5;
        }
        sw->cnt = (u16)ran_suu(1) & 0xF;
        sw->x1E = (u16)ran_suu(1) % 10 + 0x37;
        break;
    }
    sw->work14 = 0;
    sw->prim = get_prim_ptr(get_prim());
    if (sw->prim != 0) {
        sw->prim->owner = sw;
        sw->prim->trans = set14_trans;
    }
}

static void set14_m(SETW *sw) {
    PLW *pl = &player_work[game_w.master];

    switch (game_w.stage) {
    case 0:
    case 0x1A:
        sw->timer++;
        sw->pos[0] = pl->pos[0];
        sw->pos[1] = 0.0f;
        sw->pos[2] = pl->pos[2];
        break;
    case 0x33:
        if (++sw->timer > sw->se1) {
            sw->mode2 = (u16)ran_suu(1) % 4 + 1;
            sw->mode2 += sw->se0;
            if (sw->mode2 >= 5) {
                sw->mode2 -= 5;
            }
            sw->timer = 0;
            sw->se1 = (u16)ran_suu(1) % 10 + 0x37;
        }
        if (++sw->cnt > sw->x1E) {
            sw->se0 = (u16)ran_suu(1) % 4 + 1;
            sw->se0 += sw->mode2;
            if (sw->se0 >= 5) {
                sw->se0 -= 5;
            }
            sw->cnt = 0;
            sw->x1E = (u16)ran_suu(1) % 10 + 0x37;
        }
        break;
    case 0x34:
        if (++sw->timer > sw->se1) {
            sw->mode2 = (u16)ran_suu(1) % 4 + 1;
            sw->mode2 += sw->se0;
            if (sw->mode2 >= 5) {
                sw->mode2 -= 5;
            }
            sw->timer = 0;
            sw->se1 = (u16)ran_suu(1) % 10 + 0x37;
        }
        if (++sw->cnt > sw->x1E) {
            sw->se0 = (u16)ran_suu(1) % 4 + 1;
            sw->se0 += sw->mode2;
            if (sw->se0 >= 5) {
                sw->se0 -= 5;
            }
            sw->cnt = 0;
            sw->x1E = (u16)ran_suu(1) % 10 + 0x37;
        }
        break;
    case 0x35:
        if (++sw->timer > sw->se1) {
            sw->mode2 = (u16)ran_suu(1) % 4 + 1;
            sw->mode2 += sw->se0;
            if (sw->mode2 >= 5) {
                sw->mode2 -= 5;
            }
            sw->timer = 0;
            sw->se1 = (u16)ran_suu(1) % 10 + 0x37;
        }
        if (++sw->cnt > sw->x1E) {
            sw->se0 = (u16)ran_suu(1) % 4 + 1;
            sw->se0 += sw->mode2;
            if (sw->se0 >= 5) {
                sw->se0 -= 5;
            }
            sw->cnt = 0;
            sw->x1E = (u16)ran_suu(1) % 10 + 0x37;
        }
        break;
    default:
        sw->timer++;
        break;
    }
    if (sw->work14 == 0) {
        sw->prim->pos[0] = sw->pos[0];
        sw->prim->pos[1] = sw->pos[1];
        sw->prim->pos[2] = sw->pos[2];
        add_prim(ot3, sw->prim, 8, 1);
    }
}

static void set14_d(SETW *sw) {
    sw->mode++;
    sw->be_flag = 0;
}

static void set14_e(SETW *sw) {
    push_set_work(sw);
}

static void set14_trans(PRIM *pr) {
    FLMAT m;
    FLMAT uv;
    s16 *mask;
    SETW *sw = pr->owner;
    SET_MDLW *mw = set_mdlw;
    STAGE_WORK *stw = &stage_work;
    CLAY *cl;
    f32 u, v, w;
    s16 i;
    s16 t;
    s16 lim;
    u8 a;

    if (mw != 0 && mw->flag != 0) {
        cl = mw->clay;
        switch (game_w.stage) {
        case 0:
        case 0x1A:
            mask = set14_st00_mask_tbl;
            break;
        case 1:
            mask = set14_st01_mask_tbl;
            break;
        case 3:
            mask = set14_st03_mask_tbl;
            break;
        case 4:
            mask = set14_st04_mask_tbl;
            break;
        case 0x2A:
            flSetRenderState(0x6C, 0);
            mask = set14_st42_mask_tbl;
            break;
        case 0x33:
            flSetRenderState(0x6C, 0);
            mask = set14_st51_mask_tbl;
            break;
        case 0x34:
            flSetRenderState(0x6C, 0);
            mask = set14_st52_mask_tbl;
            break;
        case 0x35:
            flSetRenderState(0x6C, 0);
            mask = set14_st53_mask_tbl;
            break;
        }
        flSetRenderState(0x60, 0);
        for (i = 0; i < 4; i++) {
            if (*mask++ == 0) {
                continue;
            }
            switch (i) {
            case 0:
                if (game_w.stage == 1) {
                    u = 1.0f - 0.011111111f * (f32)stw->timer;
                    v = 0.0f;
                } else if (game_w.stage == 3) {
                    u = 1.0f - 0.0078125f * (f32)(stw->timer & 0x7F);
                    v = 0.0f;
                } else if (game_w.stage == 0x33 || game_w.stage == 0x34 || game_w.stage == 0x35) {
                    u = 0.0f;
                    v = u;
                } else {
                    u = 0.0f;
                    v = 1.0f - 0.011111111f * (f32)stw->timer;
                }
                break;
            case 1:
                if (game_w.stage == 0x33 || game_w.stage == 0x34 || game_w.stage == 0x35) {
                    u = 0.0f;
                    v = u;
                } else if (game_w.stage == 4) {
                    u = 0.0f;
                    v = 1.0f - 0.015625f * (f32)stw->timer;
                } else {
                    u = 0.0f;
                    v = 1.0f - 0.03125f * (f32)stw->timer;
                }
                break;
            case 2:
                u = uv_pos00_00678370[(sw->timer & 0x1E) >> 1][0];
                v = uv_pos00_00678370[(sw->timer & 0x1E) >> 1][1];
                break;
            case 3:
                v = 0.0f;
                u = 0.0033333334f * (f32)((u16)sw->timer % 300);
                break;
            }
            u -= flFloor(u);
            v -= flFloor(v);
            w = 0.0f;
            flmatMakeTrans(&uv, u, v, w);
            flSetRenderState(0x19, (u32)&uv);
            switch (game_w.stage) {
            case 0:
            case 0x1A:
                u = 0.0f;
                v = u;
                w = u;
                mw = stw->mdl;
                cl = &mw->clay[st00_mdl_tbl[i]];
                break;
            case 1:
            case 3:
                u = 0.0f;
                v = u;
                w = u;
                cl = &mw->clay[st01_mdl_tbl[i]];
                break;
            case 4:
                cl = &mw->clay[st04_mdl_tbl[i]];
                u = sw->pos[0];
                v = sw->pos[1];
                w = sw->pos[2];
                break;
            case 0x2A:
                u = set14_st42_pos_tbl[i][0];
                v = set14_st42_pos_tbl[i][1];
                w = set14_st42_pos_tbl[i][2];
                cl = mw->clay + i + 1;
                break;
            case 0x33:
                if (i == 0) {
                    lim = sw->se1;
                    t = sw->timer;
                    u = (f32)t * (1000.0f / (f32)lim) + set14_st51_pos_tbl[sw->mode2][0];
                    v = set14_st51_pos_tbl[sw->mode2][1];
                    w = set14_st51_pos_tbl[sw->mode2][2];
                } else {
                    lim = sw->x1E;
                    t = sw->cnt;
                    u = (f32)t * (1000.0f / (f32)lim) + set14_st51_pos_tbl[sw->se0][0];
                    v = set14_st51_pos_tbl[sw->se0][1];
                    w = set14_st51_pos_tbl[sw->se0][2];
                }
                if (t < 20) {
                    a = 255.0f * (0.05f * (f32)t);
                } else if (lim - 20 < t) {
                    a = 255.0f * ((f32)(lim - t) / 20.0f);
                } else {
                    a = 0xFF;
                }
                cl = &mw->clay[5];
                flSetRenderState(0x67, (a << 24) | 0xFFFFFF);
                break;
            case 0x34:
                if (i == 0) {
                    lim = sw->se1;
                    t = sw->timer;
                    u = (f32)t * (1000.0f / (f32)lim) + set14_st52_pos_tbl[sw->mode2][0];
                    v = set14_st52_pos_tbl[sw->mode2][1];
                    w = set14_st52_pos_tbl[sw->mode2][2];
                } else {
                    lim = sw->x1E;
                    t = sw->cnt;
                    u = (f32)t * (1000.0f / (f32)lim) + set14_st52_pos_tbl[sw->se0][0];
                    v = set14_st52_pos_tbl[sw->se0][1];
                    w = set14_st52_pos_tbl[sw->se0][2];
                }
                if (t < 20) {
                    a = 255.0f * (0.05f * (f32)t);
                } else if (lim - 20 < t) {
                    a = 255.0f * ((f32)(lim - t) / 20.0f);
                } else {
                    a = 0xFF;
                }
                cl = &mw->clay[3];
                flSetRenderState(0x67, (a << 24) | 0xFFFFFF);
                break;
            case 0x35:
                if (i == 0) {
                    lim = sw->se1;
                    t = sw->timer;
                    u = (f32)t * (1000.0f / (f32)lim) + set14_st53_pos_tbl[sw->mode2][0];
                    v = set14_st53_pos_tbl[sw->mode2][1];
                    w = set14_st53_pos_tbl[sw->mode2][2];
                } else {
                    lim = sw->x1E;
                    t = sw->cnt;
                    u = (f32)t * (1000.0f / (f32)lim) + set14_st53_pos_tbl[sw->se0][0];
                    v = set14_st53_pos_tbl[sw->se0][1];
                    w = set14_st53_pos_tbl[sw->se0][2];
                }
                if (t < 20) {
                    a = 255.0f * (0.05f * (f32)t);
                } else if (lim - 20 < t) {
                    a = 255.0f * ((f32)(lim - t) / 20.0f);
                } else {
                    a = 0xFF;
                }
                cl = mw->clay;
                flSetRenderState(0x67, (a << 24) | 0xFFFFFF);
                break;
            }
            flmatMakeTrans(&m, u, v, w);
            flSetRenderState(0x1A, (u32)&m);
            if (cl->handle != -1) {
                clay_attr_set(cl->attr);
                SetFilterMode(1);
                flExecuteClay(cl->handle, 0);
            }
        }
        clay_attr_reset();
        flSetRenderState(0x6C, 1);
    }
}
