/* eft20d - SLPM_654.95 0x0021CD40-0x0021D464: spawners and eft20_water_set.
 * Part of eft20 (whole file 0x00218590-0x0021D464). Monster dust and debris (30
 * types, arg): up to eft20_num[arg] pieces (0x30 bytes) per effect, placed by
 * eft20_pos_set from the monster's kind and joints, or at a player's foot
 * (Eft20_set_pl). On water the effect becomes Eft08 splashes
 * (eft20_water_ck / eft20_water_set; game.bin Eft08_set called by
 * address). The big functions (i, m, t,
 * pos_set) are still asm. */
#include "eft.h"
#include "em.h"
#include "pl.h"
#include "game.h"
#include "prim.h"
#include "fl.h"

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

/* One piece (0x30 bytes) of the work area. */
typedef struct EFT20_PIECE {
    u8 _pad00[2];
    s16 prim_no;        /* 0x02 */
    u8 _pad04[0x2C - 0x04];
    PRIM *prim;         /* 0x2C */
} EFT20_PIECE;

extern s16 eft20_num[30];
extern s16 eft20_water_flag[30];
extern s16 Eft_stg_type[];

u8 Pl_stg_ck(void *);
u8 Em_stg_ck(void *);
void release_prim(s16);
void get_joint_pos(void *, int, f32 *);
void flvecCopy(f32 *, f32 *);
void flvecRotY(f32 *, f32);
int GetWaterHit(f32 *, f32 *);
void se_req2(int, int, int, f32 *, int, int);
void func_544C90(f32 *, int, int, f32);    /* game.bin Eft08_set */

void eft20_move(EFTW *ew);
void eft20_i(EFTW *ew);
void eft20_m(EFTW *ew);
void eft20_d(EFTW *ew);
void eft20_e(EFTW *ew);
s16 eft20_water_ck(EMW *chr, f32 *pos, s16 arg);
s16 eft20_pos_set(f32 *pos, EMW *em, int arg, int x07);
s8 eft20_water_set(void *chr, f32 *pos, s16 kind, f32 scale);

void Eft20_set(f32 scale, EMW *em, int arg, int x07) {
    f32 pos[3];
    EFTW *ew;
    s16 k;

    if (Em_stg_ck(em) != 0) {
        k = eft20_pos_set(pos, em, arg, x07);
        if (eft20_water_set(em, pos, k, scale) == 0) {
            if ((ew = pull_eft_work(1)) != 0) {
                ew->type = 20;
                ew->move = eft20_move;
                ew->arg = arg;
                ew->owner = em;
                ew->scale = scale;
                flvecCopy(ew->pos, pos);
                ew->x07 = x07;
                ew->stg = 0;
            }
        }
    }
}

void Eft20_set2(f32 scale, f32 *pos, int arg, int ang) {
    EFTW *ew;

    if ((ew = pull_eft_work(1)) != 0) {
        ew->type = 20;
        ew->move = eft20_move;
        ew->arg = arg;
        ew->owner = 0;
        ew->scale = scale;
        ew->x07 = 0;
        ew->u0A.joint = ang;
        flvecCopy(ew->pos, pos);
        ew->stg = 2;
    }
}

void Eft20_set_pl(f32 scale, PLW *pl, s16 arg, s16 x07) {
    f32 pos[3];
    f32 v[3];
    EFTW *ew;
    s16 k;

    if (Pl_stg_ck(pl) != 0) {
        switch (arg) {
        case 2:
            get_joint_pos(pl, 8, pos);
            pos[1] = pl->x5AC;
            if (x07 == 2) {
                v[0] = 0.0f;
                v[1] = 0.0f;
                v[2] = 50.0f;
                flvecRotY(v, DEG2RAD(ANG2DEG(pl->ang[1])));
                pos[0] += v[0];
                pos[1] += v[1];
                pos[2] += v[2];
            } else if (x07 == 3) {
                v[0] = 0.0f;
                v[1] = 0.0f;
                v[2] = -50.0f;
                flvecRotY(v, DEG2RAD(ANG2DEG(pl->ang[1])));
                pos[0] += v[0];
                pos[1] += v[1];
                pos[2] += v[2];
            }
            break;
        case 3:
            get_joint_pos(pl, 5, pos);
            pos[1] = pl->x5AC;
            if (x07 == 2) {
                v[0] = 0.0f;
                v[1] = 0.0f;
                v[2] = 50.0f;
                flvecRotY(v, DEG2RAD(ANG2DEG(pl->ang[1])));
                pos[0] += v[0];
                pos[1] += v[1];
                pos[2] += v[2];
            } else if (x07 == 3) {
                v[0] = 0.0f;
                v[1] = 0.0f;
                v[2] = -50.0f;
                flvecRotY(v, DEG2RAD(ANG2DEG(pl->ang[1])));
                pos[0] += v[0];
                pos[1] += v[1];
                pos[2] += v[2];
            }
            break;
        }
        k = eft20_water_ck((EMW *)pl, pos, arg);
        if (eft20_water_set(pl, pos, k, scale) == 0) {
            if ((ew = pull_eft_work(1)) != 0) {
                ew->type = 20;
                ew->move = eft20_move;
                ew->arg = arg;
                ew->owner = (EMW *)pl;
                flvecCopy(ew->pos, pos);
                ew->scale = scale;
                ew->x07 = x07;
                ew->stg = 1;
            }
        }
    }
}

s8 eft20_water_set(void *chr, f32 *pos, s16 kind, f32 scale) {
    f32 sc;

    switch (kind) {
    case 0:
        return 0;
    case 1:
    default:
        sc = 5.0f * scale;
        func_544C90(pos, 4, 0, sc);
        pos[1] += 5.0f;
        func_544C90(pos, 5, 0, sc);
        break;
    case 2:
        func_544C90(pos, 4, 0, 5.0f * scale);
        pos[1] += 5.0f;
        sc = 4.5f * scale;
        func_544C90(pos, 1, 0, sc);
        break;
    case 3:
        sc = 10.0f * scale;
        pos[1] += 5.0f;
        func_544C90(pos, 2, 0, sc);
        break;
    }
    if (sc > 4.0f) {
        se_req2(1, 0x76, 0, pos, 1, 0);
    } else {
        se_req2(1, 0x70, 0, pos, 1, 0);
    }
    return 1;
}
