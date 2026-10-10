/*
 * rt_flow.c - the game-mode loop (src/main/game/f_game.c / f_gameb.c:
 * game0..game5) on the port runtime.
 *
 * The PC runs the in-quest part of the PS2 loop: game2 (quest running:
 * game_core, Info_control, Quest_condition_judging, Game_clear_ck),
 * game3 (the "quest clear" screen) and game5 (the result / reward screen,
 * result_prog). game_core itself (swset, move, trans, hit_check) is the
 * host's tick: the viewer registers it with rt_flow_set_core().
 *
 * The loading modes (game0/game1, game10-13, game2 steps 2-6: stage
 * reload) do PS2 file/IOP work the host does its own way; their callees
 * here are weak no-ops that say so once (RT_TRACE=1).
 */
#include "rt.h"
#include "types.h"
extern u8 game_w[];   /* +0: mode (no game header here: the no-ops below are K&R) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void rt_font_tick_begin(void);
void rt_prims_reset(void);

#define WEAK __attribute__((weak))
static void once(const char *n) { rt_log_standin(n); if (getenv("RT_TRACE")) fprintf(stderr, "rt_flow: %s not ported (no-op)\n", n); }
#define NOP(name) void name() { static int o; if (!o++) once(#name); }
#define NOP0(name) int name() { static int o; if (!o++) once(#name); return 0; }

/* ------------------------------------------------ the host tick as game_core */
static void (*core_fn)(void);
static void (*back_fn)(void);
static void (*village_fn)(void);
void rt_flow_set_back(void (*fn)(void)) { back_fn = fn; }
void rt_flow_set_village(void (*fn)(void)) { village_fn = fn; }
void rt_flow_set_core(void (*fn)(void)) { core_fn = fn; }
int game_core(void)
{
    if (core_fn)
        core_fn();
    return 0;
}

void game2(void); void game3(void); void game4(void); void game5(void);
/* One tick of the game modes past loading (game_w.mode 2..5), as
 * Game_task's step 4 dispatches them. Returns the mode it ran. */
