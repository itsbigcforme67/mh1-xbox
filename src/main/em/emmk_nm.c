/* emmk_nm - SLPM_654.95 0x0010AEB0-0x0010B6C0 part of VectorHitCheck.s: monster matrix build (enemy_mk), ride offset
   (ride_ofs_calc) ... Working file. */
#include "types.h"
#include "em.h"

void flmatMakeScale(f32 *, f32, f32, f32);
void cpRotMatrixYXZ2(s32 *, f32 *);
void flmatSetTrans(f32 *, f32, f32, f32);
void flmatMul33_2(f32 *, f32 *);
void flmatCopy(f32 *, f32 *);
void mlCalcTransEM(EMW *, u8 *, f32 *);
void flCalcTransSI(u8 *, f32 *);
void flmatInvert(f32 *, f32 *);
void flmatGetTrans(f32 *, f32 *);
void flvecApplyMat33(f32 *, f32 *, f32 *);
extern f32 sys_old_mat[][16];

/* Builds the monster's world matrix (scale, rotation, translation) into mat[], poses the per-kind special joints
   (mlCalcTransEM) and hands the skeleton to flCalcTransSI. */
void enemy_mk(EMW *em) {
    f32 m[16];
    f32 scale[16];
    EM_MDL *md = em->mdl;
    u8 kind;

    flmatMakeScale(scale, em->scale[0], em->scale[1], em->scale[2]);
    cpRotMatrixYXZ2(em->ang, m);
    flmatSetTrans(m, em->pos[0], em->pos[1], em->pos[2]);
    flmatMul33_2(m, scale);
    flmatCopy((f32 *)((u8 *)em + 0x60), m);
    kind = em->kind;
    if (kind == 1 || (u32)(kind - 6) <= 2 || kind == 0xB || (u32)(kind - 0xE) <= 1 || kind == 0x11 || (u32)(kind - 0x14) <= 2 || kind == 0x1A
        || kind == 0x21 || kind == 0x22) {
        mlCalcTransEM(em, md->bone, m);
    }
    flCalcTransSI(md->bone, m);
}

/* Position of the monster relative to saved old matrix #n (for riding), written to out as a vector. */
void ride_ofs_calc(EMW *em, int unused, s16 n, f32 *out) {
    f32 tr[4];
    f32 inv[16];
    f32 d[3];

    flmatInvert(inv, sys_old_mat[n]);
    flmatGetTrans(tr, sys_old_mat[n]);
    d[0] = em->pos[0] - tr[0];
    d[1] = em->pos[1] - tr[1];
    d[2] = em->pos[2] - tr[2];
    flvecApplyMat33(out, d, inv);
}
