/* SLPM_654.95 0x002232A0-0x002233BC: CameraAreaCheck .. CameraAreaCheck. See camarea_nm.c. */
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













/* 0x002232A0: is the entity inside one of the area's boxes (box flag byte
 * +0x2C & mask skips it)? 0 yes, 2 no. */
s32 CameraAreaCheck(CAMAREA *a, PLW *pl, s32 mask) {
    u8 *b;
    u32 n;
    f32 v[3];
    f32 d;

    if (CamAreaAttribChk(a, pl) == 0) {
        return 2;
    }
    n = EB(a, 5);
    b = EP(a, 0x18);
    for (; n != 0; n--, b += 0x40) {
        if (EB(b, 0x2C) & (mask & 0xFF)) continue;
        v[0] = pl->pos[0] - EF(b, 0x20);
        v[1] = pl->pos[1] - EF(b, 0x24);
        v[2] = pl->pos[2] - EF(b, 0x28);
        d = flvecInnerProduct((f32 *)(b + 0x30), v);
        if (d < 0.0f) continue;
        if (!(d <= EF(b, 0x3C))) continue;
        if (Area_XZ_Check((f32 *)b, pl->pos) == 0) {
            return 0;
        }
    }
    return 2;
}
