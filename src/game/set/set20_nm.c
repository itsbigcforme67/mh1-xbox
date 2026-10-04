/* NONMATCHING: Set20_set (0x00626E70), not built. The quest-number
 * compare chain in the original leaves every branch delay slot empty
 * (nop), and our compiler fills them. Tried: if-chain, goto, separate
 * case bodies, volatile quest_w / game_w, a local copy, an int cast.
 * Everything else here matches and is built from set20.c, which starts
 * after Set20_set. */
/* set20 - game.bin 0x00626E70-0x00627834. A gate (stage 25). Whether it
 * starts open comes from the quest. When the right monster (kind 2) walks
 * into the gate area it drops shut with a bounce and a quake, then rises
 * again about three seconds after the monster's animation leaves 0x403. */
#include "set.h"
#include "game.h"
#include "em.h"
#include "prim.h"
#include "clay.h"
#include "fl.h"

typedef struct SET_MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x2F];
    CLAY *clay;         /* 0x30 */
} SET_MDLW;

typedef struct QUEST_DATA {
    u8 _pad00[0x1D];
    u8 no;              /* 0x1D quest number (picks the gate's start state) */
} QUEST_DATA;

typedef struct QUEST_W {
    u8 _pad00[8];
    s16 x08;            /* 0x08 non-zero while a quest is set up */
    u8 _pad0A[0x94 - 0x0A];
    QUEST_DATA *data;   /* 0x94 */
} QUEST_W;

extern QUEST_W quest_w;
extern SET_MDLW *set_mdlw;
extern f32 set20_open_pos[2][3];
extern f32 set20_close_pos[2][3];
extern f32 set20_st25_se_pos[2][3];
extern s16 set20_model_no[2];

void release_prim(s16);
void flvecCopy(f32 *, f32 *);
u8 Em_stg_ck(EMW *);
void Em_se_req2(EMW *, int, int, f32 *, int, int);
void Shell22_set2(f32 *, int, int, int);
void set_quake_sub(int, f32 *);

static void set20_move(SETW *sw);
static void set20_i(SETW *sw);
static void set20_m(SETW *sw);
static void set20_d(SETW *sw);
static void set20_e(SETW *sw);
static void set20_trans(PRIM *pr);

void Set20_set(int arg) {
    SETW *sw;

    if (quest_w.x08 != 0) {
        switch (quest_w.data->no) {
        case 0x66:
        case 0x67:
            game_w.gate_open = 0;
            break;
        case 0x68:
        case 0x69:
        case 0x6A:
        case 0xCF:
            game_w.gate_open = 1;
            break;
        default:
            return;
        }
    }
    sw = pull_set_work(0);
    if (sw != 0) {
        sw->type = 20;
        sw->x34 = 0;
        sw->arg = arg;
        sw->move = set20_move;
    }
}

static void set20_move(SETW *sw) {
    switch (sw->mode) {
    case 0:
        set20_i(sw);
        break;
    case 1:
        set20_m(sw);
        break;
    case 2:
        set20_d(sw);
        break;
    case 3:
        set20_e(sw);
        break;
    }
}

static void set20_i(SETW *sw) {
    s16 i;
    EMW *em;

    sw->mode++;
    sw->be_flag = 1;
    sw->work14 = 0;
    sw->timer = 0;
    sw->speed = 0.0f;
    if (game_w.gate_open != 0) {
        flvecCopy(sw->pos, set20_open_pos[sw->arg]);
    } else {
        flvecCopy(sw->pos, set20_close_pos[sw->arg]);
    }
    if (game_w.flag1B3 & 2) {
        for (i = 0, em = em_work; i < 20; i++, em++) {
            if (em->be_flag != 0 && em->x01 != 0 && Em_stg_ck(em) != 0 && em->kind == 2) {
                break;
            }
        }
        if (i == 20 || em->char0 != 0x403) {
            sw->mode2 = 4;
        } else {
            sw->mode2 = 2;
        }
    }
    sw->prim_no = get_prim();
    if (sw->prim_no != -1) {
        sw->prim = get_prim_ptr(sw->prim_no);
        sw->prim->owner = sw;
        sw->prim->trans = set20_trans;
    } else {
        push_set_work(sw);
    }
}

