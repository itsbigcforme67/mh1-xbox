/* lb_ge2505 - eft25 0x0060F8D0-0x00610288: eft25_t (particle draw callback). Whole file in lb_e25.c. */
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
    u8 f5;                     /* 0x05 */
    u8 f6;                     /* 0x06 */
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
void eft_trans_sub_col(int, u8 *, int, u16, int);
void flSetRenderState();
void flmatMakeTrans(u8 *, f32, f32, f32);
void flmatMul33_2();
void make_mat_srt();
extern u8 eft25_type2_col[];
extern s32 *eft25_type5_fade_data[];
extern char fade_type1_74[];
extern char fade_type3_64[];
extern u8 *eft_mdlw[5];
void eft_alpha_linear();
void flvecRotY();
f32 flSin();
void flmatCopy();
void *get_joint_wmat();
void flvecApplyMat33_2();

#define E25_RAD(a) (2.0f * (3.1415927f * (((360.0f * (f32)(a)) / 65536.0f) / 360.0f)))

/* particle draw callback (called through the prim): sets up scale/rotation/colour for each effect type and submits the quad */
void eft25_t(u8 *prim, int arg1) {
    E25 *e;
    E25P *p;
    u8 *vp;
    f32 sc[3];
    f32 rot[3];
    u8 mat[0x40];
    u8 tr[0x40];
    s32 col;
    int x10;
    int tex;
    u16 opt;
    int mode;
    u8 b;
    u8 r;
    u8 g;
    int a;
    e = *(E25 **)(prim + 0x18);
    p = &e->part[*(s32 *)(prim + 0x1C)];
    opt = 0;
    vp = eft_mdlw[0];
    if (vp != 0 && *vp != 0) {
        x10 = *(s32 *)(vp + 0x10);
        sc[0] = p->scale * p->col[0];
        sc[1] = p->scale * p->col[1];
        sc[2] = p->scale * p->col[2];
        switch (e->type) {
        case 0:
            g = r = b = 0xFF;
            switch (p->idx) {
            case 0:
                mode = 0;
                tex = *(s32 *)(vp + 0x30) + 0x4600;
                break;
            case 1:
                mode = 0;
                opt |= 2;
                r = 0xEA;
                g = 0x94;
                tex = *(s32 *)(vp + 0x30) + 0x238C;
                break;
            case 2:
                mode = 2;
                opt |= 2;
                r = 0x7C;
                g = 0x76;
                tex = *(s32 *)(vp + 0x30) + 0x42B8;
                break;
            case 3:
                opt |= 2;
                tex = *(s32 *)(vp + 0x30) + 0x2878;
                rot[2] = E25_RAD((u32)p->ang);
                mode = 2;
                b = 0xFF;
                r = 0x39;
                g = 0x76;
                break;
            case 4:
            case 5:
            case 6:
                tex = *(s32 *)(vp + 0x30) + 0x4600;
                rot[2] = E25_RAD((u32)p->ang);
                mode = 2;
                break;
            }
            col = (g & 0xFF) | (((r & 0xFF) << 8) | ((((u32)(255.0f * p->alpha[0]) & 0xFF) << 24) | ((b & 0xFF) << 16)));
            break;
        case 1:
            mode = 0;
            tex = *(s32 *)(vp + 0x30) + 0x2878;
            eft_rgba_linear(fade_type1_74, p->time, &col);
            break;
        case 2:
            tex = *(s32 *)(vp + 0x30) + 0x2760;
            rot[2] = E25_RAD((u32)p->ang);
            a = e->x7 * 4;
            mode = 2;
            col = eft25_type2_col[a + 2] | ((eft25_type2_col[a + 1] << 8) | ((eft25_type2_col[a] << 16) | (((u32)((f32)(u32)eft25_type2_col[a + 3] * p->alpha[0]) & 0xFF) << 24)));
            break;
        case 3:
            opt |= 2;
            switch (p->idx) {
            case 0:
                tex = *(s32 *)(vp + 0x30) + 0x143C;
                col = -0x81;
                flmatMakeTrans(tr, 0.0f, 0.0f, 0.0f);
                flSetRenderState(0x19, (int)tr);
                break;
            default:
            case 1:
                tex = *(s32 *)(vp + 0x30) + 0x2300;
                eft_rgba_linear(fade_type3_64, p->time, &col);
                break;
            }
            rot[2] = E25_RAD((u32)p->ang);
            mode = 2;
            break;
        case 4:
            switch (p->idx) {
            case 0:
                b = 0xFF;
                r = 0xDF;
                g = 0xBF;
                tex = *(s32 *)(vp + 0x30) + 0x2300;
                opt |= 2;
                break;
            case 1:
                b = 0xFF;
                r = 0xDF;
                g = 0x3F;
                tex = *(s32 *)(vp + 0x30) + 0x2300;
                opt |= 2;
                break;
            case 2:
            case 3:
            default:
                tex = *(s32 *)(vp + 0x30) + 0x1324;
                break;
            }
            rot[2] = E25_RAD((u32)p->ang);
            mode = 2;
            col = (g & 0xFF) | (((r & 0xFF) << 8) | ((((u32)(255.0f * p->alpha[0]) & 0xFF) << 24) | ((b & 0xFF) << 16)));
            break;
        case 5:
            mode = 0;
            tex = *(s32 *)(vp + 0x30) + 0x468C;
            flmatMakeTrans(tr, 0.0625f * (f32)(e->f5 >> 1), 0.0625f * (f32)(e->f5 & 1), 0.0f);
            flSetRenderState(0x19, (int)tr);
            eft_rgba_linear(eft25_type5_fade_data[e->f6], p->time, &col);
            break;
        case 6:
            switch (p->idx) {
            case 0:
            case 1:
            case 2:
            case 3:
                tex = *(s32 *)(vp + 0x30) + 0x2300;
                break;
            case 4:
                tex = *(s32 *)(vp + 0x30) + 0x143C;
                break;
            case 5:
            case 6:
            default:
                tex = *(s32 *)(vp + 0x30) + 0x3020;
                break;
            }
            opt |= 2;
            rot[2] = E25_RAD((u32)p->ang);
            mode = 2;
            col = (((u32)(255.0f * p->alpha[0]) & 0xFF) << 24) | 0x7FDFFF;
            break;
        case 7:
            tex = *(s32 *)(vp + 0x30) + 0x143C;
            opt |= 2;
            rot[2] = E25_RAD((u32)p->ang);
            mode = 2;
            col = -1;
            break;
        case 8:
            tex = *(s32 *)(vp + 0x30) + 0xC94;
            col = -1;
            mode = 0;
            break;
        }
        make_mat_srt(sc, rot, prim + 8, mode & 0xFFFF, mat);
        flmatMul33_2(mat, rview_mat);
        eft_trans_sub_col(tex, mat, col, opt, x10);
    }
}
