/* eft04 - game.bin 0x00542680-0x00542D34: eft04_pos_calc to
 * Eft04_set_time. See eft04.c. */
#include "eft.h"
#include "em.h"
#include "game.h"
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

/* One piece (0x30 bytes) of the work area. */
typedef struct EFT04_PIECE {
    s16 prim_no;        /* 0x00 */
    s16 no;             /* 0x02 */
    f32 pos[3];         /* 0x04 */
    s16 lag;            /* 0x10 frame counter, starts at 0 or below */
    u16 rot[3];         /* 0x12 */
    f32 size;           /* 0x18 */
    PRIM *prim;         /* 0x1C */
    f32 scale[3];       /* 0x20 from the keyframes */
    u8 col;             /* 0x2C 0: opaque (ot1), else blended (ot0) */
    u8 joint;           /* 0x2D type 0: entry in eft04_em15_pos */
    u8 alpha;           /* 0x2E */
    u8 speed;           /* 0x2F type 2: rise per frame times time */
} EFT04_PIECE;

/* Joint offsets for type 0 (0x14 bytes each). */
typedef struct EFT04_JPOS {
    s16 joint;          /* 0x00 */
    f32 ofs[3];         /* 0x04 */
    f32 size;           /* 0x10 */
} EFT04_JPOS;

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

extern EFT_MDLW *eft_mdlw[5];
extern FLMAT rview_mat;
extern f32 D_3F2090[3];
extern void *eft04_data[];
extern s16 eft04_num[9];
extern s16 eft04_all_time[9];
extern s16 eft04_param[9];
extern s16 eft04_index[9];
extern s16 *eft04_time_tbl[9];
extern EFT04_JPOS eft04_em15_pos[];
extern u8 *eft04_type4_fade_data[];
extern u8 *eft04_type5_fade_data[2];
extern u8 fade_type3_em02_00644B70[];
extern u8 fade_type3_em03[];
extern u8 fade_type3_em05[];
extern u8 fade_type3_em07[];

u32 ran_suu(int);
u8 Em_stg_ck(EMW *);
s16 Em_area_ck(int);
void release_prim(s16);
void flvecCopy(f32 *, f32 *);
void flvecApplyMat33(f32 *, f32 *, FLMAT *);
void flvecApplyMat33_2(f32 *, FLMAT *);
void flvecOuterProduct(f32 *, f32 *, f32 *);
void flvecNormalize(f32 *);
f32 flvecInnerProduct(f32 *, f32 *);
void flmatInit(FLMAT *);
void flmatCopy(FLMAT *, FLMAT *);
void flmatRotXYZ33(FLMAT *, f32, f32, f32);
void PointToPoint(f32 *, f32 *, f32 *);
FLMAT *get_joint_wmat_em(EMW *, int);
int em_frame_check2(EMW *, int, f32);
void eft_vec_linear(f32, void *, f32 *);
void eft_alpha_linear(f32, void *, f32 *);
void eft_rgba_linear(void *, s16, u32 *);
void make_mat_srt(f32 *, f32 *, f32 *, u16, FLMAT *);
void eft_trans_sub_col(CLAY *, FLMAT *, u32, u16, void *);
void eft_trans_sub_opa(CLAY *, FLMAT *, void *);
void Eft13_set_pos(f32 *, int, f32);

void eft04_move(EFTW *ew);
void eft04_i(EFTW *ew);
void eft04_m(EFTW *ew);
void eft04_d(EFTW *ew);
void eft04_e(EFTW *ew);
void eft04_t(PRIM *pr);
void eft04_pos_calc(f32 *pos, EMW *em, f32 *ofs, int joint);
void eft04_type0_0_init(EFTW *ew, EFT04_PIECE *p);
void eft04_type3_init(EFTW *ew, EFT04_PIECE *p);
void eft04_type8_init(EFTW *ew, EFT04_PIECE *p);
void eft04_z_adj(FLMAT *m, f32 *pos);

void eft04_pos_calc(f32 *pos, EMW *em, f32 *ofs, int joint) {
    FLMAT m;
    f32 v[3];

    flmatCopy(&m, get_joint_wmat_em(em, joint));
    flvecApplyMat33(v, ofs, &m);
    pos[0] = m[3][0] + v[0];
    pos[1] = m[3][1] + v[1];
    pos[2] = m[3][2] + v[2];
}

