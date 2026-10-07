/*
 * rt_quest.c - the quest flow on the port runtime: what f_quest
 * (src/main/quest/f_quest0_nm.c, f_quest_nm.c), the game modes
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

extern u32 h_rank_tbl[];
/* Event_flag_set / _clear / _ck (0x272EC0..): u16 flags at User_data+0x24 */
void Event_flag_set(int n) { PU16(&User_data, 0x24 + 2 * (n / 16)) |= (u16)(1 << (n & 0xF)); }
void Event_flag_clear(int n) { PU16(&User_data, 0x24 + 2 * (n / 16)) &= (u16)~(1 << (n & 0xF)); }
int Event_flag_ck(int n) { return (PU16(&User_data, 0x24 + 2 * (n / 16)) & (1 << (n & 0xF))) != 0; }
/* Get_hunter_rank (0x272320): number of h_rank_tbl entries (ended by
 * 9999999) that the hunter points (u+0x1C) reach */
u8 Get_hunter_rank(u8 *u)
{
    u32 pts = PU32(u, 0x1C), *t = h_rank_tbl;
    u8 n = 0;
    if (*t == 0x98967F)
        return 0;
    for (; pts >= *t; ) {
        t++;
        n++;
        if (*t == 0x98967F)
            break;
    }
    return n;
}

/* ------------------------------------------------ network (offline) */
void net_send_sys(int kind, int pl) { (void)kind; (void)pl; }

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
/* RT_MIXTEST=1: list the item combining recipes the game's own code finds (item_nm.c) */
void *Item_preparation_adrs(short a, short b);
static void mix_test(void)
{
    int a, b, n = 0;
    for (a = 1; a < 0x100; a++)
        for (b = a; b < 0x100; b++) {
            short *e = Item_preparation_adrs((short)a, (short)b);
            if (e) {
                n++;
                fprintf(stderr, "mix: %d + %d -> %d (rate idx %d)\n", a, b, e[1], ((signed char *)e)[4]);
            }
        }
    fprintf(stderr, "mix: %d recipes\n", n);
}
void rt_hud_init(void)
{
    if (getenv("RT_MIXTEST"))
        mix_test();
    if (!lpSKey)
        SoftKeyboard_init();       /* Pit_init calls SoftKeyboard_exit */
    rt_2d_init();
    rt_font_init();
    load_pit();
    PitWork_init();
    Pit_init();
    Info_Initialization();
}
void rt_hud_tick(void)
{
    Pit_mv();
}

/* ------------------------------------------------ money (main ud / f_reward) */
extern s32 quest_price;
/* Gold_add (0x2722C0): User_data+0x20 money, clamped to 0..9999999 */
void Gold_add(int n)
{
    s32 *g = (s32 *)((u8 *)&User_data + 0x20);
    *g += n;
    if (*g >= 10000000)
        *g = 9999999;
    if (*g < 0)
        *g = 0;
    if (getenv("RT_QUEST_TRACE"))
        fprintf(stderr, "rt_quest: Gold_add(%d) -> money %d\n", n, *g);
}
/* Quest_price_return (0x290E50): give back the quest fee once */
void Quest_price_return(void)
{
    if (quest_price != 0) {
        Gold_add(quest_price);
        quest_price = 0;
    }
}
