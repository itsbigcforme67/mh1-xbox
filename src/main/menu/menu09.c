/* f_menu run (SLPM_654.95 0x0012A390-0x0012A6D0): Menu_equipment_i, Menu_equipment_mv, menu_option_i, menu_option_mv. Whole file in menu_nm.c. */
#include "menu.h"
#include "em.h"
#include "pl.h"

extern u8 Psw[];
extern u8 enemy_icon_tbl[];
extern f32 map_size[][2];
extern u8 room_member_id[];

void *memset(void *, int, int);
void se_req(int, int, int);
#include "version.h"
#ifndef __MWERKS__
signed char Monster_list_search(s8, int);   /* (declared before its first use) */
unsigned char Check_hold_item(int);   /* (declared before its first use) */
unsigned char Get_weapon_job2(u8, u16);   /* (declared before its first use) */
#endif
extern u8 Item_data[327][ITEM_ROW];
void Chat_log_clear(void);
void pit_prim_init(void);
u16 pit_key_repeat(u16, u16);
void SoftKeyboard_exit(void);
int Quest_time_get(int);
int Online_ck(void);
int Pl_Skill_ck(PLW *, int);
void trans_pit_0();
void trans_pit_1();
void trans_pit_2();
void trans_pit_1_lb();
void trans_pit_2_lb();
void func_5B3D70();
void func_609750();
int Item_ok_chk(PLW *);
void ListSelect(u8 *, int, int);
void Menu_select_mv(u8 *, int, int);
void Pl_item_erase(PLW *, u8);
void func_5B3E60();
int func_5D8370(s8);
int Item_preparation_one_ck(s16);
void *Item_preparation_adrs(s16, s16, PLW *);
int Item_preparation_list_chk_0(void *);
int ItemPickingDeclaration(int, s16 *);
int Game_clear_ck(int);
void Quest_retire_set(void);
int func_5BD520(void);
void PageSelect(u8 *, int, int);
extern u8 quest_w[];
void font_print_uf(char *, int);
u8 *func_5B4D30(u8);
extern u8 User_data[];
int Item_preparation_list_search(s8 *, s8, u16 *, u16 *);
#ifdef __MWERKS__
int Monster_list_search(s8, int);
#else
signed char Monster_list_search(s8, int);   /* the real return type (tools/check_kr_returns.py) */
#endif
#ifdef __MWERKS__
int Get_weapon_job2(u8, u16);
#else
unsigned char Get_weapon_job2(u8, u16);   /* the real return type (tools/check_kr_returns.py) */
#endif
void vib_set(int, int);
int Reibun_Edit_Core(u8);
int Reibun_Edit_Start(u8);
u8 Reibun_select_mv(u8);
extern u16 item_pick_declaration_code;
extern s16 item_pick_declaration_timer;
int Pl_master_ck(void);
#ifdef __MWERKS__
int Check_hold_item(int);
#else
unsigned char Check_hold_item(int);   /* the real return type (tools/check_kr_returns.py) */
#endif
void Pl_item_get_se(PLW *, int);
int Pl_item_stack(PLW *, int, int);
u8 set01_set(int, int, s16);
int item_stock_mv();
int Info_stack_ck(void);
void ItemCopy_Pl2Ud(PLW *);
void net_send_sys(int, u8);
f32 flSqrt(f32);
void menu_data_mix_sub(int);
void menu_data_monster_sub(int);
int menu_chcnfg_sendpl(int sw);
int menu_chcnfg_reibun(int sw);

#ifdef VERSION_MHG
#if !defined(G_PART) || G_PART == 1
int Menu_equipment_i(void) {    /* G returns 0 */
    lpPit->x43 = 0;
    lpPit->x44 = 0;
    PitMenu.x10 = 0;
    return 0;
}
#endif
#else
void Menu_equipment_i(void) {
    lpPit->x43 = 0;
    lpPit->x44 = 0;
    PitMenu.x10 = 0;
}
#endif

