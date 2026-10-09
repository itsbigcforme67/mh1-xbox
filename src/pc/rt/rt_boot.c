/*
 * rt_boot.c - power-on to the village: the game's own boot tasks on the
 * port runtime.
 *
 * On the PS2, ACRMain (main 0x11F3F0, src/main/sys/ioread_nm.c) sets up the
 * system work (InitSystemData), loads select.bin and starts its Init_task
 * in the task scheduler (src/main/sys/tsk_nm.c). From there everything is
 * the game's own code, run by the Scheduler once per frame:
 *   Init_task (select.bin: textures, sound packs, memory card check and
 *   the options auto-load, CardAtld) -> Demo_task (rating screen, logos,
 *   title, opening movie) -> Select_task (main omake: "new game /
 *   continue / extras / options") -> Edit_task (select.bin: character
 *   creation, name entry, save) or Cont_task (load a hunter from the card)
 *   -> Game_task (main f_game: offline it loads lobby.bin and runs the
 *   village, Local_main).
 * Fade_task (screen fades) and Card_task run as system tasks beside them.
 *
 * The PC does the same: rt_boot_init = InitSystemData + Tsk_Execute
 * (Init_task); rt_boot_tick = one ACRMain frame (font/prim reset,
 * Scheduler, fade_draw). The tasks draw while they run (flps0008,
 * font_draw, trans()), so a tick's draws are recorded (gfx_rec.c) and the
 * host replays the last tick's picture every frame. Game_task is where
 * the host takes over: the viewer's village/quest flow (rt_flow.c,
 * rt_village.c) is the PC's Game_task.
 */
#include "rt.h"
#include "types.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern u8 system_w[];
extern s8 option_w[];
extern u8 game_w[];
extern u8 card_w[], card_w2[];
extern u16 System_flag;

void SchedulerInit(void);
void Scheduler(void);
void Tsk_Execute(void *fn, int n);
void Tsk_Exit(void *t);
void Init_task(void *t);
void fade_draw(void);
void system_w_init(void);
void option_default_set(void);
void User_data_init(void);
void SoftKeyboard_init(void);
void Default_reibun_set(void);
void str_outmode(int);
void str_master_vol(int);
void flAdjustScreen(int, int);
void rt_font_tick_begin(void);
void rt_font_set_draw(int on);
void rt_prims_reset(void);
void rt_draw_ot(int t);
void font_draw_stack_no(int n);
void GameTrans(void);

static int boot_active, boot_done, boot_ticks;

int rt_boot_active(void) { return boot_active; }

/* ------------------------------------------------------------ system work
 * system_w_set (main, src/main/sys/ioread_nm.c): the options into system_w */
void system_w_set(void)
{
    system_w[0x1A] = 0;
    system_w[0x3C] = 0;
    system_w[0x22] = 1;
    system_w[0x39] = 5;
    system_w[0x2F] = 0xFF;
    system_w[0x38] = 0;
    system_w[0x3A] = 0;
    system_w[0x3B] = 0;
    system_w[0x07] = 1;
    system_w[0x04] = 0;
    system_w[0x24] = 0;
    system_w[0x14] = 1;
    system_w[0x31] = 1;
    system_w[0x06] = 0;
    system_w[0x05] = 0;
    system_w[0x17] = (u8)option_w[0];   /* outmode, se_vol, bgm_vol (sysw.h) */
    system_w[0x36] = (u8)option_w[1];
    system_w[0x37] = (u8)option_w[2];
    str_outmode(option_w[0]);
    str_master_vol(0);
    flAdjustScreen(option_w[5], option_w[6]);
}

/* Card_task / init_card_w (main 0x100E20, src/main/sys/adxs04.c) */
void Card_task(void *t) { (void)t; }
void init_card_w(void)
{
    memset(card_w, 0, 0x84);
    memset(card_w2, 0, 0x84);
    system_w[0x3C] = 0;
}

