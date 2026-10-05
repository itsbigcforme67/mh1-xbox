/* eft13b - SLPM_654.95 0x00106DB0-0x00107C50: eft13_d/e, the draw
 * callback eft13_t, se_req, water_ck, set_sub.
 * Part of eft13 (whole file 0x00105B10-0x00109E28). Dust, splashes and debris with
 * 35 types (arg): up to eft13_num[arg] pieces (0x30 bytes) per effect.
 * eft13_set_pos / eft13_set_pos_em pick the spawn point and type from the
 * owner (player or monster kind); on water (eft13_water_ck) the effect is
 * replaced by Eft08 splashes (game.bin Eft08_set/set2, called by address) (eft13_water_set). The big functions (i, m, t,
 * set_pos, set_sub_em, set_pos_em) are still asm. */
#include "eft.h"
#include "em.h"
#include "game.h"
#include "prim.h"
#include "fl.h"
#include "clay.h"

typedef struct EFT_MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x0F];
    void *mat;          /* 0x10 material table */
    u8 _pad14[0x1C];
    CLAY *clay;         /* 0x30 */
} EFT_MDLW;

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

/* Owner fields used here (player or monster). */
typedef struct EFT13_CHR {
    u8 be_flag;         /* 0x000 */
    u8 x01;             /* 0x001 */
    u8 kind;            /* 0x002 */
    u8 _pad003[0xA0 - 0x03];
    s32 ang[3];         /* 0x0A0 */
    u8 _pad0AC[0xB8 - 0xAC];
    f32 scale[3];       /* 0x0B8 */
    u8 _pad0C4[0x5AC - 0xC4];
    f32 x5AC;           /* 0x5AC ground height */
    u8 _pad5B0[0x909 - 0x5B0];
    u8 x909;            /* 0x909 */
} EFT13_CHR;

/* One piece (0x30 bytes) of the work area. */
typedef struct EFT13_PIECE {
    s16 no;             /* 0x00 */
    u8 _pad02[0x10 - 0x02];
    f32 scale[3];       /* 0x10 from the keyframes */
    f32 size;           /* 0x1C */
    f32 alpha;          /* 0x20 */
    u8 _pad24[2];
    u16 rot;            /* 0x26 */
    u8 _pad28[2];
    s16 prim_no;        /* 0x2A */
    PRIM *prim;         /* 0x2C */
} EFT13_PIECE;

extern s16 eft13_num[35];
extern EFT_MDLW *eft_mdlw[5];
extern FLMAT rview_mat;
extern u8 Eft_kemuri_rgb[11][4];
extern u8 eft13_type26_rgb[8][3];

void make_mat_srt(f32 *, f32 *, f32 *, u16, FLMAT *);
void eft_trans_sub(CLAY *, FLMAT *, u16, f32, void *);
void eft_trans_sub_col(CLAY *, FLMAT *, u32, u16, void *);
void SetFilterMode(int);
extern s16 eft13_water_flag[35];
extern s16 Eft_stg_type[];

u8 Pl_stg_ck(void *);
u8 Em_stg_ck(void *);
void release_prim(s16);
FLMAT *get_joint_wmat(void *, s16);
void flmatCopy(FLMAT *, FLMAT *);
void flvecCopy(f32 *, f32 *);
u16 calc_mat_angY(FLMAT *);
int GetWaterHit(f32 *, f32 *);
void se_req2(int, int, int, f32 *, int, int);
void func_544C90(f32 *, int, int, f32);    /* game.bin Eft08_set */
void func_544D20(void *, int, int, f32, f32); /* game.bin Eft08_set2 */

void eft13_move(EFTW *ew);
void eft13_i(EFTW *ew);
void eft13_m(EFTW *ew);
void eft13_d(EFTW *ew);
void eft13_e(EFTW *ew);
s16 eft13_set_pos(f32 *pos, EFT13_CHR *chr, s16 j, int arg);
s16 eft13_set_pos_em(f32 *pos, EFT13_CHR *chr, s16 j, int arg);
void eft13_set_sub_em(EFT13_CHR *chr, s16 j, int arg, EFTW *ew);
s8 eft13_water_set(EFT13_CHR *chr, f32 *pos, s16 kind, f32 scale);

