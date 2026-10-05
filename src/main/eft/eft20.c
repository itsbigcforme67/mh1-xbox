/* eft20 - SLPM_654.95 0x00218590-0x0021866C: eft20_move, eft20_se_req.
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
s16 eft20_pos_set(f32 *pos, EMW *em, int arg, int x07);
s8 eft20_water_set(void *chr, f32 *pos, s16 kind, f32 scale);

void eft20_move(EFTW *ew) {
    switch (ew->mode) {
    case 0:
        eft20_i(ew);
        break;
    case 1:
        eft20_m(ew);
        break;
    case 2:
        eft20_d(ew);
        break;
    case 3:
        eft20_e(ew);
        break;
    }
}

void eft20_se_req(EFTW *ew) {
    switch (ew->arg) {
    case 0x11:
        se_req2(1, 0x2C, 0, ew->pos, 1, 0);
        break;
    }
}

