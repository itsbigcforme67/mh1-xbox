/* set09 - game.bin 0x00618CA0-0x0061ED08: set09_set to set09_t11.
 * A stage effect manager: arg 0 is the controller, whose work holds up to
 * 32 child objects (args 1-11) and a count per kind; each kind has its own
 * init/move/draw functions (set09_iNN/mNN/tNN).
 * Kinds (our guesses from the code and table names, not checked in game):
 * 1 tumbling falling object, 2 beetle (set09_st08_beetle_tbl), 3/4 small
 * wanderers kept inside a box per stage, 5 a flapping flyer drawn with two
 * primitives, 6 spinning object at the stage start point, 7 object circling
 * a point, 8 fading sparkle (stage 36), 9 jumper on an arc (stage 5),
 * 10 drifting object, 11 bird flying between points (set09_st33_bird_tbl).
 * Which stages spawn what is in set09_i00/set09_m00. */
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
static void set09_t_sub(CLAY *cl, FLMAT *m);

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
extern f32 stage_start_pos[][3];
extern f32 set09_st33_bird_tbl[][3];
extern f32 set09_st05_type09_pos[];
extern u16 set09_st05_type09_ang[];
f32 flArcTan2(f32, f32);
void flmatRotXYZ33(FLMAT *, f32, f32, f32);
void flmatRotY33(FLMAT *, f32);
void flmatRotZ33(FLMAT *, f32);
void flmatGetTrans(f32 *, FLMAT *);
f32 flCos(f32);
f32 flSin(f32);
u16 calc_vec_ang(f32, f32, f32, f32);
int hit_point_cyl(f32 *, f32 *, f32, f32, f32);
void SetTrnslMode(int, int);
extern FLMAT rview_mat;
f32 flAbs(f32);
void flvecRotY(f32 *, f32);

/* Work of kind 1 (a tumbling falling object). */
typedef struct SET09W1 {
    FLMAT mat;          /* 0x00 */
    f32 vel[3];         /* 0x40 */
    u16 rot[3];         /* 0x4C */
    s16 spin;           /* 0x52 */
    s16 x54;            /* 0x54 kind 7: turn speed */
    s16 roll;           /* 0x56 */
} SET09W1;

static SETW *set09_set_sub_sub(s16 arg);
static void set09_set_sub(SETW *sw, s16 arg);
static void set09_set_sub3(SETW *sw, s16 arg, s16 cnt);
static void set09_08_pos_reset(SETW *sw);
static void set09_09_rate_add(SETW *sw);
static void set09_09_pos_reset(SETW *sw);
static void set09_10_pos_reset(SETW *sw);
static s16 set09_pull(SETW *sw);
static void set09_move(SETW *sw);
static void set09_i(SETW *sw);
static void set09_m(SETW *sw);
static void set09_d(SETW *sw);
static void set09_e(SETW *sw);
static void set09_trans(PRIM *pr);
static void set09_i00(SETW *sw);
static void set09_i01(SETW *sw);
static void set09_i02(SETW *sw);
static void set09_i03(SETW *sw);
static void set09_i04(SETW *sw);
static void set09_i05(SETW *sw);
static void set09_i06(SETW *sw);
static void set09_i07(SETW *sw);
static void set09_i08(SETW *sw);
static void set09_i09(SETW *sw);
static void set09_i10(SETW *sw);
static void set09_i11(SETW *sw);
static void set09_m00(SETW *sw);
static void set09_m01(SETW *sw);
static void set09_m02(SETW *sw);
static void set09_m03(SETW *sw);
static void set09_m04(SETW *sw);
static void set09_m05(SETW *sw);
static void set09_m06(SETW *sw);
static void set09_m07(SETW *sw);
static void set09_m08(SETW *sw);
static void set09_m09(SETW *sw);
static void set09_m10(SETW *sw);
static void set09_m11(SETW *sw);
static void set09_t01(PRIM *pr);
static void set09_t02(PRIM *pr);
static void set09_t03(PRIM *pr);
static void set09_t04(PRIM *pr);
static void set09_t05(PRIM *pr);
static void set09_t06(PRIM *pr);
static void set09_t07(PRIM *pr);
static void set09_t08(PRIM *pr);
static void set09_t09(PRIM *pr);
static void set09_t10(PRIM *pr);
static void set09_t11(PRIM *pr);

