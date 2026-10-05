/* Lobby: quest board (lb_select_quest, lb_set_questpage_info), hand-written from m2c drafts. */
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
int lb_select_quest(void) {
    u8 s1;
    int pad;
    int t;
    LBQUEST *q;
    u8 *p;
    s8 sx;
    pad = Get_sw2(0) & 0xFFFF;
    s1 = mhRule.x58;
    if (s1 == (sx = get_questLevelNum()) + 1) {
        q = get_quest_info();
    } else {
        q = lb_quest_all[(lb_quest_info + (s1 & 0xFF) * 5)[pNet[7]]];
    }
    p = pNet;
    switch (p[2]) {
    case 0:
        p[2] = p[2] + 1;
        lb_set_questpage_info();
        pNet[8] = 0;
        Lbc_set_prim(0, Lb_guild_trans, 0);
    default:
        return 2;
    case 1:
        t = pad & 0xFFFF;
        if (t & 0x20) {
            *(s16 *)0x3F341C = *((u8 *)q + 0x1D);
            mhRule.quest = *((u8 *)q + 0x1D);
            cnWrap_SoundRequest(0);
            return 0;
        }
        if (t & 0x40) {
            cnWrap_SoundRequest(3);
            return 3;
        }
        if (t & 0x400) {
            if (mhRule.x58 < (sx = get_questLevelNum())) {
                u8 *n = pNet;
                u8 v = n[7] + 1;
                n[7] = v;
                if (v >= 5) {
                    pNet[7] = 0;
                }
                lb_set_questpage_info();
                mhRule.quest = *((u8 *)q + 0x1D);
                cnWrap_SoundRequest(1);
            } else if (key_quest_num >= 2) {
                if (mhRule.x58 == (sx = get_questLevelNum())) {
                    u8 *n = pNet;
                    u8 v = n[7] + 1;
                    n[7] = v;
                    if (v >= key_quest_num) {
                        pNet[7] = 0;
                    }
                    lb_set_questpage_info();
                    mhRule.quest = *((u8 *)q + 0x1D);
                    cnWrap_SoundRequest(1);
                }
            }
        } else if (t & 0x800) {
            if (mhRule.x58 < (sx = get_questLevelNum())) {
                u8 *n = pNet;
                u8 v = n[7];
                if (v == 0) {
                    v = 4;
                } else {
                    v = v - 1;
                }
                n[7] = v;
                lb_set_questpage_info();
                mhRule.quest = *((u8 *)q + 0x1D);
                cnWrap_SoundRequest(1);
            } else if (key_quest_num >= 2) {
                if (mhRule.x58 == (sx = get_questLevelNum())) {
                    u8 *n = pNet;
                    u8 v = n[7];
                    if (v == 0) {
                        v = key_quest_num - 1;
                    } else {
                        v = v - 1;
                    }
                    n[7] = v;
                    lb_set_questpage_info();
                    mhRule.quest = *((u8 *)q + 0x1D);
                    cnWrap_SoundRequest(1);
                }
            }
        } else if (t & 0x200) {
            u8 v = p[8] + 1;
            p[8] = v;
            if (v >= 3) {
                pNet[8] = 0;
            }
            Lb_num_to_str(pNet[8] + 1, lb_quest_exp + 0xCC);
            cnWrap_SoundRequest(6);
        }
        return 2;
    }
}
void lb_set_questpage_info(void) {
    LBQUEST *q;
    u8 *qb;
    u8 *s0;
    int type;
    int off;
    s8 sx;
    int mins;
    int secs;
    int i;
    int c;
    if (cw[0x35D3] != 0) {
        if ((u8)mhRule.quest >= 0xC8) {
            q = get_quest_info();
        } else {
            q = lb_quest_all[(u8)mhRule.quest];
        }
    } else {
        if (mhRule.x58 == (sx = get_questLevelNum()) + 1) {
            q = get_quest_info();
        } else {
            q = lb_quest_all[(lb_quest_info + (mhRule.x58 & 0xFF) * 5)[pNet[7]]];
        }
    }
    qb = (u8 *)q;
    type = Lb_get_quest_type(q);
    off = type * 8;
    *(s16 *)(lb_quest_data_tbl + 0x20) = *(s16 *)(lb_quest_color_tex + off);
    *(s16 *)(lb_quest_data_tbl + 0x22) = *(s16 *)(lb_quest_color_tex + off + 2);
    *(s16 *)(lb_quest_data_tbl + 0x24) = *(s16 *)(lb_quest_color_tex + off + 4);
    *(s16 *)(lb_quest_data_tbl + 0x26) = *(s16 *)(lb_quest_color_tex + off + 6);
    s0 = *(u8 **)(qb + 0x18);
    sprintf((char *)lb_quest_exp + 4, lb_quest_attribute[type]);
    if (mhRule.x58 == (sx = get_questLevelNum()) + 1 || qb[0x1D] >= 0xC8) {
        strcpy((char *)lb_quest_exp + 0x68, (char *)Lb_get_quest_str(0));
        strcpy(quest_title, (char *)Lb_get_quest_str(0));
    } else {
        strcpy((char *)lb_quest_exp + 0x68, *(char **)s0);
        strcpy(quest_title, *(char **)s0);
    }
    Lb_num_to_str(pNet[8] + 1, lb_quest_exp + 0xCC);
    Lb_num_to_str(pNet[7] + 1, lb_quest_exp + 0x130);
    if (mhRule.x58 != (sx = get_questLevelNum())) {
        if (Online_ck() == 1 && mhRule.x58 == (sx = get_questLevelNum()) + 1) {
            strcpy(lb_quest_message[3], lb_guild_str[0]);
        } else {
            strcpy(lb_quest_message[3], lb_guild_str[4]);
        }
    } else {
        strcpy(lb_quest_message[3], *(char **)((u8 *)lb_quest_all + 0x31C + key_quest_num * 4));
    }
    Lb_num_to_str(*(s32 *)(qb + 8), lb_quest_exp + 0x194);
    strcat((char *)lb_quest_exp + 0x194, lit_1489_00664A90);
    Lb_num_to_str(*(s32 *)(qb + 4), lb_quest_exp + 0x1F8);
    strcat((char *)lb_quest_exp + 0x1F8, lit_1489_00664A90);
    guildPrice = *(s32 *)(qb + 4);
    mins = *(s32 *)(qb + 0x10) / 1800;
    Lb_num_to_str(mins, lb_quest_exp + 0x25C);
    secs = (*(s32 *)(qb + 0x10) - mins * 0x708) / 30;
    Lb_num_to_str(secs, lb_quest_exp + 0x2C0);
    if (secs == 0) {
        strcat((char *)lb_quest_exp + 0x2C0, lb_num_str[0]);
    }
    sprintf((char *)lb_quest_exp + 0x324, map_name[*(s32 *)(qb + 0x14)]);
    c = qb[0x1D];
    if (c != 0x6B) {
        if (c >= 0x67 && c < 0x6B) {
            sprintf((char *)lb_quest_exp + 0x388, lb_guild_str[9]);
        } else if (c == 0x65) {
            sprintf((char *)lb_quest_exp + 0x388, lb_guild_str[8]);
        } else if (qb[2] < 4 || Online_ck() == 0) {
            sprintf((char *)lb_quest_exp + 0x388, lb_guild_str[5]);
        } else {
            sprintf((char *)lb_quest_exp + 0x388, lb_guild_str[6]);
        }
    } else {
        sprintf((char *)lb_quest_exp + 0x388, lb_guild_str[9]);
    }
    lb_quest_exp[0x3EC] = 0;
    i = 0;
    if (qb[2] > 0) {
        do {
            strcat((char *)lb_quest_exp + 0x3EC, lb_num_str[10]);
            i += 1;
        } while (i < qb[2]);
    }
    if (mhRule.x58 == (sx = get_questLevelNum()) + 1 || qb[0x1D] >= 0xC8) {
        strcpy((char *)lb_quest_exp + 0x450, (char *)Lb_get_quest_str(1));
        strcpy((char *)lb_quest_exp + 0x4B4, (char *)Lb_get_quest_str(2));
        pDetail = Lb_get_quest_str(3);
        return;
    }
    strcpy((char *)lb_quest_exp + 0x450, *(char **)(s0 + 4));
    strcpy((char *)lb_quest_exp + 0x4B4, *(char **)(s0 + 8));
    pDetail = *(s32 *)(s0 + 0xC);
}

