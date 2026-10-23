/*
 * rt_quest.c - the quest flow on the port runtime: what f_quest
 * (src/main/quest: qstb*.c, f_quest*.c, f_quest_nm.c), the game modes
 * (src/main/game/f_game.c) and the result / reward screens call that is
 * not decompiled yet. Written from the asm where noted; offline only (the
 * network calls do nothing).
 */
#include "rt.h"
#include "types.h"
#include "em.h"
#include "game.h"
#include "quest.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WEAK __attribute__((weak))
#define PU8(p, o) (*(u8 *)((u8 *)(p) + (o)))
#define PS16(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define PU16(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define PU32(p, o) (*(u32 *)((u8 *)(p) + (o)))

static int qtrace(void) { static int t = -1; if (t < 0) t = getenv("RT_QUEST_TRACE") != NULL; return t; }

/* mission_area (0x38A208): where Quest_start loads the mission file
 * (questName[no], Meltw). 256 KB host buffer. */
u8 *mission_area;
static u8 mission_buf[0x40000] __attribute__((aligned(16)));
void rt_quest_mem_init(void) { mission_area = mission_buf; }

/* ------------------------------------------------ user data (main ud, 0x272xxx) */


/* ------------------------------------------------ network (offline) */
#ifndef MH1_ONLINE     /* ONLINE=1: the game's (netsyn08.c, co-op) */
void net_send_sys(int kind, int pl) { (void)kind; (void)pl; }
#endif

/* ------------------------------------------------ monster models (main 0x124820)
 * em_create_model(slot) loads the model of kind game_w+0x28[slot] into
 * model slot `slot`; release_enemy_model frees it. The host loads its
 * models itself (viewer.c), so these only log. A kind whose monster
 * program (em_prog_tbl, game.bin) is not ported yet gets no model slot
 * (game_w+0x28[slot] back to 0), so Em_direct_set does not spawn it. */
extern void *em_prog_tbl[];
static void (*em_model_fn)(int slot, int kind);
void rt_set_em_model_loader(void (*fn)(int slot, int kind)) { em_model_fn = fn; }
void em_create_model(int slot)
{
    int kind = PU8(&game_w, 0x28 + slot);
    {   /* test aid RT_QEM_KIND "from:to" (rt_em.c): the model slot of kind `from` holds kind `to` */
        int from, to;
        if (getenv("RT_QEM_KIND") && sscanf(getenv("RT_QEM_KIND"), "%d:%d", &from, &to) == 2 && kind == from)
            PU8(&game_w, 0x28 + slot) = (u8)(kind = to);
    }
    if (!em_prog_tbl[kind] || !((void **)em_prog_tbl[kind])[0] || !((void **)em_prog_tbl[kind])[3]) {
        /* [0] local init and [3] the per-tick effect script (em_effect_move)
         * are called unconditionally: both must be ported */
        if (qtrace())
            fprintf(stderr, "rt_quest: monster kind %d not ported, not spawned\n", kind);
        PU8(&game_w, 0x28 + slot) = 0;
        return;
    }
    if (qtrace())
        fprintf(stderr, "rt_quest: em_create_model slot %d kind %d\n", slot, kind);
    if (em_model_fn)
        em_model_fn(slot, kind);      /* the host's model and the motions of the slot */
    rt_snd_em_add(kind);              /* its sounds (port 6) */
}
void release_enemy_model(int slot)
{
    if (qtrace())
        fprintf(stderr, "rt_quest: release_enemy_model slot %d\n", slot);
}

/* ------------------------------------------------ game.bin calls by address */
int Tutorial_quest_ck(void);
void Tutorial_prog(void);
void em_act_set(EMW *em, int kind, u16 no);
int func_63AF40(void) { return Tutorial_quest_ck(); }
void func_63ACA0(void) { Tutorial_prog(); }
void func_535D20(EMW *em, int kind, int no) { em_act_set(em, kind, (u16)no); }
#ifdef VERSION_MHG
/* G's quest start (f_quest.c, VERSION_MHG): Tutorial_quest_ck by its game.bin address; main
 * 0x12D170 / 0x12D1B0 (below); sub_main 0x4F3B00 (written
 * from the asm: a training quest listed in quest_size_flag_tbl {event flag, ?, quest} allows random
 * monster sizes only once its event flag is set; any other quest allows them). */
int func_66E180(void) { return Tutorial_quest_ck(); }
/* main 0x12D170 / 0x12D1B0 (G-only, written from the asm): system_w bytes 0x43 and 0x10 (G 0x35F013 / 0x35EFE0; this
 * read game_w+0x2F3 / +0x2C0 before, the same G addresses but past the end of the host's game_w, and tested 0x10 the
 * wrong way round: in test runs it returned 1 from whatever followed game_w, so the quest result put the pouch back
 * and skipped the item sale; agent A, 15 Oct 2026). 0x12D170 is 1 when 0x43 is set and the network flag 0x10 is not;
 * the earlier note here: G's GAME MENU end (mode_sel_end, main 0x1EF060) sets 0x43 for its choices 2.., which the PC
 * does not port, so 0x12D170 stays 0 on the PC [inferred]. Its callers treat 1 as a practice mode: the pouch comes back
 * after the quest, no money on a failed quest, no item sale (result_prog), loadout from the hunter (Get_equip_value). */
int func_0012D170(void)
{
    return PU8(&system_w, 0x43) != 0 && PU8(&system_w, 0x10) == 0;
}
int func_0012D1B0(void)
{
    return PU8(&system_w, 0x44) != 0;
}
int Event_flag_ck(int);
extern s16 quest_size_flag_tbl[];
int sub_main_func_004F3B00(int no)
{
    s16 *p;
    for (p = quest_size_flag_tbl; p[0] != -1; p += 3)
        if (p[2] == no)
            return Event_flag_ck(p[0]) == 1;
    return 1;
}

