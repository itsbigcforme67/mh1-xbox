/* set09 - game.bin 0x00618CA0-: set09_set to set09_t11.
 * A stage effect manager: arg 0 is the controller, whose work holds up to
 * 32 child objects (args 1-11) and a count per kind; each kind has its own
 * init/move/draw functions (set09_iNN/mNN/tNN). */
#include "set.h"
#include "game.h"
#include "prim.h"
#include "clay.h"
#include "fl.h"
#include "pl.h"

/* Controller work (arg 0). */
typedef struct SET09W {
    SETW *slot[32];     /* 0x00 children, 0 = free */
    s16 cnt[12];        /* 0x80 children per kind; cnt[0] = total */
} SET09W;

void flvecCopy(void *, void *);
void release_prim(s16);
u16 ran_suu(int);
void flmatInit(FLMAT *);
void RotateX(FLMAT *, f32);
void RotateY(FLMAT *, f32);
void RotateZ(FLMAT *, f32);
void flvecApplyMat33(f32 *, f32 *, FLMAT *);
void flmatMakeTrans(FLMAT *, f32, f32, f32);
void flmatSetTrans(FLMAT *, f32, f32, f32);
void set09_t_sub(CLAY *cl, FLMAT *m);

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

typedef struct SET_MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x2F];
    CLAY *clay;         /* 0x30 */
} SET_MDLW;

extern SET_MDLW *set_mdlw;
extern s16 *stg_eft_mdl_no[];
extern PLW player_work[];
extern f32 set09_st08_beetle_tbl[][3];
f32 flArcTan2(f32, f32);
void flmatRotXYZ33(FLMAT *, f32, f32, f32);
void flmatRotY33(FLMAT *, f32);

/* Work of kind 1 (a tumbling falling object). */
typedef struct SET09W1 {
    FLMAT mat;          /* 0x00 */
    f32 vel[3];         /* 0x40 */
    u16 rot[3];         /* 0x4C */
    s16 spin;           /* 0x52 */
    u8 _pad54[2];
    s16 roll;           /* 0x56 */
} SET09W1;

SETW *set09_set_sub_sub(s16 arg);
s16 set09_pull(SETW *sw);
void set09_move(SETW *sw);
void set09_i(SETW *sw);
void set09_m(SETW *sw);
void set09_d(SETW *sw);
void set09_e(SETW *sw);
void set09_trans(PRIM *pr);
void set09_i00(SETW *sw);
void set09_i01(SETW *sw);
void set09_i02(SETW *sw);
void set09_i03(SETW *sw);
void set09_i04(SETW *sw);
void set09_i05(SETW *sw);
void set09_i06(SETW *sw);
void set09_i07(SETW *sw);
void set09_i08(SETW *sw);
void set09_i09(SETW *sw);
void set09_i10(SETW *sw);
void set09_i11(SETW *sw);
void set09_m00(SETW *sw);
void set09_m01(SETW *sw);
void set09_m02(SETW *sw);
void set09_m03(SETW *sw);
void set09_m04(SETW *sw);
void set09_m05(SETW *sw);
void set09_m06(SETW *sw);
void set09_m07(SETW *sw);
void set09_m08(SETW *sw);
void set09_m09(SETW *sw);
void set09_m10(SETW *sw);
void set09_m11(SETW *sw);
void set09_t01(PRIM *pr);
void set09_t02(PRIM *pr);
void set09_t03(PRIM *pr);
void set09_t04(PRIM *pr);
void set09_t05(PRIM *pr);
void set09_t06(PRIM *pr);
void set09_t07(PRIM *pr);
void set09_t08(PRIM *pr);
void set09_t09(PRIM *pr);
void set09_t10(PRIM *pr);
void set09_t11(PRIM *pr);

void set09_set(void) {
    SETW *sw = pull_set_work(1);

    if (sw != 0) {
        sw->type = 9;
        sw->arg = 0;
        sw->x1E = 0;
        sw->move = set09_move;
    }
}

SETW *set09_set_sub_sub(s16 arg) {
    switch (arg) {
    case 8:
    case 10:
    case 11:
        return pull_set_work(0);
    default:
        return pull_set_work(1);
    }
}

