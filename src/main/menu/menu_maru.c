/* menu_maru - f_menu 0x0012CCD0-0x0012D204: maru_disp_sub (filled circle sprite on the map), camp_disp_sub (the camp
   icon) and enemy_on_map (a monster's icon on the map, turned to its facing). camp_disp_sub / enemy_on_map matched by
   agent B (12 Oct 2026): the int fields after the coordinates, the icon's corner offsets written as -t + 2.0f * t * k
   in the loop. */
/* menu_nm - f_menu (SLPM_654.95 0x00127440-0x00134950, main.bin): the whole pit menu file as near-match C, not built; matching runs are built from it as menuNN.c.
 * Field meanings are guesses. */
#include "g_alias.h"
#include "menu.h"
#include "em.h"
#include "pl.h"
#include "fl.h"
extern u8 Psw[];
extern f32 map_size[][2];
extern u8 room_member_id[];
void *memset(void *, int, int);
void se_req(int, int, int);
#include "version.h"
#ifndef __MWERKS__
signed char SoftKeyboard_alive_check(void);   /* (declared before its first use) */
unsigned char Check_hold_item(int);   /* (declared before its first use) */
unsigned char Get_weapon_job2(u8, u16);   /* (declared before its first use) */
#endif
extern u8 Item_data[327][ITEM_ROW];
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
#ifdef __MWERKS__
int SoftKeyboard_alive_check(void);
#else
signed char SoftKeyboard_alive_check(void);   /* the real return type (tools/check_kr_returns.py) */
#endif
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
int pit_key_repeat(u16, u16);
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
s8 Monster_list_search(s8, int);
#ifdef __MWERKS__
int Get_weapon_job2(u8, u16);
#else
unsigned char Get_weapon_job2(u8, u16);   /* the real return type (tools/check_kr_returns.py) */
#endif
void vib_set(int, int);
int Reibun_Edit_Core(u8);
int Reibun_Edit_Start(u8);
u8 Reibun_select_mv(int, u8);
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
#if !defined(G_PART) || G_PART == 1
void maru_disp_sub(int col, f32 x, f32 y, f32 r) {
    PFLPS q;
    f32 w;

    SetFilterMode(1);
    q.s[0] = 0.8f * (x - r);
    q.b = 0xF00010;
    q.a = col;
    q.c = 0x1000020;
    q.s[1] = y - r;
    w = r + r;
    q.s[2] = 0.8f * w;
    q.s[3] = w;
    flps0008(&q);
}
#endif

/* The camp icon at map position (x, y). */
#if !defined(G_PART) || G_PART == 1
void camp_disp_sub(f32 x, f32 y) {
    PFLPS q;
    f32 w, h;

    reload_tex(1, GSLOT(0x119, 0x140));
    SetTextureStage(GSLOT(0x119, 0x140));
    SetFilterMode(0);
    q.s[0] = 0.8f * (x - 8.0f);
    q.s[1] = y - 8.0f;
    w = 12.8f;
    h = 16.0f;
    q.s[2] = w;
    q.s[3] = h;
    q.a = -1;
    q.b = 0xF00020;
    q.c = 0xFF002F;
    flps0008(&q);
}
#endif

/* A monster on the map: a coloured dot for small ones (icon table bit 0x80), else its icon turned to its facing. */
#if !defined(G_PART)
void enemy_on_map(EMW *em, f32 x, f32 y, f32 scale) {
    u8 c;
    f32 r;
    u32 col;
    s16 u;
    s16 u2;
    f32 v[3];
    PFLP12 q;
    FLMAT m;
    f32 out[4][3];
    f32 step;
    f32 lo;
    f32 t;
    s16 i;
    u32 ang;

    c = enemy_icon_tbl[em->kind];
    if (c == 0xFF) {
        return;
    }
    if (c & 0x80) {
        if (c & 0x40) {
            r = 5.0f * scale;
            col = boss_icon_color(em);
        } else {
            r = 3.0f;
            col = enemy_icon_color[c & 0xF];
        }
        maru_disp_sub(col, x, y, r);
        return;
    }
    x *= 0.8f;
    SetFilterMode(0);
    if (FLD8(*em, 0x388) != 2) {
        c |= 4;
    }
    u = enemy_icon_tex_u[c];
    u2 = u + 0x10;
    flmatInit(&m);
    ang = (0x18000 - em->ang[1]) & 0xFFFF;
    flmatSetZYX33(0.0f, 0.0f, 2.0f * (3.1415927f * (((360.0f * (f32)ang) / 65536.0f) / 360.0f)), &m);
    v[2] = 0.0f;
    t = 12.8f * scale;
    for (i = 0; i < 4; i++) {
        v[0] = -t + 2.0f * t * (f32)(i / 2);
        v[1] = -t + 2.0f * t * (f32)(i & 1);
        flvecApplyMat33(out[i], v, &m);
        out[i][0] = x + 0.8f * out[i][0];
        out[i][1] = out[i][1] + y;
    }
    q.p[0] = out[0][0];
    q.p[1] = out[0][1];
    q.p[2] = out[1][0];
    q.p[3] = out[1][1];
    q.p[4] = out[2][0];
    q.p[5] = out[2][1];
    q.uv[0] = u;
    q.uv[1] = 0xF0;
    q.uv[3] = 0x100;
    q.uv[5] = 0xF0;
    q.uv[2] = u;
    q.uv[4] = u2;
    q.col = boss_icon_color(em);
    flps000C(&q);
    q.p[0] = out[3][0];
    q.p[1] = out[3][1];
    q.uv[0] = u2;
    q.uv[1] = 0x100;
    flps000C(&q);
}
#endif