/* The PC's Game_task: when a boot task starts it (Edit_task after a new
 * hunter was saved, Cont_task after a load, mode_sel_end), the boot ends
 * and the host runs the game from the village (Game_task's offline path:
 * Load_overlay(3) = lobby.bin, Clear_lobby_ram, Local_main). */
void Game_task(u8 *t)
{
    if (!boot_done && getenv("RT_BOOT_TRACE"))
        fprintf(stderr, "rt_boot: tick %d Game_task started: to the village\n", boot_ticks);
    if (!boot_done)
        rt_log("boot: title / new hunter / load finished (Game_task started after %d ticks): going to the village", boot_ticks);
    boot_done = 1;
    if (system_w[0x10]) {       /* "go to town" (network mode): not on the PC yet */
        fprintf(stderr, "rt_boot: network mode is not available on the PC: going to the village\n");
        system_w[0x10] = 0;
    }
    Tsk_Exit(t);
}

/* all_reset (main 0x160A30, src/main/font/gfs01.c): the parts that are
 * game state on the PC: the TransSet list, the screen fade, sounds, the
 * font stacks, the system flags. Model/texture/memory frees and the
 * player/monster work clears stay with the host (it owns those). */
void TransReset(void);
void fade_reset(void);
void se_stop_all(void);
void font_stack_reset(void);
void rt_movie_stop(void);       /* rt_movie.c */
void all_reset(void)
{
    TransReset();
    fade_reset();
    se_stop_all();
    font_stack_reset();
    rt_movie_stop();            /* a movie left running when its task was killed */
    system_w[0x32] = 0;
    system_w[0x1B] = 1;
    system_w[0x33] = 0;
    system_w[0x3C] = 0;
    system_w[0x12] = 1;
}

/* PatchLoadinDNAS_Init / _Main (main f_net): after a load, the online
 * patch kept in the save is checked through DNAS. No network on the PC:
 * done at once (the auto-load screen CardAtld14 waits for it). */
void PatchLoadinDNAS_Init(void) {}
int PatchLoadinDNAS_Main(void) { return 1; }

/* ------------------------------------------------------------ boot */
void rt_boot_init(void)
{
    /* InitSystemData (ioread_nm.c) */
    system_w_init();
    option_default_set();
    User_data_init();
    Default_reibun_set();
    SoftKeyboard_init();
    system_w_set();
    /* ACRMain's first frame */
    SchedulerInit();
    system_w[0x0B] = 0;
    Tsk_Execute((void *)Init_task, 0);
    boot_active = 1;
    rt_log("boot: power-on sequence started (logos, title)");
    boot_done = 0;
    boot_ticks = 0;
}

/* One ACRMain frame. Returns 1 once Game_task has been started (the boot
 * is over: the caller starts the village), else 0. */
int rt_boot_tick(void)
{
    if (!boot_active)
        return 1;
    boot_ticks++;
    rt_log_boot_tick();
    rt_font_tick_begin();       /* font_stack_reset */
    rt_prims_reset();           /* ot_init */
    gfx_rec_begin();
    rt_font_set_draw(1);
    Scheduler();
    fade_draw();
    rt_font_set_draw(0);
    gfx_rec_end();
    if (getenv("RT_BOOT_TRACE")) {
        extern u8 tcb_w[];
        static u8 last[16][2];
        int i;
        for (i = 0; i < 16; i++) {
            s16 st = *(s16 *)(tcb_w + 0x20 * i);
            u8 step = tcb_w[0x20 * i + 8];
            if ((st != 0) != last[i][0] || step != last[i][1]) {
                uint32_t off;
                int func;
                void *fn = *(void **)(tcb_w + 0x20 * i + 4);
                fprintf(stderr, "rt_boot: tick %d task %d state %X step %d fn %p\n", boot_ticks, i, st, step, fn);
                (void)off; (void)func;
                last[i][0] = st != 0;
                last[i][1] = step;
            }
        }
    }
    if (boot_done) {
        boot_active = 0;
        return 1;
    }
    return 0;
}

