/* eft16 - game.bin 0x0054F7B0-0x00550F68: eft16_d to Eft16_set_impact.
 * See eft16.c. */
#include "eft.h"
#include "em.h"
#include "pl.h"
#include "prim.h"
#include "clay.h"
#include "fl.h"

typedef struct EFT_MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x0F];
    void *mat;          /* 0x10 material table */
    u8 _pad14[0x1C];
    CLAY *clay;         /* 0x30 */
} EFT_MDLW;

/* One sprite (0x34 bytes) of the work area. */
typedef struct EFT16_PIECE {
    s16 prim_no;        /* 0x00 */
    s16 lag;            /* 0x02 frame counter, starts at 0 or below */
    f32 pos[3];         /* 0x04 position (type 0: velocity) */
    f32 scale[3];       /* 0x10 from the keyframes */
    s16 no;             /* 0x1C sub-kind; 0xFF = off */
    u16 rot;            /* 0x1E */
    u16 alpha;          /* 0x20 */
    u8 _pad22[2];
    f32 size;           /* 0x24 */
    f32 grav;           /* 0x28 type 0: fall speed */
    PRIM *prim;         /* 0x2C */
    s16 uv;             /* 0x30 texture frame */
} EFT16_PIECE;

/* Neck table entry: offsets for Eft16_set_ex and directions for eft16_m. */
typedef struct EFT16_NECK {
    f32 *ofs;           /* 0x00 two vectors: type 3, type 4 */
    f32 *dir;           /* 0x04 two vectors: type 3, type 4 */
} EFT16_NECK;

/* Character fields not in the shared headers yet. */
#define CHR_ANG3EC(c) (*(u16 *)((u8 *)(c) + 0x3EC))
#define CHR_X4D8(c) (*(u8 **)((u8 *)(c) + 0x4D8))

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

extern EFT_MDLW *eft_mdlw[5];
extern FLMAT rview_mat;
extern void *eft16_data[];
extern s16 eft16_num[];
extern s16 eft16_all_time[];
extern s16 eft16_param[];
extern s16 eft16_index[];
extern s16 eft16_time[];
extern f32 eft16_scale_tbl[16];
extern EFT16_NECK *eft16_neck_tbl[];
extern s16 eft16_time_tbl1[][2];
extern s16 eft16_time_tbl2[][2];
extern s16 eft16_time_tbl3[2][2];
extern s16 uv78_00647180[];
extern u8 Eft_blood_rgb[][4];

u32 ran_suu(int);
u8 Pl_stg_ck(PLW *);
u8 Em_stg_ck(EMW *);
void release_prim(s16);
void flvecCopy(f32 *, f32 *);
void flvecRotY(f32 *, f32);
void flvecApplyMat33(f32 *, f32 *, FLMAT *);
void flvecApplyMat33_2(f32 *, FLMAT *);
void flmatCopy(FLMAT *, FLMAT *);
void flmatGetTrans(f32 *, FLMAT *);
void flmatInvert(FLMAT *, FLMAT *);
void flmatRotX33(FLMAT *, f32);
f32 flSqrt(f32);
f32 flArcTan2(f32, f32);
FLMAT *get_joint_wmat(void *, int);
void eft_vec_linear(f32, void *, f32 *);
void eft_alpha_linear(f32, void *, f32 *);
void make_mat_srt(f32 *, f32 *, f32 *, u16, FLMAT *);
void eft_trans_sub_col(CLAY *, FLMAT *, u32, u16, void *);
void SetTrnslMode(int, int);
void Eft02_set3(EMW *, u16, int, int, f32 *, f32);
void Eft02_set4(u16, u16, int, f32 *, f32);
void se_req2(int, int, int, f32 *, int, int);

void eft16_move(EFTW *ew);
void eft16_i(EFTW *ew);
void eft16_m(EFTW *ew);
void eft16_d(EFTW *ew);
void eft16_e(EFTW *ew);
void eft16_t(PRIM *pr);
s16 eft16_rot(s16 no);
s16 eft16_col_type_sel(u8 flag);
void eft16_se_req(EFTW *ew, f32 *pos);

