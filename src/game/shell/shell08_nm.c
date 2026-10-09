/* shell08 near-matches, NOT built (kept for later work):
 * - shell08_m (0x0062EA20): logic believed complete, ~1000/1898
 *   instructions differ, nearly all register allocation (the original keeps
 *   flag in fp and spills w->p / em to the stack differently).
 * - shell08_rgba (0x00632AC0): colour-key blend, 43/236 differ (channel
 *   extraction order / registers).
 * - shell08_trans (0x006309A0, appended at the end of this file): C written from
 *   the asm, same size as the original (1580 instructions), 1544 differ
 *   (registers, block order). */
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


void shell08_m(SHLW *sh) {
    f32 d3[3];
    f32 v2[3];
    f32 v[3];
    SH08W *w;
    EMW *ow;
    f32 y;
    f32 d;
    s16 step;
    s16 t;
    s16 i;
    void *dat;
    SH08P *p;
    EMW *em;
    SH08P *top;
    s16 idx;
    s16 all;
    s16 num;
    int flag;

    em = &em_work[sh->em_no];
    w = SH08_W(sh);
    top = w->p;
    p = top;
    num = shell08_num[sh->arg];
    all = shell08_all_time[sh->arg];
    idx = shell08_index[sh->arg];
    flag = 0;
    /* The original leaves f20 (the ground height) unset on frames > 33 of type 0/11 and
     * still places the glow at 10 + f20: stale register garbage. Keep the glow on the shot. */
    y = sh->pos2.y - 10.0f;
    if (sh->arg != 8) {
        sh->x61 = 0x63;
    }
    switch (sh->arg) {
    case 0:
    case 6:
    case 11:
        step = 1;
        sh->pos0.x = sh->pos2.x;
        sh->pos0.y = sh->pos2.y;
        sh->pos0.z = sh->pos2.z;
        break;
    case 1:
        ow = sh->owner;
        if (ow->be_flag == 0 || ow->x04 == 3) {
            sh->xB = 0;
            sh->x61 = 0;
            sh->mode++;
            return;
        }
        if (w->char0 != ow->char0) {
            sh->xB = 0;
            sh->x61 = 0;
            sh->mode++;
            return;
        }
        all = sh->x06;
        step = 1;
        switch (sh->x05) {
        case 0:
            shell08_type1_pos_set(sh, w);
            t = 10;
            if (++w->x2E > 10) {
                t = all - 30;
                w->x2E = 0;
                for (i = 0; i < num; i++) {
                    p[i].x02 = 0;
                }
                sh->x05++;
            }
            break;
        case 1:
            shell08_type1_pos_set(sh, w);
            t = all - 30;
            if (++w->x2E > t) {
                w->x2E = 0;
                t = 20;
                for (i = 0; i < num; i++) {
                    p[i].x02 = 0;
                }
                sh->x05++;
            }
            break;
        case 2:
            shell08_type1_pos_set(sh, w);
            t = 20;
            if (++w->x2E > 20) {
                sh->xB = 0;
                sh->x61 = 0;
                sh->mode++;
                return;
            }
            break;
        }
        break;
    case 2:
    case 3:
    case 9:
        step = 1;
        t = 10;
        sh->pos0.x = sh->pos2.x;
        sh->pos0.y = sh->pos2.y;
        sh->pos0.z = sh->pos2.z;
        break;
    case 4:
        if (w->x36 != 0) {
            switch (sh->x05) {
            case 0:
                w->x2E = 0;
                for (i = 0; i < num; i++) {
                    p[i].x02 = 0;
                    switch (p[i].no) {
                    case 0:
                        p[i].x14 = 0;
                        p[i].x1A = 0;
                        break;
                    case 1:
                        p[i].x14 = 0;
                        p[i].x1A = 0x1000;
                        break;
                    case 2:
                        p[i].x14 = 0;
                        p[i].x1A = 0x1000;
                        break;
                    }
                }
                break;
            case 1:
                w->x2E = 0;
                for (i = 0; i < num; i++) {
                    p[i].x02 = 0;
                }
                break;
            case 2:
                w->x2E = 0;
                for (i = 0; i < num; i++) {
                    p[i].x02 = 0;
                }
                sh->xB = 0;
                break;
            }
            sh->x05++;
            w->x36 = 0;
        }
        switch (sh->x05) {
        case 0:
            step = 2;
            t = 11;
            idx = 5;
            if (++w->x2E >= 11) {
                w->x36 = 1;
                if (sh->stg == game_w.stage) {
                    Eft17_set_pos_ang(&sh->pos0.x, 7, ((EMW *)sh->owner)->kind, sh->ang[1], 0.3f);
                }
            }
            break;
        case 1:
            step = 1;
            t = 7;
            idx = 6;
            if (++w->x2E >= 7) {
                w->x36 = 1;
            }
            break;
        case 2:
            step = 0;
            t = 17;
            idx = -1;
            if (++w->x2E >= 17) {
                w->x36 = 1;
                if (sh->stg == game_w.stage) {
                    Eft17_set_pos_ang(&sh->pos0.x, 6, ((EMW *)sh->owner)->kind, sh->ang[1], 1.0f);
                }
            }
            break;
        case 3:
            step = 1;
            t = 13;
            idx = 8;
            if (++w->x2E > 13) {
                sh->x61 = 0;
                sh->mode++;
                return;
            }
            break;
        }
        break;
    case 5:
        sh->pos0.x = sh->pos2.x;
        sh->pos0.y = sh->pos2.y;
        sh->pos0.z = sh->pos2.z;
        break;
    case 7:
    case 10:
        step = 0;
        sh->pos0.x = sh->pos2.x;
        sh->pos0.y = sh->pos2.y;
        sh->pos0.z = sh->pos2.z;
        break;
    case 8:
        ow = sh->owner;
        if (ow->be_flag == 0 || ow->x04 == 3) {
            sh->xB = 0;
            sh->x61 = 0;
            sh->mode++;
            return;
        }
        if (ow->mode == 4 || ow->mode == 5) {
            sh->xB = 0;
            sh->x61 = 0;
            sh->mode++;
            return;
        }
        step = 0;
        t = 8;
        sh->pos0.x = sh->pos2.x;
        sh->pos0.y = sh->pos2.y;
        sh->pos0.z = sh->pos2.z;
        break;
    }
    sh->xC8 = sh->ang[1];
    if (++sh->char0 > all) {
        sh->x61 = 0;
        sh->mode++;
        return;
    }
    switch (sh->arg) {
    case 0:
    case 11:
        if (sh->char0 > 30) {
            d = 1.0f / (f32)(sh->char0 - 30);
            sh->rate[0] *= d;
            sh->rate[1] *= d;
            sh->rate[2] *= d;
        }
        shell_rate_add_g(sh);
        if (sh->stg == game_w.stage) {
            if (sh->char0 < all - 5) {
                if (sh->char0 == 1) {
                    Eft17_type3_set(&sh->pos2.x, sh->ang[0], sh->ang[1], 0, ((EMW *)sh->owner)->kind);
                } else if (sh->char0 % 3 == 1) {
                    Eft17_type3_set(&sh->pos2.x, sh->ang[0], sh->ang[1], 2, ((EMW *)sh->owner)->kind);
                } else {
                    Eft17_type3_set(&sh->pos2.x, sh->ang[0], sh->ang[1], 0, ((EMW *)sh->owner)->kind);
                }
            }
        }
        break;
    case 2:
    case 3:
        shell_rate_add_g(sh);
        if (sh->stg == game_w.stage) {
            if (sh->char0 >= 3 && (sh->char0 & 1)) {
                Eft17_set_pos_ang(&sh->pos2.x, 0x12, ((EMW *)sh->owner)->kind, sh->ang[1], 1.0f);
            }
        }
        break;
    case 4:
        if (sh->x05 == 1 || sh->x05 == 2) {
            if (sh->stg == game_w.stage) {
                if (!(sh->char0 & 3)) {
                    Eft17_set_pos_ang(&sh->pos0.x, 0, ((EMW *)sh->owner)->kind, sh->ang[1], 1.0f);
                }
            }
        }
        break;
    case 5:
        shell_rate_add_g(sh);
        break;
    case 6:
        if (sh->char0 >= all - 2) {
            if (sh->char0 == all - 2) {
                sh->xB = 0;
                flag = 1;
            } else {
                return;
            }
        }
        shell_rate_add_g(sh);
        break;
    case 7:
    case 9:
    case 10:
        shell_rate_add_g(sh);
        break;
    }
    for (i = 0; i < num; i++) {
        switch (sh->arg) {
        case 0:
            if (i < 3) {
                t = 36;
                if (p->no != 0) {
                    idx = 1;
                }
            } else {
                t = 5;
            }
            break;
        case 6:
            t = shell08_type6_time[p->no];
            break;
        case 8:
            if (p->x02 == 0) {
                shell08_type8_init(sh, w, p);
            }
            break;
        case 11:
            if (i < 3) {
                t = 36;
                if (p->no != 0) {
                    idx = 1;
                }
            } else {
                t = 5;
            }
            break;
        }
        if (++p->x02 <= 0) {
            idx += step;
            p++;
            continue;
        }
        if (p->x02 > t) {
            switch (sh->arg) {
            case 2:
            case 3:
                p->x02 = 1;
                break;
            case 6:
                shell08_type6_init(sh, p);
                if (p->x02 <= 0) {
                    idx += step;
                    p++;
                    continue;
                }
                break;
            case 8:
                p->x02 = 0;
                idx += step;
                p++;
                continue;
            default:
                idx += step;
                p++;
                continue;
            }
        }
        switch (sh->arg) {
        case 0:
        case 11:
            dat = shell08_data[idx++];
            eft_vec_linear(p->x02, dat, p->scl);
            if (p->no == 0) {
                if (!(sh->char0 & 1)) {
                    sh->x06++;
                    sh->x06 &= 7;
                }
            } else {
                p->x10 += p->x16;
                p->x12 += p->x18;
                if (p->no == 1) {
                    p->scl[0] *= 0.9f;
                    p->scl[1] *= 0.9f;
                    p->scl[2] *= 0.9f;
                }
            }
            if (sh->arg == 11) {
                p->scl[0] *= 1.5f;
                p->scl[1] *= 1.5f;
                p->scl[2] *= 1.5f;
            }
            break;
        case 1:
            switch (sh->x05) {
            case 0:
                dat = shell08_data[idx++];
            eft_vec_linear(p->x02, dat, p->scl);
                break;
            case 1:
                break;
            case 2:
                p->scl[0] = 1.0f - (f32)p->x02 / (f32)t;
                break;
            }
            p->x14 += p->x1A;
            break;
        case 2:
        case 3:
        case 9:
            p->x14 += p->x1A;
            eft_vec_linear(p->x02, shell08_data[idx], p->scl);
            break;
        case 4:
            switch (sh->x05) {
            case 0:
                switch (p->no) {
                case 0:
                    p->x1F = 1;
                    dat = shell08_data[idx++];
            eft_vec_linear(p->x02, dat, p->scl);
                    break;
                default:
                    p->x1F = 0;
                    p++;
                    continue;
                }
                break;
            case 1:
                p->x1F = 1;
                p->x01 = 0xFF;
                switch (p->no) {
                case 0:
                case 1:
                    dat = shell08_data[idx++];
            eft_vec_linear(p->x02, dat, p->scl);
                    break;
                case 2:
                    p->scl[0] = 1.0f;
                    p->scl[1] = 1.0f;
                    p->scl[2] = 1.0f;
                    break;
                }
                break;
            case 2:
                p->x01 = 0xFF;
                switch (p->no) {
                case 0:
                    p->x1F = 0;
                    break;
                case 1:
                    p->x1F = 1;
                    p->scl[0] = 0.8f + 0.4f / 1000.0f * (ran_suu(1) & 0x3FF);
                    p->scl[1] = 1.0f;
                    p->scl[2] = 1.0f;
                case 2:
                    p->x1F = 1;
                    p->scl[0] = 1.0f;
                    p->scl[1] = 1.0f;
                    p->scl[2] = 1.0f;
                    break;
                }
                break;
            case 3:
                p->x01 = 0xFF;
                switch (p->no) {
                case 0:
                    p->x1F = 0;
                    break;
                case 1:
                    p->x1F = 1;
                    dat = shell08_data[idx++];
            eft_vec_linear(p->x02, dat, p->scl);
                    break;
                case 2:
                    p->x1F = 1;
                    dat = shell08_data[idx++];
            eft_vec_linear(p->x02, dat, p->scl);
                    break;
                }
                break;
            }
            p->x14 += p->x1A;
            break;
        case 6:
            dat = shell08_data[idx++];
            eft_vec_linear(p->x02, dat, p->scl);
            p->scl[0] *= 1.5f;
            p->scl[1] *= 1.5f;
            p->scl[2] *= 1.5f;
            break;
        case 7:
            switch (p->no) {
            case 0:
                p->scl[0] = 0.8f + 0.2f / 1000.0f * (ran_suu(1) & 0x3FF);
                break;
            case 1:
            default:
                p->scl[0] = p->scl[1] = 0.8f + 0.2f / 1000.0f * (ran_suu(1) & 0x3FF);
                break;
            }
            break;
        case 8:
            eft_vec_linear(p->x02, shell08_data[idx], p->scl);
            break;
        }
        p++;
    }
    switch (sh->arg) {
    case 0:
    case 11:
        if (sh->char0 <= 33) {
            y = GetGroundShellHit(&sh->pos2);
            if (sh->pos2.y <= y) {
                if (y - sh->pos2.y > 100.0f) {
                    shell08_impact_set(sh, 2, &sh->pos2.x);
                } else {
                    v2[0] = sh->pos2.x;
                    v2[1] = y;
                    v2[2] = sh->pos2.z;
                    shell08_impact_set(sh, 0, v2);
                }
                sh->x61 = 0;
                sh->xB = 0;
                sh->mode++;
            }
            if (sh->stg == game_w.stage) {
                flmatGetTrans(v, &rview_mat);
                v[0] -= sh->pos2.x;
                v[1] -= sh->pos2.y;
                v[2] -= sh->pos2.z;
                if (0.0f < flvecInnerProduct(v, D_3F2080)) {
                    d = flvecInnerProduct(v, v);
                    if (10000.0f > d) {
                        set_quake_sub2(2);
                    }
                    if (90000.0f > d) {
                        set_quake_sub2(1);
                    } else if (360000.0f > d) {
                        set_quake_sub2(0);
                    }
                }
            }
        }
        break;
    case 1:
        v[0] = 0.0f;
        v[1] = 0.0f;
        v[2] = 20.0f * top->scl[2];
        flvecRotX(v, DEG2RAD(ANG2DEG(sh->ang[0])));
        flvecRotY(v, DEG2RAD(ANG2DEG(sh->ang[1])));
        sh->pos2.x = sh->pos0.x + v[0];
        sh->pos2.y = sh->pos0.y + v[1];
        sh->pos2.z = sh->pos0.z + v[2];
        switch (w->x00) {
        case 0:
            y = GetGroundShellHit(&sh->pos2);
            if (sh->pos2.y <= y) {
                w->x00++;
                v2[0] = sh->pos2.x;
                v2[1] = y;
                v2[2] = sh->pos2.z;
                w->x30 = y;
                if (sh->x07 == 1) {
                    shell08_type1_impact_pos(sh, w, v2);
                    Shell08_set_shl(em, 10, 0, sh, v2);
                }
            }
            break;
        case 1:
            if (sh->x07 != 1 && !(sh->char0 & 1)) {
                shell08_type1_impact_pos(sh, w, v2);
                shell08_impact_set(sh, 0, v2);
            }
            if (sh->x05 > 2) {
                w->x00++;
            }
            break;
        case 2:
            break;
        }
        break;
    case 2:
    case 3:
    case 9:
        y = GetGroundShellHit(&sh->pos2);
        if (sh->pos2.y <= y) {
            if (y - sh->pos2.y > 100.0f) {
                shell08_impact_set(sh, 2, &sh->pos2.x);
            } else {
                v2[0] = sh->pos2.x;
                v2[1] = y;
                v2[2] = sh->pos2.z;
                shell08_impact_set(sh, 0, v2);
            }
            sh->x61 = 0;
            sh->xB = 0;
            sh->mode++;
        }
        break;
    case 4:
        v[0] = 0.0f;
        v[1] = 0.0f;
        v[2] = 6400.0f;
        flvecRotX(v, DEG2RAD(ANG2DEG(sh->ang[0])));
        flvecRotY(v, DEG2RAD(ANG2DEG(sh->ang[1])));
        sh->pos2.x = sh->pos0.x + v[0];
        sh->pos2.y = sh->pos0.y + v[1];
        sh->pos2.z = sh->pos0.z + v[2];
        break;
    case 5:
        y = GetGroundShellHit(&sh->pos2);
        if (sh->pos2.y <= y) {
            if (y - sh->pos2.y > 200.0f) {
                shell08_impact_set(sh, 2, &sh->pos2.x);
            } else {
                v2[0] = sh->pos2.x;
                v2[1] = y;
                v2[2] = sh->pos2.z;
                shell08_impact_set(sh, 0, v2);
            }
            sh->x61 = 0;
            sh->xB = 0;
            sh->mode++;
        }
        break;
    case 6:
        y = GetGroundShellHit(&sh->pos2);
        if (flAbs(y - sh->pos2.y) > 100.0f) {
            sh->x61 = 0;
            sh->xB = 0;
            sh->mode++;
        } else {
            sh->pos2.y = y;
        }
        PointToPoint(d3, &sh->pos2.x, &sh->pos0.x);
        sh->ang[0] = (u16)(s32)(0.5f + 65536.0f * flArcTan2(d3[1], flSqrt(d3[0] * d3[0] + d3[2] * d3[2])) / 6.2831855f);
        if (flag != 0) {
            if (sh->stg == game_w.stage) {
                Eft04_set_pos(&sh->pos2.x, 5, ((EMW *)sh->owner)->kind, 1.0f);
            }
        }
        break;
    case 7:
        y = GetGroundShellHit(&sh->pos2);
        if (sh->pos2.y <= y) {
            if (y - sh->pos2.y > 100.0f) {
                shell08_impact_set(sh, 2, &sh->pos2.x);
            } else {
                v2[0] = sh->pos2.x;
                v2[1] = y;
                v2[2] = sh->pos2.z;
                shell08_impact_set(sh, 0, v2);
            }
            sh->x61 = 0;
            sh->xB = 0;
            sh->mode++;
        }
        break;
    case 10:
        y = GetGroundHit(&sh->pos2);
        if (flAbs(y - sh->pos2.y) > 100.0f) {
            sh->x61 = 0;
            sh->xB = 0;
            sh->mode++;
        } else {
            sh->pos2.y = y;
        }
        if (sh->stg == game_w.stage) {
            if (!(sh->char0 & 1)) {
                if (!(sh->char0 & 2)) {
                    Em_se_req2(sh->owner, 0x25, 0, &sh->pos2.x, 1, 0);
                }
                Eft17_set_pos(&sh->pos2.x, 0xD, ((EMW *)sh->owner)->kind, 1.0f);
            }
        }
        break;
    }
    if (sh->prim != 0) {
        switch (sh->arg) {
        case 1:
        case 4:
            sh->prim->pos[0] = sh->pos0.x;
            sh->prim->pos[1] = sh->pos0.y;
            sh->prim->pos[2] = sh->pos0.z;
            add_prim(ot0, sh->prim, 0x40, 1);
            break;
        case 8:
            sh->prim->pos[0] = w->x14.x;
            sh->prim->pos[1] = w->x14.y;
            sh->prim->pos[2] = w->x14.z;
            add_prim(ot0, sh->prim, 0x40, 1);
            break;
        case 10:
            return;
        default:
            sh->prim->pos[0] = sh->pos2.x;
            sh->prim->pos[1] = sh->pos2.y;
            sh->prim->pos[2] = sh->pos2.z;
            add_prim(ot0, sh->prim, 0x40, 0);
            break;
        }
    }
    if (w->prim != 0) {
        switch (sh->arg) {
        case 0:
        case 11:
            w->prim->pos[0] = sh->pos2.x;
            w->prim->pos[1] = y + 10.0f;
            w->prim->pos[2] = sh->pos2.z;
            break;
        case 2:
        case 3:
        case 9:
            v[0] = 0.0f;
            v[1] = 0.0f;
            v[2] = -20.0f;
            flvecApplyMat33_2(v, &rview_mat);
            w->prim->pos[0] = sh->pos2.x + v[0];
            w->prim->pos[1] = sh->pos2.y + v[1];
            w->prim->pos[2] = sh->pos2.z + v[2];
            break;
        case 6:
            w->prim->pos[0] = sh->pos2.x;
            w->prim->pos[1] = sh->pos2.y;
            w->prim->pos[2] = sh->pos2.z;
            break;
        }
        add_prim(ot0, w->prim, 0x40, 0);
    }
}

