/* SLPM_654.95 0x00223500-0x00223754: GetPanTarget .. GetRailCamPos. See camarea_nm.c. */
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













/* 0x00223500: target of a pan camera: +4 = 0 fixed point (+0x2C), 1 a
 * point on the player's model matrix (+0x38, PLW+0x60), 2 player + offset
 * (+0x44); +3 = 0 keeps y (+0x30), 1 keeps x/z. */
void GetPanTarget(CAMW *cw, f32 *out, CAMAREA *a) {
    u8 *pl = (u8 *)cw->pl;

    switch (EB(a, 4)) {
    case 0:
        flvecCopy(out, (f32 *)((u8 *)a + 0x2C));
        break;
    case 1:
        nlCalcPoint(out, (f32 *)((u8 *)a + 0x38), (f32 *)(pl + 0x60));
        break;
    case 2:
        AddVector(out, (f32 *)(pl + 0xAC), (f32 *)((u8 *)a + 0x44));
        break;
    }
    switch (EB(a, 3)) {
    case 0:
        out[1] = EF(a, 0x30);
        break;
    case 1:
        out[0] = EF(a, 0x2C);
        out[2] = EF(a, 0x34);
        break;
    }
}

/* 0x002235E0: the same for rail cameras (v = point on the rail) */
void GetRailTarget(CAMW *cw, f32 *out, CAMAREA *a, f32 *v) {
    u8 *pl = (u8 *)cw->pl;

    switch (EB(a, 4)) {
    case 0:
        flvecCopy(out, v);
        break;
    case 1:
        nlCalcPoint(out, (f32 *)((u8 *)a + 0x290), (f32 *)(pl + 0x60));
        break;
    case 2:
        AddVector(out, (f32 *)(pl + 0xAC), (f32 *)((u8 *)a + 0x29C));
        break;
    }
    switch (EB(a, 3)) {
    case 0:
        out[1] = v[1];
        break;
    case 1:
        out[0] = v[0];
        out[2] = v[2];
        break;
    }
}

/* 0x002236D0: camera position on the rail spline: section CameraWork+0x5B4
 * at CameraWork+0x5B0 times the section length (+0x2C + 16 * section). */
void GetRailCamPos(f32 *out, CAMW *cw, CAMAREA *a, f32 *spl) {
    u8 sec;

    Spline(spl, (f32 *)((u8 *)a + 0x20), EB(a, 0x280));
    sec = EB(cw, 0x5B4);
    CamRailPoint(out, spl + sec * 12, EF(cw, 0x5B0) * EF((u8 *)(sec * 16) + (int)a, 0x2C));
}
