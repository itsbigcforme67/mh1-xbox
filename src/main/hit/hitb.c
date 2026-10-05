/* hitb - SLPM_654.95 0x00114280-0x00114318: dm_vec_calc. See hit.c.
 * Whole file 0x00111B20-0x00114A88. Shell (attack) hit checks: every
 * frame each live shell is tested against monsters and players and against
 * other shells. A hit fills the target's damage fields (dm_val per body
 * part, ailment build-up, knock-back direction), plays the hit sound and
 * spawns the hit-mark effects (eft02/06/14/15/16/18). Sharpness (s_gauge_tbl)
 * and monster meat values (em_meat_tbl) scale blade damage.
 * Field names in include/hit.h are mostly by offset. */
#include "hit.h"
#include "pl.h"
#include "em.h"
#include "game.h"

typedef struct EM_MEAT {
    u8 v[8];            /* 1/2 cut/blunt %, 3 shot %, 4-7 ailment % */
} EM_MEAT;

extern EM_MEAT *em_meat_tbl[];
extern f32 s_gauge_tbl[];
extern f32 slash_adj_tbl[];
extern s16 slash_minus_tbl[4];
extern s16 hit_se_tbl[];
extern HBODY *D_63FA10[];     /* game.bin: monster body tables by kind */
extern HSHL *shell_w_top;
extern char lit_951_00358250[];
extern char lit_952_00358270[];
extern char lit_953_00358290[];
extern char lit_954_003582B0[];

void Hit_id_init(void);
u8 Get_hit_id(void);
int pl_flag_ck(void *, int);
void pl_flag_set(void *, int);
void pl_flag_clr(void *, int);
void pl_atck_data_set2(void *, int);
void pl_atck_data_set_shl2(HSHL *, void *, int);
int shell_flag_ck(HSHL *, int);
int softdip_ck(int);
u8 Pl_stg_ck(void *);
int Pl_master_ck(void *);
void Pl_se_req2_com(void *, int, int, f32 *, int, int);
void Pl_slash_calc(void *, int);
void vib_set_pl(void *, int);
void printf(char *, ...);
int fptodp(f32);
void pl_body_make(void *, HCAP *, f32);
void hit_cap_pk(HCAP *, HPK *);
s16 hit_data_expand(void *, HBODY *, HCAP *, HSPH *);
s16 hit_data_expand2(f32 *, HBODY *, HCAP *, HSPH *);
s16 hit_data_expand3(f32 *, f32 *, HBODY *, HCAP *);
u8 hit_cap_cap2_m(HPK *, HPK *, f32 *);
u8 hit_cap_sphr2_m(HPK *, HSPH *, f32 *, f32);
u8 hit_sphr_cap_m(HSPH *, HPK *, f32 *, f32);
u8 hit_sphr_sphr2(HSPH *, HSPH *, f32 *, f32, f32);
u16 calc_vec_ang2(f32 *, f32 *);
void body_ptr_ck(HSHL *sh, HBODY **pb);
void eft02_set(void *, int, f32 *);
void Eft02_set3(void *, int, int, int, f32 *, f32);
void Eft02_set5(void *, s16, int, int, f32 *, f32);
void Eft06_set_hit(void *, s16, int);
void func_54B7E0(f32 *, s16, f32);              /* game.bin eft14_set */
void func_54D480(f32 *, int, int, void *, f32); /* game.bin Eft15_set */
void func_5508F0(void *, int, int, f32 *, f32); /* game.bin Eft16_set */
void func_550DD0(void *, f32 *, int, int, int, f32); /* game.bin Eft16_set_impact */
void func_5547B0(f32 *, s16, int);              /* game.bin Eft18_set2 */
int func_639FC0(u16, u16);                      /* game.bin Guard_dir_ck */

void dm_vec_calc(HSHL *sh, HCHR *c, f32 *a, f32 *b) {
    if (sh->x65 & 0x10) {
        c->dm_ang = calc_vec_ang2(a, b) + 0x4000;
    } else {
        c->dm_ang = sh->ang + (u16)(s32)(0.5f + 65536.0f * sh->x66 / 360.0f);
    }
}