void set09_set_sub(SETW *sw, s16 arg) {
    SET09W *w = sw->u.work;
    s16 n;
    SETW *c;

    n = set09_pull(sw);
    if (n != -1) {
        if ((c = set09_set_sub_sub(arg)) != 0) {
            c->type = 9;
            c->arg = arg;
            c->move = set09_move;
            c->x1E = 0;
            w->slot[n] = c;
            w->cnt[arg]++;
            w->cnt[0]++;
        }
    }
}

void set09_set_sub3(SETW *sw, s16 arg, s16 cnt) {
    SET09W *w = sw->u.work;
    s16 n;
    SETW *c;

    n = set09_pull(sw);
    if (n != -1) {
        if ((c = set09_set_sub_sub(arg)) != 0) {
            c->type = 9;
            c->arg = arg;
            c->move = set09_move;
            c->cnt = cnt;
            c->x1E = 0;
            w->slot[n] = c;
            w->cnt[arg]++;
            w->cnt[0]++;
        }
    }
}

s16 set09_pull(SETW *sw) {
    SETW **p = ((SET09W *)sw->u.work)->slot;
    s16 i;

    for (i = 0; i < 32; i++, p++) {
        if (*p == 0) {
            return i;
        }
    }
    return -1;
}

void Set09_set_ex(f32 *pos, s16 arg) {
    SETW *c;

    c = set09_set_sub_sub(arg);
    if (c != 0) {
        c->type = 9;
        c->arg = arg;
        c->move = set09_move;
        flvecCopy(c->pos, pos);
        c->x1E = 1;
    }
}

void set09_move(SETW *sw) {
    switch (sw->mode) {
    case 0:
        set09_i(sw);
        break;
    case 1:
        set09_m(sw);
        break;
    case 2:
        set09_d(sw);
        break;
    case 3:
        set09_e(sw);
        break;
    }
}

void set09_i(SETW *sw) {
    sw->mode++;
    sw->be_flag = 1;
    sw->work14 = 0;
    switch (sw->arg) {
    case 0:
        set09_i00(sw);
        break;
    case 1:
        set09_i01(sw);
        break;
    case 2:
        set09_i02(sw);
        break;
    case 3:
        set09_i03(sw);
        break;
    case 4:
        set09_i04(sw);
        break;
    case 5:
        set09_i05(sw);
        break;
    case 6:
        set09_i06(sw);
        break;
    case 7:
        set09_i07(sw);
        break;
    case 8:
        set09_i08(sw);
        break;
    case 9:
        set09_i09(sw);
        break;
    case 10:
        set09_i10(sw);
        break;
    case 11:
        set09_i11(sw);
        break;
    }
    if (sw->arg == 0 || sw->arg == 5) {
        return;
    }
    sw->prim_no = get_prim();
    if (sw->prim_no != -1) {
        sw->prim = get_prim_ptr(sw->prim_no);
        if (sw->prim != 0) {
            sw->prim->owner = sw;
            sw->prim->trans = set09_trans;
        }
    }
}

void set09_m(SETW *sw) {
    switch (sw->arg) {
    case 0:
        set09_m00(sw);
        break;
    case 1:
        set09_m01(sw);
        break;
    case 2:
        set09_m02(sw);
        break;
    case 3:
        set09_m03(sw);
        break;
    case 4:
        set09_m04(sw);
        break;
    case 5:
        set09_m05(sw);
        break;
    case 6:
        set09_m06(sw);
        break;
    case 7:
        set09_m07(sw);
        break;
    case 8:
        set09_m08(sw);
        break;
    case 9:
        set09_m09(sw);
        break;
    case 10:
        set09_m10(sw);
        break;
    case 11:
        set09_m11(sw);
        break;
    }
}

/* Work of kind 5: two extra draw primitives. */
typedef struct SET09W5 {
    s16 prim_no[2];     /* 0x00 */
    u8 _pad04[0x14 - 0x04];
    PRIM *prim[2];      /* 0x14 */
} SET09W5;

