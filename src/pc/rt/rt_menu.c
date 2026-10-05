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

/* ------------------------------------------------ online / lobby only (no-ops) */
u8 D_6EAC80[0x400];
void SoftKeyboard_set() {}
void SoftKeyboard_move() {}
void SoftKeyboard_pos_set() {}
void SoftKeyboard_exit() {}
int SoftKeyboard_alive_check() { return 0; }
void DispSoftkeyboard() {}
void net_send_chat() {}
void Reibun_print() {}
int Reibun_select_mv() { return 0; }
void func_5B3D70() {}
void func_5B3E60() {}
void func_5B3ED0() {}
void func_5B4980() {}
void func_5B4B20() {}
void func_5B4D30() {}
int func_5BD520() { return 0; }
int func_5CB100() { return 0; }
int func_5CB310() { return 0; }
int func_5D8370() { return 0; }
void func_609750() {}
void func_60CE50() {}
/* tutorial overlay pieces (game.bin 0x63B0C0 / 0x63B470): only in the
 * village tutorial */
int func_63B0C0() { return 0; }
void func_63B470() {}

/* ------------------------------------------------ not ported yet (no-ops) */
#define NOP(name) void name() { static int o; if (!o++ && getenv("RT_TRACE")) fprintf(stderr, "rt_menu: %s not ported\n", #name); }
#define NOP0(name) int name() { static int o; if (!o++ && getenv("RT_TRACE")) fprintf(stderr, "rt_menu: %s not ported\n", #name); return 0; }
NOP(Add_to_Item_preparation_list_0) NOP(DispFrameMessageA) NOP0(Get_hunter_status) NOP(ItemCopy_Pl2Ud)
NOP0(Item_preparation) NOP0(Item_preparation_adrs) NOP0(Item_preparation_list_chk) NOP0(Item_preparation_list_chk_0)
NOP0(Item_preparation_list_num) NOP0(Item_preparation_list_search) NOP0(Item_preparation_one_ck)
NOP0(Item_preparation_rate_0) NOP0(ListSelect) NOP0(Menu_select_mv) NOP0(PageSelect) NOP(Put_sprite_rotate)
NOP(set_viewproj) NOP(SetBlendingMode) NOP(Put_comment) NOP0(Get_bowgun_atk) NOP(Draw_square)
/* fonts: rt_font.c */
