/* trans_stage_nm - NOT BUILT for the PS2 (no matching attempted yet).
 * C for trans_stage (SLPM_654.95 0x0015CD90, 0x3B00 bytes, f_stage range)
 * and trans_stage_sub, written from an m2c draft checked call by call
 * against the asm (every flmat call's float arguments, which m2c lost,
 * were read from the asm). Believed equivalent; used by the PC port.
 *
 * trans_stage draws the stage: the area model (stage_work.mdl, one clay
 * per AMO part) with world matrix Trans(stage_work.pos) * Rxyz(stage_work.rot),
 * except for per-stage parts that are placed, spun or UV-scrolled here:
 * part 0 is the sky (on some stages centred on a fixed point and slowly
 * turned), and many stages draw a part at a fixed position (e.g. st05
 * part 2 twice at set05_pos_tbl1, part 3 at 10000,0,8500). Then it draws
 * set-model clays (set_mdlw) at the set??_pos_tbl positions.
 * Angles: stage_work.timer (+0x08) and game_w+0x1E (a per-tick counter) drive
 * the sky spin and the UV scrolls (texture matrix, state 0x19). */
#include "types.h"
#include "game.h"
#include "fl.h"
#include "clay.h"

typedef struct STAGE_MDLW {
    u8 flag;            /* 0x00 */
    u8 _pad01[0x2B];
    s16 nclay;          /* 0x2C */
    u8 _pad2E[2];
    CLAY *clay;         /* 0x30 */
} STAGE_MDLW;

typedef struct STAGE_W {
    u8 flag;            /* 0x00 */
    u8 x01;             /* 0x01 */
    u8 _pad02[6];
    u16 timer;          /* 0x08 */
    u8 _pad0A[6];
    f32 pos[3];         /* 0x10 */
    f32 u;              /* 0x1C UV scroll */
    f32 v;              /* 0x20 */
    f32 x24;            /* 0x24 */
    f32 rot[3];         /* 0x28 */
    u8 _pad34[8];
    STAGE_MDLW *mdl;    /* 0x3C area model */
    u8 _pad40[0x24];
} STAGE_W;

extern STAGE_W stage_work;
extern STAGE_MDLW *set_mdlw;
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

void flSetRenderState(int, u32);
void flExecuteClay(s32, int);
void clay_attr_set(s32);
void clay_attr_reset(void);
void flmatInit(FLMAT *);
void flmatMakeTrans(FLMAT *, f32, f32, f32);
void flmatMakeScale(FLMAT *, f32, f32, f32);
void flmatSetTrans(FLMAT *, f32, f32, f32);
void flmatSetXYZ33(FLMAT *, f32, f32, f32);
void flmatRotX33(FLMAT *, f32);
void flmatRotY33(FLMAT *, f32);
f32 flSin(f32);

#define GW_CNT (*(u16 *)((u8 *)&game_w + 0x1E))

/* the asm's angle: 2 pi * ((360 * (f32)x / 65536) / 360) */
static f32 rad16(s32 x)
{
    return 2.0f * (3.1415927f * (((360.0f * (f32)x) / 65536.0f) / 360.0f));
}

void trans_stage_sub(FLMAT *m, CLAY *cl)
{
    flSetRenderState(0x1A, (u32)m);
    clay_attr_set(cl->attr);
    flExecuteClay(cl->handle, 0);
}

static void base_mat(FLMAT *m)
{
    flmatMakeTrans(m, stage_work.pos[0], stage_work.pos[1], stage_work.pos[2]);
}

static void base_mat_rot(FLMAT *m)
{
    base_mat(m);
    flmatSetXYZ33(m, stage_work.rot[0], stage_work.rot[1], stage_work.rot[2]);
}

/* a part centred on (x, y, z), spun about Y by (timer & mask) << sh */
static void spin_mat(FLMAT *m, f32 x, f32 y, f32 z, int mask, int sh)
{
    flmatMakeTrans(m, x, y, z);
    flmatSetXYZ33(m, 0.0f, rad16((stage_work.timer & mask) << sh), 0.0f);
}

