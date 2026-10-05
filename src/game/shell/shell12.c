/* shell12 - game.bin 0x00634460-0x0063523C. A trap set by a player (arg
 * 0 pitfall, otherwise shock trap). It takes 55 frames to set up, then
 * waits up to about 150 seconds for a monster in a catchable state to come
 * within 700 units, holds it, and springs or collapses when it breaks free.
 * game_w.trap_num counts the master player's live traps. */
#include "shell.h"
#include "game.h"
#include "pl.h"
#include "prim.h"
#include "clay.h"
#include "fl.h"

typedef struct SET_MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x2F];
    CLAY *clay;         /* 0x30 */
} SET_MDLW;

typedef struct EFT_MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x0F];
    void *mat;          /* 0x10 material table */
    u8 _pad14[0x1C];
    CLAY *clay;         /* 0x30 */
} EFT_MDLW;

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

extern SET_MDLW *set_mdlw;
extern EFT_MDLW *eft_mdlw[5];
extern s16 *stg_eft_mdl_no[];
extern u8 shell12_tbl[4];
extern s32 shell12_body_tbl[1];

u32 ran_suu(int);
int Pl_master_ck(PLW *);
void atck_data_set_shl(SHLW *, int, u8 *);
void flvecRotY(f32 *, f32);
f32 flvecCalcDistance(void *, void *);
void flmatInit(FLMAT *);
int get_prim2(void);
PRIM *get_prim_ptr2(s16);
void release_prim2(s16);
void Material_set_sub(void *, CLAY *);
void Eft02_set2(int, int, int, VEC3 *);
void Eft20_set2(VEC3 *, int, int, f32);
void Eft17_set_ex(VEC3 *, int, int, f32);
void se_req2(int, int, int, f32 *, int, int);

static void shell12_move(SHLW *sh);
static void shell12_i(SHLW *sh);
static void shell12_m(SHLW *sh);
static void shell12_d(SHLW *sh);
static void shell12_e(SHLW *sh);
static void shell12_trans_sub(CLAY *cl, FLMAT *mat);
static void shell12_trans(PRIM *pr);
static void shell12_se_req(SHLW *sh, s16 kind);

void Shell12_set(PLW *pl, int arg) {
    SHLW *sh;
    f32 v[3];

    if ((sh = pull_shell_work(0)) != 0) {
        sh->type = 12;
        sh->move = shell12_move;
        sh->arg = arg;
        v[0] = 0.0f;
        v[1] = 0.0f;
        v[2] = 80.0f;
        flvecRotY(v, DEG2RAD(ANG2DEG(pl->ang[1])));
        sh->pos2.x = pl->pos[0] + v[0];
        sh->pos2.y = pl->pos[1] + v[1];
        sh->pos2.z = pl->pos[2] + v[2];
        sh->stg = pl->stg;
        sh->x09 = 1;
        if (Pl_master_ck(pl) != 0) {
            game_w.trap_num++;
            sh->x07 = 1;
        }
    }
}

static void shell12_move(SHLW *sh) {
    switch (sh->mode) {
    case 0:
        shell12_i(sh);
        break;
    case 1:
        shell12_m(sh);
        break;
    case 2:
        shell12_d(sh);
        break;
    case 3:
        shell12_m(sh);
        break;
    case 4:
        shell12_m(sh);
        break;
    case 5:
        shell12_d(sh);
        break;
    case 7:
        shell12_d(sh);
        break;
    case 6:
        shell12_e(sh);
        break;
    }
}

static void shell12_i(SHLW *sh) {
    sh->mode++;
    sh->be_flag = 1;
    sh->trans = 0;
    sh->x05 = 0;
    sh->x06 = 0;
    sh->char0 = 0;
    sh->x7E = 0;
    sh->x7A = 0;
    shell_flag_set(sh, 0);
    atck_data_set_shl(sh, sh->arg + 1, shell12_tbl);
    sh->x88 = shell12_body_tbl[0];
    sh->x8C = 0;
    sh->prim_no = get_prim2();
    if (sh->prim_no != -1) {
        sh->prim = get_prim_ptr2(sh->prim_no);
        sh->prim->owner = sh;
        sh->prim->trans = shell12_trans;
    } else {
        shell12_e(sh);
    }
}