void eft16_d(EFTW *ew) {
    s16 n;
    s16 i;
    EFT16_PIECE *p = ew->work;

    ew->mode++;
    n = eft16_num[ew->arg];
    for (i = 0; i < n; i++, p++) {
        if (p->prim != 0) {
            release_prim(p->prim_no);
        }
    }
}

void eft16_e(EFTW *ew) {
    push_eft_work(ew);
}

void eft16_t(PRIM *pr) {
    f32 rot[3];
    f32 sc[3];
    FLMAT m;
    FLMAT uv;
    EFTW *ew = pr->owner;
    EFT16_PIECE *p = &((EFT16_PIECE *)ew->work)[pr->no];
    EFT_MDLW *mw = eft_mdlw[0];
    void *mats;
    CLAY *cl;
    u16 flag = 0;
    s16 c;
    f32 t;
    u8 r;
    u8 g;
    u8 b;

    if (mw != 0 && mw->flag != 0) {
        r = 0xFF;
        g = 0xFF;
        mats = mw->mat;
        b = 0xFF;
        switch (ew->arg) {
        case 0:
            rot[2] = DEG2RAD(ANG2DEG(p->rot));
            sc[0] = p->scale[0] * (p->size * eft16_scale_tbl[ew->mode2 & 0xF]);
            sc[1] = p->scale[1] * (p->size * eft16_scale_tbl[ew->mode2 & 0xF]);
            sc[2] = p->scale[2] * (p->size * eft16_scale_tbl[ew->mode2 & 0xF]);
            cl = &mw->clay[97];
            make_mat_srt(sc, rot, pr->pos, 2, &m);
            flmatMakeTrans(&uv, 0.125f * (f32)(p->uv & 1), 0.125f * (f32)(p->uv >> 1), 0.0f);
            flSetRenderState(0x19, (u32)&uv);
            c = eft16_col_type_sel(ew->mode2);
            r = Eft_blood_rgb[c][0];
            g = Eft_blood_rgb[c][1];
            b = Eft_blood_rgb[c][2];
            if (p->lag >= 8) {
                t = (f32)(p->lag - 8) / 8.0f;
                r = (u8)(r - (u8)((s32)((u32)r >> 1) * t));
                g = (u8)(g - (u8)((s32)((u32)g >> 1) * t));
                b = (u8)(b - (u8)((s32)((u32)b >> 1) * t));
            }
            break;
        case 2:
            sc[0] = p->size * p->scale[0];
            sc[1] = p->size * p->scale[1];
            sc[2] = p->size * p->scale[2];
            if (pr->no == 0) {
                if ((ew->mode2 & 0x30) == 0x20) {
                    return;
                }
                rot[2] = DEG2RAD(ANG2DEG(ew->u0A.joint));
                cl = mw->clay;
            } else {
                rot[2] = DEG2RAD(ANG2DEG(p->rot));
                cl = &mw->clay[p->no] + 32;
            }
            make_mat_srt(sc, rot, pr->pos, 2, &m);
            if (ew->stg == 1) {
                r = 0xFF;
                g = 0xFF;
                flag |= 2;
                b = 0xFF;
                if (pr->no == 0) {
                    flmatMakeTrans(&uv, 0.125f, 0.0f, 0.0f);
                } else {
                    flmatMakeTrans(&uv, 0.25f, 0.0f, 0.0f);
                }
            } else {
                c = eft16_col_type_sel(ew->mode2);
                r = Eft_blood_rgb[c][0];
                g = Eft_blood_rgb[c][1];
                b = Eft_blood_rgb[c][2];
                flmatMakeTrans(&uv, 0.0f, 0.0f, 0.0f);
            }
            flSetRenderState(0x19, (u32)&uv);
            break;
        case 5:
            sc[0] = p->size * p->scale[0];
            sc[1] = p->size * p->scale[1];
            sc[2] = p->size * p->scale[2];
            rot[2] = DEG2RAD(ANG2DEG(p->rot));
            if (p->no == 0) {
                cl = mw->clay;
            } else {
                cl = &mw->clay[p->no] + 31;
            }
            make_mat_srt(sc, rot, pr->pos, 2, &m);
            if (ew->stg == 1) {
                r = 0xFF;
                flag |= 2;
                g = 0xFF;
                b = 0xFF;
                if (p->no == 0) {
                    flmatMakeTrans(&uv, 0.125f, 0.0f, 0.0f);
                } else {
                    flmatMakeTrans(&uv, 0.25f, 0.0f, 0.0f);
                }
            } else {
                c = eft16_col_type_sel(ew->mode2);
                r = Eft_blood_rgb[c][0];
                g = Eft_blood_rgb[c][1];
                b = Eft_blood_rgb[c][2];
                flmatMakeTrans(&uv, 0.0f, 0.0f, 0.0f);
            }
            flSetRenderState(0x19, (u32)&uv);
            break;
        case 6:
            flag |= 2;
            sc[0] = p->size * p->scale[0];
            sc[1] = p->size * p->scale[1];
            sc[2] = p->size * p->scale[2];
            if (pr->no == 0) {
                rot[2] = DEG2RAD(ANG2DEG(p->rot));
                cl = &mw->clay[p->no] + 32;
                make_mat_srt(sc, rot, pr->pos, 2, &m);
                flmatMakeTrans(&uv, 0.25f, 0.0f, 0.0f);
                flSetRenderState(0x19, (u32)&uv);
            } else {
                cl = &mw->clay[21];
                make_mat_srt(sc, rot, pr->pos, 0, &m);
            }
            break;
        case 8:
            flag |= 2;
            sc[0] = 0.5f * p->size * p->scale[0];
            sc[1] = 0.5f * p->size * p->scale[1];
            sc[2] = 0.5f * p->size * p->scale[2];
            rot[2] = DEG2RAD(ANG2DEG(p->rot));
            cl = &mw->clay[78];
            make_mat_srt(sc, rot, pr->pos, 2, &m);
            flmatMakeTrans(&uv, 0.25f * (f32)(p->uv & 3), 0.25f * (f32)(p->uv >> 2), 0.0f);
            flSetRenderState(0x19, (u32)&uv);
            break;
        case 9:
        case 12:
        case 13:
            flSetRenderState(0x6C, 0);
            if (ew->arg == 9) {
                sc[0] = 0.5f * p->size * p->scale[0];
                sc[1] = 0.5f * p->size * p->scale[1];
                sc[2] = 0.5f * p->size * p->scale[2];
            } else if (ew->arg == 10 || ew->arg == 13) {
                sc[0] = 0.2f * p->size * p->scale[0];
                sc[1] = 0.2f * p->size * p->scale[1];
                sc[2] = 0.2f * p->size * p->scale[2];
            } else if (ew->arg == 12) {
                sc[0] = 2.0f * p->size * p->scale[0];
                sc[1] = 2.0f * p->size * p->scale[1];
                sc[2] = 2.0f * p->size * p->scale[2];
            }
            rot[2] = DEG2RAD(ANG2DEG(p->rot));
            if (ew->arg == 0xC) {
                cl = &mw->clay[74];
            } else {
                if (!(p->no & 1)) {
                    cl = &mw->clay[77];
                    flmatMakeTrans(&uv, 0.0f, 0.25f, 0.0f);
                } else {
                    cl = &mw->clay[79];
                    flmatMakeTrans(&uv, 0.0f, 0.0f, 0.0f);
                }
                flSetRenderState(0x19, (u32)&uv);
            }
            make_mat_srt(sc, rot, pr->pos, 2, &m);
            break;
        case 14:
            sc[0] = p->size;
            sc[1] = p->size;
            sc[2] = p->size;
            rot[2] = DEG2RAD(ANG2DEG(p->rot));
            cl = &mw->clay[97];
            make_mat_srt(sc, rot, pr->pos, 2, &m);
            flmatRotX33(&m, 4.712389f);
            flmatMakeTrans(&uv, 0.125f * (f32)(p->uv & 1), 0.125f * (f32)(p->uv >> 1), 0.0f);
            flSetRenderState(0x19, (u32)&uv);
            r = Eft_blood_rgb[0][0];
            g = Eft_blood_rgb[0][1];
            b = Eft_blood_rgb[0][2];
            break;
        }
        if (ew->arg != 0xE) {
            flmatMul33_2(&m, &rview_mat);
        }
        eft_trans_sub_col(cl, &m, (p->alpha << 24) | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF), flag, mats);
        SetTrnslMode(4, 5);
        flSetRenderState(0x6C, 1);
    }
}

