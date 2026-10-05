/* set05 - game.bin 0x0061F7E0-0x006206C4, part 2 (after set05_m). In the
 * original all of these functions are static. Turnable / firing stage
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

void set05_trans(PRIM *pr);
u8 set05_balli_id_ck(SETW *sw);

void set05_d(SETW *sw) {
    sw->mode++;
    sw->be_flag = 0;
    release_prim(sw->prim_no);
}

void set05_e(SETW *sw) {
    push_set_work(sw);
}

void set05_trans(PRIM *pr) {
    FLMAT mat;
    FLMAT uv;
    SETW *sw = pr->owner;
    SET_MDLW *mw = set_mdlw;
    CLAY *cl;
    u8 kind;
    s16 *mdl;
    f32 ry;

    if (sw->arg == game_w.stage && mw != 0 && mw->flag != 0) {
        switch (sw->arg) {
        case 0xB:
                ry = 0.0f;
                kind = st11_id_tbl[sw->se1];
                mdl = &st11_mdl_no_00389A50[kind];
            break;
        case 0xC:
                ry = 0.0f;
                kind = st12_id_tbl[sw->se1];
                mdl = &st12_mdl_no_00389A60[kind];
            break;
        case 0x19:
                kind = st25_id_tbl[sw->se1];
                mdl = &st25_mdl_no_00389A70[kind];
                ry = DEG2RAD(ANG2DEG(st25_rot_tbl[sw->se1]));
            break;
        case 0x1C:
                ry = 0.0f;
                kind = st28_id_tbl[sw->se1];
                mdl = &st28_mdl_no_00389A88[kind];
            break;
        case 0x1E:
                ry = 0.0f;
                kind = st30_id_tbl[sw->se1];
                mdl = &st30_mdl_no_00389A98[kind];
            break;
        }
        flmatMakeTrans(&mat, pr->pos[0], pr->pos[1], pr->pos[2]);
        switch (kind) {
        case 2:
            switch (sw->arg) {
            case 0xC:
                flmatInit(&uv);
                flmatSetTrans(&uv, 0.0f, 1.0f - (sw->se0 & 7) / 8.0f, 0.0f);
                flSetRenderState(0x19, (u32)&uv);
                break;
            case 0x19:
                flmatRotZ33(&mat, DEG2RAD(ANG2DEG(sw->se0 << 13)));
                break;
            }
            flmatRotY33(&mat, ry);
            break;
        case 1:
            flmatRotY33(&mat, DEG2RAD(ANG2DEG(sw->timer)));
            break;
        default:
            flmatRotY33(&mat, ry);
            break;
        }
        flSetRenderState(0x1A, (u32)&mat);
        if (*mdl != -1) {
            cl = &mw->clay[*mdl];
            if (cl != 0 && cl->handle != -1) {
                clay_attr_set(cl->attr);
                flExecuteClay(cl->handle, 0);
            }
        }
        clay_attr_reset();
    }
}

u8 set05_balli_id_ck(SETW *sw) {
    u8 base;

    switch (game_w.stage) {
    case 0xC:
        base = 2;
        break;
    case 0x19:
        base = 0;
        break;
    default:
        base = 0;
        break;
    }
    return sw->se1 - base;
}
