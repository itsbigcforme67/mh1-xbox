/* eft15 - game.bin 0x0054BB60-0x0054BBE0: eft15_move. eft15_i, eft15_m and
 * eft15_t are still assembly (near-matches in eft15_nm.c); the rest is in
 * eft15b.c and eft15c.c. See eft15_nm.c for what the effect does. */
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

/* One sprite (0x2C bytes) of the work area. */
typedef struct EFT15_PIECE {
    s16 no;             /* 0x00 */
    s16 prim_no;        /* 0x02 */
    f32 pos[3];         /* 0x04 */
    f32 scale[3];       /* 0x10 from the keyframes */
    f32 size;           /* 0x1C */
    PRIM *prim;         /* 0x20 */
    s16 lag;            /* 0x24 frame counter, starts negative */
    u16 rot;            /* 0x26 */
    u16 drot;           /* 0x28 */
    u8 alpha;           /* 0x2A */
} EFT15_PIECE;

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

extern EFT_MDLW *eft_mdlw[5];
extern FLMAT rview_mat;
extern void *eft15_data[];
extern s16 eft15_num[9];
extern s16 eft15_all_time[9];
extern s16 eft15_param[9];
extern s16 eft15_index[9];
extern s16 eft15_time[9];
extern s16 *eft15_time_tbl[9];
extern s16 eft15_type0_lag[3];
extern s16 eft15_type2_lag[3];
extern s16 eft15_type4_lag[7];
extern u8 fade_type1_72[];
extern u8 fade_type6_72[];
extern u8 fade_type2_94_1[];

u32 ran_suu(int);
u8 Pl_stg_ck(PLW *);
u8 Em_stg_ck(EMW *);
void release_prim(s16);
void flvecCopy(f32 *, f32 *);
void flvecRotY(f32 *, f32);
void flvecApplyMat33_2(f32 *, FLMAT *);
void get_joint_pos_em(EMW *, int, f32 *);
int em_frame_check2(EMW *, int, f32);
void eft_vec_linear(f32, void *, f32 *);
void eft_alpha_linear(f32, void *, f32 *);
void eft_rgba_linear(void *, s16, u32 *);
void make_mat_srt(f32 *, f32 *, f32 *, u16, FLMAT *);
void eft_trans_sub_col(CLAY *, FLMAT *, u32, u16, void *);
void Pl_se_req2(EMW *, int, int, f32 *, int, int);
void se_req2(int, int, int, f32 *, int, int);

void eft15_move(EFTW *ew);
void eft15_i(EFTW *ew);
void eft15_m(EFTW *ew);
void eft15_d(EFTW *ew);
void eft15_e(EFTW *ew);
void eft15_t(PRIM *pr);
int eft15_loop_init(EFTW *ew, EFT15_PIECE *p);
void eft15_se_req(EFTW *ew);

void eft15_move(EFTW *ew) {
    switch (ew->mode) {
    case 0:
        eft15_i(ew);
        break;
    case 1:
        eft15_m(ew);
        break;
    case 2:
        eft15_d(ew);
        break;
    case 3:
        eft15_e(ew);
        break;
    }
}
