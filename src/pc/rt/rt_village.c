/*
 * rt_village.c - the village (Kokoto, lobby.bin offline) on the port
 * runtime.
 *
 * On the PS2 the offline Game_task (src/main/game/f_game_nm.c) ends a quest
 * in game mode 6: all_reset, Load_overlay(3) = lobby.bin, Clear_lobby_ram,
 * then Local_main every tick until it returns 1 (a quest was accepted at
 * the counter and the hunter left through the village gate), then game
 * mode 0 starts that quest (select_w+0xAC).
 *
 * The PC does the same with the lobby C (agent F's src/lobby/f, agent B's
 * src/lobby/lb, and src/lobby/f/lb_village_nm.c for what was not
 * decompiled): rt_village_enter = Clear_lobby_ram + step 0, rt_village_tick
 * = the host's per-tick work the PS2 does in move()/trans() around it (set
 * objects and effects, prim queues) + Local_main. The stage, the hunter and
 * the HUD are drawn by the host as in a quest.
 */
#include "rt.h"
#include "types.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern u8 lb_sys[];
extern u8 player_work[];
extern u8 em_work[];
extern u8 select_w[];
extern u8 game_w[];
extern u8 stage_work[];
void Clear_lobby_ram(void);
void clr_set_work(void);
s32 Local_main(void);
void rt_game_move(void);
void rt_font_tick_begin(void);
void rt_prims_reset(void);

static int active, tick, all_ticks, last_step = -1, last_x68 = -1, last_6 = -1;

int rt_village_active(void) { return active; }

/* Game_task mode 6 (offline): what the PS2 does before Local_main runs */
void rt_village_enter(void)
{
    memset(em_work, 0, 0xA10 * 20);     /* the quest's monsters are gone (all_reset) */
    clr_set_work();                     /* and the quest stage's set objects (all_reset -> clr_stg_work) */
    {   /* and every prim (all_reset -> prim_init): a set object's prim outlived its cleared work and
         * was drawn in the village (set19_trans on stage 87: crash after a co-op quest) */
        void prim_init(void);
        prim_init();
    }
    rt_cam_init(game_w[0x14]);          /* camera work (also with the host's own camera) */
    {
        void rt_lb_reload(void);
        static int visits;
        if (visits++)                   /* Load_overlay(3): lobby.bin data and .bss as loaded */
            rt_lb_reload();
    }
    Clear_lobby_ram();
    game_w[0x1DC] = 1;                  /* in the lobby overlay (Game_task step 1): lobby HUD prims, NPC talk sounds */
    if (getenv("RT_VILLAGE_SKIP_INTRO")) {  /* test aid (no save data): the first-visit event seen */
        void Event_flag_set(int);
        Event_flag_set(2);
    }
    if (getenv("RT_QCLEAR")) {          /* test aid: "83,84-87,...": hex quest numbers marked cleared, as f_reward's
                                           Quest_clear_bit_set does after a won quest (a save with these quests done) */
        void Quest_clear_bit_set(int);
        const char *s = getenv("RT_QCLEAR");
        while (*s) {
            char *e;
            long a = strtol(s, &e, 16), b = a;
            if (e == s)
                break;
            if (*e == '-')
                b = strtol(e + 1, &e, 16);
            for (; a <= b && a < 256; a++)
                Quest_clear_bit_set((int)a);
            s = *e ? e + 1 : e;
        }
    }
    if (getenv("RT_QUEST_TRACE")) {     /* star level as the Elder computes it, and the cleared quests */
        int lb_get_quest_level(int);
        int Quest_clear_bit_ck(int), q;
        fprintf(stderr, "rt_village: level %d, cleared:", lb_get_quest_level(1));
        for (q = 1; q < 0xB2; q++)
            if (Quest_clear_bit_ck(q))
                fprintf(stderr, " %02x", q);
        fprintf(stderr, "\n");
    }
    lb_sys[3] = 0;                      /* vs_square_init_pre */
    lb_sys[4] = 0;
    active = 1;
    tick = 0;
    if (getenv("RT_QUEST_TRACE")) {
        extern u8 User_data[];
        fprintf(stderr, "rt_village: enter (Clear_lobby_ram, Local_main from step 0), money %d\n", *(s32 *)(User_data + 0x20));
    }
}