static void uv_mat(FLMAT *uv, f32 u, f32 v)
{
    flmatMakeTrans(uv, u, v, 0.0f);
    flSetRenderState(0x19, (u32)uv);
}

/* the sky (part 0) */
static void trans_sky(CLAY *cl, FLMAT *m)
{
    f32 x, z;
    int mask, sh;

    flSetRenderState(0x6D, 7);
    switch (game_w.stage) {
    case 0x19:
        x = 13000.0f; z = 20800.0f; mask = 0xFFF; sh = 4;
        break;
    case 0x3A:
    case 0x40:
        x = 10000.0f; z = 10000.0f; mask = 0x1FFF; sh = 3;
        break;
    case 0x41:
        x = 11500.0f; z = 3000.0f; mask = 0x1FFF; sh = 3;
        break;
    case 0x42:
        x = 12000.0f; z = 8000.0f; mask = 0x1FFF; sh = 3;
        break;
    case 0x47: case 0x48: case 0x49: case 0x4A: case 0x4B:
        flmatMakeTrans(m, 10000.0f, 0.0f, 10000.0f);
        flSetRenderState(0x1A, (u32)m);
        flExecuteClay(cl->handle, 0);
        base_mat_rot(m);
        flSetRenderState(0x1A, (u32)m);
        flSetRenderState(0x6D, 3);
        return;
    default:
        flExecuteClay(cl->handle, 0);
        flSetRenderState(0x6D, 3);
        return;
    }
    spin_mat(m, x, 0.0f, z, mask, sh);
    flSetRenderState(0x1A, (u32)m);
    flExecuteClay(cl->handle, 0);
    base_mat_rot(m);
    flSetRenderState(0x1A, (u32)m);
    flSetRenderState(0x6D, 3);
}

