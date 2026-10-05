/* eft14 - game.bin 0x005498E0-0x0054BB54. Sparks and flashes with eleven
 * types (arg): up to seven sprites each (eft14_num) that wait out a lag,
 * then scale and fade along keyframe tables (eft14_data) while their UVs
 * step through animation frames (uv78 tables). Type 2 is a single
 * flickering sprite drawn from the effect itself (eft14_i01/m01). Type 5
 * also pushes a screen flash (push_senko) and raises flash_flag; type 0
 * spawns types 8 and 9 with it.
 * Near-match for the whole file: eft14_m00 keeps a dead loop counter in
 * its UV-frame searches that our build drops (about 300 instructions
 * shift). */
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

static void eft14_move(EFTW *ew);
static void eft14_i(EFTW *ew);
static void eft14_type3_init_sub(EFT14_PIECE *p);
static void eft14_type6_init_sub(EFT14_PIECE *p);
static void eft14_i00(EFTW *ew);
static void eft14_i01(EFTW *ew);
static void eft14_m(EFTW *ew);
static void eft14_m00(EFTW *ew);
static void eft14_m01(EFTW *ew);
static void eft14_d(EFTW *ew);
static void eft14_e(EFTW *ew);
static void eft14_t(PRIM *pr);
void Eft14_set3(f32 *pos, s16 arg, f32 scale, PLW *pl);

