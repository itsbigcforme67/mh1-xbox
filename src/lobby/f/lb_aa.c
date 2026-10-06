/* Lobby: chat send, player status window, friend list, hand-written from m2c drafts. */
#include "lobby_f.h"
extern u8 chatIDList[];
extern u8 frame_status_main_0064DDC0[];
extern u8 frame_status_sub_0064DDF0[];
extern char lit_349_00664B78[];
extern char lit_350_00664B90[];
extern char lit_351_00664BC0[];
extern char lit_352_00664BD0[];
extern char *status_sub_str_00389F50[];
extern char *hunter_appellation[];
extern char *Skill_name[];
extern u8 btn_friendentry[8];
extern u8 pf_friendentry[];
extern char *friend_entry_str[2];
extern PLW player_work[];
char *strcpy();
void Lb_send_chat();
int sprintf(char *, const char *, ...);
void DispFrameList();
void DispFrameListOptionArrow();
void DispFrameMessage();
u8 *GetAdrsMiniData();
void font_set_palette();
void flfntLocate();
void font_print_uf();
void PrintPlayerJob();
void Put_comment();
void PlayerEquipmentWindow();
void PutButtonICON();
void Skill_set_PL();
void Lb_get_comment();
void Lb_send_chat_plus(a, b, c)
int a;
int b;
int c;
{
    int i;
    int bit;
    u8 *p;
    u8 *id;
    if (*(u8 *)0x39DAD5 != 0) {
        cw[0x32BE] = 0;
    } else {
        bit = 1;
        i = 0;
        p = (u8 *)lb_player;
        id = chatIDList;
        cw[0x32BE] = 1;
        chatIDList[0] = 0;
        chatIDList[8] = 0;
        chatIDList[0x10] = 0;
        chatIDList[0x18] = 0;
        chatIDList[0x20] = 0;
        chatIDList[0x28] = 0;
        chatIDList[0x30] = 0;
        do {
            if (i != game_w.master && (*(u8 *)0x39DAD6 & bit)) {
                strcpy((char *)id, (char *)p + 0x24);
                id += 8;
            }
            i += 1;
            p += 0x38;
            bit *= 2;
        } while (i < 8);
    }
    Lb_send_chat(a, b, c);
}
int Lb_PlayerStatus(u8 *pl, int idx) {
    char sp50[0x40];
    int s0;
    int s1;
    int sl;
    u32 j;
    u8 *mini;
    u8 a1;
    int y;
    int n;
    if (pl[0] == 0) {
        DispFrameList(frame_status_main_0064DDC0 + (idx & 0xFF) * 0x18, lit_349_00664B78, -1);
        return -1;
    }
    s0 = idx & 0xFF;
    sprintf(sp50, lit_350_00664B90, s0 + 1);
    s1 = idx & 0xFF;
    DispFrameList(frame_status_main_0064DDC0 + s1 * 0x18, sp50, -1);
    DispFrameListOptionArrow(frame_status_main_0064DDC0);
    DispFrameMessage(frame_status_sub_0064DDF0 + s1 * 0x10, status_sub_str_00389F50[s1]);
    mini = GetAdrsMiniData(pl[0xC]);
    switch (s0) {
    case 0:
        font_set_palette(0);
        flfntLocate(0x17A, 0x52);
        font_print_uf(pl + 0x24);
        flfntLocate(0x17A, 0x66);
        font_print_uf(pl + 4);
        flfntLocate(0x17A, 0x7A);
        PrintPlayerJob(pl[0]);
        flfntLocate(0x17A, 0x8E);
        a1 = mini[1];
        font_print(lit_351_00664BC0, a1, hunter_appellation[a1]);
        Put_comment(0x132, 0xAE, 0x14, cw + (u16)pl[0xC] * 0x62 + 0x288C);
        break;
    case 1:
        PlayerEquipmentWindow(pl);
        if (pl[0x910] == 0) {
            flfntLocate(0x132, 0x13A);
            font_print_uf(*(void **)0x334FC0);
        } else {
            y = 0x13A;
            j = 0;
            do {
                n = (int)pl + j;
                if (*(u8 *)(n + 0x910) != 0) {
                    flfntLocate(0x132, y);
                    font_print(lit_352_00664BD0, (u8)Skill_name[*(u8 *)(n + 0x910)]);
                    j += 1;
                    y = (s16)(y + 0x14);
                } else {
                    break;
                }
            } while (j < 5U);
        }
        break;
    }
    return 0;
}
void Disp_FriendListEntry(s16 x, int idx) {
    *(s16 *)(pf_friendentry + 2) = x;
    DispFrameMessage(pf_friendentry, friend_entry_str[(u8)idx]);
    if (!(idx & 0xFF)) {
        *(s16 *)(btn_friendentry + 2) = x - 2;
        PutButtonICON(btn_friendentry, 1);
    }
}
void Lb_PlStatusSet(int a) {
    u8 *mini;
    PLW *pl;
    int s0;
    s16 a1;
    s16 a2;
    int a3;
    mini = GetAdrsMiniData();
    s0 = a & 0xFF;
    a1 = *(s16 *)(mini + 0xA);
    a2 = *(s16 *)(mini + 8);
    a3 = s0 * 0xA00;
    pl = (PLW *)((u8 *)player_work + a3);
    *(s16 *)((u8 *)pl + 0x360) = a1;
    *(s16 *)((u8 *)pl + 0x35E) = a2;
    *(s16 *)((u8 *)pl + 0x362) = *(s16 *)(mini + 0xC);
    *((u8 *)pl + 0x352) = mini[0xE];
    *((u8 *)pl + 0x353) = mini[0xF];
    *((u8 *)pl + 0x354) = mini[0x10];
    *((u8 *)pl + 0x355) = mini[0x11];
    *((u8 *)pl + 0x356) = mini[0x12];
    *((u8 *)pl + 0x357) = mini[0x13];
    Skill_set_PL(pl, a1, a2, a3);
    Lb_get_comment((u8 *)lb_player + s0 * 0x38 + 0x24);
}
