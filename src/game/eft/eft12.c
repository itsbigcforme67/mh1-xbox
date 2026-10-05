/* eft12 - game.bin 0x005468F0-0x005498DC. Cooking effects, six types (arg):
 * 0 the barbecue spit set down by a player, 1 a smoke cloud (screen fog
 * when the camera is inside it, push_smoke), 2/3 meat on the spit (3 also
 * gives off a smell monsters notice, push_smell; it browns through
 * eft12_meat_col/bone_col and turns to face a player who checks it), 4 a
 * thrown egg that falls and breaks (spawning eft14 and Shell09) and 5 a
 * puff of steam. */
#include "eft.h"
#include "game.h"
#include "pl.h"
#include "shell.h"
#include "prim.h"
#include "clay.h"
#include "fl.h"

/* One material table entry (0x4C bytes). */
typedef struct MATERIAL {
    u8 _pad00[4];
    f32 col[3];         /* 0x04 */
    u8 _pad10[0x4C - 0x10];
} MATERIAL;

typedef struct EFT_MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x0F];
    MATERIAL *mat;      /* 0x10 material table */
    u8 _pad14[0x1C];
    CLAY *clay;         /* 0x30 */
} EFT_MDLW;

/* Smoke or smell cloud handed to push_smoke/push_smell (0x18 bytes). */
typedef struct SMOKE {
    f32 pos[3];         /* 0x00 */
    f32 size;           /* 0x0C */
    u8 pl_id;           /* 0x10 */
    u8 x11;             /* 0x11 */
    u8 x12;             /* 0x12 */
    u8 done;            /* 0x13 */
    u8 stg;             /* 0x14 */
    u8 x15;             /* 0x15 */
    s16 dmg;            /* 0x16 */
} SMOKE;

/* One sprite (0x34 bytes) of the work area. */
typedef struct EFT12_PIECE {
    s16 prim_no;        /* 0x00 */
    s16 no;             /* 0x02 */
    f32 scale[3];       /* 0x04 from the keyframes */
    f32 size;           /* 0x10 */
    f32 alpha;          /* 0x14 */
    f32 fade;           /* 0x18 */
    s16 lag;            /* 0x1C */
    u16 rot;            /* 0x1E */
    PRIM *prim;         /* 0x20 */
    f32 pos[3];         /* 0x24 */
    s16 drot;           /* 0x30 */
    u8 _pad32[2];
} EFT12_PIECE;

typedef struct EFT12_WORK {
    EFT12_PIECE piece[6];   /* 0x000 */
    SMOKE smoke;            /* 0x138 */
} EFT12_WORK;

/* The egg's work: its model matrix and kind. */
typedef struct EGG {
    FLMAT mat;          /* 0x00 */
    u8 _pad40[0xC];
    u8 kind;            /* 0x4C */
} EGG;

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

extern EFT_MDLW *eft_mdlw[5];
extern FLMAT rview_mat;
extern FLMAT rview_matY;
extern PLW player_work[];
extern void *eft12_data[2];
extern s16 rotz77[4];
extern s16 meat_dmg_tbl[4];
extern f32 scale77[4];
extern f32 eft12_st21_set_pos[1][4];
extern u8 eft12_smoke_col[3][4];
extern u8 eft12_bone_col[4][4];
extern u8 eft12_meat_col[4][4];
extern u8 eft12_sp_bone_col[4][4];
extern u8 eft12_sp_meat_col[4][4];
extern s8 enmaku_flag;
extern u8 enmaku_alpha;

u32 ran_suu(int);
u8 Pl_stg_ck(PLW *);
int Pl_master_ck(PLW *);
int frame_check2(PLW *, int, f32);
s16 get_prim2(void);
PRIM *get_prim_ptr2(s16);
void release_prim(s16);
void release_prim2(s16);
void get_joint_pos(PLW *, int, f32 *);
FLMAT *get_joint_wmat(PLW *, int);
void flvecCopy(f32 *, f32 *);
void flvecRotY(f32 *, f32);
void flvecApplyMat33(f32 *, f32 *, FLMAT *);
void flvecApplyMat33_2(f32 *, FLMAT *);
f32 flvecInnerProduct(f32 *, f32 *);
void flmatCopy(FLMAT *, FLMAT *);
void flmatGetTrans(f32 *, FLMAT *);
void flmatRotX33(FLMAT *, f32);
void flmatRotZ33(FLMAT *, f32);
void flmatRotXYZ33(FLMAT *, f32, f32, f32);
f32 flArcTan2(f32, f32);
void flExecuteClay(s32, int);
void SetFilterMode(int);
void Eft_rendope_set(int);
void Material_set_sub(void *, CLAY *);
void PointToPoint(f32 *, f32 *, f32 *);
void SetVector(f32 *, f32, f32, f32);
void make_mat_srt(f32 *, f32 *, f32 *, u16, FLMAT *);
void eft_vec_linear(f32, void *, f32 *);
void eft_alpha_linear(f32, void *, f32 *);
void push_smoke(SMOKE *);
void pull_smoke(SMOKE *);
void push_smell(SMOKE *);
void pull_smell(SMOKE *);
void se_req2(int, int, int, f32 *, int, int);
void eft14_set(f32 *, s16, f32);
void Shell09_set(f32 *, int, int);