static void eft14_move(EFTW *ew) {
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

static void eft14_i(EFTW *ew) {
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

static void eft14_type3_init_sub(EFT14_PIECE *p) {
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

static void eft14_type6_init_sub(EFT14_PIECE *p) {
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

static void eft14_i00(EFTW *ew) {
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

static void eft14_i01(EFTW *ew) {
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

static void eft14_m(EFTW *ew) {
    if (ew->arg != 2) {
        eft14_m00(ew);
    } else {
        eft14_m01(ew);
    }
}

static void eft14_m00(EFTW *ew) {
    f32 v[3];
    EFT14_PIECE *p = ew->work;
    s16 n = eft14_num[ew->arg];
    u16 all;
    s16 idx;
    u16 lim;
    s16 i;
    s16 j;
    s16 k;
    void *d;

    switch (ew->arg) {
    case 0:
        all = 35;
        break;
    case 1:
        all = 25;
        lim = 18;
        break;
    case 3:
        lim = all = 26;
        break;
    case 4:
        all = 30;
        if (ew->timer == 12) {
            Eft14_set3(ew->pos, 5, ew->scale, (PLW *)ew->owner);
        }
        break;
    case 5:
        all = 20;
        switch (ew->mode2) {
        case 0:
            if (ew->timer >= 9) {
                if (((PLW *)ew->owner)->x10 == 0) {
                    flvecCopy(((SENKO *)(p + 7))->pos, ew->pos);
                    ((SENKO *)(p + 7))->size = 2500.0f;
                    ((SENKO *)(p + 7))->x10 = ((PLW *)ew->owner)->id;
                    ((SENKO *)(p + 7))->x15 = 5;
                    push_senko((SENKO *)(p + 7));
                }
                ew->mode2++;
                if (flash_flag == 3) {
                    if (flash_timer < 51) {
                        flash_flag = 1;
                        flash_timer = 2;
                    }
                } else if (flash_flag == 0) {
                    flash_flag = 1;
                    flash_timer = 2;
                }
            }
            break;
        case 1:
        default:
            ((SENKO *)(p + 7))->x15 = 5;
            break;
        }
        break;
    case 6:
        all = 36;
        lim = 26;
        break;
    case 8:
        all = 88;
        break;
    case 9:
        lim = all = 2;
        break;
    case 10:
        all = 9;
        idx = 6;
        lim = all;
        break;
    }
    ew->timer++;
    if (ew->arg != 3 && ew->timer > all) {
        ew->mode++;
        ew->be_flag = 0;
        return;
    }
    for (i = 0; i < n; i++) {
        switch (ew->arg) {
        case 0:
        case 10:
            switch (p->no) {
            case 0:
            case 1:
            case 2:
                idx = 0;
                lim = 27;
                break;
            case 3:
                idx = 2;
                lim = 8;
                break;
            case 4:
                idx = 4;
                lim = 9;
                break;
            case 0xFF:
                p++;
                continue;
            }
            break;
        case 1:
            idx = 18;
            break;
        case 3:
        case 6:
            idx = 25;
            break;
        case 4:
            switch (p->no) {
            case 0:
                idx = 6;
                lim = 8;
                break;
            case 1:
                idx = 8;
                lim = 6;
                break;
            case 2:
                idx = 10;
                lim = 11;
                break;
            case 3:
                idx = 12;
                lim = 21;
                break;
            }
            break;
        case 5:
            switch (p->no) {
            case 0:
                idx = 14;
                lim = 20;
                break;
            case 1:
            default:
                idx = 16;
                lim = 14;
                break;
            }
            break;
        case 8:
            if ((p->no & 2) == 0) {
                idx = 20;
                lim = 80;
            } else {
                idx = 22;
                lim = 60;
            }
            break;
        case 9:
            idx = 24;
            break;
        }
        if (p->no != 0xFF) {
            if (++p->lag <= 0) {
                p++;
                continue;
            }
        }
        if (p->no != 0xFF && p->lag > lim) {
            if (ew->arg == 3) {
                eft14_type3_init_sub(p);
            }
            p++;
            continue;
        }
        if (ew->arg != 3 && ew->arg != 6) {
            d = eft14_data[idx++];
            eft_vec_linear(p->lag, d, p->scale);
        }
        if (ew->arg != 9 && p->no != 0xFF) {
            d = eft14_data[idx++];
            eft_alpha_linear(p->lag, d, &p->alpha);
        }
        if (ew->arg == 0 && (p->no == 0 || p->no == 1 || p->no == 2)) {
            for (j = 0; uv78_00646630[j] != -1; j++) {
                if (p->lag == uv78_00646630[j]) {
                    p->uv++;
                    break;
                }
            }
        } else if (ew->arg == 1) {
            for (j = 0; type1_uv78[j] != -1; j++) {
                if (p->lag == type1_uv78[j]) {
                    p->uv++;
                    break;
                }
            }
        } else if (ew->arg == 3 || ew->arg == 6) {
            k = p->uv & 0xF;
            p->uv &= ~0xF;
            k++;
            p->uv |= k & 0xF;
        }
        if (p->prim != 0) {
            switch (ew->arg) {
            case 0:
                flvecCopy(p->prim->pos, ew->pos);
                if (p->no < 3) {
                    v[0] = p->pos[0];
                    v[1] = p->pos[1];
                    v[2] = 10.0f + p->pos[2];
                } else if (p->no == 3) {
                    v[0] = 0.0f;
                    v[1] = 0.0f;
                    v[2] = 20.0f;
                } else {
                    v[0] = 0.0f;
                    v[1] = 0.0f;
                    v[2] = 0.0f;
                }
                flvecApplyMat33_2(v, &rview_mat);
                p->prim->pos[0] += v[0];
                p->prim->pos[1] += v[1];
                p->prim->pos[2] += v[2];
                break;
            case 1:
                flvecCopy(p->prim->pos, ew->pos);
                flvecApplyMat33(v, p->pos, &rview_mat);
                p->prim->pos[0] += v[0];
                p->prim->pos[1] += v[1];
                p->prim->pos[2] += v[2];
                break;
            case 3:
            case 6:
                p->prim->pos[0] = ew->pos[0] + p->pos[0];
                p->prim->pos[1] = ew->pos[1] + p->pos[1];
                p->prim->pos[2] = ew->pos[2] + p->pos[2];
                break;
            case 4:
                if (p->no == 2) {
                    p->pos[1] -= 5.0f;
                }
            case 5:
                flvecCopy(p->prim->pos, p->pos);
                v[0] = 0.0f;
                v[1] = 0.0f;
                v[2] = 15.0f - 5.0f * (f32)i;
                if (ew->arg == 5) {
                    v[2] += 10.0f;
                }
                flvecApplyMat33_2(v, &rview_mat);
                p->prim->pos[0] += v[0];
                p->prim->pos[1] += v[1];
                p->prim->pos[2] += v[2];
                break;
            case 8:
                p->pos[1] += p->dy;
                p->rot += p->drot;
                flvecApplyMat33(v, p->pos, &rview_mat);
                p->prim->pos[0] = ew->pos[0] + v[0];
                p->prim->pos[1] = ew->pos[1] + v[1];
                p->prim->pos[2] = ew->pos[2] + v[2];
                break;
            case 9:
                v[0] = 0.0f;
                v[1] = 0.0f;
                v[2] = (f32)(25 + i * 5);
                flvecApplyMat33_2(v, &rview_mat);
                p->prim->pos[0] = ew->pos[0] + v[0];
                p->prim->pos[1] = ew->pos[1] + v[1];
                p->prim->pos[2] = ew->pos[2] + v[2];
                break;
            case 10:
                flvecCopy(p->prim->pos, ew->pos);
                break;
            }
            add_prim(ot0, p->prim, 0x40, 0);
        }
        p++;
    }
}

static void eft14_m01(EFTW *ew) {
    ew->timer += (s16)(((u16)ran_suu(1) & 1) + 1);
    if (ew->timer > 16) {
        ew->timer = 1;
    }
    add_prim(ot0, ew->prim, 0x40, 0);
}

static void eft14_d(EFTW *ew) {
    EFT14_PIECE *p = ew->work;
    s16 n;
    s16 i;

    ew->mode++;
    ew->be_flag = 0;
    if (ew->arg == 5) {
        pull_senko((SENKO *)(p + 7));
    }
    n = eft14_num[ew->arg];
    if (n == 0) {
        release_prim(ew->prim_no);
    } else {
        for (i = 0; i < n; i++, p++) {
            if (p->prim != 0) {
                release_prim(p->prim_no);
            }
        }
    }
}

static void eft14_e(EFTW *ew) {
    push_eft_work(ew);
}

static void eft14_t(PRIM *pr) {
    f32 rot[3];
    f32 sc[3];
    FLMAT m;
    FLMAT uv;
    EFTW *ew = pr->owner;
    EFT14_PIECE *p = &((EFT14_PIECE *)ew->work)[pr->no];
    EFT_MDLW *mw = eft_mdlw[0];
    s16 cl_no;
    u16 flag = 0;
    void *mats;
    f32 alpha;
    CLAY *cl;

    if (mw != 0 && mw->flag != 0) {
        mats = mw->mat;
        alpha = p->alpha;
        switch (ew->arg) {
        case 0:
            flSetRenderState(0x6C, 0);
            flag |= 2;
            sc[0] = ew->scale * p->scale[0];
            sc[1] = ew->scale * p->scale[1];
            sc[2] = ew->scale * p->scale[2];
            switch (p->no) {
            case 0:
            case 1:
            case 2:
                cl_no = 78;
                flmatMakeTrans(&uv, 0.25f * (f32)(p->uv & 3), 0.25f * (f32)(p->uv / 4), 0.0f);
                flSetRenderState(0x19, (u32)&uv);
                break;
            case 3:
                cl_no = 80;
                break;
            case 4:
                cl_no = 88;
                break;
            }
            break;
        case 1:
            sc[0] = p->size * p->scale[0];
            sc[1] = p->size * p->scale[1];
            sc[2] = p->size * p->scale[2];
            cl_no = 78;
            flag |= 2;
            flmatMakeTrans(&uv, 0.25f * (f32)(p->uv & 3), 0.25f * (f32)(p->uv >> 2), 0.0f);
            flSetRenderState(0x19, (u32)&uv);
            break;
        case 2:
            cl_no = 21;
            sc[0] = ew->scale;
            sc[1] = ew->scale;
            sc[2] = ew->scale;
            flSetRenderState(0x6C, 0);
            flag |= 2;
            alpha = 0.05f + 0.025f * (1.0f + flSin(DEG2RAD(360.0f * ((f32)ew->timer / 16.0f) - 90.0f)));
            break;
        case 3:
        case 6:
            cl_no = 96;
            sc[0] = p->scale[0];
            sc[1] = p->scale[1];
            sc[2] = p->scale[2];
            flSetRenderState(0x6C, 0);
            flag |= 2;
            flmatMakeTrans(&uv, 0.0625f * (f32)(p->uv & 7), 0.0625f * (f32)(p->uv >> 3), 0.0f);
            flSetRenderState(0x19, (u32)&uv);
            break;
        case 4:
            sc[0] = p->scale[0];
            sc[1] = p->scale[1];
            sc[2] = p->scale[2];
            switch (p->no) {
            case 0:
            case 1:
                flag |= 2;
                cl_no = 21;
                break;
            case 2:
                cl_no = 35;
                break;
            case 3:
                cl_no = 72;
                sc[0] *= 2.0f;
                sc[1] *= 2.0f;
                sc[2] *= 2.0f;
                break;
            }
            break;
        case 5:
            sc[0] = p->scale[0];
            sc[1] = p->scale[1];
            sc[2] = p->scale[2];
            cl_no = pr->no + 81;
            if (p->no < 2) {
                if (flash_flag != 1 && flash_flag != 2) {
                    flag |= 2;
                }
            }
            break;
        case 8:
            flSetRenderState(0x6C, 0);
            sc[0] = p->scale[0] * (ew->scale * p->size);
            sc[1] = p->scale[1] * (ew->scale * p->size);
            sc[2] = p->scale[2] * (ew->scale * p->size);
            if ((p->no & 1) == 0) {
                cl_no = 77;
                flmatMakeTrans(&uv, 0.0f, 0.25f, 0.0f);
            } else {
                flmatMakeTrans(&uv, 0.0f, 0.0f, 0.0f);
                cl_no = 79;
            }
            flSetRenderState(0x19, (u32)&uv);
            break;
        case 9:
            sc[0] = 3.0f * ew->scale * p->scale[0];
            sc[1] = 3.0f * ew->scale * p->scale[1];
            sc[2] = 3.0f * ew->scale * p->scale[2];
            flmatMakeTrans(&uv, 0.25f, 0.0f, 0.0f);
            flSetRenderState(0x19, (u32)&uv);
            flag |= 2;
            cl_no = 34;
            break;
        }
        flSetRenderState(0x60, 0);
        make_mat_srt(sc, rot, pr->pos, 0, &m);
        if (ew->arg == 10) {
            flmatRotZ33(&m, DEG2RAD(ANG2DEG(ew->u0A.joint)));
        } else if (ew->arg <= 1U || ew->arg == 4 || ew->arg == 8 || ew->arg == 9) {
            flmatRotZ33(&m, DEG2RAD(ANG2DEG(p->rot)));
        }
        flmatMul33_2(&m, &rview_mat);
        cl = &mw->clay[cl_no];
        if (ew->arg == 3 || ew->arg == 6) {
            eft_trans_sub_col(cl, &m, ((u8)(255.0f * alpha) << 24) | 0xFF5F00, flag, mats);
        } else {
            if (ew->arg == 5 && (flash_flag == 1 || flash_flag == 2)) {
                flag |= 1;
            }
            eft_trans_sub(cl, &m, flag, alpha, mats);
        }
        SetTrnslMode(4, 5);
        flSetRenderState(0x6C, 1);
    }
}

static void eft14_set(f32 *pos, s16 arg, f32 scale) {
    EFTW *ew;

    if ((ew = pull_eft_work(1)) != 0) {
        ew->type = 0xE;
        ew->move = eft14_move;
        ew->arg = arg;
        ew->owner = 0;
        ew->scale = scale;
        ew->pos[0] = pos[0];
        ew->pos[1] = pos[1];
        ew->pos[2] = pos[2];
        if (arg < 5) {
            ew->u0A.ang = ran_suu(1);
        }
        if (arg == 0) {
            eft14_set(pos, 8, scale);
            eft14_set(pos, 9, scale);
        }
    }
}

void Eft14_set2(f32 *pos, s16 arg) {
    EFTW *ew;

    if (arg == 2) {
        ew = pull_eft_work(0);
    } else {
        ew = pull_eft_work(1);
    }
    if (ew != 0) {
        ew->type = 0xE;
        ew->move = eft14_move;
        ew->arg = arg;
        ew->owner = 0;
        if (arg == 2) {
            ew->scale = 2.0f;
        } else {
            ew->scale = 1.0f;
        }
        ew->pos[0] = pos[0];
        ew->pos[1] = pos[1];
        ew->pos[2] = pos[2];
    }
}

void Eft14_set3(f32 *pos, s16 arg, f32 scale, PLW *pl) {
    EFTW *ew;

    if (Pl_stg_ck(pl) != 0) {
        if ((ew = pull_eft_work(1)) != 0) {
            ew->type = 0xE;
            ew->move = eft14_move;
            ew->arg = arg;
            ew->owner = (EMW *)pl;
            ew->scale = scale;
            ew->pos[0] = pos[0];
            ew->pos[1] = pos[1];
            ew->pos[2] = pos[2];
            if (arg < 5) {
                ew->u0A.ang = ran_suu(1);
            }
        }
    }
}

void Eft14_set4(PLW *pl, int arg) {
    f32 v[3];
    EFTW *ew;

    if (Pl_stg_ck(pl) != 0) {
        if ((ew = pull_eft_work(1)) != 0) {
            ew->type = 0xE;
            ew->move = eft14_move;
            ew->arg = arg;
            ew->owner = (EMW *)pl;
            ew->scale = 1.0f;
            v[0] = 0.0f;
            v[1] = 0.0f;
            v[2] = 128.0f;
            flvecRotY(v, DEG2RAD(ANG2DEG(pl->ang[1])));
            ew->pos[0] = pl->pos[0] + v[0];
            ew->pos[1] = pl->x5AC + v[1];
            ew->pos[2] = pl->pos[2] + v[2];
        }
    }
}
