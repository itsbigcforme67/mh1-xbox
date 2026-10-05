/* Lobby: guild helpers (quest level, select), hand-written from m2c drafts (SLPM_654.95 lobby overlay). */
#include "lobby_f.h"
extern s8 key_quest_num;
extern u8 key_quest;
extern u8 *pNet;
int Quest_clear_bit_ck();
int Get_sw2();
int get_questLevelNum();
int lb_get_quest_level();
int Event_flag_ck();
void Lb_put_set01();
s8 Lb_talk_check_default();
int Lb_put_npc_default();
int NPC_Message();
int Lb_cursorUD();
int lb_get_quest_level(a)
int a;
{
    int v;
    if (Online_ck() == 1) {
        v = *(u8 *)0x3C733B;
        if (v < 5) {
            return 0;
        }
        if (v < 9) {
            return 1;
        }
        if (v < 0xD) {
            return 2;
        }
        if (v < 0x11) {
            return 3;
        }
        if (v < 0x13) {
            return 4;
        }
        return 5;
    }
    if (Quest_clear_bit_ck(0xAB) == 1) {
        if ((s8)a == 1) {
            return 5;
        }
        return 4;
    }
    if (Quest_clear_bit_ck(0x8B) == 1) {
        return 4;
    }
    if (Quest_clear_bit_ck(0x9A) == 1) {
        return 3;
    }
    if (Quest_clear_bit_ck(0x89) == 1) {
        return 2;
    }
    if (Quest_clear_bit_ck(0x88) == 1) {
        return 1;
    }
    if (Quest_clear_bit_ck(0x83) != 1) {
        return -1;
    }
    return 0;
}
int check_questLevelSelect(a, b)
int a;
int b;
{
    int s1;
    int s0;
    s8 sx1;
    s8 sx2;
    s1 = (s8)lb_get_quest_level(0);
    if (s1 == -1) {
        if ((s8)a == 5 && Online_ck() == 0) {
            return 0;
        }
        return 1;
    }
    if (Online_ck() == 0 && key_quest == 0x83 && (s8)a == 0) {
        return 1;
    }
    s0 = (s8)a;
    if (s1 >= s0) {
        if ((sx1 = lb_get_quest_level(0)) >= s0) {
            return 0;
        }
        return 1;
    }
    if (s0 == (sx2 = get_questLevelNum())) {
        if (key_quest_num != 0) {
            return 0;
        }
        return 1;
    }
    if (Online_ck() == 1 && *(s8 *)(cw + 0x2C2F) != 0 && s0 == 7) {
        return 0;
    }
    return 1;
}
int lb_select_quest_level(void) {
    u16 pad;
    int t;
    s8 v;
    pad = Get_sw2(0);
    t = pad;
    pNet[0x12] = lb_get_quest_level(0);
    if (t & 0x20) {
        if (check_questLevelSelect(*(s8 *)(pNet + 8), t) == 0) {
            u8 *n = pNet;
            mhRule.x58 = n[8];
            n[0x13] = n[8];
            pNet[8] = 0;
            cnWrap_SoundRequest(0);
            return 0;
        }
        cnWrap_SoundRequest(7);
        return 2;
    }
    if (t & 0x40) {
        cnWrap_SoundRequest(3);
        return 3;
    }
    if (Online_ck() == 1) {
        v = Lb_cursorUD(pNet[8], get_questLevelNum() + 2);
    } else {
        v = Lb_cursorUD(pNet[8], get_questLevelNum() + 1);
    }
    pNet[8] = v;
    return 2;
}

