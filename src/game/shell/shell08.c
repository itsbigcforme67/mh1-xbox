/* shell08 - game.bin 0x0062D4D0-0x0062EA1C: Shell08_set_ang to shell08_i.
 * Monster projectiles (breath, fireballs, lightning): spawners, mode
 * machine and init. shell08_m and shell08_trans are still assembly
 * (near-match of shell08_m in shell08_nm.c); the rest is in shell08b-d.c. */
#include "shell08.h"
#include "game.h"

void get_joint_pos_em(EMW *, int, VEC3 *);
void flvecCopy(void *, void *);
void release_prim(s16);
void Material_set_sub(void *, CLAY *);
void Eft_rendope_set(int);
FLMAT *get_joint_wmat(void *, int);
void flmatCopy(void *, void *);
void flvecApplyMat33_2(f32 *, FLMAT *);
f32 flArcTan2(f32, f32);
f32 flSqrt(f32);
f32 flAbs(f32);
void PointToPoint(f32 *, f32 *, f32 *);
void flvecOuterProduct(f32 *, f32 *, f32 *);
void flvecNormalize(f32 *);
f32 flvecInnerProduct(f32 *, f32 *);
void Em_se_req2(EMW *, int, int, f32 *, int, int);
u16 ran_suu(int);
void pl_atck_data_set_shl(SHLW *, void *, int, u8 *);
void flvecRotY(f32 *, f32);
void AddVector(f32 *, f32 *, f32 *);
f32 GetGroundShellHit(VEC3 *);
void Eft04_set_pos(f32 *, int, int, f32);
FLMAT *get_joint_wmat_em(EMW *, int);
u16 calc_mat_angY(FLMAT *);
void flvecRotX(f32 *, f32);
f32 GetGroundHit(VEC3 *);
void eft_vec_linear(f32, void *, f32 *);
void Eft17_type3_set(f32 *, u16, u16, int, int);
void flmatGetTrans(f32 *, FLMAT *);
void set_quake_sub2(int);
void shell_rate_add_g(SHLW *);
f32 GetGroundShellHit(VEC3 *);
void eft14_set(f32 *, int, f32);
void Eft17_set_ex(f32 *, u16, int, f32);
void Eft17_set_pos(f32 *, int, int, f32);
void Eft17_set_pos_ang(f32 *, int, int, u16, f32);
void Eft18_set2(f32 *, int, int);
void Shell09_set_em(EMW *, f32 *, int);
void set_quake_sub(int, f32 *);

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))
#define DEG2ANG(d) ((u16)(s32)(0.5f + 65536.0f * (d) / 360.0f))

extern EMW em_work[];
extern FLMAT rview_mat;
extern f32 D_3F2080[3];
extern void *shell08_data[];
extern s16 shell08_all_time[];
extern s16 shell08_index[];
extern s16 shell08_type6_time[];
extern f32 shell08_ofs_tbl[][3];
extern f32 shell08_rate_tbl[][3];
extern f32 shell08_rate_g_tbl[][3];
extern s32 shell08_body_tbl[];
extern s16 shell08_num[];
extern s16 shell08_type8_thunder_num[];
extern s16 rotx_em01[3];
extern s16 roty_em01[3];
extern f32 D_3F2090[3];
extern s16 shell08_atk_no[];
extern u8 shell08_tbl[4];
extern f32 (*shell08_type8_thunder_pos[])[2];


void Shell08_set_ang(EMW *em, s16 joint, u8 arg, u8 x07, u16 ax, u16 ay) {
    SHLW *sh;

    if ((sh = pull_shell_work(1)) != 0) {
        sh->type = 8;
        sh->arg = arg;
        sh->x07 = x07;
        sh->move = shell08_move;
        sh->em_no = em->id;
        sh->x7A = em->x10;
        sh->owner = em;
        sh->xC8 = em->ang[1];
        get_joint_pos_em(em, joint, &sh->pos2);
        sh->char0 = joint;
        sh->x7E = em->kind;
        sh->ang[0] = ax;
        sh->ang[1] = ay + em->ang[1];
        sh->x05 = 1;
        sh->x06 = 0;
    }
}

void Shell08_set_ang_time(EMW *em, s16 joint, u8 arg, u8 x07, u16 ax, u16 ay, u8 time) {
    SHLW *sh;

    if ((sh = pull_shell_work(1)) != 0) {
        sh->type = 8;
        sh->arg = arg;
        sh->x07 = x07;
        sh->move = shell08_move;
        sh->em_no = em->id;
        sh->x7A = em->x10;
        sh->owner = em;
        sh->xC8 = em->ang[1];
        get_joint_pos_em(em, joint, &sh->pos2);
        sh->char0 = joint;
        sh->x7E = em->kind;
        sh->ang[0] = ax;
        sh->ang[1] = ay + em->ang[1];
        sh->x05 = 1;
        sh->x06 = time;
    }
}

