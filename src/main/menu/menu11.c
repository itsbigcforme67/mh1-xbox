/* f_menu run (SLPM_654.95 0x0012ADE0-0x0012AE60): lb_item_stock_mv, map_move. Whole file in menu_nm.c. */
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

int lb_item_stock_mv(int sw) {
    int r = item_stock_mv(sw);

    if (r != 0) {
        ItemCopy_Pl2Ud(lpPit->pl);
    }
    return r;
}

void map_move(int sw) {
    if (lpPit->lb == 0 && lpPit->x83 == 0 && ((u16)sw & 0x4000)) {
        lpPit->x3E ^= 1;
    }
}
