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

/* trans_stage lives in trans_stage.c (single definition, also used by the PC runtime). */

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
                    pos[1] = (65.0f + it->pos[1]) + (f32)(int)((r & 0x3F) - 0x20);
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