void shell08_rgba(RGBA_KEY *k, int t, u32 *out) {
    f32 rate;
    u32 c1;
    u8 g0, r0, b0, a0;
    int kt;

    while (1) {
        kt = k->time;
        if (kt == -1) return;
        if (t == kt) {
            *out = k->rgba;
            return;
        }
        if (kt < t && t < k[1].time) {
            rate = (f32)(t - kt) / (f32)(k[1].time - kt);
            b0 = k->rgba >> 16;
            a0 = k->rgba >> 24;
            g0 = k->rgba >> 8;
            r0 = k->rgba;
            c1 = k[1].rgba;
            *out = ((u8)(a0 + (u8)(rate * ((f32)(u8)(c1 >> 24) - (f32)a0))) << 24)
                 | ((u8)(b0 + (u8)(rate * ((f32)(u8)(c1 >> 16) - (f32)b0))) << 16)
                 | ((u8)(g0 + (u8)(rate * ((f32)(u8)(c1 >> 8) - (f32)g0))) << 8)
                 | (u8)(r0 + (u8)(rate * ((f32)(u8)c1 - (f32)r0)));
            return;
        }
        k++;
    }
}

/* ---- shell08_trans (0x006309A0, 6320 bytes): draw callback of the shell08
 * primitive (pr->owner = the shell, pr->no = 0/1: which of its two prims).
 * sh->arg picks the shot kind; each kind draws particles (w->p[], 0x20 bytes)
 * as clay pieces of the area model (am, game_w.area_mdlw[Em_area_ck]) or of
 * the shared effect model (eft_mdlw[0]) through shell08_trans_sub. Written
 * from the asm (m2c cannot do this one); NOT built: register allocation and
 * block order differ. Float arguments m2c drops were re-read from the asm.
 * Guesses: the meaning of each kind (0/11 impact rings + glow, 1 flash cone,
 * 2/3/9 puffs, 4 smoke, 5 single billboard, 6 sparks/shards, 7 two
 * streaks, 8 thunder). Clay index = byte offset / 0x8C. ---- */