static void eft12_move(EFTW *ew);
static void eft12_i(EFTW *ew);
static void eft12_m(EFTW *ew);
static void eft12_t(PRIM *pr);
static void eft12_i00(EFTW *ew);
static void eft12_m00(EFTW *ew);
static void eft12_t00(EFTW *ew, PRIM *pr);
static void eft12_i01(EFTW *ew);
static void eft12_m01(EFTW *ew);
static void eft12_t01(EFTW *ew, PRIM *pr);
static void eft12_i02(EFTW *ew);
static void eft12_m02(EFTW *ew);
static void eft12_t02(EFTW *ew, PRIM *pr);
static void eft12_egg_pos_calc(f32 *out, FLMAT *m, f32 *v);
static void eft12_i03(EFTW *ew);
static void eft12_m03(EFTW *ew);
static void eft12_t03(EFTW *ew, PRIM *pr);
static void eft12_i04(EFTW *ew);
static void eft12_m04(EFTW *ew);
static void eft12_t04(EFTW *ew, PRIM *pr);
static void eft12_d(EFTW *ew);
static void eft12_e(EFTW *ew);
static void eft12_se_req(EFTW *ew, s16 kind);
static EFTW *eft12_pull_work(s16 arg);
void Eft12_set3(f32 *pos, s16 ang, s16 arg, int stg);

static void eft12_move(EFTW *ew) {
    switch (ew->mode) {
    case 0:
        eft12_i(ew);
        break;
    case 1:
        eft12_m(ew);
        break;
    case 2:
        eft12_d(ew);
        break;
    case 3:
        eft12_e(ew);
        break;
    }
}

static void eft12_i(EFTW *ew) {
    ew->mode++;
    ew->mode2 = 0;
    ew->be_flag = 1;
    switch (ew->arg) {
    case 0:
        eft12_i00(ew);
        break;
    case 1:
        eft12_i01(ew);
        break;
    case 2:
    case 3:
        eft12_i02(ew);
        break;
    case 4:
        eft12_i03(ew);
        break;
    case 5:
        eft12_i04(ew);
        break;
    }
}

static void eft12_m(EFTW *ew) {
    switch (ew->arg) {
    case 0:
        eft12_m00(ew);
        break;
    case 1:
        eft12_m01(ew);
        break;
    case 2:
    case 3:
        eft12_m02(ew);
        break;
    case 4:
        eft12_m03(ew);
        break;
    case 5:
        eft12_m04(ew);
        break;
    }
}

static void eft12_t(PRIM *pr) {
    EFTW *ew = pr->owner;

    switch (ew->arg) {
    case 0:
        eft12_t00(ew, pr);
        break;
    case 1:
        eft12_t01(ew, pr);
        break;
    case 2:
    case 3:
        eft12_t02(ew, pr);
        break;
    case 4:
        eft12_t03(ew, pr);
        break;
    case 5:
        eft12_t04(ew, pr);
        break;
    }
}

static void eft12_i00(EFTW *ew) {
    PLW *pl = (PLW *)ew->owner;

    ew->work14 = 0;
    ew->timer = 0;
    get_joint_pos(pl, 0x12, ew->pos);
    ew->pos[1] = pl->x5AC;
    ew->u0A.ang = pl->ang[1];
    ew->scale = 1.75f;
    ew->pos[1] += 13.0f * ew->scale;
    ew->prim_no = get_prim();
    if (ew->prim_no != -1) {
        ew->prim = get_prim_ptr(ew->prim_no);
        ew->prim->owner = ew;
        ew->prim->trans = eft12_t;
    } else {
        push_eft_work(ew);
        return;
    }
    eft12_m(ew);
}

static void eft12_m00(EFTW *ew) {
    if (ew->timer++ >= 300) {
        ew->mode++;
        ew->be_flag = 0;
    } else {
        ew->prim->pos[0] = ew->pos[0];
        ew->prim->pos[1] = ew->pos[1];
        ew->prim->pos[2] = ew->pos[2];
        add_prim(ot1, ew->prim, 0x20, 0);
    }
}