/* G monster program entries that had no host function, so em_create_model refused the kind (no Velociprey,
 * Genprey, Ioprey, Hornetaur-type kinds 3/4/5/32 spawned in G: swarm quests 19, 31, 10018 never cleared).
 * game 0x5B7010 (em03's local init / move entries [0] [1]): a tail call eft01_set(em, 0). */
void eft01_set();
void game_func_005B7010(EMW *em)
{
    eft01_set(em, 0);
}
/* sub_main 0x4EDCD0 (em04's effect move, kinds 4/5/32): step 0 -> 1, then the sound script sub_main 0x4ECD00 and,
 * when the slot-2 overlay is number 2 (func_001F19C0(2): only G's gallery event scenes), the Eft13 dust script
 * 0x4EDAA0. Both are ef_move_sub_0058E500 in em04_nm.c (its VERSION_MHG parts: kind 32's sound, dust behind G_DUST). */
struct EM04W;
void ef_move_sub_0058E500(EMW *em, struct EM04W *w);
void sub_main_func_004EDCD0(EMW *em)
{
    u8 *w = (u8 *)em + 0x444;
    switch (*w) {
    case 0:
        *w = 1;
        break;
    case 1:
        ef_move_sub_0058E500(em, (struct EM04W *)w);
        break;
    }
}
#endif
/* Bdora_hp_ck (0x53B5C0, em_master: Fatalis' hit points), Fish_set
 * (0x5589F0: a stage's fish, eft23) and Em09_item_sub (0x5A8170) */
int Bdora_hp_ck(void);
void Em09_item_sub(EMW *em);
void Eft23_set(int arg, f32 *pos);
int func_53B5C0(void) { return Bdora_hp_ck(); }
void func_5A8170(void *e) { Em09_item_sub((EMW *)e); }
/* Fish_set (game.bin 0x5589F0; matched C in eft23b.c, which the PC does
 * not build because eft23_nm.c holds Eft23_set too): every fish of the
 * stage's spot table Fish_hani_tbl[stage] (0x18-byte entries {pos, range,
 * kind (<0 ends), count}) */
extern s32 *Fish_hani_tbl[];
void func_5589F0(int stage)
{
    s32 *sp = Fish_hani_tbl[stage];
    int n;
    for (; sp && sp[4] >= 0; sp += 6)
        for (n = sp[5]; n > 0; n--)
            Eft23_set(sp[4], (f32 *)sp);
}
/* Lb_get_quest_str2 (lobby 0x5C5E20): online quest names */
static char empty_str[1];
char *func_5C5E20(void) { return empty_str; }

/* ------------------------------------------------ the HUD ("pit", main f_menu)
 * Set-up as the game does when a quest's stage is entered: load_pit (HUD
 * textures, rt_2d.c), PitWork_init / Pit_init (menu_nm.c), the info
 * banner (Info_Initialization, set01.c). Each tick Pit_mv (the last step
 * of move(), 0x1265E0) queues the three HUD layers; rt_game_draw_2d draws
 * them. */
void rt_2d_init(void);
void load_pit(void);
void PitWork_init(void);
void Pit_init(void);
void Pit_mv(void);
void Info_Initialization(void);
void rt_font_init(void);
void SoftKeyboard_init(void);
extern unsigned char *lpSKey;
void rt_hud_init(void)
{
    if (!lpSKey)
        SoftKeyboard_init();       /* Pit_init calls SoftKeyboard_exit */
    rt_2d_init();
    rt_font_init();
    load_pit();
    PitWork_init();
    Pit_init();
    if (Tutorial_quest_ck()) {  /* stage_init's next step (flow01.c: Pit_init, SonchoInit 0x63B020, Q_camera_init): the
                                 * Elder's lessons. The PS2 runs it for every quest and then indexes tuto_flag_tbl with
                                 * (quest - first training quest), far past the table for the others; only training
                                 * quests use it (game_w.x1E7, from the same check), so the PC skips it there */
        void SonchoInit(void);
        SonchoInit();
    }
    Info_Initialization();
}
void rt_hud_tick(void)
{
    Pit_mv();
}

/* ------------------------------------------------ money (main ud / f_reward) */
extern s32 quest_price;
void Gold_add(int n);    /* f_ud.c */
/* Quest_price_return (0x290E50): give back the quest fee once */
void Quest_price_return(void)
{
    if (quest_price != 0) {
        Gold_add(quest_price);
        quest_price = 0;
    }
}

#ifdef VERSION_MHG
/* The village story quests whose monsters play a scripted scene the first time (em*_main: "quest Q on stage S while
 * event flag F is clear: act 6/n"). MH1's monster code tests MH1's quest numbers (0x87 ..); G moved those quests to
 * 10005 .. (read from G's em*_main, agent D 21 Oct 2026). This gives the MH1 number for G's quest, so the shared C
 * keeps its tests; any other quest gives -1. G's new scenes (10008, 10028, 10035, 10046 ...) are written out in the
 * em*_main themselves. */
int rt_story_quest(void)
{
    switch (*(u16 *)((u8 *)&quest_w + 8)) {
    case 10005: return 0x87;
    case 10006: return 0x88;
    case 10014: return 0x89;
    case 10010: return 0x8A;
    case 10032: return 0x8B;
    case 10015: return 0x94;
    case 10041: return 0xAB;
    }
    return -1;
}
#endif
