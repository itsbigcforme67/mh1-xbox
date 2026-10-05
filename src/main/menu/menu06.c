/* f_menu run (SLPM_654.95 0x00129470-0x001296A4): mix_item_chk, mix_item_2_chk, menu_mix_clear, Menu_mix_i. Whole file in menu_nm.c. */
#include "menu.h"
#include "em.h"
#include "pl.h"

extern u8 Psw[];
extern u8 enemy_icon_tbl[];
extern f32 map_size[][2];
extern u8 room_member_id[];

void *memset(void *, int, int);
void se_req(int, int, int);
extern u8 Item_data[327][16];
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
int Monster_list_search(s8, int);
int Get_weapon_job2(u8, u16);
void vib_set(int, int);
int Reibun_Edit_Core(u8);
int Reibun_Edit_Start(u8);
u8 Reibun_select_mv(u8);
extern u16 item_pick_declaration_code;
extern s16 item_pick_declaration_timer;
int Pl_master_ck(void);
int Check_hold_item(int);
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

int mix_item_chk(u8 no, int id) {
    if (no == 0xFF) {
        return 0;
    }
    if (lpPit->pl->item[no].id != (id & 0xFFFF)) {
        return 0;
    }
    return Item_preparation_one_ck((s16)id) != 0 ? 1 : 0;
}

int mix_item_2_chk(u8 a, u8 b) {
    PLW *pl = lpPit->pl;

    if (a == 0xFF) {
        return 0;
    }
    if (b == a) {
        return 0;
    }
    lpPit->x68 = (s32)Item_preparation_adrs(pl->item[b].id, pl->item[a].id, pl);
    if (lpPit->x68 == 0) {
        return 0;
    }
    if (Item_preparation_list_chk_0((void *)lpPit->x68) != 0) {
        lpPit->x74 = *(s16 *)(lpPit->x68 + 2);
    } else {
        lpPit->x74 = -1;
    }
    return 1;
}

void menu_mix_clear(void) {
    lpPit->x7A = 0;
    lpPit->x49 = 0;
    lpPit->x7B = 0;
    lpPit->x70 = 0xFFFF;
    lpPit->x6E = 0xFFFF;
    lpPit->x6C = 0xFFFF;
    lpPit->x74 = -1;
    lpPit->x68 = 0;
    lpPit->x84 = 0;
}

int Menu_mix_i(void) {
    if (Item_ok_chk(lpPit->pl) == 0) {
        return -1;
    }
    lpPit->x76 = -1;
    if ((s16)ItemPickingDeclaration(0, &lpPit->x76) < 0) {
        return -1;
    }
    menu_mix_clear();
    PitMenu.x10 = 0;
    PitMenu.x11 = 1;
    PitMenu.x12 = 0;
    return 0;
}
