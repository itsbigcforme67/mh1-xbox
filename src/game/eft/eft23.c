/* eft23 - game.bin 0x00557480-0x005589EC; fish_type_set is still assembly
 * (see eft23_nm.c), Eft23_set is in eft23b.c. A fish in a fishing spot. It
 * appears (toujyou), wanders inside its range looking for a float
 * (eft22, found by uki_serch), nibbles and bites (atari), and once a
 * player hooks it (turare) it follows the float until landed. The fish
 * kind comes from weighted per-spot tables (fish_type_set). */
#include "eft.h"
#include "game.h"
#include "pl.h"
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

/* The fish (ew->work). */
typedef struct FISH {
    f32 home[3];        /* 0x00 centre of its range */
    f32 scale[3];       /* 0x0C */
    f32 range;          /* 0x18 */
    f32 dist;           /* 0x1C to the float */
    f32 accel;          /* 0x20 */
    s16 bite_time;      /* 0x24 */
    s16 nibbles;        /* 0x26 */
    EFTW *uki;          /* 0x28 the float it is after */
    s32 tgt_ang;        /* 0x2C */
    s32 turn;           /* 0x30 */
    s32 sway;           /* 0x34 */
    s32 rot[3];         /* 0x38 */
    s8 type;            /* 0x44 index into fish_type_data */
    s8 hooked;          /* 0x45 */
} FISH;

typedef struct FISH_DATA {
    u16 catch_time;     /* 0x00 */
    u8 _pad02[2];
    u16 mdl;            /* 0x04 */
    u16 col;            /* 0x06 */
    f32 scale[3];       /* 0x08 */
} FISH_DATA;

typedef struct FISH_CHANCE {
    u16 weight;         /* 0xFFFF ends the list */
    s8 type;
    u8 _pad03;
} FISH_CHANCE;

typedef struct QUEST_W {
    u8 _pad00[0x14E];
    s8 x14E;            /* 0x14E selects the alternate fish tables */
} QUEST_W;

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

extern EFT_MDLW *eft_mdlw[5];
extern EFTW eft_work[128];
extern QUEST_W quest_w;
extern FISH_DATA fish_type_data[];
extern FISH_CHANCE *fish_type_tbl[26];
extern u16 kuituki_time_tbl[4];
extern u8 Eft_fish_rgb[][3];
extern u8 Eft_hire_rgb[][3];

u32 ran_suu(int);
int Pl_master_ck(PLW *);
int act_ck(void *, int, int);
void release_prim(s16);
f32 CalcDistanceXZ(f32 *, f32 *);
u16 Em_Calc_angY(f32 *, f32 *);
void em09_dir_calc(s32 *, s32 *, s32);
void cpRotMatrix(s32 *, FLMAT *);
void PointToPoint(f32 *, f32 *, f32 *);
f32 flvecCalcLength(f32 *);
void flvecNormalize(f32 *);
void flvecCopy(f32 *, f32 *);
void flvecApplyMat33(f32 *, f32 *, FLMAT *);
void flmatRotZXY33(FLMAT *, f32, f32, f32);
void vib_set_pl(void *, int);
f32 Eft22_suimen_ck(EFTW *);
void Eft20_set2(f32, f32 *, int, int);

void eft23_move(EFTW *ew);
static void eft23_i(EFTW *ew);
static EFTW *uki_serch(EFTW *ew, FISH *w, int mode);
static void eft23_toujyou_mv(EFTW *ew, FISH *w);
static void eft23_normal_mv(EFTW *ew, FISH *w);
static void eft23_atari_mv(EFTW *ew, FISH *w);
static void eft23_turare_mv(EFTW *ew, FISH *w);
static void eft23_m(EFTW *ew);
static void eft23_d(EFTW *ew);
static void eft23_e(EFTW *ew);
static void eft23_t(PRIM *pr);
s8 fish_type_set(EFTW *ew);

void eft23_move(EFTW *ew) {
    switch (ew->mode) {
    case 0:
        eft23_i(ew);
        break;
    case 1:
        eft23_m(ew);
        break;
    case 2:
        eft23_d(ew);
        break;
    case 3:
        eft23_e(ew);
        break;
    }
}