/* One village tick. Returns the quest number when Local_main reports an
 * accepted quest (1), -1 when it leaves the village another way, else 0. */
int rt_village_tick(void)
{
    s32 r;
    tick++;
    all_ticks++;
    rt_font_tick_begin();
    rt_prims_reset();
    rt_game_move();                     /* set objects and effects (move_set / move_eft) */
    if (getenv("RT_LB_WARP")) {         /* test aid: "tick,x,z[,ang];...": put the hunter there at that village tick
                                         * (counted over all village visits of the run) */
        const char *s = getenv("RT_LB_WARP");
        while (s && *s) {
            int t = 0, a = -1;
            float x, z;
            int n = sscanf(s, "%d,%f,%f,%x", &t, &x, &z, &a);
            if (n >= 3 && t == all_ticks) {
                *(float *)(player_work + 0xAC) = x;
                *(float *)(player_work + 0xB4) = z;
                if (n == 4)
                    *(u16 *)(player_work + 0xE) = (u16)a;
                fprintf(stderr, "rt_village: tick %d (all %d) hunter warped to %.0f %.0f\n", tick, all_ticks, x, z);
            }
            s = strchr(s, ';');
            if (s)
                s++;
        }
    }
    r = Local_main();
    static int spots_stage = -1;
    if (getenv("RT_VILLAGE_TRACE") && game_w[0x14] != spots_stage && lb_sys[3] == 4) {     /* the stage's unique spots (exits, chairs ...) */
        spots_stage = game_w[0x14];
        void *Stage_unique_data_get(int st);
        u8 *r = Stage_unique_data_get(game_w[0x14]);
        fprintf(stderr, "rt_village: stage %d spots:\n", game_w[0x14]);
        for (; r && *(float *)(r + 4) != -1.0f; r += 0x18)
            fprintf(stderr, "rt_village:   spot kind %d at %.0f %.0f %.0f r %.0f ang %04X\n", *(u16 *)(r + 2),
                    *(float *)(r + 4), *(float *)(r + 8), *(float *)(r + 0xC), *(float *)(r + 0x10), *(u16 *)(r + 0x14));
    }
    if (getenv("RT_VILLAGE_TRACE") && tick % 90 == 60) {   /* the stage's NPCs: slot, model kind, type, talk kind, position */
        int i;
        for (i = 0; i < 20; i++) {
            u8 *e = em_work + 0xA10 * i;
            if (e[0] && e[0x1E])
                fprintf(stderr, "rt_village:   npc slot %d kind %d type %d talk %d act %d/%d pos %.0f %.0f %.0f\n", i, e[2], e[0x1B],
                        e[0x452], e[0x14], e[0x15], *(float *)(e + 0xAC), *(float *)(e + 0xB0), *(float *)(e + 0xB4));
        }
    }
    if (getenv("RT_VILLAGE_TRACE") && tick % 30 == 0) {
        float *p = (float *)(player_work + 0xAC), e[3], t[3], roll, fov;
        rt_cam_view(e, t, &roll, &fov);
        fprintf(stderr, "rt_village: stage_work %d %d mdls %p\n", stage_work[0], stage_work[1], *(void **)(stage_work + 0x3C));
        fprintf(stderr, "rt_village: tick %d pl %.0f %.0f %.0f ang %04X act %d/%d char %d | cam %.0f %.0f %.0f -> %.0f %.0f %.0f fov %.2f roll %.2f\n",
                tick, p[0], p[1], p[2], *(u16 *)(player_work + 0xE), player_work[0x14], player_work[0x15],
                *(u16 *)(player_work + 0x2DC), e[0], e[1], e[2], t[0], t[1], t[2], fov, roll);
    }
    if (getenv("RT_VILLAGE_TRACE") && *(s32 *)(lb_sys + 0x68) != 0 && tick % 10 == 0) {
        extern u8 lb_pit[];
        fprintf(stderr, "rt_village: tick %d talk %d/%d pit x0 %d pos %p page %d sel %d done %d\n", tick, (s8)lb_sys[6], (s8)lb_sys[7],
                *(s32 *)lb_pit, *(void **)(lb_pit + 4), (s8)lb_pit[8], (s8)lb_pit[9], (s8)lb_pit[0xB]);
    }
    if (getenv("RT_QUEST_TRACE") && *(s32 *)(lb_sys + 0x68) != 0 && lb_sys[6] != last_6) {
        extern u8 lb_pit[];
        fprintf(stderr, "rt_village: tick %d talk state %d (x68 %d) page %d sel %d\n", tick, (s8)lb_sys[6],
                *(s32 *)(lb_sys + 0x68), (s8)lb_pit[8], (s8)lb_pit[9]);
        last_6 = lb_sys[6];
    }
    if (getenv("RT_QUEST_TRACE") && (lb_sys[3] != last_step || *(s32 *)(lb_sys + 0x68) != last_x68)) {
        float *p = (float *)(player_work + 0xAC);
        extern u8 *cw;
        fprintf(stderr, "rt_village: tick %d step %d sub %d x68 %d stage %d pl %.0f %.0f %.0f act %d/%d quest %d (accepted %d)\n",
                tick, (s8)lb_sys[3], (s8)lb_sys[4], *(s32 *)(lb_sys + 0x68), game_w[0x14],
                p[0], p[1], p[2], player_work[0x14], player_work[0x15], *(s16 *)(select_w + 0xAC), cw ? cw[0x35D3] : -1);
        if (0) {
            int i;
            for (i = 0; i < 20; i++) {
                u8 *e = em_work + 0xA10 * i;
                if (e[0] && e[0x1E])
                    fprintf(stderr, "rt_village:   npc slot %d kind %d type %d talk %d pos %.0f %.0f %.0f\n", i, e[2], e[0x1B],
                            e[0x452], *(float *)(e + 0xAC), *(float *)(e + 0xB0), *(float *)(e + 0xB4));
            }
        }
        last_step = lb_sys[3];
        last_x68 = *(s32 *)(lb_sys + 0x68);
    }
    if (getenv("RT_QUEST_TRACE")) {     /* the Elder's quest list (Lb_make_quest_tbl_local: 5 levels x 5, urgent at +0x19) */
        extern u8 lb_quest_info[];
        extern u8 key_quest;
        static u8 last[0x28];
        if (memcmp(last, lb_quest_info, 0x28)) {
            int l, i;
            memcpy(last, lb_quest_info, 0x28);
            fprintf(stderr, "rt_village: tick %d quest list (key %02x):", tick, key_quest);
            for (l = 0; l < 5; l++) {
                fprintf(stderr, " %d*[", l + 1);
                for (i = 0; i < 5; i++)
                    fprintf(stderr, i ? " %02x" : "%02x", lb_quest_info[l * 5 + i]);
                fprintf(stderr, "]");
            }
            fprintf(stderr, " urgent %02x %02x %02x %02x %02x\n", lb_quest_info[0x19], lb_quest_info[0x1A],
                    lb_quest_info[0x1B], lb_quest_info[0x1C], lb_quest_info[0x1D]);
        }
    }
    if (getenv("RT_SHOP_TRACE")) {      /* the shop step machines (lbShop: step +0x14, sub +0x15, mode +0x19) */
        extern u8 lbShop[];
        static int last = -1;
        int v = lbShop[0x14] | lbShop[0x15] << 8 | lbShop[0x19] << 16 | lbShop[0x1B] << 24;
        if (v != last)
            fprintf(stderr, "rt_village: tick %d shop step %d sub %d mode %d x1B %d x68 %d\n", tick, (s8)lbShop[0x14],
                    (s8)lbShop[0x15], (s8)lbShop[0x19], lbShop[0x1B], *(s32 *)(lb_sys + 0x68));
        last = v;
    }
    if (r == 1 || r == -1) {
        void com_motion_load(int n);
        active = 0;
        game_w[0x1DC] = 0;
        com_motion_load(0);             /* the quest's common motions (plcom_tbl) again */
        return r == 1 ? *(s16 *)(select_w + 0xAC) : -1;
    }
    return 0;
}