/* parts 1.. of the area model */
static void trans_part(CLAY *cl, int i, FLMAT *m, FLMAT *uv)
{
    s16 t = (s16)stage_work.timer;
    int k;

    switch (game_w.stage) {
    case 0:
    case 0x1A:
        if (i == 1)
            flExecuteClay(cl->handle, 0);
        break;
    case 4:
        if (i == 2) {
            flSetRenderState(0x60, 0);
            spin_mat(m, 13200.0f, 0.0f, 5190.0f, 0x3FFF, 2);
        } else if (i == 5) {
            flSetRenderState(0x60, 0);
            base_mat(m);
        } else {
            flSetRenderState(0x60, 0x80);
            base_mat(m);
        }
        flSetRenderState(0x1A, (u32)m);
        flExecuteClay(cl->handle, 0);
        break;
    case 5:
        if (i == 2) {
            for (k = 0; k < 2; k++) {
                flmatMakeTrans(m, set05_pos_tbl1[k][0], set05_pos_tbl1[k][1], set05_pos_tbl1[k][2]);
                flSetRenderState(0x1A, (u32)m);
                flExecuteClay(cl->handle, 0);
            }
        }
        if (i == 3) {
            flmatMakeTrans(m, 10000.0f, 0.0f, 8500.0f);
            flSetRenderState(0x1A, (u32)m);
        }
        flExecuteClay(cl->handle, 0);
        break;
    case 6:
    case 7:
        if (i != 2)
            flExecuteClay(cl->handle, 0);
        break;
    case 0x15: case 0x18: case 0x22: case 0x23: case 0x24: case 0x29: case 0x2A: case 0x2B:
        if (i == 1) {
            stage_work.v = 0.02f + 0.02f * flSin(2.0f * (3.1415927f * ((f32)(stage_work.timer % 360) / 360.0f)));
            uv_mat(uv, 0.0f, stage_work.v);
        }
        flExecuteClay(cl->handle, 0);
        break;
    case 9:
        if (i == 1) {
            flSetRenderState(0x60, 0);
            spin_mat(m, 10000.0f, 0.0f, 10000.0f, 0xFFF, 4);
        } else if (i == 4) {
            flSetRenderState(0x60, 0);
            base_mat(m);
        } else {
            flSetRenderState(0x60, 0x80);
            base_mat(m);
        }
        flSetRenderState(0x1A, (u32)m);
        flExecuteClay(cl->handle, 0);
        break;
    case 0xE:
        if (i == 2) {
            flSetRenderState(0x60, 0);
            spin_mat(m, 11800.0f, 0.0f, 12000.0f, 0xFFF, 4);
        } else if (i == 4) {
            if (game_w.info_stop == 1)
                break;
            flSetRenderState(0x60, 0x80);
            spin_mat(m, 11100.0f, 0.0f, 14160.0f, 0x7FF, 5);
        } else {
            flSetRenderState(0x60, 0x80);
            base_mat(m);
        }
        flSetRenderState(0x1A, (u32)m);
        flExecuteClay(cl->handle, 0);
        break;
    case 0x12:
        if (i == 6) {
            flSetRenderState(0x60, 0);
            stage_work.v = (f32)(t & 0x3F) / 64.0f;
            uv_mat(uv, 0.0f, stage_work.v);
            flExecuteClay(cl->handle, 0);
            flSetRenderState(0x60, 0x80);
        } else {
            base_mat(m);
            flSetRenderState(0x1A, (u32)m);
            flExecuteClay(cl->handle, 0);
        }
        break;
    case 0x13:
        if (i == 2) {
            flSetRenderState(0x60, 0);
            spin_mat(m, 9720.0f, 0.0f, 9300.0f, 0x3FFF, 2);
            flSetRenderState(0x1A, (u32)m);
        } else if (i == 7) {
            flSetRenderState(0x60, 0);
            base_mat(m);
            stage_work.u = (f32)(u8)stage_work.timer / 256.0f;
            uv_mat(uv, stage_work.u, 0.0f);
            flSetRenderState(0x1A, (u32)m);
        } else {
            base_mat(m);
            flSetRenderState(0x1A, (u32)m);
        }
        flExecuteClay(cl->handle, 0);
        flSetRenderState(0x60, 0x80);
        break;
    case 0x16:
    case 0x17:
        if (i == 6) {
            flSetRenderState(0x60, 0);
            stage_work.v = 1.0f - (f32)(t & 0x3F) / 64.0f;
            uv_mat(uv, 0.0f, stage_work.v);
            flExecuteClay(cl->handle, 0);
            flSetRenderState(0x60, 0x80);
        } else {
            base_mat(m);
            flSetRenderState(0x1A, (u32)m);
            flExecuteClay(cl->handle, 0);
        }
        break;
    case 0x19:
        if (i == 1) {
            flSetRenderState(0x60, 0);
            stage_work.v = 1.0f - (f32)(t & 0x7F) / 128.0f;
            uv_mat(uv, 0.0f, stage_work.v);
            flmatSetXYZ33(m, 0.0f, rad16((stage_work.timer & 0x7FF) << 5), 0.0f);
            flmatSetTrans(m, 13200.0f, 0.0f, 21000.0f);
            flSetRenderState(0x1A, (u32)m);
            flExecuteClay(cl->handle, 0);
            flSetRenderState(0x60, 0x80);
        } else {
            flSetRenderState(0x60, 0x80);
            base_mat(m);
            flSetRenderState(0x1A, (u32)m);
            flExecuteClay(cl->handle, 0);
        }
        break;
    case 0x1F:
        if (i == 2) {
            flSetRenderState(0x60, 0);
            spin_mat(m, 10300.0f, 0.0f, 12800.0f, 0xFFF, 4);
        } else if (i == 4) {
            flSetRenderState(0x60, 0);
            spin_mat(m, 16600.0f, 0.0f, 6200.0f, 0x7FF, 5);
        } else {
            flSetRenderState(0x60, 0x80);
            base_mat(m);
        }
        flSetRenderState(0x1A, (u32)m);
        flExecuteClay(cl->handle, 0);
        break;
    case 0x20:
        if (i == 1) {
            flSetRenderState(0x60, 0);
            flmatInit(m);
            flmatRotY33(m, rad16((stage_work.timer & 0x3FFF) << 2));
            flSetRenderState(0x1A, (u32)m);
            flExecuteClay(cl->handle, 0);
            flSetRenderState(0x60, 0x80);
        } else if (i == 7) {
            flSetRenderState(0x60, 0);
            flSetRenderState(0x6C, 0);
            base_mat(m);
            flSetRenderState(0x1A, (u32)m);
            flExecuteClay(cl->handle, 0);
            flSetRenderState(0x60, 0x80);
            flSetRenderState(0x6C, 1);
        } else {
            base_mat(m);
            flSetRenderState(0x1A, (u32)m);
            flExecuteClay(cl->handle, 0);
        }
        break;
    case 0x1B:
    case 0x21:
        if (i == 2) {
            flSetRenderState(0x60, 0);
            spin_mat(m, 13000.0f, 0.0f, 11500.0f, 0x3FFF, 2);
        } else {
            flSetRenderState(0x60, 0x80);
            base_mat(m);
        }
        flSetRenderState(0x1A, (u32)m);
        flExecuteClay(cl->handle, 0);
        break;
    case 8:
    case 0xF:
    case 0x25:
        if (i == 2) {
            flSetRenderState(0x60, 0);
            spin_mat(m, 10000.0f, 0.0f, 10000.0f, 0x3FFF, 2);
            flSetRenderState(0x1A, (u32)m);
            flExecuteClay(cl->handle, 0);
            flSetRenderState(0x60, 0x80);
        } else if (i == 4) {
            flSetRenderState(0x60, 0);
            base_mat(m);
            flSetRenderState(0x1A, (u32)m);
            stage_work.v = (f32)(t & 0x3FF) / 1024.0f;
            uv_mat(uv, 0.0f, stage_work.v);
            flExecuteClay(cl->handle, 0);
            flSetRenderState(0x60, 0x80);
        } else {
            flSetRenderState(0x60, 0x80);
            base_mat(m);
            flSetRenderState(0x1A, (u32)m);
            flExecuteClay(cl->handle, 0);
        }
        break;
    case 0x26:
        if (i == 2) {
            flSetRenderState(0x60, 0);
            spin_mat(m, 22000.0f, 0.0f, 17000.0f, 0x3FFF, 2);
            flSetRenderState(0x1A, (u32)m);
            flExecuteClay(cl->handle, 0);
            flSetRenderState(0x60, 0x80);
        } else if (i == 5 || i == 6) {
            flSetRenderState(0x60, 0);
            base_mat(m);
            flSetRenderState(0x1A, (u32)m);
            if (i == 5)
                stage_work.v = 1.0f - (f32)(t & 0x1FF) / 512.0f;
            else
                stage_work.v = 1.0f - (f32)(t & 0x3FF) / 1024.0f;
            uv_mat(uv, 0.0f, stage_work.v);
            flExecuteClay(cl->handle, 0);
            flSetRenderState(0x60, 0x80);
        } else {
            flSetRenderState(0x60, 0x80);
            base_mat(m);
            flSetRenderState(0x1A, (u32)m);
            flExecuteClay(cl->handle, 0);
        }
        break;
    case 0x1D:
    case 0x27:
        if (i == 2) {
            flSetRenderState(0x60, 0);
            spin_mat(m, 10300.0f, 0.0f, 10900.0f, 0x3FFF, 2);
            flSetRenderState(0x1A, (u32)m);
            flExecuteClay(cl->handle, 0);
            flSetRenderState(0x60, 0x80);
        } else if (i == 3) {
            flSetRenderState(0x60, 0);
            base_mat(m);
            flSetRenderState(0x1A, (u32)m);
            stage_work.v = (f32)(t & 0x1FF) / 512.0f;
            uv_mat(uv, 0.0f, stage_work.v);
            flExecuteClay(cl->handle, 0);
            flSetRenderState(0x60, 0x80);
        } else if (i == 4) {
            flSetRenderState(0x60, 0);
            base_mat(m);
            flSetRenderState(0x1A, (u32)m);
            uv_mat(uv, 0.25f * (f32)(t & 3), 0.25f * (f32)((t >> 2) & 3));
            flExecuteClay(cl->handle, 0);
            flSetRenderState(0x60, 0x80);
        } else {
            flSetRenderState(0x60, 0x80);
            base_mat(m);
            flSetRenderState(0x1A, (u32)m);
            flExecuteClay(cl->handle, 0);
        }
        break;
    case 0x28:
        if (i == 3)
            flSetRenderState(0x60, 0);
        flExecuteClay(cl->handle, 0);
        flSetRenderState(0x60, 0x80);
        break;
    case 0x3B:
    case 0x3C:
        switch (i) {
        case 4:
        case 6:
        case 8:
            flSetRenderState(0x60, 0);
            uv_mat(uv, 1.0f - (f32)(GW_CNT & 0x7F) / 128.0f, 0.0f);
            break;
        case 5:
        case 7:
            flSetRenderState(0x60, 0);
            break;
        }
        flExecuteClay(cl->handle, 0);
        flSetRenderState(0x60, 0x80);
        break;
    case 0x3D:
        if (i == 1) {
            flSetRenderState(0x60, 0);
            spin_mat(m, 16300.0f, 18900.0f, 1460.0f, 0x3FFF, 2);
            flSetRenderState(0x1A, (u32)m);
        } else if (i == 5) {
            flSetRenderState(0x60, 0);
            base_mat(m);
            flSetRenderState(0x1A, (u32)m);
        } else {
            base_mat(m);
            flSetRenderState(0x1A, (u32)m);
        }
        flExecuteClay(cl->handle, 0);
        flSetRenderState(0x60, 0x80);
        break;
    case 0x3E:
        switch (i) {
        case 1:
            flSetRenderState(0x60, 0x80);
            spin_mat(m, 4090.0f, 0.0f, -2710.0f, 0x3FFF, 2);
            break;
        case 2:
            flSetRenderState(0x60, 0);
            flmatMakeTrans(m, 4090.0f, 0.0f, -2710.0f);
            uv_mat(uv, 0.0f, 1.0f - (f32)(u8)GW_CNT / 256.0f);
            break;
        case 5:
            flSetRenderState(0x60, 0);
            base_mat(m);
            break;
        case 6:
            flSetRenderState(0x60, 0);
            base_mat(m);
            uv_mat(uv, 1.0f - (f32)(GW_CNT & 0x1F) / 32.0f, 0.0f);
            break;
        default:
            flSetRenderState(0x60, 0x80);
            base_mat(m);
            break;
        }
        flSetRenderState(0x1A, (u32)m);
        flExecuteClay(cl->handle, 0);
        break;
    case 0x40:
        if (i == 1) {
            flSetRenderState(0x60, 0);
            uv_mat(uv, (f32)(GW_CNT & 0x1FF) / 512.0f, 0.0f);
        } else {
            flSetRenderState(0x60, 0x80);
        }
        flSetRenderState(0x1A, (u32)m);
        flExecuteClay(cl->handle, 0);
        break;
    case 0x41:
        switch (i) {
        case 1:
            flSetRenderState(0x60, 0);
            uv_mat(uv, (f32)(GW_CNT & 0x3FF) / 1024.0f, 0.0f);
            break;
        case 3:
            flSetRenderState(0x60, 0);
            uv_mat(uv, (f32)(GW_CNT & 0x1FF) / 512.0f, 0.0f);
            break;
        case 6:
            flSetRenderState(0x60, 0x80);
            uv_mat(uv, 0.0f, (f32)(u8)GW_CNT / 256.0f);
            break;
        default:
            flSetRenderState(0x60, 0x80);
            break;
        }
        flSetRenderState(0x1A, (u32)m);
        flExecuteClay(cl->handle, 0);
        break;
    case 0x42:
        switch (i) {
        case 1:
            flSetRenderState(0x60, 0);
            uv_mat(uv, (f32)(GW_CNT & 0x3FF) / 1024.0f, 0.0f);
            break;
        case 3:
            flSetRenderState(0x60, 0);
            uv_mat(uv, 0.0f, 1.0f - (f32)(GW_CNT & 0x1FF) / 512.0f);
            break;
        case 4:
            flSetRenderState(0x60, 0);
            uv_mat(uv, (f32)(GW_CNT & 0x1FF) / 512.0f, 0.0f);
            break;
        case 7:
            flSetRenderState(0x60, 0x80);
            uv_mat(uv, 0.0f, (f32)(u8)GW_CNT / 256.0f);
            break;
        default:
            flSetRenderState(0x60, 0x80);
            break;
        }
        flSetRenderState(0x1A, (u32)m);
        flExecuteClay(cl->handle, 0);
        break;
    case 0x46:
        if (i == 2)
            flSetRenderState(0x60, 0);
        flExecuteClay(cl->handle, 0);
        flSetRenderState(0x60, 0x80);
        break;
    case 0x47:
    case 0x48:
    case 0x4A:
        if (i == 1) {
            flSetRenderState(0x60, 0);
            spin_mat(m, 10000.0f, 0.0f, 10000.0f, 0x7FF, 5);
        } else {
            flSetRenderState(0x60, 0x80);
            base_mat(m);
        }
        flSetRenderState(0x1A, (u32)m);
        flExecuteClay(cl->handle, 0);
        break;
    case 0x49:
    case 0x4B:
        if (i != 3) {
            if (i == 4)
                flSetRenderState(0x60, 0);
            flExecuteClay(cl->handle, 0);
            flSetRenderState(0x60, 0x80);
        }
        break;
    case 0x4C:
        flSetRenderState(0x60, i == 3 ? 0 : 0x80);
        flExecuteClay(cl->handle, 0);
        flSetRenderState(0x60, 0x80);
        break;
    case 0x4D:
        if (i == 2) {
            flSetRenderState(0x60, 0);
            flExecuteClay(cl->handle, 0);
            flSetRenderState(0x60, 0x80);
        } else if (i == 4) {
            flSetRenderState(0x60, 0);
            uv_mat(uv, 0.0f, 0.5f * (f32)((GW_CNT >> 1) & 1));
            flExecuteClay(cl->handle, 0);
            flSetRenderState(0x60, 0x80);
        } else {
            flExecuteClay(cl->handle, 0);
        }
        break;
    case 0x4E:
    case 0x4F:
    case 0x57:
        if (i == 2) {
            flSetRenderState(0x60, 0);
            flExecuteClay(cl->handle, 0);
            flSetRenderState(0x60, 0x80);
        } else {
            flExecuteClay(cl->handle, 0);
        }
        break;
    case 0x55:
        if (i == 1) {
            flSetRenderState(0x60, 0);
            uv_mat(uv, (f32)(GW_CNT & 0x1FF) / 512.0f, 0.0f);
            flExecuteClay(cl->handle, 0);
            flSetRenderState(0x60, 0x80);
        } else {
            flExecuteClay(cl->handle, 0);   /* (the asm repeats the i == 1 test here) */
        }
        break;
    default:
        flExecuteClay(cl->handle, 0);
        break;
    }
}

