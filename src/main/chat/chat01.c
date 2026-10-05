/* chat01 - f_chat 0x002756C0-0x0027573C: Name_ID_change, Disp_name_or_id. Whole file in chat_nm.c. */
#include "types.h"
#include "menu.h"
#include "ud.h"

#define F8(p, o) (*(u8 *)((u8 *)(p) + (o)))
#define FS8(p, o) (*(s8 *)((u8 *)(p) + (o)))
#define F16(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define FS16(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define F32(p, o) (*(u32 *)((u8 *)(p) + (o)))
#define FS32(p, o) (*(s32 *)((u8 *)(p) + (o)))
#define PM ((u8 *)&PitMenu)
void KinshiYogo_chk(char *);
struct PIT_CHAT;
void chat_log_add(u8, s8 *, struct PIT_CHAT *);
int Get_chat_line_num(void);
int Plaza_get_chat_line_num(void);

extern u16 System_timer;
f32 flSin(f32);
void flps0004(void *);
void flps0008(void *);
void SetTextureStage(int);
void SetFilterMode(int);
void SetTrnslMode(int, int);
void reload_tex(int, int);
void flfntLocate(int, int);
void flfntSetSize(int, int);
void font_set_palette(int);
void font_print(void *, ...);
void font_print_sp(void *, ...);
void Put_sprite_rotate(void *, int);
void DispFrameListA(void *, char *, int, int);
void DispFrameList(void *, char *, int);
void DispFrameListOptionArrowC(void *, int);
void DispFrameMessageA(void *, void *, int);
void DispFrameMessage(void *, void *);
void PutButtonICON(void *, int);
void disp_cursorC(s16, s16, s16, s16, s16, int);
void Disp_help_mess(int, int);
u8 Equip_moji_color_rare(u8);

typedef struct PFLP4 { s16 p[4]; u32 col; } PFLP4;
typedef struct PFLP8 { s16 p[4]; u32 col; s16 uv[4]; } PFLP8;



extern u8 pf_menu_sub[];
extern u8 btn_menu_sub[8];
extern u8 lit_2047[];


extern u8 button_icon_uv[][8];


extern u8 equip_color_rare_idx[];
extern u32 equip_color_rare_tbl[];
extern u8 moji_color_rare_2099[5];




extern u8 lit_2244[];







extern u8 minisight_tbl[4][10];







extern u8 Item_data[][16];
extern u8 help_mess_00354680[];
extern u32 item_col_tbl[];
extern char lit_2796[8];
extern s32 *pit_help_str_tbl[];





extern u8 Snd_em_id_conv_tbl[];
typedef struct PSWC { u16 x0; u8 _p2[2]; u16 x4; u8 _p6[2]; u16 x8; u8 _pA[2]; u16 xC; } PSWC;
extern PSWC Psw;
#define GW(o) (*(u8 *)((u8 *)&game_w + (o)))



int NPCZoomInCameraCheck();
void SoftKeyboard_pos_set(f32, int);
void SoftKeyboard_set(int, int, int, int);
void Chat_move(int);


int ChatKinsoku_chk(u8 *);
int Menu_chatlog_i(void);
void SoftKeyboard_exit(void);
s8 SoftKeyboard_move(s8 *, u16, u16);
void chat_log_add(u8, s8 *, PIT_CHAT *);
void func_5CB100(u8, s8 *, u8);
void net_send_chat(u8, int, s8 *, int);
void set01_set(int, int, int);



char *strcpy(char *, const char *);
extern u8 chat_font_color[];
extern u8 chat_cnfg_font_color[];
extern u8 my_user_id[];





extern u8 chat_font_color[];
int sprintf(char *, const char *, ...);
void font_print_uf(void *, ...);
void font_print_double2(int, int, int, int);
void Put_megaphone(int, int, int);
void disp_chat_log_sub(int, s16, int);
void Put_receive_mark(int);
int Get_chat_line_num(void);
int Plaza_get_chat_line_num(void);
void PutArrow(s16, s16, s16, s16, int, int);
int Online_ck();
extern char room_member_id[][8];



