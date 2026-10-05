/* eft06c - SLPM_654.95 0x001056B0-0x00105B10: sound and spawners. See
 * eft06.c. Whole file 0x00102BD0-0x00105B10. Hit sparks on players and
 * monsters with ten types (arg): each spawns up to eft06_num[arg] sprites
 * (0x38-byte pieces in ew->work) placed on a joint of the owner (ew->stg),
 * with per-type start values (eft06_i) and keyframe animation (eft06_m,
 * eft06_t, still asm). Type 7 uses the second prim pool. Spawned by
 * Eft06_set (on a joint), Eft06_set2 (at a point) and Eft06_set_hit (on a
 * hit monster, sized by enemy_shadow/mahi tables of game.bin). Names of the
 * types are not known. */
#include "eft.h"
#include "em.h"
#include "prim.h"
#include "fl.h"

/* One sprite (0x38 bytes) of the work area. */
typedef struct EFT06_PIECE {
    s16 prim_no;        /* 0x00 */
    s16 no;             /* 0x02 sprite kind, 0xFF = unused */
    f32 scale[3];       /* 0x04 */
    f32 pos[3];         /* 0x10 */
    u32 col;            /* 0x1C */
    u16 rot[3];         /* 0x20 */
    s16 drot;           /* 0x26 */
    f32 size;           /* 0x28 */
    PRIM *prim;         /* 0x2C */
    s16 lag;            /* 0x30 frame counter, starts at or below 0 */
    u16 x32;            /* 0x32 */
    u16 x34;            /* 0x34 */
    u16 x36;            /* 0x36 */
} EFT06_PIECE;

extern s16 eft06_num[10];
extern s16 eft06_type3_lag_tbl[6];
extern s16 eft06_type4_lag_tbl[3];
extern f32 eft06_em_scale[35];
extern f32 D_63BD60[];      /* game.bin enemy_mahi_size */

u32 ran_suu(int);
u8 Pl_stg_ck(void *);
int Pl_master_ck(void *);
s16 get_prim2(void);
PRIM *get_prim_ptr2(s16);
void release_prim(s16);
void release_prim2(s16);
FLMAT *get_joint_wmat(void *, int);
void get_joint_pos(void *, int, f32 *);
void flmatCopy(FLMAT *, FLMAT *);
void flvecCopy(f32 *, f32 *);
void flvecApplyMat33_2(f32 *, FLMAT *);
void se_req2(int, int, int, f32 *, int, int);
void Pl_se_req2(void *, int, int, f32 *, int, int);

void eft06_move(EFTW *ew);
void eft06_i(EFTW *ew);
void eft06_m(EFTW *ew);
void eft06_d(EFTW *ew);
void eft06_e(EFTW *ew);
void eft06_t(PRIM *pr);
void eft06_se_req(EFTW *ew, f32 *pos);
void Eft06_set(void *chr, s16 arg, int x05, int joint, f32 scale);

void eft06_se_req(EFTW *ew, f32 *pos) {
    switch (ew->arg) {
    case 0:
        se_req2(1, 0x39, 0, pos, 1, 0);
        break;
    case 3:
        se_req2(1, 0x23, 0, pos, 1, 0);
        break;
    case 5:
    case 9:
        se_req2(1, 0x22, 0, pos, 1, 0);
        break;
    case 6:
        switch (ew->x07) {
        case 0:
            Pl_se_req2(ew->owner, 0xC, 0, ew->pos, 1, 0);
            break;
        case 1:
            Pl_se_req2(ew->owner, 0xD, 0, ew->pos, 1, 0);
            break;
        case 2:
            Pl_se_req2(ew->owner, 0xE, 0, ew->pos, 1, 0);
            break;
        }
        break;
    }
}

EFTW *eft06_set_com(void *chr, s16 arg, int x05) {
    EFTW *ew;

    if (arg != 7 && Pl_stg_ck(chr) == 0) {
        return 0;
    }
    ew = pull_eft_work(1);
    if (ew == 0) {
        return 0;
    }
    ew->type = 6;
    ew->move = eft06_move;
    ew->owner = chr;
    ew->arg = arg;
    ew->mode2 = x05;
    return ew;
}

void Eft06_set(void *chr, s16 arg, int x05, int joint, f32 scale) {
    EFTW *ew;

    if (arg == 6 && Pl_master_ck(chr) == 0) {
        return;
    }
    if ((ew = eft06_set_com(chr, arg, x05)) != 0) {
        ew->scale = scale;
        get_joint_pos(chr, joint, ew->pos);
        ew->stg = joint;
        if (arg == 0) {
            Eft06_set(chr, 1, x05, joint, scale);
        } else if (arg == 7) {
            ew->prim2 = 1;
        }
    }
}

void Eft06_set2(f32 scale, void *chr, s16 arg, int joint, f32 *pos) {
    EFTW *ew = eft06_set_com(chr, arg, 0);

    if (ew != 0) {
        ew->scale = scale;
        ew->stg = joint;
        flvecCopy(ew->pos, pos);
    }
}

void Eft06_set_hit(EMW *em, s16 arg, int x05) {
    EFTW *ew;

    if ((ew = eft06_set_com(em, arg, x05)) != 0) {
        ew->stg = 2;
        get_joint_pos(em, ew->stg, ew->pos);
        switch (ew->arg) {
        case 0:
            if (em->x10 == 0) {
                ew->scale = 4.0f;
            } else {
                ew->scale = em->scale[0] * eft06_em_scale[em->kind];
            }
            Eft06_set(em, 1, x05, ew->stg, ew->scale);
            break;
        case 9:
            if (em->x10 == 0) {
                ew->scale = 0.75f;
            } else {
                ew->scale = em->scale[0] * D_63BD60[em->kind];
            }
            break;
        default:
            ew->scale = 1.0f;
            break;
        }
    }
}
