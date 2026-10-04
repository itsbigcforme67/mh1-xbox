/* set22 - game.bin 0x00627840-0x0062813C, part 2 (after set22_m). In the
 * original all of these functions are static. A flying creature seen in the
 * distance: after a random wait it appears at a random point (around the
 * master player, or at a fixed spot on stage 25), turns and scales by
 * kind, fades in and is drawn as a billboard. */
#include "set.h"
#include "game.h"
#include "pl.h"
#include "prim.h"
#include "clay.h"
#include "fl.h"

typedef struct SET_MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x2F];
    CLAY *clay;         /* 0x30 */
} SET_MDLW;

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

extern SET_MDLW *set_mdlw;
extern FLMAT rview_mat;
extern u8 ot3[];

u32 ran_suu(int);
void release_prim(s16);
void flvecCopy(f32 *, f32 *);
void flvecRotY(f32 *, f32);
void flmatRotZ33(FLMAT *, f32);
void Set13_set2(int, f32 *, s16);
void Eft17_set_ex(f32 *, int, int, f32);

void push_set_work(SETW *);
void set22_trans(PRIM *pr);

void set22_d(SETW *sw) {
    sw->mode++;
    sw->be_flag = 0;
    release_prim(sw->prim_no);
}

void set22_e(SETW *sw) {
    push_set_work(sw);
}

void set22_trans(PRIM *pr) {
    FLMAT mat;
    SETW *sw = pr->owner;
    SET_MDLW *mw = set_mdlw;
    CLAY *cl;
    s16 m;
    u8 a;

    if (mw != 0 && mw->flag != 0) {
        flSetRenderState(0x60, 0);
        flmatMakeScale(&mat, sw->speed, sw->speed, sw->speed);
        flmatRotZ33(&mat, DEG2RAD(ANG2DEG(sw->cnt)));
        flmatSetTrans(&mat, pr->pos[0], pr->pos[1], pr->pos[2]);
        flmatMul33_2(&mat, &rview_mat);
        if (sw->timer < 4) {
            a = 255.0f * (sw->timer / 4.0f);
        } else {
            a = 0xFF;
        }
        switch (game_w.stage) {
        case 7:
            m = 2;
            break;
        case 0x19:
            m = 3;
            break;
        }
        flSetRenderState(0x67, (a << 24) | 0xFFFFFF);
        flSetRenderState(0x1A, (u32)&mat);
        cl = &mw->clay[m] + sw->se0 * 2;
        if (sw->timer < 6) {
            cl++;
        }
        if (cl != 0 && cl->handle != -1) {
            clay_attr_set(cl->attr);
            flExecuteClay(cl->handle, 0);
        }
        clay_attr_reset();
    }
}