void set09_d(SETW *sw) {
    SET09W5 *w = sw->u.work;
    s16 i;

    sw->mode++;
    sw->be_flag = 0;
    if (sw->arg != 5) {
        if (sw->prim != 0) {
            release_prim(sw->prim_no);
        }
    } else {
        for (i = 0; i < 2; i++) {
            if (w->prim[i] != 0) {
                release_prim(w->prim_no[i]);
            }
        }
    }
}

void set09_e(SETW *sw) {
    push_set_work(sw);
}

void set09_trans(PRIM *pr) {
    switch (((SETW *)pr->owner)->arg) {
    case 1:
        set09_t01(pr);
        break;
    case 2:
        set09_t02(pr);
        break;
    case 3:
        set09_t03(pr);
        break;
    case 4:
        set09_t04(pr);
        break;
    case 5:
        set09_t05(pr);
        break;
    case 6:
        set09_t06(pr);
        break;
    case 7:
        set09_t07(pr);
        break;
    case 8:
        set09_t08(pr);
        break;
    case 9:
        set09_t09(pr);
        break;
    case 10:
        set09_t10(pr);
        break;
    case 11:
        set09_t11(pr);
        break;
    }
}

void set09_t_sub(CLAY *cl, FLMAT *m) {
    flSetRenderState(0x1A, (u32)m);
    if (cl != 0 && cl->handle != -1) {
        clay_attr_set(cl->attr);
        flExecuteClay(cl->handle, 0);
    }
    clay_attr_reset();
}

void set09_set_sub(SETW *sw, s16 arg);

void set09_i00(SETW *sw) {
    SET09W *w = sw->u.work;
    s16 i;

    sw->timer = 0;
    for (i = 0; i < 32; i++) {
        w->slot[i] = 0;
    }
    w->cnt[0] = 0;
    for (i = 0; i < 11; i++) {
        w->cnt[i + 1] = 0;
    }
    switch (game_w.stage) {
    case 8:
        return;
    case 15:
        for (i = 0; i < 2; i++) {
            if (w->cnt[0] < 32 && w->cnt[7] < 2) {
                set09_set_sub(sw, 7);
            }
        }
        break;
    case 16:
        if (w->cnt[0] < 32 && w->cnt[6] <= 0) {
            set09_set_sub(sw, 6);
        }
        break;
    case 20:
    case 32:
    case 51:
    case 52:
    case 53:
    case 55:
        if (w->cnt[0] < 32) {
            for (i = 0; i < 4; i++) {
                if (w->cnt[7] < 4) {
                    set09_set_sub(sw, 7);
                }
            }
        }
        break;
    case 41:
        if (w->cnt[0] < 32 && w->cnt[6] <= 0) {
            set09_set_sub(sw, 6);
        }
        break;
    }
}

void set09_set_sub3(SETW *sw, s16 arg, s16 cnt);

