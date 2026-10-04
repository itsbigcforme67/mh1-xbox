/* shell14 - game.bin 0x00635680-0x00635E68. em09's charge attack: a ball
 * that grows between two joints for ~320 frames, flickers and crackles,
 * then explodes (effect 14 + a shell09 blast). Ends early if em09 stops. */
#include "shell.h"
#include "game.h"
#include "prim.h"
#include "clay.h"
#include "fl.h"

#define ANG2DEG(a)  (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d)  (2.0f * (3.1415927f * ((d) / 360.0f)))

typedef struct EFT_MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x0F];
    s32 mat;            /* 0x10 */
    u8 _pad14[0x1C];
    CLAY *clay;         /* 0x30 */
} EFT_MDLW;

extern struct {
    EFT_MDLW *p;
    u8 _pad04[0x10];
} eft_mdlw;
extern s16 item_chr_tbl_00389C10[2];

void get_joint_pos_em(EMW *, int, VEC3 *);
int em09_status_ck(EMW *);
void flvecCopy(void *, void *);
void se_req2(int, int, int, VEC3 *, int, int);
void eft14_set(f32, VEC3 *, int);
void Shell09_set(VEC3 *, int, int);
void Eft02_set2(int, int, int, VEC3 *);
void release_prim(s16);
void flmatMakeScale(FLMAT *, f32, f32, f32);
void Material_set_sub(s32, CLAY *);

static void shell14_pos_get(SHLW *sh, EMW *em);
static void shell14_move(SHLW *sh);
static void shell14_i00(SHLW *sh);
static void shell14_m00(SHLW *sh);
static void shell14_i(SHLW *sh);
static void shell14_m(SHLW *sh);
static void shell14_d(SHLW *sh);
static void shell14_e(SHLW *sh);
static void shell14_trans(PRIM *pr);

void shell14_set(EMW *em, int arg) {
    SHLW *sh;

    if ((sh = pull_shell_work(0)) != 0) {
        sh->type = 14;
        sh->arg = arg;
        sh->move = shell14_move;
        sh->em_no = em->id;
        sh->x7A = em->x10;
        sh->owner = em;
        sh->xC8 = em->ang[1];
        sh->stg = em->stg;
        shell14_pos_get(sh, em);
        sh->x09 = 0;
    }
}

static void shell14_pos_get(SHLW *sh, EMW *em) {
    VEC3 a, b;

    get_joint_pos_em(em, 6, &a);
    get_joint_pos_em(em, 9, &b);
    sh->pos2.x = (a.x + b.x) / 2.0f;
    sh->pos2.y = 20.0f + (a.y + b.y) / 2.0f;
    sh->pos2.z = (a.z + b.z) / 2.0f;
    sh->ang[0] = em->ang[0];
    sh->ang[1] = em->ang[1];
    sh->ang[2] = em->ang[2];
}

static void shell14_move(SHLW *sh) {
    switch (sh->mode) {
    case 0:
        shell14_i(sh);
        break;
    case 1:
        shell14_m(sh);
        break;
    case 2:
        shell14_d(sh);
        break;
    case 3:
        shell14_d(sh);
        break;
    case 4:
        shell14_d(sh);
        break;
    case 5:
        shell14_d(sh);
        break;
    case 7:
        shell14_d(sh);
        break;
    case 6:
        shell14_e(sh);
        break;
    }
}

static void shell14_i00(SHLW *sh) {
    sh->xB = 0;
    sh->x7E = 340;
    sh->char0 = 30;
    sh->scale = 0.0f;
}

static void shell14_m00(SHLW *sh) {
    EMW *em = sh->owner;

    switch (sh->x05) {
    case 0:
        sh->scale += 1.0f / 30.0f;
        if (--sh->char0 <= 0) {
            sh->x05++;
            sh->scale = 1.0f;
        }
        if (em09_status_ck(em) != 1) {
            sh->be_flag ^= 1;
            sh->mode++;
            return;
        }
        break;
    case 1:
        if (sh->char0 >= 60) {
            sh->x05++;
            sh->x06 = 15;
            if (sh->stg == game_w.stage) {
                se_req2(1, 0x24, 0, &sh->pos2, 1, 0);
            }
        }
    case 2:
        if (++sh->char0 >= 340 || em09_status_ck(em) != 1) {
            if (sh->stg == game_w.stage) {
                eft14_set(1.0f, &sh->pos2, 0);
                Shell09_set(&sh->pos2, 0, sh->stg);
            }
            sh->be_flag ^= 1;
            sh->mode++;
            return;
        }
        if (sh->char0 >= 320) {
            sh->be_flag ^= 1;
        } else {
            if (sh->x07 == 0) {
                if (0.8f * sh->x7E > (f32)(320 - sh->char0)) {
                    sh->be_flag ^= 1;
                    if (320 - sh->char0 > 2) {
                        sh->x07 = 2;
                    }
                }
            } else if (--sh->x07 == 0) {
                sh->be_flag ^= 1;
                sh->x7E = 320 - sh->char0;
            }
            if (sh->stg == game_w.stage && sh->char0 >= 60) {
                if (sh->char0 & 1) {
                    Eft02_set2(0xC000, 0, 2, &sh->pos2);
                }
                if (--sh->x06 <= 0) {
                    sh->x06 = 15;
                    se_req2(1, 0x25, 0, &sh->pos2, 1, 0);
                }
            }
        }
        break;
    }
    shell14_pos_get(sh, em);
}

static void shell14_i(SHLW *sh) {
    sh->mode++;
    sh->be_flag = 1;
    sh->trans = 0;
    shell14_i00(sh);
    sh->prim_no = get_prim();
    if (sh->prim_no != -1) {
        sh->prim = get_prim_ptr(sh->prim_no);
        sh->prim->owner = sh;
        sh->prim->trans = shell14_trans;
        shell14_m(sh);
        return;
    }
    push_shell_work(sh);
}

static void shell14_m(SHLW *sh) {
    shell14_m00(sh);
    if (sh->prim != 0) {
        flvecCopy(&sh->prim->pos, &sh->pos2);
        add_prim(ot1, sh->prim, 0x20, 0);
    }
}

static void shell14_d(SHLW *sh) {
    sh->xB = 0;
    sh->mode = 6;
    sh->be_flag = 0;
    if (sh->prim != 0) {
        release_prim(sh->prim_no);
    }
}

static void shell14_e(SHLW *sh) {
    push_shell_work(sh);
}

static void shell14_trans(PRIM *pr) {
    FLMAT mat;
    SHLW *sh;
    CLAY *cl;
    s32 m;
    EFT_MDLW *mw;
    EMW *em;

    sh = pr->owner;
    mw = eft_mdlw.p;
    em = sh->owner;

    if (sh->be_flag != 0 && sh->stg == game_w.stage && mw != 0 && mw->flag != 0) {
        m = mw->mat;
        flmatMakeScale(&mat, sh->scale, sh->scale, sh->scale);
        flmatSetTrans(&mat, pr->pos[0], pr->pos[1], pr->pos[2]);
        flmatRotXYZ33(&mat, 0.0f, DEG2RAD(ANG2DEG(em->ang[1])), 0.0f);
        cl = &mw->clay[item_chr_tbl_00389C10[sh->arg]];
        flSetRenderState(0x1A, (u32)&mat);
        if (cl != 0 && cl->handle != -1) {
            Material_set_sub(m, cl);
            clay_attr_set(cl->attr);
            flExecuteClay(cl->handle, 0);
        }
        clay_attr_reset();
    }
}