static void shell12_m(SHLW *sh) {
    EMW *em = sh->x9C;
    EMW *e;
    s16 i;
    u8 found = 0;
    s16 t;

    sh->char0++;
    switch (sh->x06) {
    case 0:
        t = sh->char0;
        if (t < 55) {
            if (sh->stg == game_w.stage) {
                if (!(t & 7)) {
                    Eft02_set2(0xC000, 0, 2, &sh->pos2);
                }
                if (!(sh->char0 & 0xF)) {
                    shell12_se_req(sh, 0);
                }
            }
        } else if (t == 55 && sh->stg == game_w.stage) {
            Eft20_set2(&sh->pos2, 0xC, ran_suu(1), 0.8f);
            Eft17_set_ex(&sh->pos2, ran_suu(1), 0xA, 0.4f);
            Eft20_set2(&sh->pos2, 0x14, ran_suu(1), 2.0f);
            shell12_se_req(sh, 1);
        }
        if (sh->char0 >= 60) {
            sh->x06++;
            shell12_se_req(sh, 2);
        }
        break;
    case 1:
        sh->x7E++;
        sh->x61 = 99;
        if (sh->char0 > 121) {
            sh->x06++;
        }
        break;
    case 2:
        if (sh->char0 >= 0x23A1) {
            sh->xB = 0;
            sh->mode = 2;
            return;
        }
        sh->x61 = 99;
        if (sh->x1E != 0) {
            found = 1;
        } else {
            for (e = em_work, i = 0; i < 20; i++, e++) {
                if (e->be_flag != 0 && e->stg == sh->stg && e->x9EA >= 20 &&
                    flvecCalcDistance(&sh->pos2, e->pos) <= 700.0f) {
                    sh->x9C = e;
                    em = e;
                    found = 2;
                    break;
                }
            }
        }
        if (found) {
            if (em == 0) {
                sh->x05 = 0;
            } else {
                if (found == 1) {
                    if (sh->arg == 0) {
                        em->x959 = 6;
                    } else {
                        em->x959 = 9;
                    }
                }
                sh->x05 = 1;
            }
            sh->x06++;
            sh->x7E = 0;
            sh->xB = 0;
        }
        break;
    case 3:
        if (++sh->x7E > 5) {
            if (sh->x05 == 0) {
                sh->x06 = 6;
            } else {
                sh->x06++;
            }
            sh->x7E = 0;
        }
        break;
    case 4:
        if (++sh->x7E > 60) {
            sh->xB = 0;
            sh->x7E = 0;
            sh->x06 = 6;
        } else if (em == 0) {
            sh->x05 = 0;
            sh->x06 = 6;
            sh->xB = 0;
            sh->x7E = 0;
        } else if (em->be_flag == 0) {
            sh->x05 = 0;
            sh->x06 = 6;
            sh->xB = 0;
            sh->x7E = 0;
        } else if (em->x9EA != 0) {
            sh->xB = 0;
            sh->x7E = 0;
            sh->x06++;
        }
        break;
    case 5:
        if (em == 0) {
            sh->x05 = 0;
            sh->x06 = 6;
            sh->xB = 0;
            sh->x7E = 0;
        } else if (em->be_flag == 0) {
            sh->x05 = 0;
            sh->x06 = 6;
            sh->xB = 0;
            sh->x7E = 0;
        } else if (em->x9EA <= 0) {
            sh->xB = 0;
            sh->x7E = 0;
            sh->x06++;
        }
        break;
    case 6:
        if (++sh->x7E > 5) {
            sh->xB = 0;
            sh->mode = 2;
            return;
        }
        break;
    }
    sh->prim->pos[0] = sh->pos2.x;
    sh->prim->pos[1] = sh->pos2.y;
    sh->prim->pos[2] = sh->pos2.z;
    add_prim(ot1, sh->prim, 0x20, 0);
}

static void shell12_d(SHLW *sh) {
    sh->xB = 0;
    sh->mode = 6;
    sh->be_flag = 0;
    release_prim2(sh->prim_no);
}

static void shell12_e(SHLW *sh) {
    if (sh->x07 != 0) {
        game_w.trap_num--;
    }
    push_shell_work(sh);
}