/* Draw this frame's picture of the boot screens (the last tick's draws). */
void rt_boot_draw(void)
{
    PICK(PK_HUD, 200, 0, 0, 0);
    gfx_rec_replay();
}

/* ------------------------------------------------------------ system tasks
 * After the boot (and in runs that start straight in a quest) the host's
 * game flow stands for Game_task; the scheduler's system tasks still run
 * every tick: Fade_task in slot 9 (Card_task in 13 does nothing). The
 * scheduler's own per-frame work (TransReset, the sprite list) belongs to
 * the host flow then, so only these slots are stepped here, with the
 * Scheduler's state machine. */
void Fade_task(void *t);
extern u8 tcb_w[];
void rt_sys_init(void)
{
    SchedulerInit();
    Tsk_Execute((void *)Fade_task, 9);
    Tsk_Execute((void *)Card_task, 13);
}

#ifndef MH1_ONLINE     /* ONLINE=1: rt_np.c counts it (and paces the ticks) */
extern u16 System_timer;
#endif
void rt_sys_tick(void)
{
    int i;
#ifndef MH1_ONLINE
    /* the PS2's Scheduler (tsk_01.c) counts System_timer every frame: the map's boss icon pulse, the extras menu
     * glow and the monsters' eye/blink tables read it (it stood at 0 on the PC) */
    System_timer++;
#endif
    for (i = 6; i < 16; i++) {
        u8 *t = tcb_w + 0x20 * i;
        s16 *st = (s16 *)t, *tm = (s16 *)(t + 2);
        switch (*st) {
        case 4: case 0xC: *st = 8; break;
        case 8: (*(void (**)(void *))(t + 4))(t); break;
        case 2: *tm = 0; *st = 4; break;
        case 0x10:
            if (--*tm < 0) *tm = 0;
            else if (*tm == 0) *st = 4;
            break;
        }
    }
}

/* fade_draw for the host frame (after the screen layers, as ACRMain draws
 * it after the tasks); RT_NO_FADE=1 leaves fades out (debugging) */
void rt_fade_draw(void)
{
    if (!getenv("RT_NO_FADE")) {
        PICK(PK_FADE, 0, 0, 0, 0);
        gfx_set_2d_anchor(GFX_A_STRETCH);       /* the fade covers the whole window, not just the 4:3 part */
        fade_draw();
        gfx_set_2d_anchor(GFX_A_CENTER);
        PICK(PK_OTHER, 0, 0, 0, 0);
    }
}

/* ------------------------------------------------------------ trans
 * trans (main 0x163BC0, src/main/weapon/trans.c): the frame's draw order.
 * The boot tasks call it at the end of their frame; it runs the TransSet
 * callbacks and the ordering tables, then the screen layers and their font
 * stacks. Outside the boot the host draws (rt_game_draw / _2d), and the
 * game modes' calls do nothing here. */
void trans_sprite(void);
extern s16 spr_list_no;
extern u8 *sprite_area;
void trans(void)
{
    static const int order[5] = { 5, 6, 7, 8, 2 }, fstack[5] = { 0, 1, 2, 4, 3 };
    int i;
    void InitRenderState(int soft);
    void SetFilterMode(int);
    void SetTrnslMode(int, int);
    void flSetRenderState(int, u32);
    if (!boot_active)
        return;
    SetFilterMode(0);
    SetTrnslMode(4, 5);
    flSetRenderState(0x60, 0);
    flSetRenderState(0x6C, 1);
    GameTrans();
    rt_draw_ot(1);
    rt_draw_ot(3);
    rt_draw_ot(0);
    rt_draw_ot(4);
    if (sprite_area && spr_list_no > 0)
        trans_sprite();
    for (i = 0; i < 5; i++) {
        rt_draw_ot(order[i]);
        font_draw_stack_no(fstack[i]);
    }
    InitRenderState(1);
}
