/* shell22 - game.bin 0x00637F60-0x006384F0: Shell22_set to shell22_move.
 * shell22_i and shell22_h are still assembly (near-matches in
 * shell22_nm.c, which also says what the shell does); the rest is in
 * shell22b.c and shell22c.c. */
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

void Shell22_set(PLW *pl, u8 arg) {
    f32 v[3];
    SHLW *sh;
    f32 (*pos)[3];
    u16 *ang;

    if ((sh = pull_shell_work(0)) != 0) {
        sh->type = 0x16;
        sh->arg = arg;
        sh->move = shell22_move;
        sh->em_no = pl->id;
        sh->x7A = pl->x10;
        sh->owner = pl;
        sh->stg = pl->stg;
        sh->x06 = pl->cnt39A;
        sh->xB8 = (pl->cnt39A & 0xFF00) >> 8;
        switch (arg) {
        case 0:
            sh->pos2.x = pl->pos[0];
            sh->pos2.y = pl->pos[1];
            sh->pos2.z = pl->pos[2];
            sh->ang[0] = 0;
            sh->ang[1] = pl->ang[1];
            v[0] = 0.0f;
            v[1] = 100.0f;
            v[2] = 250.0f;
            flvecRotY(v, DEG2RAD(ANG2DEG(sh->ang[1])));
            sh->pos2.x += v[0];
            sh->pos2.y += v[1];
            sh->pos2.z += v[2];
            break;
        case 1:
            switch (sh->stg) {
            case 0xC:
                pos = shell22_st12_taihou_pos;
                ang = shell22_st12_taihou_ang;
                break;
            case 0x19:
                pos = shell22_st25_taihou_pos;
                ang = shell22_st25_taihou_ang;
                break;
            default:
                push_shell_work(sh);
                return;
            }
            flvecCopy(&sh->pos2, pos[sh->x06]);
            sh->ang[0] = 0xF05C;
            sh->ang[1] = ang[sh->x06];
            v[0] = 0.0f;
            v[1] = 50.0f;
            v[2] = 255.0f;
            flvecRotX(v, DEG2RAD(ANG2DEG(sh->ang[0])));
            flvecRotY(v, DEG2RAD(ANG2DEG(sh->ang[1])));
            sh->pos2.x += v[0];
            sh->pos2.y += v[1];
            sh->pos2.z += v[2];
            break;
        }
        sh->xC8 = sh->ang[1];
    }
}

void Shell22_set2(f32 *pos, u8 arg, int stg, u16 ang) {
    SHLW *sh;
    s16 n;
    s16 i;

    switch (arg) {
    case 2:
        n = 4;
        break;
    default:
        n = 1;
        break;
    }
    for (i = 0; i < n; i++) {
        sh = pull_shell_work(0);
        if (sh != 0) {
            sh->type = 0x16;
            sh->arg = arg;
            sh->move = shell22_move;
            sh->em_no = 0;
            sh->x7A = 0;
            sh->owner = 0;
            sh->xC8 = ang;
            sh->pos2.x = pos[0];
            sh->pos2.y = pos[1];
            sh->pos2.z = pos[2];
            sh->stg = stg;
            sh->ang[0] = 0;
            sh->ang[1] = ang;
            sh->x06 = 0;
            sh->x07 = i;
        }
    }
}

void Shell22_set3(EMW *em, int arg, int x07) {
    u16 dir;
    f32 p[3];
    SHLW *sh;

    if (Em_stg_ck(em) != 0 && shell22_rock_sel(em->pos, p, &dir) != 0) {
        sh = pull_shell_work(0);
        if (sh != 0) {
            sh->type = 0x16;
            sh->arg = arg;
            sh->move = shell22_move;
            sh->em_no = 0;
            sh->x7A = 1;
            sh->owner = em;
            sh->xC8 = dir;
            sh->pos2.x = p[0];
            sh->pos2.y = p[1];
            sh->pos2.z = p[2];
            sh->stg = em->stg;
            sh->ang[0] = 0;
            sh->ang[1] = dir;
            sh->x06 = 0;
            sh->x07 = x07;
        }
    }
}

void shell22_move(SHLW *sh) {
    switch (sh->mode) {
    case 0:
        shell22_i(sh);
        break;
    case 1:
        shell22_m(sh);
        break;
    case 2:
        shell22_d(sh);
        break;
    case 3:
        shell22_h(sh);
        break;
    case 4:
        shell22_d(sh);
        break;
    case 5:
        shell22_d(sh);
        break;
    case 7:
        shell22_d(sh);
        break;
    case 6:
        shell22_e(sh);
        break;
    }
}
