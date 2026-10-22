/* lb_v02 - guild: quest level, talk, rule sheet trans 0x005C8470-0x005C8544: lb_rule_seet_trans. Whole file in lb_v.c. */
#include "lbsys_g.h"
#include "lobby_f.h"
#ifndef __MWERKS__
signed char SoftKeyboard_move();   /* (declared before its first use) */
unsigned char Get_kb_input();   /* (declared before its first use) */
unsigned short Get_sw2();   /* (declared before its first use) */
#endif
extern s8 key_quest_num;
extern u8 key_quest;
extern u8 *pNet;
int Quest_clear_bit_ck();
#ifdef __MWERKS__
int Get_sw2();
#else
unsigned short Get_sw2();   /* the real return type (tools/check_kr_returns.py) */
#endif
int get_questLevelNum();
int lb_get_quest_level();
int Event_flag_ck();
void Lb_put_set01();
s8 Lb_talk_check_default();
int Lb_put_npc_default();
int NPC_Message();
int Lb_cursorUD();

extern u8 lb_pit[0xC];
extern u8 RoomRule[];
extern u8 *lb_rule_member[];
extern u8 lb_rule_exp[];
extern u8 lb_rule_message[];
extern u8 lb_quest_data_tbl[];
extern u8 lb_rule_data_tbl[];
extern u8 guildStr[];
extern s32 guildPrice;
void Gold_add(int);
int Lbc_ReadRoomInfo();
int Lbc_ReserveRoom();
int Lbc_GetRoomRule();
int Lbc_SetRoomRule();
int Lbc_SetPropaty();
void SetDialogData_HTML();
int strcmp();
int strlen();
char *strcpy();
char *strcat();
char *strrchr();
void Get_sw();
#ifdef __MWERKS__
int Get_kb_input();
#else
unsigned char Get_kb_input();   /* the real return type (tools/check_kr_returns.py) */
#endif
void SoftKeyboard_pos_set(f32, int);
void SoftKeyboard_set();
#ifdef __MWERKS__
int SoftKeyboard_move();
#else
signed char SoftKeyboard_move();   /* the real return type (tools/check_kr_returns.py) */
#endif
void SoftKeyboard_exit();
void Lbc_set_prim();
void Lb_guild_trans();
void lb_rule_seet_trans_ot2();
void font_set_stack_no();
void font_set_palette();
void flfntSetSize();
void Lb_put_msg_type2();
void Lb_put_msg();
void Lb_put_room_message();
int lb_rule_seet_trans_ot();
void reload_tex();
void SetTextureStage();
void SetFilterMode();
void flSetRenderState();
void Put_2TF();
int Draw_square();
int Ck_hankaku();
void flfntLocate();
void font_print_uf();
void Lb_num_to_str();
void font_print_uf();

void lb_rule_seet_trans(int a, int b, int c) {
    u8 *s1;
    u8 *s0;
    int i;
    s1 = lb_rule_message;
    s0 = lb_rule_exp;
    font_set_stack_no(*(s32 *)(a + 0x18), b, c);
    if (*(s8 *)((u8 *)&lb_sys + LBSYS_OFS(8)) == 1) {
        font_set_palette(0);
        flfntSetSize(0x12, 0x12);
        i = 0;
        do {
            Lb_put_msg_type2(s1);
            if (*(s8 *)(s0 + 4) != 0) {
                if (i == 4) {
                    Lb_put_room_message(s0);
                } else {
                    Lb_put_msg(s0);
                }
            }
            i += 1;
            s0 += 0x64;
            s1 += 8;
        } while (i < 5);
        lb_rule_seet_trans_ot(a);
    }
}