static void eft12_t00(EFTW *ew, PRIM *pr) {
    FLMAT m;
    EFT_MDLW *mw = eft_mdlw[0];
    MATERIAL *mats;
    CLAY *cl;
    MATERIAL *mt;
    int i;

    if (mw != 0 && mw->flag != 0) {
        SetFilterMode(0);
        flmatMakeScale(&m, ew->scale, ew->scale, ew->scale);
        flmatSetTrans(&m, ew->pos[0], 30.0f + ew->pos[1], ew->pos[2]);
        flmatRotY33(&m, DEG2RAD(ANG2DEG(ew->u0A.ang)));
        flSetRenderState(0x1A, (u32)&m);
        cl = &mw->clay[58];
        flSetRenderState(0x67, -1);
        if (cl != 0 && cl->handle != -1) {
            mats = mw->mat;
            for (i = 0; i < cl->mat_num; i++) {
                mt = &mats[cl->mat_no[i]];
                switch (i) {
                case 0:
                    mt->col[0] = 0.8862745f;
                    mt->col[1] = 0.92941177f;
                    mt->col[2] = 0.81960785f;
                    break;
                case 1:
                    mt->col[0] = 1.0f;
                    mt->col[1] = 0.8235294f;
                    mt->col[2] = 0.7529412f;
                    break;
                }
                flSetRenderState((u8)(i + 0x3A), (u32)mt);
            }
            clay_attr_set(cl->attr);
            Eft_rendope_set(0x10);
            flExecuteClay(cl->handle, 0);
        }
        clay_attr_reset();
    }
}

static void eft12_i01(EFTW *ew) {
    EFT12_WORK *w = ew->work;
    EFT12_PIECE *p;
    s16 i;

    ew->work14 = 0;
    ew->timer = 0;
    ew->u0A.joint = 0;
    switch (ew->stg) {
    case 0:
        w->smoke.size = 1500.0f;
        w->smoke.x12 = 1;
        break;
    case 1:
        w->smoke.size = 3000.0f;
        w->smoke.x12 = 2;
        break;
    }
    w->smoke.x15 = 3;
    w->smoke.stg = ew->x07;
    SetVector(w->smoke.pos, ew->pos[0], ew->pos[1], ew->pos[2]);
    push_smoke(&w->smoke);
    p = w->piece;
    for (i = 0; i < 4; i++, p++) {
        p->prim_no = get_prim();
        if (p->prim_no != -1) {
            p->no = i + 1;
            p->lag = 0;
            p->rot = ran_suu(1);
            p->prim = get_prim_ptr(p->prim_no);
            p->prim->owner = ew;
            p->prim->no = i;
            p->prim->trans = eft12_t;
        } else {
            p->prim = 0;
        }
    }
    eft12_m(ew);
}

static void eft12_m01(EFTW *ew) {
    f32 cam[3];
    f32 d[3];
    f32 v2[3];
    f32 v[3];
    EFT12_WORK *w = ew->work;
    EFT12_PIECE *p;
    u8 full;
    u8 a;
    s16 i;
    s16 *r;

    ew->u0A.joint++;
    w->smoke.x15 = 3;
    switch (ew->mode2) {
    case 0:
        full = 0;
        if (++ew->timer >= 120) {
            full = 1;
            ew->mode2++;
            ew->timer = 0;
        }
        break;
    case 1:
        full = 1;
        if (++ew->timer >= 450) {
            full = 0;
            ew->mode2++;
            ew->timer = 120;
        }
        break;
    case 2:
        full = 0;
        if (--ew->timer <= 0) {
            ew->mode++;
            ew->be_flag = 0;
            pull_smoke(&w->smoke);
        }
        break;
    }
    if (ew->be_flag == 0) {
        return;
    }
    flmatGetTrans(cam, &rview_mat);
    PointToPoint(d, cam, ew->pos);
    p = w->piece;
    if (flvecInnerProduct(d, d) > 3.61e6f) {
        for (i = 0, r = rotz77; i < 4; i++, p++, r++) {
            p->rot += *r;
            v[0] = 0.0f;
            v[1] = 0.0f;
            v[2] = 400.0f * (f32)i;
            flvecApplyMat33_2(v, &rview_mat);
            if (full == 0) {
                p->alpha = (f32)ew->timer / 120.0f;
            } else {
                p->alpha = 1.0f;
            }
            if (p->prim != 0) {
                p->no = i + 1;
                p->prim->pos[0] = ew->pos[0] + v[0];
                p->prim->pos[1] = ew->pos[1] + v[1];
                p->prim->pos[2] = ew->pos[2] + v[2];
                add_prim(ot0, p->prim, 0x40, 0);
            }
        }
    } else {
        v2[0] = 0.0f;
        v2[1] = 0.0f;
        v2[2] = -1.1f;
        flvecApplyMat33(v, v2, &rview_mat);
        a = full == 0 ? (u32)(2.125f * (f32)ew->timer) : 0xFF;
        if (enmaku_flag == 0) {
            enmaku_alpha = a;
            if (p->prim != 0) {
                p->no = 0;
                flmatGetTrans(p->prim->pos, &rview_mat);
                p->prim->pos[0] += v[0];
                p->prim->pos[1] += v[1];
                p->prim->pos[2] += v[2];
                add_prim(ot0, p->prim, 0x40, 0);
                enmaku_flag = 1;
            }
        } else if (enmaku_alpha < a) {
            enmaku_alpha = a;
        }
    }
}