void set09_m00(SETW *sw) {
    SET09W *w = sw->u.work;
    s16 i;
    int n;

    sw->timer++;
    switch (game_w.stage) {
    case 5:
        if (sw->timer >= 60 && w->cnt[0] < 32) {
            set09_set_sub(sw, 1);
            sw->timer = 0;
        }
        if (w->cnt[0] < 32 && w->cnt[5] < 2) {
            set09_set_sub(sw, 5);
        }
        if (w->cnt[0] < 32 && w->cnt[9] < 5) {
            set09_set_sub(sw, 9);
        }
        if (w->cnt[0] < 32 && w->cnt[10] <= 0) {
            set09_set_sub(sw, 10);
        }
        break;
    case 8:
        if (sw->timer >= 60 && w->cnt[0] < 32) {
            set09_set_sub(sw, 1);
            sw->timer = 0;
        }
        if (w->cnt[0] < 32 && w->cnt[2] <= 0) {
            set09_set_sub(sw, 2);
        }
        if (w->cnt[0] < 32 && w->cnt[3] < 3) {
            set09_set_sub(sw, 3);
        }
        if (w->cnt[0] < 32 && w->cnt[4] < 5) {
            set09_set_sub(sw, 4);
        }
        if (w->cnt[0] < 32 && w->cnt[5] < 3) {
            set09_set_sub(sw, 5);
        }
        break;
    case 16:
        if (w->cnt[0] < 32 && w->cnt[2] < 2) {
            set09_set_sub(sw, 2);
        }
        if (w->cnt[0] < 32 && w->cnt[3] < 3) {
            set09_set_sub(sw, 3);
        }
        if (w->cnt[0] < 32 && w->cnt[5] < 2) {
            set09_set_sub(sw, 5);
        }
        break;
    case 27:
    case 33:
        if ((sw->timer & 0xFF) == 0 && w->cnt[0] < 32 && w->cnt[11] < 2) {
            n = (s16)(ran_suu(1) & 1) + 1;
            for (i = 0; i < n; i++) {
                set09_set_sub3(sw, 11, 2);
                if (w->cnt[0] >= 32) {
                    break;
                }
            }
        }
        break;
    case 36:
        if (w->cnt[0] < 32 && w->cnt[8] < 2) {
            set09_set_sub(sw, 8);
        }
        break;
    case 40:
        if (w->cnt[0] < 32 && w->cnt[4] < 2) {
            set09_set_sub(sw, 4);
        }
        break;
    case 41:
        if (sw->timer >= 60 && w->cnt[0] < 32) {
            set09_set_sub(sw, 1);
            sw->timer = 0;
        }
        if (w->cnt[0] < 32 && w->cnt[3] < 3) {
            set09_set_sub(sw, 3);
        }
        if (w->cnt[0] < 32 && w->cnt[4] < 5) {
            set09_set_sub(sw, 4);
        }
        if (w->cnt[0] < 32 && w->cnt[5] < 2) {
            set09_set_sub(sw, 5);
        }
        break;
    }
    for (i = 0; i < 32; i++) {
        if (w->slot[i] != 0 && w->slot[i]->mode >= 2) {
            w->cnt[w->slot[i]->arg]--;
            w->slot[i] = 0;
            w->cnt[0]--;
        }
    }
}

void set09_i01(SETW *sw) {
    SET09W1 *w = sw->u.work;
    PLW *pl = &player_work[game_w.master];

    sw->mode2 = ran_suu(1) & 3;
    switch (game_w.stage) {
    case 5:
        sw->pos[0] = 9650.0f + (ran_suu(1) & 0x3FF);
        sw->pos[1] = 1500.0f;
        sw->pos[2] = 5500.0f + (ran_suu(1) & 0x1FFF);
        break;
    case 8:
        sw->pos[0] = pl->pos[0];
        sw->pos[1] = 800.0f + ran_suu(1) % 100;
        sw->pos[2] = pl->pos[2];
        break;
    case 41:
        sw->pos[0] = 9773.0f + (ran_suu(1) & 0x7FF);
        sw->pos[1] = 3000.0f;
        sw->pos[2] = 9173.0f + (ran_suu(1) & 0x3FF);
        break;
    }
    sw->timer = 0;
    w->vel[0] = 0.0f;
    w->vel[1] = 0.0f;
    w->vel[2] = 0.0f;
    w->rot[0] = ran_suu(1);
    w->rot[1] = ran_suu(1);
    w->rot[2] = ran_suu(1);
    w->spin = 0;
    w->roll = 0;
}

void set09_m01(SETW *sw) {
    SET09W1 *w = sw->u.work;
    f32 v[3];

    w->spin = 0.95f * w->spin;
    w->roll = 0.975f * w->roll;
    w->spin += (s16)((ran_suu(1) & 0xFF) - 0x80);
    w->roll += (s16)((ran_suu(1) & 0x3F) - 0x20);
    w->rot[0] += w->spin;
    w->rot[1] += (u16)((ran_suu(1) & 0x1F) - 15);
    w->rot[2] += w->roll;
    flmatInit(&w->mat);
    RotateX(&w->mat, DEG2RAD(ANG2DEG(w->rot[0])));
    RotateY(&w->mat, DEG2RAD(ANG2DEG(w->rot[1])));
    RotateZ(&w->mat, DEG2RAD(ANG2DEG(w->rot[2])));
    switch (game_w.stage) {
    case 5:
        w->vel[0] += -4.0f * w->mat[0][1];
        w->vel[1] += -4.0f * w->mat[1][1];
        w->vel[2] += -4.0f * w->mat[2][1];
        break;
    default:
        w->vel[0] += -2.0f * w->mat[0][1];
        w->vel[1] += -2.0f * w->mat[1][1];
        w->vel[2] += -2.0f * w->mat[2][1];
        break;
    }
    w->vel[0] *= 0.6f;
    w->vel[1] *= 0.25f;
    w->vel[2] *= 0.7f;
    flvecApplyMat33(v, w->vel, &w->mat);
    sw->pos[0] += v[0];
    sw->pos[1] += v[1];
    sw->pos[2] += v[2];
    if (0.0f > sw->pos[1]) {
        sw->mode++;
    } else {
        sw->prim->pos[0] = sw->pos[0];
        sw->prim->pos[1] = sw->pos[1];
        sw->prim->pos[2] = sw->pos[2];
        add_prim(ot1, sw->prim, 0x20, 0);
    }
}

