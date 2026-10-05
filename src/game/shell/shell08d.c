/* shell08 - game.bin 0x00632E70-0x006331C0: shell08_type6_init to
 * shell08_type8_pos_set (type-8 lightning strikes). See shell08.c.
 * shell08_rgba (before it) is still assembly; near-match in shell08_nm.c. */
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


void shell08_type6_init(SHLW *sh, SH08P *p) {
    Em_se_req2(sh->owner, 0x37, 0, &sh->pos2.x, 7, 1);
    switch (p->no) {
    case 0:
        p->x02 = 1;
        break;
    case 1:
        p->x02 = 1;
        p->x14 = ran_suu(1);
        break;
    case 2:
        p->x02 = 1;
        break;
    case 3:
        p->x02 = 1;
        p->x14 = ran_suu(1);
        break;
    case 4:
        p->x02 = -1;
        p->x14 = ran_suu(1);
        break;
    }
}

void shell08_type8_init(SHLW *sh, SH08W *w, SH08P *p) {
    f32 v[3];
    f32 top[3];
    f32 (*tp)[2];

    if (w->x00 == 0) {
        p->x02 = -80;
        sh->xB = 0;
        return;
    }
    if (w->x01 == 0) {
        p->x02 = -7;
    } else {
        sh->x06 = ran_suu(1);
        sh->x06 = (sh->x06 & 1) | ((((sh->x06 >> 3) % 3) << 3) | (((sh->x06 >> 1) % 3) << 1));
        shell08_type8_pos_set(sh, w);
        if (game_w.stage == sh->stg) {
            Em_se_req2(sh->owner, 0x18, 0, &sh->pos2.x, 1, 0);
        }
        if (w->x00 == 1) {
            w->x00--;
            return;
        }
    }
    pl_atck_data_set_shl(sh, sh->owner, sh->x07 + shell08_atk_no[sh->arg], shell08_tbl);
    tp = shell08_type8_thunder_pos[sh->x07 % 6];
    v[0] = tp[w->x01][0];
    v[1] = 0.0f;
    v[2] = tp[w->x01][1];
    flvecRotY(v, DEG2RAD(ANG2DEG(sh->ang[1])));
    AddVector(&sh->pos2.x, &w->base.x, v);
    sh->pos2.y = GetGroundShellHit(&sh->pos2);
    top[0] = sh->pos2.x;
    top[1] = sh->pos2.y + 1000.0f;
    top[2] = sh->pos2.z;
    if (game_w.stage == sh->stg) {
        Eft04_set_pos(top, 6, ((EMW *)sh->owner)->kind, 1.0f);
        Eft04_set_pos(&sh->pos2.x, 7, ((EMW *)sh->owner)->kind, 1.0f);
    }
    w->x00--;
    w->x01++;
}

void shell08_type8_pos_set(SHLW *sh, SH08W *w) {
    flvecCopy(&w->x14, &sh->pos2);
}
