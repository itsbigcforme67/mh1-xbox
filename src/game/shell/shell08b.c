/* shell08 - game.bin 0x006307C0-0x0063099C: shell08_h to shell08_trans_sub.
 * See shell08.c. */
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


void shell08_h(SHLW *sh) {
    f32 v[3];

    switch (sh->arg) {
    case 0:
    case 5:
    case 7:
    case 11:
        v[0] = sh->pos2.x;
        v[1] = sh->pos2.y;
        v[2] = sh->pos2.z;
        sh->x61 = 0;
        sh->mode = 2;
        shell08_impact_set(sh, 1, v);
        break;
    case 2:
    case 3:
    case 9:
        sh->x61 = 0;
        sh->mode = 2;
        break;
    default:
        shell08_m(sh);
        break;
    }
}

void shell08_d(SHLW *sh) {
    SH08W *w = SH08_W(sh);

    if (sh->prim != 0) {
        release_prim(sh->prim_no);
    }
    if (w->prim != 0) {
        release_prim(w->prim_no);
    }
    sh->xB = 0;
    sh->mode = 6;
    sh->be_flag = 0;
}

void shell08_e(SHLW *sh) {
    push_shell_work(sh);
}

void shell08_trans_sub(CLAY *cl, FLMAT *mat, u32 tex, int ope, void *mats) {
    if (cl != 0 && cl->handle != -1) {
        flSetRenderState(0x1A, (u32)mat);
        flSetRenderState(0x67, tex);
        Material_set_sub(mats, cl);
        clay_attr_set(cl->attr);
        Eft_rendope_set(ope);
        flExecuteClay(cl->handle, 0);
        clay_attr_reset();
    }
}
