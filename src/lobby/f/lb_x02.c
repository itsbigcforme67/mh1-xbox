/* lb_x02 - guild quest board 0x005C9A50-0x005C9DE4: lb_select_quest. Whole file in lb_x.c. */
#include "lobby_f.h"
extern s8 key_quest_num;
extern u8 *pNet;
int Get_sw2();
int get_questLevelNum();
void lb_set_questpage_info();
void Lbc_set_prim();
void Lb_guild_trans();
void Lb_num_to_str();
extern u8 lb_quest_exp[];
extern u8 lb_quest_info[];
int lb_select_quest(void) {
    u8 *p2;
    u8 *p;
    LBQUEST *q;
    int pad;
    u8 s1;
    int t;
    s8 sx;
    pad = Get_sw2(0) & 0xFFFF;
    s1 = mhRule.x58;
    if (s1 == (sx = get_questLevelNum()) + 1) {
        q = get_quest_info();
    } else {
        q = lb_quest_all[(lb_quest_info + (s1 & 0xFF) * 5)[pNet[7]]];
    }
    p = pNet;
    p2 = p + 2;
    switch (*p2) {
    case 0:
        *p2 = *p2 + 1;
        lb_set_questpage_info();
        pNet[8] = 0;
        Lbc_set_prim(0, Lb_guild_trans, 0);
        break;
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
            } else if (key_quest_num > 1) {
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
                u8 *pv = pNet + 7;
                if (*pv == 0) {
                    *pv = 4;
                } else {
                    *pv = *pv - 1;
                }
                lb_set_questpage_info();
                mhRule.quest = *((u8 *)q + 0x1D);
                cnWrap_SoundRequest(1);
            } else if (key_quest_num > 1) {
                if (mhRule.x58 == (sx = get_questLevelNum())) {
                    u8 *pv = pNet + 7;
                    if (*pv == 0) {
                        *pv = key_quest_num - 1;
                    } else {
                        *pv = *pv - 1;
                    }
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
        break;
    }
    return 2;
}
