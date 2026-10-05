/* set05 - game.bin 0x0061F7E0-0x006206C4, split in two around set05_m
 * (see set05_nm.c). In the original all of these functions are static. Turnable / firing stage
 * fixtures ("balli", kinds 1 and 2) on stages 11, 12, 25, 28 and 30, 2-5
 * per stage. Kind 1 turns to a player's facing when they use it
 * (animation 0x19E, frame 66) within 200 units. Kind 2, once
 * game_w.x1B2 is set, fires a shell and then rises and sinks. */
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
extern u8 st11_id_tbl[3];
extern f32 st11_pos_tbl_00677B50[][3];
extern s16 st11_mdl_no_00389A50[3];
extern u8 st12_id_tbl[5];
extern f32 st12_pos_tbl[][3];
extern s16 st12_mdl_no_00389A60[3];
extern u8 st25_id_tbl[3];
extern f32 st25_pos_tbl[][3];
extern s16 st25_mdl_no_00389A70[3];
extern u8 st28_id_tbl[2];
extern f32 st28_pos_tbl_00677BF0[][3];
extern s16 st28_mdl_no_00389A88[3];
extern u8 st30_id_tbl[3];
extern f32 st30_pos_tbl_00677C10[][3];
extern s16 st30_mdl_no_00389A98[3];
extern u16 st25_rot_tbl[3];

void release_prim(s16);
void flmatInit(FLMAT *);
void flvecRotY(f32 *, f32);
void flmatRotZ33(FLMAT *, f32);
f32 flvecCalcDistance(f32 *, f32 *);
u8 Pl_stg_ck(PLW *);
int Pl_master_ck(PLW *);
int frame_check2(PLW *, int, f32);
void Shell22_set2(f32 *, int, int, int);

void set05_move(SETW *sw);
static void set05_i(SETW *sw);
void set05_m(SETW *sw);
void set05_d(SETW *sw);
void set05_e(SETW *sw);
void set05_trans(PRIM *pr);
u8 set05_balli_id_ck(SETW *sw);

void Set05_set(u8 arg) {
    SETW *sw;
    u8 n, i;

    switch (arg) {
    case 0xB:
    case 0x19:
    case 0x1E:
        n = 3;
        break;
    case 0xC:
        n = 5;
        break;
    case 0x1C:
        n = 2;
        break;
    default:
        n = 0;
        break;
    }
    for (i = 0; i < n; i++) {
        sw = pull_set_work(0);
        if (sw != 0) {
            sw->type = 5;
            sw->arg = arg;
            sw->move = set05_move;
            sw->se1 = i;
        }
    }
}

void set05_move(SETW *sw) {
    switch (sw->mode) {
    case 0:
        set05_i(sw);
        break;
    case 1:
        set05_m(sw);
        break;
    case 2:
        set05_d(sw);
        break;
    case 3:
        set05_e(sw);
        break;
    }
}

static void set05_i(SETW *sw) {
    f32 *p;
    u8 kind;

    switch (sw->arg) {
    case 0xB:
        p = st11_pos_tbl_00677B50[sw->se1];
        kind = st11_id_tbl[sw->se1];
        break;
    case 0xC:
        p = st12_pos_tbl[sw->se1];
        kind = st12_id_tbl[sw->se1];
        break;
    case 0x19:
        p = st25_pos_tbl[sw->se1];
        kind = st25_id_tbl[sw->se1];
        break;
    case 0x1C:
        p = st28_pos_tbl_00677BF0[sw->se1];
        kind = st28_id_tbl[sw->se1];
        break;
    case 0x1E:
        p = st30_pos_tbl_00677C10[sw->se1];
        kind = st30_id_tbl[sw->se1];
        break;
    }
    if (kind == 2) {
        if (game_w.x1B2 == 0) {
            sw->mode2 = 0;
        } else {
            sw->mode2 = 5;
        }
    }
    sw->mode++;
    sw->be_flag = 1;
    sw->work14 = 0;
    sw->timer = 0;
    sw->cnt = 0;
    sw->pos[0] = *p++;
    sw->pos[1] = *p++;
    sw->pos[2] = *p++;
    sw->prim_no = get_prim();
    sw->prim = get_prim_ptr(sw->prim_no);
    if (sw->prim != 0) {
        sw->prim->owner = sw;
        sw->prim->trans = set05_trans;
    } else {
        push_set_work(sw);
    }
}