typedef struct EFT_MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x0F];
    void *mat;          /* 0x10 material table */
    u8 _pad14[0x1C];
    CLAY *clay;         /* 0x30 */
} EFT_MDLW;

extern EFT_MDLW *eft_mdlw[5];
extern s16 shell08_all_time[];
extern RGBA_KEY *shell08_type0_col_data[];
extern RGBA_KEY *shell08_type11_col_data[];
extern RGBA_KEY shell08_type2_fade[];
extern RGBA_KEY shell08_type3_fade[];
extern RGBA_KEY shell08_type9_fade[];
extern RGBA_KEY *shell08_type6_fade_data[];
extern s16 shell08_type6_mdl_no[];
extern RGBA_KEY fade_type4_em00_0[];
extern RGBA_KEY fade_type4_em00_2[];
extern RGBA_KEY fade_type8_em05_0[];
extern FLMAT rview_mat;
s16 Em_area_ck(int);
void SetTrnslMode(int, int);
void clay_attr_reset(void);
void flmatRotX33(FLMAT *, f32);
void flmatRotZ33(FLMAT *, f32);
void flmatRotZXY33(FLMAT *, f32, f32, f32);
void shell08_trans_sub(CLAY *cl, FLMAT *mat, u32 tex, int ope, void *mats);

