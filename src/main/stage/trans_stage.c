/* trans_stage (SLPM_654.95 0x0015CD90-0x00160890, 15152 bytes = exactly the
 * original size). NOT BUILT for the PS2 yet (near-match) but it is the single
 * definition: the PC runtime links this file too (tools/build_pc.sh).
 * Pass 1 draws the stage model's clay layers (layer 0 with state 0x6D = 7, the
 * rest with per-stage UV scrolling / rotation), pass 2 draws the set objects of
 * the stage (set_mdlw clays at fixed positions from the setNN_pos_tbl tables).
 * Against the asm: the first pass is instruction-identical apart from register
 * numbers; the second pass differs in which s-register each per-case local
 * lives in. Checked with tools/align.py. (It replaces agent A's rewrite
 * trans_stage_nm.c: a call-trace comparison of the two over all 88 stages
 * differed only for stage 0x28, where the original falls through from the
 * case 0x28 body into the 0x3B code, so the layer is drawn twice.)
 * trans_stage_sub is in f_stagec.c (PS2) / rt_main.c (PC). */
#include "flow.h"

/* ---- trans_stage (0x15CD90-0x160890): translucent/animated stage layers ----
 * Pass 1 draws the stage model's clay layers (layer 0 with state 0x6D = 7, the rest
 * with per-stage UV scrolling / rotation), pass 2 draws the set objects of the stage
 * (set_mdlw clays at fixed positions from the setNN_pos_tbl tables). */
typedef f32 SFLMAT[4][4];

typedef struct SCLAY {
    s32 handle;         /* 0x00 */
    u8 _pad04[0x84];
    s32 attr;           /* 0x88 */
} SCLAY;                /* 0x8C */

typedef struct STG_MDLS {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x2B];
    s16 num;            /* 0x2C layers */
    u8 _pad2E[2];
    SCLAY *clay;        /* 0x30 */
} STG_MDLS;

extern STG_MDLS *set_mdlw;
extern f32 st00_pos_tbl[4][3];
extern f32 set04_pos_tbl[3];
extern f32 set05_pos_tbl1[2][3];
extern f32 set09_pos_tbl[14][4];
extern f32 set20_pos_tbl[2][3];
extern f32 set28_pos_tbl[4];
extern f32 set33_pos_tbl[9][4];
extern f32 set34_pos_tbl[9][4];
extern f32 set36_pos_tbl[2][4];
extern f32 set38_pos_tbl[11][4];
extern f32 set39_pos_tbl[2][3];
extern f32 set43_pos_tbl[2][4];
extern f32 set45_pos_tbl[4][4];
extern f32 set50_pos_tbl[3];
extern f32 set54_pos_tbl[3];
extern f32 set58_pos_tbl[6][4];
extern f32 set62_pos_tbl[3][4];
extern f32 set63_pos_tbl[5][4];
extern f32 set64_pos_tbl[3][4];
extern f32 set71_pos_tbl[11][4];
extern f32 set72_pos_tbl[6][4];
extern f32 set73_pos_tbl[2][4];
extern f32 set75_pos_tbl[2][4];
void light_set();
void get_tex_num();
void reload_tex();
void clay_attr_reset(void);
void flmatInit(SFLMAT *);
void flmatMakeTrans(SFLMAT *, f32, f32, f32);
void flmatMakeScale(SFLMAT *, f32, f32, f32);
void flmatSetTrans(SFLMAT *, f32, f32, f32);
void flmatSetXYZ33(SFLMAT *, f32, f32, f32);
void flmatRotX33(SFLMAT *, f32);
void flmatRotY33(SFLMAT *, f32);
f32 flSin(f32);

#define ANG(x) (2.0f * (3.1415927f * (360.0f * (f32)(x) / 65536.0f / 360.0f)))
#define RS flSetRenderState
#define MT(x, y, z) flmatMakeTrans(&mat, x, y, z)
#define MTW() flmatMakeTrans(&mat, w->pos[0], w->pos[1], w->pos[2])
#define MTP(p) flmatMakeTrans(&mat, (p)[0], (p)[1], (p)[2])
#define SXA(a) flmatSetXYZ33(&mat, 0.0f, ANG(a), 0.0f)
#define SXW() flmatSetXYZ33(&mat, w->rot[0], w->rot[1], w->rot[2])
#define UVT(u, v) flmatMakeTrans(&mat2, u, v, 0.0f)
#define EXC() flExecuteClay(mdl->handle, 0)
#define FRM ((u16)w->x08)
#define SFRM ((s16)w->x08)
#define X1E (*(u16 *)&game_w.x1E)
void trans_stage_sub();

