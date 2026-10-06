/* lb_gv01 - near-match fixes 0x005C6F40-0x005C71D0: lb_guild_startMsg. Whole file in lb_v.c. */
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

int lb_guild_startMsg(void) {
    int a1;
    int s0;
    s8 sx1;
    a1 = *(s8 *)(lb_pit + 8) * 8;
    s0 = *(s32 *)(lb_pit + 4) + a1;
    switch (lb_sys.x07) {
    case 0:
        lb_sys.x07 = lb_sys.x07 + 1;
        if (Online_ck() == 0) {
            if (Event_flag_ck(1) == 1) {
                *(s32 *)lb_pit = 0;
                lb_pit[8] = 0xB;
                return 0;
            }
            if (Quest_clear_bit_ck(0x8B) == 1) {
                *(s32 *)lb_pit = 0;
                lb_pit[8] = 0x14;
                *(s32 *)(lb_pit + 4) = *(s32 *)(lb_pit + 4) + *(s8 *)(lb_pit + 8) * 8;
                return 0;
            }
            if (Quest_clear_bit_ck(0x88) == 1) {
                *(s32 *)lb_pit = 0;
                lb_pit[8] = 0xA;
                return 0;
            }
            if (Quest_clear_bit_ck(0x83) == 0 && Event_flag_ck(0) == 0) {
                *(s32 *)lb_pit = 0;
                lb_pit[8] = 0xC;
                *(s32 *)(lb_pit + 4) = *(s32 *)(lb_pit + 4) + *(s8 *)(lb_pit + 8) * 8;
                return 0;
            }
        }
        lb_pit[8] = 0;
        break;
    case 1:
        if (Online_ck() == 0) {
            if (Event_flag_ck(1) == 0 && Quest_clear_bit_ck(0x8B) == 1) {
                if (Lb_put_npc_default() == 0) {
                    lb_sys.x07 = 0;
                    return 1;
                }
                return 0;
            }
            if (Quest_clear_bit_ck(0x83) == 0 && Event_flag_ck(0) == 0) {
                if (Lb_put_npc_default() == 0) {
                    Gold_add(0x5DC);
                    cnWrap_SoundRequest(8);
                    Lb_put_set01(2);
                    lb_sys.x07 = 0;
                    return 1;
                }
                return 0;
            }
        }
        *(s8 *)(lb_pit + 0xB) = NPC_Message(*(s32 *)(s0 + 4), *(s32 *)lb_pit, *(u16 *)s0, *(s8 *)(lb_pit + 9));
        if ((sx1 = Lb_talk_check_default(2)) != 0) {
            lb_sys.x07 = 0;
            return 1;
        }
    }
    return 0;
}
