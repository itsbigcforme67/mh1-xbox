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
#include "lbsys_g.h"
#include "gwx.h"
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

static int active, ready, tick, all_ticks, last_step = -1, last_x68 = -1, last_6 = -1;

#ifdef MH1_ONLINE
int rt_online_active(void);
int rt_village_active(void) { return active || rt_online_active(); }     /* the online town is drawn as the village */
#else
int rt_village_active(void) { return active; }
#endif

void rt_village_leave(void);

/* Game_task mode 6 (offline): what the PS2 does before Local_main runs */
void rt_motion_em_release(void);
/* what all_reset clears of the world (the village entry; the debug menu's quest start / title) */
void rt_world_reset(void)
{
    memset(em_work, 0, 0xA10 * 20);     /* the quest's monsters are gone (all_reset) */
    rt_motion_em_release();             /* and their motions (all_reset -> em_motion_free; rt_motion.c) */
    clr_set_work();                     /* and the quest stage's set objects (all_reset -> clr_stg_work) */
    {   /* and every effect and shell (all_reset -> init_move_work): a quest's shell kept moving in the village, reading
         * the joints of its cleared monster (shell08_i after quest 140) */
        void rt_eft_shell_reset(void);
        rt_eft_shell_reset();
    }
    {   /* and every prim (all_reset -> prim_init): a set object's prim outlived its cleared work and
         * was drawn in the village (set19_trans on stage 87: crash after a co-op quest) */
        void prim_init(void);
        prim_init();
    }
}

