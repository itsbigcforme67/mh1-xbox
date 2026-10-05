/* eft14 - game.bin 0x005498E0-0x0054A408: eft14_move to eft14_m.
 * eft14_m00 is still assembly (near-match in eft14_nm.c, which also says
 * what the effect does); the rest is in eft14b.c. */
#include "eft.h"
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

/* Flash effect handed to the renderer with push_senko/pull_senko. */
typedef struct SENKO {
    f32 pos[3];         /* 0x00 */
    f32 size;           /* 0x0C */
    u8 x10;             /* 0x10 */
    u8 _pad11[4];
    u8 x15;             /* 0x15 */
} SENKO;

/* One sprite (0x34 bytes) of the work area. */
typedef struct EFT14_PIECE {
    s16 uv;             /* 0x00 animation frame */
    s16 lag;            /* 0x02 frame counter, starts negative */
    s16 prim_no;        /* 0x04 */
    s16 no;             /* 0x06 sprite kind, 0xFF = unused */
    u16 rot;            /* 0x08 */
    s16 drot;           /* 0x0A */
    f32 scale[3];       /* 0x0C from the keyframes */
    f32 pos[3];         /* 0x18 */
    f32 alpha;          /* 0x24 */
    f32 size;           /* 0x28 */
    f32 dy;             /* 0x2C */
    PRIM *prim;         /* 0x30 */
} EFT14_PIECE;

#define ANG2DEG(a) (360.0f * (f32)(a) / 65536.0f)
#define DEG2RAD(d) (2.0f * (3.1415927f * ((d) / 360.0f)))

extern EFT_MDLW *eft_mdlw[5];
extern FLMAT rview_mat;
extern void *eft14_data[26];
extern s16 eft14_num[11];
extern s16 eft14_type4_lag[7];
extern f32 eft14_type3_scale[5][2];
extern s16 type1_uv78[15];
extern s16 uv78_00646630[];
extern s16 flash_flag;
extern s16 flash_timer;

u32 ran_suu(int);
u8 Pl_stg_ck(PLW *);
void release_prim(s16);
void flvecCopy(f32 *, f32 *);
void flvecRotY(f32 *, f32);
void flvecApplyMat33(f32 *, f32 *, FLMAT *);
void flvecApplyMat33_2(f32 *, FLMAT *);
void flmatRotZ33(FLMAT *, f32);
void eft_vec_linear(f32, void *, f32 *);
void eft_alpha_linear(f32, void *, f32 *);
void make_mat_srt(f32 *, f32 *, f32 *, u16, FLMAT *);
void eft_trans_sub(CLAY *, FLMAT *, u16, f32, void *);
void eft_trans_sub_col(CLAY *, FLMAT *, u32, u16, void *);
void SetTrnslMode(int, int);
void push_senko(SENKO *);
void pull_senko(SENKO *);
void set12_set(int, int, int, f32 *, s16);

void eft14_move(EFTW *ew);
void eft14_i(EFTW *ew);
void eft14_type3_init_sub(EFT14_PIECE *p);
void eft14_type6_init_sub(EFT14_PIECE *p);
void eft14_i00(EFTW *ew);
void eft14_i01(EFTW *ew);
void eft14_m(EFTW *ew);
void eft14_m00(EFTW *ew);
void eft14_m01(EFTW *ew);
void eft14_d(EFTW *ew);
void eft14_e(EFTW *ew);
void eft14_t(PRIM *pr);
void Eft14_set3(f32 *pos, s16 arg, f32 scale, PLW *pl);

void eft14_move(EFTW *ew) {
    switch (ew->mode) {
    case 0:
        eft14_i(ew);
        break;
    case 1:
        eft14_m(ew);
        break;
    case 2:
        eft14_d(ew);
        break;
    case 3:
        eft14_e(ew);
        break;
    }
}

void eft14_i(EFTW *ew) {
    ew->mode++;
    ew->mode2 = 0;
    ew->be_flag = 1;
    ew->work14 = 0;
    ew->timer = 0;
    if (ew->arg != 2) {
        eft14_i00(ew);
    } else {
        eft14_i01(ew);
    }
}

void eft14_type3_init_sub(EFT14_PIECE *p) {
    s16 n;

    p->lag = -9;
    p->uv = (u16)ran_suu(1) & 0x1F;
    n = (u16)ran_suu(1) % 5;
    p->scale[0] = eft14_type3_scale[n][0];
    p->scale[1] = eft14_type3_scale[n][1];
    p->scale[2] = 1.0f;
    p->pos[0] = 0.015000001f * (f32)(((u16)ran_suu(1) & 0x3FF) - 0x200);
    p->pos[1] = 0.005f * (f32)((u16)ran_suu(1) & 0x3FF);
    p->pos[2] = 0.015000001f * (f32)(((u16)ran_suu(1) & 0x3FF) - 0x200);
}

void eft14_type6_init_sub(EFT14_PIECE *p) {
    s16 n;

    p->uv = (u16)ran_suu(1) & 0x1F;
    n = (u16)ran_suu(1) % 5;
    p->scale[0] = eft14_type3_scale[n][0];
    p->scale[1] = eft14_type3_scale[n][1];
    p->scale[2] = 1.0f;
    p->pos[0] = 0.010000001f * (f32)(((u16)ran_suu(1) & 0x3FF) - 0x200);
    p->pos[1] = 0.0050000004f * (f32)((u16)ran_suu(1) & 0x3FF);
    p->pos[2] = 0.010000001f * (f32)(((u16)ran_suu(1) & 0x3FF) - 0x200);
}

