/* shit401 - slide check on a steep floor and normal angle (SLPM_654.95 0x0011C920-0x0011CA74): check_slide (check_angle is in shit7.c). Whole file in shit4_nm.c. */
/* shit4_nm (not built): the rest of f_sphr (SLPM_654.95 0x00114D90-0x0011CA70,
 * stage hit queries) written as C but not matched yet; see shit1.c (loader),
 * shit2a.c (grid helpers) and shit3_nm.c (ground height queries). Function
 * by function status in docs/agents/agent-D.md. Names are guesses from the
 * code. */
#include "types.h"
#include "hit3.h"
#include "game.h"
void UnitNormalVectorCCW(f32 *, f32 *, f32 *, f32 *);
void PointToPoint(f32 *, f32 *, f32 *);
void ScaleVector(f32 *, f32 *, f32);
void AddVector(f32 *, f32 *, f32 *);
void SetVector(f32 *, f32, f32, f32);
f32 flvecInnerProduct(f32 *, f32 *);
f32 flvecCalcLength(f32 *);
void flvecNormalize(f32 *);
f32 flArcCos(f32);
s32 *GetGroundTblAdrs(f32 *);
int GroundFieldInCheck(f32 *);
int PointHitCheckF3(f32 *, f32 *);
/* 0x0011C920: slide vector for a polygon pl under entity e (pos at +0xAC):
 * plane distance of the point lowered by ang/10000*scale, written to out. */
int check_slide(HPOLY *pl, u8 *e, s32 *ang, f32 *out, f32 scale) {
    f32 nn;
    int new_var; /* permuter: matching register choice */
    f32 t;
    f32 py;

    new_var = 1;
    if (*ang < 0x1500) return 0;
    if (*ang == 0x4000) *ang = 0;
    py = *(f32 *)(e + 0xB0) - (f32)((f32)*ang / 10000.0f) * scale;
    nn = pl->n[0] * pl->n[0] + pl->n[new_var] * pl->n[new_var] + pl->n[2] * pl->n[2];
    t = pl->d + (pl->n[0] * *(f32 *)(e + 0xAC) + pl->n[new_var] * py + pl->n[2] * *(f32 *)(e + 0xB4));
    out[0] = -(pl->n[0] * t / nn);
    out[new_var] = -(pl->n[new_var] * t / nn);
    out[2] = -(pl->n[2] * t / nn);
    return new_var;
}