static void eft23_i(EFTW *ew) {
    ew->mode++;
    ew->mode2 = 0;
    ew->be_flag = 1;
    ew->work14 = 0;
    ew->prim_no = get_prim();
    if (ew->prim_no != -1) {
        ew->prim = get_prim_ptr(ew->prim_no);
        ew->prim->owner = ew;
        ew->prim->trans = eft23_t;
    } else {
        ew->prim = 0;
    }
    eft23_m(ew);
}

static EFTW *uki_serch(EFTW *ew, FISH *w, int mode) {
    s32 i;
    EFTW *e;
    u16 a;

    for (i = 0, e = eft_work; i < 128; i++, e++) {
        if (e->type == 22 && e->arg != 1 && e->mode2 == 2 && e->x07 == 0 &&
            Pl_master_ck((PLW *)e->owner) == 1 && CalcDistanceXZ(w->home, e->pos) < w->range) {
            a = Em_Calc_angY(ew->pos, e->pos);
            if ((u16)(a - w->rot[1] + 0x3000) < 0x6000) {
                if (mode == 0) {
                    w->tgt_ang = a;
                    return e;
                }
                if ((w->dist = CalcDistanceXZ(ew->pos, e->pos)) < 30.0f) {
                    e->x07 = 1;
                    return e;
                }
            }
        }
    }
    return 0;
}

static void eft23_toujyou_mv(EFTW *ew, FISH *w) {
    FISH_DATA *fd = &fish_type_data[w->type];

    switch (ew->stg) {
    case 0:
        ew->stg++;
        w->hooked = 0;
        w->type = fish_type_set(ew);
        flvecCopy(ew->pos, w->home);
        ew->timer = 10;
        w->rot[2] = 0;
        w->rot[0] = 0;
        w->tgt_ang = w->rot[1] = ((u16)ran_suu(1) & 0xF) << 12;
        w->turn = 0x20;
        w->sway = 0;
        w->scale[0] = w->scale[1] = w->scale[2] = 0.1f;
        w->rot[0] = 0;
        break;
    case 1:
        w->scale[0] += fd->scale[0] / 10.0f;
        w->scale[1] += fd->scale[1] / 10.0f;
        w->scale[2] += fd->scale[2] / 10.0f;
        if (--ew->timer <= 0.0f) {
            ew->mode2 = 1;
            ew->stg = 0;
            ew->timer = ((u16)ran_suu(1) & 0x1F) + 10;
            w->scale[0] = fd->scale[0];
            w->scale[1] = fd->scale[1];
            w->scale[2] = fd->scale[2];
        }
        break;
    }
}

static void eft23_normal_mv(EFTW *ew, FISH *w) {
    f32 v[3];
    f32 out[3];
    FLMAT m;
    s32 a;

    switch (ew->stg) {
    case 0:
        if (--ew->timer <= 0 || w->rot[1] - w->tgt_ang == 0) {
            ew->stg++;
            ew->scale = 3.0f;
            w->sway = 0x100;
            if (uki_serch(ew, w, 0) == 0) {
                a = Em_Calc_angY(ew->pos, w->home);
                w->tgt_ang = a - 0x4000 + (((u16)ran_suu(1) & 0xF) << 11);
            }
        }
        w->turn += (0x20 - w->turn) / 8;
        em09_dir_calc(&w->rot[1], &w->tgt_ang, w->turn);
        break;
    case 1:
        v[0] = 0.0f;
        v[1] = 0.0f;
        v[2] = ew->scale;
        cpRotMatrix(w->rot, &m);
        flvecApplyMat33(out, v, &m);
        ew->pos[0] += out[0];
        ew->pos[1] += out[1];
        ew->pos[2] += out[2];
        w->sway = -w->sway;
        w->sway -= w->sway / 2;
        if ((w->uki = uki_serch(ew, w, 1)) != 0 && ((PLW *)w->uki->owner)->x8EA != 0) {
            ew->mode2 = 2;
            ew->stg = 0;
            ew->timer = 3;
            w->sway = 0;
        } else if ((ew->scale += -0.1f) <= 0.0f) {
            ew->stg = 0;
            ew->timer = ((u16)ran_suu(1) & 0x1F) + 10;
            w->sway = 0;
        }
        w->turn += (0x100 - w->turn) / 8;
        em09_dir_calc(&w->rot[1], &w->tgt_ang, w->turn);
        break;
    }
}