static void eft12_t01(EFTW *ew, PRIM *pr) {
    FLMAT m;
    FLMAT uv;
    EFT12_PIECE *p = &((EFT12_PIECE *)ew->work)[pr->no];
    MATERIAL *mats;
    EFT_MDLW *mw = eft_mdlw[0];
    CLAY *cl;
    u8 a;

    if (ew->x07 == game_w.stage && mw != 0 && mw->flag != 0) {
        mats = mw->mat;
        flSetRenderState(0x60, 0);
        if (p->no == 0) {
            flmatMakeScale(&m, 1.0f, 1.0f, 1.0f);
            a = enmaku_alpha;
            flmatMakeTrans(&uv, 0.00048828125f * (f32)(int)game_w.x1E, 0.0f, 0.0f);
            flSetRenderState(0x19, (u32)&uv);
            cl = &mw->clay[67];
        } else {
            flmatMakeScale(&m, scale77[pr->no], scale77[pr->no], scale77[pr->no]);
            flmatRotZ33(&m, DEG2RAD(ANG2DEG(p->rot)));
            a = 255.0f * p->alpha;
            flmatMakeTrans(&uv, 0.0f, 0.0f, 0.0f);
            flSetRenderState(0x19, (u32)&uv);
            cl = &mw->clay[77];
        }
        flmatSetTrans(&m, pr->pos[0], pr->pos[1], pr->pos[2]);
        flmatMul33_2(&m, &rview_mat);
        flSetRenderState(0x1A, (u32)&m);
        flSetRenderState(0x67, ((eft12_smoke_col[ew->stg][1] << 8) | ((a << 24) | (eft12_smoke_col[ew->stg][0] << 16))) |
                                   eft12_smoke_col[ew->stg][2]);
        if (cl != 0 && cl->handle != -1) {
            Material_set_sub(mats, cl);
            clay_attr_set(cl->attr);
            flExecuteClay(cl->handle, 0);
        }
        enmaku_flag = 0;
        clay_attr_reset();
    }
}

static void eft12_i02(EFTW *ew) {
    ew->mode2 = 0;
    ew->work14 = 0;
    ew->timer = 0;
    if (ew->arg == 2) {
        ew->pos[0] = eft12_st21_set_pos[ew->stg][0];
        ew->pos[1] = eft12_st21_set_pos[ew->stg][1];
        ew->pos[2] = eft12_st21_set_pos[ew->stg][2];
        ew->u0A.joint = 0.5f + 65536.0f * eft12_st21_set_pos[ew->stg][3] / 6.2831855f;
        ew->prim_no = get_prim();
        ew->stg = 0;
    } else if (ew->arg == 3) {
        ew->prim_no = get_prim2();
    }
    if (ew->prim_no != -1) {
        if (ew->prim2 == 0) {
            ew->prim = get_prim_ptr(ew->prim_no);
        } else {
            ew->prim = get_prim_ptr2(ew->prim_no);
        }
        ew->prim->owner = ew;
        ew->prim->trans = eft12_t;
    } else {
        eft12_e(ew);
        return;
    }
    eft12_m(ew);
}

static void eft12_m02(EFTW *ew) {
    EFT12_WORK *w = ew->work;

    ew->timer++;
    if (ew->arg == 2) {
        ew->mode2 = 1;
    } else {
        if (ew->arg == 3 && (ew->timer > 5400 || w->smoke.done != 0)) {
            ew->mode++;
            ew->be_flag = 0;
            pull_smell(&w->smoke);
            return;
        }
        ew->mode2 = 0;
    }
    ew->prim->pos[0] = ew->pos[0];
    ew->prim->pos[1] = ew->pos[1];
    ew->prim->pos[2] = ew->pos[2];
    add_prim(ot1, ew->prim, 0x20, 0);
}

