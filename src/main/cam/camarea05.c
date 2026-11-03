/* camarea05 - f_camarea 0x00223070-0x0022318C: SetAreaData. Whole file in camarea_nm.c. */
#include "types.h"
#ifdef VERSION_MHG
#define GW_MASTER 0x3A /* G moved master from 0xD1 */
#else
#define GW_MASTER 0xD1
#endif
#include "cam.h"
#include "game.h"
#include "version.h"

#define EB(p, o) (*(u8 *)((u8 *)(p) + MH2_OFS(p, o)))
#define EH(p, o) (*(u16 *)((u8 *)(p) + MH2_OFS(p, o)))
#define EW(p, o) (*(u32 *)((u8 *)(p) + MH2_OFS(p, o)))
#define EF(p, o) (*(f32 *)((u8 *)(p) + MH2_OFS(p, o)))
#define EP(p, o) (*(u8 **)((u8 *)(p) + MH2_OFS(p, o)))

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













/* 0x00223070: the area the camera's player is in. -1 no camera data,
 * 0 none found (area = first block), 1 found (area set). */
#if defined(VERSION_MH2) && defined(__MWERKS__)
/* MH2's own (byte-matched): CameraWork as MH2 lays it out (data 0x388, area 0x38C, grid 0x390...). The PC keeps the rest of the
 * camera C on MH1's CAMW (include/cam.h; MH2's CameraWork is not laid out yet), so it takes the form below, which agrees with it. */
s32 SetAreaData(CAMW *cw) {
    PLW *pl = PLW_AT(EB(&game_w, GWX(0xD1)));
    u8 *d = EP(cw, 0x388);
    u8 *cell;
    CAMAREA **l;
    u32 n;

    if (d == 0) {
        EW(cw, 0x38C) = 0;
        return -1;
    }
    EW(cw, 0x38C) = EW(d, 0x28);
    if (EB(cw, 0x3AC) != 0) {
        return 0;
    }
    if (EP(d, 0x1C) == 0) {
        return 0;
    }
    cell = EP(d, 0x1C) + (EH(cw, 0x3A8) + EH(cw, 0x390) * EH(cw, 0x3AA)) * 8;
    n = EW(cell, 0);
    if (n == 0) {
        return 0;
    }
    l = (CAMAREA **)EP(cell, 4);
    for (; n != 0; n--, l++) {
        if (CameraAreaCheck(*l, pl, 1) == 0) {
            EW(cw, 0x38C) = (u32)*l;
            return 1;
        }
    }
    return 0;
}
#else
s32 SetAreaData(CAMW *cw) {
#ifdef VERSION_MH2
    PLW *pl = PLW_AT(EB(&game_w, GWX(0xD1)));
#else
    PLW *pl = &player_work[EB(&game_w, GW_MASTER)];
#endif
    u8 *d = cw->data;
    u8 *cell;
    CAMAREA **l;
    u32 n;

    if (d == 0) {
        cw->area = 0;
        return -1;
    }
    cw->area = (CAMAREA *)EP(d, 0x28);
    if ((u8)cw->grid != 0) {
        return 0;
    }
    if (EP(d, 0x1C) == 0) {
        return 0;
    }
    cell = EP(d, 0x1C) + ((u16)cw->gx + EH(cw, 0x590) * (u16)cw->gz) * 8;
    n = EW(cell, 0);
    if (n == 0) {
        return 0;
    }
    l = (CAMAREA **)EP(cell, 4);
    for (; n != 0; n--, l++) {
        if (CameraAreaCheck(*l, pl, 1) == 0) {
            cw->area = *l;
            return 1;
        }
    }
    return 0;
}
#endif
