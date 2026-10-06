/* lb_v01 - guild: quest level, talk, rule sheet trans 0x005C71D0-0x005C7348: lb_get_quest_level. Whole file in lb_v.c. */
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

int lb_get_quest_level(a)
int a;
{
    int v;
    if (Online_ck() == 1) {
        v = *(u8 *)0x3C733B;
        if (v < 5) {
            return 0;
        }
        if (v < 9) {
            return 1;
        }
        if (v < 0xD) {
            return 2;
        }
        if (v < 0x11) {
            return 3;
        }
        if (v < 0x13) {
            return 4;
        }
        return 5;
    }
    if (Quest_clear_bit_ck(0xAB) == 1) {
        if ((s8)a == 1) {
            return 5;
        }
        return 4;
    }
    if (Quest_clear_bit_ck(0x8B) == 1) {
        return 4;
    }
    if (Quest_clear_bit_ck(0x9A) == 1) {
        return 3;
    }
    if (Quest_clear_bit_ck(0x89) == 1) {
        return 2;
    }
    if (Quest_clear_bit_ck(0x88) == 1) {
        return 1;
    }
    if (Quest_clear_bit_ck(0x83) != 1) {
        return -1;
    }
    return 0;
}
