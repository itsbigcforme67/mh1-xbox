/* set10 - game.bin 0x00621C70-0x0062217C. A periodic falling object: a
 * tremor (screen quake, rumble if a kind-7 monster is about), then it falls
 * 62.5 units per frame until the next random wait of 60-91 frames. */
#include "set.h"
#include "game.h"
#include "pl.h"
#include "em.h"
#include "prim.h"
#include "clay.h"
#include "fl.h"

typedef struct SET_MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x2F];
    CLAY *clay;         /* 0x30 */
} SET_MDLW;

extern SET_MDLW *set_mdlw;
extern f32 set10_st10_pos[3];
extern s8 Snd_em_id_conv_tbl[];
extern f32 rand_ofs_tbl[4][3];

void set_quake_sub2(int);
u16 ran_suu(int);
void se_req2(int, int, int, f32 *, int, int);

static void set10_move(SETW *sw);
static void set10_i(SETW *sw);
static void set10_m(SETW *sw);
static void set10_d(SETW *sw);
static void set10_e(SETW *sw);
static void set10_trans(PRIM *pr);

void Set10_set(void) {
    SETW *sw = pull_set_work(0);

    if (sw != 0) {
        sw->type = 10;
        sw->arg = 0;
        sw->move = set10_move;
    }
}

static void set10_move(SETW *sw) {
    switch (sw->mode) {
    case 0:
        set10_i(sw);
        break;
    case 1:
        set10_m(sw);
        break;
    case 2:
        set10_d(sw);
        break;
    case 3:
        set10_e(sw);
        break;
    }
}

static void set10_i(SETW *sw) {
    sw->mode++;
    sw->be_flag = 1;
    sw->work14 = 0;
    sw->cnt = 0;
    sw->pos[0] = set10_st10_pos[0];
    sw->pos[1] = set10_st10_pos[1];
    sw->pos[2] = set10_st10_pos[2];
    sw->timer = 0;
    sw->prim = get_prim_ptr(get_prim());
    if (sw->prim != 0) {
        sw->prim->owner = sw;
        sw->prim->trans = set10_trans;
    }
}

static void set10_m(SETW *sw) {
    EMW *em = em_work;
    PLW *pl = &player_work[game_w.master];
    f32 pos[3];
    s16 i;
    int k;

    switch (sw->mode2) {
    case 0:
        if (--sw->timer <= 0) {
            sw->mode2++;
            sw->timer = 16;
            set_quake_sub2(0);
            for (i = 0; i < 20; i++, em++) {
                if (em->be_flag != 0 && em->x01 != 0 && em->kind == 7) {
                    se_req2(6, 0x29, Snd_em_id_conv_tbl[em->kind], pl->pos, 4, 0);
                    return;
                }
            }
        }
        break;
    case 1:
        if (--sw->timer <= 0) {
            sw->mode2 = 0;
            sw->timer = (ran_suu(1) & 0x1F) + 60;
            sw->pos[1] = set10_st10_pos[1];
            return;
        }
        if (sw->timer == 13) {
            for (i = 0; i < 20; i++, em++) {
                if (em->be_flag != 0 && em->x01 != 0 && em->kind == 7) {
                    k = (s16)(ran_suu(1) & 3);
                    pos[0] = pl->pos[0] + rand_ofs_tbl[k][0];
                    pos[1] = pl->pos[1] + rand_ofs_tbl[k][1];
                    pos[2] = pl->pos[2] + rand_ofs_tbl[k][2];
                    se_req2(6, 0x2A, Snd_em_id_conv_tbl[em->kind], pos, 4, 0);
                    break;
                }
            }
        }
        sw->pos[1] -= 62.5f;
        sw->prim->pos[0] = sw->pos[0];
        sw->prim->pos[1] = sw->pos[1];
        sw->prim->pos[2] = sw->pos[2];
        add_prim(ot1, sw->prim, 0x20, 1);
        break;
    }
}

static void set10_d(SETW *sw) {
    sw->mode++;
    sw->be_flag = 0;
}

static void set10_e(SETW *sw) {
    push_set_work(sw);
}

static void set10_trans(PRIM *pr) {
    FLMAT mat;
    SET_MDLW *mw = set_mdlw;
    CLAY *cl;

    if (mw != 0 && mw->flag != 0) {
        cl = mw->clay;
        flSetRenderState(0x60, 0x80);
        flmatMakeTrans(&mat, pr->pos[0], pr->pos[1], pr->pos[2]);
        flSetRenderState(0x1A, (u32)&mat);
        if (cl->handle != -1) {
            clay_attr_set(cl->attr);
            flExecuteClay(cl->handle, 0);
        }
        clay_attr_reset();
    }
}