static void shell12_trans_sub(CLAY *cl, FLMAT *mat) {
    flSetRenderState(0x1A, (u32)mat);
    if (cl != 0 && cl->handle != -1) {
        clay_attr_set(cl->attr);
        flExecuteClay(cl->handle, 0);
    }
}

static void shell12_trans(PRIM *pr) {
    FLMAT mat;
    FLMAT uv;
    SHLW *sh = pr->owner;
    SET_MDLW *mw = set_mdlw;
    EFT_MDLW *ew = eft_mdlw[0];
    void *mats;
    s16 mdl;
    s16 t;
    f32 sy, uvy, sx, sz;

    if (sh->stg == game_w.stage && mw != 0 && mw->flag != 0) {
        switch (sh->x06) {
        case 0:
            break;
        case 1:
        case 2:
            t = sh->x7E;
            sx = 1.0f;
            sz = sx;
            if (t < 15) {
                uvy = sx - 0.3f * (t / 15.0f);
            } else if (t < 30) {
                uvy = 0.7f - 0.2f * ((t - 15) / 15.0f);
            } else {
                uvy = 0.5f;
            }
            if (t > 61) {
                sy = 0.16f;
            } else if (t > 15) {
                sy = 1.0f - 0.84f * flSin(1.5707964f * (t - 15) / 46.0f);
            } else {
                sy = 1.0f;
            }
            break;
        case 3:
            t = sh->x7E;
            sx = 1.0f - t / 5.0f;
            sy = 0.16f * sx;
            uvy = 0.5f;
            sz = sx;
            break;
        case 4:
        case 5:
            sx = 0.0f;
            sy = sx;
            sz = sx;
            uvy = sx;
            break;
        case 6:
            sx = 1.0f - (5 - sh->x7E) / 5.0f;
            sy = 0.0f;
            sz = sy;
            uvy = sy;
            break;
        }
        mdl = *stg_eft_mdl_no[sh->stg];
        if (sh->x06 < 3) {
            mats = ew->mat;
            flSetRenderState(0x60, 0x80);
            flmatInit(&mat);
            flmatSetTrans(&mat, sh->pos2.x, sh->pos2.y, sh->pos2.z);
            Material_set_sub(mats, &ew->clay[116]);
            shell12_trans_sub(&ew->clay[116], &mat);
        }
        if (sh->x06 != 4 && sh->x06 != 5 && sh->x06 != 6) {
            flSetRenderState(0x60, 0);
            flmatMakeScale(&mat, sx, sy, sz);
            flmatSetTrans(&mat, sh->pos2.x, sh->pos2.y, sh->pos2.z);
            flmatMakeTrans(&uv, 0.0f, uvy, 0.0f);
            flSetRenderState(0x19, (u32)&uv);
            shell12_trans_sub(&mw->clay[mdl], &mat);
            if (sh->arg == 0) {
                flSetRenderState(0x67, -1);
            } else {
                flSetRenderState(0x67, 0xFFFF4040);
            }
            flmatMakeScale(&mat, sx, sy, sz);
            flmatSetTrans(&mat, sh->pos2.x, 2.0f + sh->pos2.y, sh->pos2.z);
            flSetRenderState(0x19, (u32)&uv);
            shell12_trans_sub(&mw->clay[mdl] + 1, &mat);
        }
        flSetRenderState(0x60, 0x80);
        if (sh->x06 == 3 || sh->x06 == 4 || sh->x06 == 5 || sh->x06 == 6) {
            flmatMakeScale(&mat, 1.0f - sx, 1.0f - sx, 1.0f - sx);
            flmatSetTrans(&mat, sh->pos2.x, sh->pos2.y, sh->pos2.z);
            shell12_trans_sub(&mw->clay[mdl] + 2, &mat);
        }
        clay_attr_reset();
    }
}

static void shell12_se_req(SHLW *sh, s16 kind) {
    if (sh->stg == game_w.stage) {
        switch (kind) {
        case 0:
            se_req2(1, 0x25, 0, &sh->pos2.x, 1, 0);
            break;
        case 1:
            se_req2(1, 0x26, 0, &sh->pos2.x, 1, 0);
            break;
        case 2:
            se_req2(1, 0x46, 0, &sh->pos2.x, 1, 0);
            break;
        }
    }
}
