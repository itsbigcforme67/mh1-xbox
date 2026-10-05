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

#define WEAK __attribute__((weak))
static void once(const char *n) { if (getenv("RT_TRACE")) fprintf(stderr, "rt_flow: %s not ported (no-op)\n", n); }
#define NOP(name) WEAK void name() { static int o; if (!o++) once(#name); }
#define NOP0(name) WEAK int name() { static int o; if (!o++) once(#name); return 0; }

/* ------------------------------------------------ the host tick as game_core */
static void (*core_fn)(void);
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
int rt_flow_tick(void)
{
    static u32 last = 0xFFFFFFFF;
    static int tick;
    int m = game_w[0];
    u32 st = game_w[0] | game_w[1] << 8 | game_w[0xD5] << 16 | (u32)(quest_w[0] | quest_w[1] << 4 | (quest_w[6] & 0xF) << 8) << 24;
    tick++;
    if (getenv("RT_QUEST_TRACE") && st != last) {   /* mode, step, game_w+0xD5, quest_w x00/x01/x06 */
        fprintf(stderr, "rt_flow: tick %d mode %d step %d D5 %d quest x00 %d x01 %d x06 %d time %d monsters %d\n",
                tick, game_w[0], game_w[1], game_w[0xD5], (s8)quest_w[0], (s8)quest_w[1], (s8)quest_w[6],
                *(s32 *)(quest_w + 0x10), *(s16 *)(quest_w + 0x34));
        last = st;
    }
    switch (m) {
    case 2: game2(); break;
    case 3: game3(); break;
    case 4: game4(); break;
    case 5: game5(); break;
    default: game_core(); break;
    }
    return m;
}

/* ------------------------------------------------ loading / system (no-ops) */
NOP(flFlip) NOP(flSndPortStop) NOP0(flSndPackLoadStatus) NOP0(load_busy_ck) NOP(FlushCache)
NOP(snd_joint_load_init) NOP(load_bin_req) NOP(flSndPackLoadBG2) NOP(flSndPackLoadBG)
NOP(all_reset) NOP(view_reset) NOP(stage_fog_set) NOP(stage_bgm_set) NOP0(snd_joint_load)
NOP(round_init) NOP(flCompact) NOP(fade_set) NOP(DispWholeMap) NOP(Zero_rev_set) NOP(vib_stop_all)
NOP(Tsk_Execute) NOP(st_model_load) NOP(stage_w_init) NOP(stage_load) NOP(stage_init)
NOP(stage_free) NOP0(snd_joint_load_pl) NOP(smoke_init) NOP(smell_init) NOP(senko_init) NOP(prim_init)
NOP(Plsel_task) NOP(ot_init) NOP0(net_start_ck) NOP(net_receive_pl_pos_set) NOP(Load_overlay)
NOP(init_light_work) NOP(flInitPhaseStarted) NOP(flInitPhaseFinished) NOP(EvDemoMove)
NOP(EvDemoInitialize) NOP(em_yobi_init) NOP(em_effect_pull) NOP(ear_init) NOP(Disp_load_start)
NOP(Copy_user_id) NOP(Disp_NowLoading2) NOP(Start_item_init)

/* ------------------------------------------------ 2D (replaced as it is ported) */
NOP(font_print) NOP(font_draw) NOP(font_set_palette) NOP(flfntLocate) NOP(flfntSetSize)
NOP(SpritePut) NOP(trans) NOP(result_prog) NOP(Info_control) NOP(Info_Initialization)
