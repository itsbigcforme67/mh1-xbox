/* shell22_r01 - near-match fixes: shell22_h. Whole file in shell22_nm.c. 0x00638D50-0x00638E50: shell22_h. Whole file in shell22_nm.c. */
#include "shell.h"
#include "game.h"
#include "pl.h"
#include "prim.h"
#include "clay.h"
#include "fl.h"

typedef struct MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x0F];
    void *mat;          /* 0x10 material table */
    u8 _pad14[0x1C];
    CLAY *clay;         /* 0x30 */
} MDLW;

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

extern MDLW *set_mdlw;
extern MDLW *eft_mdlw[5];
extern EMW em_work[];
extern f32 D_3F2090[3];
extern u8 shell22_tbl[4];
extern s32 shell22_body_tbl[];
extern f32 shell22_rate_tbl[][3];
extern f32 shell22_rate_g_tbl[];
extern s16 shell22_all_time[];
extern f32 rock_add_rate[3][4];
extern f32 taihou_rand_tbl[4];
extern f32 shell22_st12_taihou_pos[][3];
extern f32 shell22_st25_taihou_pos[][3];
extern u16 shell22_st12_taihou_ang[2];
extern u16 shell22_st25_taihou_ang[2];
extern f32 st11_pos_tbl_006839D0[3][3];
extern f32 st28_pos_tbl_00683A00[2][3];
extern f32 st30_pos_tbl_00683A20[3][3];
extern u16 st11_dir_tbl[3];
extern u16 st28_dir_tbl[2];
extern u16 st30_dir_tbl[3];

u8 Em_stg_ck(EMW *);
void release_prim(s16);
void flvecCopy(void *, void *);
void flvecRotX(f32 *, f32);
void flvecRotY(f32 *, f32);
f32 flvecCalcDistance(void *, void *);
void flmatInit(FLMAT *);
void Material_set_sub(void *, CLAY *);
void flExecuteClay(s32, int);
int shell_flag_set(SHLW *, int);
void atck_data_set_shl(SHLW *, int, u8 *);
void pl_atck_data_set_shl(SHLW *, void *, int, u8 *);
void shell_rate_add(SHLW *);
void shell_rate_add_g(SHLW *);
f32 GetGroundShellHit(VEC3 *);
void Eft18_set5(f32 *, int, u16, u16);
void Eft02_set_pos2(int, int, int, VEC3 *, f32);
void Eft13_set_pos2(f32, void *, VEC3 *, int);
void set_quake_sub2(int);
void se_req2(int, int, int, f32 *, int, int);
void Em_se_req2(EMW *, int, int, f32 *, int, int);

void shell22_move(SHLW *sh);
void shell22_i(SHLW *sh);
void shell22_m(SHLW *sh);
void shell22_h(SHLW *sh);
void shell22_d(SHLW *sh);
void shell22_e(SHLW *sh);
void disp_sub_00638EA0(CLAY *cl, FLMAT *m, void *mats);
void shell22_trans(PRIM *pr);
void shell22_se_req(SHLW *sh, s16 kind);
int shell22_rock_sel(f32 *pos, f32 *out, u16 *dir);
void shell22_rock_quake(SHLW *sh);















void shell22_h(SHLW *sh) {
    switch (sh->arg) {
    case 0:
    case 1:
    default:
        shell22_d(sh);
        break;
    case 2:
    case 3:
        shell22_m(sh);
        break;
    case 4:
        if (sh->stg == game_w.stage) {
            shell22_rock_quake(sh);
            Eft02_set_pos2(0, 0xB, 0, &sh->pos2, 2.0f);
            Eft02_set_pos2(0, 0xB, 1, &sh->pos2, 2.0f);
            Eft13_set_pos2(16.0f, sh->owner, &sh->pos2, 0x22);
        }
        sh->x61 = 0;
        sh->xB = 0;
        sh->mode++;
        shell22_d(sh);
        break;
    }
}
