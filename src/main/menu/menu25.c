/* f_menu run (SLPM_654.95 0x0012AE60-0x0012AFC0): map_sign_move. Whole file in menu_nm.c. */
#include "menu.h"
#include "em.h"
#include "pl.h"
#include "fl.h"

extern u8 Psw[];
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
void disp_whole_map(int, f32, f32);
typedef struct PFLPS {
    s16 s[4];
    u32 a;
    u32 b;
    u32 c;
} PFLPS;
void flps0008(void *);
typedef struct PFLPS2 {
    s16 s[4];
    u32 col;
    s16 uv[4];
} PFLPS2;
void efct_circle(int, int, int, f32, f32);
typedef struct PFLP12 {
    s16 p[6];
    u32 col;
    s16 uv[6];
} PFLP12;
void flps000C(void *);
void flmatSetZYX33(f32, f32, f32, void *);
extern u8 enemy_icon_tbl[];
extern u32 enemy_icon_color[];
extern s16 enemy_icon_tex_u[];
void maru_disp_sub(int, f32, f32, f32);
void SetTrnslMode(int, int);
void flSetRenderState(int, u32);
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





































#define OPT(i) (((i) + (u8 *)lpPit)[0x88])

#undef OPT





#define SIGN(o) (*(s16 *)((u8 *)lpPit + (o) + 0x34))























#define MIX70 FLD8(*lpPit, 0x70)
#define MIX71 FLD8(*lpPit, 0x71)
#undef MIX70
#undef MIX71





void map_sign_move(int sw) {
    int i;
    int o;

    o = 0;
    for (i = 0; i < 4; i++, o += 2) {
        if (SIGN(o) < 100 && SIGN(o) % 20 == 0) {
            se_req(1, SIGN(o) / 20 + 0x7B, 0);
        }
        if (SIGN(o) >= 0) {
            SIGN(o)++;
        }
        if (SIGN(o) > 105) {
            SIGN(o) = -1;
        }
    }
    if ((((s16 *)((u8 *)lpPit + 0x34))[game_w.master]) < 0 && ((u16)sw & 2)) {
        lpPit->x3C--;
        if (lpPit->x3C <= 0) {
            (((s16 *)((u8 *)lpPit + 0x34))[game_w.master]) = 0;
            net_send_sys(9, game_w.master);
        }
        return;
    }
    lpPit->x3C = 10;
}
