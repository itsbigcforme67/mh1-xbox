/* fl library projection matrices (SLPM_654.95 0x001715E0-0x001717B8): flmatrMakeProjection builds matrix slot n of the flMATRIX table (perspective from
 * far/near/fov/aspect, parameters pass straight through in the float registers), updates the PS2 clip projection and the combined view-projection
 * matrices; flmatMakeProjection is the perspective matrix itself, flPS2MakeClipProjection scales a copy for the GS clipping volume. */
#include "types.h"

extern u8 flMATRIX[];
extern f32 flPS2CLIPPROJ[];
extern f32 flPS2VIEWPROJ[];
extern f32 flACRVIEWPROJ[];
extern f32 flACRVIEWPORT[];
extern f32 flViewportDH;
extern f32 flViewportDW;
extern f32 flViewportLX;
extern f32 flViewportLY;

void flmatInit(f32 *);
void flSinCos(f32 *, f32 *, f32);
void flmatCopy(f32 *, f32 *);
void flmatMul(f32 *, f32 *, f32 *);
void flmatMakeProjection(f32 *, f32, f32, f32, f32);
void flPS2MakeClipProjection(f32 *);

void flmatrMakeProjection(int n, f32 far, f32 near, f32 fov, f32 aspect) {
    f32 *m = (f32 *)(flMATRIX + n * 0x40);

    flmatMakeProjection(m, far, near, fov, aspect);
    flPS2MakeClipProjection(m);
    flmatMul(flPS2VIEWPROJ, (f32 *)(flMATRIX + 0x800), (f32 *)(flMATRIX + 0x880));
    flmatMul(flACRVIEWPROJ, (f32 *)(flMATRIX + 0x800), flACRVIEWPORT);
}

void flmatMakeProjection(f32 *m, f32 far, f32 near, f32 fov, f32 aspect) {
    f32 c;
    f32 s;

    fov *= 0.5f;
    flSinCos(&s, &c, fov);
    flmatInit(m);
    m[0] = c / s / aspect;
    m[5] = c / s;
    m[10] = -(far + near) / (far - near);
    *(int *)(m + 11) = 0xBF800000;
    m[14] = -(2.0f * far * near) / (far - near);
    *(int *)(m + 15) = 0;
}

void flPS2MakeClipProjection(f32 *m) {
    flmatCopy(flPS2CLIPPROJ, m);
    flPS2CLIPPROJ[0] = 2.0f * flPS2CLIPPROJ[0] / (flViewportLX / (0.5f * flViewportDW));
    flPS2CLIPPROJ[5] = 2.0f * flPS2CLIPPROJ[5] / (flViewportLY / (0.5f * flViewportDH));
}
