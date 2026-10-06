/* shell22 - game.bin 0x00637F60-0x006399C4. Cannon shots and falling rocks.
 * arg 0 is a ballista/cannon round fired by a player, arg 1 the fixed
 * stage cannons on stages 12 and 25 (positions and angles from tables,
 * with a little random spread), arg 2 and 3 debris, arg 4 rocks dropped on
 * stages 11, 28 and 30 when a monster stands in one of the marked areas
 * (shell22_rock_sel). Landing rocks shake the camera by distance
 * (shell22_rock_quake).
 * Near-match for the whole file: shell22_i is 67 instructions off (one
 * delay slot left unfilled shifts the rest) and shell22_h 3 (the
 * original tests case 1 explicitly; `case 0:` added here gives the same
 * layout but compares 0). */
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

static void shell22_move(SHLW *sh);
static void shell22_i(SHLW *sh);
static void shell22_m(SHLW *sh);
static void shell22_h(SHLW *sh);
static void shell22_d(SHLW *sh);
static void shell22_e(SHLW *sh);
static void disp_sub_00638EA0(CLAY *cl, FLMAT *m, void *mats);
static void shell22_trans(PRIM *pr);
static void shell22_se_req(SHLW *sh, s16 kind);
static int shell22_rock_sel(f32 *pos, f32 *out, u16 *dir);
static void shell22_rock_quake(SHLW *sh);

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

