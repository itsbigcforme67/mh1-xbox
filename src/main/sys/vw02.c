/* vw02 - view and projection 0x00169CF0-0x00169D98: Create_FOV, set_aov. Whole file in view_nm.c. */
#include "types.h"
#include "sysw.h"

typedef f32 MAT4[4][4];

typedef struct VIEWW {          /* lpView */
    f32 eye[3];                 /* 0x00 */
    f32 at[3];                  /* 0x0C */
    f32 up[3];                  /* 0x18 computed in View_move */
    f32 x24;                    /* 0x24 projection parameter (near/aspect, from proj_tbl[n][1]) */
    f32 x28;                    /* 0x28 projection parameter (proj_tbl[n][0]) */
    f32 aov;                    /* 0x2C angle of view (proj_tbl[n][2]); SetAngleOfView writes it */
    f32 aov_set;                /* 0x30 aov last applied */
    f32 roll;                   /* 0x34 RollView */
    u8 proj[0x20];              /* 0x38 projection handed to render state 0x18 */
} VIEWW;

extern VIEWW *lpView;
extern f32 proj_tbl[];
extern u8 fov[];
extern MAT4 view_mat, rview_mat, rview_matY;

void flQuatSetRot2(f32 *, void *, f32);
void flQuatCnv(void *, void *);
void flvecApplyMat33(void *, void *, void *);
void flmatMakeLookAt(void *, void *, void *, void *);
void flSetRenderState(int, int);
void flmatrLoad(void *, int);
void flmatInvert(void *, void *);
int calc_mat_angY(void *);
void flmatInit(void *);
void flmatSetXYZ33(void *, f32, f32, f32);
void flmatrMakeProjection(int, f32, f32, f32, f32);
void plSetupFOVClipPlanes(void *, f32, f32, f32, f32);
void set_aov(void);





void Create_FOV(f32 far_) {
    plSetupFOVClipPlanes(fov, lpView->aov, 1.4285715f, lpView->x24, far_);
}

void set_aov(void) {
    flmatrMakeProjection(0x20, lpView->x28, lpView->x24, lpView->aov, 1.4285715f);
    flSetRenderState(0x18, (int)lpView->proj);
    plSetupFOVClipPlanes(fov, lpView->aov, 1.4285715f, lpView->x24, 3000.0f);
}
