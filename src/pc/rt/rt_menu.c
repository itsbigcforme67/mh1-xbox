/*
 * rt_menu.c - what the in-quest menus and HUD (src/main/menu/menu_nm.c,
 * menu_disp_nm.c, src/main/chat/chat_nm.c) call that is not decompiled:
 * the item checks of the g_load_pit asm file (main 0x274E10-0x2755C0),
 * written from the asm here, and online-only parts (chat, soft keyboard,
 * lobby) as no-ops.
 */
#include "rt.h"
#include "types.h"
#include "pl.h"
#include "plf.h"
#include "game.h"

#include <stdio.h>
#include <stdlib.h>


#define PU8(p, o) (*(u8 *)((u8 *)(p) + (o)))
#define PS16(p, o) (*(s16 *)((u8 *)(p) + (o)))

int Modori_dama_ck(void);
long Pl_item_num_ck(PLW *, u16);

/* UseItemChk (0x275230): pouch slot n holds an item (count > 0) whose
 * Item_data kind byte (+1) is 1 (usable) */
int UseItemChk(PLW *pl, u16 n)
{
    s16 id = PS16(pl, 0x828 + 4 * n), num = PS16(pl, 0x82A + 4 * n);
    return num > 0 && id != 0 && Item_data[id][1] == 1;
}

/* Item_valid_chk (0x275290): can item id be used where the master player
 * stands (traps, meat, barrels, pickaxe/net spots, flute, return ball ...) */
int Item_valid_chk(u16 id)
{
    PLW *pl = &player_work[PU8(&game_w, 0xD1)];
    u16 k;
    f32 pos[3];
    u32 w;

    switch (id) {
    case 0xA5:
        return Modori_dama_ck();
    case 0xA2:
        w = *(u32 *)((u8 *)pl + 0x878);
        return w != 0 && *(u16 *)(uintptr_t)(w + 2) == 0x11;
    case 0x86: case 0x87: case 0x88:
        if ((St_pick_ck(pl, &k, pos) & 0xFFFF) == 0xFFFF)
            return 0;
        return (s16)k == 4;
    case 0x83: case 0x84: case 0x85:
        if ((St_pick_ck(pl, &k, pos) & 0xFFFF) == 0xFFFF)
            return 0;
        return (s16)k == 3;
    case 0x17: case 0x18: case 0x16: case 0x12:
        return Niku_ok_ck();
    case 0x20:
        return Taru_ok_ck();
    case 0x6A: case 0x5E: case 0x9B: case 0x69:
        return !(pl->kind == 1 || pl->kind == 5);
    case 0x143: case 0x81:
        return Nikuyaki_ck(pl);
    case 0x1E:
        return Pl_trap_use_ck(pl) >= 0;
    }
    if (Item_data[id][5] != 4 || pl_flag_ck(pl, 0x80000))
        return 1;
    w = *(u32 *)((u8 *)pl + 0x878);
    return w != 0 && *(u16 *)(uintptr_t)(w + 2) == 2;
}

/* Item_ok_chk (0x275540) */
int Item_ok_chk(PLW *pl) { return PU8(pl, 0x8F0) != 0; }

/* Pit_shot_ok_chk (0x275550) */
int Pit_shot_ok_chk(PLW *pl)
{
    if (PU8(pl, 0x8ED))
        return 1;
    if ((s16)act_ck(pl, 0, 0x36) && (s16)Pl_item_num_ck(pl, 0xA2))
        return 1;
    return 0;
}

/* ------------------------------------------------ lobby.bin functions main calls by address
 * (lobby C linked on the PC: the village start menu, Pit_init's lobby set-up) */
void Lb_Menu_Init(void);
void Lb_menu_exit(void);
int Lb_menu_move_Core(void);
void DispLobbyMenu(void);
void Disp_lb_menu(s8);
void *Lb_get_player_id(s32);
void Lobby_quest_print(void);
int Lb_get_pl_stat2(int);
void Lb_ItemBox_init(void);
void func_5B3D70(void) { Lb_Menu_Init(); }
void func_5B3E60(void) { Lb_menu_exit(); }
int func_5B3ED0(void) { return Lb_menu_move_Core(); }
void func_5B4980(void) { DispLobbyMenu(); }
void func_5B4B20(s8 a) { Disp_lb_menu(a); }
void *func_5B4D30(s32 a) { return Lb_get_player_id(a); }
int func_5CB310(void) { Lobby_quest_print(); return 0; }
int func_5D8370(int a) { return Lb_get_pl_stat2(a); }
void func_609750(void) { Lb_ItemBox_init(); }

