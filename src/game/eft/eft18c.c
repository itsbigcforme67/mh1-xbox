/* eft18 - game.bin 0x00554750-0x00554978: Eft18_set to Eft18_set5.
 * See eft18.c. */
#include "eft.h"
#include "pl.h"
#include "shell.h"
#include "prim.h"
#include "clay.h"
#include "fl.h"

typedef struct EFT_MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x0F];
    void *mat;          /* 0x10 material table */
    u8 _pad14[0x1C];
    CLAY *clay;         /* 0x30 */
} EFT_MDLW;

/* One sprite (0x2C bytes), after the effect's matrix in the work area. */
typedef struct EFT18_PIECE {
    f32 scale[3];       /* 0x00 from the keyframes */
    f32 alpha;          /* 0x0C */
    f32 pos[3];         /* 0x10 */
    PRIM *prim;         /* 0x1C */
    s16 prim_no;        /* 0x20 */
    s16 no;             /* 0x22 */
    s16 lag;            /* 0x24 */
    s16 uv;             /* 0x26 */
    u16 rot;            /* 0x28 */
    s16 drot;           /* 0x2A */
} EFT18_PIECE;

typedef struct EFT18_WORK {
    FLMAT mat;              /* 0x00 */
    EFT18_PIECE piece[1];   /* 0x40 */
} EFT18_WORK;

/* One rock (0x28 bytes) of type 4. */
typedef struct EFT18_ROCK {
    u8 alive;           /* 0x00 */
    u8 x01;             /* 0x01 */
    s16 prim_no;        /* 0x02 */
    PRIM *prim;         /* 0x04 */
    f32 vel[3];         /* 0x08 */
    f32 pos[3];         /* 0x14 */
    u16 rx;             /* 0x20 */
    u16 ry;             /* 0x22 */
    u16 rz;             /* 0x24 */
    u8 _pad26[2];
} EFT18_ROCK;

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

extern EFT_MDLW *eft_mdlw[5];
extern FLMAT rview_mat;
extern f32 D_3F2090[3];
extern void *eft18_data[];
extern s16 eft18_num[12];
extern s16 eft18_all_time[12];
extern s16 eft18_index[12];
extern f32 eft18_type1_scale[];
extern f32 eft18_type1_rot[];
extern s16 eft18_type1_time_tbl[];
extern s16 eft18_type5_time_tbl[];
extern s16 eft18_type8_time[];
extern s16 eft18_type8_lag[];
extern void **eft18_type5_fade_data[];
extern void *fade74_data[];
extern void *fade_type7_data[];
extern void *fade_type8_data[];

u32 ran_suu(int);
u8 Pl_stg_ck(PLW *);
void release_prim(s16);
FLMAT *get_joint_wmat(PLW *, int);
void flvecCopy(f32 *, f32 *);
void flvecRotX(f32 *, f32);
void flvecRotY(f32 *, f32);
void flvecApplyMat33_2(f32 *, FLMAT *);
void flvecNormalize(f32 *);
void flvecOuterProduct(f32 *, f32 *, f32 *);
f32 flvecInnerProduct(f32 *, f32 *);
f32 flvecCalcDistance(f32 *, f32 *);
void flmatCopy(FLMAT *, FLMAT *);
void flmatGetTrans(f32 *, FLMAT *);
void flmatRotX33(FLMAT *, f32);
void RotateY(FLMAT *, f32);
void RotateZ(FLMAT *, f32);
void PointToPoint(f32 *, f32 *, f32 *);
f32 GetGroundHit(f32 *);
void eft_vec_linear(f32, void *, f32 *);
void eft_alpha_linear(f32, void *, f32 *);
void eft_rgba_linear(void *, s16, u32 *);
void make_mat_srt(f32 *, f32 *, f32 *, u16, FLMAT *);
void eft_trans_sub_col(CLAY *, FLMAT *, u32, u16, void *);
void eft_trans_sub_opa(CLAY *, FLMAT *, void *);
void SetTrnslMode(int, int);
void se_req2(int, int, int, f32 *, int, int);
void Eft13_set_pos(f32, f32 *, int);

void eft18_move(EFTW *ew);
void eft18_i(EFTW *ew);
void eft18_m(EFTW *ew);
void eft18_t(PRIM *pr);
void eft18_i00(EFTW *ew);
void eft18_m00(EFTW *ew);
void eft18_t00(PRIM *pr);
void eft18_i01(EFTW *ew);
void eft18_m01(EFTW *ew);
void eft18_t01(PRIM *pr);
void eft18_i02(EFTW *ew);
void eft18_m02(EFTW *ew);
void eft18_t02(PRIM *pr);
void eft18_d(EFTW *ew);
void eft18_e(EFTW *ew);
void eft18_z_adj(FLMAT *m, f32 *pos);
void eft18_se_req(EFTW *ew, f32 *pos);
EFTW *eft18_set_com(s16 arg);

void Eft18_set(PLW *pl, s16 joint, s16 arg) {
    EFTW *ew;

    if (Pl_stg_ck(pl) != 0) {
        ew = eft18_set_com(arg);
        if (ew != 0) {
            ew->u0A.joint = joint;
            ew->owner = (EMW *)pl;
        }
    }
}

void Eft18_set2(f32 *pos, s16 arg, int x07) {
    EFTW *ew = eft18_set_com(arg);

    if (ew != 0) {
        ew->x07 = x07;
        flvecCopy(ew->pos, pos);
    }
}

void Eft18_set3(SHLW *sh, s16 arg, int x07) {
    EFTW *ew;

    if ((ew = eft18_set_com(arg)) != 0) {
        ew->mode2 = sh->ang[0] >> 8;
        ew->stg = sh->ang[1] >> 8;
        ew->x07 = x07;
        flvecCopy(ew->pos, &sh->pos2.x);
        ew->scale = 0.5f * flvecCalcDistance(&sh->pos2.x, &sh->pos0.x);
    }
}

void Eft18_set4(PLW *pl, s16 joint, s16 arg, int x07) {
    EFTW *ew;

    if (Pl_stg_ck(pl) != 0) {
        ew = eft18_set_com(arg);
        if (ew != 0) {
            ew->stg = pl->ang[1] >> 8;
            ew->x07 = x07;
            ew->u0A.joint = joint;
            ew->owner = (EMW *)pl;
        }
    }
}

void Eft18_set5(f32 *pos, s16 arg, s16 ax, s16 ay) {
    EFTW *ew;

    if ((ew = eft18_set_com(arg)) != 0) {
        ew->timer = ax;
        ew->u0A.joint = ay;
        flvecCopy(ew->pos, pos);
        ew->owner = 0;
    }
}
