/* camarea04 - f_camarea 0x002233C0-0x00223410: CamAreaAttribChk. Whole file in camarea_nm.c. */
#include "types.h"
#include "cam.h"
#include "game.h"

#define EB(p, o) (*(u8 *)((u8 *)(p) + (o)))
#define EH(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define EW(p, o) (*(u32 *)((u8 *)(p) + (o)))
#define EF(p, o) (*(f32 *)((u8 *)(p) + (o)))
#define EP(p, o) (*(u8 **)((u8 *)(p) + (o)))

extern u8 *cam_data_area;
extern f32 *stage_camera_data_tbl[4];  /* per zoom level 1-4: 7 floats per stage */
extern PLW player_work[];

void flvecCopy(f32 *, f32 *);
void AddVector(f32 *, f32 *, f32 *);
f32 flvecInnerProduct(f32 *, f32 *);
f32 flvecCalcDistance(f32 *, f32 *);
void flvecApplyMat33(f32 *, f32 *, f32 *);
void Spline(f32 *out, f32 *pts, int n);
int GetOrthogonalPoint(f32 *out, f32 *coef, f32 *q, int mode);
void CamRailPoint(f32 *out, f32 *c, f32 t);




s32 CameraAreaCheck(CAMAREA *a, PLW *pl, s32 mask);













/* 0x002233C0: areas with flag 0x80 only count for the player actions
 * 0x27-0x2B of state 0 (cannon? guess) */
s32 CamAreaAttribChk(CAMAREA *a, PLW *pl) {
    if (EB(a, 6) & 0x80) {
        if (EB(pl, 0x14) != 0 || EB(pl, 0x15) < 0x27 || EB(pl, 0x15) > 0x2B) {
            return 0;
        }
    }
    return 1;
}