void set09_t01(PRIM *pr) {
    FLMAT m;
    SETW *sw = pr->owner;
    FLMAT *wm = sw->u.work;
    SET_MDLW *mw = set_mdlw;
    CLAY *cl;
    int n;

    if (mw != 0 && mw->flag != 0) {
        if ((n = stg_eft_mdl_no[game_w.stage][2]) >= 0) {
            cl = &mw->clay[n];
            if (game_w.stage != 27) {
                flmatMakeTrans(&m, 0.25f * sw->mode2, 0.0f, 0.0f);
                flSetRenderState(0x19, (u32)&m);
            }
            flmatSetTrans(wm, pr->pos[0], pr->pos[1], pr->pos[2]);
            set09_t_sub(cl, wm);
        }
    }
}

void set09_i02(SETW *sw) {
    SET09W1 *w = sw->u.work;
    u8 k;
    f32 dx;
    f32 dz;

    sw->mode2 = ran_suu(1) % 4;
    k = ran_suu(1) % 3;
    if (k >= sw->mode2) {
        k++;
    }
    sw->se0 = 0;
    sw->se1 = 0;
    sw->timer = 0;
    switch (game_w.stage) {
    case 8:
        sw->pos[0] = set09_st08_beetle_tbl[sw->mode2][0];
        sw->pos[1] = set09_st08_beetle_tbl[sw->mode2][1];
        sw->pos[2] = set09_st08_beetle_tbl[sw->mode2][2];
        dx = set09_st08_beetle_tbl[k][0] - sw->pos[0];
        dz = set09_st08_beetle_tbl[k][2] - sw->pos[2];
        w->rot[0] = (ran_suu(1) & 0xFF) - 0xEF;
        w->rot[1] = (u16)(s32)(0.5f + 65536.0f * flArcTan2(dx, dz) / 6.2831855f);
        w->rot[2] = 0;
        break;
    }
    w->vel[0] = 0.0f;
    w->vel[1] = 0.0f;
    w->vel[2] = 6.0f + 0.002f * (ran_suu(1) & 0x3FF);
    w->spin = 0;
    w->roll = 0;
}

void set09_m02(SETW *sw) {
    SET09W1 *w = sw->u.work;
    f32 v[3];

    sw->timer++;
    if (sw->timer >= 500) {
        sw->mode++;
        return;
    }
    switch (sw->se0) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
        sw->se0 = ran_suu(1) & 7;
        if (sw->se0 > 5) {
            sw->se1 = (ran_suu(1) & 0xF) + 10;
        }
        break;
    case 6:
        sw->se1--;
        w->rot[1] -= 0x20;
        if (sw->se1 <= 0) {
            sw->se0 = 0;
        }
        break;
    case 7:
        sw->se1--;
        w->rot[1] += 0x20;
        if (sw->se1 <= 0) {
            sw->se0 = 0;
        }
        break;
    }
    switch (w->spin) {
    case 0:
    case 1:
        if (--w->roll < 0) {
            w->spin = ran_suu(1) & 3;
            if (w->spin > 1) {
                w->roll = (ran_suu(1) & 7) + 3;
            } else {
                w->roll = 0;
            }
        }
        break;
    case 2:
        if (w->rot[0] > -0x1000) {
            w->rot[0] -= 0x60;
        }
        w->spin = 0;
        break;
    case 3:
        if (w->rot[0] < 0) {
            w->rot[1] += 0x600;
        }
        w->spin = 0;
        break;
    }
    flmatInit(&w->mat);
    flmatRotXYZ33(&w->mat, DEG2RAD(ANG2DEG(w->rot[0])), DEG2RAD(ANG2DEG(w->rot[1])), DEG2RAD(ANG2DEG(w->rot[2])));
    flvecApplyMat33(v, w->vel, &w->mat);
    sw->pos[0] += v[0];
    sw->pos[1] += v[1];
    sw->pos[2] += v[2];
    sw->prim->pos[0] = sw->pos[0];
    sw->prim->pos[1] = sw->pos[1];
    sw->prim->pos[2] = sw->pos[2];
    add_prim(ot1, sw->prim, 0x20, 0);
}