static void eft12_t02(EFTW *ew, PRIM *pr) {
    f32 v[3];
    f32 d[3];
    f32 jp[3];
    FLMAT m;
    FLMAT jm;
    EFT_MDLW *mw = eft_mdlw[0];
    MATERIAL *mats;
    s16 i;
    PLW *pl = player_work;
    CLAY *cl;
    MATERIAL *mt;
    int j;
    f32 x, y, z;
    u8 br, bg, bb;
    u8 mr, mg, mb;

    if (ew->x07 == game_w.stage && mw != 0 && mw->flag != 0) {
        cl = &mw->clay[58];
        flmatMakeTrans(&m, ew->pos[0], ew->pos[1], ew->pos[2]);
        switch (ew->arg) {
        case 2:
            mr = eft12_meat_col[ew->mode2][0];
            mg = eft12_meat_col[ew->mode2][1];
            mb = eft12_meat_col[ew->mode2][2];
            br = eft12_bone_col[ew->mode2][0];
            bg = eft12_bone_col[ew->mode2][1];
            bb = eft12_bone_col[ew->mode2][2];
            v[0] = -52.0f;
            v[1] = 0.0f;
            v[2] = 0.0f;
            flvecRotY(v, DEG2RAD(ANG2DEG(ew->u0A.joint)));
            x = ew->pos[0] + v[0];
            y = ew->pos[1] + v[1];
            z = ew->pos[2] + v[2];
            for (i = 0; i < 4; i++, pl++) {
                if (pl->be_flag != 0 && pl->x01 != 0 && Pl_stg_ck(pl) != 0 && pl->char0 == 0x323 &&
                    frame_check2(pl, 0, 56.0f)) {
                    d[0] = x - pl->pos[0];
                    d[2] = z - pl->pos[2];
                    if (d[0] * d[0] + d[2] * d[2] < 22500.0f) {
                        v[0] = 12.0f;
                        v[1] = 0.0f;
                        v[2] = 9.0f;
                        flmatCopy(&jm, get_joint_wmat(pl, 0xE));
                        flvecApplyMat33_2(v, &jm);
                        flmatGetTrans(jp, &jm);
                        jp[0] += v[0];
                        jp[1] += v[1];
                        jp[2] += v[2];
                        d[0] = jp[0] - x;
                        d[1] = jp[1] - y;
                        d[2] = jp[2] - z;
                        flvecRotY(d, DEG2RAD(ANG2DEG(-ew->u0A.joint)));
                        ew->stg = (u16)(s32)(0.5f + 65536.0f * flArcTan2(d[2], d[1]) / 6.2831855f) >> 8;
                        break;
                    }
                }
            }
            flmatRotX33(&m, DEG2RAD(ANG2DEG(ew->stg << 8)));
            break;
        case 3:
            mr = eft12_sp_meat_col[ew->stg][0];
            mg = eft12_sp_meat_col[ew->stg][1];
            mb = eft12_sp_meat_col[ew->stg][2];
            br = eft12_sp_bone_col[ew->stg][0];
            bg = eft12_sp_bone_col[ew->stg][1];
            bb = eft12_sp_bone_col[ew->stg][2];
            break;
        }
        flmatRotY33(&m, DEG2RAD(ANG2DEG(ew->u0A.joint)));
        flSetRenderState(0x1A, (u32)&m);
        flSetRenderState(0x67, -1);
        if (cl != 0 && cl->handle != -1) {
            mats = mw->mat;
            for (j = 0; j < cl->mat_num; j++) {
                mt = &mats[cl->mat_no[j]];
                switch (j) {
                case 0:
                    mt->col[0] = br / 255.0f;
                    mt->col[1] = bg / 255.0f;
                    mt->col[2] = bb / 255.0f;
                    break;
                case 1:
                    mt->col[0] = mr / 255.0f;
                    mt->col[1] = mg / 255.0f;
                    mt->col[2] = mb / 255.0f;
                    break;
                }
                flSetRenderState((u8)(j + 0x3A), (u32)mt);
            }
            clay_attr_set(cl->attr);
            flExecuteClay(cl->handle, 0);
        }
        if (ew->arg == 2) {
            v[0] = -52.0f;
            v[1] = 0.0f;
            v[2] = 0.0f;
            flvecRotY(v, DEG2RAD(ANG2DEG(ew->u0A.joint)));
            cl = &mw->clay[60];
            flmatSetTrans(&m, x, y, z);
            flSetRenderState(0x1A, (u32)&m);
            flSetRenderState(0x67, 0xFF727162);
            if (cl != 0 && cl->handle != -1) {
                mats = mw->mat;
                for (j = 0; j < cl->mat_num; j++) {
                    flSetRenderState((u8)(j + 0x3A), (u32)&mats[cl->mat_no[j]]);
                }
                clay_attr_set(cl->attr);
                flExecuteClay(cl->handle, 0);
            }
        }
        clay_attr_reset();
    }
}

static void eft12_egg_pos_calc(f32 *out, FLMAT *m, f32 *v) {
    flvecApplyMat33_2(v, m);
    flmatGetTrans(out, m);
    out[0] += v[0];
    out[1] += v[1];
    out[2] += v[2];
}

static void eft12_i03(EFTW *ew) {
    ew->mode2 = 0;
    ew->stg = 0;
    ew->work14 = 0;
    ew->timer = 0;
    ew->prim_no = get_prim();
    if (ew->prim_no != -1) {
        ew->prim = get_prim_ptr(ew->prim_no);
        ew->prim->owner = ew;
        ew->prim->trans = eft12_t;
        flvecCopy(ew->prim->pos, ew->pos);
        ew->pos[1] = ((PLW *)ew->owner)->x5AC;
    } else {
        push_eft_work(ew);
        return;
    }
    eft12_m(ew);
}

