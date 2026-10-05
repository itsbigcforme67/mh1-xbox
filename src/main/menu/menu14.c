/* f_menu run (SLPM_654.95 0x0012CAF0-0x0012CCCC): boss_icon_color. Whole file in menu_nm.c. */
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
void PlayerStatusWindow(PLW *, u8);
void DispFrameMessage(void *, int);
void flfntLocate(int, int);
void font_print_sp(void *);
extern u8 frame_retire[];
extern int retire_str;
extern void *retire_yesno_str[2];
extern f32 wyvern_area_tbl[][4];
void SetFilterMode(int);
void SetTextureStage(int);
void reload_tex(int, int);
void disp_whole_map(f32, f32);
typedef struct PFLPS {
    s16 s[4];
    u32 a;
    u32 b;
    u32 c;
} PFLPS;
void flps0008(void *);
void SetTrnslMode(int, int);
void flSetRenderState(int, int);
void font_set_stack_no(int);
void disp_timer(void);
void disp_pl_vital(void);
void disp_slash_level(void);
void disp_pachinger(void);
void disp_cannon(void);
void disp_others_info(void);
void disp_name(void);
void Pit_disp_chat(void);
void disp_item_stock(void);
void func_5B4980(void);
void func_60CE50(void);
void Disp_NPC_message(void);
void Pit_disp_pit_effect(void);
void Pit_disp_receive_mes(void);
int SoftKeyboard_alive_check(void);
void DispSoftkeyboard(u8);
void func_63B470(void);
void disp_map(void);
void disp_menu(int, PIT_W *);
void Disp_menu_help(void);
void disp_item(void);
void disp_item_sub_select_ex(void);
void trans_box(void);
extern void (*disp_menu_jmp[])(int, PIT_W *);
f32 flSin(f32);
extern u16 System_timer;
void Chat_init(void);
void Chat_move(int);
void Join_pl_chk(void);
void Pit_effect_move(void);
void Receive_mess_move(void);
void add_prim2(void *, void *, int, int);
void func_5B3ED0(int);
int kb_chat_in_chk(void);
int softkey_ck(void);
extern int ot6;
extern int ot7;
int Game_clear_ck(int);
int Pit_shot_ok_chk(PLW *);
void Pl_box_select(PLW *);
int UseItemChk(PLW *, u16);
int func_63B0C0(int);
void menu_init(void);
void menu_move(int);
extern int ot5;
void Name_ID_change(void);
int Menu_chatlog_i(void);
extern int (*menu_mv_jmp[])(int);
void Chat_log_clear(void);
void pit_prim_init(void);
u16 pit_key_repeat(u16, u16);
void SoftKeyboard_exit(void);
int Quest_time_get(int);
int Online_ck();
int Pl_Skill_ck(PLW *, int);
void trans_pit_0(void);
void trans_pit_1(void);
void trans_pit_2(void);
void trans_pit_1_lb(void);
void trans_pit_2_lb(void);
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

/* Colour of a boss icon on the map: pulses with System_timer. */
u32 boss_icon_color(EMW *em) {
    u32 t;
    int c;
    u32 r;

    if (em->mode == 5) {
        t = (*(u8 *)&System_timer << 8) & 0xFFFF;
        c = ((s8)(int)(32.0f * flSin(0.0000958738f * (f32)t)) + 0x80) & 0xFF;
        r = c | ((c << 16) | 0xC0000000 | (c << 8));
    } else if (FLD8(*em, 0x888) == 1) {
        t = ((System_timer & 0xF) << 12) & 0xFFFF;
        r = (((s8)(int)(64.0f * flSin(0.0000958738f * (f32)t)) + 0x48) << 8) | 0xFFF00000;
    } else {
        t = ((System_timer & 0x3F) << 10) & 0xFFFF;
        r = (((s8)(int)(48.0f * flSin(0.0000958738f * (f32)t)) + 0x80) << 8) | 0xFF1000E0;
    }
    return r;
}