void set09_t02(PRIM *pr) {
    SETW *sw = pr->owner;
    FLMAT *wm = sw->u.work;
    SET_MDLW *mw = set_mdlw;
    CLAY *cl;
    int n;

    if (mw != 0 && mw->flag != 0) {
        if ((n = stg_eft_mdl_no[game_w.stage][3]) >= 0) {
            cl = &mw->clay[n];
            flSetRenderState(0x60, 0);
            flmatSetTrans(wm, pr->pos[0], pr->pos[1], pr->pos[2]);
            set09_t_sub(cl, wm);
            flSetRenderState(0x60, 0x80);
        }
    }
}

void set09_i03(SETW *sw) {
    SET09W1 *w = sw->u.work;

    sw->mode2 = 0;
    sw->se0 = 0;
    sw->se1 = 0;
    sw->timer = 0;
    switch (game_w.stage) {
    case 8:
        sw->pos[0] = (ran_suu(1) & 0x7FF) + 0x6A4;
        sw->pos[1] = (ran_suu(1) & 0xFF) + 300;
        sw->pos[2] = (ran_suu(1) & 0xFFF) + 2000;
        break;
    case 16:
        sw->pos[0] = 9625.0f + (ran_suu(1) & 0x7FF);
        sw->pos[1] = 400.0f;
        sw->pos[2] = 7926.0f + (ran_suu(1) & 0x3FF);
        break;
    case 41:
        sw->pos[0] = 9773.0f + (ran_suu(1) & 0x7FF);
        sw->pos[1] = 300.0f;
        sw->pos[2] = 9173.0f + (ran_suu(1) & 0x3FF);
        break;
    }
    w->rot[1] = ran_suu(1);
    w->vel[0] = 0.0f;
    w->vel[1] = 0.0f;
    w->vel[2] = 0.5f;
    w->spin = ran_suu(1) & 0x1F;
    w->roll = 0;
}