void trans_stage(void)
{
    int n;
    STG_MDLS *m;
    f32 *p;
    int i;
    int k;
    STGW *w = &stage_work;
    SCLAY *mdl;
    SFLMAT mat;
    SFLMAT mat2;
    u32 u;

    if (w->x00 != 0) {
    if (w->x01 == 0) {
    } else {
    light_set(0);
    m = (STG_MDLS *)w->mdls;
    if (m != 0 && m->flag != 0) {
    get_tex_num(0xEA);
    reload_tex(0x10, 0xEA);
    RS(0x60, 0x80);
    RS(0x67, -1);
    mdl = m->clay;
    flmatMakeTrans(&mat, w->pos[0], w->pos[1], w->pos[2]);
    flmatSetXYZ33(&mat, w->rot[0], w->rot[1], w->rot[2]);
    RS(0x1A, (u32)&mat);
    n = m->num;
    if (game_w.stage == 0 || game_w.stage == 0x1A) {
        n = (s16)n - 1;
    }
    for (i = 0; i < n; i++, mdl++) {
        clay_attr_set(mdl->attr);
        if (i == 0) {
            RS(0x6D, 7);
            switch (game_w.stage) {
            case 0x19:
                MT(13000.0f, 0.0f, 20800.0f);
                SXA((FRM & 0xFFF) << 4);
                RS(0x1A, (u32)&mat);
                EXC();
                MTW();
                SXW();
                RS(0x1A, (u32)&mat);
                break;
            case 0x3A:
            case 0x40:
                MT(10000.0f, 0.0f, 10000.0f);
                SXA((FRM & 0x1FFF) << 3);
                RS(0x1A, (u32)&mat);
                EXC();
                MTW();
                SXW();
                RS(0x1A, (u32)&mat);
                break;
            case 0x41:
                MT(11500.0f, 0.0f, 3000.0f);
                SXA((FRM & 0x1FFF) << 3);
                RS(0x1A, (u32)&mat);
                EXC();
                MTW();
                SXW();
                RS(0x1A, (u32)&mat);
                break;
            case 0x42:
                MT(12000.0f, 0.0f, 8000.0f);
                SXA((FRM & 0x1FFF) << 3);
                RS(0x1A, (u32)&mat);
                EXC();
                MTW();
                SXW();
                RS(0x1A, (u32)&mat);
                break;
            case 0x47:
            case 0x48:
            case 0x49:
            case 0x4A:
            case 0x4B:
                MT(10000.0f, 0.0f, 10000.0f);
                RS(0x1A, (u32)&mat);
                EXC();
                MTW();
                SXW();
                RS(0x1A, (u32)&mat);
                break;
            default:
                EXC();
                break;
            }
            RS(0x6D, 3);
        } else {
            switch (game_w.stage) {
            case 0:
            case 0x1A:
                if (i == 1) {
                    EXC();
                }
                break;
            case 4:
                if (i == 2) {
                    RS(0x60, 0);
                    MT(13200.0f, 0.0f, 5190.0f);
                    SXA((FRM & 0x3FFF) << 2);
                } else if (i == 5) {
                    RS(0x60, 0);
                    MTW();
                } else {
                    RS(0x60, 0x80);
                    MTW();
                }
                RS(0x1A, (u32)&mat);
                EXC();
                break;
            case 5:
                if (i == 2) {
                    for (k = 0; k < 2; k++) {
                        MTP(set05_pos_tbl1[k]);
                        RS(0x1A, (u32)&mat);
                        EXC();
                    }
                }
                if (i == 3) {
                    MT(10000.0f, 0.0f, 8500.0f);
                    RS(0x1A, (u32)&mat);
                    EXC();
                } else {
                    EXC();
                }
                break;
            case 6:
            case 7:
                if (i == 2) {
                    break;
                }
                EXC();
                break;
            case 0x15:
            case 0x18:
            case 0x22:
            case 0x23:
            case 0x24:
            case 0x29:
            case 0x2A:
            case 0x2B:
                if (i == 1) {
                    w->x20 = 0.02f + 0.02f * flSin(2.0f * (3.1415927f * ((f32)(FRM % 360) / 360.0f)));
                    UVT(0.0f, w->x20);
                    RS(0x19, (u32)&mat2);
                }
                EXC();
                break;
            case 9:
                if (i == 1) {
                    RS(0x60, 0);
                    MT(10000.0f, 0.0f, 10000.0f);
                    SXA((FRM & 0xFFF) << 4);
                    RS(0x1A, (u32)&mat);
                } else if (i == 4) {
                    RS(0x60, 0);
                    MTW();
                    RS(0x1A, (u32)&mat);
                } else {
                    RS(0x60, 0x80);
                    MTW();
                    RS(0x1A, (u32)&mat);
                }
                EXC();
                break;
            case 0xE:
                if (i == 2) {
                    RS(0x60, 0);
                    MT(11800.0f, 0.0f, 12000.0f);
                    SXA((FRM & 0xFFF) << 4);
                    RS(0x1A, (u32)&mat);
                } else if (i == 4) {
                    if (game_w.info_stop == 1) {
                        break;
                    }
                    RS(0x60, 0x80);
                    MT(11100.0f, 0.0f, 14160.0f);
                    SXA((FRM & 0x7FF) << 5);
                    RS(0x1A, (u32)&mat);
                } else {
                    RS(0x60, 0x80);
                    MTW();
                    RS(0x1A, (u32)&mat);
                }
                EXC();
                break;
            case 0x12:
                if (i == 6) {
                    RS(0x60, 0);
                    w->x20 = (f32)(SFRM & 0x3F) / 64.0f;
                    UVT(0.0f, w->x20);
                    RS(0x19, (u32)&mat2);
                    EXC();
                    RS(0x60, 0x80);
                } else {
                    MTW();
                    RS(0x1A, (u32)&mat);
                    EXC();
                }
                break;
            case 0x13:
                if (i == 2) {
                    RS(0x60, 0);
                    MT(9720.0f, 0.0f, 9300.0f);
                    SXA((FRM & 0x3FFF) << 2);
                    RS(0x1A, (u32)&mat);
                } else if (i == 7) {
                    RS(0x60, 0);
                    MTW();
                    w->x1C = (f32)(int)(u8)w->x08 / 256.0f;
                    UVT(w->x1C, 0.0f);
                    RS(0x19, (u32)&mat2);
                    RS(0x1A, (u32)&mat);
                } else {
                    MTW();
                    RS(0x1A, (u32)&mat);
                }
                EXC();
                RS(0x60, 0x80);
                break;
            case 0x16:
            case 0x17:
                if (i == 6) {
                    RS(0x60, 0);
                    w->x20 = 1.0f - (f32)(SFRM & 0x3F) / 64.0f;
                    UVT(0.0f, w->x20);
                    RS(0x19, (u32)&mat2);
                    EXC();
                    RS(0x60, 0x80);
                } else {
                    MTW();
                    RS(0x1A, (u32)&mat);
                    EXC();
                }
                break;
            case 0x19:
                if (i == 1) {
                    RS(0x60, 0);
                    w->x20 = 1.0f - (f32)(SFRM & 0x7F) / 128.0f;
                    UVT(0.0f, w->x20);
                    RS(0x19, (u32)&mat2);
                    SXA((FRM & 0x7FF) << 5);
                    flmatSetTrans(&mat, 13200.0f, 0.0f, 21000.0f);
                    RS(0x1A, (u32)&mat);
                    EXC();
                    RS(0x60, 0x80);
                } else {
                    RS(0x60, 0x80);
                    MTW();
                    RS(0x1A, (u32)&mat);
                    EXC();
                }
                break;
            case 0x1F:
                if (i == 2) {
                    RS(0x60, 0);
                    MT(10300.0f, 0.0f, 12800.0f);
                    SXA((FRM & 0xFFF) << 4);
                    RS(0x1A, (u32)&mat);
                } else if (i == 4) {
                    RS(0x60, 0);
                    MT(16600.0f, 0.0f, 6200.0f);
                    SXA((FRM & 0x7FF) << 5);
                    RS(0x1A, (u32)&mat);
                } else {
                    RS(0x60, 0x80);
                    MTW();
                    RS(0x1A, (u32)&mat);
                }
                EXC();
                break;
            case 0x20:
                if (i == 1) {
                    RS(0x60, 0);
                    flmatInit(&mat);
                    flmatRotY33(&mat, ANG((FRM & 0x3FFF) << 2));
                    RS(0x1A, (u32)&mat);
                    EXC();
                    RS(0x60, 0x80);
                } else if (i == 7) {
                    RS(0x60, 0);
                    RS(0x6C, 0);
                    MTW();
                    RS(0x1A, (u32)&mat);
                    EXC();
                    RS(0x60, 0x80);
                    RS(0x6C, 1);
                } else {
                    MTW();
                    RS(0x1A, (u32)&mat);
                    EXC();
                }
                break;
            case 0x1B:
            case 0x21:
                if (i == 2) {
                    RS(0x60, 0);
                    MT(13000.0f, 0.0f, 11500.0f);
                    SXA((FRM & 0x3FFF) << 2);
                    RS(0x1A, (u32)&mat);
                } else {
                    RS(0x60, 0x80);
                    MTW();
                    RS(0x1A, (u32)&mat);
                }
                EXC();
                break;
            case 8:
            case 0xF:
            case 0x25:
                if (i == 2) {
                    RS(0x60, 0);
                    MT(10000.0f, 0.0f, 10000.0f);
                    SXA((FRM & 0x3FFF) << 2);
                    RS(0x1A, (u32)&mat);
                    EXC();
                    RS(0x60, 0x80);
                } else if (i == 4) {
                    RS(0x60, 0);
                    MTW();
                    RS(0x1A, (u32)&mat);
                    w->x20 = (f32)(SFRM & 0x3FF) / 1024.0f;
                    UVT(0.0f, w->x20);
                    RS(0x19, (u32)&mat2);
                    EXC();
                    RS(0x60, 0x80);
                } else {
                    RS(0x60, 0x80);
                    MTW();
                    RS(0x1A, (u32)&mat);
                    EXC();
                }
                break;
            case 0x26:
                if (i == 2) {
                    RS(0x60, 0);
                    MT(22000.0f, 0.0f, 17000.0f);
                    SXA((FRM & 0x3FFF) << 2);
                    RS(0x1A, (u32)&mat);
                    EXC();
                    RS(0x60, 0x80);
                } else if (i == 5) {
                    RS(0x60, 0);
                    MTW();
                    RS(0x1A, (u32)&mat);
                    w->x20 = 1.0f - (f32)(SFRM & 0x1FF) / 512.0f;
                    UVT(0.0f, w->x20);
                    RS(0x19, (u32)&mat2);
                    EXC();
                    RS(0x60, 0x80);
                } else if (i == 6) {
                    RS(0x60, 0);
                    MTW();
                    RS(0x1A, (u32)&mat);
                    w->x20 = 1.0f - (f32)(SFRM & 0x3FF) / 1024.0f;
                    UVT(0.0f, w->x20);
                    RS(0x19, (u32)&mat2);
                    EXC();
                    RS(0x60, 0x80);
                } else {
                    RS(0x60, 0x80);
                    MTW();
                    RS(0x1A, (u32)&mat);
                    EXC();
                }
                break;
            case 0x1D:
            case 0x27:
                if (i == 2) {
                    RS(0x60, 0);
                    MT(10300.0f, 0.0f, 10900.0f);
                    SXA((FRM & 0x3FFF) << 2);
                    RS(0x1A, (u32)&mat);
                    EXC();
                    RS(0x60, 0x80);
                } else if (i == 3) {
                    RS(0x60, 0);
                    MTW();
                    RS(0x1A, (u32)&mat);
                    w->x20 = (f32)(SFRM & 0x1FF) / 512.0f;
                    UVT(0.0f, w->x20);
                    RS(0x19, (u32)&mat2);
                    EXC();
                    RS(0x60, 0x80);
                } else if (i == 4) {
                    RS(0x60, 0);
                    MTW();
                    RS(0x1A, (u32)&mat);
                    flmatMakeTrans(&mat2, 0.25f * (f32)(SFRM & 3), 0.25f * (f32)((SFRM >> 2) & 3), 0.0f);
                    RS(0x19, (u32)&mat2);
                    EXC();
                    RS(0x60, 0x80);
                } else {
                    RS(0x60, 0x80);
                    MTW();
                    RS(0x1A, (u32)&mat);
                    EXC();
                }
                break;
            case 0x28:
                if (i == 3) {
                    RS(0x60, 0);
                }
                EXC();
                RS(0x60, 0x80);
                /* fall through */
            case 0x3B:
                if (i == 4 || i == 8) {
                    RS(0x60, 0);
                    flmatMakeTrans(&mat2, 1.0f - (f32)(X1E & 0x7F) / 128.0f, 0.0f, 0.0f);
                    RS(0x19, (u32)&mat2);
                } else if (i == 5 || i == 7) {
                    RS(0x60, 0);
                } else if (i == 6) {
                    RS(0x60, 0);
                    flmatMakeTrans(&mat2, 1.0f - (f32)(X1E & 0x7F) / 128.0f, 0.0f, 0.0f);
                    RS(0x19, (u32)&mat2);
                }
                EXC();
                RS(0x60, 0x80);
                break;
            case 0x3C:
                if (i == 4 || i == 8) {
                    RS(0x60, 0);
                    flmatMakeTrans(&mat2, 1.0f - (f32)(X1E & 0x7F) / 128.0f, 0.0f, 0.0f);
                    RS(0x19, (u32)&mat2);
                } else if (i == 5 || i == 7) {
                    RS(0x60, 0);
                } else if (i == 6) {
                    RS(0x60, 0);
                    flmatMakeTrans(&mat2, 1.0f - (f32)(X1E & 0x7F) / 128.0f, 0.0f, 0.0f);
                    RS(0x19, (u32)&mat2);
                }
                EXC();
                RS(0x60, 0x80);
                break;
            case 0x3D:
                if (i == 1) {
                    RS(0x60, 0);
                    MT(16300.0f, 18900.0f, 1460.0f);
                    SXA((FRM & 0x3FFF) << 2);
                    RS(0x1A, (u32)&mat);
                } else if (i == 5) {
                    RS(0x60, 0);
                    MTW();
                    RS(0x1A, (u32)&mat);
                } else {
                    MTW();
                    RS(0x1A, (u32)&mat);
                }
                EXC();
                RS(0x60, 0x80);
                break;
            case 0x3E:
                if (i == 1) {
                    RS(0x60, 0x80);
                    MT(4090.0f, 0.0f, -2710.0f);
                    SXA((FRM & 0x3FFF) << 2);
                } else if (i == 2) {
                    RS(0x60, 0);
                    MT(4090.0f, 0.0f, -2710.0f);
                    flmatMakeTrans(&mat2, 0.0f, 1.0f - (f32)(int)game_w.x1E / 256.0f, 0.0f);
                    RS(0x19, (u32)&mat2);
                } else if (i == 5) {
                    RS(0x60, 0);
                    MTW();
                } else if (i == 6) {
                    RS(0x60, 0);
                    MTW();
                    flmatMakeTrans(&mat2, 1.0f - (f32)(X1E & 0x1F) / 32.0f, 0.0f, 0.0f);
                    RS(0x19, (u32)&mat2);
                } else {
                    RS(0x60, 0x80);
                    MTW();
                }
                RS(0x1A, (u32)&mat);
                EXC();
                break;
            case 0x40:
                if (i == 1) {
                    RS(0x60, 0);
                    flmatMakeTrans(&mat2, (f32)(X1E & 0x1FF) / 512.0f, 0.0f, 0.0f);
                    RS(0x19, (u32)&mat2);
                } else {
                    RS(0x60, 0x80);
                }
                RS(0x1A, (u32)&mat);
                EXC();
                break;
            case 0x41:
                if (i == 1) {
                    RS(0x60, 0);
                    flmatMakeTrans(&mat2, (f32)(X1E & 0x3FF) / 1024.0f, 0.0f, 0.0f);
                    RS(0x19, (u32)&mat2);
                } else if (i == 3) {
                    RS(0x60, 0);
                    flmatMakeTrans(&mat2, (f32)(X1E & 0x1FF) / 512.0f, 0.0f, 0.0f);
                    RS(0x19, (u32)&mat2);
                } else if (i == 6) {
                    RS(0x60, 0x80);
                    flmatMakeTrans(&mat2, 0.0f, (f32)(int)game_w.x1E / 256.0f, 0.0f);
                    RS(0x19, (u32)&mat2);
                } else {
                    RS(0x60, 0x80);
                }
                RS(0x1A, (u32)&mat);
                EXC();
                break;
            case 0x42:
                if (i == 1) {
                    RS(0x60, 0);
                    flmatMakeTrans(&mat2, (f32)(X1E & 0x3FF) / 1024.0f, 0.0f, 0.0f);
                    RS(0x19, (u32)&mat2);
                } else if (i == 3) {
                    RS(0x60, 0);
                    flmatMakeTrans(&mat2, 0.0f, 1.0f - (f32)(X1E & 0x1FF) / 512.0f, 0.0f);
                    RS(0x19, (u32)&mat2);
                } else if (i == 4) {
                    RS(0x60, 0);
                    flmatMakeTrans(&mat2, (f32)(X1E & 0x1FF) / 512.0f, 0.0f, 0.0f);
                    RS(0x19, (u32)&mat2);
                } else if (i == 7) {
                    RS(0x60, 0x80);
                    flmatMakeTrans(&mat2, 0.0f, (f32)(int)game_w.x1E / 256.0f, 0.0f);
                    RS(0x19, (u32)&mat2);
                } else {
                    RS(0x60, 0x80);
                }
                RS(0x1A, (u32)&mat);
                EXC();
                break;
            case 0x46:
                if (i == 2) {
                    RS(0x60, 0);
                }
                EXC();
                RS(0x60, 0x80);
                break;
            case 0x47:
            case 0x48:
            case 0x4A:
                if (i == 1) {
                    RS(0x60, 0);
                    MT(10000.0f, 0.0f, 10000.0f);
                    SXA((FRM & 0x7FF) << 5);
                    RS(0x1A, (u32)&mat);
                } else {
                    RS(0x60, 0x80);
                    MTW();
                    RS(0x1A, (u32)&mat);
                }
                EXC();
                break;
            case 0x49:
            case 0x4B:
                if (i == 3) {
                    break;
                }
                if (i == 4) {
                    RS(0x60, 0);
                }
                EXC();
                RS(0x60, 0x80);
                break;
            case 0x4C:
                if (i == 3) {
                    RS(0x60, 0);
                } else {
                    RS(0x60, 0x80);
                }
                EXC();
                RS(0x60, 0x80);
                break;
            case 0x4D:
                if (i == 2) {
                    RS(0x60, 0);
                    EXC();
                    RS(0x60, 0x80);
                } else if (i == 4) {
                    RS(0x60, 0);
                    flmatMakeTrans(&mat2, 0.0f, 0.5f * (f32)((X1E >> 1) & 1), 0.0f);
                    RS(0x19, (u32)&mat2);
                    EXC();
                    RS(0x60, 0x80);
                } else {
                    EXC();
                }
                break;
            case 0x4E:
            case 0x4F:
            case 0x57:
                if (i == 2) {
                    RS(0x60, 0);
                    EXC();
                    RS(0x60, 0x80);
                } else {
                    EXC();
                }
                break;
            case 0x55:
                if (i == 1) {
                    RS(0x60, 0);
                    flmatMakeTrans(&mat2, (f32)(X1E & 0x1FF) / 512.0f, 0.0f, 0.0f);
                    RS(0x19, (u32)&mat2);
                    EXC();
                    RS(0x60, 0x80);
                } else if (i == 1) {
                    RS(0x60, 0);
                    EXC();
                    RS(0x60, 0x80);
                } else {
                    EXC();
                }
                break;
            default:
                EXC();
                break;
            }
        }
    }
    switch (game_w.stage) {
    case 0:
    case 0x1A:
        p = (f32 *)st00_pos_tbl;
        mdl = &m->clay[3];
        for (k = 0; k < 4; k++, p += 3) {
            MTP(p);
            RS(0x1A, (u32)&mat);
            EXC();
        }
        break;
    case 4:
        mdl = &set_mdlw->clay[4];
        MTP(set04_pos_tbl);
        flmatRotY33(&mat, ANG((FRM & 0x3FF) << 6));
        RS(0x1A, (u32)&mat);
        EXC();
        break;
    case 5:
        mdl = &set_mdlw->clay[0];
        RS(0x60, 0);
        w->x20 = 0.02f + 0.02f * flSin(2.0f * (3.1415927f * ((f32)(FRM % 360) / 360.0f)));
        UVT(0.0f, w->x20);
        RS(0x19, (u32)&mat2);
        flmatInit(&mat);
        trans_stage_sub((int)&mat, (u8 *)mdl);
        RS(0x60, 0x80);
        break;
    case 9:
        m = set_mdlw;
        p = (f32 *)set09_pos_tbl;
        for (k = 0; k < 14; k++, p += 4) {
            if (k < 5) {
                mdl = &m->clay[0];
            } else if (k < 8) {
                mdl = &m->clay[1];
            } else {
                mdl = &m->clay[2];
            }
            MTP(p);
            flmatRotY33(&mat, p[3]);
            trans_stage_sub((int)&mat, (u8 *)mdl);
        }
        break;
    case 0x14:
        m = set_mdlw;
        p = (f32 *)set20_pos_tbl;
        for (k = 0; k < 2; k++, p += 3) {
            mdl = &m->clay[k];
            MTP(p);
            trans_stage_sub((int)&mat, (u8 *)mdl);
        }
        break;
    case 0x1C:
        p = set28_pos_tbl;
        mdl = &set_mdlw->clay[0];
        MTP(p);
        flmatRotY33(&mat, p[3]);
        trans_stage_sub((int)&mat, (u8 *)mdl);
        break;
    case 0x1B:
    case 0x21:
        m = set_mdlw;
        p = (f32 *)set33_pos_tbl;
        for (k = 0; k < 9; k++, p += 4) {
            switch (k) {
            case 0:
            case 1:
            case 2:
                mdl = &m->clay[4];
                break;
            case 3:
            case 4:
            case 5:
            case 6:
                mdl = &m->clay[5];
                break;
            case 7:
            case 8:
                mdl = &m->clay[7];
                break;
            }
            MTP(p);
            flmatRotY33(&mat, p[3]);
            trans_stage_sub((int)&mat, (u8 *)mdl);
        }
        break;
    case 0x18:
    case 0x22:
        m = set_mdlw;
        p = (f32 *)set34_pos_tbl;
        for (k = 0; k < 9; k++, p += 4) {
            switch (k) {
            case 0:
            case 1:
                mdl = &m->clay[3];
                break;
            case 2:
            case 3:
                mdl = &m->clay[4];
                break;
            case 4:
            case 5:
            case 6:
                mdl = &m->clay[8];
                break;
            case 7:
            case 8:
                mdl = &m->clay[9];
                break;
            }
            MTP(p);
            flmatRotY33(&mat, p[3]);
            trans_stage_sub((int)&mat, (u8 *)mdl);
        }
        break;
    case 0x24:
        m = set_mdlw;
        p = (f32 *)set36_pos_tbl;
        for (k = 0; k < 2; k++, p += 4) {
            mdl = &m->clay[9];
            MTP(p);
            flmatRotY33(&mat, p[3]);
            trans_stage_sub((int)&mat, (u8 *)mdl);
        }
        break;
    case 0x26:
        m = set_mdlw;
        p = (f32 *)set38_pos_tbl;
        for (k = 0; k < 11; k++, p += 4) {
            if (k < 3) {
                mdl = &m->clay[4];
            } else if (k < 9) {
                mdl = &m->clay[5];
            } else {
                mdl = &m->clay[7];
            }
            MTP(p);
            flmatRotY33(&mat, p[3]);
            trans_stage_sub((int)&mat, (u8 *)mdl);
        }
        break;
    case 0x1D:
    case 0x27:
        m = set_mdlw;
        p = (f32 *)set39_pos_tbl;
        for (k = 0; k < 2; k++, p += 3) {
            mdl = &m->clay[1 + k];
            MTP(p);
            if (k == 0) {
                flmatRotY33(&mat, ANG((FRM & 0x3FF) << 6));
            } else if (k == 1) {
                if (SFRM & 0x20) {
                    flmatMakeTrans(&mat2, 0.25f * (f32)(SFRM & 3), 0.125f * (f32)((SFRM >> 2) & 7), 0.0f);
                } else {
                    flmatMakeTrans(&mat2, 0.75f - 0.25f * (f32)(SFRM & 3), 0.875f - 0.125f * (f32)((SFRM >> 2) & 7), 0.0f);
                }
                RS(0x19, (u32)&mat2);
            }
            trans_stage_sub((int)&mat, (u8 *)mdl);
        }
        break;
    case 0x2B:
        p = (f32 *)set43_pos_tbl;
        mdl = &set_mdlw->clay[1];
        for (k = 0; k < 2; k++, p += 4) {
            if (k == 0) {
                flmatMakeScale(&mat, 0.6f, 0.6f, 0.6f);
                flmatSetTrans(&mat, p[0], p[1], p[2]);
            } else {
                MTP(p);
            }
            flmatRotY33(&mat, p[3]);
            trans_stage_sub((int)&mat, (u8 *)mdl);
        }
        break;
    case 0x2D:
        p = (f32 *)set45_pos_tbl;
        mdl = &set_mdlw->clay[3];
        for (k = 0; k < 4; k++, p += 4) {
            MTP(p);
            flmatRotY33(&mat, p[3]);
            trans_stage_sub((int)&mat, (u8 *)mdl);
        }
        break;
    case 0x32:
        mdl = &set_mdlw->clay[1];
        RS(0x60, 0);
        flmatMakeTrans(&mat2, (f32)(X1E & 0x3F) / 64.0f, (f32)(X1E & 0x3F) / 64.0f, 0.0f);
        RS(0x19, (u32)&mat2);
        MTP(set50_pos_tbl);
        trans_stage_sub((int)&mat, (u8 *)mdl);
        RS(0x60, 0x80);
        break;
    case 0x36:
        mdl = &set_mdlw->clay[5];
        flmatMakeTrans(&mat2, 0.0f, 1.0f - (f32)(X1E & 0x3F) / 64.0f, 0.0f);
        RS(0x19, (u32)&mat2);
        MTP(set54_pos_tbl);
        trans_stage_sub((int)&mat, (u8 *)mdl);
        break;
    case 0x3A:
        p = (f32 *)set58_pos_tbl;
        mdl = &set_mdlw->clay[0];
        for (k = 0; k < 6; k++, p += 4) {
            MTP(p);
            flmatRotY33(&mat, p[3]);
            trans_stage_sub((int)&mat, (u8 *)mdl);
        }
        break;
    case 0x3E:
        m = set_mdlw;
        mdl = &m->clay[0];
        MT(10900.0f, 0.0f, -6560.0f);
        flmatRotY33(&mat, ANG((X1E & 0x7F) << 9));
        flmatMakeTrans(&mat2, 1.0f - (f32)(X1E & 0x1F) / 32.0f, 0.0f, 0.0f);
        RS(0x19, (u32)&mat2);
        trans_stage_sub((int)&mat, (u8 *)mdl);
        p = (f32 *)set62_pos_tbl;
        for (k = 0; k < 3; k++, p += 4) {
            mdl = &m->clay[1];
            MTP(p);
            flmatRotX33(&mat, p[3]);
            trans_stage_sub((int)&mat, (u8 *)mdl);
        }
        RS(0x60, 0);
        mdl = &m->clay[2];
        MT(10900.0f, 0.0f, -6560.0f);
        flmatMakeTrans(&mat2, 1.0f - (f32)(X1E & 0xF) / 16.0f, 0.0f, 0.0f);
        RS(0x19, (u32)&mat2);
        trans_stage_sub((int)&mat, (u8 *)mdl);
        RS(0x60, 0x80);
        mdl = &m->clay[4];
        MT(11000.0f, -560.0f, 9500.0f);
        flmatMakeTrans(&mat2, 0.0f, (f32)(X1E & 0x7F) / 128.0f, 0.0f);
        RS(0x19, (u32)&mat2);
        trans_stage_sub((int)&mat, (u8 *)mdl);
        break;
    case 0x3F:
        m = set_mdlw;
        p = (f32 *)set63_pos_tbl;
        for (k = 0; k < 5; k++, p += 4) {
            mdl = &m->clay[0];
            MTP(p);
            flmatRotY33(&mat, p[3]);
            trans_stage_sub((int)&mat, (u8 *)mdl);
        }
        break;
    case 0x40:
        m = set_mdlw;
        p = (f32 *)set64_pos_tbl;
        for (k = 0; k < 3; k++, p += 4) {
            mdl = &m->clay[0];
            MTP(p);
            flmatRotY33(&mat, p[3]);
            trans_stage_sub((int)&mat, (u8 *)mdl);
        }
        break;
    case 0x47:
        m = set_mdlw;
        p = (f32 *)set71_pos_tbl;
        for (k = 0; k < 11; k++, p += 4) {
            if (k < 3) {
                mdl = &m->clay[0];
            } else if (k < 5) {
                mdl = &m->clay[1];
            } else {
                mdl = &m->clay[2];
            }
            MTP(p);
            flmatRotY33(&mat, p[3]);
            trans_stage_sub((int)&mat, (u8 *)mdl);
        }
        break;
    case 0x48:
    case 0x4A:
        m = set_mdlw;
        p = (f32 *)set72_pos_tbl;
        for (k = 0; k < 6; k++, p += 4) {
            if (k < 2) {
                mdl = &m->clay[0];
            } else if (k < 4) {
                mdl = &m->clay[1];
            } else {
                mdl = &m->clay[2];
            }
            MTP(p);
            flmatRotY33(&mat, p[3]);
            trans_stage_sub((int)&mat, (u8 *)mdl);
        }
        break;
    case 0x49:
        p = (f32 *)set73_pos_tbl;
        mdl = &set_mdlw->clay[0];
        for (k = 0; k < 2; k++, p += 4) {
            MTP(p);
            flmatRotY33(&mat, p[3]);
            flmatMakeTrans(&mat2, 0.0f, 1.0f - (f32)(X1E & 0x3F) / 64.0f, 0.0f);
            RS(0x19, (u32)&mat2);
            trans_stage_sub((int)&mat, (u8 *)mdl);
        }
        break;
    case 0x4B:
        p = (f32 *)set75_pos_tbl;
        mdl = &set_mdlw->clay[0];
        for (k = 0; k < 2; k++, p += 4) {
            MTP(p);
            flmatRotY33(&mat, p[3]);
            flmatMakeTrans(&mat2, 0.0f, 1.0f - (f32)(X1E & 0x3F) / 64.0f, 0.0f);
            RS(0x19, (u32)&mat2);
            trans_stage_sub((int)&mat, (u8 *)mdl);
        }
        break;
    case 0x4F:
        mdl = &set_mdlw->clay[0];
        RS(0x60, 0);
        flmatInit(&mat);
        u = (u32)(255.0f * (0.5f + 0.5f * flSin(ANG((X1E & 0x1F) << 11))));
        RS(0x67, (u << 24) | 0xFFFFFF);
        trans_stage_sub((int)&mat, (u8 *)mdl);
        RS(0x67, -1);
        RS(0x60, 0x80);
        break;
    }
    clay_attr_reset();
    RS(0x60, 0);
    }
    }
    }
}
