/* lb_x01 - guild quest board page 0x005C9590-0x005C9908: lb_questpage_trans (draws the quest text lines; `if (Lbs_InRoomCheck() == 0 || i != 3)` shares one call;
   s3..s0 are declared first (descending registers), the second loop has its own p2/j declared after i/p). Whole file in lb_x.c. */
#include "lobby_f.h"
extern s8 key_quest_num;
extern u8 *pNet;
extern u8 lb_quest_info[];
extern u8 lb_quest_exp[];
extern u8 lb_quest_data_tbl[];
extern u8 lb_quest_color_tex[];
extern char *lb_quest_attribute[];
extern char *lb_guild_str[];
extern char *lb_num_str[];
extern char *map_name[];
extern char *lb_quest_message[];
extern char quest_title[];
extern char lit_1489_00664A90[];
extern s32 guildPrice;
extern s32 pDetail;
int Get_sw2();
int get_questLevelNum();
void lb_set_questpage_info();
void Lbc_set_prim();
void Lb_guild_trans();
void Lb_num_to_str();
int Lb_get_quest_type();
int Lb_get_quest_str();
int sprintf(char *, const char *, ...);
char *strcpy();
char *strcat();
extern u8 lb_quest_level_str[];
extern char lit_1576_00664A98[];
extern u8 lb_quest_font_color[];
extern u8 lb_quest_clear[];
void reload_tex();
void SetTextureStage();
void SetFilterMode();
void flSetRenderState();
void flfntSetSize();
void Put_2TF();
void Lb_put_icon();
void font_print_double();
int Lbs_InRoomCheck();
int Lb_get_cursor_col();
void font_set_stack_no();
void Lb_put_button();
void Lb_put_gold();
void font_set_palette();
void flfntLocate();
void Lb_put_msg();
void Lb_put_msg_type2();
int Quest_clear_bit_ck();
void guild_trans_ot0();
extern u8 key_quest;

void lb_questpage_trans(int a) {
    int s3;
    int s2;
    int s1;
    int s0;
    int i;
    u8 *p;
    u8 *p2;
    int j;
    LBQUEST *q;
    s8 sx;
    if (Lbs_InRoomCheck() == 0) {
        if (mhRule.x58 == (sx = get_questLevelNum()) + 1) {
            q = get_quest_info();
        } else {
            q = lb_quest_all[(lb_quest_info + (mhRule.x58 & 0xFF) * 5)[pNet[7]]];
        }
    } else if (mhRule.quest >= 0xC8U) {
        q = get_quest_info();
    } else {
        q = lb_quest_all[mhRule.quest];
    }
    font_set_stack_no(*(s32 *)(a + 0x18));
    if (Quest_clear_bit_ck(*((u8 *)q + 0x1D)) == 1) {
        flfntSetSize(0x12, 0x12);
        font_set_palette(5);
        flfntLocate(0x20C, 0x46);
        font_print(lit_1576_00664A98, lb_guild_str[7]);
    }
    Lb_put_gold();
    flfntSetSize(0x12, 0x12);
    font_set_palette(((s16 *)lb_quest_font_color)[Lb_get_quest_type(q)]);
    Lb_put_msg(lb_quest_exp);
    font_set_palette(0);
    i = 1;
    p = lb_quest_exp + 0x64;
    do {
        if (Lbs_InRoomCheck() == 0 || i != 3) {
            Lb_put_msg(p);
        }
        i += 1;
        p += 0x64;
    } while (i < 4);
    j = 0;
    p2 = (u8 *)lb_quest_message;
    do {
        if (Lbs_InRoomCheck() == 0 || j != 1) {
            Lb_put_msg_type2(p2);
        }
        j += 1;
        p2 += 8;
    } while (j < 2);
    switch (pNet[8]) {
    case 0:
        s1 = 2;
        s3 = 4;
        s2 = 0xA;
        s0 = 7;
        break;
    case 1:
        s3 = 0xA;
        s2 = 0xD;
        s1 = 7;
        s0 = 0xA;
        break;
    case 2:
        s1 = 0xA;
        s3 = 0xD;
        s2 = 0xE;
        s0 = 0xA;
        flfntLocate(0x157, 0xA0);
        font_print(lit_1576_00664A98, pDetail);
        break;
    }
    if (s3 < s2) {
        p = lb_quest_exp + s3 * 0x64;
        do {
            if (Lbs_InRoomCheck() == 0 || s3 != 3) {
                Lb_put_msg(p);
            }
            s3 += 1;
            p += 0x64;
        } while (s3 < s2);
    }
    if (s1 < s0) {
        p = (u8 *)lb_quest_message + s1 * 8;
        do {
            if (Lbs_InRoomCheck() == 0 || s1 != 1) {
                Lb_put_msg_type2(p);
            }
            s1 += 1;
            p += 8;
        } while (s1 < s0);
    }
    guild_trans_ot0(a);
}