/* set-model clays placed by the stage: table rows {x, y, z[, ang Y]} */
static void set_row(FLMAT *m, const f32 *p, CLAY *cl, int rot)
{
    flmatMakeTrans(m, p[0], p[1], p[2]);
    if (rot)
        flmatRotY33(m, p[3]);
    trans_stage_sub(m, cl);
}

static void trans_set(STAGE_MDLW *st, FLMAT *m, FLMAT *uv)
{
    CLAY *c = set_mdlw ? set_mdlw->clay : 0;
    int k;
    f32 a;

    if (game_w.stage != 0 && game_w.stage != 0x1A && !c)
        return;
    switch (game_w.stage) {
    case 0:
    case 0x1A:
        for (k = 0; k < 4; k++) {
            flmatMakeTrans(m, st00_pos_tbl[k][0], st00_pos_tbl[k][1], st00_pos_tbl[k][2]);
            flSetRenderState(0x1A, (u32)m);
            flExecuteClay(st->clay[3].handle, 0);
        }
        break;
    case 4:
        flmatMakeTrans(m, set04_pos_tbl[0], set04_pos_tbl[1], set04_pos_tbl[2]);
        flmatRotY33(m, rad16((stage_work.timer & 0x3FF) << 6));
        flSetRenderState(0x1A, (u32)m);
        flExecuteClay(c[4].handle, 0);
        break;
    case 5:
        flSetRenderState(0x60, 0);
        stage_work.v = 0.02f + 0.02f * flSin(2.0f * (3.1415927f * ((f32)(stage_work.timer % 360) / 360.0f)));
        uv_mat(uv, 0.0f, stage_work.v);
        flmatInit(m);
        trans_stage_sub(m, &c[0]);
        flSetRenderState(0x60, 0x80);
        break;
    case 9:
        for (k = 0; k < 14; k++)
            set_row(m, set09_pos_tbl[k], &c[k < 5 ? 0 : k < 8 ? 1 : 2], 1);
        break;
    case 0x14:
        for (k = 0; k < 2; k++)
            set_row(m, set20_pos_tbl[k], &c[k], 0);
        break;
    case 0x1C:
        set_row(m, set28_pos_tbl, &c[0], 1);
        break;
    case 0x1B:
    case 0x21:
        for (k = 0; k < 9; k++)
            set_row(m, set33_pos_tbl[k], &c[k < 3 ? 4 : k < 7 ? 5 : 7], 1);
        break;
    case 0x18:
    case 0x22:
        for (k = 0; k < 9; k++)
            set_row(m, set34_pos_tbl[k], &c[k < 2 ? 3 : k < 4 ? 4 : k < 7 ? 8 : 9], 1);
        break;
    case 0x24:
        for (k = 0; k < 2; k++)
            set_row(m, set36_pos_tbl[k], &c[9], 1);
        break;
    case 0x26:
        for (k = 0; k < 11; k++)
            set_row(m, set38_pos_tbl[k], &c[k < 3 ? 4 : k < 9 ? 5 : 7], 1);
        break;
    case 0x1D:
    case 0x27:
        for (k = 0; k < 2; k++) {
            s16 t = (s16)stage_work.timer;
            flmatMakeTrans(m, set39_pos_tbl[k][0], set39_pos_tbl[k][1], set39_pos_tbl[k][2]);
            if (k == 0) {
                flmatRotY33(m, rad16((stage_work.timer & 0x3FF) << 6));
            } else {
                if (t & 0x20)
                    uv_mat(uv, 0.25f * (f32)(t & 3), 0.125f * (f32)((t >> 2) & 7));
                else
                    uv_mat(uv, 0.75f - 0.25f * (f32)(t & 3), 0.875f - 0.125f * (f32)((t >> 2) & 7));
            }
            trans_stage_sub(m, &c[k + 1]);
        }
        break;
    case 0x2B:
        for (k = 0; k < 2; k++) {
            if (k == 0) {
                flmatMakeScale(m, 0.6f, 0.6f, 0.6f);
                flmatSetTrans(m, set43_pos_tbl[k][0], set43_pos_tbl[k][1], set43_pos_tbl[k][2]);
            } else {
                flmatMakeTrans(m, set43_pos_tbl[k][0], set43_pos_tbl[k][1], set43_pos_tbl[k][2]);
            }
            flmatRotY33(m, set43_pos_tbl[k][3]);
            trans_stage_sub(m, &c[1]);
        }
        break;
    case 0x2D:
        for (k = 0; k < 4; k++)
            set_row(m, set45_pos_tbl[k], &c[3], 1);
        break;
    case 0x32:
        flSetRenderState(0x60, 0);
        a = (f32)(GW_CNT & 0x3F) / 64.0f;
        uv_mat(uv, a, a);
        set_row(m, set50_pos_tbl, &c[1], 0);
        flSetRenderState(0x60, 0x80);
        break;
    case 0x36:
        uv_mat(uv, 0.0f, 1.0f - (f32)(GW_CNT & 0x3F) / 64.0f);
        set_row(m, set54_pos_tbl, &c[5], 0);
        break;
    case 0x3A:
        for (k = 0; k < 6; k++)
            set_row(m, set58_pos_tbl[k], &c[0], 1);
        break;
    case 0x3E:
        flmatMakeTrans(m, 10900.0f, 0.0f, -6560.0f);
        flmatRotY33(m, rad16((GW_CNT & 0x7F) << 9));
        uv_mat(uv, 1.0f - (f32)(GW_CNT & 0x1F) / 32.0f, 0.0f);
        trans_stage_sub(m, &c[0]);
        for (k = 0; k < 3; k++) {
            flmatMakeTrans(m, set62_pos_tbl[k][0], set62_pos_tbl[k][1], set62_pos_tbl[k][2]);
            flmatRotX33(m, set62_pos_tbl[k][3]);
            trans_stage_sub(m, &c[1]);
        }
        flSetRenderState(0x60, 0);
        flmatMakeTrans(m, 10900.0f, 0.0f, -6560.0f);
        uv_mat(uv, 1.0f - (f32)(GW_CNT & 0xF) / 16.0f, 0.0f);
        trans_stage_sub(m, &c[2]);
        flSetRenderState(0x60, 0x80);
        flmatMakeTrans(m, 11000.0f, -560.0f, 9500.0f);
        uv_mat(uv, 0.0f, (f32)(GW_CNT & 0x7F) / 128.0f);
        trans_stage_sub(m, &c[4]);
        break;
    case 0x3F:
        for (k = 0; k < 5; k++)
            set_row(m, set63_pos_tbl[k], &c[0], 1);
        break;
    case 0x40:
        for (k = 0; k < 3; k++)
            set_row(m, set64_pos_tbl[k], &c[0], 1);
        break;
    case 0x47:
        for (k = 0; k < 11; k++)
            set_row(m, set71_pos_tbl[k], &c[k < 3 ? 0 : k < 5 ? 1 : 2], 1);
        break;
    case 0x48:
    case 0x4A:
        for (k = 0; k < 6; k++)
            set_row(m, set72_pos_tbl[k], &c[k < 2 ? 0 : k < 4 ? 1 : 2], 1);
        break;
    case 0x49:
    case 0x4B:
        for (k = 0; k < 2; k++) {
            const f32 *p = game_w.stage == 0x49 ? set73_pos_tbl[k] : set75_pos_tbl[k];
            flmatMakeTrans(m, p[0], p[1], p[2]);
            flmatRotY33(m, p[3]);
            uv_mat(uv, 0.0f, 1.0f - (f32)(GW_CNT & 0x3F) / 64.0f);
            trans_stage_sub(m, &c[0]);
        }
        break;
    case 0x4F: {
        u32 al;
        flSetRenderState(0x60, 0);
        flmatInit(m);
        a = 255.0f * (0.5f + 0.5f * flSin(rad16((GW_CNT & 0x1F) << 11)));
        al = a >= 2147483648.0f ? (u32)(s32)(a - 2147483648.0f) | 0x80000000u : (u32)(s32)a;
        flSetRenderState(0x67, (al & 0xFF) << 24 | 0xFFFFFF);
        trans_stage_sub(m, &c[0]);
        flSetRenderState(0x67, (u32)-1);
        flSetRenderState(0x60, 0x80);
        break;
    }
    }
}

void trans_stage(void)
{
    FLMAT m, uv;
    STAGE_MDLW *st;
    CLAY *cl;
    s16 n;
    int i;

    if (stage_work.flag == 0 || stage_work.x01 == 0)
        return;
    /* light_set(0), get_tex_num / reload_tex(0x10, 0xEA): textures stay
     * resident in the port */
    st = stage_work.mdl;
    if (st == 0 || st->flag == 0)
        return;
    flSetRenderState(0x60, 0x80);
    flSetRenderState(0x67, (u32)-1);
    cl = st->clay;
    base_mat_rot(&m);
    flSetRenderState(0x1A, (u32)&m);
    n = st->nclay;
    if (game_w.stage == 0 || game_w.stage == 0x1A)
        n--;
    for (i = 0; i < n; i++, cl++) {
        clay_attr_set(cl->attr);
        if (i == 0)
            trans_sky(cl, &m);
        else
            trans_part(cl, i, &m, &uv);
    }
    trans_set(st, &m, &uv);
    clay_attr_reset();
    flSetRenderState(0x60, 0);
}