void set09_set(void) {
    SETW *sw = pull_set_work(1);

    if (sw != 0) {
        sw->type = 9;
        sw->arg = 0;
        sw->x1E = 0;
        sw->move = set09_move;
    }
}

static SETW *set09_set_sub_sub(s16 arg) {
    switch (arg) {
    case 8:
    case 10:
    case 11:
        return pull_set_work(0);
    default:
        return pull_set_work(1);
    }
}

static void set09_set_sub(SETW *sw, s16 arg) {
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

static void set09_set_sub3(SETW *sw, s16 arg, s16 cnt) {
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

static s16 set09_pull(SETW *sw) {
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

static void set09_move(SETW *sw) {
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

static void set09_i(SETW *sw) {
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

static void set09_m(SETW *sw) {
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
    f32 spd;            /* 0x04 forward speed */
    f32 side;           /* 0x08 sideways speed */
    u16 ang;            /* 0x0C heading */
    u16 bank;           /* 0x0E roll angle, drawn mirrored for the second primitive */
    u16 bank_to;        /* 0x10 */
    s16 bank_t;         /* 0x12 frames to reach bank_to */
    PRIM *prim[2];      /* 0x14 */
} SET09W5;

static void set09_d(SETW *sw) {
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

static void set09_e(SETW *sw) {
    push_set_work(sw);
}

static void set09_trans(PRIM *pr) {
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

static void set09_t_sub(CLAY *cl, FLMAT *m) {
    flSetRenderState(0x1A, (u32)m);
    if (cl != 0 && cl->handle != -1) {
        clay_attr_set(cl->attr);
        flExecuteClay(cl->handle, 0);
    }
    clay_attr_reset();
}


static void set09_i00(SETW *sw) {
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


static void set09_m00(SETW *sw) {
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

static void set09_i01(SETW *sw) {
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

static void set09_m01(SETW *sw) {
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

static void set09_t01(PRIM *pr) {
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

static void set09_i02(SETW *sw) {
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

static void set09_m02(SETW *sw) {
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

static void set09_t02(PRIM *pr) {
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

static void set09_i03(SETW *sw) {
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

static void set09_m03(SETW *sw) {
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

static void set09_t03(PRIM *pr) {
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

static void set09_i04(SETW *sw) {
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

static void set09_m04(SETW *sw) {
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

static void set09_t04(PRIM *pr) {
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

static void set09_i05(SETW *sw) {
    SET09W5 *w = sw->u.work;
    s16 i;

    sw->mode2 = 0;
    sw->se0 = 0;
    switch (game_w.stage) {
    case 5:
        sw->pos[0] = 9150.0f + (ran_suu(1) & 0x3FF);
        sw->pos[1] = 200.0f;
        sw->pos[2] = 4500.0f + (ran_suu(1) & 0x1FFF);
        break;
    case 16:
        sw->pos[0] = 9325.0f + (ran_suu(1) & 0x7FF);
        sw->pos[1] = 150.0f;
        sw->pos[2] = 7626.0f + (ran_suu(1) & 0x3FF);
        break;
    case 41:
        sw->pos[0] = 9773.0f + (ran_suu(1) & 0x7FF);
        sw->pos[1] = 150.0f;
        sw->pos[2] = 9173.0f + (ran_suu(1) & 0x3FF);
        break;
    default:
        sw->pos[0] = (ran_suu(1) & 0x7FF) + 1500;
        sw->pos[1] = (ran_suu(1) & 0xF) + 30;
        sw->pos[2] = (ran_suu(1) & 0xFFF) + 2000;
        w->ang = ran_suu(1);
        break;
    }
    sw->timer = 0;
    w->spd = 0.0f;
    w->side = 0.0f;
    w->bank = ran_suu(1) % 0x6000 - 0x3000;
    for (i = 0; i < 2; i++) {
        w->prim_no[i] = get_prim();
        if (w->prim_no[i] != -1) {
            w->prim[i] = get_prim_ptr(w->prim_no[i]);
            w->prim[i]->owner = sw;
            w->prim[i]->no = i;
            w->prim[i]->trans = set09_trans;
        } else {
            w->prim[i] = 0;
        }
    }
}

static void set09_m05(SETW *sw) {
    SET09W5 *w = sw->u.work;
    f32 v[3];
    f32 k;
    s16 a;
    s16 d;
    s16 i;

    switch (game_w.stage) {
    case 5:
        if (sw->pos[0] < 9150.0f || sw->pos[0] > 10850.0f || sw->pos[1] > 2000.0f || sw->pos[1] < -100.0f
            || sw->pos[2] < 4500.0f || sw->pos[2] > 15500.0f) {
            sw->mode++;
            return;
        }
        k = 2.0f;
        break;
    case 16:
        if (sw->pos[0] < 9325.0f || sw->pos[0] > 11384.0f || sw->pos[1] > 2000.0f || sw->pos[1] < -100.0f
            || sw->pos[2] < 7626.0f || sw->pos[2] > 9375.0f) {
            sw->mode++;
            return;
        }
        k = 1.0f;
        break;
    case 41:
        if (sw->pos[0] < 9773.0f || sw->pos[0] > 11174.0f || sw->pos[1] > 2000.0f || sw->pos[1] < -100.0f
            || sw->pos[2] < 9173.0f || sw->pos[2] > 10576.0f) {
            sw->mode++;
            return;
        }
        k = 1.0f;
        break;
    default:
        if (sw->pos[0] < 1500.0f || sw->pos[0] > 4000.0f || sw->pos[1] > 2000.0f || sw->pos[1] < -100.0f
            || sw->pos[2] < 3000.0f || sw->pos[2] > 7000.0f) {
            sw->mode++;
            return;
        }
        k = 1.0f;
        break;
    }
    switch ((s16)(ran_suu(1) % 5)) {
    case 0:
        break;
    case 1:
        if (w->spd < 4.0f * k) {
            w->spd += 0.15f;
        }
        break;
    case 2:
        w->spd -= 0.15f * k;
        if (w->spd < 0.0f) {
            w->spd = 0.0f;
        }
        break;
    case 3:
        if (w->spd < 4.0f * k) {
            w->spd += 0.2f;
        }
        break;
    case 4:
        w->spd *= 0.5f;
        break;
    }
    switch ((s16)(ran_suu(1) % 3)) {
    case 0:
        break;
    case 1:
        if (w->side < 0.8f * k) {
            w->side += 0.05f;
        }
        break;
    case 2:
        if (w->side > 0.8f * -k) {
            w->side -= 0.05f;
        }
        break;
    }
    v[0] = w->side;
    v[1] = 0.0f;
    v[2] = w->spd;
    switch (sw->mode2) {
    case 0:
        sw->mode2++;
        a = ran_suu(1) % 0x1800;
        w->bank_t = (s16)(2.0f * k) + ran_suu(1) % 3;
        if (sw->pos[1] < 200.0f) {
            w->bank_t--;
        } else if (sw->pos[1] > 300.0f) {
            w->bank_t++;
        }
        if ((s16)w->bank >= 0) {
            w->bank_to = -0x2000 - a;
        } else {
            w->bank_to = a + 0x2000;
        }
        break;
    case 1:
        d = (s16)(w->bank_to - (s16)w->bank) / w->bank_t;
        if (d < 0) {
            sw->pos[1] -= 0.6f * ((1.5f + k) * ((d - 0x300) / 1600.0f));
        }
        w->bank += d;
        w->bank_t--;
        if (w->bank_t <= 0) {
            sw->mode2++;
        }
        break;
    case 2:
        sw->mode2 = 0;
        break;
    }
    switch (sw->se0) {
    case 0:
        sw->timer = ran_suu(1) % 20 + 10;
        sw->se0 = ran_suu(1) % 3 + 1;
    case 1:
        if (--sw->timer <= 0) {
            sw->se0 = 0;
        }
        break;
    case 2:
        if (--sw->timer <= 0) {
            sw->se0 = 1;
        }
        w->ang -= 0x180;
        break;
    case 3:
        if (--sw->timer <= 0) {
            sw->se0 = 1;
        }
        w->ang += 0x180;
        break;
    }
    sw->pos[1] += -2.9f * (((s16)flAbs((s16)w->bank) + 0x1000) / 15000.0f);
    flvecRotY(v, DEG2RAD(ANG2DEG(w->ang)));
    sw->pos[0] += v[0];
    sw->pos[1] += v[1];
    sw->pos[2] += v[2];
    for (i = 0; i < 2; i++) {
        w->prim[i]->pos[0] = sw->pos[0];
        w->prim[i]->pos[1] = sw->pos[1];
        w->prim[i]->pos[2] = sw->pos[2];
        add_prim(ot1, w->prim[i], 0x20, 0);
    }
}

static void set09_t05(PRIM *pr) {
    FLMAT m;
    SETW *sw = pr->owner;
    SET09W5 *w = sw->u.work;
    SET_MDLW *mw = set_mdlw;
    CLAY *cl;
    int n;

    if (mw != 0 && mw->flag != 0) {
        if ((n = stg_eft_mdl_no[game_w.stage][6]) >= 0) {
            cl = &mw->clay[n] + pr->no;
            flmatInit(&m);
            if (pr->no == 0) {
                flmatRotZ33(&m, DEG2RAD(ANG2DEG(w->bank)));
            } else {
                flmatRotZ33(&m, DEG2RAD(ANG2DEG(-w->bank)));
            }
            flmatRotY33(&m, DEG2RAD(ANG2DEG(w->ang)));
            flmatSetTrans(&m, pr->pos[0], pr->pos[1], pr->pos[2]);
            set09_t_sub(cl, &m);
        }
    }
}

static void set09_i06(SETW *sw) {
    SET09W1 *w = sw->u.work;

    sw->pos[0] = stage_start_pos[game_w.stage][0];
    sw->pos[1] = 100.0f + stage_start_pos[game_w.stage][1];
    sw->pos[2] = stage_start_pos[game_w.stage][2];
    sw->timer = 0;
    w->rot[0] = (ran_suu(1) & 0xF) - 8;
    w->rot[1] = (ran_suu(1) & 0xF) - 8;
    w->rot[2] = (ran_suu(1) & 0xF) - 8;
    flmatInit(&w->mat);
}

static void set09_m06(SETW *sw) {
    SET09W1 *w = sw->u.work;

    flmatRotXYZ33(&w->mat, DEG2RAD(ANG2DEG(w->rot[0])), DEG2RAD(ANG2DEG(w->rot[1])), DEG2RAD(ANG2DEG(w->rot[2])));
    sw->timer += 2;
    if (sw->timer >= 20) {
        sw->timer = 0;
    }
    sw->prim->pos[0] = sw->pos[0];
    sw->prim->pos[1] = sw->pos[1];
    sw->prim->pos[2] = sw->pos[2];
    add_prim(ot1, sw->prim, 0x20, 0);
}

static void set09_t06(PRIM *pr) {
    FLMAT m;
    SETW *sw = pr->owner;
    FLMAT *wm = sw->u.work;
    SET_MDLW *mw = set_mdlw;
    CLAY *cl;
    int n;

    if (mw != 0 && mw->flag != 0) {
        if ((n = stg_eft_mdl_no[game_w.stage][7]) >= 0) {
            cl = &mw->clay[n];
            flmatMakeTrans(&m, 0.05f * sw->timer, 0.0f, 0.0f);
            flSetRenderState(0x19, (u32)&m);
            flmatSetTrans(wm, pr->pos[0], pr->pos[1], pr->pos[2]);
            set09_t_sub(cl, wm);
        }
    }
}

static void set09_i07(SETW *sw) {
    SET09W1 *w = sw->u.work;
    f32 r;
    f32 t;

    switch (game_w.stage) {
    case 15:
        sw->pos[0] = 2000.0f + (ran_suu(1) & 0x3FFF);
        sw->pos[1] = 2000.0f;
        sw->pos[2] = 2000.0f + (ran_suu(1) & 0x3FFF);
        if (sw->pos[0] < 10000.0f) {
            r = sw->pos[0];
        } else {
            r = 20000.0f - sw->pos[0];
        }
        if (sw->pos[2] < 10000.0f) {
            t = sw->pos[2];
        } else {
            t = 20000.0f - sw->pos[2];
        }
        if (t < r) {
            r = t;
        }
        if (r > 6000.0f) {
            r = 6000.0f;
        }
        break;
    case 20:
    case 32:
        sw->pos[0] = 9737.0f + ((ran_suu(1) & 0x3FF) - 0x200);
        sw->pos[1] = 8412.0f + (ran_suu(1) & 0x3FF);
        sw->pos[2] = 6057.0f + ((ran_suu(1) & 0x3FF) - 0x200);
        r = 2000.0f + (ran_suu(1) & 0x3FF);
        break;
    case 51:
        sw->pos[0] = 10100.0f + ((ran_suu(1) & 0x3FF) - 0x200);
        sw->pos[1] = 1700.0f + (ran_suu(1) & 0x3FF);
        sw->pos[2] = 10000.0f + ((ran_suu(1) & 0x3FF) - 0x200);
        r = 2000.0f + (ran_suu(1) & 0xFF);
        break;
    case 52:
        sw->pos[0] = 10500.0f + ((ran_suu(1) & 0x7FF) - 0x400);
        sw->pos[1] = 3000.0f + (ran_suu(1) & 0x7FF);
        sw->pos[2] = 10000.0f + ((ran_suu(1) & 0x7FF) - 0x400);
        r = 2000.0f + (ran_suu(1) & 0x1FF);
        break;
    case 53:
        sw->pos[0] = 10000.0f + ((ran_suu(1) & 0x7FF) - 0x400);
        sw->pos[1] = 2500.0f + (ran_suu(1) & 0x7FF);
        sw->pos[2] = 9500.0f + ((ran_suu(1) & 0x7FF) - 0x400);
        r = 2000.0f + (ran_suu(1) & 0xFF);
        break;
    case 55:
        sw->pos[0] = 6300.0f + ((ran_suu(1) & 0x1FF) - 0x100);
        sw->pos[1] = 1500.0f + (ran_suu(1) & 0x3FF);
        sw->pos[2] = 5900.0f + ((ran_suu(1) & 0x1FF) - 0x100);
        r = 500.0f + (ran_suu(1) & 0x1FF);
        break;
    }
    w->x54 = 65536.0f * (33.0f / (6.2831855f * r));
    sw->timer = 0;
    w->rot[1] = ran_suu(1);
    flmatMakeTrans(&w->mat, 0.0f, 0.0f, r);
    if (ran_suu(1) & 1) {
        flmatRotY33(&w->mat, 1.5707964f);
        sw->se1 = 0;
    } else {
        w->x54 = -w->x54;
        flmatRotY33(&w->mat, 4.712389f);
        sw->se1 = 1;
    }
}

static void set09_m07(SETW *sw) {
    SET09W1 *w = sw->u.work;
    f32 v[3];

    w->rot[1] += (u16)w->x54;
    flmatGetTrans(v, &w->mat);
    flvecRotY(v, DEG2RAD(ANG2DEG(w->rot[1])));
    sw->prim->pos[0] = sw->pos[0] + v[0];
    sw->prim->pos[1] = sw->pos[1] + v[1];
    sw->prim->pos[2] = sw->pos[2] + v[2];
    add_prim(ot1, sw->prim, 0x20, 0);
}

static void set09_t07(PRIM *pr) {
    FLMAT m;
    SETW *sw = pr->owner;
    SET09W1 *w = sw->u.work;
    SET_MDLW *mw = set_mdlw;
    CLAY *cl;
    int n;

    if (mw != 0 && mw->flag != 0) {
        if ((n = stg_eft_mdl_no[game_w.stage][8]) >= 0) {
            cl = &mw->clay[n];
            switch (game_w.stage) {
            default:
                flmatInit(&m);
                break;
            case 20:
            case 32:
                flmatMakeScale(&m, 3.0f, 3.0f, 3.0f);
                break;
            }
            flmatRotY33(&m, DEG2RAD(ANG2DEG(w->rot[1])));
            flmatMul33_2(&m, &w->mat);
            flmatSetTrans(&m, pr->pos[0], pr->pos[1], pr->pos[2]);
            if (sw->se1 == 0) {
                RotateZ(&m, -0.17453294f);
            } else {
                RotateZ(&m, 0.17453294f);
            }
            set09_t_sub(cl, &m);
        }
    }
}

static void set09_08_pos_reset(SETW *sw) {
    if (sw->x1E != 1) {
        switch (game_w.stage) {
        case 36:
            sw->pos[0] = 11750.0f + ((ran_suu(1) & 0x1FF) - 0x100);
            sw->pos[1] = 200.0f;
            sw->pos[2] = 9500.0f + ((ran_suu(1) & 0x1FF) - 0x100);
            break;
        }
    }
    sw->timer = 0;
    sw->cnt = ((ran_suu(1) & 3) + 5) * 60;
    sw->se0 = ran_suu(1) & 1;
    sw->se1 = ran_suu(1) & 1;
    sw->speed = 1.0f;
}

static void set09_i08(SETW *sw) {
    set09_08_pos_reset(sw);
}

static void set09_m08(SETW *sw) {
    f32 a;
    f32 b;

    switch (sw->mode2) {
    case 0:
        sw->timer++;
        if (sw->timer >= sw->cnt) {
            sw->mode2++;
            sw->timer = 10;
        } else {
            sw->speed = flAbs(flCos(DEG2RAD(ANG2DEG(sw->timer << 10))));
        }
        a = 0.2f + 0.02f * (ran_suu(1) & 0x3F);
        b = 0.2f + 0.02f * (ran_suu(1) & 0x3F);
        if (sw->se0 != 0) {
            a *= -1.0f;
        }
        if (sw->se1 != 0) {
            b *= -1.0f;
        }
        sw->pos[0] += a;
        sw->pos[2] += b;
        sw->pos[1] += (a + b) / 2.0f;
        break;
    case 1:
        if (--sw->timer <= 0) {
            if (sw->x1E == 1) {
                sw->mode++;
                return;
            }
            sw->mode2 = 0;
            set09_08_pos_reset(sw);
        } else {
            sw->speed -= sw->speed / 10.0f;
        }
        break;
    }
    sw->prim->pos[0] = sw->pos[0];
    sw->prim->pos[1] = sw->pos[1];
    sw->prim->pos[2] = sw->pos[2];
    add_prim(ot0, sw->prim, 0x40, 0);
}

static void set09_t08(PRIM *pr) {
    FLMAT m;
    SETW *sw = pr->owner;
    SET_MDLW *mw = set_mdlw;
    CLAY *cl;
    int n;

    if (mw != 0 && mw->flag != 0) {
        if ((n = stg_eft_mdl_no[game_w.stage][9]) >= 0) {
            cl = &mw->clay[n];
            SetTrnslMode(4, 1);
            flSetRenderState(0x60, 0);
            flmatMakeScale(&m, sw->speed, sw->speed, sw->speed);
            flmatSetTrans(&m, pr->pos[0], pr->pos[1], pr->pos[2]);
            flmatMul33_2(&m, &rview_mat);
            flSetRenderState(0x67, ((u8)(255.0f * sw->speed) << 24) | 0xFFFFFF);
            set09_t_sub(cl, &m);
            SetTrnslMode(4, 5);
        }
    }
}

static void set09_09_rate_add(SETW *sw) {
    SET09W1 *w = sw->u.work;

    sw->pos[0] += w->vel[0];
    sw->pos[1] += w->vel[1];
    sw->pos[2] += w->vel[2];
}

static void set09_09_pos_reset(SETW *sw) {
    SET09W1 *w = sw->u.work;
    s16 k;
    f32 spread;

    spread = 0.0f;  /* a random spread, switched off */
    sw->se1 = 0;
    k = (s16)(ran_suu(1) & 7) * 2;
    sw->pos[0] = set09_st05_type09_pos[k];
    sw->pos[1] = 0.0f;
    sw->pos[2] = set09_st05_type09_pos[k + 1];
    sw->timer = 50;
    sw->cnt = set09_st05_type09_ang[k] + (u8)(spread * (0.001f * (ran_suu(1) & 0x3FF)));
    w->rot[0] = 0;
    w->vel[0] = 0.0f;
    w->vel[1] = 12.0f;
    w->vel[2] = 25.0f;
    flvecRotY(w->vel, DEG2RAD(ANG2DEG(sw->cnt)));
}

static void set09_i09(SETW *sw) {
    set09_09_pos_reset(sw);
}

static void set09_m09(SETW *sw) {
    SET09W1 *w = sw->u.work;

    sw->se1++;
    switch (sw->mode2) {
    case 0:
        if (--sw->timer <= 0) {
            sw->mode2++;
            sw->timer = 50;
        }
        set09_09_rate_add(sw);
        break;
    case 1:
        if (--sw->timer <= 0) {
            sw->mode2++;
            sw->timer = 50;
            w->vel[0] *= 0.8f;
            w->vel[2] *= 0.8f;
        }
        w->vel[1] -= 0.25f;
        set09_09_rate_add(sw);
        break;
    case 2:
        if (--sw->timer <= 0) {
            sw->mode2++;
            sw->timer = 50;
            w->vel[0] *= 0.8f;
            w->vel[2] *= 0.8f;
        }
        w->rot[0] -= 0x60;
        w->vel[1] -= 0.2f;
        set09_09_rate_add(sw);
        break;
    case 3:
        if (--sw->timer <= 0) {
            sw->mode2++;
            sw->timer = (ran_suu(1) & 0xF) + 30;
        }
        set09_09_rate_add(sw);
        break;
    case 4:
        if (--sw->timer <= 0) {
            sw->mode2 = 0;
            set09_09_pos_reset(sw);
        }
        return;
    }
    sw->prim->pos[0] = sw->pos[0];
    sw->prim->pos[1] = sw->pos[1];
    sw->prim->pos[2] = sw->pos[2];
    add_prim(ot1, sw->prim, 0x20, 0);
}

static void set09_t09(PRIM *pr) {
    FLMAT m;
    SETW *sw = pr->owner;
    SET09W1 *w = sw->u.work;
    SET_MDLW *mw = set_mdlw;
    CLAY *cl;

    if (mw != 0 && mw->flag != 0) {
        cl = &mw->clay[(s16)((sw->se1 >> 1) & 3)] + 4;
        flSetRenderState(0x60, 0x80);
        flmatMakeTrans(&m, pr->pos[0], pr->pos[1], pr->pos[2]);
        flmatRotY33(&m, DEG2RAD(ANG2DEG(sw->cnt)));
        RotateX(&m, DEG2RAD(ANG2DEG(w->rot[0])));
        set09_t_sub(cl, &m);
    }
}

static void set09_10_pos_reset(SETW *sw) {
    sw->pos[0] = 10000.0f;
    sw->pos[1] = 0.0f;
    sw->pos[2] = 8250.0f;
    sw->cnt = ran_suu(1);
    sw->speed = 0.0f;
}

static void set09_i10(SETW *sw) {
    sw->mode2++;
    set09_08_pos_reset(sw);
}

static void set09_m10(SETW *sw) {
    FLMAT m;
    f32 v[3];
    f32 d[3];

    sw->speed += 0.01f;
    if (sw->speed >= 0.5f) {
        sw->speed = 0.0f;
        flmatInit(&m);
        v[0] = 0.0f;
        v[2] = 330.0f;
        v[1] = 0.0f;
        flmatRotY33(&m, DEG2RAD(ANG2DEG((u16)sw->cnt)));
        flvecApplyMat33(d, v, &m);
        sw->pos[0] += d[0];
        sw->pos[1] += d[1];
        sw->pos[2] += d[2];
    }
    if (sw->pos[0] < 7500.0f || sw->pos[0] > 12500.0f || sw->pos[2] < 7000.0f || sw->pos[2] > 9500.0f) {
        set09_10_pos_reset(sw);
    }
    sw->prim->pos[0] = sw->pos[0];
    sw->prim->pos[1] = sw->pos[1];
    sw->prim->pos[2] = sw->pos[2];
    add_prim(ot0, sw->prim, 0x40, 0);
}

static void set09_t10(PRIM *pr) {
    FLMAT m2;
    FLMAT m;
    SETW *sw = pr->owner;
    SET_MDLW *mw = set_mdlw;
    CLAY *cl;

    if (mw != 0 && mw->flag != 0) {
        cl = &mw->clay[10];
        flmatMakeTrans(&m, 0.0f, sw->speed, 0.0f);
        flSetRenderState(0x19, (u32)&m);
        flmatInit(&m2);
        flmatRotY33(&m2, DEG2RAD(ANG2DEG((u16)sw->cnt)));
        flmatSetTrans(&m2, pr->pos[0], pr->pos[1], pr->pos[2]);
        set09_t_sub(cl, &m2);
    }
}

static void set09_i11(SETW *sw) {
    sw->mode2 = ran_suu(1) % sw->cnt;
    sw->se0 = ran_suu(1) % (sw->cnt - 1) + 1;
    sw->se0 += sw->mode2;
    if (sw->se0 >= sw->cnt) {
        sw->se0 -= sw->cnt;
    }
    sw->speed = 25.0f * (0.5f + 0.001f * (ran_suu(1) & 0x3FF));
    sw->se1 = 0;
    sw->timer = ran_suu(1);
    switch (game_w.stage) {
    case 27:
    case 33:
        sw->pos[0] = set09_st33_bird_tbl[sw->mode2][0];
        sw->pos[1] = set09_st33_bird_tbl[sw->mode2][1] + ((ran_suu(1) & 0xFF) - 0x80);
        sw->pos[2] = set09_st33_bird_tbl[sw->mode2][2];
        sw->cnt = calc_vec_ang(set09_st33_bird_tbl[sw->se0][0], set09_st33_bird_tbl[sw->se0][2], sw->pos[0], sw->pos[2]) + 0x4000;
        break;
    }
    sw->mode2 = (ran_suu(1) & 0x1F) + 0x30;
}

static void set09_m11(SETW *sw) {
    f32 v[3];
    f32 tgt[3];
    f32 s;

    sw->timer++;
    sw->se1++;
    sw->se1 &= 3;
    switch (game_w.stage) {
    case 27:
    case 33:
        tgt[0] = set09_st33_bird_tbl[sw->se0][0];
        tgt[1] = set09_st33_bird_tbl[sw->se0][1];
        tgt[2] = set09_st33_bird_tbl[sw->se0][2];
        break;
    }
    if (hit_point_cyl(sw->pos, tgt, 100.0f, -1.0f, -1.0f)) {
        sw->mode++;
        return;
    }
    v[0] = 0.0f;
    v[1] = 0.0f;
    v[2] = 10.0f;
    flvecRotY(v, DEG2RAD(ANG2DEG(sw->cnt)));
    sw->pos[0] += v[0];
    sw->pos[1] += v[1];
    sw->pos[2] += v[2];
    s = sw->speed * flSin(DEG2RAD(360.0f * ((f32)sw->timer / (f32)(sw->mode2 << 1))));
    sw->prim->pos[0] = sw->pos[0];
    sw->prim->pos[1] = sw->pos[1] + s;
    sw->prim->pos[2] = sw->pos[2];
    add_prim(ot1, sw->prim, 0x20, 0);
}

static void set09_t11(PRIM *pr) {
    FLMAT m;
    SETW *sw = pr->owner;
    SET_MDLW *mw = set_mdlw;
    CLAY *cl;

    if (mw != 0 && mw->flag != 0) {
        cl = &mw->clay[(s16)sw->se1] + 8;
        flSetRenderState(0x60, 0x80);
        flmatMakeTrans(&m, pr->pos[0], pr->pos[1], pr->pos[2]);
        flmatRotY33(&m, DEG2RAD(ANG2DEG(sw->cnt)));
        set09_t_sub(cl, &m);
    }
}