void Shell08_set_shl(EMW *em, u8 arg, u8 x07, SHLW *src, f32 *pos) {
    SHLW *sh;

    if ((sh = pull_shell_work(1)) != 0) {
        sh->type = 8;
        sh->arg = arg;
        sh->x07 = x07;
        sh->move = shell08_move;
        sh->em_no = src->em_no;
        sh->x7A = src->x7A;
        sh->owner = src->owner;
        sh->xC8 = src->xC8;
        flvecCopy(&sh->pos2, pos);
        sh->x7E = src->x7E;
        sh->ang[0] = 0;
        sh->ang[1] = src->ang[1];
        sh->x05 = 2;
        sh->x06 = 0;
    }
}

void shell08_move(SHLW *sh) {
    switch (sh->mode) {
    case 0:
        shell08_i(sh);
        break;
    case 1:
        shell08_m(sh);
        break;
    case 2:
        shell08_d(sh);
        break;
    case 3:
        shell08_h(sh);
        break;
    case 4:
        shell08_d(sh);
        break;
    case 5:
        shell08_d(sh);
        break;
    case 7:
        shell08_d(sh);
        break;
    case 6:
        shell08_e(sh);
        break;
    }
}

void shell08_i(SHLW *sh) {
    FLMAT m;
    f32 v[3];
    SH08W *w;
    SH08P *p;
    EMW *em;
    s16 num;
    s16 i;

    w = SH08_W(sh);
    em = &em_work[sh->em_no];
    sh->mode++;
    p = w->p;
    sh->be_flag = 1;
    sh->trans = 0;
    shell_flag_set(sh, 0x200);
    sh->xB4 = 0;
    sh->stg = em->stg;
    if (sh->arg != 8) {
        pl_atck_data_set_shl(sh, em, sh->x07 + shell08_atk_no[sh->arg], shell08_tbl);
    }
    sh->x88 = shell08_body_tbl[sh->body];
    sh->x8C = 0;
    w->x00 = 0;
    w->x01 = 0;
    w->x36 = 0;
    w->joint = sh->char0;
    w->x2E = 0;
    sh->char0 = 0;
    if (sh->x05 != 2) {
        flmatCopy(&m, get_joint_wmat_em(em, w->joint));
        v[0] = shell08_ofs_tbl[sh->arg][0];
        v[1] = shell08_ofs_tbl[sh->arg][1];
        v[2] = shell08_ofs_tbl[sh->arg][2];
        flvecApplyMat33_2(v, &m);
        sh->pos2.x += v[0];
        sh->pos2.y += v[1];
        sh->pos2.z += v[2];
        flvecCopy(&sh->pos0, &sh->pos2);
    }
    if (sh->x05 == 0) {
        sh->rate[0] = shell08_rate_tbl[sh->arg][0];
        sh->rate[1] = shell08_rate_tbl[sh->arg][1];
        sh->rate[2] = shell08_rate_tbl[sh->arg][2];
        flvecApplyMat33_2(sh->rate, &m);
        sh->rate_g[0] = shell08_rate_g_tbl[sh->arg][0];
        sh->rate_g[1] = shell08_rate_g_tbl[sh->arg][1];
        sh->rate_g[2] = shell08_rate_g_tbl[sh->arg][2];
        sh->ang[1] = calc_mat_angY(&m) + 0x4000;
    } else {
        sh->rate[0] = shell08_rate_tbl[sh->arg][0];
        sh->rate[1] = shell08_rate_tbl[sh->arg][1];
        sh->rate[2] = shell08_rate_tbl[sh->arg][2];
        flvecRotX(sh->rate, DEG2RAD(ANG2DEG(sh->ang[0])));
        flvecRotY(sh->rate, DEG2RAD(ANG2DEG(sh->ang[1])));
        sh->rate_g[0] = shell08_rate_g_tbl[sh->arg][0];
        sh->rate_g[1] = shell08_rate_g_tbl[sh->arg][1];
        sh->rate_g[2] = shell08_rate_g_tbl[sh->arg][2];
    }
    sh->x05 = 0;
    sh->prim_no = get_prim();
    if (sh->prim_no != -1) {
        sh->prim = get_prim_ptr(sh->prim_no);
        sh->prim->owner = sh;
        sh->prim->no = 0;
        sh->prim->trans = shell08_trans;
    } else {
        sh->prim = 0;
    }
    w->prim = 0;
    num = shell08_num[sh->arg];
    switch (sh->arg) {
    case 0:
    case 11:
        w->prim_no = get_prim();
        if (w->prim_no != -1) {
            w->prim = get_prim_ptr(w->prim_no);
            w->prim->owner = sh;
            w->prim->no = 1;
            w->prim->trans = shell08_trans;
        } else {
            w->prim = 0;
        }
        for (i = 0; i < num; i++) {
            p->no = i;
            p->x02 = 0;
            if (i < 3) {
                p->x10 = 0;
                p->x12 = 0;
                if (i == 0) {
                    p->x14 = ran_suu(1);
                } else {
                    p->x14 = 0;
                    p->x16 = rotx_em01[i - 1] + DEG2ANG(0.1f * ((ran_suu(1) & 0x1F) - 16));
                    p->x18 = roty_em01[i - 1] + DEG2ANG(0.1f * ((ran_suu(1) & 0x1F) - 16));
                }
            } else {
                p->x02 = 0;
                p->x10 = 0xC000;
                p->x16 = 0;
                p->x18 = 0;
                p->x1A = 0;
            }
            p++;
        }
        break;
    case 1:
        w->x01 = sh->ang[0] >> 8;
        w->x37 = (sh->ang[1] - em->ang[1]) >> 8;
        w->char0 = ((EMW *)sh->owner)->char0;
        shell08_type1_pos_set(sh, w);
        for (i = 0; i < num; i++, p++) {
            p->no = i;
            p->x02 = 0;
            p->x10 = 0;
            p->x12 = 0;
            p->x14 = 0;
            p->x16 = 0;
            p->x18 = 0;
            p->x1A = 0;
        }
        break;
    case 2:
    case 3:
    case 9:
        w->prim_no = get_prim();
        if (w->prim_no != -1) {
            w->prim = get_prim_ptr(w->prim_no);
            w->prim->owner = sh;
            w->prim->no = 1;
            w->prim->trans = shell08_trans;
        } else {
            w->prim = 0;
        }
        for (i = 0; i < num; i++, p++) {
            p->no = 0;
            p->x02 = -i * 4;
            p->x10 = 0;
            p->x12 = 0;
            p->x14 = ran_suu(1);
            p->x1A = 0x199A;
            p->x16 = 0;
            p->x18 = 0;
        }
        break;
    case 4:
        for (i = 0; i < num; i++, p++) {
            p->no = i;
            switch (p->no) {
            case 0:
                p->x1A = 0x1000;
                break;
            default:
                p->x1A = 0;
                break;
            }
            p->x02 = 0;
            p->x16 = 0;
            p->x18 = 0;
            p->x1C = 0;
            p->x1D = 0;
            p->x1E = 0;
            p->x1F = 0;
            p->x10 = 0;
            p->x12 = 0;
            p->x14 = 0;
        }
        break;
    case 5:
        for (i = 0; i < num; i++, p++) {
            p->no = i;
            p->x02 = 0;
            p->x10 = 0;
            p->x12 = 0;
            p->x14 = 0;
            p->x16 = 0;
            p->x18 = 0;
            p->x1A = 0;
        }
        break;
    case 6:
        sh->pos2.y = em->x5AC;
        Em_se_req2(sh->owner, 0x37, 0, &sh->pos2.x, 1, 0);
        w->prim_no = get_prim();
        if (w->prim_no != -1) {
            w->prim = get_prim_ptr(w->prim_no);
            w->prim->owner = sh;
            w->prim->no = 1;
            w->prim->trans = shell08_trans;
        } else {
            w->prim = 0;
        }
        for (i = 0; i < num; i++, p++) {
            p->no = i;
            if (num <= 0) {
                p->x02 = 0;
                p->x16 = 0;
                p->x18 = 0;
                p->x1A = 0;
                p->x1C = 0;
                p->x1D = 0;
                p->x1E = 0;
                p->x1F = 0;
                p->x10 = 0;
                p->x12 = 0;
                p->x14 = 0;
            } else {
                if (i == 5) {
                    p->no = 4;
                    p->x02 = -5;
                }
                p->x14 = ran_suu(1);
            }
        }
        break;
    case 7:
        for (i = 0; i < num; i++, p++) {
            p->no = i;
            p->x02 = 0;
            p->x10 = 0;
            p->x12 = 0;
            p->x14 = 0;
        }
        break;
    case 8:
        sh->x06 = ran_suu(1);
        sh->x06 = (sh->x06 & 1) | ((((sh->x06 >> 3) % 3) << 3) | (((sh->x06 >> 1) % 3) << 1));
        w->x00 = shell08_type8_thunder_num[sh->x07 % 6] + 1;
        flvecCopy(&w->base, ((EMW *)sh->owner)->pos);
        for (i = 0; i < num; i++) {
            p->no = 0;
            p->x02 = 0;
            p->x10 = 0;
            p->x12 = 0;
            p->x14 = 0;
            if (i == 0) {
                shell08_type8_init(sh, w, p);
            } else {
                p->x02 = -20;
            }
        }
        break;
    case 10:
        sh->pos2.y = GetGroundHit(&sh->pos2);
        for (i = 0; i < num; i++, p++) {
            p->no = i;
            p->x02 = 0;
            p->x16 = 0;
            p->x18 = 0;
            p->x1A = 0;
            p->x1C = 0;
            p->x1D = 0;
            p->x1E = 0;
            p->x1F = 0;
            p->x10 = 0;
            p->x12 = 0;
            p->x14 = 0;
        }
        break;
    }
    shell08_m(sh);
}
