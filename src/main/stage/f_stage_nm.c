/* Near-match / unfinished stage functions (not built): stage_se_move is
 * ~100 instructions off (register allocation), stage_m and move_stage were
 * written from the asm but never compiled or compared. */

#include "flow.h"

extern s16 flash_flag;
extern s16 flash_timer;
extern s16 Stg_env_type[];

#define PL8(p, o)  (*(u8 *)((u8 *)(p) + (o)))
#define PL16(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define PLF(p, o)  (*(f32 *)((u8 *)(p) + (o)))

/* Stage movement trigger zones (guess: stage change areas), 0x34 bytes each,
 * list ends with id 0xFFFF. kind 0 = circle on XZ, 1 = box. */
typedef struct STG_MV {
    u16 id;             /* 0x00 next stage number (copied to PLW+0x73A) */
    s16 kind;           /* 0x02 */
    f32 pos[3];         /* 0x04 */
    f32 r;              /* 0x10 radius */
    f32 h;              /* 0x14 height */
    f32 box[3];         /* 0x18 */
    f32 dest[3];        /* 0x24 position in the next stage */
    u16 ang;            /* 0x30 */
    u8 _pad32[2];
} STG_MV;

STG_MV *Stage_mv_data_get();
f32 flSqrt(f32);
int hit_point_cbd(f32, f32, f32 *, f32 *, f32 *);
void Pl_ofs_set();
void net_send_pl();
void se_req2();
extern f32 st01_se_pos[3][2];
extern f32 st03_se_pos[2][2];
extern f32 st03_se_pos2[2];
extern f32 st26_se_pos[6][2];
extern f32 st48_se_pos[2][2];
extern f32 st52_se_pos[3][2];
extern f32 st54_se_pos[6][2];
extern f32 st62_se_pos[4][2];
void *memset();
int Pl_master_ck();
void PilebunkerCameraRequest();
void stage_i();
void stage_m();
void flSetRenderState();
void clay_attr_set();
void flExecuteClay();

void stage_se_move();

/* Stage item/object list entry (0x18 bytes), list ends with pos[0] == -1.0f */
typedef struct STG_ITEM {
    f32 pos[3];         /* 0x00 */
    f32 x0C;            /* 0x0C */
    u16 kind;           /* 0x10 object kind (0x1A, 0x52 = torches?) */
    u16 num;            /* 0x12 */
    u16 sub;            /* 0x14 */
    u8 _pad16[2];
} STG_ITEM;

STG_ITEM *Stage_item_data_get();
void func_60E330(f32, f32 *, int, int);
void Eft13_set_pos(f32, f32 *, int);
void func_618F00(f32 *, int);
int ran_suu();

/* ---- */

extern f32 *sun_pos_tbl[];
extern u8 *st_sun_rgba_tbl[];
extern s16 *st_sun_tb_tbl[];
extern f32 rview_mat[];
f32 flArcTan2(f32, f32);
f32 flSin(f32);
void SetOpeMode();
void SetTrnslMode();
void flps0004();
void flps0005();

#define SUN_COL(c, o) ((c)[(o) + 2] | (((c)[(o) + 1] << 8) | (((c)[(o) + 3] << 24) | ((c)[(o)] << 16))))

typedef struct SKYR {
    s16 r[4];           /* x0, y0, x1, y1 */
    u32 c[4];           /* corner colours */
} SKYR;

typedef struct FLR {
    s16 r[4];
    u32 c;
} FLR;

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

