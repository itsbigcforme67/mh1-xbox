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