static void eft12_m03(EFTW *ew) {
    EGG *egg = ew->work;

    ew->timer++;
    ew->stg++;
    switch (ew->mode2) {
    case 0:
        ew->prim->pos[1] += -0.72727275f * ew->stg;
        if (ew->prim->pos[1] < ew->pos[1]) {
            switch (egg->kind) {
            case 0:
            case 1:
                ew->prim->pos[1] = ew->pos[1];
                ew->timer = 0;
                ew->mode2++;
                if (ew->x07 == game_w.stage) {
                    se_req2(1, 0x74, 0, ew->prim->pos, 1, 0);
                }
                break;
            case 2:
            case 3:
            case 4:
                ew->mode++;
                ew->be_flag = 0;
                return;
            case 5:
                ew->mode++;
                ew->be_flag = 0;
                if (ew->x07 == game_w.stage) {
                    eft14_set(ew->prim->pos, 0, 1.0f);
                }
                Shell09_set(ew->prim->pos, 0, ew->x07);
                return;
            }
        }
        break;
    case 1:
    default:
        if (ew->timer > 10) {
            ew->mode++;
            ew->be_flag = 0;
            return;
        }
        break;
    }
    add_prim(ot1, ew->prim, 0x20, 0);
}

static void eft12_t03(EFTW *ew, PRIM *pr) {
    FLMAT m;
    FLMAT uv;
    EFT_MDLW *mw = eft_mdlw[0];
    EGG *egg = ew->work;
    CLAY *cl;
    MATERIAL *mats;
    MATERIAL *mt;
    int j;
    u8 r, g, b;

    if (ew->x07 == game_w.stage && mw != 0 && mw->flag != 0) {
        switch (ew->mode2) {
        case 0:
            flmatSetTrans(&egg->mat, pr->pos[0], pr->pos[1], pr->pos[2]);
            flmatCopy(&m, &egg->mat);
            switch (egg->kind) {
            case 0:
            case 1:
                cl = &mw->clay[133];
                flmatMakeTrans(&uv, 0.0f, 0.0625f, 0.0f);
                flSetRenderState(0x19, (u32)&uv);
                r = 0xFF;
                g = r;
                b = r;
                break;
            case 2:
                cl = &mw->clay[133];
                flmatMakeTrans(&uv, 0.0f, 0.0f, 0.0f);
                flSetRenderState(0x19, (u32)&uv);
                r = 0xFF;
                g = r;
                b = r;
                break;
            case 3:
                cl = &mw->clay[41];
                r = 0x3F;
                b = 0xFF;
                g = r;
                break;
            case 4:
                cl = &mw->clay[41];
                g = 0x3F;
                r = 0xFF;
                b = g;
                break;
            case 5:
                cl = &mw->clay[41];
                r = 0x3F;
                g = r;
                b = r;
                break;
            }
            break;
        case 1:
        default:
            flmatMakeScale(&m, 1.5f, 1.5f, 1.5f);
            flmatMul33_2(&m, &rview_matY);
            cl = &mw->clay[119];
            flmatSetTrans(&m, pr->pos[0], pr->pos[1], pr->pos[2]);
            r = 0xEE;
            g = 0xDC;
            b = 0xA8;
            break;
        }
        flSetRenderState(0x1A, (u32)&m);
        flSetRenderState(0x67, -1);
        if (cl != 0 && cl->handle != -1) {
            mats = mw->mat;
            for (j = 0; j < cl->mat_num; j++) {
                mt = &mats[cl->mat_no[j]];
                switch (j) {
                case 1:
                    mt->col[0] = r / 255.0f;
                    mt->col[1] = g / 255.0f;
                    mt->col[2] = b / 255.0f;
                    break;
                }
                flSetRenderState((u8)(j + 0x3A), (u32)mt);
            }
            clay_attr_set(cl->attr);
            flExecuteClay(cl->handle, 0);
        }
        clay_attr_reset();
    }
}

static void eft12_i04(EFTW *ew) {
    EFT12_PIECE *p = ew->work;
    s16 i;

    ew->work14 = 0;
    ew->timer = 0;
    ew->u0A.joint = 0;
    eft12_se_req(ew, 0);
    for (i = 0; i < 6; i++, p++) {
        p->prim_no = get_prim();
        if (p->prim_no != -1) {
            p->no = i;
            p->lag = -i * 5;
            p->rot = ran_suu(1);
            flvecCopy(p->pos, ew->pos);
            p->prim = get_prim_ptr(p->prim_no);
            p->prim->owner = ew;
            p->prim->no = i;
            p->prim->trans = eft12_t;
        } else {
            p->prim = 0;
        }
    }
    eft12_m(ew);
}