void stage_se_move(w)
STGW *w;
{
    f32 best, px, pz;
    int i;
    PLW *pl = &player_work[game_w.master];
    int cnt;
    int n;
    f32 *p;
    f32 d;
    f32 pos[3];

    if (((*(u16 *)&game_w.x1E) & 3) != 0) {
        return;
    }
    pos[1] = 0;
    switch (game_w.stage) {
    case 0x1A:
        p = st26_se_pos[0];
        cnt = 6;
        n = 9;
        break;
    case 1:
        p = st01_se_pos[0];
        cnt = 3;
        n = 9;
        break;
    case 3:
        pos[0] = st03_se_pos2[0];
        pos[2] = st03_se_pos2[1];
        se_req2(7, 0x22, 0, pos, 0xB, 1);
        p = st03_se_pos[0];
        cnt = 2;
        n = 9;
        break;
    case 0x30:
        p = st48_se_pos[0];
        cnt = 2;
        n = 9;
        break;
    case 0x34:
        p = st52_se_pos[0];
        cnt = 3;
        n = 9;
        break;
    case 0x36:
        p = st54_se_pos[0];
        cnt = 6;
        n = 9;
        break;
    case 0x3E:
        p = st62_se_pos[0];
        cnt = 4;
        n = 0xB;
        break;
    default:
        return;
    }
    px = pl->pos[0];
    pz = pl->pos[2];
    best = -1.0f;
    for (i = 0; i < cnt; i++, p += 2) {
        f32 dx = px - p[0];
        f32 dz = pz - p[1];
        d = flSqrt(dx * dx + dz * dz);
        if (best < 0.0f || best > d) {
            best = d;
            pos[0] = p[0];
            pos[2] = p[1];
        }
    }
    if (game_w.stage == 0x1A) {
        se_req2(7, 0x22, 0, pos, n, 1);
    } else {
        se_req2(7, 0x21, 0, pos, n, 1);
    }
}

void stage_m(STGW *w)
{
    STG_ITEM *it;
    f32 pos[3];
    s16 t;
    u16 r;

    w->x08++;
    if (flash_flag == 0) {
    } else if (flash_flag == 1) {
        t = flash_timer - 1;
        flash_timer = t;
        if (t <= 0) {
            flash_flag = 2;
            flash_timer = 2;
        }
    } else if (flash_flag == 2) {
        t = flash_timer - 1;
        flash_timer = t;
        if (t <= 0) {
            flash_flag = 3;
            flash_timer = 0x5A;
        }
    } else {
        t = flash_timer - 1;
        flash_timer = t;
        if (t <= 0) {
            flash_flag = 0;
            flash_timer = 0;
        }
    }
    switch (game_w.stage) {
    case 0x4F:
        if (game_w.x1DC != 0) {
            switch (*(u16 *)&game_w.x1E % 10) {
            case 0:
                pos[0] = 1575.0f;
                pos[1] = 100.0f;
                pos[2] = 1740.0f;
                func_60E330(3.0f, pos, 2, 2);
                break;
            case 3:
                pos[0] = 1950.0f;
                pos[1] = 50.0f;
                pos[2] = 1825.0f;
                func_60E330(1.5f, pos, 2, 2);
                break;
            case 6:
                pos[0] = 1337.5f;
                pos[1] = 50.0f;
                pos[2] = 1892.5f;
                func_60E330(2.0f, pos, 2, 1);
                break;
            }
        }
        break;
    }
    it = Stage_item_data_get(game_w.stage);
    if (it != 0) {
        for (; it->pos[0] != -1.0f; it++) {
            if (it->num > 0) {
                if (it->kind == 0x1A || it->kind == 0x52) {
                    if ((*(u16 *)&game_w.x1E & 0x7F) == 0) {
                        pos[0] = it->pos[0];
                        pos[1] = 65.0f + it->pos[1];
                        pos[2] = it->pos[2];
                        Eft13_set_pos(0.7f, pos, 0x15);
                    }
                }
                if (it->sub == 4 && (*(u16 *)&game_w.x1E & 0x7F) == 0) {
                    r = ran_suu(1);
                    pos[0] = it->pos[0] + (f32)((r & 0x3F) - 0x20);
                    r = ran_suu(1);
                    pos[1] = 65.0f + it->pos[1] + (f32)((r & 0x3F) - 0x20);
                    r = ran_suu(1);
                    pos[2] = it->pos[2] + (f32)((r & 0x3F) - 0x20);
                    func_618F00(pos, 8);
                }
            }
        }
    }
    stage_se_move(w);
    stage_mv_ck();
}

void move_stage(void)
{
    STGW *w = &stage_work;

    switch (stage_work.step) {
    case 0:
        stage_i(w);
        break;
    case 1:
        stage_m(w);
        break;
    }
}

