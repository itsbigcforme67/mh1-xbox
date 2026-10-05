/* view2_nm - SLPM_654.95 0x00169990-0x00169A80 (g_vib_stop.s): vibration stop helpers and view work initialisation.
   See view_nm.c for the rest of the view code. */
#include "types.h"

typedef struct VIEWW {          /* lpView, see view_nm.c */
    f32 eye[3];                 /* 0x00 */
    f32 at[3];                  /* 0x0C */
    f32 up[3];                  /* 0x18 */
    f32 x24;                    /* 0x24 */
    f32 x28;                    /* 0x28 */
    f32 aov;                    /* 0x2C angle of view */
    f32 aov_set;                /* 0x30 aov last applied */
    f32 roll;                   /* 0x34 */
    s32 vp_x;                   /* 0x38 viewport x */
    s32 vp_y;                   /* 0x3C */
    s32 vp_w;                   /* 0x40 */
    s32 vp_h;                   /* 0x44 */
    s32 vp_z0;                  /* 0x48 */
    f32 vp_z1;                  /* 0x4C */
} VIEWW;

extern VIEWW *lpView;
extern VIEWW view_work;
extern u8 view_w[];

void flPADShockSet(int, int, int);
void vib_stop(int pad);
void View_init(void);

void vib_stop(int pad) {
    flPADShockSet(pad, 0, 0);
}

void vib_stop_all(void) {
    int i;

    for (i = 0; i < 2; i++) {
        vib_stop(i);
    }
}

void View_initialize(void) {
    lpView = &view_work;
    lpView->vp_x = 0;
    lpView->vp_y = 0;
    lpView->vp_w = 0x200;
    lpView->vp_h = 0x1C0;
    lpView->vp_z0 = 0;
    lpView->vp_z1 = 1.0f;
    View_init();
}

void view_reset(void) {
    *(s32 *)(view_w + 0x40) = 0;
    view_w[0xBB] = 1;
}

void View_init(void) {
    lpView->aov = 0.87266463f;
    lpView->aov_set = -999.0f;
    *(s32 *)&lpView->roll = 0;
}
