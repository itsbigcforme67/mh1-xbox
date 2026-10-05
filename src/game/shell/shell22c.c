/* shell22 - game.bin 0x00638E50-0x006399C0: shell22_d to
 * shell22_rock_quake. See shell22.c. */
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

void shell22_d(SHLW *sh) {
    sh->xB = 0;
    sh->mode = 6;
    sh->be_flag = 0;
    if (sh->prim != 0) {
        release_prim(sh->prim_no);
    }
}

void shell22_e(SHLW *sh) {
    push_shell_work(sh);
}

void disp_sub_00638EA0(CLAY *cl, FLMAT *m, void *mats) {
    flSetRenderState(0x1A, (u32)m);
    if (cl != 0 && cl->handle != -1) {
        Material_set_sub(mats, cl);
        clay_attr_set(cl->attr);
        flExecuteClay(cl->handle, 0);
    }
    clay_attr_reset();
}

void shell22_trans(PRIM *pr) {
    FLMAT m;
    FLMAT uv;
    SHLW *sh = pr->owner;
    MDLW *mw;
    CLAY *cl;
    void *mats;

    if (game_w.stage == sh->stg) {
        switch (sh->arg) {
        case 0:
            mw = set_mdlw;
            if (mw == 0 || mw->flag == 0) {
                goto end;
            }
            flmatMakeScale(&m, 2.0f, 2.0f, 1.0f);
            mats = mw->mat;
            switch (sh->stg) {
            case 0xC:
                cl = &mw->clay[4];
                break;
            case 0x19:
                cl = &mw->clay[13];
                break;
            default:
                return;
            }
            flmatRotY33(&m, DEG2RAD(ANG2DEG(sh->ang[1])));
            break;
        case 1:
            mw = eft_mdlw[0];
            if (mw == 0 || mw->flag == 0) {
                goto end;
            }
            flmatInit(&m);
            cl = &mw->clay[133];
            mats = mw->mat;
            flmatMakeTrans(&uv, 0.0f, 0.0f, 0.0f);
            flSetRenderState(0x19, (u32)&uv);
            break;
        case 4:
            mw = set_mdlw;
            if (mw == 0 || mw->flag == 0) {
                goto end;
            }
            flmatInit(&m);
            mats = mw->mat;
            switch (sh->stg) {
            case 0xB:
            case 0x1E:
                cl = &mw->clay[2];
                break;
            case 0x1C:
                cl = &mw->clay[24];
                break;
            default:
                return;
            }
            break;
        default:
            return;
        }
        flmatSetTrans(&m, pr->pos[0], pr->pos[1], pr->pos[2]);
        disp_sub_00638EA0(cl, &m, mats);
    }
end:;
}

void shell22_se_req(SHLW *sh, s16 kind) {
    EMW *em = em_work;
    s16 i;

    if (sh->stg == game_w.stage) {
        switch (sh->arg) {
        case 0:
            switch (kind) {
            case 0:
                se_req2(7, 0x38, 0, &sh->pos2.x, 10, 0);
                break;
            case 1:
                se_req2(7, 0x30, 0, &sh->pos2.x, 10, 0);
                break;
            }
            break;
        case 1:
            switch (kind) {
            case 0:
                break;
            case 1:
                se_req2(7, 0x32, 0, &sh->pos2.x, 10, 0);
                break;
            }
            break;
        case 2:
        case 3:
            switch (kind) {
            case 0:
                se_req2(7, 0x34, 0, &sh->pos2.x, 10, 0);
                break;
            }
            break;
        case 4:
            for (i = 0; i < 20; i++, em++) {
                if (em->be_flag != 0 && em->x01 != 0 && em->kind == 7) {
                    break;
                }
            }
            switch (kind) {
            case 0:
                Em_se_req2(em, 0x2C, 0, &sh->pos2.x, 3, 0);
                break;
            case 1:
                Em_se_req2(em, 0x2D, 0, &sh->pos2.x, 3, 0);
                break;
            case 2:
                Em_se_req2(em, 0x2B, 0, &sh->pos2.x, 3, 0);
                break;
            }
            break;
        }
    }
}

int shell22_rock_sel(f32 *pos, f32 *out, u16 *dir) {
    s16 n;

    switch (game_w.stage) {
    case 0xB:
        if (pos[0] > 12000.0f && pos[0] < 16000.0f && pos[2] > 20500.0f && pos[2] < 23000.0f) {
            n = 0;
        } else if (pos[0] > 12000.0f && pos[0] < 16000.0f && pos[2] > 26000.0f && pos[2] < 29000.0f) {
            n = 1;
        } else if (pos[0] > 16500.0f && pos[0] < 19000.0f && pos[2] > 28000.0f && pos[2] < 32000.0f) {
            n = 2;
        } else {
            return 0;
        }
        flvecCopy(out, st11_pos_tbl_006839D0[n]);
        *dir = st11_dir_tbl[n];
        return 1;
    case 0x1C:
        if (pos[0] > 14000.0f && pos[0] < 17000.0f && pos[2] > 18000.0f && pos[2] < 25000.0f) {
            n = 0;
        } else if (pos[0] > 17000.0f && pos[0] < 20000.0f && pos[2] > 22000.0f && pos[2] < 29000.0f) {
            n = 1;
        } else {
            return 0;
        }
        flvecCopy(out, st28_pos_tbl_00683A00[n]);
        *dir = st28_dir_tbl[n];
        return 1;
    case 0x1E:
        if (pos[0] > 9000.0f && pos[0] < 13000.0f && pos[2] > 21000.0f && pos[2] < 26000.0f) {
            n = 0;
        } else if (pos[0] > 14000.0f && pos[0] < 18000.0f && pos[2] > 21000.0f && pos[2] < 26000.0f) {
            n = 1;
        } else if (pos[0] > 20000.0f && pos[0] < 25000.0f && pos[2] > 20000.0f && pos[2] < 25000.0f) {
            n = 2;
        } else {
            return 0;
        }
        flvecCopy(out, st30_pos_tbl_00683A20[n]);
        *dir = st30_dir_tbl[n];
        return 1;
    default:
        return 0;
    }
}

void shell22_rock_quake(SHLW *sh) {
    f32 d = flvecCalcDistance(&sh->pos2, D_3F2090);

    if (d < 500.0f) {
        set_quake_sub2(5);
    } else if (d < 1000.0f) {
        set_quake_sub2(4);
    } else if (d < 1500.0f) {
        set_quake_sub2(3);
    } else if (d < 2000.0f) {
        set_quake_sub2(2);
    } else if (d < 2500.0f) {
        set_quake_sub2(1);
    } else if (d < 3000.0f) {
        set_quake_sub2(0);
    }
}
