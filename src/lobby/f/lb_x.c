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