typedef struct LBTF { s16 x, y; u8 p[0x10]; } LBTF;   /* 2TF record, 0x14 bytes */
typedef struct LBS8 { s16 x, y; u8 p[4]; } LBS8;      /* text position entry, 8 bytes */
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
void lb_select_quest_level_trans(void) {
    LBTF sp80;
    s16 *sp82 = &sp80.y;
    int i;
    int s1;
    int s2;
    int s0;
    u8 *p;
    u8 *c;
    s8 sx;
    sp80 = *(LBTF *)(lb_quest_data_tbl + 0x64);
    p = lb_quest_level_str;
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    flfntSetSize(0x12, 0x12);
    i = 0;
    c = lb_quest_clear;
    if ((sx = get_questLevelNum()) > 0) {
        do {
            s1 = -0xC;
            if (i == pNet[8]) {
            } else {
                s1 = 0;
            }
            if (*(s8 *)(pNet + 0x12) >= i && (i != 0 || Online_ck() != 0 || key_quest != 0x83)) {
                s2 = 5;
            } else {
                s2 = 9;
            }
            sp80.x += s1;
            Put_2TF(&sp80);
            if (i == pNet[8]) {
                Lb_put_icon(sp80.x - 0xA, sp80.y + 0xC, 1, 0xFF00FF00);
            }
            sp80.y += 0x30;
            if (Online_ck() == 1) {
                s0 = (s16)s1;
                font_print_double((s16)(((LBS8 *)p)->x + s0), ((LBS8 *)p)->y, 1, (s16)s2);
                p += 8;
                font_print_double((s16)(((LBS8 *)(p - 8))->x + s0), ((LBS8 *)p)->y, 1, (s16)s2);
            } else {
                s0 = (s16)s1;
                font_print_double((s16)(((LBS8 *)p)->x + s0), (s16)(((LBS8 *)p)->y + 0xA), 1, (s16)s2);
                p += 8;
            }
            if (*c != 0) {
                if (Online_ck() == 0) {
                    font_print_double((s16)(s0 + 0x228), (s16)(((LBS8 *)p)->y - 0x13), 1, 7);
                } else {
                    font_print_double((s16)(s0 + 0x228), ((LBS8 *)p)->y, 1, 7);
                }
            }
            p += 8;
            c += 1;
            i += 1;
            sp80.x -= s1;
        } while (i < (sx = get_questLevelNum()));
    }
    p = lb_quest_level_str + 0x60;
    if (key_quest_num != 0) {
        s0 = 2;
        p += 8;
    } else {
        s0 = 9;
    }
    if (pNet[8] == (sx = get_questLevelNum())) {
        s2 = -0xC;
    } else {
        s2 = 0;
    }
    sp80.x += s2;
    Put_2TF(&sp80);
    if (pNet[8] == (sx = get_questLevelNum())) {
        Lb_put_icon(sp80.x - 0xA, sp80.y + 0xA, 1, 0xFF00FF00);
    }
    if (Online_ck() == 0) {
        font_print_double((s16)(((LBS8 *)p)->x + (s16)s2), (s16)(((LBS8 *)p)->y - 0x30), 1, (s16)s0);
        return;
    }
    font_print_double((s16)(((LBS8 *)p)->x + (s16)s2), ((LBS8 *)p)->y, 1, (s16)s0);
    sp80.x -= s2;
    sp80.y += 0x30;
    s0 = *(s8 *)(cw + 0x2C2F);
    s1 = -0xC;
    if (pNet[8] == (sx = get_questLevelNum()) + 1) {
    } else {
        s1 = 0;
    }
    sp80.x += s1;
    Put_2TF(&sp80);
    if (pNet[8] == (sx = get_questLevelNum()) + 1) {
        Lb_put_icon(sp80.x - 0xA, sp80.y + 0xA, 1, 0xFF00FF00);
    }
    font_print_double((s16)(((LBS8 *)(lb_quest_level_str + 0x70))->x + (s16)s1), ((LBS8 *)(lb_quest_level_str + 0x70))->y, 1, (s16)((s0 == 0) ? 9 : 5));
    sp80.x -= s1;
}
void lb_questpage_trans(int a) {
    LBQUEST *q;
    int s0;
    int s1;
    int s2;
    int s3;
    int i;
    u8 *p;
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
        if (Lbs_InRoomCheck() != 0) {
            if (i != 3) {
                Lb_put_msg(p);
            }
        } else {
            Lb_put_msg(p);
        }
        i += 1;
        p += 0x64;
    } while (i < 4);
    i = 0;
    p = (u8 *)lb_quest_message;
    do {
        if (Lbs_InRoomCheck() != 0) {
            if (i != 1) {
                Lb_put_msg_type2(p);
            }
        } else {
            Lb_put_msg_type2(p);
        }
        i += 1;
        p += 8;
    } while (i < 2);
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
            if (Lbs_InRoomCheck() != 0) {
                if (s3 != 3) {
                    Lb_put_msg(p);
                }
            } else {
                Lb_put_msg(p);
            }
            s3 += 1;
            p += 0x64;
        } while (s3 < s2);
    }
    if (s1 < s0) {
        p = (u8 *)lb_quest_message + s1 * 8;
        do {
            if (Lbs_InRoomCheck() != 0) {
                if (s1 != 1) {
                    Lb_put_msg_type2(p);
                }
            } else {
                Lb_put_msg_type2(p);
            }
            s1 += 1;
            p += 8;
        } while (s1 < s0);
    }
    guild_trans_ot0(a);
}
void guild_trans_ot0(int a) {
    int col;
    int i;
    u8 *p;
    s8 sx;
    col = Lb_get_cursor_col();
    font_set_stack_no(*(s32 *)(a + 0x18));
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    i = 0;
    p = lb_quest_data_tbl;
    do {
        if (Lbs_InRoomCheck() != 0) {
            if (i != 3) {
                Put_2TF(p);
            }
        } else {
            Put_2TF(p);
        }
        i += 1;
        p += 0x14;
    } while (i < 4);
    Lb_put_button(0x1E6, 0x134, 3);
    if (lb_sys.x68 == 2 && lb_sys.x06 < 0xA) {
        if (mhRule.x58 >= (s8)get_questLevelNum()) {
            if (key_quest_num >= 2) {
                Lb_put_icon(0x13E, 0x1A, 0, col);
                Lb_put_icon(0x19E, 0x1A, 1, col);
            }
        } else {
            Lb_put_icon(0x13E, 0x1A, 0, col);
            Lb_put_icon(0x19E, 0x1A, 1, col);
        }
    }
}
