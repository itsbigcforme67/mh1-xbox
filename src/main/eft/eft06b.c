/* eft06b - SLPM_654.95 0x00104D30-0x00104E18: eft06_d / eft06_e. See
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

void eft06_d(EFTW *ew) {
    EFT06_PIECE *p = ew->work;
    s16 n;
    s16 i;

    ew->mode++;
    n = eft06_num[ew->arg];
    if (n == 0) {
        if (ew->prim != 0) {
            if (ew->prim2 != 0) {
                release_prim2(ew->prim_no);
            } else {
                release_prim(ew->prim_no);
            }
        }
    } else {
        for (i = 0; i < n; i++, p++) {
            if (p->prim != 0) {
                release_prim(p->prim_no);
            }
        }
    }
}

void eft06_e(EFTW *ew) {
    push_eft_work(ew);
}

