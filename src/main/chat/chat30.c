/* chat30 - chat/menu windows 0x00278C50-0x00278DB8: Join_pl_chk. Whole file in chat_nm.c. */
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
void chat_log_add(int, s8 *, struct PIT_CHAT *);
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
void flfntLocate(int, s16);
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
void disp_cursorC(s16, s16, s16, s16, int, int);
void Disp_help_mess(int, int);
u8 Equip_moji_color_rare(u8);
int Equip_moji_color_rare_i(u8);

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
extern char lit_2796[];
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
s8 SoftKeyboard_move(s8 *, s16, s16);
void chat_log_add(int, s8 *, PIT_CHAT *);
void func_5CB100(u8, s8 *, u8);
void net_send_chat(u8, int, s8 *, u8);
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





extern void *receive_mes_str[2];


extern s16 receive_mark_pos[2][2];
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


int EquipmentDescriptionWindowA(u8 *, s16, s16, int, u8 *, int);


extern char lit_3701[];
extern char lit_3702[];
void Put_PageArrow(int, int, int, int);
void flfntLocate_i(int, int);
void equip_exp_core(u8 *, s16, s16, int, u8 *);
void Get_equip_icon_uv(u8 *, s16 *, s16 *);



extern char *equip_exp_str_sword[];
extern char *equip_exp_str_gun[];
extern char *equip_exp_str_armor[];
extern char *weapon_exp_str_common[];
extern char *armor_exp_str_common[];
extern char *reload_level_str[];
extern char *wearable_tbl[];
extern char *lv123str[];
extern char *lv12str[];
extern u8 weapon_exp[][16];
extern u8 armor_exp[][16];
extern char lit_4150[];
extern char lit_4151[];
extern char lit_4152[];
extern char lit_4153[];
extern char lit_4154[];
extern char lit_4155[];
extern char lit_4156[];
extern char lit_4157[];
extern char lit_4158[];
extern char lit_4159[];
extern char lit_4160[];
extern char lit_4161[];
extern char lit_4162[];
extern char lit_4163[];
extern char lit_4164[];
extern char lit_4165[];
extern char lit_4166[];
extern char lit_4167[];
extern char lit_4168[];
extern char lit_4169[];
extern char lit_4170[];
extern char lit_4171[];
extern char lit_4172[];
extern char lit_4173[];
void *Get_equip_data_ptr(void *);
void font_print_strings(int, int, void *, int);
int Get_bowgun_atk(void *);
int Get_weapon_job(void *);
u8 Get_equip_rare(u8, u16);
void sword_zokusei(u8 *, int, s16);
void slash_level_bar(u8 *, s16);

#define ATKCONV(v, job) ((u16)((f32)(v) * job_atk_adj_tbl[job]))


extern char *equip_exp_str_sw_attr[];
extern char lit_4221[];
extern char lit_4222[];


void EquipmentCompareWindowA(u8 *cur, u8 *other, s16 x, s16 y, int page, int alpha);



extern u8 Battle_type[];
extern s16 *Pl_slash_tbl[];
extern int slash_bar_color[];
f32 flps0009(void *);


extern char lit_4368[];


extern u8 lit_4374[];
extern u8 setumei_shousai_4372[8];
void font_print_ex(int, int, int, void *);


extern s16 equip_icon_u_tbl[];
extern s16 weapon_icon_u_tbl[];


extern char *ng_word_tbl_0[];
extern char *ng_word_tbl_2[];
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



void Join_pl_chk(void) {
    u32 i;
    u8 *g;

    PitMenu._pad1A = 0;
    PitMenu.x19 = 0;
    if (GW(0x1DC) == 0) {
        for (i = 0, g = (u8 *)&game_w; i < 4; i++, g++) {
            if (game_w.master != i && g[0x208] == 1) {
                PitMenu.x19 |= (1 << i) & 0xFF;
                PitMenu._pad1A++;
            }
        }
    } else {
        for (i = 0; i < 8; i++) {
            if (game_w.master != i && func_5D8370(i) == 0) {
                PitMenu.x19 |= (1 << i) & 0xFF;
                PitMenu._pad1A++;
            }
        }
    }
    if (PitMenu.x16 != 0) {
        PitMenu.x16 &= PitMenu.x19;
        if (PitMenu.x16 == 0) {
            PitMenu.x16 = 0;
            PitMenu.x15 = 1;
            PitMenu.x17 = 3;
            PitMenu.x18 = 1;
        }
    }
}