s16 eft16_rot(s16 no) {
    switch (no & 6) {
    case 0:
        return 0x111;
    case 2:
        return -0x110;
    case 4:
        return 0x89;
    case 6:
        return -0x88;
    }
    return 0;
}

s16 eft16_col_type_sel(u8 flag) {
    s16 c;

    switch (flag & 0xC0) {
    case 0x80:
        c = 2;
        break;
    default:
        c = 0;
        break;
    }
    return c;
}

void eft16_se_req(EFTW *ew, f32 *pos) {
    switch (ew->arg) {
    case 0:
        if (eft16_scale_tbl[ew->mode2 & 0xF] <= 0.5f) {
            se_req2(1, 0x6E, 0, pos, 1, 0);
            break;
        }
        switch (ew->mode2 & 0x30) {
        case 0:
            se_req2(1, 0x6E, 0, pos, 1, 0);
            break;
        case 0x10:
            se_req2(1, 0x6F, 0, pos, 1, 0);
            break;
        }
        break;
    case 2:
    case 5:
        switch (ew->stg) {
        case 1:
            switch (ew->mode2 & 0x30) {
            case 0x10:
                se_req2(1, 0x65, 0, pos, 1, 0);
                break;
            case 0:
                se_req2(1, 0x64, 0, pos, 1, 0);
                break;
            }
            break;
        }
        break;
    case 6:
        se_req2(1, 0x63, 0, pos, 1, 0);
        break;
    }
}

