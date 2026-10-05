/* eft13c - SLPM_654.95 0x00107980-0x00107C50: se_req, water_ck, set_sub.
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
    u8 _pad00[0x2A];
    s16 prim_no;        /* 0x2A */
    PRIM *prim;         /* 0x2C */
} EFT13_PIECE;

extern s16 eft13_num[35];
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

