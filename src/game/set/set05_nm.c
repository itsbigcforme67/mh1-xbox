/* NONMATCHING: set05_m (0x0061FB50), not built. 2 instructions off: the
 * shell type n is set with `andi a1, a0, 0xFF` (case 0xC, reusing the
 * register that holds 2) and `daddiu a1, 3`, which is what a u8 local gives,
 * but declaring n as u8 moves every temporary register (68 off). int n
 * keeps the registers and loses the two constant loads. A permuter run
 * found nothing better. The rest of this file matches and is built from
 * set05.c (before set05_m) and set05b.c (after). */
/* set05 - game.bin 0x0061F7E0-0x006206C4. Turnable / firing stage
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

static void set05_move(SETW *sw);
static void set05_i(SETW *sw);
static void set05_m(SETW *sw);
static void set05_d(SETW *sw);
static void set05_e(SETW *sw);
static void set05_trans(PRIM *pr);
static u8 set05_balli_id_ck(SETW *sw);

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

static void set05_move(SETW *sw) {
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

static void set05_m(SETW *sw) {
    PLW *pl = player_work;
    u16 found = 0;
    f32 v[3];
    u8 on[4];
    s16 i;
    u32 ang;
    u8 kind;
    int n;

    v[0] = 0.0f;
    v[1] = 0.0f;
    v[2] = 0.0f;
    switch (sw->arg) {
    case 0xB:
        kind = st11_id_tbl[sw->se1];
        break;
    case 0xC:
        kind = st12_id_tbl[sw->se1];
        break;
    case 0x19:
        kind = st25_id_tbl[sw->se1];
        break;
    case 0x1C:
        kind = st28_id_tbl[sw->se1];
        break;
    case 0x1E:
        kind = st30_id_tbl[sw->se1];
        break;
    }
    sw->cnt++;
    switch (kind) {
    case 2:
        sw->timer++;
        switch (sw->arg) {
        case 0xC:
            n = 2;
            ang = 0;
            break;
        case 0x19:
            n = 3;
            ang = st25_rot_tbl[sw->se1];
            break;
        }
        switch (sw->mode2) {
        case 0:
            if (game_w.x1B2 != 0) {
                sw->mode2++;
                sw->timer = 0;
            }
            break;
        case 1:
            if (sw->timer > 45) {
                sw->timer = 0;
                sw->mode2++;
                Shell22_set2(sw->pos, n, game_w.stage, ang);
            }
            break;
        case 2:
            sw->se0++;
            v[2] = 1300.0f * (sw->timer / 4.0f);
            if (sw->timer >= 4) {
                se_req2(7, 0x36, 0, sw->pos, 10, 0);
                sw->timer = 0;
                sw->mode2++;
            }
            break;
        case 3:
            sw->se0++;
            v[2] = 1300.0f;
            if (sw->timer >= 90) {
                sw->timer = 0;
                sw->mode2++;
            }
            break;
        case 4:
            sw->se0++;
            v[2] = 1300.0f - 1300.0f * (sw->timer / 150.0f);
            if (sw->timer >= 150) {
                sw->timer = 0;
                sw->mode2 = 5;
            }
            break;
        case 5:
            break;
        }
        flvecRotY(v, DEG2RAD(ANG2DEG(ang)));
        break;
    case 1:
        if (sw->se0 > 0) {
            sw->se0--;
        }
        for (i = 0; i < 4; i++, pl++) {
            on[i] = 0;
            if (pl->be_flag != 0 && pl->x01 != 0 && Pl_stg_ck(pl) != 0 && pl->char0 == 0x19E &&
                frame_check2(pl, 0, 66.0f) != 0) {
                on[i] = 1;
                if (Pl_master_ck(pl) != 0) {
                    found = 1;
                }
            }
        }
        if (found) {
            pl = &player_work[game_w.master];
            v[0] = sw->pos[0];
            v[1] = pl->pos[1];
            v[2] = sw->pos[2];
            if (flvecCalcDistance(pl->pos, v) <= 200.0f) {
                if (sw->se0 == 0 && (u16)sw->timer != pl->ang[1]) {
                    se_req2(7, set05_balli_id_ck(sw) + 0x3A, 0, sw->pos, 1, 0);
                    sw->se0 = 10;
                }
                sw->timer = pl->ang[1];
                sw->x1E = game_w.master;
                goto clear;
            }
        }
        for (pl = player_work, i = 0; i < 4; i++, pl++) {
            if (on[i] != 0 && pl->be_flag != 0 && pl->x01 != 0 && Pl_stg_ck(pl) != 0 && Pl_master_ck(pl) == 0) {
                v[0] = sw->pos[0];
                v[1] = pl->pos[1];
                v[2] = sw->pos[2];
                if (flvecCalcDistance(pl->pos, v) <= 200.0f) {
                    if (sw->se0 == 0 && (u16)sw->timer != pl->ang[1]) {
                        se_req2(7, set05_balli_id_ck(sw) + 0x3A, 0, sw->pos, 1, 0);
                        sw->se0 = 10;
                    }
                    sw->timer = pl->ang[1];
                    sw->x1E = i;
                    goto clear;
                }
            }
        }
        switch (sw->arg) {
        case 0xC:
            sw->timer = 0;
            break;
        case 0x19:
            sw->timer = st25_rot_tbl[sw->se1];
            break;
        }
    clear:
        v[0] = 0.0f;
        v[1] = 0.0f;
        v[2] = 0.0f;
        break;
    }
    sw->prim->pos[0] = sw->pos[0] + v[0];
    sw->prim->pos[1] = sw->pos[1] + v[1];
    sw->prim->pos[2] = sw->pos[2] + v[2];
    add_prim(ot1, sw->prim, 0x20, 0);
}

static void set05_d(SETW *sw) {
    sw->mode++;
    sw->be_flag = 0;
    release_prim(sw->prim_no);
}

static void set05_e(SETW *sw) {
    push_set_work(sw);
}

static void set05_trans(PRIM *pr) {
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

static u8 set05_balli_id_ck(SETW *sw) {
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