static void eft23_atari_mv(EFTW *ew, FISH *w) {
    f32 v[3];
    f32 out[3];
    FLMAT m;
    EFTW *u = w->uki;
    PLW *pl;

    if (u->type != 22 || u->pad0 == 0 || u->mode > 1 || act_ck(u->owner, 0, 0x64) != 0 ||
        act_ck(u->owner, 0, 0x54) != 0) {
        ew->mode2 = 1;
        ew->stg = 0;
        ew->timer = ((u16)ran_suu(1) & 0x1F) + 10;
        return;
    }
    pl = (PLW *)u->owner;
    if (pl->fish_time == 0 && act_ck(pl, 0, 0x53) != 0) {
        ew->mode2 = 3;
        ew->stg = 0;
        ((PLW *)u->owner)->fish_time = fish_type_data[w->type].catch_time;
        return;
    }
    switch (ew->stg) {
    case 0:
        w->nibbles = (u16)ran_suu(1) & 3;
    case 1:
        if (w->nibbles-- <= 0) {
            ew->stg = 5;
        } else {
            ew->stg++;
        }
        ew->timer = 5;
        ew->scale = CalcDistanceXZ(ew->pos, u->pos) / ew->timer;
        w->tgt_ang = w->rot[1] = Em_Calc_angY(ew->pos, w->uki->pos);
        break;
    case 2:
    case 5:
        v[0] = 0.0f;
        v[1] = 0.0f;
        v[2] = ew->scale;
        cpRotMatrix(w->rot, &m);
        flvecApplyMat33(out, v, &m);
        ew->pos[0] += out[0];
        ew->pos[1] += out[1];
        ew->pos[2] += out[2];
        if (--ew->timer <= 0) {
            if (ew->stg == 5) {
                ((PLW *)u->owner)->x881 = kuituki_time_tbl[(u16)ran_suu(1) & 3];
                w->bite_time = kuituki_time_tbl[(u16)ran_suu(1) & 3];
                if (((u16)ran_suu(1) & 3) == 0) {
                    ((PLW *)u->owner)->x8EA = 0;
                }
                vib_set_pl(u->owner, 1);
            } else {
                v[0] = u->pos[0];
                v[1] = 10.0f + Eft22_suimen_ck(u);
                v[2] = u->pos[2];
                Eft20_set2(0.5f, v, 0x11, ran_suu(1));
                vib_set_pl(u->owner, 4);
            }
            ew->stg++;
            ew->scale = -5.0f;
            w->accel = 0.5f;
        }
        break;
    case 3:
    case 6:
        v[0] = 0.0f;
        v[1] = 0.0f;
        v[2] = ew->scale;
        cpRotMatrix(w->rot, &m);
        flvecApplyMat33(out, v, &m);
        ew->pos[0] += out[0];
        ew->pos[1] += out[1];
        ew->pos[2] += out[2];
        if ((ew->scale += w->accel) >= 0.0f) {
            ew->stg++;
            ew->timer = 20;
        }
        break;
    case 4:
        if (--ew->timer <= 0) {
            ew->stg = 1;
        }
        break;
    case 7:
        if (--ew->timer <= 0) {
            ew->stg++;
            ew->timer = ((u16)ran_suu(1) & 0x1F) + 60;
            w->tgt_ang = w->rot[1] + (((u16)ran_suu(1) & 7) << 12);
            u->x07 = 0;
            ew->scale = 8.0f;
            w->accel = -0.3f;
        }
        break;
    case 8:
        v[0] = 0.0f;
        v[1] = 0.0f;
        v[2] = ew->scale;
        cpRotMatrix(w->rot, &m);
        flvecApplyMat33(out, v, &m);
        ew->pos[0] += out[0];
        ew->pos[1] += out[1];
        ew->pos[2] += out[2];
        if ((ew->scale += w->accel) <= 0.0f) {
            ew->mode2 = 1;
            ew->stg = 0;
            ew->timer = ((u16)ran_suu(1) & 0x1F) + 10;
        }
        break;
    }
    em09_dir_calc(&w->rot[1], &w->tgt_ang, 0x100);
}