static void shell22_move(SHLW *sh) {
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

static void shell22_i(SHLW *sh) {
    PLW *pl = sh->owner;

    sh->mode++;
    sh->be_flag = 1;
    sh->trans = 0;
    sh->char0 = 0;
    if (sh->arg == 4) {
        shell_flag_set(sh, 0);
    } else {
        shell_flag_set(sh, 0x400);
    }
    sh->x7E = 0;
    if (pl == 0) {
        atck_data_set_shl(sh, sh->arg + 1, shell22_tbl);
    } else {
        pl_atck_data_set_shl(sh, pl, sh->arg + 1, shell22_tbl);
    }
    switch (sh->arg) {
    case 2:
        sh->x88 = shell22_body_tbl[sh->body + sh->x07];
        sh->x8C = 0;
        break;
    default:
        sh->x88 = shell22_body_tbl[sh->body];
        sh->x8C = 0;
        break;
    }
    flvecCopy(&sh->pos0, &sh->pos2);
    sh->rate[0] = shell22_rate_tbl[sh->arg][0];
    sh->rate[1] = shell22_rate_tbl[sh->arg][1];
    sh->rate[2] = shell22_rate_tbl[sh->arg][2];
    sh->rate_g[0] = 0.0f;
    sh->rate_g[1] = shell22_rate_g_tbl[sh->arg];
    sh->rate_g[2] = 0.0f;
    switch (sh->arg) {
    case 4:
        sh->rate[0] += rock_add_rate[0][sh->x07 & 3];
        sh->rate[1] += rock_add_rate[1][sh->x07 & 3];
        sh->rate[2] += rock_add_rate[2][sh->x07 & 3];
        break;
    case 1:
        sh->rate[0] += taihou_rand_tbl[(u16)sh->xB8 & 3];
        sh->rate[1] += 0.4f * taihou_rand_tbl[((u16)sh->xB8 >> 2) & 3];
        sh->rate[2] += taihou_rand_tbl[((u16)sh->xB8 >> 4) & 3];
        sh->rate_g[1] += 0.2f * taihou_rand_tbl[((u16)sh->xB8 >> 6) & 3];
        break;
    }
    flvecRotX(sh->rate, DEG2RAD(ANG2DEG(sh->ang[0])));
    flvecRotY(sh->rate, DEG2RAD(ANG2DEG(sh->ang[1])));
    shell22_se_req(sh, 0);
    switch (sh->arg) {
    default:
    case 0:
    case 1:
    case 4:
        sh->prim_no = get_prim();
        if (sh->prim_no != -1) {
            sh->prim = get_prim_ptr(sh->prim_no);
            sh->prim->owner = sh;
            sh->prim->trans = shell22_trans;
            flvecCopy(sh->prim->pos, &sh->pos2);
            switch (sh->arg) {
            case 0:
            case 1:
                break;
            default:
                add_prim(ot1, sh->prim, 0x20, 0);
                break;
            }
            if (sh->arg == 4 && sh->stg == game_w.stage) {
                shell22_rock_quake(sh);
            }
        } else {
            push_shell_work(sh);
        }
        break;
    case 2:
    case 3:
    case 5:
        sh->prim = 0;
        break;
    }
}

static void shell22_m(SHLW *sh) {
    f32 g;

    sh->char0++;
    flvecCopy(&sh->pos0, &sh->pos2);
    if (sh->x61 != 0) {
        sh->x61 = 0x63;
    }
    if (sh->char0 > shell22_all_time[sh->arg]) {
        sh->x61 = 0;
        sh->mode++;
        return;
    }
    switch (sh->arg) {
    default:
    case 0:
        if (sh->char0 < 5) {
            return;
        }
        if (sh->char0 < 13) {
            if (sh->char0 == 5) {
                shell22_se_req(sh, 1);
                if (sh->stg == game_w.stage) {
                    Eft18_set5(&sh->pos2.x, 10, sh->ang[0], sh->ang[1]);
                    Eft18_set5(&sh->pos2.x, 11, sh->ang[0], sh->ang[1]);
                }
            }
            shell_rate_add(sh);
        } else {
            shell_rate_add_g(sh);
        }
        if (sh->char0 > 5) {
            if (sh->pos2.y <= GetGroundShellHit(&sh->pos2)) {
                sh->x61 = 0;
                sh->mode++;
            }
        }
        break;
    case 1:
        switch (sh->x07) {
        case 0:
            if (sh->char0 > 10) {
                sh->x07++;
                shell22_se_req(sh, 1);
                if (sh->stg == game_w.stage) {
                    Eft18_set5(&sh->pos2.x, 10, sh->ang[0], sh->ang[1]);
                    Eft18_set5(&sh->pos2.x, 11, sh->ang[0], sh->ang[1]);
                }
            }
            return;
        case 1:
            shell_rate_add_g(sh);
            if (sh->char0 > 15) {
                if (sh->pos2.y <= GetGroundShellHit(&sh->pos2)) {
                    sh->x61 = 0;
                    sh->mode++;
                }
            }
            break;
        }
        break;
    case 3:
        if (++sh->x7E >= 25) {
            atck_data_set_shl(sh, sh->arg + 1, shell22_tbl);
            sh->x7E = 0;
        }
    case 2:
        if (sh->char0 < 5) {
            shell_rate_add_g(sh);
        }
        break;
    case 4:
        shell_rate_add_g(sh);
        if (sh->char0 > 5) {
            if (sh->char0 == 7) {
                shell22_se_req(sh, 1);
            }
            g = 50.0f + GetGroundShellHit(&sh->pos2);
            if (sh->pos2.y <= g) {
                sh->pos2.y = g;
                if (sh->stg == game_w.stage) {
                    shell22_rock_quake(sh);
                    shell22_se_req(sh, 2);
                    Eft02_set_pos2(0, 0xB, 0, &sh->pos2, 2.0f);
                    Eft02_set_pos2(0, 0xB, 1, &sh->pos2, 2.0f);
                    Eft13_set_pos2(16.0f, sh->owner, &sh->pos2, 0x22);
                }
                sh->x61 = 0;
                sh->xB = 0;
                sh->mode++;
            }
        }
        break;
    case 5:
        break;
    }
    if (sh->prim != 0) {
        flvecCopy(sh->prim->pos, &sh->pos2);
        add_prim(ot1, sh->prim, 0x20, 0);
    }
}

static void shell22_h(SHLW *sh) {
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

static void shell22_d(SHLW *sh) {
    sh->xB = 0;
    sh->mode = 6;
    sh->be_flag = 0;
    if (sh->prim != 0) {
        release_prim(sh->prim_no);
    }
}

static void shell22_e(SHLW *sh) {
    push_shell_work(sh);
}

static void disp_sub_00638EA0(CLAY *cl, FLMAT *m, void *mats) {
    flSetRenderState(0x1A, (u32)m);
    if (cl != 0 && cl->handle != -1) {
        Material_set_sub(mats, cl);
        clay_attr_set(cl->attr);
        flExecuteClay(cl->handle, 0);
    }
    clay_attr_reset();
}

static void shell22_trans(PRIM *pr) {
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

static void shell22_se_req(SHLW *sh, s16 kind) {
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

static int shell22_rock_sel(f32 *pos, f32 *out, u16 *dir) {
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

static void shell22_rock_quake(SHLW *sh) {
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