void eft04_type0_0_init(EFTW *ew, EFT04_PIECE *p) {
    s16 sel[4];
    s16 used[4];
    s16 i;
    s16 j;
    s16 n;
    EFT04_JPOS *jp;

    p++;
    for (i = 0; i < 4; i++, p++) {
        sel[i] = (u16)ran_suu(1) % (10 - i);
        for (j = 0; j < i; j++) {
            used[j] = 0;
        }
        while (1) {
            n = 0;
            for (j = 0; j < i; j++) {
                if (used[j] == 0 && sel[j] <= sel[i]) {
                    used[j] = 1;
                    n++;
                }
            }
            if (n <= 0) {
                break;
            }
            sel[i] += n;
        }
        p->joint = sel[i];
        p->rot[2] = ran_suu(1);
        p->size = eft04_em15_pos[p->joint].size * (0.9f + 0.00020000001f * (f32)((u16)ran_suu(1) & 0x3FF));
        jp = &eft04_em15_pos[p->joint];
        eft04_pos_calc(p->pos, ew->owner, jp->ofs, jp->joint);
    }
}

void eft04_type3_init(EFTW *ew, EFT04_PIECE *p) {
    switch (p->no) {
    case 0:
        p->lag = 1;
        p->rot[2] = ran_suu(1);
        break;
    case 1:
        p->lag = 1;
        break;
    case 2:
        p->lag = 1;
        p->rot[2] = ran_suu(1);
        break;
    case 3:
        p->lag = -1;
        p->rot[2] = ran_suu(1);
        break;
    }
}

void eft04_type8_init(EFTW *ew, EFT04_PIECE *p) {
    p->lag = 1;
    switch (p->no) {
    case 0:
        p->rot[0] = ran_suu(1);
        p->rot[1] = ran_suu(1);
        p->rot[2] = ran_suu(1);
        break;
    default:
        p->rot[2] = ran_suu(1);
        p->size = 1.2f + 0.0006f * (f32)((u16)ran_suu(1) & 0x3FF);
        break;
    }
}

void eft04_z_adj(FLMAT *m, f32 *pos) {
    f32 a[3];
    f32 b[3];
    f32 c[3];
    f32 d[3];

    PointToPoint(a, D_3F2090, pos);
    flvecCopy(b, (*m)[2]);
    flvecOuterProduct(c, a, b);
    flvecNormalize(c);
    flvecOuterProduct(d, b, c);
    if (flvecInnerProduct(d, d) > 0.0001f) {
        flvecOuterProduct(c, d, b);
        flvecCopy((*m)[0], c);
        flvecCopy((*m)[1], d);
    }
}

void Eft04_set(EMW *em, int arg) {
    EFTW *ew;

    if (Em_stg_ck(em) != 0) {
        ew = pull_eft_work(1);
        if (ew != 0) {
            ew->type = 4;
            ew->move = eft04_move;
            ew->arg = arg;
            ew->owner = em;
            ew->x07 = em->kind;
            ew->prim = 0;
            ew->scale = 1.0f;
        }
    }
}

void Eft04_set_pos(f32 *pos, int arg, int kind, f32 scale) {
    EFTW *ew;

    if ((ew = pull_eft_work(1)) != 0) {
        ew->type = 4;
        ew->move = eft04_move;
        ew->arg = arg;
        ew->owner = 0;
        ew->x07 = kind;
        ew->prim = 0;
        ew->scale = scale;
        flvecCopy(ew->pos, pos);
    }
}

void Eft04_set_time(EMW *em, int arg, int time, f32 scale) {
    EFTW *ew;

    if (Em_stg_ck(em) != 0) {
        ew = pull_eft_work(1);
        if (ew != 0) {
            ew->type = 4;
            ew->move = eft04_move;
            ew->arg = arg;
            ew->owner = em;
            ew->x07 = em->kind;
            ew->prim = 0;
            ew->scale = scale;
            ew->u0A.joint = time;
        }
    }
}