/* ------------------------------------------------ online / lobby only (no-ops) */
u8 D_6EAC80[0x400];
/* The soft keyboard (main f_sk, src/main/sk/sk_nm.c: the on-screen
 * keyboard with kana/kanji conversion) is not ported. The PC's stand-in
 * is plain typing: SoftKeyboard_set starts host text input (the viewer
 * feeds it, rt_set_text_input), SoftKeyboard_move copies what was typed
 * so far into the caller's buffer as full-width characters (han2zen, as
 * the keyboard writes them) and reports done on Enter or the pad's start
 * button; an empty name then becomes "HUNTER". Scripted runs: RT_NAME=xxx
 * is typed at once. Used by the character screen's name (Edit_task). */
static void (*text_begin_fn)(int on);
static int (*text_take_fn)(char *out, int n);
void rt_set_text_input(void (*begin)(int on), int (*take)(char *out, int n))
{
    text_begin_fn = begin;
    text_take_fn = take;
}
static int sk_on, sk_max;
static char sk_text[40];
void han2zen(u8 *src, u8 *dst);
void SoftKeyboard_set(int type, int mode, int maxlen, char *init)
{
    (void)type; (void)mode;
    sk_on = 1;
    sk_max = maxlen > 0 && maxlen < 16 ? maxlen : 8;
    sk_text[0] = 0;
    if (text_begin_fn)
        text_begin_fn(1);
    (void)init;
}
int SoftKeyboard_move(char *buf, int held, int push)
{
    char in[64];
    int n, i, len, done = 0;
    (void)held;
    if (!sk_on)
        return 1;
    if (getenv("RT_NAME")) {
        snprintf(sk_text, sizeof sk_text, "%.*s", sk_max, getenv("RT_NAME"));
        done = 1;
    } else {
        n = text_take_fn ? text_take_fn(in, sizeof in) : 0;
        for (len = 0; sk_text[len]; len++)
            ;
        for (i = 0; i < n; i++) {
            if (in[i] == '\b') {
                if (len)
                    sk_text[--len] = 0;
            } else if (in[i] == '\n') {
                done = 1;
            } else if (in[i] >= 0x20 && in[i] < 0x7F && len < sk_max) {
                sk_text[len++] = in[i];
                sk_text[len] = 0;
            }
        }
        if (push & 0x8000)
            done = 1;
    }
    if (done && !sk_text[0])
        snprintf(sk_text, sizeof sk_text, "HUNTER");
    han2zen((u8 *)sk_text, (u8 *)in);
    for (i = 0; in[i]; i++)
        buf[i] = in[i];
    buf[i] = 0;
    return done;
}
void SoftKeyboard_pos_set() {}
int SoftKeyboard_exit(void)
{
    if (sk_on && text_begin_fn)
        text_begin_fn(0);
    sk_on = 0;
    return 0;
}
int SoftKeyboard_alive_check() { return sk_on; }
void DispSoftkeyboard() {}
void net_send_chat() {}
void Reibun_print() {}
int Reibun_select_mv() { return 0; }
int func_5BD520() { return 0; }
int func_5CB100() { return 0; }
void func_60CE50() {}
/* tutorial overlay pieces (game.bin 0x63B0C0 / 0x63B470): only in the
 * village tutorial */
int func_63B0C0() { return 0; }
void func_63B470() {}

/* ItemCopy_Pl2Ud / ItemCopy_Ud2Pl (0x272280 / 0x2722A0, as udmisc02.c): the
 * pouch (PLW+0x828, 0x50 bytes of {id, n}) <-> User_data+0x37C. The village copies
 * the user's pouch to the hunter every tick (Lb_move_common) and back after
 * its item menus (lb_menu_item_mv). */
extern u8 User_data[];
void ItemCopy_Pl2Ud(PLW *pl) { __builtin_memcpy((u8 *)User_data + 0x37C, (u8 *)pl + 0x828, 0x50); }
void ItemCopy_Ud2Pl(PLW *pl) { __builtin_memcpy((u8 *)pl + 0x828, (u8 *)User_data + 0x37C, 0x50); }

/* ------------------------------------------------ not ported yet (no-ops) */
#define NOP(name) void name() { static int o; if (!o++ && getenv("RT_TRACE")) fprintf(stderr, "rt_menu: %s not ported\n", #name); }
#define NOP0(name) int name() { static int o; if (!o++ && getenv("RT_TRACE")) fprintf(stderr, "rt_menu: %s not ported\n", #name); return 0; }
NOP(Add_to_Item_preparation_list_0) NOP0(Get_hunter_status)
NOP0(Item_preparation) NOP0(Item_preparation_adrs) NOP0(Item_preparation_list_chk) NOP0(Item_preparation_list_chk_0)
NOP0(Item_preparation_list_num) NOP0(Item_preparation_list_search) NOP0(Item_preparation_one_ck)
NOP0(Item_preparation_rate_0) NOP(Put_sprite_rotate)
NOP(set_viewproj) NOP(SetBlendingMode) NOP0(Get_bowgun_atk) NOP(Draw_square)
/* fonts: rt_font.c */