void rt_village_enter(void)
{
    rt_world_reset();
    rt_cam_init(game_w[0x14]);          /* camera work (also with the host's own camera) */
    {
        void rt_lb_reload(void);
        static int visits;
        if (visits++)                   /* Load_overlay(3): lobby.bin data and .bss as loaded */
            rt_lb_reload();
    }
    Clear_lobby_ram();              /* G has no Clear_lobby_ram; its nearest is lobby 0x59A270 (lb_sys, lb_player,
                                         * cw, pNet set-up), which G's Game_task (main 0x239750) calls when Online_ck().
                                         * MH1's on G's memory writes MH1 offsets (and lbSendInterval has no G address):
                                         * skipped until the village code has G copies (docs/mhg.md section 5) */
    game_w[GWX(0x1DC)] = 1;                  /* in the lobby overlay (Game_task step 1): lobby HUD prims, NPC talk sounds */
    if (getenv("RT_VILLAGE_SKIP_INTRO")) {  /* test aid (no save data): the first-visit event seen */
        void Event_flag_set(int);
        Event_flag_set(2);
    }
    if (getenv("RT_EVENT_SET")) {       /* test aid: "1,51,...": event flags set (decimal), as the story would have */
        void Event_flag_set(int);
        const char *e = getenv("RT_EVENT_SET");
        while (*e) {
            char *end;
            long f = strtol(e, &end, 0);
            if (end == e)
                break;
            Event_flag_set((int)f);
            e = *end ? end + 1 : end;
        }
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
    if (getenv("RT_QCLEAR_ALL")) {      /* test aid: the debug menu's "Mark all offline quests cleared" (both games) */
        int rt_dbg_clear_all_quests(void);
        rt_dbg_clear_all_quests();
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
    {   /* test aids (no save data), first visit only: RT_MONEY=n, RT_POUCH="id:n,..." (20 slots), RT_BOX_ITEMS="id:n,..." (item box = User_data+0x1C4, 100 slots),
         * RT_WARE="kind:id,..." (stored equipment: kind 0 weapon .. as the box lists them; User_data+0x44, 6 bytes each) */
        extern u8 User_data[];
        static int done;
        if (!done) {
            const char *q;
            int id, n, used, k;
            done = 1;
            if (getenv("RT_MONEY"))
                *(s32 *)(User_data + 0x20) = atoi(getenv("RT_MONEY"));
            if ((q = getenv("RT_BOX_ITEMS")))
                for (k = 0; k < 100 && sscanf(q, "%i:%i%n", &id, &n, &used) == 2; k++) {
                    *(u16 *)(User_data + UDO(0x1C4) + 4 * k) = (u16)id;
                    *(s16 *)(User_data + UDO(0x1C6) + 4 * k) = (s16)n;
                    q += used;
                    if (*q++ != ',')
                        break;
                }
            if ((q = getenv("RT_POUCH")))       /* the pouch (G: User_data+0xC24, MH1 +0x37C: UDO(0x37C)), 20 slots */
                for (k = 0; k < 20 && sscanf(q, "%i:%i%n", &id, &n, &used) == 2; k++) {
                    *(u16 *)(User_data + UDO(0x37C) + 4 * k) = (u16)id;
                    *(s16 *)(User_data + UDO(0x37E) + 4 * k) = (s16)n;
                    q += used;
                    if (*q++ != ',')
                        break;
                }
            {   /* the village hunter takes name, pouch and equipment from the save, as init_pl_work does (the status page printed an empty name line) */
                extern void Set_userdata(void *);
                if (!User_data[8] && getenv("RT_NAME"))     /* a test hunter without a save: the name the scripts give (RT_NAME), as rt_online does */
                    snprintf((char *)User_data + 8, 0x10, "%s", getenv("RT_NAME"));
                if (!User_data[0] && User_data[8])  /* "slot in use", as user_data_copy sets it at character creation: without it a */
                    User_data[0] = 1;               /* bed save of the test hunter loads as "no character data" (CardConld03) */
                Set_userdata(player_work + 0xA00 * game_w[GWX(0xD1)]);
                if (getenv("RT_QUEST_TRACE"))
                    fprintf(stderr, "rt_village: hunter name '%s' (save: '%s')\n", (char *)(player_work + 0xA00 * game_w[GWX(0xD1)] + 0x8D4), (char *)User_data + 8);
            }
            if ((q = getenv("RT_WARE")))
                for (k = 0; k < 64 && sscanf(q, "%i:%i%n", &n, &id, &used) == 2; k++) {
                    User_data[0x44 + 6 * k] = 1;
                    User_data[0x45 + 6 * k] = (u8)n;
                    *(u16 *)(User_data + 0x46 + 6 * k) = (u16)id;
                    q += used;
                    if (*q++ != ',')
                        break;
                }
        }
    }
    lb_sys[LBSYS_OFS(3)] = 0;                      /* vs_square_init_pre */
    lb_sys[LBSYS_OFS(4)] = 0;
    active = 1;
    ready = 0;
    tick = 0;
    if (getenv("RT_QUEST_TRACE")) {
        extern u8 User_data[];
        fprintf(stderr, "rt_village: enter (Clear_lobby_ram, Local_main from step 0), money %d\n", *(s32 *)(User_data + 0x20));
    }
}

/* the village is running: Local_main has reached its main step (4: the stage, the hunter and the NPCs are set up). The
 * referee's village play ticks count from here (rt_ref.c), as the PS2's start in a village that is already running. */
int rt_village_ready(void) { return active && ready; }

/* test aid RT_VILLAGE_ACCEPT=N (MH1): quest N accepted at the counter, as the referee's gate save state has it (the
 * "depart" prompt at the gate, the quest flag over the hunter): what lb_u.c's offline accept sets, without the fee (the
 * PS2's User_data already paid it). G's quest table is laid out differently: not done there. */
#ifndef VERSION_MHG
static void village_accept(int q)
{
    extern u8 *cw;
    extern u8 D_3F360A[];
    extern s16 D_3F33DC[];
    extern s32 quest_price;
    extern u8 *lb_quest_all[];
    void Lb_menu_quest_info();
    signed char Lb_get_quest_type();
    u8 *qq = q > 0 && q < 0xC8 ? lb_quest_all[q] : NULL;
    if (!cw || !qq) {
        fprintf(stderr, "rt_village: RT_VILLAGE_ACCEPT %d: no such quest in the village's table\n", q);
        return;
    }
    cw[0x35D3] = 1;
    cw[0x32C5] = 1;
    D_3F360A[0] = 1;
    D_3F33DC[0] = (s16)q;
    quest_price = *(s32 *)(qq + 4);
    Lb_menu_quest_info(qq);
    {   /* the quest flag over the hunter: PLW+0x916 and the hunter's mini data (name plate) the lobby draws it from */
        extern u8 lbCommer[];
        void Lb_set_mini_data();
        int a = game_w[GWX(0xD1)], v = (u8)(Lb_get_quest_type(qq) | 0x40);
        player_work[0xA00 * a + 0x916] = (u8)v;
        Lb_set_mini_data(cw + a * 0x2FC + 0x1346, v, a);
        memcpy(lbCommer + a * 0x5C + 0x1C, cw + a * 0x2FC + 0x1346, 0x40);
    }
    *(s16 *)(select_w + 0xAC) = (s16)q;
    fprintf(stderr, "rt_village: quest %d accepted (RT_VILLAGE_ACCEPT)\n", q);
}
#endif

/* One village tick. Returns the quest number when Local_main reports an
 * accepted quest (1), -1 when it leaves the village another way, else 0. */
int rt_village_tick(void)
{
    s32 r;
    if (!ready && (s8)lb_sys[LBSYS_OFS(3)] == 4) {
        ready = 1;
#ifndef VERSION_MHG
        if (getenv("RT_VILLAGE_ACCEPT"))
            village_accept(atoi(getenv("RT_VILLAGE_ACCEPT")));
#endif
    }
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
    if (getenv("RT_DEMO_NOWALL")) {        /* debug aid: the demo camera's wall push-out (G only) off */
        extern u8 CameraWork[];
        CameraWork[4 * 0x100 + 0x80 + 0x66] = 0;
    }
    r = Local_main();
    if (getenv("RT_CAM_TRACE") && tick % 10 == 0) {     /* the view the village camera made (eye, target, roll, fov) every 10 ticks */
        void rt_cam_view(float eye[3], float tar[3], float *roll, float *fov);
        extern u8 CameraWork[];
        u8 *dm = CameraWork + 4 * 0x100 + 0x80;     /* slot 4's demo work (G) */
        float e[3], t[3], ro, fv;
        rt_cam_view(e, t, &ro, &fv);
        float *m = (float *)(player_work + 0x60 + 0xA00 * game_w[GWX(0xD1)]);
        fprintf(stderr, "vcam: tick %d eye %.0f %.0f %.0f tar %.0f %.0f %.0f roll %.3f fov %.3f | demo no %d st %d pos %d tar %d move %d x66 %d\n", tick, e[0], e[1], e[2], t[0], t[1], t[2], ro, fv,
                dm[0x4E], (s8)dm[0x4F], dm[0x49], dm[0x4A], dm[0x48], dm[0x66]);(void)m;
        {
            void rt_cam_dump(int);
            rt_cam_dump(tick);
        }
    }
    if (getenv("RT_VILLAGE_LEAVE")) {   /* test aid "tick,quest[,title]": leave for that quest at that village tick
                                         * (all ticks), as if accepted at the counter (which sets quest_title, shown on
                                         * the loading screen) and left by the gate */
        extern char quest_title[];
        int t = -1, q = 0, n = 0;
        if (sscanf(getenv("RT_VILLAGE_LEAVE"), "%d,%d%n", &t, &q, &n) >= 2 && t == all_ticks) {
            const char *title = getenv("RT_VILLAGE_LEAVE") + n;
            *(s16 *)(select_w + (rt_ver_mhg() ? 0xD0 : 0xAC)) = (s16)q;
            if (*title == ',')
                snprintf(quest_title, 32, "%s", title + 1);  /* (0x20 bytes) */
            r = 1;
        }
    }
    if (getenv("RT_QUEST_TRACE")) {     /* the saved hunter's data when it changes: money, pouch, item box, stored equipment, worn gear */
        extern u8 User_data[];
        static u8 last[0xE00];
        static int first = 1;
        if (first || memcmp(last, User_data, rt_ver_mhg() ? 0xE00 : 0x460)) {
            int k;
            first = 0;
            memcpy(last, User_data, rt_ver_mhg() ? 0xE00 : 0x460);
            fprintf(stderr, "rt_village: tick %d ud money %d pouch:", tick, *(s32 *)(User_data + 0x20));
            for (k = 0; k < 20; k++)
                if (*(u16 *)(User_data + UDO(0x37C) + 4 * k))
                    fprintf(stderr, " %d:%d", *(u16 *)(User_data + UDO(0x37C) + 4 * k), *(s16 *)(User_data + UDO(0x37E) + 4 * k));
            fprintf(stderr, " | box:");
            for (k = 0; k < (rt_ver_mhg() ? 300 : 100); k++)
                if (*(u16 *)(User_data + UDO(0x1C4) + 4 * k))
                    fprintf(stderr, " %d:%d", *(u16 *)(User_data + UDO(0x1C4) + 4 * k), *(s16 *)(User_data + UDO(0x1C6) + 4 * k));
            fprintf(stderr, " | ware:");
            for (k = 0; k < (rt_ver_mhg() ? 300 : 64); k++)
                if (User_data[0x44 + 6 * k])
                    fprintf(stderr, " %d/%d/%d", User_data[0x45 + 6 * k], *(u16 *)(User_data + 0x46 + 6 * k), *(u16 *)(User_data + 0x48 + 6 * k));
            fprintf(stderr, " | wear: w%d/%d/%x a %d %d %d %d %d | time %u\n", User_data[UDO(0x3CD)], *(u16 *)(User_data + UDO(0x3CE)), *(u16 *)(User_data + UDO(0x3D0)),
                    User_data[UDO(0x3D2)], User_data[UDO(0x3D3)], User_data[UDO(0x3D4)], User_data[UDO(0x3D5)], User_data[UDO(0x3D6)],
                    *(u32 *)(User_data + (rt_ver_mhg() ? 0xC1C : 0x374)));     /* play time in seconds (the save screen's TIME, mcdisp_nm.c) */
            fprintf(stderr, "rt_village: tick %d event flags %04x %04x %04x %04x %04x %04x %04x %04x\n", tick, *(u16 *)(User_data + 0x24), *(u16 *)(User_data + 0x26),
                    *(u16 *)(User_data + 0x28), *(u16 *)(User_data + 0x2A), *(u16 *)(User_data + 0x2C), *(u16 *)(User_data + 0x2E), *(u16 *)(User_data + 0x30), *(u16 *)(User_data + 0x32));
        }
    }
    static int spots_stage = -1;
    if (getenv("RT_VILLAGE_TRACE") && game_w[0x14] != spots_stage && lb_sys[LBSYS_OFS(3)] == 4) {     /* the stage's unique spots (exits, chairs ...) */
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
    if (getenv("RT_VILLAGE_TRACE") && (tick % 30 == 0 || (getenv("RT_VT_FINE") && tick >= atoi(getenv("RT_VT_FINE")) && tick < atoi(getenv("RT_VT_FINE")) + 40))) {
        extern u8 CameraWork[];     /* the demo camera slot (sl[4]): mode, step, no, state */
        u8 *dc = CameraWork + 0x80 + 4 * 0x100;
        fprintf(stderr, "rt_village: tick %d hint0 '%.20s' hint1 '%.20s'\n", tick, (char *)lb_sys + LBSYS_OFS(0xA), (char *)lb_sys + LBSYS_OFS(0x28));
        fprintf(stderr, "rt_village: tick %d demo cam mode %d step %d no %d state %d req %d\n", tick, dc[0x78], dc[0x7C], dc[0x90 + 0x4E], (s8)dc[0x90 + 0x4F], CameraWork[0x5F4]);
        float *p = (float *)(player_work + 0xAC), e[3], t[3], roll, fov;
        fprintf(stderr, "rt_village: tick %d pl x05 %d stat %d chr %d frame %.1f spd %.2f loopfr %.1f end %.1f loop %d\n", tick, player_work[5], *(s32 *)(player_work + 0x194), *(s32 *)(player_work + 0x198), *(float *)(player_work + 0x19C), *(float *)(player_work + 0x1A0), *(float *)(player_work + 0x1A4), *(float *)(player_work + 0x1A8), *(s32 *)(player_work + 0x1AC));
        rt_cam_view(e, t, &roll, &fov);
        fprintf(stderr, "rt_village: stage_work %d %d mdls %p\n", stage_work[0], stage_work[1], *(void **)(stage_work + 0x3C));
        fprintf(stderr, "rt_village: tick %d pl %.0f %.0f %.0f ang %04X act %d/%d char %d | cam %.0f %.0f %.0f -> %.0f %.0f %.0f fov %.2f roll %.2f\n",
                tick, p[0], p[1], p[2], *(u16 *)(player_work + 0xE), player_work[0x14], player_work[0x15],
                *(u16 *)(player_work + 0x2DC), e[0], e[1], e[2], t[0], t[1], t[2], fov, roll);
    }
    if (getenv("RT_VILLAGE_TRACE") && *(s32 *)(lb_sys + LBSYS_OFS(0x68)) != 0 && tick % 10 == 0) {
        extern u8 lb_pit[];
        fprintf(stderr, "rt_village: tick %d talk %d/%d pit x0 %d pos %p page %d sel %d done %d\n", tick, (s8)lb_sys[LBSYS_OFS(6)], (s8)lb_sys[LBSYS_OFS(7)],
                *(s32 *)lb_pit, *(void **)(lb_pit + 4), (s8)lb_pit[8], (s8)lb_pit[9], (s8)lb_pit[0xB]);
    }
    if (getenv("RT_FH_TRACE") && (tick % 10 == 0 || getenv("RT_FH_EVERY"))) {      /* the area record under the hunter (player+0x878): kind at +2 */
        u8 *pl = player_work + 0xA00 * game_w[GWX(0xD1)];
        u8 *fh = *(u8 **)(pl + 0x878);
        fprintf(stderr, "rt_village: tick %d fh %p kind %d pos %.0f %.0f\n", tick, (void *)fh, fh ? *(u16 *)(fh + 2) : -1,
                *(float *)(pl + 0xAC), *(float *)(pl + 0xB4));
    }
    if (getenv("RT_QUEST_TRACE") && *(s32 *)(lb_sys + LBSYS_OFS(0x68)) != 0 && lb_sys[LBSYS_OFS(6)] != last_6) {
        extern u8 lb_pit[];
        fprintf(stderr, "rt_village: tick %d talk state %d (x68 %d) page %d sel %d\n", tick, (s8)lb_sys[LBSYS_OFS(6)],
                *(s32 *)(lb_sys + LBSYS_OFS(0x68)), (s8)lb_pit[8], (s8)lb_pit[9]);
        last_6 = lb_sys[LBSYS_OFS(6)];
    }
    if (getenv("RT_QUEST_TRACE") && (lb_sys[LBSYS_OFS(3)] != last_step || *(s32 *)(lb_sys + LBSYS_OFS(0x68)) != last_x68)) {
        float *p = (float *)(player_work + 0xAC);
        extern u8 *cw;
        fprintf(stderr, "rt_village: tick %d step %d sub %d x68 %d stage %d pl %.0f %.0f %.0f act %d/%d quest %d (accepted %d)\n",
                tick, (s8)lb_sys[LBSYS_OFS(3)], (s8)lb_sys[LBSYS_OFS(4)], *(s32 *)(lb_sys + LBSYS_OFS(0x68)), game_w[0x14],
                p[0], p[1], p[2], player_work[0x14], player_work[0x15], *(s16 *)(select_w + (rt_ver_mhg() ? 0xD0 : 0xAC)), cw ? cw[rt_ver_mhg() ? 0xFA5F : 0x35D3] : -1);
        if (0) {
            int i;
            for (i = 0; i < 20; i++) {
                u8 *e = em_work + 0xA10 * i;
                if (e[0] && e[0x1E])
                    fprintf(stderr, "rt_village:   npc slot %d kind %d type %d talk %d pos %.0f %.0f %.0f\n", i, e[2], e[0x1B],
                            e[0x452], *(float *)(e + 0xAC), *(float *)(e + 0xB0), *(float *)(e + 0xB4));
            }
        }
        last_step = lb_sys[LBSYS_OFS(3)];
        last_x68 = *(s32 *)(lb_sys + LBSYS_OFS(0x68));
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
#ifdef VERSION_MHG
#define LBS(o) lbShop[(o) == 0x14 ? 0x8E : (o) == 0x15 ? 0x8F : (o) == 0x19 ? 0x93 : 0x97]   /* include/lbshop_g.h */
#else
#define LBS(o) lbShop[o]
#endif
        static int last = -1, lastk = -1, lastc = -1;
#ifdef VERSION_MHG
#define LBW(o) (*(s32 *)(lbShop + ((o) == 0x80 ? 0x78 : (o) == 0x74 ? 0x70 : (o) == 0x64 ? 0x64 : (o) == 0x50 ? 0x50 : 0x4C)))
#else
#define LBW(o) (*(s32 *)(lbShop + (o)))
#endif
        int v = LBS(0x14) | LBS(0x15) << 8 | LBS(0x19) << 16 | LBS(0x1B) << 24;
        if (v != last || (u32)*(u16 *)(lbShop + LBSHOP_OFS(0x8C)) != (u32)lastk || LBW(0x74) != lastc)
            fprintf(stderr, "rt_village: tick %d shop step %d sub %d mode %d x1B %d x68 %d | count %d cur %d tbl %08x help %d list %08x key %04x item0 price %d state %d name %.12s\n", tick, (s8)LBS(0x14),
                    (s8)LBS(0x15), (s8)LBS(0x19), LBS(0x1B), *(s32 *)(lb_sys + LBSYS_OFS(0x68)), LBW(0x80), LBW(0x74), LBW(0x64), LBW(0x4C),
                    (u32)LBW(0x50), (u32)*(u16 *)(lbShop + LBSHOP_OFS(0x8C)), LBW(0x50) ? *(s32 *)LBW(0x50) : 0, LBW(0x50) ? *(s16 *)((u8 *)LBW(0x50) + 0x24) : 0, LBW(0x50) ? (char *)LBW(0x50) + 4 : "");
        last = v;
        lastk = *(u16 *)(lbShop + LBSHOP_OFS(0x8C));
        lastc = LBW(0x74);
    }
    if (r == 1 || r == -1) {
        rt_village_leave();
        return r == 1 ? *(s16 *)(select_w + (rt_ver_mhg() ? 0xD0 : 0xAC)) : -1;     /* the quest number (G +0xD0) */
    }
    return 0;
}

/* Leaving the village (Local_main's end, or the debug menu starting a quest / the title from here) */
void rt_village_leave(void)
{
    void com_motion_load(int n);
    if (!active)
        return;
    active = 0;
    ready = 0;
    game_w[GWX(0x1DC)] = 0;
    rt_motion_em_release();         /* the NPCs' motions go with the village */
    com_motion_load(0);             /* the quest's common motions (plcom_tbl) again */
}

/* com_motion_load (main 0x111A80): the common motion set n
 * (common_motion_data, 0x2EBC70: 0 plcom_tbl, 1 lbcom_tbl the village's,
 * 2 selcom_tbl) through load_plcom_motion + create_plcom_motion. The files
 * stay loaded while motion sets point into them (rt_village_files_gc). */
uint8_t *rt_file_load(int idx, size_t *n);
void rt_motion_load_plcom(const uint8_t *tbl);
int rt_motion_refs(const void *p, size_t n);
static struct kept { uint8_t *p; size_t n; } com_keep[3], em_keep[64];
/* The motion files above whose motion sets rt_motion_gc has freed, freed too (at each stage
 * load, rt_flow.c; loaded again when the game asks for them): on the PS2 the quest's player
 * motions (plcom, 1 MB) and the village's (lbcom, 1 MB) take turns in one area, and the NPC
 * motion files go with the village. Here all of them stayed loaded (docs/xbox.md memory). */
void rt_village_files_gc(void)
{
    struct kept *k[2] = { com_keep, em_keep };
    int nk[2] = { 3, 64 }, a, i;
    for (a = 0; a < 2; a++)
        for (i = 0; i < nk[a]; i++)
            if (k[a][i].p && !rt_motion_refs(k[a][i].p, k[a][i].n)) {
                if (getenv("RT_QUEST_TRACE"))
                    fprintf(stderr, "rt_village: %s motion file %d freed (%zu bytes)\n", a ? "monster/NPC" : "common", i, k[a][i].n);
                free(k[a][i].p);
                k[a][i].p = NULL;
            }
}

void com_motion_load(int n)
{
    struct kept *keep = com_keep;
    const uint8_t *p = rt_addr(RTVA(0x2EBC70) + 4 * (uint32_t)n, 4);
    int32_t idx;
    if (n < 0 || n > 2 || !p)
        return;
    memcpy(&idx, p, 4);
    if (!keep[n].p && !(keep[n].p = rt_file_load(idx, &keep[n].n))) {
        fprintf(stderr, "rt_village: common motions %d (AFS %d) not loaded\n", n, idx);
        return;
    }
    rt_motion_load_plcom(keep[n].p);
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

void rt_em_motion_create(int slot, int kind, const uint8_t *tbl);
/* em_motion_load (main 0x111AE0): em_motion_data[n] -> create_em_motion */
void em_motion_load(int slot, int n)
{
    struct kept *keep = em_keep;
    const uint8_t *p = rt_addr(RTVA(0x2EC830) + 4 * (uint32_t)n, 4);
    int32_t idx;
    if (n < 0 || n >= 64 || !p)
        return;
    memcpy(&idx, p, 4);
    if (!keep[n].p && !(keep[n].p = rt_file_load(idx, &keep[n].n)))
        return;
    rt_em_motion_create(slot, n, keep[n].p);
}

void npc_create_model(int slot)
{
    static const int motions[4] = { -1, 10, 9, 32 };
    const uint8_t *a = rt_addr(RTVA(0x2ECED0) + 4 * (uint32_t)slot, 4), *t = rt_addr(RTVA(0x2EF2C0) + 4 * (uint32_t)slot, 4);
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
