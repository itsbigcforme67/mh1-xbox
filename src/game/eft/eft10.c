/* eft10 - game.bin 0x005454D0-0x00545E8C, split in two around eft10_m
 * (see eft10_nm.c). In the original all eft10_* functions are static. Dust puffs kicked up at a
 * monster's foot: each puff grows, drifts and fades along keyframe tables
 * (eft10_data), tinted with the stage's dust colour (Eft_kemuri_rgb). */
#include "eft.h"
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

/* One puff (0x30 bytes) of the work area. */
typedef struct EFT10_PUFF {
    s16 prim_no;        /* 0x00 */
    s16 no;             /* 0x02 odd puffs drift the other way */
    f32 pos[3];         /* 0x04 */
    f32 scale[3];       /* 0x10 */
    f32 alpha;          /* 0x1C */
    f32 size;           /* 0x20 */
    PRIM *prim;         /* 0x24 */
    s16 time;           /* 0x28 */
    u16 rot;            /* 0x2A */
    s16 drot;           /* 0x2C */
} EFT10_PUFF;

typedef struct EFT10_POS {
    s16 joint;          /* 0x00 */
    u8 _pad02[2];
    f32 ofs[3];         /* 0x04 */
} EFT10_POS;

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

extern EFT_MDLW *eft_mdlw[5];
extern FLMAT rview_mat;
extern s16 eft10_num[1];
extern s16 eft10_all_time[1];
extern s16 eft10_param[1];
extern s16 eft10_time[1];
extern s16 eft10_index[1];
extern s16 *eft10_time_tbl[1];
extern void *eft10_data[];
extern EFT10_POS eft10_type0_pos[];
extern s16 Eft_stg_type[];
extern u8 Eft_kemuri_rgb[][4];

u32 ran_suu(int);
u8 Em_stg_ck(EMW *);
void release_prim(s16);
FLMAT *get_joint_wmat_em(EMW *, int);
u16 calc_mat_angY(FLMAT *);
void flmatCopy(FLMAT *, FLMAT *);
void flvecCopy(f32 *, f32 *);
void flvecRotY(f32 *, f32);
void eft_vec_linear(f32, void *, f32 *);
void eft_alpha_linear(f32, void *, f32 *);
void make_mat_srt(f32 *, f32 *, f32 *, int, FLMAT *);
void eft_trans_sub_col(CLAY *, FLMAT *, u32, int, void *);

void eft10_move(EFTW *ew);
static void eft10_i(EFTW *ew);
void eft10_m(EFTW *ew);
void eft10_d(EFTW *ew);
void eft10_e(EFTW *ew);
void eft10_t(PRIM *pr);

void eft10_move(EFTW *ew) {
    switch (ew->mode) {
    case 0:
        eft10_i(ew);
        break;
    case 1:
        eft10_m(ew);
        break;
    case 2:
        eft10_d(ew);
        break;
    case 3:
        eft10_e(ew);
        break;
    }
}

static void eft10_i(EFTW *ew) {
    FLMAT m;
    EFT10_PUFF *p = ew->work;
    EMW *em = ew->owner;
    s16 n;
    s16 i;

    ew->mode++;
    ew->mode2 = 0;
    ew->be_flag = 1;
    ew->work14 = 0;
    ew->timer = 0;
    n = eft10_num[ew->arg];
    switch (ew->arg) {
    case 0:
        flmatCopy(&m, get_joint_wmat_em(em, eft10_type0_pos[ew->x07].joint));
        ew->u0A.ang = calc_mat_angY(&m) + 0x4000;
        ew->pos[0] = m[3][0];
        ew->pos[1] = em->x5AC;
        ew->pos[2] = m[3][2];
        break;
    }
    for (i = 0; i < n; i++, p++) {
        p->prim_no = get_prim();
        p->no = i;
        p->time = 0;
        p->size = ew->scale;
        switch (ew->arg) {
        case 0:
            p->rot = ran_suu(1);
            p->drot = (ran_suu(1) & 0xFF) - 0x80;
            flvecCopy(p->pos, ew->pos);
            break;
        }
        if (p->prim_no != -1) {
            p->prim = get_prim_ptr(p->prim_no);
            p->prim->owner = ew;
            p->prim->no = i;
            p->prim->trans = eft10_t;
        } else {
            p->prim = 0;
        }
    }
}