void set09_m03(SETW *sw) {
    SET09W1 *w = sw->u.work;
    f32 v[3];

    switch (game_w.stage) {
    case 8:
        if (sw->pos[0] < 1500.0f || sw->pos[0] > 4000.0f || sw->pos[1] > 2000.0f
            || sw->pos[2] < 3000.0f || sw->pos[2] > 7000.0f) {
            sw->mode++;
            return;
        }
        break;
    case 41:
        if (sw->pos[0] < 9773.0f || sw->pos[0] > 11174.0f || sw->pos[1] > 2000.0f
            || sw->pos[2] < 9173.0f || sw->pos[2] > 10576.0f) {
            sw->mode++;
            return;
        }
        break;
    }
    switch (sw->mode2) {
    case 0:
        if (--w->spin <= 0) {
            sw->mode2++;
            w->spin = ran_suu(1) % 6 + 2;
        }
        break;
    case 1:
        if (w->vel[2] < 10.0f) {
            if ((s16)(ran_suu(1) & 1) == 0) {
                w->vel[2] += 0.5f;
            } else {
                w->vel[2] += 0.3f;
            }
        }
        if (--w->spin <= 0) {
            sw->mode2++;
            w->spin = ran_suu(1) % 8 + 3;
        }
        break;
    case 2:
        if (--w->spin <= 0) {
            sw->mode2++;
            w->spin = (ran_suu(1) & 4) + 2;
        }
        break;
    case 3:
        if ((s16)(ran_suu(1) & 1) == 0) {
            w->vel[2] *= 0.5f;
        } else {
            w->vel[2] *= 0.4f;
        }
        if (--w->spin <= 0) {
            sw->mode2 = 0;
            w->spin = ran_suu(1) % 20 + 7;
        }
        break;
    }
    switch (sw->se0) {
    case 0:
        sw->timer = ran_suu(1) % 20 + 5;
        sw->se0 = ran_suu(1) % 3 + 1;
        break;
    case 1:
        if (--sw->timer <= 0) {
            sw->se0 = 0;
        }
        break;
    case 2:
        if (--sw->timer <= 0) {
            sw->timer = ran_suu(1) % 10 + 5;
            sw->se0 = 1;
        }
        w->rot[1] -= 0x180;
        break;
    case 3:
        if (--sw->timer <= 0) {
            sw->timer = ran_suu(1) % 10 + 5;
            sw->se0 = 1;
        }
        w->rot[1] += 0x180;
        break;
    case 4:
        if (--sw->timer <= 0) {
            sw->timer = ran_suu(1) % 10 + 5;
            sw->se0 = 1;
        }
        w->rot[1] -= 0x200;
        break;
    case 5:
        if (--sw->timer <= 0) {
            sw->timer = ran_suu(1) % 10 + 5;
            sw->se0 = 1;
        }
        w->rot[1] += 0x200;
        break;
    }
    switch (sw->se1) {
    case 0:
        w->roll = (ran_suu(1) & 0xF) + 5;
        sw->se1 = ran_suu(1) % 3 + 1;
        break;
    case 1:
        if (--w->roll <= 0) {
            sw->se1 = 0;
        }
        break;
    case 2:
        if (--w->roll <= 0) {
            w->roll = (ran_suu(1) & 0xF) + 5;
            sw->se1 = 1;
        }
        sw->pos[1] += 1.0f;
        break;
    case 3:
        if (--w->roll <= 0) {
            w->roll = (ran_suu(1) & 0xF) + 5;
            sw->se1 = 1;
        }
        if (sw->pos[1] > 250.0f) {
            sw->pos[1] -= 1.0f;
        }
        break;
    }
    flmatInit(&w->mat);
    flmatRotY33(&w->mat, DEG2RAD(ANG2DEG(w->rot[1])));
    flvecApplyMat33(v, w->vel, &w->mat);
    sw->pos[0] += v[0];
    sw->pos[1] += v[1];
    sw->pos[2] += v[2];
    sw->prim->pos[0] = sw->pos[0];
    sw->prim->pos[1] = sw->pos[1];
    sw->prim->pos[2] = sw->pos[2];
    add_prim(ot1, sw->prim, 0x20, 0);
}

void set09_t03(PRIM *pr) {
    SETW *sw = pr->owner;
    FLMAT *wm = sw->u.work;
    SET_MDLW *mw = set_mdlw;
    CLAY *cl;
    int n;

    if (mw != 0 && mw->flag != 0) {
        if ((n = stg_eft_mdl_no[game_w.stage][4]) >= 0) {
            cl = &mw->clay[n];
            flSetRenderState(0x60, 0);
            flmatSetTrans(wm, pr->pos[0], pr->pos[1], pr->pos[2]);
            set09_t_sub(cl, wm);
            flSetRenderState(0x60, 0x80);
        }
    }
}

void set09_i04(SETW *sw) {
    SET09W1 *w = sw->u.work;

    sw->mode2 = 0;
    sw->se0 = 0;
    sw->se1 = 0;
    sw->timer = 0;
    switch (game_w.stage) {
    case 8:
        sw->pos[0] = ran_suu(1) % 2500 + 2000;
        sw->pos[1] = (ran_suu(1) & 0xF) + 30;
        sw->pos[2] = (ran_suu(1) & 0x1194) + 2500;
        w->rot[1] = ran_suu(1);
        break;
    case 40:
        sw->pos[0] = 10750.0f + (ran_suu(1) & 0xFF);
        sw->pos[1] = 450.0f;
        sw->pos[2] = 11700.0f + (ran_suu(1) & 0xFF);
        break;
    case 41:
        sw->pos[0] = 9773.0f + (ran_suu(1) & 0x7FF);
        sw->pos[1] = 150.0f;
        sw->pos[2] = 9173.0f + (ran_suu(1) & 0x3FF);
        break;
    }
    w->vel[0] = 0.0f;
    w->vel[1] = 0.0f;
    w->vel[2] = 5.0f;
}