void trans_stage_sub(a, p)
int a;
u8 *p;
{
    flSetRenderState(0x1A, a);
    clay_attr_set(*(s32 *)(p + 0x88));
    flExecuteClay(*(s32 *)p, 0);
}

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
        {
            f32 *p;
            int k;
            SCLAY *mdl;
            p = (f32 *)st00_pos_tbl;
            mdl = &m->clay[3];
            for (k = 0; k < 4; k++, p += 3) {
                MTP(p);
                RS(0x1A, (u32)&mat);
                EXC();
            }
            break;
        }
    case 4:
        {
            SCLAY *mdl;
            mdl = &set_mdlw->clay[4];
            MTP(set04_pos_tbl);
            flmatRotY33(&mat, ANG((FRM & 0x3FF) << 6));
            RS(0x1A, (u32)&mat);
            EXC();
            break;
        }
    case 5:
        {
            SCLAY *mdl;
            mdl = &set_mdlw->clay[0];
            RS(0x60, 0);
            w->x20 = 0.02f + 0.02f * flSin(2.0f * (3.1415927f * ((f32)(FRM % 360) / 360.0f)));
            UVT(0.0f, w->x20);
            RS(0x19, (u32)&mat2);
            flmatInit(&mat);
            trans_stage_sub((int)&mat, (u8 *)mdl);
            RS(0x60, 0x80);
            break;
        }
    case 9:
        {
            STG_MDLS *m;
            int k;
            f32 *p;
            SCLAY *mdl;
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
        }
    case 0x14:
        {
            int k;
            STG_MDLS *m;
            f32 *p;
            SCLAY *mdl;
            m = set_mdlw;
            p = (f32 *)set20_pos_tbl;
            for (k = 0; k < 2; k++, p += 3) {
                mdl = &m->clay[k];
                MTP(p);
                trans_stage_sub((int)&mat, (u8 *)mdl);
            }
            break;
        }
    case 0x1C:
        {
            f32 *p;
            SCLAY *mdl;
            p = set28_pos_tbl;
            mdl = &set_mdlw->clay[0];
            MTP(p);
            flmatRotY33(&mat, p[3]);
            trans_stage_sub((int)&mat, (u8 *)mdl);
            break;
        }
    case 0x1B:
    case 0x21:
        {
            STG_MDLS *m;
            int k;
            f32 *p;
            SCLAY *mdl;
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
        }
    case 0x18:
    case 0x22:
        {
            STG_MDLS *m;
            f32 *p;
            int k;
            SCLAY *mdl;
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
        }
    case 0x24:
        {
            STG_MDLS *m;
            f32 *p;
            SCLAY *mdl;
            int k;
            m = set_mdlw;
            p = (f32 *)set36_pos_tbl;
            for (k = 0; k < 2; k++, p += 4) {
                mdl = &m->clay[9];
                MTP(p);
                flmatRotY33(&mat, p[3]);
                trans_stage_sub((int)&mat, (u8 *)mdl);
            }
            break;
        }
    case 0x26:
        {
            STG_MDLS *m;
            f32 *p;
            int k;
            SCLAY *mdl;
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
        }
    case 0x1D:
    case 0x27:
        {
            STG_MDLS *m;
            SCLAY *mdl;
            f32 *p;
            int k;
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
        }
    case 0x2B:
        {
            f32 *p;
            int k;
            SCLAY *mdl;
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
        }
    case 0x2D:
        {
            f32 *p;
            int k;
            SCLAY *mdl;
            p = (f32 *)set45_pos_tbl;
            mdl = &set_mdlw->clay[3];
            for (k = 0; k < 4; k++, p += 4) {
                MTP(p);
                flmatRotY33(&mat, p[3]);
                trans_stage_sub((int)&mat, (u8 *)mdl);
            }
            break;
        }
    case 0x32:
        {
            SCLAY *mdl;
            mdl = &set_mdlw->clay[1];
            RS(0x60, 0);
            flmatMakeTrans(&mat2, (f32)(X1E & 0x3F) / 64.0f, (f32)(X1E & 0x3F) / 64.0f, 0.0f);
            RS(0x19, (u32)&mat2);
            MTP(set50_pos_tbl);
            trans_stage_sub((int)&mat, (u8 *)mdl);
            RS(0x60, 0x80);
            break;
        }
    case 0x36:
        {
            SCLAY *mdl;
            mdl = &set_mdlw->clay[5];
            flmatMakeTrans(&mat2, 0.0f, 1.0f - (f32)(X1E & 0x3F) / 64.0f, 0.0f);
            RS(0x19, (u32)&mat2);
            MTP(set54_pos_tbl);
            trans_stage_sub((int)&mat, (u8 *)mdl);
            break;
        }
    case 0x3A:
        {
            f32 *p;
            int k;
            SCLAY *mdl;
            p = (f32 *)set58_pos_tbl;
            mdl = &set_mdlw->clay[0];
            for (k = 0; k < 6; k++, p += 4) {
                MTP(p);
                flmatRotY33(&mat, p[3]);
                trans_stage_sub((int)&mat, (u8 *)mdl);
            }
            break;
        }
    case 0x3E:
        {
            STG_MDLS *m;
            f32 *p;
            int k;
            SCLAY *mdl;
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
        }
    case 0x3F:
        {
            STG_MDLS *m;
            f32 *p;
            int k;
            SCLAY *mdl;
            m = set_mdlw;
            p = (f32 *)set63_pos_tbl;
            for (k = 0; k < 5; k++, p += 4) {
                mdl = &m->clay[0];
                MTP(p);
                flmatRotY33(&mat, p[3]);
                trans_stage_sub((int)&mat, (u8 *)mdl);
            }
            break;
        }
    case 0x40:
        {
            STG_MDLS *m;
            f32 *p;
            int k;
            SCLAY *mdl;
            m = set_mdlw;
            p = (f32 *)set64_pos_tbl;
            for (k = 0; k < 3; k++, p += 4) {
                mdl = &m->clay[0];
                MTP(p);
                flmatRotY33(&mat, p[3]);
                trans_stage_sub((int)&mat, (u8 *)mdl);
            }
            break;
        }
    case 0x47:
        {
            STG_MDLS *m;
            f32 *p;
            int k;
            SCLAY *mdl;
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
        }
    case 0x48:
    case 0x4A:
        {
            STG_MDLS *m;
            f32 *p;
            int k;
            SCLAY *mdl;
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
        }
    case 0x49:
        {
            f32 *p;
            int k;
            SCLAY *mdl;
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
        }
    case 0x4B:
        {
            f32 *p;
            int k;
            SCLAY *mdl;
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
        }
    case 0x4F:
        {
            SCLAY *mdl;
            u32 u;
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
    }
    clay_attr_reset();
    RS(0x60, 0);
    }
    }
    }
}