void Eft16_set(PLW *pl, int arg, s16 hit, f32 *pos, f32 scale) {
    FLMAT m;
    FLMAT inv;
    f32 t[3];
    EFTW *ew;
    s16 *jl;

    if (pl != 0 && Pl_stg_ck(pl) != 0) {
        ew = pull_eft_work(1);
        if (ew != 0) {
            ew->type = 0x10;
            ew->move = eft16_move;
            ew->arg = arg;
            ew->owner = (EMW *)pl;
            switch (hit) {
            default:
            case 2:
                ew->mode2 = 0;
                break;
            case 3:
                ew->mode2 = 0x10;
                break;
            case 0:
                ew->mode2 = 0x20;
                break;
            }
            ew->scale = scale;
            if (ew->arg == 6) {
                ew->pos[0] = pos[0];
                ew->pos[1] = pos[1];
                ew->pos[2] = pos[2];
                ew->u0A.ang = ran_suu(1);
                ew->x07 = 0xFF;
                return;
            }
            ew->u0A.ang = CHR_ANG3EC(pl) + 0x8000;
            ew->mode2 &= 0x30;
            if (pl->x10 != 0) {
                switch (pl->kind) {
                case 9:
                case 0x17:
                    ew->mode2 |= 2;
                    break;
                case 0x13:
                case 0x18:
                    ew->mode2 |= 0x82;
                    break;
                }
                if (CHR_X4D8(pl) != 0 && (jl = *(s16 **)(CHR_X4D8(pl) + 0xA0)) != 0) {
                    ew->x07 = *jl;
                    flmatCopy(&m, get_joint_wmat(pl, ew->x07));
                    flmatGetTrans(t, &m);
                    ew->pos[0] = pos[0] - t[0];
                    ew->pos[1] = pos[1] - t[1];
                    ew->pos[2] = pos[2] - t[2];
                    flmatInvert(&inv, &m);
                    flvecApplyMat33_2(ew->pos, &inv);
                    return;
                }
            } else {
                ew->mode2 |= 1;
                ew->x07 = 0xA;
                if (ew->arg == 8 || ew->arg == 9) {
                    ew->pos[0] = 0.0f;
                    ew->pos[1] = 0.0f;
                    ew->pos[2] = 0.0f;
                } else {
                    flmatCopy(&m, get_joint_wmat(pl, ew->x07));
                    flmatGetTrans(t, &m);
                    ew->pos[0] = pos[0] - t[0];
                    ew->pos[1] = pos[1] - t[1];
                    ew->pos[2] = pos[2] - t[2];
                    flmatInvert(&inv, &m);
                    flvecApplyMat33_2(ew->pos, &inv);
                }
                return;
            }
            ew->x07 = 0xFF;
            ew->pos[0] = pos[0];
            ew->pos[1] = pos[1];
            ew->pos[2] = pos[2];
        }
    }
}

