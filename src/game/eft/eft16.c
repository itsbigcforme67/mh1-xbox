/* eft16 - game.bin 0x0054D660-0x0054DC2C: eft16_move and eft16_i. eft16_m is
 * still assembly (near-match in eft16_nm.c); the rest is in eft16b.c. See
 * eft16_nm.c for what the effect does. */
#include "eft.h"
#include "em.h"
#include "pl.h"
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

/* One sprite (0x34 bytes) of the work area. */
typedef struct EFT16_PIECE {
    s16 prim_no;        /* 0x00 */
    s16 lag;            /* 0x02 frame counter, starts at 0 or below */
    f32 pos[3];         /* 0x04 position (type 0: velocity) */
    f32 scale[3];       /* 0x10 from the keyframes */
    s16 no;             /* 0x1C sub-kind; 0xFF = off */
    u16 rot;            /* 0x1E */
    u16 alpha;          /* 0x20 */
    u8 _pad22[2];
    f32 size;           /* 0x24 */
    f32 grav;           /* 0x28 type 0: fall speed */
    PRIM *prim;         /* 0x2C */
    s16 uv;             /* 0x30 texture frame */
} EFT16_PIECE;

/* Neck table entry: offsets for Eft16_set_ex and directions for eft16_m. */
typedef struct EFT16_NECK {
    f32 *ofs;           /* 0x00 two vectors: type 3, type 4 */
    f32 *dir;           /* 0x04 two vectors: type 3, type 4 */
} EFT16_NECK;

/* Character fields not in the shared headers yet. */
#define CHR_ANG3EC(c) (*(u16 *)((u8 *)(c) + 0x3EC))
#define CHR_X4D8(c) (*(u8 **)((u8 *)(c) + 0x4D8))

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

extern EFT_MDLW *eft_mdlw[5];
extern FLMAT rview_mat;
extern void *eft16_data[];
extern s16 eft16_num[];
extern s16 eft16_all_time[];
extern s16 eft16_param[];
extern s16 eft16_index[];
extern s16 eft16_time[];
extern f32 eft16_scale_tbl[16];
extern EFT16_NECK *eft16_neck_tbl[];
extern s16 eft16_time_tbl1[][2];
extern s16 eft16_time_tbl2[][2];
extern s16 eft16_time_tbl3[2][2];
extern s16 uv78_00647180[];
extern u8 Eft_blood_rgb[][4];

u32 ran_suu(int);
u8 Pl_stg_ck(PLW *);
u8 Em_stg_ck(EMW *);
void release_prim(s16);
void flvecCopy(f32 *, f32 *);
void flvecRotY(f32 *, f32);
void flvecApplyMat33(f32 *, f32 *, FLMAT *);
void flvecApplyMat33_2(f32 *, FLMAT *);
void flmatCopy(FLMAT *, FLMAT *);
void flmatGetTrans(f32 *, FLMAT *);
void flmatInvert(FLMAT *, FLMAT *);
void flmatRotX33(FLMAT *, f32);
f32 flSqrt(f32);
f32 flArcTan2(f32, f32);
FLMAT *get_joint_wmat(void *, int);
void eft_vec_linear(f32, void *, f32 *);
void eft_alpha_linear(f32, void *, f32 *);
void make_mat_srt(f32 *, f32 *, f32 *, u16, FLMAT *);
void eft_trans_sub_col(CLAY *, FLMAT *, u32, u16, void *);
void SetTrnslMode(int, int);
void Eft02_set3(EMW *, u16, int, int, f32 *, f32);
void Eft02_set4(u16, u16, int, f32 *, f32);
void se_req2(int, int, int, f32 *, int, int);

void eft16_move(EFTW *ew);
void eft16_i(EFTW *ew);
void eft16_m(EFTW *ew);
void eft16_d(EFTW *ew);
void eft16_e(EFTW *ew);
void eft16_t(PRIM *pr);
s16 eft16_rot(s16 no);
s16 eft16_col_type_sel(u8 flag);
void eft16_se_req(EFTW *ew, f32 *pos);

void eft16_move(EFTW *ew) {
    switch (ew->mode) {
    case 0:
        eft16_i(ew);
        break;
    case 1:
        eft16_m(ew);
        break;
    case 2:
        eft16_d(ew);
        break;
    case 3:
        eft16_e(ew);
        break;
    }
}

void eft16_i(EFTW *ew) {
    s16 n;
    EFT16_PIECE *p = ew->work;
    s16 i;

    ew->mode++;
    ew->be_flag = 1;
    ew->work14 = 0;
    ew->timer = 0;
    n = eft16_num[ew->arg];
    switch (ew->arg) {
    case 2:
    case 5:
    case 6:
        eft16_se_req(ew, ew->pos);
        break;
    case 3:
    case 4:
        return;
    }
    for (i = 0; i < n; p++, i++) {
        p->prim_no = get_prim();
        if (p->prim_no != -1) {
            switch (ew->arg) {
            case 0:
                p->no = i;
                if (!(ew->mode2 & 0x20)) {
                    p->lag = -10 - ((u16)ran_suu(1) & 7);
                } else {
                    p->lag = 0;
                }
                p->size = ew->scale;
                p->grav = -0.8f;
                break;
            case 2:
                p->lag = -i;
                p->no = 0;
                p->alpha = 0xFF;
                p->size = ew->scale;
                p->rot = ran_suu(1);
                break;
            case 5:
                if (i < 3) {
                    p->no = 0;
                    p->lag = -i;
                    p->size = ew->scale;
                    p->rot = ew->u0A.joint + (i * 0x2AAA + (u16)ran_suu(1) / 3);
                } else {
                    p->no = 1;
                    p->lag = 0;
                    p->size = ew->scale;
                    p->rot = ran_suu(1);
                }
                p->alpha = 0xFF;
                break;
            case 6:
                p->lag = 0;
                p->no = i;
                if (i == 0) {
                    p->alpha = 0xFF;
                }
                p->size = ew->scale;
                break;
            case 8:
                p->no = i;
                p->lag = -4 * i - 10;
                p->size = ew->scale * (0.5f + 0.5f * (0.001f * (f32)((u16)ran_suu(1) & 0x3FF)));
                p->uv = 0;
                break;
            case 9:
                p->no = (u16)ran_suu(1) & 7;
                p->lag = -12 * i - 10;
                p->size = ew->scale * (0.5f + 0.5f * (0.001f * (f32)((u16)ran_suu(1) & 0x3FF)));
                break;
            case 12:
                p->no = (u16)ran_suu(1) & 7;
                p->lag = -10 * i;
                p->size = ew->scale * (0.5f + 0.5f * (0.001f * (f32)((u16)ran_suu(1) & 0x3FF)));
                break;
            case 13:
                p->no = (u16)ran_suu(1) & 7;
                p->lag = -ew->stg * i;
                p->size = ew->scale * (0.5f + 0.5f * (0.001f * (f32)((u16)ran_suu(1) & 0x3FF)));
                break;
            case 14:
                p->no = i;
                p->uv = (u16)ran_suu(1) & 3;
                p->rot = ran_suu(1);
                p->lag = -ew->stg;
                p->size = ew->scale;
                p->alpha = 0xFF;
                break;
            }
            p->pos[0] = ew->pos[0];
            p->pos[1] = ew->pos[1];
            p->pos[2] = ew->pos[2];
            p->prim = get_prim_ptr(p->prim_no);
            p->prim->owner = ew;
            p->prim->no = i;
            p->prim->trans = eft16_t;
        } else {
            p->prim = 0;
        }
    }
}