static void eft12_m04(EFTW *ew) {
    EFT12_PIECE *p = ew->work;
    s16 i;

    if (++ew->timer > 126) {
        ew->mode++;
        ew->be_flag = 0;
        return;
    }
    if (ew->timer < 90 && ew->timer % 20 == 0) {
        eft12_se_req(ew, 1);
    }
    for (i = 0; i < 6; i++) {
        if (++p->lag <= 0) {
            p++;
            continue;
        }
        if (p->lag == 1) {
            if (ew->timer < 50) {
                p->size = 4.0f + 4.0f * ((f32)ew->timer / 50.0f);
            } else {
                p->size = 8.0f;
            }
            if (ew->timer < 90) {
                p->fade = 1.0f;
            } else {
                p->fade = 1.0f - (f32)(ew->timer - 90) / 15.0f;
                if (p->fade < 0.0f) {
                    p->fade = 0.0f;
                }
            }
            p->rot = ran_suu(1);
            p->drot = 0.5f + 65536.0f * (0.002f * (f32)(((u16)ran_suu(1) & 0x3FF) - 0x200)) / 360.0f;
            flvecCopy(p->pos, ew->pos);
        } else if (p->lag > 21) {
            if (ew->timer < 105) {
                p->lag = -9;
                p->no += 6;
            }
            p++;
            continue;
        }
        eft_vec_linear(p->lag, eft12_data[0], p->scale);
        eft_alpha_linear(p->lag, eft12_data[1], &p->alpha);
        p->alpha *= p->fade;
        p->pos[1] += 40.0f * p->size / 21.0f;
        p->rot += p->drot;
        if (p->prim != 0) {
            flvecCopy(p->prim->pos, p->pos);
            add_prim(ot0, p->prim, 0x40, 0);
        }
        p++;
    }
}

static void eft12_t04(EFTW *ew, PRIM *pr) {
    f32 sc[3];
    f32 rot[3];
    FLMAT m;
    EFT12_PIECE *p = &((EFT12_PIECE *)ew->work)[pr->no];
    EFT_MDLW *mw = eft_mdlw[0];
    MATERIAL *mats;
    CLAY *cl;
    u8 a;

    if (mw != 0 && mw->flag != 0) {
        mats = mw->mat;
        flSetRenderState(0x60, 0);
        flSetRenderState(0x6C, 0);
        sc[0] = p->size * p->scale[0];
        sc[1] = p->size * p->scale[1];
        sc[2] = p->size * p->scale[2];
        rot[2] = DEG2RAD(ANG2DEG(p->rot));
        make_mat_srt(sc, rot, pr->pos, 2, &m);
        a = 255.0f * p->alpha;
        cl = &mw->clay[72];
        flmatMul33_2(&m, &rview_mat);
        flSetRenderState(0x1A, (u32)&m);
        flSetRenderState(0x67, ((eft12_smoke_col[ew->stg][1] << 8) | ((a << 24) | (eft12_smoke_col[ew->stg][0] << 16))) |
                                   eft12_smoke_col[ew->stg][2]);
        if (cl != 0 && cl->handle != -1) {
            Material_set_sub(mats, cl);
            clay_attr_set(cl->attr);
            flExecuteClay(cl->handle, 0);
        }
        clay_attr_reset();
        flSetRenderState(0x6C, 1);
    }
}

static void eft12_d(EFTW *ew) {
    EFT12_PIECE *p = ew->work;
    s16 n;
    s16 i;

    ew->mode++;
    ew->be_flag = 0;
    switch (ew->arg) {
    default:
        release_prim(ew->prim_no);
        return;
    case 1:
        n = 4;
        break;
    case 3:
        release_prim2(ew->prim_no);
        return;
    case 5:
        n = 6;
        break;
    }
    for (i = 0; i < n; i++, p++) {
        if (p->prim != 0) {
            release_prim(p->prim_no);
        }
    }
}

static void eft12_e(EFTW *ew) {
    if (ew->arg == 3 && ew->x1E == game_w.master) {
        game_w.meat_num--;
    }
    push_eft_work(ew);
}

static void eft12_se_req(EFTW *ew, s16 kind) {
    switch (ew->arg) {
    case 5:
        switch (kind) {
        case 0:
            se_req2(1, 0x26, 0, ew->pos, 1, 0);
            break;
        case 1:
            se_req2(1, 0x27, 0, ew->pos, 1, 0);
            break;
        }
        break;
    }
}

static EFTW *eft12_pull_work(s16 arg) {
    switch (arg) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
        return pull_eft_work(1);
    default:
        return pull_eft_work(0);
    }
}