extern u8 quest_w[];
int rt_flow_mode(void) { return game_w[0]; }
void rt_flow_set_mode(int m) { game_w[0] = (u8)m; game_w[1] = 0; }
int rt_game_stage(void) { return game_w[0x14]; }
int rt_flow_tick(void)
{
    static u32 last = 0xFFFFFFFF;
    static int tick;
    int m = game_w[0];
    u32 st = game_w[0] | game_w[1] << 8 | game_w[0xD5] << 16 | (u32)(quest_w[0] | quest_w[1] << 4 | (quest_w[6] & 0xF) << 8) << 24 ^ (u32)*(s16 *)(quest_w + 0x30) * 0x9E3779B1u ^ (u32)*(s16 *)(quest_w + 0x32) * 0x85EBCA6Bu;
    tick++;
    {   /* the debug log: mode / stage / quest changes (quest id = select_w+0xAC) */
        extern u8 select_w[];
        rt_log_game(m, game_w[1], game_w[0x14], select_w[0xAC], game_w[0xD5]);
    }
    rt_font_tick_begin();       /* the text of the previous tick is replaced */
    rt_prims_reset();           /* ot_init */
    if (getenv("RT_QUEST_TRACE")) {     /* the master player's pouch (PLW+0x828, 20 x {id, n}: ItemCopy copies 0x50 bytes) when it changes */
        extern u8 player_work[];
        static s16 old[40];
        s16 *it = (s16 *)(player_work + 0x828);
        int i;
        if (memcmp(old, it, sizeof old)) {
            memcpy(old, it, sizeof old);
            fprintf(stderr, "rt_flow: tick %d pouch:", tick);
            for (i = 0; i < 20; i++)
                if (it[2 * i])
                    fprintf(stderr, " %d:%d", it[2 * i], it[2 * i + 1]);
            fprintf(stderr, "\n");
        }
    }
    if (getenv("RT_QUEST_TRACE")) {     /* the money (User_data+0x20) when it changes: the reward (Gold_add, f_ud.c), fees */
        extern u8 User_data[];
        static s32 old_gold = -1;
        s32 g = *(s32 *)(User_data + 0x20);
        if (old_gold >= 0 && g != old_gold)
            fprintf(stderr, "rt_quest: tick %d money %+d -> %d\n", tick, g - old_gold, g);
        old_gold = g;
    }
    if (getenv("RT_QUEST_TRACE") && game_w[0] == 5 && game_w[1] == 1 && st != last) {   /* the reward list (game_w+0x128) */
        s16 *r = (s16 *)(game_w + 0x128);
        int i;
        fprintf(stderr, "rt_flow: rewards:");
        for (i = 0; i < 32; i++)
            if (r[2 * i])
                fprintf(stderr, " %d:%d", r[2 * i], r[2 * i + 1]);
        fprintf(stderr, "\n");
    }
    if (getenv("RT_QUEST_TRACE") && st != last) {   /* mode, step, game_w+0xD5, quest_w x00/x01/x06 */
        fprintf(stderr, "rt_flow: tick %d mode %d step %d D5 %d quest x00 %d x01 %d x06 %d time %d monsters %d kills left %d x %d, %d x %d\n",
                tick, game_w[0], game_w[1], game_w[0xD5], (s8)quest_w[0], (s8)quest_w[1], (s8)quest_w[6],
                *(s32 *)(quest_w + 0x10), *(s16 *)(quest_w + 0x34),
                *(s16 *)(quest_w + 0x30), *(s16 *)(quest_w + 0x2C), *(s16 *)(quest_w + 0x32), *(s16 *)(quest_w + 0x2E));
        last = st;
    }
    switch (m) {
    case 2: game2(); break;
    case 3: game3(); break;
    case 4: game4(); break;
    case 5: game5(); break;
    case 6:                     /* Game_task: offline, all_reset and back to the
                                 * village (lobby.bin Local_main, rt_village.c);
                                 * without it the host starts the quest again */
        if (village_fn) {
            village_fn();
        } else if (back_fn) {
            if (getenv("RT_QUEST_TRACE"))
                fprintf(stderr, "rt_flow: tick %d mode 6: back (quest restarted by the host)\n", tick);
            back_fn();
        }
        break;
    default: game_core(); break;
    }
    return m;
}

/* ------------------------------------------------ stage change (game2 steps 2-6)
 * st_model_load(stage) (main 0x11EE00..) loads the area and set models;
 * the host loads the stage's files, collision, camera file and sound
 * itself (viewer.c registers the loader). The sound joint loads count as
 * done at once. */
static int (*stage_loader)(int);
void rt_set_stage_loader(int (*fn)(int)) { stage_loader = fn; }
void st_model_load(int stage)
{
    if (getenv("RT_QUEST_TRACE"))
        fprintf(stderr, "rt_flow: st_model_load(%d)\n", stage);
    if (stage_loader)
        stage_loader(stage);
}
int snd_joint_load() { return 1; }
int snd_joint_load_pl() { return 1; }

/* ------------------------------------------------ loading / system (no-ops) */
NOP(flFlip) NOP(flSndPortStop) NOP0(flSndPackLoadStatus) NOP0(load_busy_ck) NOP(FlushCache)
NOP(snd_joint_load_init) NOP(load_bin_req) NOP(flSndPackLoadBG2) NOP(flSndPackLoadBG)
NOP(view_reset) 
/* init_light_work (flow02.c): light_init (src/main/model/light_init_nm.c) clears and fills light_work from the stage light rows */
void light_init(void);
void init_light_work(void) { light_init(); }
NOP(round_init) NOP(flCompact) NOP(vib_stop_all)
NOP(stage_load) NOP(stage_init)
NOP(stage_free)
NOP(Plsel_task) NOP(ot_init) NOP(Load_overlay)
#ifndef MH1_ONLINE     /* ONLINE=1: rt_np.c / netsyn03.c (co-op) */
NOP0(net_start_ck) NOP(net_receive_pl_pos_set)
#endif
NOP(flInitPhaseStarted) NOP(flInitPhaseFinished)
NOP(em_effect_pull) NOP(Disp_load_start)
NOP(Copy_user_id) NOP(Disp_NowLoading2)

/* trans: rt_boot.c (draws only while the boot screens run) */
