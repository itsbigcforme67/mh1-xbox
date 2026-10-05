/* eft05 - game.bin 0x00542D40-0x00543718, part 2 (after eft05_t). A weapon's slash trail. Each
 * frame the weapon tip's matrix (joint 14 or 18, plus an offset per arg) is
 * pushed into a six-entry history while the player stays in the attack
 * animation, and eft05_t draws a skinned ribbon through it. */
#include "eft.h"
#include "game.h"
#include "pl.h"
#include "prim.h"
#include "clay.h"
#include "fl.h"

typedef struct EFT05_WORK {
    u8 _pad00[0x10];
    FLMAT hist[6];      /* 0x10 newest first */
} EFT05_WORK;

typedef struct EFT_MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x0F];
    void *mat;          /* 0x10 material table */
    u8 _pad14[0x10];
    u8 *skin;           /* 0x24 skin matrix list (count-ish at +0xC2) */
    u8 _pad28[8];
    CLAY *clay;         /* 0x30 */
} EFT_MDLW;

extern f32 eft05_ofs[2][3];
extern EFT_MDLW *eft_mdlw[5];
extern f32 mat_calc[6][2];
extern f32 sprine_calc[6][3];
extern u8 Eft_blood_rgb[];

u8 Pl_stg_ck(PLW *);
void release_prim(s16);
FLMAT *get_joint_wmat(PLW *, int);
void flmatCopy(FLMAT *, FLMAT *);
void flmatGetTrans(f32 *, FLMAT *);
void flvecApplyMat33_2(f32 *, FLMAT *);
void flmatBlend(FLMAT *, FLMAT *, FLMAT *, f32, f32);
void flSetSkinTrans(void *);
void SetFilterMode(int);
void Material_set_sub(void *, CLAY *);
void Eft_rendope_set(int);

void eft05_move(EFTW *ew);

void Eft05_set(PLW *pl, int timer, int arg) {
    EFTW *ew;

    if (Pl_stg_ck(pl) != 0) {
        ew = pull_eft_work(2);
        if (ew != 0) {
            ew->type = 5;
            ew->move = eft05_move;
            ew->work14 = 0;
            ew->owner = (EMW *)pl;
            ew->x1E = pl->id;
            ew->timer = timer;
            ew->u0A.ang = pl->char0;
            ew->arg = arg;
        }
    }
}