static u32 spr_disp_sub(f32 t, u32 a, u32 b)
{
    int c0, c1, c2, c3;
    u32 x;
    u32 r0, r1, r2, r3;

    c2 = (a >> 16) & 0xFF;
    x = (u32)(t * (f32)(s16)(((b >> 16) & 0xFF) - c2));
    r2 = (c2 + (x & 0xFF)) & 0xFF;
    c1 = (a >> 8) & 0xFF;
    x = (u32)(t * (f32)(s16)(((b >> 8) & 0xFF) - c1));
    r1 = (c1 + (x & 0xFF)) & 0xFF;
    c0 = a & 0xFF;
    c3 = (a >> 24) & 0xFF;
    x = (u32)(t * (f32)(s16)(((b >> 24) & 0xFF) - c3));
    r3 = (c3 + (x & 0xFF)) & 0xFF;
    x = (u32)(t * (f32)(s16)((b & 0xFF) - c0));
    r0 = (c0 + (x & 0xFF)) & 0xFF;
    return r0 | ((r1 << 8) | ((r3 << 24) | (r2 << 16)));
}

void stage_spr_disp(void)
{
    FLR fl;
    SKYR sky;
    f32 *sp;
    u8 *c;
    s16 *tb;
    u32 v;
    u32 sv;
    u32 t8, t7, t6, t5, t4, t3, t2, t1;
    f32 sx, sz;
    int s0;
    u32 u;

    flSetRenderState(0x60, 0);
    sp = sun_pos_tbl[game_w.stage];
    sz = sp[2];
    sx = sp[0];
    switch (game_w.stage) {
    case 0:
    case 0x1A:
        sky.r[2] = 0x280;
        sky.r[0] = 0;
        sky.r[3] = 0xE0;
        sky.r[1] = 0;
        sky.c[0] = 0x80FFFFA0;
        sky.c[1] = 0x80FFFFA0;
        sky.c[2] = 0xDCFF80;
        sky.c[3] = 0xDCFF80;
        flps0005(&sky);
        break;
    default:
        sv = (u16)(int)(0.5f + 65536.0f * flArcTan2(-rview_mat[8], -rview_mat[10]) / 6.2831855f);
        v = (u16)(int)(0.5f + 65536.0f * flArcTan2(sx, sz) / 6.2831855f);
        c = st_sun_rgba_tbl[game_w.stage];
        tb = st_sun_tb_tbl[game_w.stage];
        sky.r[0] = 0;
        sky.r[1] = tb[0];
        sky.r[2] = 0x280;
        sky.r[3] = tb[1];
        t8 = SUN_COL(c, 0);
        t7 = SUN_COL(c, 4);
        t6 = SUN_COL(c, 8);
        t5 = SUN_COL(c, 12);
        t4 = SUN_COL(c, 16);
        t3 = SUN_COL(c, 20);
        t2 = SUN_COL(c, 24);
        t1 = SUN_COL(c, 28);
        v = (v - (u16)sv) & 0xFFFF;
        if ((int)v < 0x4000) {
            sky.c[0] = spr_disp_sub((f32)v / 16384.0f, t8, t6);
            sky.c[1] = spr_disp_sub((f32)v / 16384.0f, t8, t5);
            sky.c[2] = spr_disp_sub((f32)v / 16384.0f, t4, t2);
            sky.c[3] = spr_disp_sub((f32)v / 16384.0f, t4, t1);
        } else if ((int)v < 0x8000) {
            sky.c[0] = spr_disp_sub((f32)(int)(v - 0x4000) / 16384.0f, t6, t7);
            sky.c[1] = spr_disp_sub((f32)(int)(v - 0x4000) / 16384.0f, t5, t7);
            sky.c[2] = spr_disp_sub((f32)(int)(v - 0x4000) / 16384.0f, t2, t3);
            sky.c[3] = spr_disp_sub((f32)(int)(v - 0x4000) / 16384.0f, t1, t3);
        } else if ((int)v < 0xC000) {
            sky.c[0] = spr_disp_sub((f32)(int)(v - 0x8000) / 16384.0f, t7, t5);
            sky.c[1] = spr_disp_sub((f32)(int)(v - 0x8000) / 16384.0f, t7, t6);
            sky.c[2] = spr_disp_sub((f32)(int)(v - 0x8000) / 16384.0f, t3, t1);
            sky.c[3] = spr_disp_sub((f32)(int)(v - 0x8000) / 16384.0f, t3, t2);
        } else {
            sky.c[0] = spr_disp_sub((f32)(int)(v - 0x8000 - 0x4000) / 16384.0f, t5, t8);
            sky.c[1] = spr_disp_sub((f32)(int)(v - 0x8000 - 0x4000) / 16384.0f, t6, t8);
            sky.c[2] = spr_disp_sub((f32)(int)(v - 0x8000 - 0x4000) / 16384.0f, t1, t4);
            sky.c[3] = spr_disp_sub((f32)(int)(v - 0x8000 - 0x4000) / 16384.0f, t2, t4);
        }
        flps0005(&sky);
        break;
    }
    if (flash_flag != 0) {
        fl.r[0] = 0;
        fl.r[2] = 0x280;
        fl.r[3] = 0x1C0;
        fl.r[1] = 0;
        if (flash_flag == 1) {
            SetTrnslMode(4, 5);
            s0 = 0xFF;
            SetTrnslMode(1, 1);
            SetOpeMode(1);
        } else if (flash_flag == 2) {
            SetTrnslMode(4, 5);
            u = (u32)(127.5f * (f32)flash_timer);
            s0 = (0xFF - (u & 0xFF)) & 0xFF;
            fl.c = s0 << 24;
            flps0004(&fl);
            SetTrnslMode(1, 1);
            SetOpeMode(1);
        } else {
            u = (u32)(255.0f * (1.0f + flSin(2.0f * (3.1415927f * (360.0f * (f32)(flash_timer * 0xB6 + 0x7FFF + 0x4001) / 65536.0f / 360.0f)))));
            s0 = u & 0xFF;
            SetTrnslMode(4, 5);
        }
        if (flash_flag == 1 || flash_flag == 2) {
            fl.c = -1;
        } else {
            fl.c = ((s0 & 0xFF) << 24) | 0xFFFFFF;
        }
        flps0004(&fl);
        if (flash_flag == 1 || flash_flag == 2) {
            SetOpeMode(0);
        }
    }
    flSetRenderState(0x60, 0x80);
}
