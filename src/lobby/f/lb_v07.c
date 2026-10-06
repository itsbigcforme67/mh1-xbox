/* lb_v07 - guild quest level list 0x005C8810-0x005C8D70: lb_select_quest_level_trans. Whole file in lb_x.c. */
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
typedef struct LBTF { s16 x, y; s32 w[4]; } LBTF;   /* 2TF record, 0x14 bytes */
typedef struct LBS8 { s16 x, y; char *s; } LBS8;      /* text position entry, 8 bytes */
extern u8 lb_quest_level_str[];
extern char lit_1576_00664A98[];
extern u8 lb_quest_font_color[];
extern u8 lb_quest_clear[6];
void reload_tex();
void SetTextureStage();
void SetFilterMode();
void flSetRenderState();
void flfntSetSize();
void Put_2TF();
void Lb_put_icon(s16, s16, int, int);
void font_print_double(s16, s16, s16, s16, char *);
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
extern char lit_462_00664988[];
void lb_select_quest_level_trans(void) {
    LBTF sp80;
    s16 *sp82;
    u8 *c;
    u8 *p;
    int i;
    int pal;
    s16 xo;
    int sext;
    u8 *p2;
    int pal3;
    s16 xo3;
    s16 *py;
    u8 *q;
    int pal2;
    s16 xo2;
    s8 sx;
    sp80 = *(LBTF *)(lb_quest_data_tbl + 0x64);
    p = lb_quest_level_str;
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    flfntSetSize(0x12, 0x12);
    i = 0;
    if (0 < (sx = get_questLevelNum())) {
        c = lb_quest_clear;
        sp82 = &sp80.y;
        do {
            if (i == pNet[8]) {
                xo = -0xC;
            } else {
                xo = 0;
            }
            if (i <= *(s8 *)(pNet + 0x12) && (i != 0 || Online_ck() != 0 || key_quest != 0x83)) {
                pal = 5;
            } else {
                pal = 9;
            }
            sp80.x += xo;
            Put_2TF(&sp80);
            if (i == pNet[8]) {
                Lb_put_icon(sp80.x - 0xA, *sp82 + 0xC, 1, 0xFF00FF00);
            }
            *sp82 += 0x30;
            if (Online_ck() == 1) {
                sext = (s16)xo;
                font_print_double(((LBS8 *)p)->x + sext, ((LBS8 *)p)->y, 1, pal, ((LBS8 *)p)->s);
                p += 8;
                font_print_double(((LBS8 *)p)->x + sext, ((LBS8 *)p)->y, 1, pal, ((LBS8 *)p)->s);
            } else {
                sext = (s16)xo;
                font_print_double(((LBS8 *)p)->x + sext, ((LBS8 *)p)->y + 0xA, 1, pal, ((LBS8 *)p)->s);
                p += 8;
            }
            if (*c != 0) {
                if (Online_ck() == 0) {
                    font_print_double(sext + 0x228, ((LBS8 *)p)->y - 0x13, 1, 7, lit_462_00664988);
                } else {
                    font_print_double(sext + 0x228, ((LBS8 *)p)->y, 1, 7, lit_462_00664988);
                }
            }
            p += 8;
            c += 1;
            i += 1;
            sp80.x -= xo;
        } while (i < (sx = get_questLevelNum()));
    }
    p2 = lb_quest_level_str + 0x60;
    if (key_quest_num != 0) {
        pal2 = 2;
        p2 += 8;
    } else {
        pal2 = 9;
    }
    if (pNet[8] == (sx = get_questLevelNum())) {
        xo2 = -0xC;
    } else {
        xo2 = 0;
    }
    sp80.x += xo2;
    Put_2TF(&sp80);
    if (pNet[8] == (sx = get_questLevelNum())) {
        Lb_put_icon(sp80.x - 0xA, sp80.y + 0xA, 1, 0xFF00FF00);
    }
    if (Online_ck() == 0) {
        font_print_double(((LBS8 *)p2)->x + (s16)xo2, ((LBS8 *)p2)->y - 0x30, 1, pal2, ((LBS8 *)p2)->s);
        return;
    }
    font_print_double(((LBS8 *)p2)->x + (s16)xo2, ((LBS8 *)p2)->y, 1, pal2, ((LBS8 *)p2)->s);
    q = lb_quest_level_str + 0x70;
    py = &sp80.y;
    sp80.x -= xo2;
    *py += 0x30;
    pal3 = (*(s8 *)(cw + 0x2C2F) != 0) ? 5 : 9;
    if (pNet[8] == (sx = get_questLevelNum()) + 1) {
        xo3 = -0xC;
    } else {
        xo3 = 0;
    }
    sp80.x += xo3;
    Put_2TF(&sp80);
    if (pNet[8] == (sx = get_questLevelNum()) + 1) {
        Lb_put_icon(sp80.x - 0xA, *py + 0xA, 1, 0xFF00FF00);
    }
    font_print_double(((LBS8 *)q)->x + (s16)xo3, ((LBS8 *)q)->y, 1, pal3, ((LBS8 *)q)->s);
    sp80.x -= xo3;
}
