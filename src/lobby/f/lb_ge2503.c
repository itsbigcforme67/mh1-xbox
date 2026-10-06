/* lb_ge2503 - eft25 0x0060E450-0x0060EB90: eft25_i. Whole file in lb_e25.c. */
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



void eft25_i(E25 *e) {
    f32 v[3];
    f32 m[16];
    E25P *p;
    s16 i;
    s16 *lag;
    s16 n;
    long nn;
    u8 ty;
    p = e->part;
    e->state += 1;
    e->f5 = 0;
    e->f6 = 0;
    e->on = 1;
    e->x14 = 0;
    e->t = 0;
    ty = e->type;
    n = eft25_num[ty];
    switch (ty) {
    case 0:
        e->rot = (u16)ran_suu(1);
    case 1:
        flmatCopy(m, get_joint_wmat(e->joint, 0xA));
        v[0] = 0.0f;
        v[1] = 50.0f;
        v[2] = 25.0f;
        flvecApplyMat33_2(v, m);
        e->pos[0] = m[12] + v[0];
        e->pos[1] = m[13] + v[1];
        e->pos[2] = m[14] + v[2];
        break;
    case 2:
        if (e->joint != 0) {
            E25_ROOT(e, 0xB, -26.4f, 2.8f, 7.7f);
        }
        break;
    case 5:
        E25_ROOT(e, 0x14, 0.0f, 40.0f, 0.0f);
        e->f5 = (u16)ran_suu(1) & 3;
        e->f6 = (u16)ran_suu(1) & 3;
        break;
    case 6:
        E25_ROOT(e, 0x12, -8.0f, 0.0f, 70.0f);
        break;
    case 8:
        E25_ROOT(e, 0x14, 0.0f, 40.0f, 0.0f);
        break;
    }
    nn = n;
    i = 0;
    if (0 < nn) {
        lag = eft25_type0_lag;
        do {
            p->prim = get_prim();
            if (p->prim != -1) {
                p->time = 0;
                p->scale = e->scale;
                flvecCopy(p->pos, e->pos);
                p->idx = i;
                switch (e->type) {
                case 0:
                    p->time = *lag - 10;
                    switch (p->idx) {
                    case 0:
                        p->ang = 0;
                        p->angspd = 0;
                        break;
                    case 1:
                        p->ang = 0;
                        p->angspd = 0;
                        p->scale = p->scale * 3.0f;
                        break;
                    case 2:
                        p->ang = (u16)ran_suu(1);
                        p->angspd = ((u16)ran_suu(1) & 0x3FF) - 0x200;
                        break;
                    case 3:
                        p->ang = (u16)ran_suu(1);
                        p->angspd = ((u16)ran_suu(1) & 0xFF) - 0x80;
                        p->scale = p->scale * 3.0f;
                        break;
                    case 4:
                    case 5:
                    case 6:
                        p->ang = (u16)ran_suu(1);
                        p->angspd = ((u16)ran_suu(1) & 0x3FF) - 0x200;
                        p->scale = p->scale * (0.5f + 0.0003f * (f32)((u16)ran_suu(1) & 0x3FF));
                        break;
                    }
                    break;
                case 1:
                    p->ang = 0;
                    p->angspd = ((u16)ran_suu(1) & 0x7FF) - 0x400;
                    p->scale = p->scale * (0.8f + 0.00040000002f * (f32)((u16)ran_suu(1) & 0x3FF));
                    p->rnd = (u16)ran_suu(1);
                    break;
                case 2:
                    p->ang = (u16)ran_suu(1);
                    p->angspd = ((u16)ran_suu(1) & 0x1FF) - 0x100;
                    p->rnd = (u16)ran_suu(1);
                    break;
                case 3:
                    switch (p->idx) {
                    case 0:
                        p->ang = 0x2000;
                        break;
                    default:
                    case 1:
                        p->ang = 0;
                        break;
                    }
                    p->angspd = 0;
                    break;
                case 4:
                    switch (p->idx) {
                    case 0:
                    case 1:
                        p->ang = 0;
                        break;
                    default:
                    case 2:
                    case 3:
                        p->ang = (u16)ran_suu(1);
                        break;
                    }
                    p->angspd = 0;
                    break;
                case 5:
                    p->ang = 0;
                    p->angspd = 0;
                    break;
                case 6:
                    switch (p->idx) {
                    case 3:
                        p->ang = 0x4000;
                        break;
                    default:
                    case 1:
                        p->ang = 0;
                        break;
                    }
                    p->angspd = 0;
                    break;
                case 7:
                    p->time = -i * 5 - 10;
                    p->ang = 0x2000;
                    p->angspd = 0;
                    break;
                case 8:
                    p->time = 0;
                    p->ang = 0;
                    p->angspd = 0;
                    p->col[0] = 0.3f;
                    p->col[1] = 0.3f;
                    p->col[2] = 0.3f;
                    break;
                }
                p->pr = get_prim_ptr(p->prim);
                *(E25 **)(p->pr + 0x18) = e;
                *(s32 *)(p->pr + 0x1C) = i;
                *(void **)(p->pr + 0x14) = eft25_t;
            } else {
                p->pr = 0;
            }
            p += 1;
            i++;
            lag += 1;
        } while (i < nn);
    }
}
