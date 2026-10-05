/*
 * rt_player.c - the hunter driven by the game's own player code.
 *
 * Each tick (rt_player_tick): rt_pad_tick turns the host pad into Psw
 * (ioRead_sub), then pl_move (src/main/pl/pl48.c, matched) runs exactly
 * as on the PS2: pl_sw_set fills PLW.sw, pl_move_sub (pl_nm.c) runs the
 * timers, damage, the state machine (pl_move_sub_sub -> pl_normal /
 * pl_attack / pl_damage ... -> pl_mvNNN / pl_atNNN through their jump
 * tables), turning, the motion step (pl_chr_sub -> frame_init /
 * frame_move), the per-motion sound/effect hook (pl01_effect_move) and
 * the stage collision; hit_timer_calc_shl ages the attack shells.
 * The helpers that are not decompiled yet are in rt_pl.c (from the asm).
 *
 * Set-up (rt_player_game_init) does what init_pl_work (main 0x1116E0,
 * g_game_init) does for the master player in an offline quest: equipment
 * (+0x35E type/+0x360 weapon id, +0x34C = Ken_data[id][0], kind =
 * Battle_type[+0x34C]) and User_data for Get_equip_value; then the game's
 * pl_init(0) (pl01.c) places the hunter at stage_start_pos and starts the
 * idle motion (pl_init_sub -> normal_char_set).
 *
 * RT_PL_STANDIN=1 keeps the old host stand-in (turn/run/idle only).
 */
#include "rt.h"
#include "types.h"
#include "game.h"
#include "pl.h"
#include "fl.h"
#include "frame.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern FLMAT rview_mat;
void pl_sw_set(void);
void pl_move(void);
void pl_init(int mode);
void rt_pad_tick(void);
void hit_chk_init(void);
extern u8 Ken_data[][0x18];
extern u8 Gun_data[][0x14];
extern u8 Battle_type[];
extern u8 User_data[];

#define PF(pl, T, o) (*(T *)((u8 *)(pl) + (o)))

static int use_game = -1;

/* Weapon: Ken_data id (sword type 6) or Gun_data id (type 7). Default:
 * Ken 156, the first sword-and-shield (job 4) in the table. RT_WEAPON=id
 * picks another sword (e.g. 1 = the first great sword, job 0). */
void rt_player_game_init(int no)
{
    PLW *pl = &player_work[no];
    int wid = getenv("RT_WEAPON") ? atoi(getenv("RT_WEAPON")) : 156;
    int type = 6;
    if (wid < 0 || wid >= 234) wid = 156;
    /* init_pl_work's offline master path (Set_userdata) [read from the asm] */
    pl->be_flag = 1;
    pl->id = (u16)no;
    pl->stg = game_w.stage;
    PF(pl, u8, 0x35E) = 0;
    PF(pl, u8, 0x35F) = (u8)type;
    pl->wpn_kind = (u16)wid;
    pl->work34C = Ken_data[wid][0];
    pl->kind = Battle_type[pl->work34C];
    PF(User_data, u8, 0x3CD) = (u8)type;
    PF(User_data, u16, 0x3CE) = (u16)wid;
    game_w.pl_state[no] = 1;
    pl_init(0);
    if (getenv("RT_PL_TRACE"))
        fprintf(stderr, "rt_player: weapon %d model %d job %d at %.0f %.0f %.0f act %d/%d chr %d/%d\n",
                wid, pl->work34C, pl->kind, pl->pos[0], pl->pos[1], pl->pos[2], pl->flag14, pl->flag15,
                PF(pl, u16, 0x2DC), PF(pl, u16, 0x2DE));
}

/* ---------------------------------------------- old host stand-in */
void HitWallPlayer(void *ent, int keep);
int GetFloorSlide(void *ent, f32 *out, int flag);
int GetGroundHitStatusAreaPl(void *ent, f32 *pos, void *attr, f32 *out, f32 *flag);
u8 Pl_stg_ck(void *);

static int moving[8];
static float fall_v[8];

static void set_motion(PLW *pl, int legs, int upper, int blend)
{
    FRW *w = (FRW *)pl;
    w->chr[0] = (u16)legs;
    w->chr[1] = (u16)upper;
    w->mt[0].spd = 1.0f;
    w->mt[1].spd = 1.0f;
    frame_init(w, 0, blend, 0);
    frame_init(w, 0, blend, 1);
}