u32 chat_log_disp_line(u8 top);



extern u8 pf_chat_log_base[];
extern u8 lit_3171[];
extern char *str_3166[];


extern char lit_3181_00383570[];





extern void *receive_mes_str[];


extern s16 receive_mark_pos[][2];
extern u8 pf_receive_mark[];
extern char lit_3351[];




int func_5D8370(s8);


extern u8 lit_3439[];
extern char lit_3440[];
extern char lit_3441[];
extern char lit_3442[];


extern u8 item_list_frame[];
extern char *item_list_title[];
extern char lit_3511[];
extern char lit_3512[];
extern char lit_3513[];
extern char lit_3514[];
extern char *item_str[];
extern u8 Item_data[][16];
int Item_preparation_one_ck(s16);


extern u8 frame_status_main_00354770[][0x18];
extern u8 frame_status_sub_003547A0[][0x10];
extern char lit_3587[];
extern char *menu_status_str_003546E0[][10];
extern char *status_sub_str_00387C60[];
extern char lit_3588_003837D0[];
extern char lit_3589[];
extern char lit_3590[];
extern char lit_3591[];
extern char lit_3592[];
extern char lit_3593[];
extern char lit_3594[];
extern char *hunter_appellation[];
extern f32 job_atk_adj_tbl[];
extern char *Skill_name[];
extern u8 my_user_id[];
int Event_flag_ck(int);
void Get_hunter_status(void *, u8 *, int *, int *);
int Get_weapon_job2(u8, u16);
void PrintPlayerJob(void *);
void PlayerEquipmentWindow(void *);
void Put_comment(int, int, int, void *);


extern u8 Armor_Head_Data[][0x14];
extern u8 Armor_Body_Data[][0x14];
extern u8 Armor_Arm_Data[][0x14];
extern u8 Armor_Waist_Data[][0x14];
extern u8 Armor_Leg_Data[][0x14];
extern u8 menu_stat_icon_tbl1[5];
extern u8 menu_stat_icon_tbl2[];
extern char lit_3652[];
int Get_equip_name(u8, u16);
u8 Get_equip_rare(u8, u16);


extern char *menu_stat_job_str[];


int EquipmentDescriptionWindowA(u8 *, s16, s16, int);


extern char lit_3701[];
extern char lit_3702[];
void Put_PageArrow(s16, s16, int, int);
void equip_exp_core(void *, s16, s16, int);
void Get_equip_icon_uv(u8 *, s16 *, s16 *);


extern char *equip_exp_str_sw_attr[];
extern char lit_4221[];
extern char lit_4222[];


void EquipmentCompareWindowA(int a, s16 b, s16 c, s16 d, int e);



extern u8 Battle_type[];
extern s16 *Pl_slash_tbl[];
extern int slash_bar_color[];
f32 flps0009(void *);


extern char lit_4368[];


extern u8 lit_4374[];
extern u8 setumei_shousai_4372[];
void font_print_ex(int, int, int, void *);


extern s16 equip_icon_u_tbl[];
extern s16 weapon_icon_u_tbl[];
int Get_weapon_job(u8);


extern s32 ng_word_tbl_0[][2];
extern s32 ng_word_tbl_2[][2];
int ng_word_sub(char *, char *, s8);


u32 strlen(const char *);
char *strstr(const char *, const char *);
int zen_kigou_suuji_chk(u8 *);




extern u8 default_reibun[];
void *memcpy(void *, const void *, int);
void Init_reibun(void);



typedef struct REIBUN { s8 *s[3]; s8 *edit; } REIBUN;
extern REIBUN str_tbl_reibun0[];



int softkey_ck();



void Name_ID_change(void) {
    PitMenu.x14 ^= 1;
    se_req(7, 0x11, 0);
}

void Disp_name_or_id(s16 v) {
    FS16(pf_menu_sub, 2) = v;
    DispFrameMessage(pf_menu_sub, lit_2047);
    FS16(btn_menu_sub, 2) = v;
    PutButtonICON(btn_menu_sub, 1);
}
