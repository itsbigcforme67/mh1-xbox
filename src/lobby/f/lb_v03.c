/* lb_v03 - guild: quest level, talk, rule sheet trans 0x005C8D70-0x005C8EF0: lb_guild_talk. Whole file in lb_v.c. */
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

void lb_guild_talk(void) {
    int s0;
    char *t;
    if (lb_sys.x06 >= 2) {
        s0 = *(s32 *)(lb_pit + 4) + *(s8 *)(lb_pit + 8) * 8;
        font_set_palette(0);
        switch (lb_sys.x06) {
        case 6:
            strcpy((char *)guildStr, (char *)*(s32 *)(s0 + 4));
            t = strrchr((char *)guildStr, 0x24);
            if (t != 0) {
                Lb_num_to_str(guildPrice, t);
                strcat((char *)guildStr, strrchr((char *)*(s32 *)(s0 + 4), 0x24) + 1);
            }
            *(s8 *)(lb_pit + 0xB) = NPC_Message(guildStr, *(s32 *)lb_pit, *(u16 *)s0, *(s8 *)(lb_pit + 9));
            return;
        case 3:
        case 5:
        case 8:
        case 2:
            break;
        case 0:
        case 9:
        case 17:
            NPC_Message(0, *(s32 *)lb_pit, 3, *(s8 *)(lb_pit + 9));
            return;
        default:
            *(s8 *)(lb_pit + 0xB) = NPC_Message(*(s32 *)(s0 + 4), *(s32 *)lb_pit, *(u16 *)s0, *(s8 *)(lb_pit + 9));
            break;
        }
    }
}