extern u8 lb_pit[0xC];
extern u8 RoomRule[];
extern u8 *lb_rule_member[];
extern u8 lb_rule_exp[];
extern u8 lb_rule_message[];
extern u8 lb_quest_data_tbl[];
extern u8 lb_rule_data_tbl[];
extern u8 guildStr[];
extern s32 guildPrice;
void Gold_add(int);
int Lbc_ReadRoomInfo();
int Lbc_ReserveRoom();
int Lbc_GetRoomRule();
int Lbc_SetRoomRule();
int Lbc_SetPropaty();
void SetDialogData_HTML();
int strcmp();
int strlen();
char *strcpy();
char *strcat();
char *strrchr();
void Get_sw();
int Get_kb_input();
void SoftKeyboard_pos_set(f32, int);
void SoftKeyboard_set();
int SoftKeyboard_move();
void SoftKeyboard_exit();
void Lbc_set_prim();
void Lb_guild_trans();
void lb_rule_seet_trans_ot2();
void font_set_stack_no();
void font_set_palette();
void flfntSetSize();
void Lb_put_msg_type2();
void Lb_put_msg();
void Lb_put_room_message();
int lb_rule_seet_trans_ot();
void reload_tex();
void SetTextureStage();
void SetFilterMode();
void flSetRenderState();
void Put_2TF();
int Draw_square();
int Ck_hankaku();
void flfntLocate();
void font_print_uf();
void Lb_num_to_str();
void font_print_uf();
int lb_guild_startMsg(void) {
    int a1;
    int s0;
    s8 sx1;
    a1 = *(s8 *)(lb_pit + 8) * 8;
    s0 = *(s32 *)(lb_pit + 4) + a1;
    switch (lb_sys.x07) {
    case 0:
        lb_sys.x07 = lb_sys.x07 + 1;
        if (Online_ck() == 0) {
            if (Event_flag_ck(1) == 1) {
                *(s32 *)lb_pit = 0;
                lb_pit[8] = 0xB;
                return 0;
            }
            if (Quest_clear_bit_ck(0x8B) == 1) {
                *(s32 *)lb_pit = 0;
                lb_pit[8] = 0x14;
                *(s32 *)(lb_pit + 4) = *(s32 *)(lb_pit + 4) + *(s8 *)(lb_pit + 8) * 8;
                return 0;
            }
            if (Quest_clear_bit_ck(0x88) == 1) {
                *(s32 *)lb_pit = 0;
                lb_pit[8] = 0xA;
                return 0;
            }
            if (Quest_clear_bit_ck(0x83) == 0 && Event_flag_ck(0) == 0) {
                *(s32 *)lb_pit = 0;
                lb_pit[8] = 0xC;
                *(s32 *)(lb_pit + 4) = *(s32 *)(lb_pit + 4) + *(s8 *)(lb_pit + 8) * 8;
                return 0;
            }
        }
        lb_pit[8] = 0;
        break;
    case 1:
        if (Online_ck() == 0) {
            if (Event_flag_ck(1) == 0 && Quest_clear_bit_ck(0x8B) == 1) {
                if (Lb_put_npc_default() == 0) {
                    lb_sys.x07 = 0;
                    return 1;
                }
                return 0;
            }
            if (Quest_clear_bit_ck(0x83) == 0 && Event_flag_ck(0) == 0) {
                if (Lb_put_npc_default() == 0) {
                    Gold_add(0x5DC);
                    cnWrap_SoundRequest(8);
                    Lb_put_set01(2);
                    lb_sys.x07 = 0;
                    return 1;
                }
                return 0;
            }
        }
        *(s8 *)(lb_pit + 0xB) = NPC_Message(*(s32 *)(s0 + 4), *(s32 *)lb_pit, *(u16 *)s0, *(s8 *)(lb_pit + 9));
        if ((sx1 = Lb_talk_check_default(2)) != 0) {
            lb_sys.x07 = 0;
            return 1;
        }
    }
    return 0;
}
int lb_guild_make_room(void) {
    int var_s0;
    s16 temp_v1;
    int pad;
    s8 s1;
    pad = Get_sw2(0) & 0xFFFF;
    pNet[0xC] = 0;
    switch (lb_sys.x07) {
    case 0:
        switch (Lbc_ReadRoomInfo(lb_sys.x07, pad)) {
        case 1:
            break;
        case 0:
            lb_sys.x07 = lb_sys.x07 + 1;
            break;
        }
    default:
        return 2;
    case 1:
        switch (Lbc_ReserveRoom(lb_sys.x07, pad)) {
        case 0:
            lb_sys.x07 = lb_sys.x07 + 1;
            break;
        case 1:
            SetDialogData_HTML(cw + 0x32D1);
            *(s8 *)0x3F36AB = 0;
            lb_sys.x07 = 6;
            break;
        }
        return 2;
    case 2:
        if (Lbc_GetRoomRule(lb_sys.x07, pad) == 1) {
            memcpy(RoomRule + 2, mhRule.pass, 9);
            s1 = 0;
            if (RoomRule[0x97] > 0) {
                var_s0 = (int)RoomRule;
                do {
                    if (strcmp(lb_rule_member[mhRule.x00], (char *)var_s0 + 0xBD) == 0) {
                        RoomRule[0x9A] = s1;
                        break;
                    }
                    s1 += 1;
                    var_s0 += 0x41;
                } while (s1 < RoomRule[0x97]);
            }
            lb_sys.x07 = lb_sys.x07 + 1;
        }
        return 2;
    case 3:
        if (Lbc_SetRoomRule(lb_sys.x07, pad) == 1) {
            lb_sys.x07 = lb_sys.x07 + 1;
        }
        return 2;
    case 4:
        lb_sys.x07 = lb_sys.x07 + 1;
        temp_v1 = *(s16 *)0x3C6FDA;
        *(s16 *)0x3F33DC = mhRule.quest;
        *(s16 *)0x3F3608 = temp_v1;
        mhRule.x5C = (mhRule.x5C & ~0x1FE) | ((mhRule.quest & 0xFF) * 2);
        mhRule.x5C = (mhRule.x5C & 0xFE0001FF) | ((temp_v1 & 0xFFFF) << 9);
        return 2;
    case 5:
        switch (Lbc_SetPropaty(mhRule.x5C, pad)) {
        case 0:
            lb_sys.x07 = 0;
            cw[0x32C5] = 1;
            return 0;
        case 1:
            SetDialogData_HTML(cw + 0x32D1);
            *(u8 *)0x3F36AB = 0;
            lb_sys.x07 = 6;
            return 2;
        }
        break;
    case 6:
        pNet[0xC] = 1;
        if ((u16)pad & 0x20) {
            *(u8 *)0x3F36AB = 1;
            cnWrap_SoundRequest(3);
            return 1;
        }
        return 2;
    }
}
int guild_input_message(int a) {
    u8 *p;
    int v;
    Get_sw(0);
    Get_kb_input();
    p = pNet;
    switch (p[2]) {
    case 0:
        p[2] = p[2] + 1;
        SoftKeyboard_pos_set(80.0f, 0x3A);
        SoftKeyboard_set(1, 0, 0x3D, a);
        Lbc_set_prim(0, Lb_guild_trans, lb_rule_seet_trans_ot2);
        *(s8 *)0x3F36AB = 0;
    default:
        return 0;
    case 1:
        v = (s8)SoftKeyboard_move(a, *(s16 *)0x3F3710, *(s16 *)0x3F3714);
        if (v != -1 && v != 1) {
        } else {
            pNet[2] = pNet[2] + 1;
        }
        return 0;
    case 2:
        SoftKeyboard_exit(p + 2);
        pNet[2] = 0;
        *(u8 *)0x3F36AB = 1;
        return 1;
    }
}
int guild_input_pass(int a) {
    u8 *p;
    s8 sx1;
    Get_sw(0);
    p = pNet;
    switch (p[2]) {
    case 0:
        p[2] = p[2] + 1;
        SoftKeyboard_pos_set(80.0f, 0x3A);
        SoftKeyboard_set(3, 6, 8, a);
        Lbc_set_prim(0, Lb_guild_trans, lb_rule_seet_trans_ot2);
        *(s8 *)0x3F36AB = 0;
    default:
        return 0;
    case 1:
        if ((sx1 = SoftKeyboard_move(a, *(s16 *)0x3F3710, *(s16 *)0x3F3714)) != 0) {
            pNet[2] = pNet[2] + 1;
        }
        return 0;
    case 2:
        SoftKeyboard_exit(p + 2);
        pNet[2] = 0;
        *(u8 *)0x3F36AB = 1;
        return 1;
    }
}
void lb_rule_seet_trans(int a, int b, int c) {
    u8 *s1;
    u8 *s0;
    int i;
    s1 = lb_rule_message;
    s0 = lb_rule_exp;
    font_set_stack_no(*(s32 *)(a + 0x18), b, c);
    if (*(s8 *)((u8 *)&lb_sys + 8) == 1) {
        font_set_palette(0);
        flfntSetSize(0x12, 0x12);
        i = 0;
        do {
            Lb_put_msg_type2(s1);
            if (*(s8 *)(s0 + 4) != 0) {
                if (i == 4) {
                    Lb_put_room_message(s0);
                } else {
                    Lb_put_msg(s0);
                }
            }
            i += 1;
            s0 += 0x64;
            s1 += 8;
        } while (i < 5);
        lb_rule_seet_trans_ot(a);
    }
}
int lb_rule_seet_trans_ot(void) {
    int i;
    u8 *s0;
    s0 = lb_quest_data_tbl;
    reload_tex(1, 0x157);
    SetTextureStage(0x157);
    SetFilterMode(1);
    flSetRenderState(0x60, 0);
    Put_2TF(s0);
    i = 0;
    s0 = lb_rule_data_tbl;
    do {
        Put_2TF(s0);
        i += 1;
        s0 += 0x14;
    } while (i < 8);
    if (mhRule.x4F == 5 || mhRule.x4F == 6) {
        return Draw_square(0x178, 0xC8, 0xCE, 0x40, 0xC000FF00);
    }
    return Draw_square(0x178, 0xC8, 0xCE, 0x40, 0x80206020);
}
void Lb_put_room_message(u8 *a) {
    char buf[0x14];
    s8 b1;
    s8 b2;
    char *p;
    s16 y;
    s16 x;
    int n;
    int len;
    p = (char *)a + 4;
    y = *(s16 *)(a + 2);
    x = *(s16 *)a;
    if (p != 0) {
        n = 0;
        while (p != 0) {
            len = strlen(p);
            strcpy(buf, p);
            if (len >= 0x15) {
                if (Ck_hankaku(buf, 0x14) == 0) {
                    ((s8 *)buf)[0x15] = 0;
                    p += 0x15;
                } else {
                    ((s8 *)buf)[0x14] = 0;
                    p += 0x14;
                }
                flfntLocate(x, y);
                font_print_uf(buf);
                n += 1;
                y = y + 0x16;
                if (n >= 3) {
                    break;
                }
            } else {
                flfntLocate(x, y);
                font_print_uf(buf);
                break;
            }
        }
    }
}
void lb_guild_talk(void) {
    int s0;
    char *t;
    if (lb_sys.x06 >= 2) {
        s0 = *(s32 *)(lb_pit + 4) + *(s8 *)(lb_pit + 8) * 8;
        font_set_palette(0);
        switch (lb_sys.x06) {
        case 6:
            strcpy((char *)guildStr, (char *)*(s32 *)(s0 + 4));
            t = strrchr((char *)guildStr, 0x24);
            if (t != 0) {
                Lb_num_to_str(guildPrice, t);
                strcat((char *)guildStr, strrchr((char *)*(s32 *)(s0 + 4), 0x24) + 1);
            }
            *(s8 *)(lb_pit + 0xB) = NPC_Message(guildStr, *(s32 *)lb_pit, *(u16 *)s0, *(s8 *)(lb_pit + 9));
            return;
        case 3:
        case 5:
        case 8:
        case 2:
            break;
        case 0:
        case 9:
        case 17:
            NPC_Message(0, *(s32 *)lb_pit, 3, *(s8 *)(lb_pit + 9));
            return;
        default:
            *(s8 *)(lb_pit + 0xB) = NPC_Message(*(s32 *)(s0 + 4), *(s32 *)lb_pit, *(u16 *)s0, *(s8 *)(lb_pit + 9));
            break;
        }
    }
}
