/* lb_gv03 - near-match fixes 0x005C8230-0x005C8470: guild_input_message, guild_input_pass. Whole file in lb_v.c. */
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

int guild_input_message(int a) {
    u8 *p;
    u8 *q;
    int v;
    Get_sw(0);
    Get_kb_input();
    p = pNet;
    q = p + 2;
    switch (p[2]) {
    case 0:
        *q = p[2] + 1;
        SoftKeyboard_pos_set(80.0f, 0x3A);
        SoftKeyboard_set(1, 0, 0x3D, a);
        Lbc_set_prim(0, Lb_guild_trans, lb_rule_seet_trans_ot2);
        *(s8 *)0x3F36AB = 0;
        break;
    case 1:
        v = (s8)SoftKeyboard_move(a, *(s16 *)0x3F3710, *(s16 *)0x3F3714);
        switch (v) {
        case 1:
        case -1:
            pNet[2] = pNet[2] + 1;
        }
        break;
    case 2:
        SoftKeyboard_exit();
        pNet[2] = 0;
        *(u8 *)0x3F36AB = 1;
        return 1;
    }
    return 0;
}

int guild_input_pass(int a) {
    u8 *p;
    u8 *q;
    s8 sx1;
    Get_sw(0);
    p = pNet;
    q = p + 2;
    switch (p[2]) {
    case 0:
        *q = p[2] + 1;
        SoftKeyboard_pos_set(80.0f, 0x3A);
        SoftKeyboard_set(3, 6, 8, a);
        Lbc_set_prim(0, Lb_guild_trans, lb_rule_seet_trans_ot2);
        *(s8 *)0x3F36AB = 0;
        break;
    case 1:
        if ((sx1 = SoftKeyboard_move(a, *(s16 *)0x3F3710, *(s16 *)0x3F3714)) != 0) {
            pNet[2] = pNet[2] + 1;
        }
        break;
    case 2:
        SoftKeyboard_exit();
        pNet[2] = 0;
        *(u8 *)0x3F36AB = 1;
        return 1;
    }
    return 0;
}