/* Disp_NowLoading (disp1_nm.c, weak there): the "now loading" screen the
 * PS2 draws while it streams files; the PC loads at once, nothing to show */
void Disp_NowLoading(void) {}

/* com_motion_load (main 0x111A80): the common motion set n
 * (common_motion_data, 0x2EBC70: 0 plcom_tbl, 1 lbcom_tbl the village's,
 * 2 selcom_tbl) through load_plcom_motion + create_plcom_motion. The files
 * stay loaded (the motion handles point into them). */
uint8_t *rt_file_load(int idx, size_t *n);
void rt_motion_load_plcom(const uint8_t *tbl);
void com_motion_load(int n)
{
    static uint8_t *keep[3];
    const uint8_t *p = rt_addr(0x2EBC70 + 4 * (uint32_t)n, 4);
    int32_t idx;
    size_t sz;
    if (n < 0 || n > 2 || !p)
        return;
    memcpy(&idx, p, 4);
    if (!keep[n] && !(keep[n] = rt_file_load(idx, &sz))) {
        fprintf(stderr, "rt_village: common motions %d (AFS %d) not loaded\n", n, idx);
        return;
    }
    rt_motion_load_plcom(keep[n]);
}

/* ------------------------------------------------------------ village NPCs
 * npc_create_model (main 0x124A30): model slot n (= NPC kind, EMW+0x34F):
 * npc_model_data[n] / its texture (0x2EF2C0[n]): 0 npc00 villagers (one
 * model, parts shown per NPC by +0x4E6), 1 npc01 the elder, 2 em09, 3
 * em32; slots 1-3 also load their motions (em_motion_load(n, 10 / 9 /
 * 32)); the villagers use the common (lbcom) motions. The host draws the
 * models (viewer.c), so the model files go to the host's loader. */
