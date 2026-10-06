/* Lobby: generic senders, sound call, set01 helper (SLPM_654.95 lobby overlay 0x5D3640-0x5D6420). Whole file; runs split into lb_nNN.c */
#include "lobby_f.h"

void sound_call_005D3640(a, b)
int a;
s8 b;
{
    lb_sys.x80 = a;
    lb_sys.x84 = b;
    lb_sys.x7C = 0x14;
}

void Lb_send_chair_status(name, a, b)
char *name;
s8 a;
s8 b;
{
    struct { s8 a; s8 b; char n[0xB]; } p;
    p.a = a;
    p.b = b;
    strcpy(p.n, name);
    lb_send_data(0xD, 5, &p);
    Lb_send_data_to_myself(0xD, 5, &p);
}

void Lb_send_item_result(a0, res)
int a0;
s8 res;
{
    struct { s16 item; s16 num; u8 id[8]; s8 r; } t;
    PLW *pl = &player_work[game_w.master];
    memcpy(t.id, CWPLAYER(pl->work909) + 0x132C, 8);
    t.item = pl->work904;
    t.num = pl->work906;
    t.r = res;
    lb_send_dataTU(0xE, 0xE, &t, a0);
}

void lb_send_data(len, type, data)
int len;
u8 type;
u8 *data;
{
    int t;
    u8 m;
    int l;
    if (Online_ck() != 0 && (t = type, CW8(0x35D5) != 0)) {
        if (t == 3 || Lbs_CheckMatchingFlag(t) != 1) {
            m = CW8(0x2C31);
            switch (m) {
            default:
                return;
            case 3:
            case 2:
                sendDat[0] = type;
                if (len & 0xFF) {
                    flMemcpy(sendDat + 1, data, len & 0xFF);
                }
                cnLBS_Send_ChatBinary(sendDat, (u8)(((u8)len) + 1));
            }
        }
    }
}

void lb_send_dataTU(len, type, data, a3)
int len;
u8 type;
u8 *data;
int a3;
{
    if (Online_ck() != 0 && Lbs_CheckMatchingFlag() != 1) {
        sendDat[0] = type;
        if (len & 0xFF) {
            flMemcpy(sendDat + 1, data, len & 0xFF);
        }
        cnLBS_Send_ChatBinaryTU(a3, sendDat, (u8)(((u8)len) + 1), CallBack_Result_SendChatBinaryTU);
    }
}

void Lb_send_data_to_myself(len, type, data)
int len;
s8 type;
u8 *data;
{
    if (Online_ck() != 0) {
        sendDat[0] = type;
        flMemcpy(sendDat + 1, data, len & 0xFF);
        Lb_check_receipt(cw + 0x440, sendDat);
    }
}

void Lb_put_set01(int n) {
    if (lb_sys.x72 == 0) {
        set01_set2(lb_set01_msg[n]);
        lb_sys.x72 = 0x5A;
    }
}

s8 Lb_get_quest_type(u16 *p) {
    s8 v;
    u16 t = *p;
    v = 3;
    if (!((t >> 3) & 1)) {
        for (;;) {
            v = v - 1;
            if (v <= -1) {
                v = 0;
                break;
            }
            if ((t >> v) & 1) break;
        }
    }
    return v;
}

void Lb_guild_trans(u8 *p) {
    if (lb_sys.x06 >= 2) {
        font_set_stack_no(F(s32, p, 0x18));
        font_set_palette(0);
        switch (lb_sys.x06) {
        case 3:
            lb_select_quest_level_trans(lb_sys.x06);
            return;
        case 15:
        case 5:
            lb_questpage_trans(p);
            return;
        case 8:
            lb_rule_seet_trans(p);
            break;
        }
    }
}

int Lb_check_existF(void) {
    if (Quest_clear_bit_ck(0x67) == 1) {
        return 0;
    }
    if (Quest_clear_bit_ck(0x68) == 1) {
        return 0;
    }
    if (Quest_clear_bit_ck(0x69) == 1) {
        return 0;
    }
    if (Quest_clear_bit_ck(0x6A) != 1) {
        return 1;
    }
    return 0;
}

int Lb_guild_check_requireF(void) {
    if (*(u8 *)0x3C733B < 0x14) {
        return 0;
    }
    if (Quest_clear_bit_ck(0x6B) == 0) {
        return 0;
    }
    if (Event_flag_ck(0x4E) == 0) {
        return 0;
    }
    if (*(u8 *)0x3C741C < 0x32) {
        return 0;
    }
    if (*(u8 *)0x3C741D >= 0x32) {
        return 1;
    }
    return 0;
}

int Lb_get_pl_stat2(int a0) {
    s8 i;
    int r;
    if (Online_ck() == 0) {
        return 0;
    }
    i = a0;
    if (lbCommer[i].mac[0] != 0) {
        r = PLU8(&player_work[i], 0x736) == 0;
    } else {
        r = 2;
    }
    return r;
}

extern u8 D_3E55F0[], D_3E5FF0[], D_3E69F0[], D_3E73F0[], D_3E7DF0[], D_3E87F0[], D_3E91F0[];

void Clear_lobby_ram(void) {
    flMemset(&lb_sys, 0, 0x90);
    client_work[0x2BFE] = 0;
    lb_player[0].pl = &player_work[0];
    lb_player[1].pl = (PLW *)D_3E55F0;
    lb_player[2].pl = (PLW *)D_3E5FF0;
    lb_player[3].pl = (PLW *)D_3E69F0;
    lb_player[4].pl = (PLW *)D_3E73F0;
    lb_player[5].pl = (PLW *)D_3E7DF0;
    lb_player[6].pl = (PLW *)D_3E87F0;
    lb_player[7].pl = (PLW *)D_3E91F0;
    client_work[0x2BFF] = 0;
    client_work[0x2C00] = 0;
    client_work[0x2C01] = 0;
    client_work[0x2C02] = 0;
    client_work[0x2C03] = 0;
    client_work[0x2C04] = 0;
    client_work[0x2C05] = 0;
    init_set_work();
    pNet = network_work;
    Lbc_init_network_work();
    *(s8 *)0x3F3415 = 0;
    cw = client_work;
    lbSendInterval = 0xF;
}

void Lb_put_hint(int a, int n) {
    int t = n * 4;
    if (n == 0x63) {
        sprintf((char *)&lb_sys + a * 0x1E + 0xA, lit_275_00665668, t);
        return;
    }
    sprintf((char *)&lb_sys + a * 0x1E + 0xA, lit_743_00665D60, *(int *)((u8 *)hint_tbl[a] + t));
}

int lb_key_quest_ck(int n) {
    switch (n & 0xFF) {
    case 0x27:
    case 9:
    case 0x65:
    case 0x4F:
    case 0x61:
    case 0x6B:
    case 0x83:
    case 0x88:
    case 0x89:
    case 0x9A:
    case 0x8B:
    case 0xAB:
    case 0xAA:
    case 0xAF:
    case 0xAE:
    case 0xAD:
    case 0xAC:
    case 0x67:
    case 0x68:
    case 0x69:
    case 0x6A:
        return 1;
    }
    return 0;
}