void eft12_set(PLW *pl, s16 arg) {
    FLMAT jm;
    f32 v[3];
    EFTW *ew;
    EGG *egg;
    f32 sx, sy, sz;
    f32 rx, ry, rz;

    if (arg == 3 || Pl_stg_ck(pl) != 0) {
        ew = eft12_pull_work(arg);
        if (ew != 0) {
            ew->type = 0xC;
            ew->move = eft12_move;
            ew->arg = arg;
            ew->owner = (EMW *)pl;
            ew->x07 = pl->stg;
            ew->u0A.ang = pl->ang[1];
            switch (arg) {
            case 3:
                flvecCopy(ew->pos, pl->pos);
                ew->stg = 0;
                v[0] = 0.0f;
                v[1] = 13.0f;
                v[2] = 50.0f;
                flvecRotY(v, DEG2RAD(ANG2DEG(ew->u0A.joint)));
                ew->pos[0] += v[0];
                ew->pos[1] += v[1];
                ew->pos[2] += v[2];
                ew->prim2 = 1;
                ew->x1E = ((PLW *)ew->owner)->id;
                if (Pl_master_ck(pl)) {
                    game_w.meat_num++;
                }
                break;
            case 4:
                egg = ew->work;
                egg->kind = ((u8)pl->work56B & 0xF0) >> 4;
                switch (egg->kind) {
                case 0:
                case 1:
                    sx = 1.0f;
                    sy = 1.3f;
                    sz = 1.0f;
                    rx = -0.9424779f;
                    ry = -3.281219f;
                    rz = 2.0594885f;
                    v[0] = 5.5f;
                    v[1] = -28.0f;
                    v[2] = 12.0f;
                    break;
                case 2:
                    sx = 1.0f;
                    sy = 1.0f;
                    sz = 1.0f;
                    rx = -0.9424779f;
                    ry = -3.281219f;
                    rz = 2.0594885f;
                    v[0] = 8.9f;
                    v[1] = -25.0f;
                    v[2] = 4.6f;
                    break;
                case 3:
                case 4:
                case 5:
                    sx = 2.5f;
                    sy = 3.0f;
                    sz = 2.0f;
                    rx = -0.24434611f;
                    ry = 2.86234f;
                    rz = 1.2566371f;
                    v[0] = 3.3f;
                    v[1] = -24.0f;
                    v[2] = 4.0f;
                    break;
                }
                flmatCopy(&jm, get_joint_wmat(pl, 0xE));
                eft12_egg_pos_calc(ew->pos, &jm, v);
                flmatMakeScale(&egg->mat, sx, sy, sz);
                flmatRotXYZ33(&egg->mat, rx, ry, rz);
                flmatMul33_2(&egg->mat, &jm);
                break;
            default:
                flvecCopy(ew->pos, pl->pos);
                break;
            }
        }
    }
}

void eft12_set_sh(SHLW *sh, s16 arg, int stg) {
    EFTW *ew;

    if (sh->stg == game_w.stage) {
        ew = eft12_pull_work(arg);
        if (ew != 0) {
            ew->type = 0xC;
            ew->move = eft12_move;
            ew->arg = arg;
            ew->pos[0] = sh->pos2.x;
            ew->pos[1] = sh->pos2.y;
            ew->pos[2] = sh->pos2.z;
            ew->stg = stg;
            ew->x07 = sh->stg;
            if (ew->arg == 1) {
                Eft12_set3(ew->pos, 0, 5, ew->stg);
            }
        }
    }
}

void Eft12_set3(f32 *pos, s16 ang, s16 arg, int stg) {
    EFTW *ew = eft12_pull_work(arg);

    if (ew != 0) {
        ew->type = 0xC;
        ew->move = eft12_move;
        ew->arg = arg;
        ew->stg = stg;
        ew->u0A.joint = ang;
        flvecCopy(ew->pos, pos);
    }
}

void Eft12_set4(PLW *pl, s16 arg, int stg) {
    f32 v[3];
    EFTW *ew;
    EFT12_WORK *w;

    ew = eft12_pull_work(arg);
    if (ew != 0) {
        ew->type = 0xC;
        ew->move = eft12_move;
        ew->arg = arg;
        ew->owner = (EMW *)pl;
        ew->stg = stg;
        ew->x07 = pl->stg;
        ew->u0A.ang = pl->ang[1];
        flvecCopy(ew->pos, pl->pos);
        v[0] = 0.0f;
        v[1] = 13.0f;
        v[2] = 50.0f;
        flvecRotY(v, DEG2RAD(ANG2DEG(ew->u0A.joint)));
        ew->pos[0] += v[0];
        ew->pos[1] += v[1];
        ew->pos[2] += v[2];
        ew->prim2 = 1;
        if (arg == 3) {
            ew->x1E = ((PLW *)ew->owner)->id;
            if (Pl_master_ck(pl)) {
                game_w.meat_num++;
            }
            w = ew->work;
            w->smoke.size = 5000.0f;
            SetVector(w->smoke.pos, ew->pos[0], ew->pos[1], ew->pos[2]);
            w->smoke.pl_id = ((PLW *)ew->owner)->id;
            w->smoke.x11 = game_w.x80[((PLW *)ew->owner)->id];
            w->smoke.done = 0;
            w->smoke.x12 = ew->stg;
            w->smoke.dmg = meat_dmg_tbl[ew->stg];
            w->smoke.stg = ((PLW *)ew->owner)->stg;
            push_smell(&w->smoke);
        }
    }
}