void eft14_i00(EFTW *ew) {
    EFT14_PIECE *w = ew->work;
    s16 n = eft14_num[ew->arg];
    s16 i;
    f32 dy;

    if (ew->arg == 3) {
        set12_set(7, 10, 9, ew->pos, -1);
    }
    for (i = 0; i < n; i++) {
        w[i].prim_no = get_prim();
        if (w[i].prim_no != -1) {
            w[i].prim = get_prim_ptr(w[i].prim_no);
            w[i].prim->owner = ew;
            w[i].prim->no = i;
            w[i].lag = 0;
            w[i].uv = 0;
            switch (ew->arg) {
            case 0:
                w[i].no = i;
                if (i == 0) {
                    w[i].pos[0] = 0.0f;
                    w[i].pos[1] = 0.0f;
                    w[i].pos[2] = 0.0f;
                }
                if (i == 1 || i == 2) {
                    if ((u16)ran_suu(1) & 1) {
                        w[i].no = 0xFF;
                    } else {
                        w[i].lag = -(((u16)ran_suu(1) & 3) + 3);
                        w[i].pos[0] = 0.2f * ew->scale * (f32)(((u16)ran_suu(1) & 0x3FF) - 0x200);
                        w[i].pos[1] = 0.2f * ew->scale * (f32)(((u16)ran_suu(1) & 0x3FF) - 0x200);
                        w[i].pos[2] = 0.0f;
                    }
                }
                w[i].rot = ran_suu(1);
                break;
            case 1:
                w[i].no = i;
                w[i].lag = -3 - i * 2;
                w[i].pos[0] = ew->scale * (f32)(((u16)ran_suu(1) & 0x1F) - 0x10);
                w[i].pos[1] = ew->scale * (f32)(((u16)ran_suu(1) & 0x1F) - 0x10);
                w[i].pos[2] = 20.0f + (f32)(i * 20);
                w[i].size = ew->scale * (1.0f + 0.5f * (0.001f * (f32)((u16)ran_suu(1) & 0x3FF)));
                w[i].rot = ran_suu(1);
                break;
            case 3:
                if (i == 0) {
                    w[i].no = 0xFF;
                    w[i].lag = 0;
                    w[i].uv = (u16)ran_suu(1) & 0xF;
                    w[i].scale[0] = 2.0f;
                    w[i].scale[1] = 2.0f;
                    w[i].scale[2] = 1.0f;
                    w[i].alpha = 1.0f;
                    w[i].pos[0] = 0.0f;
                    w[i].pos[1] = 0.0f;
                    w[i].pos[2] = 0.0f;
                } else {
                    w[i].no = i - 1;
                    eft14_type3_init_sub(&w[i]);
                    w[i].lag = -w[i].no * 5;
                }
                w[i].size = 1.0f;
                break;
            case 4:
                w[i].lag = eft14_type4_lag[i];
                if (i < 3) {
                    w[i].rot = 0;
                    w[i].no = i;
                } else {
                    w[i].rot = ran_suu(1);
                    w[i].no = 3;
                }
                flvecCopy(w[i].pos, ew->pos);
                break;
            case 5:
                w[i].no = i;
                flvecCopy(w[i].pos, ew->pos);
                break;
            case 6:
                w[i].no = i - 1;
                eft14_type6_init_sub(&w[i]);
                w[i].lag = -i * 5;
                w[i].size = 1.0f;
                break;
            case 8:
                w[i].lag = -6;
                w[i].rot = ran_suu(1);
                w[i].no = (u16)ran_suu(1) & 7;
                if ((w[i].no & 2) == 0) {
                    dy = 0.5f;
                    w[i].drot = 0.5f + 65536.0f * (0.375f * (0.001f * (f32)((u16)ran_suu(1) & 0x3FF))) / 360.0f;
                } else {
                    dy = 0.5f;
                    w[i].drot = 0.5f + 65536.0f * (0.5f * (0.001f * (f32)((u16)ran_suu(1) & 0x3FF))) / 360.0f;
                }
                if (w[i].no & 4) {
                    w[i].drot = -w[i].drot;
                }
                w[i].size = 0.7f + 0.0008f * (f32)((u16)ran_suu(1) & 0x3FF);
                w[i].dy = dy + 0.0005f * (f32)((u16)ran_suu(1) & 0x3FF);
                w[i].pos[0] = 0.2f * ew->scale * (f32)(((u16)ran_suu(1) & 0x3FF) - 0x200);
                w[i].pos[1] = 0.1f * ew->scale * (f32)(((u16)ran_suu(1) & 0x3FF) - 0x200);
                w[i].pos[2] = -30.0f - 5.0f * (f32)i;
                break;
            case 9:
                w[i].no = i;
                w[i].rot = ran_suu(1);
                w[i].alpha = 1.0f;
                break;
            case 10:
                w[i].no = 4;
                w[i].alpha = 1.0f;
                break;
            default:
                w[i].no = i;
                break;
            }
            w[i].prim->trans = eft14_t;
        } else {
            w[i].prim = 0;
        }
    }
}

void eft14_i01(EFTW *ew) {
    ew->prim_no = get_prim();
    if (ew->prim_no != -1) {
        ew->prim = get_prim_ptr(ew->prim_no);
        ew->prim->owner = ew;
        ew->prim->trans = eft14_t;
        flvecCopy(ew->prim->pos, ew->pos);
    } else {
        push_eft_work(ew);
    }
}

void eft14_m(EFTW *ew) {
    if (ew->arg != 2) {
        eft14_m00(ew);
    } else {
        eft14_m01(ew);
    }
}
