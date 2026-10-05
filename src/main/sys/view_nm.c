/* View matrix and projection setup. SLPM_654.95 0x00169A80-0x00169D98 (f_set). */
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

void View_move(void) {
    f32 m[16];
    f32 q[4];
    f32 d[3];
    f32 r;
    int changed = 0;

    if (lpView->aov_set != lpView->aov) {
        lpView->aov_set = lpView->aov;
        changed++;
    }
    if (changed != 0) {
        set_aov();
    }
    d[0] = lpView->at[0] - lpView->eye[0];
    d[1] = lpView->at[1] - lpView->eye[1];
    d[2] = lpView->at[2] - lpView->eye[2];
    flQuatSetRot2(d, q, lpView->roll);
    flQuatCnv(q, m);
    d[0] = 0;
    d[1] = 1.0f;
    d[2] = 0;
    flvecApplyMat33(lpView->up, d, m);
    flmatMakeLookAt(m, lpView->eye, lpView->at, lpView->up);
    flSetRenderState(0x16, (int)m);
    flmatrLoad(view_mat, 0x21);
    flmatInvert(rview_mat, view_mat);
    r = 2.0f * (3.1415927f * ((360.0f * (f32)(((calc_mat_angY(rview_mat) & 0xFFFF) + 0x4000) & 0xFFFF) / 65536.0f) / 360.0f));
    flmatInit(rview_matY);
    flmatSetXYZ33(rview_matY, 0.0f, r, 0.0f);
}

void set_viewproj(int n) {
    f32 t[4];

    if (n == 0xFF) {
        set_aov();
    } else if (system_w.x2F != n) {
        system_w.x2F = n;
        t[0] = proj_tbl[n * 4];
        t[1] = proj_tbl[n * 4 + 1];
        t[2] = proj_tbl[n * 4 + 2];
        t[3] = proj_tbl[n * 4 + 3];
        lpView->x28 = t[0];
        lpView->x24 = t[1];
        lpView->aov = t[2];
        *(s32 *)&lpView->roll = 0;
        set_aov();
    }
}

void Create_FOV(f32 far_) {
    plSetupFOVClipPlanes(fov, lpView->aov, 1.4285715f, lpView->x24, far_);
}

void set_aov(void) {
    flmatrMakeProjection(0x20, lpView->x28, lpView->x24, lpView->aov, 1.4285715f);
    flSetRenderState(0x18, (int)lpView->proj);
    plSetupFOVClipPlanes(fov, lpView->aov, 1.4285715f, lpView->x24, 3000.0f);
}
