/* Stage work (stage_work, 0x3D8230): per-frame stage logic, sound sources,
 * sun/flash sprites. SLPM_654.95 main, f_stage range. */
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

void stage_mv_ck(void) {
    STG_MV *p;
    PLW *pl = &player_work[game_w.master];
    s16 hit;
    f32 a[3], b[3];
    f32 dx, dz;

    if (pl->vital <= 0 || GW8(0xD5) == 6 || GW8(0xD5) == 4) {
        if (pl->x738 == 1) {
            pl->x738 = 0;
        }
        return;
    }
    if (pl->x738 != 0) {
        return;
    }
    if (pl->x56F != 0) {
        return;
    }
    p = Stage_mv_data_get(pl->stg, game_w.master);
    if (p == 0) {
        return;
    }
    for (; p->id != 0xFFFF; p++) {
        if (pl->pos[1] < p->pos[1]) {
            continue;
        }
        if (!(pl->pos[1] < p->pos[1] + p->h)) {
            continue;
        }
        switch (p->kind) {
        case 0:
            dx = pl->pos[0] - p->pos[0];
            dz = pl->pos[2] - p->pos[2];
            if (flSqrt(dx * dx + dz * dz) <= p->r) {
                hit = 1;
            } else {
                hit = 0;
            }
            break;
        case 1:
            a[0] = p->pos[0];
            a[1] = p->pos[1];
            a[2] = p->pos[2];
            b[0] = p->box[0];
            b[1] = p->box[1];
            b[2] = p->box[2];
            if (hit_point_cbd(p->h, p->r, pl->pos, a, b) != 0) {
                hit = 1;
            } else {
                hit = 0;
            }
            break;
        }
        if (hit != 0) {
            pl->x738 = 1;
            pl->x73A = p->id;
            pl->work73C = p->dest[0];
            pl->work740 = p->dest[1];
            pl->work744 = p->dest[2];
            pl->work570 = p->ang + 0x4000;
            Pl_ofs_set(pl, &pl->work73C, (u16)pl->work570);
            net_send_pl(pl, 5, 0);
        }
    }
}


void clr_stg_work(void) {
    memset(&stage_work, 0, 0x64);
}

void clr_flash(void) {
    flash_flag = 0;
    flash_timer = 0;
}

s16 Stage_env_ck(u8 stg) {
    return Stg_env_type[stg];
}

void Pile_on(void) {
    game_w.x1B2 = 1;
    if (Pl_master_ck() != 0) {
        PilebunkerCameraRequest();
    }
}

/* Ambient sound sources per stage: stage_i starts the first one and
 * stage_se_move re-targets the nearest one to the master player every 4th
 * frame.  stNN_se_pos hold x/z pairs. */
void stage_i(STGW *w) {
    f32 pos[3];
    f32 *p;
    s16 n;

    w->step++;
    w->x08 = 0;
    pos[1] = 0;
    switch (game_w.stage) {
    case 0x1A:
        pos[0] = st26_se_pos[0][0];
        pos[2] = st26_se_pos[0][1];
        se_req2(7, 0x22, 0, pos, 9, 0);
        return;
    case 1:
        p = st01_se_pos[0];
        n = 9;
        break;
    case 3:
        pos[0] = st03_se_pos2[0];
        pos[2] = st03_se_pos2[1];
        se_req2(7, 0x22, 0, pos, 0xB, 0);
        p = st03_se_pos[0];
        n = 9;
        break;
    case 0x30:
        p = st48_se_pos[0];
        n = 9;
        break;
    case 0x34:
        p = st52_se_pos[0];
        n = 9;
        break;
    case 0x36:
        p = st54_se_pos[0];
        n = 9;
        break;
    case 0x3E:
        p = st62_se_pos[0];
        n = 0xB;
        break;
    default:
        return;
    }
    pos[0] = p[0];
    pos[2] = p[1];
    se_req2(7, 0x21, 0, pos, n, 0);
}
