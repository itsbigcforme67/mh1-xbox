/* lb_select_set_data (0x5B0CE0): logic complete, 13/286 differ (address registers of the two colour-table copies). Not built. */
#include "lobby_b.h"
extern char lit_427_0065E260[];
extern char lit_428_0065E270[];
extern char lit_429_0065E280[];
extern char lit_430_0065E288[];
extern char lb_board_exp[];
extern char lb_quest_data_tbl[];
extern s16 lb_quest_color_tex[];
extern char *lb_quest_attribute[];
extern char *lb_num_str[];
extern char *lb_rule_msg_etc[];
extern char *lb_guild_str[];
extern char *map_name[];
typedef struct { s16 a, b; } P2;
typedef struct { u8 pad0[2]; u8 x2; u8 pad3; s32 x4; s32 x8; u8 padC[4]; s32 x10; s32 x14; char **x18; u8 pad1C; u8 x1D; } QI;
typedef struct { u8 pad0[2]; u16 x2; u16 x4; u8 pad6[0xB]; u8 x11; u8 pad12[0x43]; char x55[1]; } RI;
void han2zen(char *, char *);
char *Lb_get_quest_str();
void Lb_num_to_str();
void KinshiYogo_chk();
RI *Lbs_GetRoomInfo();
static void lb_select_set_data(s32 arg0) {
    s32 o;
    QI *q;
    char sp80[0x20];
    s32 t;
    RI *r;
    s32 mm;
    s32 cls;
    char **qs;
    char sp60[0x20];
    s32 c;
    s32 ty;
    s32 id;
    s16 *d1;
    s16 *cp2;
    s16 *cp;
    s16 *d2;

    r = Lbs_GetRoomInfo(F(u8, pNet, 7));
    cls = Lbs_GetClassAdd() & 0xFFFF;
    id = arg0 & 0xFF;
    if (id >= 0xC8) {
        q = (QI *)get_quest_info();
    } else {
        q = (QI *)lb_quest_all[id];
    }
    ty = Lb_get_quest_type(q);
    o = ty * 8;
    cp = (s16 *)((char *)lb_quest_color_tex + o);
    d1 = (s16 *)(lb_quest_data_tbl + 0x98);
    d2 = (s16 *)(lb_quest_data_tbl + 0x9C);
    d1[0] = cp[0];
    d1[1] = cp[1];
    cp2 = (s16 *)((char *)(lb_quest_color_tex + 2) + o);
    d2[0] = cp2[0];
    d2[1] = cp2[1];
    qs = q->x18;
    sprintf(lb_board_exp + 4, lb_quest_attribute[ty]);
    if (id >= 0xC8) {
        strcpy(lb_board_exp + 0x68, Lb_get_quest_str(0));
    } else {
        strcpy(lb_board_exp + 0x68, *qs);
    }
    sprintf(lb_board_exp + 0xCC, lit_427_0065E260, lb_num_str[1 + F(u8, pNet, 7)], lb_num_str[0xB], lb_num_str[cls & 0xFFFF]);
    sprintf(lb_board_exp + 0x130, lit_428_0065E270, lb_num_str[r->x2], lb_num_str[0xB], lb_num_str[r->x4], lb_rule_msg_etc[0]);
    if (r->x11 != 0) {
        strcpy(lb_board_exp + 0x194, lb_rule_msg_etc[2]);
    } else {
        strcpy(lb_board_exp + 0x194, lb_rule_msg_etc[1]);
    }
    c = F(s8, r, 0x55);
    if (c == 0 || ((c & 0xFF) == 0x81 && F(u8, r, 0x56) == 0x40 && F(u8, r, 0x57) == 0)) {
        strcpy(lb_board_exp + 0x25C, lb_rule_msg_etc[1]);
    } else {
        strcpy(lb_board_exp + 0x25C, lb_rule_msg_etc[2]);
    }
    KinshiYogo_chk(r->x55);
    strcpy(lb_board_exp + 0x2C0, r->x55);
    Lb_num_to_str(q->x8, lb_board_exp + 0x324);
    strcat(lb_board_exp + 0x324, lit_429_0065E280);
    Lb_num_to_str(q->x4, lb_board_exp + 0x388);
    strcat(lb_board_exp + 0x388, lit_429_0065E280);
    t = q->x10;
    mm = t / 60 / 30;
    sprintf(sp80, lit_430_0065E288, mm, (t - mm * 0x708) / 30);
    han2zen(sp80, sp60);
    strcpy(lb_board_exp + 0x3EC, sp60);
    sprintf(lb_board_exp + 0x450, map_name[q->x14]);
    if (q->x1D == 0x6B || (q->x1D >= 0x67 && q->x1D < 0x6B)) {
        sprintf(lb_board_exp + 0x4B4, lb_guild_str[9]);
        return;
    }
    if (q->x1D == 0x65) {
        sprintf(lb_board_exp + 0x4B4, lb_guild_str[8]);
        return;
    }
    if (q->x2 <= 3 || Online_ck() == 0) {
        sprintf(lb_board_exp + 0x4B4, lb_guild_str[5]);
        return;
    }
    sprintf(lb_board_exp + 0x4B4, lb_guild_str[6]);
}