#define A2R(a) (2.0f * (3.1415927f * (360.0f * (f32)(a) / 65536.0f / 360.0f)))

/* scale the three rows of the rotation part by a, b, c (type 1/6 shots) */
#define SCALE_ROWS(m, a, b, c) \
    do { \
        m[0][0] *= (a); m[0][1] *= (a); m[0][2] *= (a); \
        m[1][0] *= (b); m[1][1] *= (b); m[1][2] *= (b); \
        m[2][0] *= (c); m[2][1] *= (c); m[2][2] *= (c); \
    } while (0)

void shell08_trans(PRIM *pr) {
    SHLW *sh = (SHLW *)pr->owner;
    SH08W *w = SH08_W(sh);
    SH08P *p = w->p;
    EFT_MDLW *em = eft_mdlw[0];
    EFT_MDLW *am;
    void *mats;
    FLMAT m;            /* sp+0x160 */
    FLMAT uv;           /* sp+0x120 */
    u32 col;            /* sp+0x1AC */
    u8 alpha, cr, cg, cb;   /* sp+0x110, 0xE0, 0xF0, 0x100 */
    s16 cnt1, cnt0, thr;    /* sp+0xD0, 0xB0, 0xC0 */
    RGBA_KEY **kp;
    RGBA_KEY *key;
    s16 ci;             /* clay index */
    s16 i, n;
    u16 ope;
    f32 fa, fb;
    s16 area;

    ope = 0;
    ci = 5;
    am = 0;
    if (sh->stg != game_w.stage) return;
    if (sh->arg != 9) {
        area = Em_area_ck(sh->x7E);
        if (area == -1) return;
        am = game_w.area_mdlw[area];
        if (am == 0 || am->flag == 0) return;
    }
    if (em == 0 || em->flag == 0) return;

    flSetRenderState(0x60, 0);
    mats = em->mat;
    switch (sh->arg) {
    case 0:
    case 11:
        cnt1 = 1;
        thr = 0x1E;
        cnt0 = 3;
        cb = 0xFF;
        if (sh->arg == 0) {
            kp = shell08_type0_col_data;
            cg = 0xCB;
            cr = 0x79;
        } else {
            kp = shell08_type11_col_data;
            cg = 0xF3;
            cr = 0xDF;
        }
        if (sh->char0 < thr) {
            alpha = 0xFF;
            fa = 0;
        } else {
            fa = (1.0f / 6.0f) * (f32)(shell08_all_time[sh->arg] - sh->char0);
            alpha = (u8)(s32)(255.0f * fa);
        }
        if (pr->no == 0) {
            flSetRenderState(0x6C, 0);
            fa = 3.5f - 2.5f * fa;
            for (i = 0; i < cnt0; i++, p++, kp++) {
                f32 rx = A2R(p->x10);
                f32 ry = A2R(p->x12);
                f32 rz = A2R(p->x14);
                f32 sx, sy, sz;
                if (sh->char0 < thr) {
                    sx = p->scl[0];
                    sy = p->scl[1];
                    sz = p->scl[2];
                } else {
                    sx = p->scl[0] * fa;
                    sy = p->scl[1] * fa;
                    sz = p->scl[2] * fa;
                }
                key = *kp;
                flmatMakeScale(&m, sx, sy, sz);
                flmatRotXYZ33(&m, rx, ry, rz);
                flmatSetTrans(&m, pr->pos[0], pr->pos[1], pr->pos[2]);
                if (p->no == 0) {
                    flmatMakeTrans(&uv, 0.125f * (f32)sh->x06, 0.0f, 0.0f);
                    flSetRenderState(0x19, (u32)&uv);
                    flmatMul33_2(&m, &rview_mat);
                    ope |= 2;
                    ci = 0;
                    shell08_rgba(key, sh->x06, &col);
                } else {
                    ci = 1;
                    if (p->no == 2) ope |= 2;
                    shell08_rgba(key, sh->char0 % 9, &col);
                }
                shell08_trans_sub(&am->clay[ci], &m, col, ope, am->mat);
                ope = 0;
            }
        } else if (pr->no == 1) {
            p += cnt0;
            flSetRenderState(0x6C, 0);
            for (i = 0; i < cnt1; i++, p++) {
                flmatMakeScale(&m, 4.0f * p->scl[0], 4.0f * p->scl[1], 4.0f * p->scl[2]);
                flmatRotX33(&m, A2R(p->x10));
                flmatSetTrans(&m, pr->pos[0], pr->pos[1], pr->pos[2]);
                col = ((u32)(u8)(s32)(0.3f * (f32)alpha) << 24) | (cb << 16) | (cg << 8) | cr;
                shell08_trans_sub((CLAY *)((u8 *)em->clay + 0x2044), &m, col, 0, mats);
            }
            flSetRenderState(0x6C, 1);
        }
        break;
    case 1:
        flmatInit(&m);
        flmatRotXYZ33(&m, A2R(sh->ang[0]), A2R(sh->ang[1]), 0.0f);
        shell08_z_adj(&m, &sh->pos0.x);
        SCALE_ROWS(m, p->scl[0], p->scl[1], p->scl[2]);
        mats = am->mat;
        flmatSetTrans(&m, pr->pos[0], pr->pos[1], pr->pos[2]);
        flSetRenderState(0x6C, 0);
        flmatMakeTrans(&uv, 0.0f, 0.1f * (f32)(sh->char0 % 10), 0.0f);
        flSetRenderState(0x19, (u32)&uv);
        shell08_trans_sub((CLAY *)((u8 *)am->clay + 0x2BC), &m, -1, 2, mats);
        break;
    case 2:
    case 3:
    case 9: {
        f32 sx, sy, sz;
        CLAY *cl;
        if (pr->no == 1) p++;
        if (p->x02 > 0 && p->x02 < 0xB) {
            sz = p->scl[2];
            sy = p->scl[1];
            sx = p->scl[0];
            if (sh->arg == 9) {
                sx *= 0.5f;
                cl = (CLAY *)((u8 *)em->clay + 0x3FFC);
                sy *= 0.5f;
                sz *= 0.5f;
            } else {
                mats = am->mat;
                cl = (CLAY *)((u8 *)am->clay + 0x118);
            }
            if (sh->arg == 2) {
                key = shell08_type2_fade;
            } else if (sh->arg == 3) {
                key = shell08_type3_fade;
            } else {
                key = shell08_type9_fade;
            }
            shell08_rgba(key, p->x02, &col);
            flmatMakeScale(&m, sx, sy, sz);
            flmatRotZ33(&m, A2R(p->x14));
            flmatSetTrans(&m, pr->pos[0], pr->pos[1], pr->pos[2]);
            flmatMul33_2(&m, &rview_mat);
            shell08_trans_sub(cl, &m, col, 0, mats);
        }
        break;
    }
    case 4:
        mats = am->mat;
        flSetRenderState(0x6C, 0);
        for (i = 0; i < 3; i++) {
            if (p->x1F == 0) {
                p++;
                continue;
            }
            switch (sh->x05) {
            case 0:
                if (p->no != 0) {
                    p++;
                    continue;
                }
                ci = 0;
                flmatMakeTrans(&uv, 0.0f, 1.0f - 0.125f * (f32)(sh->char0 & 7), 0.0f);
                flSetRenderState(0x19, (u32)&uv);
                shell08_rgba(fade_type4_em00_0, p->x02, &col);
                break;
            case 1:
                switch (p->no) {
                case 0:
                    ci = 7;
                    break;
                case 1:
                    ci = 0;
                    flmatMakeTrans(&uv, 0.0f, 1.0f - 0.125f * (f32)(sh->char0 & 7), 0.0f);
                    flSetRenderState(0x19, (u32)&uv);
                    break;
                case 2:
                    ci = 1;
                    flmatMakeTrans(&uv, 0.0f, 0.1f * (f32)(sh->char0 % 10), 0.0f);
                    flSetRenderState(0x19, (u32)&uv);
                    break;
                default:
                    p++;
                    continue;
                }
                col = ((u32)p->x01 << 24) | 0xFFFFFF;
                break;
            case 2:
            case 3:
                switch (p->no) {
                case 1:
                    ci = 0;
                    flmatMakeTrans(&uv, 0.0f, 1.0f - 0.125f * (f32)(sh->char0 & 7), 0.0f);
                    flSetRenderState(0x19, (u32)&uv);
                    break;
                case 2:
                    ci = 1;
                    flmatMakeTrans(&uv, 0.0f, 0.1f * (f32)(sh->char0 % 5), 0.0f);
                    flSetRenderState(0x19, (u32)&uv);
                    break;
                default:
                    p++;
                    continue;
                }
                if (sh->x05 == 3 && p->no == 1) {
                    shell08_rgba(fade_type4_em00_2, p->x02, &col);
                } else {
                    col = ((u32)p->x01 << 24) | 0xFFFFFF;
                }
                break;
            default:
                p++;
                continue;
            }
            if (sh->x05 == 1 && p->no == 0) {
                flmatMakeScale(&m, p->scl[0], p->scl[1], p->scl[2]);
                flmatSetTrans(&m, pr->pos[0], pr->pos[1], pr->pos[2]);
                flmatMul33_2(&m, &rview_mat);
            } else {
                flmatMakeScale(&m, p->scl[0], p->scl[1], 4.0f);
                flmatRotZXY33(&m, A2R(sh->ang[0]), A2R(sh->ang[1]), A2R(p->x14));
                flmatSetTrans(&m, pr->pos[0], pr->pos[1], pr->pos[2]);
            }
            shell08_trans_sub(&am->clay[ci], &m, col, 2, am->mat);
            p++;
        }
        break;
    case 5:
        mats = am->mat;
        flmatInit(&m);
        flmatSetTrans(&m, pr->pos[0], pr->pos[1], pr->pos[2]);
        flmatMul33_2(&m, &rview_mat);
        shell08_trans_sub(&am->clay[9], &m, -1, 0, mats);
        break;
    case 6:
        n = 1;
        if (pr->no == 1) {
            n = 5;
            p++;
        }
        flSetRenderState(0x6C, 0);
        for (i = 0; i < n; i++, p++) {
            if (p->x02 <= 0 || shell08_type6_time[p->no] < p->x02) continue;
            shell08_rgba(shell08_type6_fade_data[p->no], p->x02, &col);
            ci = shell08_type6_mdl_no[p->no];
            if (pr->no == 0) {
                flmatInit(&m);
                flmatRotXYZ33(&m, A2R(sh->ang[0]), A2R(sh->ang[1]), 0.0f);
                shell08_z_adj(&m, &sh->pos2.x);
                SCALE_ROWS(m, p->scl[0], p->scl[1], p->scl[2]);
                flmatSetTrans(&m, pr->pos[0], pr->pos[1], pr->pos[2]);
                ope |= 2;
            } else if (pr->no == 1) {
                flmatMakeScale(&m, p->scl[0], p->scl[1], p->scl[2]);
                if (p->no != 3) {
                    flSetRenderState(0x60, 0);
                    flmatRotZ33(&m, A2R(p->x14));
                    flmatMul33_2(&m, &rview_mat);
                    ope |= 2;
                } else {
                    flSetRenderState(0x60, 0x80);
                    fa = A2R(sh->ang[1]);
                    flmatRotZ33(&m, A2R(p->x14));
                    flmatRotY33(&m, fa);
                    flmatMakeTrans(&uv, 0.0f, 0.0234375f * (f32)p->x02, 0.0f);
                    flSetRenderState(0x19, (u32)&uv);
                }
                flmatSetTrans(&m, pr->pos[0], pr->pos[1], pr->pos[2]);
            }
            shell08_trans_sub(&am->clay[ci], &m, col, ope, am->mat);
        }
        break;
    case 7:
        flSetRenderState(0x6C, 0);
        for (i = 0; i < 2; i++) {
            if (p->no == 1) continue;           /* p is not advanced */
            if (p->no == 0) {
                flmatInit(&m);
                flmatRotXYZ33(&m, A2R(sh->ang[0]), A2R(sh->ang[1]), 0.0f);
                shell08_z_adj(&m, &sh->pos2.x);
                m[0][0] *= p->scl[0]; m[0][1] *= p->scl[0]; m[0][2] *= p->scl[0];
                m[2][0] *= 2.0f; m[2][1] *= 2.0f; m[2][2] *= 2.0f;
                flmatSetTrans(&m, pr->pos[0], pr->pos[1], pr->pos[2]);
                ci = 5;
            }
            flmatSetTrans(&m, pr->pos[0], pr->pos[1], pr->pos[2]);
            col = 0xFFD8B68A;
            shell08_trans_sub(&am->clay[ci], &m, col, 0, am->mat);
            p++;
        }
        break;
    case 8:
        /* the loop in the original runs exactly once */
        if (p->x02 > 0 && p->x02 < 9) {
            CLAY *cl;
            flSetRenderState(0x6C, 0);
            mats = am->mat;
            switch (sh->x06 >> 3) {
            case 0: ci = 5; break;
            case 1: ci = 9; break;
            case 2: ci = 10; break;
            }
            ope |= 2;
            cl = &am->clay[ci];
            flmatMakeScale(&m, p->scl[0], p->scl[1], p->scl[2]);
            if (sh->x06 & 1) flmatRotY33(&m, 3.1415927f);
            flmatSetTrans(&m, pr->pos[0], pr->pos[1], pr->pos[2]);
            shell08_rgba(fade_type8_em05_0, p->x02, &col);
            flmatMakeTrans(&uv, 0.25f * (f32)((sh->x06 >> 1) % 3), 0.125f - (1.0f / 64.0f) * (f32)p->x02, 0.0f);
            flSetRenderState(0x19, (u32)&uv);
            flmatMul33_2(&m, &rview_mat);
            shell08_trans_sub(cl, &m, col, ope, mats);
        }
        break;
    }
    clay_attr_reset();
    SetTrnslMode(4, 5);
    flSetRenderState(0x6C, 1);
}
