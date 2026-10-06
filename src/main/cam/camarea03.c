/* camarea03 - f_camarea 0x00222E20-0x00223070: default_area_data, StageCamInit. Whole file in camarea_nm.c. */
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













/* 0x00222E20: one follow-the-player area covering everything, built in
 * cam_data_area; zoom entries 1-4 from the stage's rows of
 * stage_camera_data_tbl[0..3] (y, z, tar_y, gnd), entry 0 fixed. */
static void default_area_data(CAMW *cw)
{
  u8 *d = cam_data_area;
  u8 *new_var;
  int k;
  u8 *new_var2;
  new_var2 = (u8 *) (((u8 *) d) + 0x30);
  *((u16 *) (((u8 *) d) + 0x00)) = 0x102;
  *((u16 *) (((u8 *) d) + 0x02)) = 0;
  *((u16 *) (((u8 *) d) + 0x04)) = 1;
  *((u16 *) (((u8 *) d) + 0x06)) = 1;
  *((u16 *) (((u8 *) d) + 0x08)) = 20000;
  *((u16 *) (((u8 *) d) + 0x0A)) = 20000;
  *((u32 *) (((u8 *) d) + 0x0C)) = 0;
  *((u32 *) (((u8 *) d) + 0x10)) = 0;
  *((u32 *) (((u8 *) d) + 0x14)) = 20000;
  *((u32 *) (((u8 *) d) + 0x18)) = 20000;
  *((u32 *) (((u8 *) d) + 0x1C)) = 0;
  *((u32 *) (((u8 *) d) + 0x20)) = 0;
  *((u32 *) (((u8 *) d) + 0x24)) = 0;
  *((u8 **) (((u8 *) d) + 0x28)) = d + 0x30;
  *new_var2 = 0;
  *((u8 *) (((u8 *) d) + 0x31)) = 0;
  *((u8 *) (((u8 *) d) + 0x32)) = 0;
  *((u8 *) (((u8 *) d) + 0x33)) = 2;
  *((u8 *) (((u8 *) d) + 0x34)) = 0;
  new_var = (u8 *) d;
  *((u8 *) (new_var + 0x35)) = 0;
  *((f32 *) (new_var + 0x38)) = 400.0f;
  *((f32 *) (new_var + 0x3C)) = 2400.0f;
  *((f32 *) (new_var + 0x40)) = 1.0f;
  *((f32 *) (new_var + 0x44)) = 0.75f;
  *((u32 *) (new_var + 0x48)) = 0;
  *((u32 *) (new_var + 0x4C)) = 0;
  for (k = 0; k < 4; k++)
  {
    f32 *row = (f32 *) (((u8 *) stage_camera_data_tbl[k]) + (game_w.stage * 28));
    u8 *e = (d + 0x90) + (k * 0x20);
    *((f32 *) (((u8 *) e) + 0x04)) = row[1];
    *((f32 *) (((u8 *) e) + 0x08)) = row[2];
    *((f32 *) (((u8 *) e) + 0x10)) = row[4];
    *((f32 *) (((u8 *) e) + 0x18)) = row[6];
  }

  *((f32 *) (new_var + 0x74)) = 300.0f;
  *((f32 *) (new_var + 0x78)) = 160.0f;
  *((f32 *) (new_var + 0x80)) = 184.0f;
  *((f32 *) (new_var + 0x88)) = 80.0f;
  *((f32 *) (new_var + 0x50)) = 0.87266463f;
  *((f32 *) (new_var + 0x54)) = 0.0f;
  *((f32 *) (new_var + 0x58)) = 0.0f;
  *((u16 *) (new_var + 0x5E)) = 0;
  *((u16 *) (new_var + 0x5C)) = 0;
  cw->data = d;
}

/* 0x00223000 */
void StageCamInit(CAMW *cw, u8 no) {
    u8 *d;

    if (cw->data == 0) {
        default_area_data(cw);
    }
    d = cw->data;
    EH(cw, 0x590) = EH(d, 0x4);
    EH(cw, 0x592) = EH(d, 0x6);
    EH(cw, 0x594) = EH(d, 0x8);
    EH(cw, 0x596) = EH(d, 0xA);
    EW(cw, 0x598) = EW(d, 0xC);
    EW(cw, 0x59C) = EW(d, 0x10);
    EW(cw, 0x5A0) = EW(d, 0x14);
    EW(cw, 0x5A4) = EW(d, 0x18);
}
