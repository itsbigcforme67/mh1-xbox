/* NONMATCHING: eft11_i (0x00545F20), not built. Same code, but our build
 * loads eft11_t0's address before the position stores instead of just
 * before the copy loop, which shifts the registers (163 differ). The rest
 * matches, built from eft11.c (eft11_move) and eft11b.c. */
/* eft11 - game.bin 0x00545E90-0x005468E8. A 13-part burst on a monster
 * (scale, fade and spin of each part follow keyframe tables). The effect
 * work doubles as its own draw primitive: work14 (PRIM+0x14, trans) holds
 * eft11_t0 and work (PRIM+0x18, owner) the part list. */
#include "eft.h"
#include "game.h"
#include "prim.h"
#include "clay.h"
#include "fl.h"

typedef struct SET_MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x2F];
    CLAY *clay;         /* 0x30 */
} SET_MDLW;

/* One part (0x24 bytes). */
typedef struct EFT11_PART {
    VEC3 pos;           /* 0x00 */
    VEC3 scale;         /* 0x0C */
    f32 alpha;          /* 0x18 */
    u16 roty;           /* 0x1C */
    u8 _pad1E[2];
    PRIM *prim;         /* 0x20 */
} EFT11_PART;

typedef struct KEY3 {
    s32 time;
    f32 v[3];
} KEY3;

typedef struct KEY1 {
    s32 time;
    f32 v;
} KEY1;

typedef struct KEYA {
    s32 time;
    u16 v;
    u8 _pad06[2];
} KEYA;

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

extern SET_MDLW *set_mdlw;
extern FLMAT rview_matY;
extern EFT11_PART eft11_def[13];
extern KEY3 *scale_tbl[13];
extern KEY1 *alpha_tbl[13];
extern KEYA *roty_tbl[13];
extern u16 rotz_tbl[13];
extern s32 mdl_tbl_00645EB0[13];

u8 Em_stg_ck(EMW *);
void flvecApplyMat33_2(f32 *, FLMAT *);
void flmatRotZ33(FLMAT *, f32);

static void eft11_move(EFTW *ew);
static void eft11_i(EFTW *ew);
static void eft11_m(EFTW *ew);
static void eft11_d(EFTW *ew);
static void eft11_e(EFTW *ew);
static void eft11_t0(EFTW *ew);

static void eft11_move(EFTW *ew) {
    switch (ew->mode) {
    case 0:
        eft11_i(ew);
        break;
    case 1:
        eft11_m(ew);
        break;
    case 2:
        eft11_d(ew);
        break;
    case 3:
        eft11_e(ew);
        break;
    }
}

static void eft11_i(EFTW *ew) {
    EMW *em = ew->owner;
    EFT11_PART *w = ew->work;
    s32 i;

    ew->mode++;
    ew->mode2 = 0;
    ew->be_flag = 1;
    ew->work14 = 0;
    ew->timer = 0;
    ew->pos[0] = em->pos[0];
    ew->pos[1] = 50.2f + em->x5AC;
    ew->pos[2] = em->pos[2];
    for (i = 0; i < 13; i++) {
        w[i].pos = eft11_def[i].pos;
        w[i].scale = eft11_def[i].scale;
        w[i].alpha = eft11_def[i].alpha;
        w[i].roty = eft11_def[i].roty;
        ew->work14 = (s32)eft11_t0;
    }
}

static void eft11_m(EFTW *ew) {
    EFT11_PART *w = ew->work;
    KEY3 *k;
    KEY1 *a;
    KEYA *r;
    s32 i;

    if (++ew->timer > 60) {
        ew->mode++;
        ew->be_flag = 0;
        return;
    }
    for (i = 0; i < 13; i++, w++) {
        if ((k = scale_tbl[i]) != 0) {
            while (k->time < ew->timer) {
                k++;
            }
            w->scale.x += (k->v[0] - k[-1].v[0]) / (f32)(k->time - k[-1].time);
            w->scale.y += (k->v[1] - k[-1].v[1]) / (f32)(k->time - k[-1].time);
            w->scale.z += (k->v[2] - k[-1].v[2]) / (f32)(k->time - k[-1].time);
        }
        if ((a = alpha_tbl[i]) != 0) {
            while (a->time < ew->timer) {
                a++;
            }
            if (a->v == 0.0f && a[-1].v == 0.0f) {
                w->alpha = 0.0f;
            } else {
                w->alpha += (a->v - a[-1].v) / (f32)(a->time - a[-1].time);
            }
        }
        if ((r = roty_tbl[i]) != 0) {
            while (r->time < ew->timer) {
                r++;
            }
            w->roty += (u16)((r->v - r[-1].v) / (r->time - r[-1].time));
        }
        if (ew->work14 == 0) {
            w->prim->pos[0] = w->pos.x;
            w->prim->pos[1] = w->pos.y;
            w->prim->pos[2] = w->pos.z;
            add_prim(ot0, w->prim, 0x40, 0);
        }
    }
}

static void eft11_d(EFTW *ew) {
    ew->mode++;
}

static void eft11_e(EFTW *ew) {
    push_eft_work(ew);
}

static void eft11_t0(EFTW *ew) {
    f32 v[3];
    FLMAT m;
    SET_MDLW *mw = set_mdlw;
    EFT11_PART *w = ew->work;
    CLAY *cl;
    s32 i;

    if (mw != 0 && mw->flag != 0) {
        flSetRenderState(0x6C, 0);
        flSetRenderState(0x60, 0);
        for (i = 0; i < 13; i++, w++) {
            flSetRenderState(0x67, ((u8)(255.0f * w->alpha) << 24) | 0xFFFFFF);
            v[0] = w->pos.x;
            v[1] = w->pos.y;
            v[2] = w->pos.z;
            flvecApplyMat33_2(v, &rview_matY);
            if (ew->arg == 0) {
                flmatMakeScale(&m, 0.4f * w->scale.x, 0.4f * w->scale.y, 0.4f * w->scale.z);
            } else {
                flmatMakeScale(&m, 0.8f * w->scale.x, 0.8f * w->scale.y, 0.8f * w->scale.z);
            }
            flmatRotY33(&m, DEG2RAD(ANG2DEG(w->roty)));
            flmatRotZ33(&m, DEG2RAD(ANG2DEG(rotz_tbl[i])));
            if (i < 4) {
                flmatMul33_2(&m, &rview_matY);
            }
            flmatSetTrans(&m, ew->pos[0] + v[0], ew->pos[1] + v[1], ew->pos[2] + v[2]);
            flSetRenderState(0x1A, (u32)&m);
            cl = &mw->clay[mdl_tbl_00645EB0[i]];
            if (cl != 0) {
                if (cl->handle != -1) {
                    clay_attr_set(cl->attr);
                    flExecuteClay(cl->handle, 0);
                }
                clay_attr_reset();
            }
        }
        flSetRenderState(0x60, 0);
        flSetRenderState(0x6C, 1);
    }
}

void eft11_set(EMW *em, f32 *pos, int arg) {
    EFTW *ew;

    if (Em_stg_ck(em) != 0) {
        ew = pull_eft_work(1);
        if (ew != 0) {
            ew->type = 11;
            ew->move = eft11_move;
            ew->owner = em;
            *(VEC3 *)ew->pos = *(VEC3 *)pos;
            ew->arg = arg;
        }
    }
}
