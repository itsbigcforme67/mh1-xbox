/* Rotates n points around a center by the Euler angles (degrees) in a sprite descriptor.
 * SLPM_654.95 0x0015A520-0x0015A6C0 (f_calcpoint, first function; SpritePut not done). */
#include "types.h"

typedef f32 MAT4[4][4];

extern MAT4 view_mat;

void flmatInvert(void *, void *);
void flmatInit(void *);
void flmatSetZYX33(void *, f32, f32, f32);
void flmatMul(void *, void *, void *);
void *cpApplyMatrix(void *, f32 *, f32 *);

typedef struct SPRD {           /* sprite descriptor (SpritePut argument), fields used here */
    u8 _pad00[0x38];
    u32 flags;                  /* 0x38 bit 13: rotate in view space */
    u8 _pad3C[4];
    f32 ang[3];                 /* 0x40 euler angles in degrees */
} SPRD;

void CalcPoint(f32 *pts, f32 *center, int n, SPRD *d) {
    MAT4 rot;
    MAT4 view;
    f32 r[3];
    f32 v[3];
    int i;

    if (d->flags & 0x2000) {
        flmatInvert(view, view_mat);
    } else {
        flmatInit(view);
    }
    flmatInit(rot);
    flmatSetZYX33(rot, 2.0f * (3.1415927f * (d->ang[0] / 360.0f)),
                  2.0f * (3.1415927f * (d->ang[1] / 360.0f)),
                  2.0f * (3.1415927f * (d->ang[2] / 360.0f)));
    flmatMul(rot, rot, view);
    for (i = 0; i < n; i++) {
        v[0] = pts[0] - center[0];
        v[1] = pts[1] - center[1];
        v[2] = pts[2] - center[2];
        cpApplyMatrix(rot, v, r);
        pts[0] = r[0] + center[0];
        pts[1] = r[1] + center[1];
        pts[2] = r[2] + center[2];
        pts += 3;
    }
}