void eft13_d(EFTW *ew) {
    s16 n;
    s16 i;
    EFT13_PIECE *p = ew->work;

    ew->mode++;
    n = eft13_num[ew->arg];
    for (i = 0; i < n; i++, p++) {
        if (p->prim != 0) {
            release_prim(p->prim_no);
        }
    }
}

void eft13_e(EFTW *ew) {
    push_eft_work(ew);
}

void eft13_t(PRIM *pr) {
    u8 r;
    u8 g;
    u8 b;
    f32 rot[3];
    f32 sc[3];
    FLMAT m;
    EFTW *ew = pr->owner;
    EFT_MDLW *mw = eft_mdlw[0];
    u16 flag = 0;
    s16 t;
    EFT13_PIECE *p = &((EFT13_PIECE *)ew->work)[pr->no];
    f32 alpha;
    void *mats;
    CLAY *cl;

    if (mw != 0 && mw->flag != 0) {
        t = Eft_stg_type[game_w.stage];
        r = Eft_kemuri_rgb[t][0];
        g = Eft_kemuri_rgb[t][1];
        b = Eft_kemuri_rgb[t][2];
        mats = mw->mat;
        alpha = Eft_kemuri_rgb[t][3] / 255.0f;
        switch (ew->arg) {
        case 0:
            cl = &mw->clay[74];
            sc[0] = p->size * p->scale[0];
            sc[1] = p->size * p->scale[1];
            sc[2] = p->size * p->scale[2];
            rot[2] = DEG2RAD(ANG2DEG(p->rot));
            make_mat_srt(sc, rot, pr->pos, 2, &m);
            flmatMul33_2(&m, &rview_mat);
            break;
        case 3:
        case 4:
        case 6:
        case 7:
        case 9:
        case 11:
        case 12:
        case 13:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 23:
        case 24:
        case 25:
        case 27:
        case 31:
        case 32:
        case 33:
        case 34:
            cl = &mw->clay[74];
            sc[0] = p->size * p->scale[0];
            sc[1] = p->size * p->scale[1];
            sc[2] = p->size * p->scale[2];
            rot[2] = DEG2RAD(ANG2DEG(p->rot));
            make_mat_srt(sc, rot, pr->pos, 2, &m);
            flmatMul33_2(&m, &rview_mat);
            break;
        case 5:
        case 10:
            cl = &mw->clay[74];
            sc[0] = p->size * p->scale[0];
            sc[1] = p->size * p->scale[1];
            sc[2] = p->size * p->scale[2];
            rot[2] = DEG2RAD(ANG2DEG(p->rot));
            make_mat_srt(sc, rot, pr->pos, 2, &m);
            flmatMul33_2(&m, &rview_mat);
            break;
        case 8:
            cl = &mw->clay[74];
            sc[0] = 0.8f * p->scale[0];
            sc[1] = 0.8f * p->scale[1];
            sc[2] = 0.8f * p->scale[2];
            rot[2] = DEG2RAD(ANG2DEG(p->rot));
            make_mat_srt(sc, rot, pr->pos, 2, &m);
            flmatMul33_2(&m, &rview_mat);
            break;
        case 14:
            switch (p->no) {
            case 0:
                cl = &mw->clay[74];
                r = 0xFF;
                alpha = 1.0f;
                g = r;
                b = r;
                break;
            case 1:
                cl = &mw->clay[35];
                r = 0x33;
                alpha = 1.0f;
                g = r;
                b = r;
                break;
            }
            sc[0] = p->size * p->scale[0];
            sc[1] = p->size * p->scale[1];
            sc[2] = p->size * p->scale[2];
            rot[2] = DEG2RAD(ANG2DEG(p->rot));
            make_mat_srt(sc, rot, pr->pos, 2, &m);
            flmatMul33_2(&m, &rview_mat);
            break;
        case 21:
        case 26:
        case 29:
            if (pr->no == 0) {
                cl = &mw->clay[65];
            } else {
                cl = &mw->clay[64];
            }
            flag |= 2;
            if (ew->arg == 0x15) {
                r = 0xBF;
                b = 0xFF;
                g = r;
            } else if (ew->arg == 0x1D) {
                g = 0x7F;
                r = 0xFF;
                b = g;
            } else if (game_w.x1DC == 0) {
                r = eft13_type26_rgb[ew->x1E][0];
                g = eft13_type26_rgb[ew->x1E][1];
                b = eft13_type26_rgb[ew->x1E][2];
            } else {
                r = 0xFF;
            }
            alpha = 1.0f;
            sc[0] = p->size * p->scale[0];
            sc[1] = p->size * p->scale[1];
            sc[2] = p->size * p->scale[2];
            rot[2] = DEG2RAD(ANG2DEG(p->rot + ew->u0A.joint));
            make_mat_srt(sc, rot, pr->pos, 2, &m);
            flmatMul33_2(&m, &rview_mat);
            break;
        case 22:
            r = 0xFF;
            cl = &mw->clay[72];
            alpha = 1.0f;
            g = r;
            b = r;
            sc[0] = p->size * p->scale[0];
            sc[1] = p->size * p->scale[1];
            sc[2] = p->size * p->scale[2];
            rot[2] = DEG2RAD(ANG2DEG(p->rot));
            make_mat_srt(sc, rot, pr->pos, 2, &m);
            flmatMul33_2(&m, &rview_mat);
            break;
        case 28:
            cl = &mw->clay[72];
            r = 0xD8;
            alpha = 1.0f;
            g = 0xCE;
            b = 0xB9;
            sc[0] = p->size * p->scale[0];
            sc[1] = p->size * p->scale[1];
            sc[2] = p->size * p->scale[2];
            rot[2] = DEG2RAD(ANG2DEG(p->rot));
            make_mat_srt(sc, rot, pr->pos, 2, &m);
            flmatMul33_2(&m, &rview_mat);
            break;
        case 30:
            r = 0xFF;
            cl = &mw->clay[74];
            alpha = 1.0f;
            g = r;
            b = r;
            sc[0] = p->size * p->scale[0];
            sc[1] = p->size * p->scale[1];
            sc[2] = p->size * p->scale[2];
            rot[2] = DEG2RAD(ANG2DEG(p->rot));
            make_mat_srt(sc, rot, pr->pos, 2, &m);
            flmatMul33_2(&m, &rview_mat);
            break;
        }
        if (ew->arg == 1 || ew->arg == 2 || ew->arg == 16) {
            eft_trans_sub(cl, &m, flag, p->alpha, mats);
        } else {
            eft_trans_sub_col(cl, &m, (((u8)(255.0f * alpha * p->alpha) << 24) | (r << 16)) | (g << 8) | b, flag, mats);
        }
        SetFilterMode(1);
    }
}