static void (*npc_model_fn)(int slot, int amh, int tex);
void rt_set_npc_model_loader(void (*fn)(int slot, int amh, int tex)) { npc_model_fn = fn; }

extern u8 *pl_area_top;
void create_em_motion(int no, int em);
/* em_motion_load (main 0x111AE0): em_motion_data[n] -> create_em_motion */
void em_motion_load(int slot, int n)
{
    static uint8_t *keep[64];
    const uint8_t *p = rt_addr(0x2EC830 + 4 * (uint32_t)n, 4);
    int32_t idx;
    size_t sz;
    if (n < 0 || n >= 64 || !p)
        return;
    memcpy(&idx, p, 4);
    if (!keep[n] && !(keep[n] = rt_file_load(idx, &sz)))
        return;
    pl_area_top = keep[n];
    create_em_motion(slot, n);
}

void npc_create_model(int slot)
{
    static const int motions[4] = { -1, 10, 9, 32 };
    const uint8_t *a = rt_addr(0x2ECED0 + 4 * (uint32_t)slot, 4), *t = rt_addr(0x2EF2C0 + 4 * (uint32_t)slot, 4);
    int32_t amh, tex;
    if (slot < 0 || slot > 3 || !a || !t)
        return;
    memcpy(&amh, a, 4);
    memcpy(&tex, t, 4);
    if (npc_model_fn)
        npc_model_fn(slot, amh, tex);
    if (motions[slot] >= 0)
        em_motion_load(slot, motions[slot]);
}
