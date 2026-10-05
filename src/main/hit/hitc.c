/* hitc - SLPM_654.95 0x00114880-0x00114A88: shell hit timers. See hit.c.
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

void hit_data_on_shl_sub(HSHL *sh, HCHR *pl) {
    pl->x41C = &sh->hit_time;
}

void hit_shell_minus(HSHL *sh) {
    s8 n = (s8)(sh->hit_cnt & 0xF) - 1;

    sh->hit_cnt = (sh->hit_cnt & 0xF0) | n;
    if (sh->hit_cnt >> 4 != 0) {
        sh->hit_time = sh->hit_cnt >> 4;
        sh->hit_mode = 1;
        sh->hit_id = Get_hit_id();
    }
}

static void hit_timer_calc_shl_sub(HSHL *sh) {
    switch (sh->hit_mode) {
    case 0:
        break;
    case 1:
        if (sh->hit_time-- != 0) {
            break;
        }
        sh->hit_time = 0;
        sh->hit_mode++;
    case 2:
        if (sh->hit_wait == 0) {
            if (sh->x73 != 0) {
                pl_atck_data_set_shl2(sh, &player_work[sh->no], sh->x73);
                sh->hit_mode = 1;
                if (sh->hit_time == 0) {
                    sh->hit_mode = 2;
                    sh->hit_wait--;
                } else {
                    sh->hit_mode = 1;
                    break;
                }
            } else {
                sh->hit_mode = 0;
                sh->hit_time = 0;
                sh->hit_wait = 0;
                break;
            }
        } else {
            sh->hit_wait--;
        }
        if (sh->x75 != 0 && sh->x75 != 0xFF) {
            sh->x75--;
        } else if (sh->x76 != 0) {
            sh->x76--;
        }
        break;
    }
}

void hit_timer_calc_shl(void) {
    HSHL *sh = shell_w_top;

    if (sh != 0) {
        do {
            if (sh->be_flag != 0 && sh->x7B == 0) {
                hit_timer_calc_shl_sub(sh);
            }
            sh = sh->next;
        } while (sh != 0);
    }
}
