/* lb_v06 - guild quest board 0x005C8FF0-0x005C9590: lb_set_questpage_info. Whole file in lb_x.c. */
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
typedef struct UV { s16 u, v; } UV;
void lb_set_questpage_info(void) {
    u8 *qb;
    int *s0;
    int type;
    int n;
    UV *d1;
    UV *d2;
    UV *c1;
    UV *c2;
    if (cw[0x35D3] != 0) {
        if ((u8)mhRule.quest >= 200) {
            qb = (u8 *)get_quest_info();
        } else {
            qb = (u8 *)lb_quest_all[(u8)mhRule.quest];
        }
    } else {
        if (mhRule.x58 == (s8)get_questLevelNum() + 1) {
            qb = (u8 *)get_quest_info();
        } else {
            qb = (u8 *)lb_quest_all[(lb_quest_info + (mhRule.x58 & 0xFF) * 5)[pNet[7]]];
        }
    }
    type = Lb_get_quest_type(qb);
    d1 = (UV *)(lb_quest_data_tbl + 0x20);
    d2 = (UV *)(lb_quest_data_tbl + 0x24);
    c1 = (UV *)(lb_quest_color_tex + type * 8);
    c2 = (UV *)((u8 *)(lb_quest_color_tex + 4) + type * 8);
    *d1 = *c1;
    *d2 = *c2;
    s0 = *(int **)(qb + 0x18);
    sprintf((char *)lb_quest_exp + 4, lb_quest_attribute[type]);
    if (mhRule.x58 == (s8)get_questLevelNum() + 1 || qb[0x1D] >= 0xC8) {
        strcpy((char *)lb_quest_exp + 0x68, (char *)Lb_get_quest_str(0));
        strcpy(quest_title, (char *)Lb_get_quest_str(0));
    } else {
        strcpy((char *)lb_quest_exp + 0x68, *(char **)s0);
        strcpy(quest_title, *(char **)s0);
    }
    Lb_num_to_str(pNet[8] + 1, lb_quest_exp + 0xCC);
    Lb_num_to_str(pNet[7] + 1, lb_quest_exp + 0x130);
    if (mhRule.x58 != (s8)get_questLevelNum()) {
        if (Online_ck() == 1 && mhRule.x58 == (s8)get_questLevelNum() + 1) {
            strcpy(lb_quest_message[3], lb_guild_str[0]);
        } else {
            strcpy(lb_quest_message[3], lb_guild_str[4]);
        }
    } else {
        strcpy(lb_quest_message[3], ((char **)(lb_quest_all + 199))[key_quest_num]);
    }
    Lb_num_to_str(*(s32 *)(qb + 8), lb_quest_exp + 0x194);
    strcat((char *)lb_quest_exp + 0x194, lit_1489_00664A90);
    Lb_num_to_str(*(s32 *)(qb + 4), lb_quest_exp + 0x1F8);
    strcat((char *)lb_quest_exp + 0x1F8, lit_1489_00664A90);
    guildPrice = *(s32 *)(qb + 4);
    n = *(s32 *)(qb + 0x10) / 60 / 30;
    Lb_num_to_str(n, lb_quest_exp + 0x25C);
    n = (*(s32 *)(qb + 0x10) - n * 1800) / 30;
    Lb_num_to_str(n, lb_quest_exp + 0x2C0);
    if (n == 0) {
        strcat((char *)lb_quest_exp + 0x2C0, lb_num_str[0]);
    }
    sprintf((char *)lb_quest_exp + 0x324, map_name[*(s32 *)(qb + 0x14)]);
    if (qb[0x1D] == 0x6B || (qb[0x1D] >= 0x67 && qb[0x1D] < 0x6B)) {
        sprintf((char *)lb_quest_exp + 0x388, lb_guild_str[9]);
    } else if (qb[0x1D] == 0x65) {
        sprintf((char *)lb_quest_exp + 0x388, lb_guild_str[8]);
    } else if (qb[2] <= 3 || Online_ck() == 0) {
        sprintf((char *)lb_quest_exp + 0x388, lb_guild_str[5]);
    } else {
        sprintf((char *)lb_quest_exp + 0x388, lb_guild_str[6]);
    }
    lb_quest_exp[0x3EC] = 0;
    n = 0;
    if (0 < qb[2]) {
        do {
            strcat((char *)(lb_quest_exp + 0x3EC), lb_num_str[10]);
            n += 1;
        } while (n < qb[2]);
    }
    if (mhRule.x58 == (s8)get_questLevelNum() + 1 || qb[0x1D] >= 0xC8) {
        strcpy((char *)lb_quest_exp + 0x450, (char *)Lb_get_quest_str(1));
        strcpy((char *)lb_quest_exp + 0x4B4, (char *)Lb_get_quest_str(2));
        pDetail = Lb_get_quest_str(3);
    } else {
        strcpy((char *)lb_quest_exp + 0x450, (char *)s0[1]);
        s0++;
        s0++;
        strcpy((char *)lb_quest_exp + 0x4B4, (char *)*s0);
        pDetail = *(s0 + 1);
    }
}
