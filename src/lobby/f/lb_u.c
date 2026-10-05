/* Lobby: guild (quest counter) state machine, hand-written from the m2c draft (SLPM_654.95 lobby overlay). */
#include "lobby_f.h"
extern u8 lb_rule_exp[];
extern u8 lb_pit[0xC];
extern u8 lb_quest_info[0x40];
extern s32 npc_dialog_table[];
extern s8 key_quest_num;
extern u8 key_quest;
extern s32 guildPrice;
extern s32 quest_price;
extern u8 D_3E5505[];
extern u8 D_3E5506[];
extern u8 lb_quest_exp[];
extern u8 *pNet;
extern PLW player_work[];
extern u8 ClassInfo[];
int Get_sw2();
void Lbc_init_network_work();
void Lbc_set_prim();
void Lb_guild_trans();
int Lbs_InRoomCheck();
void lb_set_questpage_info();
int lb_guild_startMsg();
int Event_flag_ck();
void Event_flag_set();
void lb_guild_end();
int lb_get_quest_level();
void Gunner_wasure_ck();
int lb_select_quest_level();
s8 Lb_talk_check_default();
int lb_select_quest();
int Lb_check_money();
void Lb_menu_quest_info();
void Lbc_SendMiniData();
int lb_rule_seet_set();
int lb_guild_make_room();
int get_questLevelNum();
void Lb_num_to_str();
void Lb_back_money();
int Lbs_RoomExit();
void lb_guild_talk();
int Quest_clear_bit_ck();
s8 lb_set_key_quest_local();
void flMemset();
extern u8 User_data[];
extern u16 *D_3F3714;
void Lb_guild(void) {
    PLW *pl;
    u8 *em;
    LBQUEST *q;
    LBQUEST *qq;
    int v;
    int a;
    u8 *pm;
    pl = &player_work[game_w.master];
    em = (u8 *)pl->x3B0 + 0x444;
    q = get_quest_info();
    Get_sw2(0);
    switch (lb_sys.x06) {
    case 0:
        lb_rule_exp[4] = 0;
        lb_rule_exp[0x68] = 0;
        lb_rule_exp[0xCC] = 0;
        lb_rule_exp[0x130] = 0;
        lb_rule_exp[0x194] = 0;
        a = em[0xE] * 4;
        *(s32 *)(lb_pit + 4) = *(s32 *)((u8 *)npc_dialog_table + a);
        if (*(s8 *)(cw + 0x2C2F) != 0) {
            lb_quest_info[0x23] = *((u8 *)q + 0x1D);
        }
        lb_sys._pad07[0] = 0;
        lb_sys.x06 = lb_sys.x06 + 1;
        if (Online_ck(a) == 0) {
            if (Quest_clear_bit_ck(0xAA) == 1 && Event_flag_ck(0x52) == 0) {
                lb_sys.x06 = 0;
                lb_sys.x68 = 0x31;
                return;
            }
            if (Quest_clear_bit_ck(0xAB) == 1 && Event_flag_ck(0x4D) == 0) {
                lb_sys.x06 = 0;
                lb_sys.x68 = 0x2E;
                return;
            }
            if (key_quest == 0xAA) {
                key_quest_num = lb_set_key_quest_local();
            }
        } else if (Event_flag_ck(4) == 0) {
            lb_sys.x06 = 0;
            lb_sys.x68 = 5;
            return;
        }
        cw[0x2C08] = 0;
talk:
    default:
        lb_guild_talk();
        return;
    case 1:
        Lbc_init_network_work();
        cnWrap_SoundRequest(0xC);
        Lbc_set_prim(0, Lb_guild_trans, 0);
        if (Online_ck() == 0) {
            if (Lbs_InRoomCheck() == 0) {
                lb_sys.x06 = lb_sys.x06 + 1;
            } else {
                *(s32 *)lb_pit = 0;
                lb_pit[8] = 8;
                lb_pit[9] = 1;
                lb_sys.x06 = 0xF;
                mhRule.quest = *(u16 *)0x3F341C;
                lb_set_questpage_info();
            }
        } else if (Lbs_InRoomCheck() == 0) {
            lb_pit[8] = 0;
            *(s32 *)lb_pit = 0;
            lb_sys.x06 = lb_sys.x06 + 1;
            flMemset(&mhRule, 0, 0x68);
        } else {
            *(s32 *)lb_pit = 0;
            lb_pit[8] = 0xA;
            lb_pit[9] = 1;
            lb_sys.x06 = 0xF;
            mhRule.quest = *(u16 *)0x3F341C;
            lb_set_questpage_info();
        }
        goto talk;
    case 2:
        if (lb_guild_startMsg() == 1) {
            if (Online_ck() == 0) {
                if (Event_flag_ck(0) == 0) {
                    Event_flag_set(0);
                    lb_guild_end();
                    return;
                }
                if (Event_flag_ck(1) == 0 && Quest_clear_bit_ck(0x8B) == 1) {
                    Event_flag_set(3);
                    lb_guild_end();
                    return;
                }
            }
            cnWrap_SoundRequest(9);
            if (key_quest_num == 0) {
                pNet[8] = lb_get_quest_level(0);
            } else if (Online_ck(0) == 1) {
                pNet[8] = 6;
            } else {
                pNet[8] = 5;
            }
            if ((s8)pNet[8] < 0) {
                pNet[8] = 5;
            }
            lb_sys.x06 = lb_sys.x06 + 1;
            Gunner_wasure_ck(User_data);
        }
        goto talk;
    case 3:
        v = lb_select_quest_level();
        switch (v) {
        case 0:
            *(s32 *)lb_pit = 0;
            lb_pit[8] = 1;
            lb_sys.x06 = lb_sys.x06 + 1;
            break;
        case 3:
            *(s32 *)lb_pit = 0;
            lb_pit[8] = 6;
            lb_sys.x06 = 0xD;
            break;
        }
        goto talk;
    case 4:
        if (Lb_talk_check_default(2) != 0) {
            lb_sys.x06 = lb_sys.x06 + 1;
            lb_set_questpage_info();
            cnWrap_SoundRequest(9);
        }
        goto talk;
    case 5:
        v = lb_select_quest();
        switch (v) {
        case 0:
            if (guildPrice > 0) {
                *(s32 *)lb_pit = 0;
                lb_pit[8] = 2;
            } else {
                *(s32 *)lb_pit = 0;
                lb_pit[8] = 9;
            }
            lb_sys.x06 = lb_sys.x06 + 1;
            break;
        case 3:
            lb_sys.x06 = 3;
            pNet[7] = 0;
            pNet[8] = pNet[0x13];
            break;
        }
        goto talk;
    case 6:
        if (Lb_talk_check_default(2) != 0) {
            if (lb_pit[9] == 0) {
                if (Lb_check_money(mhRule.quest) == 0) {
                    if (Online_ck() == 0) {
                        cw[0x35D3] = 1;
                        cw[0x32C5] = 1;
                        *(u8 *)0x3F360A = cw[0x32C5];
                        Lb_menu_quest_info(lb_quest_all[mhRule.quest]);
                        *(s32 *)lb_pit = 0;
                        lb_pit[8] = 4;
                        lb_sys.x06 = 0xA;
                        *(s16 *)0x3F33DC = mhRule.quest;
                        qq = lb_quest_all[mhRule.quest];
                        quest_price = qq->fee;
                        Lb_menu_quest_info(qq);
                        a = game_w.master;
                        v = Lb_get_quest_type(qq) | 0x40;
                        D_3E5506[a * 0xA00] = v;
                        Lb_set_mini_data(cw + a * 0x2FC + 0x1346, v, a);
                        a = game_w.master;
                        memcpy((u8 *)&lbCommer + a * 0x5C + 0x1C, cw + a * 0x2FC + 0x1346, 0x40);
                    } else {
                        *(s32 *)lb_pit = 0;
                        lb_pit[8] = 3;
                        lb_sys.x06 = lb_sys.x06 + 1;
                    }
                } else {
                    *(s32 *)lb_pit = 0;
                    lb_sys.x06 = 0xB;
                    lb_pit[8] = 7;
                    cnWrap_SoundRequest(7);
                }
            } else {
                lb_pit[9] = 0;
                *(s32 *)lb_pit = 0;
                lb_sys.x06 = 0xC;
                lb_pit[8] = 5;
                cnWrap_SoundRequest(3);
            }
        }
        goto talk;
    case 7:
        if (Lb_talk_check_default(2) != 0) {
            if (lb_pit[9] == 0) {
                lb_sys.x06 = lb_sys.x06 + 1;
                cnWrap_SoundRequest(9);
            } else {
                lb_sys.x06 = 9;
                cnWrap_SoundRequest(3);
            }
        }
        goto talk;
    case 8:
        if (lb_rule_seet_set() == 1) {
            lb_sys._pad07[0] = 0;
            lb_sys.x06 = lb_sys.x06 + 1;
        }
        goto talk;
    case 9:
        v = lb_guild_make_room();
        switch (v) {
        case 0:
            a = mhRule._pad58[0];
            if (a == (s8)get_questLevelNum() + 1) {
                qq = get_quest_info();
            } else {
                qq = lb_quest_all[mhRule.quest];
            }
            Lb_menu_quest_info(qq);
            v = Lb_get_quest_type(qq) | 0x40;
            a = game_w.master;
            D_3E5506[a * 0xA00] = v;
            Lbc_SendMiniData(a, v);
            Lb_set_mini_data(cw + game_w.master * 0x2FC + 0x1346);
            Lb_set_mini_data((u8 *)&lbCommer + game_w.master * 0x5C + 0x1C);
            if (*(s8 *)&mhRule == 0) {
                *(s32 *)lb_pit = 0;
                lb_pit[8] = 8;
            } else {
                *(s32 *)lb_pit = 0;
                lb_pit[8] = 4;
            }
            lb_sys.x06 = lb_sys.x06 + 1;
            *(s16 *)0x3F33DC = mhRule.quest;
            *(u16 *)0x3F341C = mhRule.quest;
            quest_price = qq->fee;
            break;
        case 1:
            *(s32 *)lb_pit = 0;
            lb_pit[8] = 6;
            lb_sys.x06 = 0xD;
            break;
        }
        goto talk;
    case 10:
        if (Lb_talk_check_default(0) != 0) {
            if (Online_ck() == 0) {
                D_3E5506[0] = Lb_get_quest_type(lb_quest_all[mhRule.quest]) | 0x40;
                Lb_set_mini_data(my_user_mini_data);
                memcpy(cw + 0x45A, my_user_mini_data, 0x40);
                a = game_w.master;
                memcpy((u8 *)&lbCommer + a * 0x5C + 0x1C, cw + a * 0x2FC + 0x1346, 0x40);
            }
            lb_guild_end();
        }
        goto talk;
    case 15:
        if (Lb_talk_check_default(2) != 0) {
            if (lb_pit[9] == 0) {
                if (Online_ck() == 0) {
                    *(s32 *)lb_pit = 0;
                    lb_pit[8] = 3;
                    cw[0x35D3] = 0;
                    lb_sys.x06 = 0xD;
                    a = game_w.master;
                    qq = lb_quest_all[mhRule.quest];
                    D_3E5506[a * 0xA00] = 0;
                    Lb_set_mini_data(cw + a * 0x2FC + 0x1346, a);
                    a = game_w.master;
                    memcpy((u8 *)&lbCommer + a * 0x5C + 0x1C, cw + a * 0x2FC + 0x1346, 0x40);
                    if (qq->fee > 0) {
                        cnWrap_SoundRequest(8);
                        Gold_add(qq->fee);
                        quest_price = 0;
                    } else {
                        cnWrap_SoundRequest(3);
                    }
                    goto b111;
                }
                if (cw[0x32C5] == 1) {
                    lb_pit[9] = 1;
                    lb_pit[8] = 0xB;
                    *(s32 *)lb_pit = 0;
                    lb_sys.x06 = lb_sys.x06 + 1;
                    cnWrap_SoundRequest(0);
                } else {
                    lb_sys.x06 = 0x11;
                    cnWrap_SoundRequest(0);
                }
            } else {
                *(s32 *)lb_pit = 0;
                lb_pit[8] = 6;
                lb_sys.x06 = 0xD;
            }
        } else {
b111:
            if (*D_3F3714 & 0x200) {
                v = pNet[8] + 1;
                pNet[8] = v;
                if ((v & 0xFF) >= 3) {
                    pNet[8] = 0;
                }
                Lb_num_to_str(pNet[8] + 1, lb_quest_exp + 0xCC);
                cnWrap_SoundRequest(6);
            }
        }
        goto talk;
    case 16:
        if (Lb_talk_check_default(0) != 0) {
            if (lb_pit[9] == 0) {
                *(s32 *)lb_pit = 0;
                lb_pit[8] = 0xB;
                lb_pit[9] = 1;
                lb_sys.x06 = lb_sys.x06 + 1;
            } else {
                *(s32 *)lb_pit = 0;
                lb_pit[8] = 6;
                lb_sys.x06 = 0xD;
            }
        }
        goto talk;
    case 17:
        if (Lbs_RoomExit() == 1) {
            if (cw[0x32C5] == 1) {
                Lb_back_money();
                quest_price = 0;
                cw[0x32C5] = 0;
            }
            *(s32 *)lb_pit = 0;
            lb_pit[8] = 0xC;
            lb_sys.x06 = 0x12;
        }
        goto talk;
    case 18:
        if (Lb_talk_check_default(0) != 0) {
            a = game_w.master;
            D_3E5505[a * 0xA00] = 0;
            D_3E5506[a * 0xA00] = 0;
            Lbc_SendMiniData(a * 0xA00, a);
            Lb_set_mini_data(cw + game_w.master * 0x2FC + 0x1346);
            Lb_set_mini_data((u8 *)&lbCommer + game_w.master * 0x5C + 0x1C);
            lb_sys.x6C = 0;
            lb_sys.x87 = 0x14;
            lb_sys.x06 = 0;
            lb_sys.x68 = 0;
            Lbc_init_network_work();
            NPCZoomInCameraCancel();
b136:
            cw[0x2C08] = 1;
        }
        goto talk;
    case 11:
        if (Lb_talk_check_default(0) != 0) {
            *(s32 *)lb_pit = 0;
            lb_pit[8] = 5;
            lb_sys.x06 = 0xC;
            lb_pit[9] = 0;
        }
        goto talk;
    case 12:
        if (Lb_talk_check_default(2) != 0) {
            if (lb_pit[9] == 0) {
                lb_sys.x06 = 5;
                cnWrap_SoundRequest(9);
                Lbc_set_prim(0, Lb_guild_trans, 0);
                cnWrap_SoundRequest(9);
            } else {
                *(s32 *)lb_pit = 0;
                lb_sys.x06 = 0xD;
                lb_pit[8] = 6;
                cnWrap_SoundRequest(3);
            }
        }
        goto talk;
    case 13:
        if (Lb_talk_check_default(0) != 0) {
            lb_sys.x6C = 0;
            lb_sys.x87 = 0x14;
            lb_sys.x06 = 0;
            lb_sys.x68 = 0;
            Lbc_init_network_work();
            NPCZoomInCameraCancel();
            goto b136;
        }
        goto talk;
    }
}