static int ang_diff(int a, int b)
{
    return (s16)(u16)(b - a);
}

static void standin_collide(PLW *pl)
{
    f32 v[4], gy;
    int no = pl->id & 7;
    if (Pl_stg_ck(pl) & 0xFF)
        HitWallPlayer(pl, 0);
    if (pl->flag14 == 0 && (Pl_stg_ck(pl) & 0xFF))
        GetFloorSlide(pl, v, 1);
    if (GetGroundHitStatusAreaPl(pl, pl->pos, (u8 *)pl + 0x70C, &gy, (f32 *)((u8 *)pl + 0x7E4)) == 1)
        pl->x5AC = gy;
    if (!(Pl_stg_ck(pl) & 0xFF))
        return;
    gy = pl->x5AC;
    if (pl->pos[1] < gy || pl->pos[1] - gy < 30.0f) {
        pl->pos[1] = gy;
        fall_v[no] = 0;
    } else {
        fall_v[no] -= 3.0f;
        pl->pos[1] += fall_v[no];
        if (pl->pos[1] < gy) {
            pl->pos[1] = gy;
            fall_v[no] = 0;
        }
    }
}

static void standin_tick(int no)
{
    PLW *pl = &player_work[no];
    int want;
    PF(pl, f32, 0x5A0) = pl->pos[0];
    PF(pl, f32, 0x5A4) = pl->pos[1];
    PF(pl, f32, 0x5A8) = pl->pos[2];
    rt_pad_tick();
    pl_sw_set();
    want = pl->sw.pow[0] > 0;
    if (want) {
        float a = (float)pl->sw.ang[0] * (6.2831853f / 65536.0f);
        float sx = cosf(a), sy = sinf(a);
        float rx = rview_mat[0][0], rz = rview_mat[0][2];
        float fx = -rview_mat[2][0], fz = -rview_mat[2][2];
        float dx = rx * sx + fx * sy, dz = rz * sx + fz * sy;
        int target = (int)(atan2f(dx, dz) * (65536.0f / 6.2831853f)) & 0xFFFF;
        int d = ang_diff(pl->ang[1], target);
        if (d > 0x800) d = 0x800;
        if (d < -0x800) d = -0x800;
        pl->ang[1] = (pl->ang[1] + d) & 0xFFFF;
        pl->ang_y = (s16)pl->ang[1];
    }
    if (want != moving[no]) {
        moving[no] = want;
        set_motion(pl, want ? 3 : 1, want ? 103 : 101, 4);
    }
    rt_snd_player_motion(no);
    frame_move((FRW *)pl);
    standin_collide(pl);
}

/* ---------------------------------------------- tick */
int rt_player_uses_game(void)
{
    if (use_game < 0)
        use_game = getenv("RT_PL_STANDIN") == NULL;
    return use_game;
}

void rt_player_tick(int no)
{
    if (!rt_player_uses_game()) {
        standin_tick(no);
        return;
    }
    rt_pad_tick();
    pl_move();
    if (getenv("RT_PL_TRACE")) {
        PLW *pl = &player_work[no];
        printf("pl: act %d/%d step %d chr %d/%d fr %.1f spd %.1f pos %.0f %.0f %.0f ang %04X st %d sw %04X/%04X\n",
               pl->flag14, pl->flag15, PF(pl, u8, 5), PF(pl, u16, 0x2DC), PF(pl, u16, 0x2DE),
               PF(pl, f32, 0x19C), PF(pl, f32, 0x1A0), pl->pos[0], pl->pos[1], pl->pos[2],
               pl->ang[1] & 0xFFFF, pl->st, pl->sw.now, pl->sw.trg);
    }
}

/* Read back what the game code saw (for tests): buttons, stick. */
void rt_player_sw(int no, int *now, int *ang, int *pow)
{
    PLW *pl = &player_work[no];
    *now = pl->sw.now;
    *ang = pl->sw.ang[0];
    *pow = pl->sw.pow[0];
}

void rt_player_set_ang(int no, int ang_y)
{
    player_work[no].ang[1] = ang_y & 0xFFFF;
    player_work[no].ang_y = (s16)ang_y;
}
