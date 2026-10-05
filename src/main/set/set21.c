/* set21 - SLPM_654.95 0x00225FC0-0x002267EC. A model held between two joints
 * (6 and 9) of a monster (SETW+0x34). When the monster's animation 0x432
 * reaches frame 48 it is thrown: it flies for 10 frames towards a per-stage
 * spot (stages 0x51-0x55, set21_st8x_pos/ang by arg), lands with an Eft13
 * puff and stays for 300 frames. */
#include "set.h"
#include "em.h"
#include "game.h"
#include "prim.h"
#include "clay.h"
#include "fl.h"

typedef struct SET_MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x2F];
    CLAY *clay;         /* 0x30 */
} SET_MDLW;

/* Flight step (SETW+0x18). */
typedef struct SET21_RATE {
    f32 v[3];           /* 0x00 per frame; v[1] falls by 2 each frame */
    s16 ang;            /* 0x0C */
} SET21_RATE;

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

extern SET_MDLW *set_mdlw;
extern f32 set21_st81_pos[1][3];
extern f32 set21_st82_pos[1][3];
extern f32 set21_st83_pos[1][3];
extern f32 set21_st84_pos[1][3];
extern f32 set21_st85_pos[2][3];
extern s16 set21_st81_ang[1];
extern s16 set21_st82_ang[1];
extern s16 set21_st83_ang[1];
extern s16 set21_st84_ang[1];
extern s16 set21_st85_ang[2];

void release_prim(s16);
void get_joint_pos_em(EMW *, int, f32 *);
int em_frame_check(EMW *, int, f32);
void flvecCopy(f32 *, f32 *);
void Eft13_set_pos(f32, f32 *, int);

static void set21_move(SETW *sw);
static void set21_i(SETW *sw);
static void set21_m(SETW *sw);
static void set21_d(SETW *sw);
static void set21_e(SETW *sw);
static void set21_trans(PRIM *pr);

void Set21_set(EMW *em, int arg) {
    SETW *sw = pull_set_work(1);

    if (sw != 0) {
        sw->type = 21;
        sw->move = set21_move;
        sw->x34 = (s32)em;
        sw->arg = arg;
    }
}

static void set21_pos_get(SETW *sw, EMW *em) {
    f32 a[3];
    f32 b[3];

    get_joint_pos_em(em, 6, a);
    get_joint_pos_em(em, 9, b);
    sw->pos[0] = (a[0] + b[0]) / 2.0f;
    sw->pos[1] = 2.0f + (a[1] + b[1]) / 2.0f;
    sw->pos[2] = (a[2] + b[2]) / 2.0f;
    sw->cnt = em->ang[1];
}

static void set21_st_pos_get(f32 *pos, s16 n) {
    f32 *p;

    switch (game_w.stage) {
    case 0x51:
        p = set21_st81_pos[n];
        break;
    case 0x52:
        p = set21_st82_pos[n];
        break;
    case 0x53:
        p = set21_st83_pos[n];
        break;
    case 0x54:
        p = set21_st84_pos[n];
        break;
    case 0x55:
        p = set21_st85_pos[n];
        break;
    }
    flvecCopy(pos, p);
}

static void set21_st_ang_get(s16 *ang, s16 n) {
    switch (game_w.stage) {
    case 0x51:
        *ang = set21_st81_ang[n];
        break;
    case 0x52:
        *ang = set21_st82_ang[n];
        break;
    case 0x53:
        *ang = set21_st83_ang[n];
        break;
    case 0x54:
        *ang = set21_st84_ang[n];
        break;
    case 0x55:
        *ang = set21_st85_ang[n];
        break;
    }
}

static void set21_rate_init(SETW *sw, SET21_RATE *r) {
    f32 pos[3];
    s16 ang;

    set21_st_pos_get(pos, sw->arg);
    set21_st_ang_get(&ang, sw->arg);
    r->v[0] = (pos[0] - sw->pos[0]) / 10.0f;
    r->v[1] = (pos[1] - sw->pos[1]) / 10.0f - -10.0f;
    r->v[2] = (pos[2] - sw->pos[2]) / 10.0f;
    r->ang = (f32)(sw->cnt - ang) / 10.0f;
}

static void set21_rate_add(SETW *sw, SET21_RATE *r) {
    sw->pos[0] += r->v[0];
    sw->pos[1] += r->v[1];
    sw->pos[2] += r->v[2];
    r->v[1] += -2.0f;
    sw->cnt += r->ang;
}

static void set21_move(SETW *sw) {
    switch (sw->mode) {
    case 0:
        set21_i(sw);
        break;
    case 1:
        set21_m(sw);
        break;
    case 2:
        set21_d(sw);
        break;
    case 3:
        set21_e(sw);
        break;
    }
}

static void set21_i(SETW *sw) {
    sw->mode++;
    sw->be_flag = 1;
    sw->work14 = 0;
    sw->mode2 = 0;
    sw->se0 = 0;
    sw->se1 = 0;
    sw->timer = 0;
    set21_pos_get(sw, (EMW *)sw->x34);
    sw->prim_no = get_prim();
    if (sw->prim_no != -1) {
        sw->prim = get_prim_ptr(sw->prim_no);
        sw->prim->owner = sw;
        sw->prim->trans = set21_trans;
    } else {
        push_set_work(sw);
    }
}

static void set21_m(SETW *sw) {
    SET21_RATE *r = sw->u.work;
    EMW *em = (EMW *)sw->x34;

    switch (sw->mode2) {
    case 0:
        if (em->be_flag == 0) {
            sw->mode++;
            return;
        }
        set21_pos_get(sw, em);
        if (em->char0 == 0x432 && em_frame_check(em, 0, 48.0f) != 0) {
            set21_rate_init(sw, r);
            sw->timer = 0;
            sw->mode2++;
        }
        break;
    case 1:
        set21_rate_add(sw, r);
        if (++sw->timer >= 10) {
            set21_st_pos_get(sw->pos, sw->arg);
            set21_st_ang_get(&sw->cnt, sw->arg);
            Eft13_set_pos(0.8f, sw->pos, 7);
            sw->timer = 0;
            sw->mode2++;
        }
        break;
    case 2:
        if (++sw->timer >= 300) {
            sw->mode++;
            return;
        }
        break;
    }
    sw->prim->pos[0] = sw->pos[0];
    sw->prim->pos[1] = sw->pos[1];
    sw->prim->pos[2] = sw->pos[2];
    add_prim(ot1, sw->prim, 0x20, 0);
}

static void set21_d(SETW *sw) {
    sw->mode++;
    sw->be_flag = 0;
    release_prim(sw->prim_no);
}

static void set21_e(SETW *sw) {
    push_set_work(sw);
}

static void set21_trans(PRIM *pr) {
    FLMAT m;
    SET_MDLW *mw = set_mdlw;
    SETW *sw = pr->owner;
    CLAY *cl;

    if (mw != 0 && mw->flag != 0) {
        flmatMakeTrans(&m, pr->pos[0], pr->pos[1], pr->pos[2]);
        flmatRotY33(&m, DEG2RAD(ANG2DEG(sw->cnt)));
        flSetRenderState(0x1A, (u32)&m);
        cl = mw->clay;
        if (cl != 0 && cl->handle != -1) {
            clay_attr_set(cl->attr);
            flExecuteClay(cl->handle, 0);
        }
        clay_attr_reset();
    }
}
