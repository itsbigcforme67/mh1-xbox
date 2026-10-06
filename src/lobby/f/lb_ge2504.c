/* lb_ge2504 - eft25 0x0060F810-0x0060F8C0: eft25_d. Whole file in lb_e25.c. */
#include "lobby_f.h"

typedef struct E25P {          /* one particle */
    s16 prim;                  /* 0x00 prim handle, -1 = none */
    s16 idx;                   /* 0x02 index inside the effect */
    f32 col[3];                /* 0x04 */
    f32 pos[3];                /* 0x10 */
    f32 alpha[2];              /* 0x1C alpha fade state */
    u16 ang;                   /* 0x24 */
    s16 angspd;                /* 0x26 */
    f32 scale;                 /* 0x28 */
    u8 *pr;                    /* 0x2C prim pointer */
    s16 time;                  /* 0x30 */
    u16 rnd;                   /* 0x32 */
} E25P;

typedef struct E25 {           /* effect work */
    u8 x0;
    u8 on;                     /* 0x01 active */
    u8 id;                     /* 0x02 */
    u8 type;                   /* 0x03 effect type 0..8 */
    u8 state;                  /* 0x04 0 init, 1 move, 2 destroy, 3 end */
    s8 f5;                     /* 0x05 */
    s8 f6;                     /* 0x06 */
    u8 x7;                     /* 0x07 */
    s16 t;                     /* 0x08 frame counter */
    s16 rot;                   /* 0x0A */
    u8 padC[8];
    s32 x14;                   /* 0x14 */
    E25P *part;                /* 0x18 particle array */
    u8 pad1C[4];
    void (*fn)();              /* 0x20 handler */
    f32 pos[3];                /* 0x24 */
    f32 scale;                 /* 0x30 */
    u8 *joint;                 /* 0x34 owner (first byte = alive flag) */
    s32 x38;                   /* 0x38 */
    s16 x3C;                   /* 0x3C prim handle of the root */
} E25;

void *pull_eft_work();
void push_eft_work();
void flvecCopy();
void eft25_move();
void eft25_i();
void eft25_m();
void eft25_d();
void eft25_e();
void release_prim();
extern s16 eft25_num[9];
extern s16 eft25_type0_lag[7];
void eft25_t();
extern s16 eft25_index[9];
extern s16 eft25_param[9];
extern s16 eft25_all_time[9];
extern u16 eft25_time[9];
extern u16 *eft25_time_tbl[9];
extern f32 *eft25_data[];
extern f32 rview_mat[16];
extern char ot0[];
void eft_vec_linear();
void eft_rgba_linear();
void eft_trans_sub_col();
void flSetRenderState();
void flmatMakeTrans();
void flmatMul33_2();
void make_mat_srt();
extern u8 eft25_type2_col[];
extern s32 *eft25_type5_fade_data[];
extern char fade_type1_74[];
extern char fade_type3_64[];
extern u8 *D_3C8DC0;
void eft_alpha_linear();
void flvecRotY();
f32 flSin();
void flmatCopy();
void *get_joint_wmat();
void flvecApplyMat33_2();





/* place the effect root on the joint with a local offset */
#define E25_ROOT(e, joint_no, ox, oy, oz) \
    flmatCopy(m, get_joint_wmat((e)->joint, joint_no, 1)); \
    v[0] = (ox); \
    v[1] = (oy); \
    v[2] = (oz); \
    flvecApplyMat33_2(v, m); \
    (e)->pos[0] = m[12] + v[0]; \
    (e)->pos[1] = m[13] + v[1]; \
    (e)->pos[2] = m[14] + v[2]


#define E25_RAD(a) (2.0f * (3.1415927f * (((360.0f * (f32)(a)) / 65536.0f) / 360.0f)))



void eft25_d(E25 *e) {
    E25P *p;
    s16 n;
    s16 i;
    s16 *np;
    p = e->part;
    e->state += 1;
    np = &eft25_num[e->type];
    n = *np;
    if (e->x38 != 0) {
        release_prim(e->x3C, np, e->type * 2);
    }
    i = 0;
    if (0 < n) {
        do {
            if (p->pr != 0) {
                release_prim(p->prim);
            }
            i++;
            p += 1;
        } while (i < n);
    }
}