#if !defined(G_PART)
int Menu_equipment_mv(int sw) {
    PLW *pl = lpPit->pl;
    u8 sel;
    int job;
#ifdef VERSION_MHG
    u8 ent[6];
    u8 *menu_equip_get_equip_g(u8 *, u8, PLW *);
#endif

    sel = lpPit->x43;
    ListSelect(&sel, sw, 6);
    if (sel != lpPit->x43) {
        lpPit->x43 = sel;
        lpPit->x44 = 0;
    }
#ifdef VERSION_MHG
    /* G (main 0x11C830): the pages of the selected piece: a weapon 2 (a gun 4), armour 3 */
    if (menu_equip_get_equip_g(ent, lpPit->x43, pl) != 0) {
        lpPit->x45 = 3;
        if (lpPit->x43 == 0) {
            lpPit->x45 = 2;
            if ((job = Get_weapon_job2(FLD8(*pl, 0x35F), FLD16(*pl, 0x360)) & 0xFF) == 1 || job == 5) {
                lpPit->x45 = 4;
            }
        }
        PageSelect(&lpPit->x44, sw, lpPit->x45);
    }
#else
    if (menu_equip_get_equip(lpPit->x43) != 0) {
        lpPit->x45 = 2;
        if (lpPit->x43 == 0 && ((job = Get_weapon_job2(FLD8(*pl, 0x35F), FLD16(*pl, 0x360)) & 0xFF) == 1 || job == 5)) {
            lpPit->x45 = 4;
        }
        PageSelect(&lpPit->x44, sw, lpPit->x45);
    }
#endif
    return sw;
}
#endif

#ifdef VERSION_MHG
#if !defined(G_PART) || G_PART == 2
int menu_option_i(void) {       /* G returns 0 */
    lpPit->x43 = 0;
    PitMenu.x10 = 0;
    return 0;
}
#endif
#else
void menu_option_i(void) {
    lpPit->x43 = 0;
    PitMenu.x10 = 0;
}
#endif

/* 0x12A4C0: option screen: ListSelect over the 5 rows, toggle / cycle the setting, copy them to option_w and
 * game_w, start the pad vibration when it was switched on. */
#define lpPit_opt(i) ((&lpPit->x88)[i])   /* option row values, lpPit+0x88.. */
#if !defined(G_PART)
int menu_option_mv(int sw) {
    u8 c;

    ListSelect(&lpPit->x43, sw, 5);
    if ((u16)sw & 0xC00) {
        c = lpPit->x43;
        switch (c) {
        case 1:
            if (lpPit->x88 != 0) {
                se_req(7, 0x15, 0);
            } else {
        case 0:
                lpPit_opt(c) ^= 1;
                se_req(7, 0x16, 0);
            }
            break;
        case 2:
        case 3:
            if ((u16)sw & 0x800) {
                if (lpPit_opt(c) == 0) {
                    lpPit_opt(c) = 2;
                } else {
                    lpPit_opt(c)--;
                }
            } else {
                lpPit_opt(c)++;
                if (lpPit_opt(lpPit->x43) > 2) {
                    lpPit_opt(lpPit->x43) = 0;
                }
            }
            se_req(7, 0x16, 0);
            break;
        case 4:
            if (FLD16(Psw, 4) & 0xC00) {
                lpPit->x8C ^= 1;
                se_req(7, 0x16, 0);
            }
            break;
        }
    }
    c = lpPit->x8A;
    option_w.x04 = c;
    GW8(0x1DD) = c;
    c = lpPit->x8B;
    option_w.x07 = c;
    GW8(0xF) = c;
    c = lpPit->x8C;
    if (c != option_w.x03) {
        option_w.x03 = c;
        if (option_w.x03 != 0) {
            vib_set(0, 1);
            vib_set(1, 1);
        }
    }
    return (u16)((u16)sw & 0x8040);
}
#endif
