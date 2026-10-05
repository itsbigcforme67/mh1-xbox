/* eft13 - SLPM_654.95 0x00105B10-0x00105B94: eft13_move.
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

void eft13_move(EFTW *ew) {
    switch (ew->mode) {
    case 0:
        eft13_i(ew);
        break;
    case 1:
        eft13_m(ew);
        break;
    case 2:
        eft13_d(ew);
        break;
    case 3:
        eft13_e(ew);
        break;
    }
}