void Eft16_set_ex(EMW *em, int arg, int joint) {
    EFTW *ew;
    EFT16_NECK *nk;
    f32 *ofs;

    if (Em_stg_ck(em) != 0 && (nk = eft16_neck_tbl[em->kind]) != 0 && (ew = pull_eft_work(0)) != 0) {
        ew->type = 0x10;
        ew->move = eft16_move;
        ew->arg = arg;
        ew->x07 = joint;
        ew->owner = em;
        ofs = nk->ofs;
        if (ew->arg == 3) {
            ew->pos[0] = *ofs++ * em->scale[0];
            ew->pos[1] = *ofs++ * em->scale[1];
            ew->pos[2] = *ofs++ * em->scale[2];
        } else {
            ofs += 3;
            ew->pos[0] = *ofs++ * em->scale[0];
            ew->pos[1] = *ofs++ * em->scale[1];
            ew->pos[2] = *ofs++ * em->scale[2];
        }
    }
}

void Eft16_set_ex3(f32 *pos, int arg, s16 ang, int cnt, f32 scale) {
    EFTW *ew;

    if ((ew = pull_eft_work(1)) != 0) {
        ew->type = 0x10;
        ew->move = eft16_move;
        ew->arg = arg;
        ew->u0A.joint = ang;
        ew->scale = scale;
        ew->stg = cnt;
        ew->x07 = 0xFF;
        ew->pos[0] = pos[0];
        ew->pos[1] = pos[1];
        ew->pos[2] = pos[2];
    }
}

void Eft16_set_impact(PLW *pl, f32 *pos, int arg, s16 hit, s16 wpn, f32 scale) {
    EFTW *ew;

    if (pl != 0 && Pl_stg_ck(pl) != 0) {
        ew = pull_eft_work(1);
        if (ew != 0) {
            ew->type = 0x10;
            ew->move = eft16_move;
            ew->arg = arg;
            ew->owner = (EMW *)pl;
            switch (hit) {
            default:
            case 2:
                ew->mode2 = 0;
                break;
            case 3:
                ew->mode2 = 0x10;
                break;
            case 0:
                ew->mode2 = 0x20;
                break;
            }
            ew->mode2 &= 0x3F;
            switch (wpn) {
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
                ew->stg = 1;
                break;
            default:
                if (pl->x10 != 0 && (pl->kind == 0x13 || pl->kind == 0x18)) {
                    ew->mode2 |= 0x80;
                }
                break;
            }
            ew->scale = scale;
            ew->pos[0] = pos[0];
            ew->pos[1] = pos[1];
            ew->pos[2] = pos[2];
            ew->u0A.ang = ran_suu(1);
            ew->x07 = 0xFF;
        }
    }
}
