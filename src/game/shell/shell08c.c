/* shell08 - game.bin 0x00632250-0x00632AB8: shell08_impact_set to
 * shell08_z_adj (impact effects, type-1 beam positions). See shell08.c. */
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


void shell08_impact_set(SHLW *sh, s16 kind, f32 *pos) {
    f32 scale;
    f32 s;
    int n;

    switch (sh->arg) {
    case 0:
    case 11:
        if (sh->x07 == 0) {
            scale = 1.5f;
            n = 1;
        } else {
            n = 4;
            scale = 2.15f;
        }
        if (sh->arg == 11) {
            scale *= 1.5f;
        }
        if (sh->stg == game_w.stage) {
            Em_se_req2(sh->owner, 0x25, 0, pos, 1, 0);
            s = 0.15f * scale;
            eft14_set(pos, 1, s);
            Eft17_set_ex(pos, sh->ang[1], 4, s);
            Eft17_set_ex(pos, sh->ang[1], 9, scale);
        }
        switch (kind) {
        case 0:
            if (sh->stg == game_w.stage) {
                Eft17_set_ex(pos, sh->ang[1], 8, scale);
                Eft17_set_ex(pos, sh->ang[1], 0xB, scale);
                set_quake_sub(2, pos);
            }
            Shell09_set_em(sh->owner, pos, n);
            break;
        case 2:
            Shell09_set_em(sh->owner, pos, n);
            break;
        }
        break;
    case 1:
        if (sh->stg == game_w.stage) {
            if (sh->char0 & 2) {
                Em_se_req2(sh->owner, 0x25, 0, pos, 1, 0);
            }
            Eft17_set_pos(pos, 0xD, ((EMW *)sh->owner)->kind, 1.0f);
        }
        break;
    case 2:
        if (sh->stg == game_w.stage) {
            Em_se_req2(sh->owner, 0x25, 0, pos, 1, 0);
        }
        if (sh->stg == game_w.stage) {
            Eft17_set_ex(pos, sh->ang[1], 5, 1.0f);
            Eft17_set_pos_ang(pos, 0xF, ((EMW *)sh->owner)->kind, sh->ang[1], 1.4f);
        }
        Shell09_set_em(sh->owner, pos, 0xB);
        break;
    case 3:
        if (sh->stg == game_w.stage) {
            Em_se_req2(sh->owner, 0x25, 0, pos, 1, 0);
        }
        if (sh->stg == game_w.stage) {
            Eft17_set_ex(pos, sh->ang[1], 0x10, 1.0f);
            Eft17_set_pos_ang(pos, 0xF, ((EMW *)sh->owner)->kind, sh->ang[1], 1.2f);
        }
        Shell09_set_em(sh->owner, pos, 0xC);
        break;
    case 5:
        if (sh->stg == game_w.stage) {
            Em_se_req2(sh->owner, 0x1D, 0, pos, 1, 0);
            Eft04_set_pos(pos, 1, 0xF, 1.0f);
            Eft04_set_pos(pos, 2, 0xF, 1.0f);
        }
        break;
    case 7:
        switch (kind) {
        case 0:
            if (sh->stg == game_w.stage) {
                Eft17_set_pos(pos, 0x14, ((EMW *)sh->owner)->kind, 1.0f);
            }
            Shell09_set_em(sh->owner, pos, 0x11);
            break;
        case 1:
        case 2:
            if (sh->stg == game_w.stage) {
                Eft17_set_pos(pos, 0x13, ((EMW *)sh->owner)->kind, 1.0f);
            }
            break;
        }
        break;
    case 9:
        if (sh->stg == game_w.stage) {
            Eft18_set2(pos, 9, 1);
        }
        break;
    }
}

void shell08_type1_pos_set(SHLW *sh, SH08W *w) {
    FLMAT m;
    f32 v[3];

    flmatCopy(&m, get_joint_wmat(sh->owner, w->joint));
    v[0] = shell08_ofs_tbl[sh->arg][0];
    v[1] = shell08_ofs_tbl[sh->arg][1];
    v[2] = shell08_ofs_tbl[sh->arg][2];
    flvecApplyMat33_2(v, &m);
    sh->pos2.x = m[3][0] + v[0];
    sh->pos2.y = m[3][1] + v[1];
    sh->pos2.z = m[3][2] + v[2];
    flvecCopy(&sh->pos0, &sh->pos2);
    if (sh->x07 == 0) {
        sh->ang[1] = (w->x37 << 8) + (u16)(s32)(0.5f + 65536.0f * flArcTan2(m[2][0], m[2][2]) / 6.2831855f);
    } else {
        sh->ang[0] = (u16)(s32)(0.5f + 65536.0f * flArcTan2(-m[2][1], flSqrt(m[2][0] * m[2][0] + m[2][2] * m[2][2])) / 6.2831855f);
        sh->ang[0] += w->x01 << 8;
    }
}

void shell08_type1_impact_pos(SHLW *sh, SH08W *w, f32 *out) {
    f32 dz;
    f32 t;

    if (flAbs(sh->pos2.y - sh->pos0.y) < 0.0001f) {
        out[0] = sh->pos2.x;
        out[1] = sh->pos2.y;
        out[2] = sh->pos2.z;
    } else if (w->x30 > sh->pos0.y) {
        out[0] = sh->pos0.x;
        out[1] = sh->pos0.y;
        out[2] = sh->pos0.z;
    } else {
        t = (w->x30 - sh->pos0.y) / (sh->pos2.y - sh->pos0.y);
        dz = t * (sh->pos2.z - sh->pos0.z);
        out[0] = sh->pos0.x + t * (sh->pos2.x - sh->pos0.x);
        out[1] = w->x30;
        out[2] = sh->pos0.z + dz;
    }
}

void shell08_z_adj(FLMAT *m, f32 *pos) {
    f32 a[4];
    f32 z[4];
    f32 x[4];
    f32 y[4];

    PointToPoint(a, D_3F2090, pos);
    flvecCopy(z, (*m)[2]);
    flvecOuterProduct(x, a, z);
    flvecNormalize(x);
    flvecOuterProduct(y, z, x);
    if (flvecInnerProduct(y, y) > 0.0001f) {
        flvecOuterProduct(x, y, z);
        flvecCopy((*m)[0], x);
        flvecCopy((*m)[1], y);
    }
}
