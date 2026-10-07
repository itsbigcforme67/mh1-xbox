/* lb_select_tag (0x5B1350): logic complete; 178/376 differ, almost all register naming: the original spills the per-iteration copy of i (spA0, 160(sp)) and keeps the class count in fp, we do the opposite. Not built. */
#define flfntLocate flfntLocate_hdr
#define font_print_double font_print_double_hdr
#include "lobby_b.h"
#undef flfntLocate
#undef font_print_double
void font_print_double(int, int, int, int, char *);
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

extern char lit_554_0065E298[];
extern char lit_555_0065E2A8[];
extern char lit_556_0065E2B0[];
extern char lit_557_0065E2C0[];
extern char lit_558_0065E2D8[];
extern char lb_quest_level_str[];
extern s16 lb_quest_font_color[];
typedef struct { f32 f[5]; } F5;
typedef struct { s16 x, y, w, h; u32 col; u8 pad[0xC]; } TF;
void Lb_put_icon();
void Put_2TF();
static void lb_select_tag(void) {
    TF t;
    RI *r;
    s32 i;
    QI *q;
    s32 ty;
    s32 cls;
    char spB0[0x20];
    s32 col;
    s32 spA0;
    s32 lv;
    char **ns;

    *(F5 *)&t = *(F5 *)(lb_quest_data_tbl + 0x64);
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    flfntSetSize(0x12, 0x12);
    cls = Lbs_GetClassAdd() & 0xFFFF;
    i = 0;
    ns = lb_num_str;
    if (0 < cls) {
        do {
            spA0 = i & 0xFFFF;
            if (spA0 == F(u8, pNet, 7)) {
                t.x = t.x - 0xC;
            }
            r = Lbs_GetRoomInfo((s16)i);
            if (F(u8, r, 0x10) == 3) {
                t.col = 0xFF917953;
                lv = ((u32)(F(s32, r, 0x158) & 0x1FE) >> 1) & 0xFF;
                col = 5;
                if (lv != 0) {
                    if (lv >= 0xC8) {
                        q = (QI *)get_quest_info();
                        if (F(s8, cw, 0x2C2F) == 0 || q->x1D != lv) {
                            col = 9;
                            t.col = 0xFF756143;
                            font_print_double((s16)(t.x + 0x28), (s16)(t.y + 0xC), 1, 9, lit_554_0065E298);
                        } else {
                            ty = Lb_get_quest_type(q);
                            font_print_double((s16)(t.x + 0x28), (s16)(t.y + 4), 1, lb_quest_font_color[ty], lb_quest_attribute[ty]);
                            font_print_double((s16)(t.x + 0x28), (s16)(t.y + 0x17), 1, col, *(char **)(lb_quest_level_str + q->x2 * 16 - 0xC));
                        }
                    } else {
                        q = (QI *)lb_quest_all[lv];
                        ty = Lb_get_quest_type(q);
                        font_print_double((s16)(t.x + 0x28), (s16)(t.y + 4), 1, lb_quest_font_color[ty], lb_quest_attribute[ty]);
                        font_print_double((s16)(t.x + 0x28), (s16)(t.y + 0x17), 1, col, *(char **)(lb_quest_level_str + q->x2 * 16 - 0xC));
                    }
                }
                sprintf(spB0, lit_428_0065E270, lb_rule_msg_etc[4], lb_num_str[r->x2], lb_num_str[0xB], lb_num_str[r->x4]);
                font_print_double((s16)(t.x + 0xBE), t.y + 4, 1, (s16)col, spB0);
            } else {
                t.col = 0xFF756143;
                col = 9;
                switch (F(u8, r, 0x10)) {
                case 0:
                case 1:
                case 7:
                case 8:
                    font_print_double((s16)(t.x + 0x28), (s16)(t.y + 0xC), 1, col, lb_rule_msg_etc[5]);
                    break;
                case 2:
                    font_print_double((s16)(t.x + 0x28), (s16)(t.y + 0xC), 1, 6, lit_555_0065E2A8);
                    break;
                case 4:
                    font_print_double((s16)(t.x + 0x28), (s16)(t.y + 0xC), 1, 6, lit_556_0065E2B0);
                    break;
                case 5:
                    font_print_double((s16)(t.x + 0x28), (s16)(t.y + 0xC), 1, 6, lit_557_0065E2C0);
                    break;
                case 6:
                    font_print_double((s16)(t.x + 0x28), (s16)(t.y + 0xC), 1, 6, lit_558_0065E2D8);
                    break;
                }
            }
            Put_2TF(&t);
            font_print_double((s16)(t.x + 0xA), (s16)(t.y + 0xC), 1, (s16)col, ns[1]);
            if (spA0 == F(u8, pNet, 7)) {
                Lb_put_icon((s16)(t.x - 0xE), (s16)(t.y + 8), 1, 0xFF00FF00);
                t.x = t.x + 0xC;
            }
            i = (i + 1) & 0xFFFF;
            ns++;
            t.y = t.y + 0x30;
        } while (i < cls);
    }
}
