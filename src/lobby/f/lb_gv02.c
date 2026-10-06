/* lb_gv02 - near-match fixes 0x005C7350-0x005C74A4: check_questLevelSelect. Whole file in lb_v.c. */
#include "lobby_f.h"
extern s8 key_quest_num;
extern u8 key_quest;
extern u8 *pNet;
int Quest_clear_bit_ck();
int Get_sw2();
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
int Get_kb_input();
void SoftKeyboard_pos_set(f32, int);
void SoftKeyboard_set();
int SoftKeyboard_move();
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

int check_questLevelSelect(a, b)
int a;
int b;
{
    int s1;
    int s0;
    s8 sx1;
    s8 sx2;
    s1 = (s8)lb_get_quest_level(0);
    if (s1 == -1) {
        if ((s8)a == 5 && Online_ck() == 0) {
            return 0;
        }
        return 1;
    }
    if (Online_ck() == 0 && key_quest == 0x83 && (s8)a == 0) {
        return 1;
    }
    s0 = (s8)a;
    if (s0 > s1) {
        goto rest;
    }
    sx1 = lb_get_quest_level(0);
    if (s0 > sx1) {
        goto ret1;
    }
    return 0;
rest:
    sx2 = get_questLevelNum();
    if (s0 == sx2) {
        if (key_quest_num != 0) {
            return 0;
        }
    } else if (Online_ck() == 1 && *(s8 *)(cw + 0x2C2F) != 0 && s0 == 7) {
        return 0;
    }
ret1:
    return 1;
}