void eft13_se_req(EFTW *ew) {
    switch (ew->arg) {
    case 0x1A:
        se_req2(1, 0x71, 0, ew->pos, 1, 0);
        break;
    }
}

s16 eft13_water_ck(EFT13_CHR *chr, f32 *pos, s16 arg) {
    s16 *flag = &eft13_water_flag[arg];
    f32 h;

    if (*flag == 0) {
        return 0;
    }
    if (Eft_stg_type[game_w.stage] == 10) {
        pos[1] = 5.0f + chr->x5AC;
        if (game_w.stage == 0x1A) {
            pos[1] += 10.0f;
        }
        return *flag;
    }
    if (GetWaterHit(pos, &h) != 0) {
        if (h < pos[1] - 18.0f) {
            pos[1] = chr->x5AC;
        } else {
            pos[1] = 5.0f + h;
            return *flag;
        }
    } else {
        pos[1] = chr->x5AC;
    }
    return 0;
}

void eft13_set_sub(EFT13_CHR *chr, s16 j, int arg, EFTW *ew) {
    FLMAT m;

    ew->type = 13;
    ew->move = eft13_move;
    ew->arg = arg;
    ew->u0A.joint = chr->ang[1];
    switch (ew->arg) {
    case 0:
    case 3:
    case 4:
    case 9:
    case 0x12:
    case 0x14:
        if (j == 8) {
            ew->mode2 = 0;
        } else {
            ew->mode2 = 1;
        }
        break;
    case 0xA:
        ew->scale = 0.7f;
        break;
    case 0xD:
        ew->mode2 = j;
        break;
    case 0x16:
        flmatCopy(&m, get_joint_wmat(chr, j));
        ew->u0A.joint = calc_mat_angY(&m) + 0x4000;
        break;
    case 0x1A:
        ew->x1E = chr->x909;
        break;
    }
    ew->owner = (EMW *)chr;
}