static void set20_m(SETW *sw) {
    EMW *em = em_work;
    s16 i;
    f32 v[3];

    switch (sw->arg) {
    case 1:
        if (game_w.gate_open != 0) {
            switch (sw->mode2) {
            case 0:
                for (i = 0; i < 20; i++, em++) {
                    if (em->be_flag != 0 && em->x01 != 0 && Em_stg_ck(em) != 0 && em->kind == 2) {
                        break;
                    }
                }
                if (i == 20) {
                    break;
                }
                if (!(game_w.flag1B3 & 2) && em->char0 != 0x403) {
                    if (em->x388 != 0) {
                        break;
                    }
                    if (!(em->pos[0] > 10900.0f && em->pos[0] < 13000.0f &&
                          em->pos[2] > 19700.0f && em->pos[2] < 20200.0f)) {
                        break;
                    }
                }
                Em_se_req2(em, 0x36, 0, set20_st25_se_pos[0], 10, 0);
                Em_se_req2(em, 0x37, 0, set20_st25_se_pos[1], 10, 0);
                sw->mode2++;
                sw->speed = -100.0f;
                v[0] = sw->pos[0];
                v[1] = em->x5AC;
                v[2] = sw->pos[2];
                Shell22_set2(v, 5, em->stg, 0);
                break;
            case 1:
                sw->pos[1] += sw->speed;
                sw->speed += -20.0f;
                if (sw->pos[1] <= set20_open_pos[sw->arg][1] - 1750.0f) {
                    sw->pos[1] = set20_open_pos[sw->arg][1] - 1750.0f;
                    sw->speed *= -0.2f;
                    sw->mode2++;
                    set_quake_sub(5, sw->pos);
                    sw->timer = 0;
                }
                break;
            case 2:
                if (sw->se0 < 3) {
                    sw->pos[1] += sw->speed;
                    sw->speed += -20.0f;
                    if (sw->pos[1] <= set20_open_pos[sw->arg][1] - 1750.0f) {
                        sw->pos[1] = set20_open_pos[sw->arg][1] - 1750.0f;
                        sw->speed *= -0.2f;
                        sw->se0++;
                    }
                }
                for (i = 0; i < 20; i++, em++) {
                    if (em->be_flag != 0 && em->x01 != 0 && Em_stg_ck(em) != 0 && em->kind == 2) {
                        break;
                    }
                }
                if (++sw->timer > 180 && (i == 20 || em->char0 != 0x403)) {
                    sw->mode2++;
                    sw->timer = 0;
                    for (i = 0; i < 20; i++, em++) {
                        if (em->be_flag != 0 && em->x01 != 0 && Em_stg_ck(em) != 0 && em->kind == 2) {
                            Em_se_req2(em, 0x38, 0, set20_st25_se_pos[0], 10, 0);
                            Em_se_req2(em, 0x39, 0, set20_st25_se_pos[1], 10, 0);
                            break;
                        }
                    }
                }
                break;
            case 3:
                sw->pos[1] += 29.166666f;
                if (sw->pos[1] > set20_open_pos[sw->arg][1]) {
                    sw->mode2++;
                    sw->pos[1] = set20_open_pos[sw->arg][1];
                    sw->timer = 0;
                }
                break;
            case 4:
                break;
            }
        }
        break;
    }
    sw->prim->pos[0] = sw->pos[0];
    sw->prim->pos[1] = sw->pos[1];
    sw->prim->pos[2] = sw->pos[2];
    add_prim(ot1, sw->prim, 0x20, 0);
}

static void set20_d(SETW *sw) {
    sw->mode++;
    sw->be_flag = 0;
    release_prim(sw->prim_no);
}

static void set20_e(SETW *sw) {
    push_set_work(sw);
}

static void set20_trans(PRIM *pr) {
    FLMAT mat;
    SET_MDLW *mw = set_mdlw;
    SETW *sw = pr->owner;
    CLAY *cl;

    if (mw != 0 && mw->flag != 0) {
        flmatMakeTrans(&mat, sw->pos[0], sw->pos[1], sw->pos[2]);
        flSetRenderState(0x1A, (u32)&mat);
        cl = &mw->clay[set20_model_no[sw->arg]];
        if (cl != 0 && cl->handle != -1) {
            clay_attr_set(cl->attr);
            flExecuteClay(cl->handle, 0);
        }
        clay_attr_reset();
    }
}