static void eft23_turare_mv(EFTW *ew, FISH *w) {
    EFTW *u = w->uki;

    if (u->type != 22 || u->pad0 == 0 || u->mode >= 2) {
        ew->mode2 = 0;
        ew->stg = 0;
        return;
    }
    switch (ew->stg) {
    case 0:
        ew->stg++;
        ew->timer = 10;
    case 1:
        w->tgt_ang = Em_Calc_angY(ew->pos, u->pos);
        em09_dir_calc(&w->rot[1], &w->tgt_ang, 0x800);
        ew->pos[0] = u->pos[0];
        ew->pos[2] = u->pos[2];
        if (--ew->timer <= 0) {
            ew->timer = 10;
            vib_set_pl(u->owner, 1);
        }
        if (u->mode2 > 3) {
            ew->stg++;
            w->hooked = 1;
            vib_set_pl(u->owner, 3);
        }
        break;
    case 2:
        w->rot[0] = -0x4000;
        flvecCopy(ew->pos, u->pos);
        break;
    }
}

static void eft23_m(EFTW *ew) {
    f32 d[3];
    FISH *w = ew->work;

    switch (ew->mode2) {
    case 0:
        eft23_toujyou_mv(ew, w);
        break;
    case 1:
        eft23_normal_mv(ew, w);
        break;
    case 2:
        eft23_atari_mv(ew, w);
        break;
    case 3:
        eft23_turare_mv(ew, w);
        break;
    }
    if (ew->mode2 < 3) {
        PointToPoint(d, ew->pos, w->home);
        d[1] = 0.0f;
        if (flvecCalcLength(d) > w->range) {
            flvecNormalize(d);
            ew->pos[0] = w->home[0] + d[0] * w->range;
            ew->pos[1] = w->home[1] + d[1] * w->range;
            ew->pos[2] = w->home[2] + d[2] * w->range;
        }
    }
    ew->prim->pos[0] = ew->pos[0];
    ew->prim->pos[1] = ew->pos[1];
    ew->prim->pos[2] = ew->pos[2];
    add_prim(ot1, ew->prim, 0x20, 0);
}

static void eft23_d(EFTW *ew) {
    ew->mode++;
    release_prim(ew->prim_no);
}

static void eft23_e(EFTW *ew) {
    push_eft_work(ew);
}

static void eft23_t(PRIM *pr) {
    FLMAT m;
    EFTW *ew = pr->owner;
    EFT_MDLW *mw = eft_mdlw[0];
    FISH *w = ew->work;
    FISH_DATA *fd = &fish_type_data[w->type];
    MATERIAL *mt;
    MATERIAL *mm;
    CLAY *cl;
    s32 i;
    f32 br;

    if (mw == 0 || mw->flag == 0) {
        return;
    }
    flSetRenderState(0x60, 0x80);
    cl = &mw->clay[fd->mdl];
    mt = mw->mat;
    flmatMakeScale(&m, w->scale[0], w->scale[1], w->scale[2]);
    flmatRotZXY33(&m, DEG2RAD(ANG2DEG(w->rot[0])), DEG2RAD(ANG2DEG(w->rot[1] + w->sway)), DEG2RAD(ANG2DEG(w->rot[2])));
    flmatSetTrans(&m, ew->pos[0], ew->pos[1], ew->pos[2]);
    flSetRenderState(0x67, -1);
    if (cl != 0 && cl->handle != -1) {
        flSetRenderState(0x60, 0x80);
        flSetRenderState(0x1A, (u32)&m);
        if (w->hooked == 0) {
            br = 0.5f;
        } else {
            br = 1.0f;
        }
        for (i = 0; i < cl->mat_num; i++) {
            mm = &mt[cl->mat_no[i]];
            switch (i) {
            case 0:
                mm->col[0] = br * (Eft_fish_rgb[fd->col][0] / 255.0f);
                mm->col[1] = br * (Eft_fish_rgb[fd->col][1] / 255.0f);
                mm->col[2] = br * (Eft_fish_rgb[fd->col][2] / 255.0f);
                break;
            case 1:
                mm->col[0] = br * (Eft_hire_rgb[fd->col][0] / 255.0f);
                mm->col[1] = br * (Eft_hire_rgb[fd->col][1] / 255.0f);
                mm->col[2] = br * (Eft_hire_rgb[fd->col][2] / 255.0f);
                break;
            }
            flSetRenderState((u8)(i + 0x3A), (u32)mm);
        }
        clay_attr_set(cl->attr);
        flExecuteClay(cl->handle, 0);
        clay_attr_reset();
    }
}