void set09_m04(SETW *sw) {
    SET09W1 *w = sw->u.work;
    f32 v[3];

    switch (game_w.stage) {
    case 8:
        if (sw->pos[0] < 1500.0f || sw->pos[0] > 4000.0f || sw->pos[1] > 2000.0f
            || sw->pos[2] < 3000.0f || sw->pos[2] > 7000.0f) {
            sw->mode++;
            return;
        }
        break;
    case 16:
        if (sw->pos[0] < 9325.0f || sw->pos[0] > 11384.0f || sw->pos[1] > 1500.0f
            || sw->pos[2] < 7626.0f || sw->pos[2] > 9375.0f) {
            sw->mode++;
            return;
        }
        break;
    case 40:
        if (sw->pos[0] < 10450.0f || sw->pos[0] > 11850.0f || sw->pos[1] > 1500.0f
            || sw->pos[2] < 11400.0f || sw->pos[2] > 12500.0f) {
            sw->mode++;
            return;
        }
        break;
    case 41:
        if (sw->pos[0] < 9773.0f || sw->pos[0] > 11174.0f || sw->pos[1] > 2000.0f
            || sw->pos[2] < 9173.0f || sw->pos[2] > 10576.0f) {
            sw->mode++;
            return;
        }
        break;
    }
    switch (sw->mode2) {
    case 0:
        sw->timer = ran_suu(1) % 40 + 10;
        sw->mode2 = ran_suu(1) % 3 + 1;
        break;
    case 1:
        if (--sw->timer <= 0) {
            sw->mode2 = 0;
        }
        break;
    case 2:
        if (--sw->timer <= 0) {
            sw->mode2 = 1;
        }
        w->rot[1] -= 0x120;
        break;
    case 3:
        if (--sw->timer <= 0) {
            sw->mode2 = 1;
        }
        w->rot[1] += 0x120;
        break;
    }
    switch (sw->se0) {
    case 0:
        sw->se1 = (ran_suu(1) & 0xF) + 5;
        sw->se0 = ran_suu(1) % 2 + 1;
        break;
    case 1:
        if (--sw->se1 <= 0) {
            sw->se0 = 0;
        }
        break;
    case 2:
        if (--sw->se1 <= 0) {
            sw->se1 = (ran_suu(1) & 0xF) + 5;
            sw->se0 = 1;
        }
        sw->pos[1] += 1.0f;
        break;
    case 3:
        if (--sw->se1 <= 0) {
            sw->se1 = (ran_suu(1) & 0xF) + 5;
            sw->se0 = 1;
        }
        if (sw->pos[1] > 50.0f) {
            sw->pos[1] -= 1.0f;
        }
        break;
    }
    flmatInit(&w->mat);
    flmatRotY33(&w->mat, DEG2RAD(ANG2DEG(w->rot[1])));
    flvecApplyMat33(v, w->vel, &w->mat);
    sw->pos[0] += v[0];
    sw->pos[1] += v[1];
    sw->pos[2] += v[2];
    sw->prim->pos[0] = sw->pos[0];
    sw->prim->pos[1] = sw->pos[1];
    sw->prim->pos[2] = sw->pos[2];
    add_prim(ot1, sw->prim, 0x20, 0);
}

void set09_t04(PRIM *pr) {
    SETW *sw = pr->owner;
    FLMAT *wm = sw->u.work;
    SET_MDLW *mw = set_mdlw;
    CLAY *cl;
    int n;

    if (mw != 0 && mw->flag != 0) {
        if ((n = stg_eft_mdl_no[game_w.stage][5]) >= 0) {
            cl = &mw->clay[n];
            flmatSetTrans(wm, pr->pos[0], pr->pos[1], pr->pos[2]);
            set09_t_sub(cl, wm);
        }
    }
}
