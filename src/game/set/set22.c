/* set22 - game.bin 0x00627840-0x0062813C, split in two around set22_m
 * (see set22_nm.c). In the original all of these functions are static. A flying creature seen in the
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

static void set22_move(SETW *sw);
static void set22_i(SETW *sw);
void set22_m(SETW *sw);
void set22_d(SETW *sw);
void set22_e(SETW *sw);
void set22_trans(PRIM *pr);

void Set22_set(void) {
    SETW *sw = pull_set_work(0);

    if (sw != 0) {
        sw->type = 22;
        sw->move = set22_move;
    }
}

static void set22_move(SETW *sw) {
    switch (sw->mode) {
    case 0:
        set22_i(sw);
        break;
    case 1:
        set22_m(sw);
        break;
    case 2:
        set22_d(sw);
        break;
    case 3:
        set22_e(sw);
        break;
    }
}

static void set22_i(SETW *sw) {
    sw->mode++;
    sw->be_flag = 1;
    sw->work14 = 0;
    sw->mode2 = 0;
    sw->se0 = 0;
    sw->se1 = 0;
    sw->timer = (u16)ran_suu(1) & 0xF;
    sw->cnt = 0;
    sw->prim_no = get_prim();
    if (sw->prim_no != -1) {
        sw->prim = get_prim_ptr(sw->prim_no);
        sw->prim->owner = sw;
        sw->prim->trans = set22_trans;
        return;
    }
    push_set_work(sw);
}
