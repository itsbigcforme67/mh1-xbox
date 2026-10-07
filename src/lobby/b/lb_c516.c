/* lb_c516 - agent C round 5 0x005B1A50-0x005B1CD8: lb_select_trans (guild room info panel; the second loop reuses the first loop counter i). lb_put_room_member is lb_c513. */
#define flfntLocate flfntLocate_hdr
#define font_print_double font_print_double_hdr
#include "lobby_b.h"
#undef flfntLocate
#undef font_print_double
void font_print_double(int, int, int, int, char *);
void han2zen(char *, char *);
extern char lit_585_0065E318[];
extern char lit_586_0065E320[];
extern u8 join_member[];
extern char lb_board_exp[];
extern char lb_board_message[];
extern char lb_quest_data_tbl[];
extern s16 lb_quest_font_color[];
extern s32 joinQuest;
void Lb_put_msg();
void Lb_put_msg_type2();
void Lb_put_room_message();
void Lb_put_button();
void Lb_put_gold();
int Lb_get_quest_type();
void Put_2TF();
void lb_put_room_member(void);
void lb_select_trans(void) {
    s32 j;
    s32 i;
    char *pe;
    u16 n1;
    s32 q;
    u16 n2;
    char *pm;

    if (joinQuest >= 0xC8) {
        q = (s32)get_quest_info();
    } else {
        q = (s32)lb_quest_all[F(u8, pNet, 0x12)];
    }
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    font_set_palette(0);
    Lb_put_gold();
    flfntSetSize(0x12, 0x12);
    Put_2TF(lb_quest_data_tbl + 0x78);
    Put_2TF(lb_quest_data_tbl + 0x3C);
    Lb_put_msg(lb_board_exp + 0xC8);
    if (F(u8, pNet, 0xE) == 1) {
        pm = lb_board_message + 0x60;
        Lb_put_msg(lb_board_exp + 0xC8);
        font_print_double(*(s16 *)pm, *(s16 *)(pm + 2), 1, 0xA, *(char **)(pm + 4));
        return;
    }
    Put_2TF(lb_quest_data_tbl + 0x8C);
    Put_2TF(lb_quest_data_tbl + 0xA0);
    Lb_put_button(0x1E6, 0x17A, 3);
    pm = lb_board_message;
    font_set_palette(lb_quest_font_color[Lb_get_quest_type(q)]);
    Lb_put_msg(lb_board_exp);
    font_set_palette(0);
    Lb_put_msg(lb_board_exp + 0x64);
    switch (F(u8, pNet, 8)) {
    case 0:
        n1 = 4;
        pe = lb_board_exp + 0x12C;
        n2 = 4;
        Lb_put_room_message(lb_board_exp + 0x2BC);
        break;
    case 1:
        lb_put_room_member();
        n1 = 2;
        pm = lb_board_message + 0x20;
        n2 = 0;
        break;
    case 2:
        pm = lb_board_message + 0x30;
        n1 = 6;
        pe = lb_board_exp + 0x320;
        n2 = 5;
        break;
    }
    n1 = n1 & 0xFFFF;
    i = 0;
    if (0 < n1) {
        do {
            Lb_put_msg_type2(pm);
            i = (i + 1) & 0xFFFF;
            pm += 8;
        } while (i < n1);
    }
    n2 = n2 & 0xFFFF;
    i = 0;
    if (0 < n2) {
        do {
            Lb_put_msg(pe);
            i = (i + 1) & 0xFFFF;
            pe += 0x64;
        } while (i < n2);
    }
}
