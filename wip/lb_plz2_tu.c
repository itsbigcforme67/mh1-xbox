/* lb_plz2 - one translation unit 0x00594260-0x00598DB0: plaza_moveMain, plaza_enterLobby, plaza_movePlaza, plaza_backToServer, getFriendNow, getUserInfo, Lb_get_comment, plaza_checkFriend, mail_input, get_friend_page_num, get_page_num, plaza_searchAll, plaza_searchMember, my_comment_input, plaza_checkMyStatus, plaza_setMyComment, Plaza_add_friend, plaza_req_input, getHandleFromID, plaza_mailBox, plaza_setChatMode, lb_chatMemberCheck, Lb_checkChatID. Built by tools/lbtu.py from the per-run files; functions that are
   not C yet stay original bytes (asm stubs, build/raw/*.inc). */
#pragma readonly_strings on
#define plaza_movePlaza plaza_movePlaza_hdr
#define plaza_searchAll plaza_searchAll_hdr
#define plaza_searchMember plaza_searchMember_hdr
#define plaza_setChatMode plaza_setChatMode_hdr
#include "lbui_proto.h"
#undef plaza_movePlaza
#undef plaza_searchAll
#undef plaza_searchMember
#undef plaza_setChatMode
void plaza_movePlaza();
void plaza_searchAll();
void plaza_searchMember();
void plaza_setChatMode();
#define X0Ap(p) (*(u8 *)&(p)->x0A)
extern u8 tl_member_buff[];
extern u8 D_3A1622[];
int Lbs_GetLobbyMemberList();
int Lbs_request_enter_lobby();
int Lbc_DownloadQuest();
void To_EnterLobby();
#define X0A (*(u8 *)&a->x0A)
extern u8 D_3A2940[];
int Lbs_ExitAndEnterPlaza();
void To_EnterPlaza();
void To_TopMenu();
/* sprite template for Put_2TF / Put_sprite_rotate (helpLineTbl entries, stride 0x14) */
typedef struct { s16 x; s16 y; s16 w; s16 h; u8 pad08[4]; s16 u0; s16 v0; s16 u1; s16 v1; } DLGSPR;
typedef struct { f32 f[5]; } DLGF5;
void Put_sprite_rotate();
/* dialog frame: top/bottom edge strips of 0x28 high tiles then the two rotated side strips (near-match) */
void Paint_square();
/* menu frame: filled body (Paint_square or 5-high strips) + four 20-unit edge strips + four 7x6 corners (near-match) */
void put_button_help(int a, int b, int c, u16 d);
extern u8 Friend_data[];
int Lbc_ConditionSearch();
int strlen();
#define X6 (*(u8 *)&a->x06)
#include "sysw.h"
extern u8 D_32D471[];
int mail_input();
void get_friend_page_num();
int Lbc_SendMail();
int SaveNetFile_ForLobby();
int Lb_select();
int net_Delete_FriendData();
int net_Check_FriendSuu();
void cpn_PutCNData();
void SaveNetFile_init();
void str_pause();
void str_fadein_vol();
void SetDialogYesNo();
int getFriendNow();
int getUserInfo();
#define XAF (*(u8 *)&a->x0A)
#define X06F (*(u8 *)&a->x06)
#define X0DF (*(u8 *)&a->x0D)
#define X12F (*(u8 *)&a->x12)
#define X13F (*((u8 *)a + 0x13))
typedef struct { u8 b[0x9A]; } BLK9A;
typedef struct { u8 b[0x2FC]; } BLK2FC;
#define XAC (*(u8 *)&a->x0A)
#define X12C (*(u8 *)&a->x12)
#define X13C (*((u8 *)a + 0x13))
extern u8 chatLogBuff[];
int Plaza_log_id_chk();
#define X0A (*(u8 *)&pNet->x0A)
#define X12P (*(u8 *)&pNet->x12)
extern u8 my_user_id[];
extern u8 RecvMailInfo[][0x9A];
int Plaza_add_friend();
#define XA (*(u8 *)&a->x0A)
#define X12 (*(u8 *)&a->x12)
#define X04 (*(u8 *)&a->x04)
#define X06 (*(u8 *)&a->x06)
extern char seekStr[];
int kb_input_ck_enter();
int plaza_req_input();
int Lb_cursorUD();
extern u8 D_3C73B4[];
int my_comment_input();
/* original bytes: build/raw/Plaza_add_friend.inc (config/c_rawfuncs.txt) */
#ifdef __MWERKS__
#endif
/* original bytes: build/raw/plaza_mailBox.inc (config/c_rawfuncs.txt) */
void plaza_moveMain(void)
{
    LB_NETW *a = pNet;

    switch (a->sel) {
    case 0:
        switch (plaza_enterLobby(a)) {
        case 3:
            tl_exit_sub_menu(0);
            break;
        }
        break;
    case 1:
        plaza_movePlaza(a);
        break;
    case 2:
        plaza_backToServer(a);
        break;
    case 3:
        switch (plaza_checkFriend(a)) {
        case 3:
            tl_exit_sub_menu(0);
            break;
        }
        break;
    case 5:
        plaza_searchAll(a);
        break;
    case 6:
        plaza_searchMember(a);
        break;
    case 7:
        plaza_checkMyStatus(a);
        break;
    case 8:
        switch (plaza_setMyComment(a)) {
        case 3:
            tl_exit_sub_menu(0);
            break;
        }
        break;
    case 4:
        switch (plaza_mailBox(a)) {
        case 3:
            tl_exit_sub_menu(0);
            break;
        }
        break;
    case 9:
        plaza_setChatMode(a);
        break;
    case 10:
        plaza_ReibunEdit(a);
        break;
    case 11:
        plaza_checkChatLog(a);
        break;
    case 12:
        plaza_capcomPage(a);
        break;
    case 13:
        plaza_logOut(a);
        break;
    case 14:
    default:
        plaza_chatMain(a);
        break;
    }
}

int plaza_enterLobby()
{
    u16 sw = Get_sw2(0);

    switch (pNet->step) {
    case 0:
        pNet->step++;
        break;
    case 1:
        pNet->x28 = Get_sw_on2(0);
        if (sw & 0x20) {
            pNet->step = 4;
            ClassInfo.lobby = X0Ap(pNet) + 1;
            cw[0x2C41] = X0Ap(pNet);
            cw[0x2C35] = 0;
            SetDialogData(0x15, 5);
            pNet->x0C = 1;
            cnWrap_SoundRequest(0);
            break;
        }
        if (sw & 0x40) {
            return 3;
        }
        if (sw & 0x2000) {
            if (X0Ap(pNet) % 7 != 0) {
                X0Ap(pNet) = X0Ap(pNet) - 1;
            } else {
                X0Ap(pNet) = X0Ap(pNet) / 7 * 7 + 6;
            }
            cnWrap_SoundRequest(1);
        } else if (sw & 0x1000) {
            X0Ap(pNet)++;
            if (X0Ap(pNet) >= 0xE || X0Ap(pNet) % 7 == 0) {
                X0Ap(pNet) -= 7;
            }
            cnWrap_SoundRequest(1);
        } else if (sw & 0xC00) {
            if (X0Ap(pNet) >= 7) {
                X0Ap(pNet) -= 7;
            } else {
                X0Ap(pNet) += 7;
            }
            cnWrap_SoundRequest(1);
        } else if (sw & 0x100) {
            pNet->x06 = *(u16 *)(D_3A1622 + X0Ap(pNet) * 0x15C);
            cnWrap_SoundRequest(6);
            memset(tl_member_buff, 0, 0x17E0);
            pNet->step++;
        }
        break;
    case 2:
        if (Lbs_GetLobbyMemberList(X0Ap(pNet)) == 1) {
            pNet->step++;
        }
        break;
    case 3:
        pNet->x28 = Get_sw_on2(0);
        if (sw & 0x40) {
            pNet->step = 1;
            cnWrap_SoundRequest(3);
        }
        break;
    case 4:
        pNet->x0C = 1;
        switch (Lbs_request_enter_lobby()) {
        case 0:
            pNet->step++;
            break;
        case 1:
            pNet->step = 6;
            ((LB_CW *)cw)->x30B6 = 0;
            ClassInfo.lobby = 0;
            SetDialogData_HTML(cw + 0x32D1);
            break;
        }
        break;
    case 5:
        pNet->x0C = 1;
        switch (Lbc_DownloadQuest()) {
        case 0:
            fade_set(0xA);
            To_EnterLobby();
            str_stop(0);
            str_stop(1);
            return 0;
        case 1:
            pNet->step = 6;
            SetDialogData(0xE, 5);
            break;
        }
        break;
    case 6:
        pNet->x0C = 1;
        if (sw & 0x20) {
            cw[0x2C32] = 0;
            cw[0x2C33] = 0;
            cw[0x2C34] = 0;
            return 1;
        }
        break;
    case 7:
        pNet->x0C = 1;
        if (sw & 0x20) {
            pNet->step = 0;
        }
        break;
    }
    return 2;
}

#undef X0A
#define X0A (*(u8 *)&a->x0A)
void plaza_movePlaza(a)
LB_NETW *a;
{
    u16 sw = Get_sw2(0);

    switch (a->step) {
    case 0:
        a->x28 = Get_sw_on2(0);
        if (sw & 0x20) {
            if (ClassInfo.plaza == X0A + 1) {
                a->step = 5;
                SetDialogData(0x13, 3);
                cnWrap_SoundRequest(7);
                return;
            }
            if (D_3A2940[X0A * 0x15C] == 3) {
                a->step++;
                cnWrap_SoundRequest(0, 3);
                return;
            }
            a->step = 5;
            SetDialogData(0x12, 3);
            cnWrap_SoundRequest(7);
            return;
        }
        if (sw & 0x40) {
            tl_exit_sub_menu(0);
            return;
        }
        if (sw & 0x2000) {
            if (X0A % 7 != 0) {
                X0A--;
            } else {
                X0A = X0A / 7 * 7 + 6;
                if (X0A >= 10) {
                    X0A = 9;
                }
            }
            cnWrap_SoundRequest(1);
            return;
        }
        if (sw & 0x1000) {
            X0A++;
            if (X0A >= 10 || X0A % 7 == 0) {
                if (X0A > 7) {
                    X0A = 7;
                } else {
                    X0A -= 7;
                }
            }
            cnWrap_SoundRequest(1);
            return;
        }
        if (sw & 0xC00) {
            a->x24 ^= 1;
            if (X0A >= 7) {
                X0A -= 7;
            } else {
                X0A += 7;
                if (X0A >= 10) {
                    X0A = 9;
                }
            }
            cnWrap_SoundRequest(1);
        }
        break;
    case 1:
        switch (Lbs_ExitAndEnterPlaza(X0A + 1)) {
        case 0:
            ClassInfo.plaza = X0A + 1;
            To_EnterPlaza();
            return;
        case 1:
            a->step++;
            SetDialogData(0x10, 3);
            return;
        }
        break;
    case 2:
        a->x0C = 1;
        if (sw & 0x20) {
            a->step++;
            cnWrap_SoundRequest(0);
            return;
        }
        break;
    case 3:
        switch (Lbs_ExitAndEnterPlaza(ClassInfo.plaza)) {
        case 0:
            To_EnterPlaza();
            return;
        case 1:
            a->step++;
            SetDialogData(0x11, 0);
            return;
        }
        break;
    case 4:
        a->x0C = 1;
        if (sw & 0x20) {
            To_TopMenu(1);
            cnWrap_SoundRequest(0);
            return;
        }
        break;
    case 5:
        a->x0C = 1;
        if (sw & 0x20) {
            a->step = 0;
            cnWrap_SoundRequest(3);
        }
        break;
    }
}

/* button help line of the plaza menus: which of the four buttons are shown for each menu / sub menu step (near-match) */

#undef X0A
#define X0A (*(u8 *)&pNet->x0A)
void plaza_backToServer(a)
LB_NETW *a;
{
    int sw = Get_sw2(0) & 0xFFFF;

    if (BsLbsCount > 1) {
        switch (a->step) {
        case 0:
            a->step++;
            SetDialogData(0x28, 2);
            SetDialogYesNo(1);
            break;
        case 1:
            a->x28 = Get_sw_on2(0);
            a->x0C = 1;
            switch (Lb_select()) {
            case 0:
                a->x10 = 2;
                fade_set(0xA);
                str_stop(0);
                str_stop(1);
                break;
            case 3:
                tl_exit_sub_menu(1);
                break;
            }
            break;
        }
    } else {
        switch (a->step) {
        case 0:
            a->step++;
            SetDialogData(0x14, 3);
            break;
        case 1:
            a->x0C = 1;
            if ((u16)sw & 0x20) {
                tl_exit_sub_menu(0);
            }
            break;
        }
    }
}

int getFriendNow(a, n, cnt)
LB_NETW *a;
u8 n;
s8 cnt;
{
    u8 *f = Friend_data + a->x24 * 0x150;
    u8 *sr;
    int k;

    switch (a->x04) {
    case 0:
        a->x04++;
        X6 = n;
    case 1:
        a->x04++;
        strcpy(SearchCondition.s, (char *)f + X6 * 0x30);
        if (SearchCondition.s[0] == 0 || X6 + a->x24 * 7 >= 0x32) {
            a->x04 = 0;
            return 0;
        }
        SearchCondition.len = strlen((char *)f + X6 * 0x30);
        SearchCondition.flag = 1;
        break;
    case 2:
        switch (Lbc_ConditionSearch(&SearchCondition, 1)) {
        case 0:
        case 1:
            sr = SearchResult;
            if (*sr != 0) {
                memcpy(tl_member_buff + X6 * 0x2FC + 0x280, sr + 4, 8);
                memcpy(tl_member_buff + X6 * 0x2FC + 0x288, SearchResult + 0xC, 0x11);
                memcpy(tl_member_buff + X6 * 0x2FC + 0x29A, SearchResult + 0x20, 0x40);
            }
            k = X6 + 1;
            X6 = k;
            if ((u8)k >= cnt) {
                a->x04 = 0;
                return 0;
            }
            a->x04--;
            break;
        }
        break;
    }
    return 2;
}

/* button help line of the plaza menus: which of the four buttons are shown for each menu / sub menu step (near-match) */

int getUserInfo() {
    switch (Lbs_SeekId()) {
    case 0:
        Lbc_RequestNetComment(CW->x2F80);
        return 0;
    case 1:
        return 1;
    default:
        return 2;
    }
}

int Lb_get_comment(a)
int a;
{
    int id = Lb_get_plID() & 0xFF;

    if (id != 0xFF) {
        memset(CW->comment[id], 0, 0x62);
        Lbc_RequestNetComment(a);
        return 1;
    }
    return 0;
}

int plaza_checkFriend()
{
    u16 sw = Get_sw2(0);
    LB_NETW *a = pNet;
    u8 *st = &a->step;
    int t;
    int i;
    int r;
    u8 *e;
    u8 k;
    u8 *p;
    u8 *pp;
    u8 *q;
    int kk;

    switch (*st) {
    case 0:
        (*st)++;
        *(u8 *)&pNet->x0A = 0;
        pNet->x04 = 0;
        pNet->x24 = 0;
        *(u8 *)&pNet->x0D = 1;
        get_friend_page_num(pNet);
        memset(tl_member_buff, 0, 0x17E0);
    case 1:
        switch (getFriendNow(pNet, 0, 7)) {
        case 0:
            pNet->step++;
            break;
        }
        break;
    case 2:
        pNet->x28 = Get_sw_on2(0);
        if (net_Check_FriendSuu(Friend_data, 0x32) == 0) {
            if (sw & 0x40) {
                if (*(u8 *)&pNet->x0D == 1) {
                    return 3;
                }
                pNet->step = 0xB;
                cw[0x2C08] = 0;
                system_w.softkey = 0;
                if (cw[0x35D5] != 0) {
                    str_stop(1, 1);
                    str_pause(0, 1);
                } else {
                    str_pause(0, 1);
                    str_pause(1, 1);
                    str_stop(1);
                }
                *(u8 *)&pNet->x12 = 3;
                cpn_PutCNData();
                SaveNetFile_init();
                break;
            }
            if (sw & 0x2A0) {
                cnWrap_SoundRequest(7);
            }
            break;
        }
        if (sw & 0x40) {
            if (*(u8 *)&pNet->x0D == 1) {
                return 3;
            }
            cnWrap_SoundRequest(3);
            pNet->step = 0xB;
            cw[0x2C08] = 0;
            system_w.softkey = 0;
            str_pause(0, 1);
            str_pause(1, 1);
            *(u8 *)&pNet->x12 = 3;
            cpn_PutCNData();
            SaveNetFile_init();
            break;
        }
        if (sw & 0x20) {
            if (*((s8 *)(tl_member_buff + 0x280) + *(u8 *)&pNet->x0A * 0x2FC) != 0) {
                pNet->step++;
                *(u8 *)&pNet->x06 = *(u8 *)&pNet->x0A;
                *(u8 *)&pNet->x12 = 0;
                strcpy((char *)cw + 0x2F80, (char *)Friend_data + (*(u8 *)&pNet->x06 + pNet->x24 * 7) * 0x30);
                memset(cw + 0x2B9C, 0, 0x62);
                cw[0x2F99] = 0;
                cnWrap_SoundRequest(0);
            } else {
                cnWrap_SoundRequest(7);
            }
        } else if (sw & 0x80) {
            pNet->step = 5;
            *(u8 *)&pNet->x06 = *(u8 *)&pNet->x0A;
            *(u8 *)&pNet->x0A = 0;
            e = Friend_data + (*(u8 *)&pNet->x06 + pNet->x24 * 7) * 0x30;
            strcpy((char *)cw + 0x2F80, (char *)e);
            strcpy((char *)cw + 0x2F88, (char *)e + 8);
            cw[0x2F99] = 0;
            cnWrap_SoundRequest(6);
        } else if (sw & 0x200) {
            pNet->step = 0xA;
            *(u8 *)&pNet->x06 = *(u8 *)&pNet->x0A;
            SetDialogData(0x16, 2);
            SetDialogYesNo(1);
            cnWrap_SoundRequest(6);
        } else if (sw & 0x800) {
            if (pNet->x26 > 1) {
                t = pNet->x24 - 1;
                pNet->x24 = t;
                if ((s16)t < 0) {
                    pNet->x24 = pNet->x26 - 1;
                }
                i = 0;
                p = tl_member_buff;
                *(u8 *)&pNet->x0A = 0;
                pNet->step = 1;
                do {
                    p[0x280] = 0;
                    memset(p + 0x29A, 0, 0x40);
                    i = (i + 1) & 0xFF;
                    p += 0x2FC;
                } while (i < 8);
                cnWrap_SoundRequest(1);
            }
        } else if (sw & 0x400) {
            t = pNet->x26;
            if (t > 1) {
                r = pNet->x24 + 1;
                pNet->x24 = r;
                if ((s16)r >= t) {
                    pNet->x24 = 0;
                }
                i = 0;
                p = tl_member_buff;
                *(u8 *)&pNet->x0A = 0;
                pNet->step = 1;
                do {
                    p[0x280] = 0;
                    memset(p + 0x29A, 0, 0x40);
                    i = (i + 1) & 0xFF;
                    p += 0x2FC;
                } while (i < 8);
                cnWrap_SoundRequest(1);
            }
        } else if (sw & 0x2000) {
            if (*(u8 *)&pNet->x0A != 0) {
                *(u8 *)&pNet->x0A = *(u8 *)&pNet->x0A - 1;
            } else {
                *(u8 *)&pNet->x0A = 6;
                if (*(s8 *)(Friend_data + (*(u8 *)&pNet->x0A + pNet->x24 * 7) * 0x30) == 0) {
                    *(u8 *)&pNet->x0A = net_Check_FriendSuu(Friend_data, 0x32) % 7 - 1;
                }
            }
            cnWrap_SoundRequest(1);
        } else if (sw & 0x1000) {
            t = *(u8 *)&pNet->x0A + 1;
            *(u8 *)&pNet->x0A = t;
            if ((u8)t >= 7) {
                *(u8 *)&pNet->x0A = 0;
            } else {
                r = *(u8 *)&pNet->x0A + pNet->x24 * 7;
                if (r >= 0x32 || *(s8 *)(Friend_data + r * 0x30) == 0) {
                    *(u8 *)&pNet->x0A = 0;
                }
            }
            cnWrap_SoundRequest(1);
        }
        break;
    case 3:
        switch (getUserInfo(a)) {
        case 0:
            pNet->step++;
            (*((u8 *)pNet + 0x13)) = 3;
            *(u8 *)&pNet->x12 = 0;
            break;
        case 1:
            SetDialogData_HTML(cw + 0x32D1);
            pNet->step = 9;
            break;
        }
        break;
    case 4:
        pNet->x28 = Get_sw_on2(0);
        if (sw & 0x40) {
            pNet->step = 2;
            *(u8 *)&pNet->x0A = *(u8 *)&pNet->x06;
            cnWrap_SoundRequest(3);
        } else if (sw & 0x80) {
            pNet->step++;
            *(u8 *)&pNet->x0A = 0;
            e = Friend_data + (*(u8 *)&pNet->x06 + pNet->x24 * 7) * 0x30;
            strcpy((char *)cw + 0x2F80, (char *)e);
            strcpy((char *)cw + 0x2F88, (char *)e + 8);
            cw[0x2F99] = 0;
            cnWrap_SoundRequest(6);
        } else if (sw & 0x200) {
            pNet->step = 0xA;
            *(u8 *)&pNet->x06 = *(u8 *)&pNet->x0A;
            SetDialogData(0x16, 2);
            SetDialogYesNo(1);
            cnWrap_SoundRequest(6);
        } else if (sw & 0x800) {
            k = *(u8 *)&pNet->x12;
            if (k == 0) {
                *(u8 *)&pNet->x12 = 2;
            } else {
                *(u8 *)&pNet->x12 = k - 1;
            }
            cnWrap_SoundRequest(1);
        } else if (sw & 0x400) {
            t = *(u8 *)&pNet->x12 + 1;
            *(u8 *)&pNet->x12 = t;
            if ((u8)t >= 3) {
                *(u8 *)&pNet->x12 = 0;
            }
            cnWrap_SoundRequest(1);
        }
        break;
    case 5:
        pNet->x28 = Get_sw_on2(0);
        if (sw & 0x40) {
            pNet->step = 2;
            *(u8 *)&pNet->x0A = *(u8 *)&pNet->x06;
            cnWrap_SoundRequest(3);
        } else if (sw & 0x20) {
            if (*(u8 *)&pNet->x0A == 0) {
                pNet->step++;
            } else {
                pNet->step = 7;
                pNet->x0C = 1;
                SetDialogData(0x2A, 2);
                SetDialogYesNo(1);
                *(BLK9A *)(cw + 0x3019) = *(BLK9A *)(cw + 0x2F7F);
            }
            cnWrap_SoundRequest(0);
        } else if ((sw & 0x3000) && *(s8 *)(cw + 0x2F99) != 0) {
            *(u8 *)&pNet->x0A = *(u8 *)&pNet->x0A ^ 1;
            cnWrap_SoundRequest(1);
        }
        break;
    case 6:
        if (mail_input(a, cw + 0x2F99) == 1) {
            KinshiYogo_chk(cw + 0x2F99);
            pNet->step = 5;
            if (*(s8 *)(cw + 0x2F99) != 0) {
                *(u8 *)&pNet->x0A = 1;
            } else {
                *(u8 *)&pNet->x0A = 0;
            }
        }
        break;
    case 7:
        a->x0C = 1;
        switch (Lb_select()) {
        case 0:
            pNet->step = 8;
            SetDialogData(0x2D, 5);
            break;
        case 3:
            pNet->step = 5;
            break;
        }
        break;
    case 8:
        a->x0C = 1;
        switch (Lbc_SendMail()) {
        case 0:
            SetDialogData(0x26, 3);
            pNet->step++;
            break;
        case 1:
            SetDialogData_HTML(cw + 0x32D1);
            pNet->step++;
            break;
        }
        break;
    case 9:
        a->x0C = 1;
        if (sw & 0x20) {
            *(u8 *)&pNet->x0A = *(u8 *)&pNet->x06;
            pNet->step = 2;
            cnWrap_SoundRequest(0);
        }
        break;
    case 10:
        a->x0C = 1;
        switch (Lb_select()) {
        case 0:
            *(u8 *)&pNet->x0D = 0;
            memcpy(cw + 0x2F80, Friend_data + (*(u8 *)&pNet->x0A + pNet->x24 * 7) * 0x30, 8);
            memcpy(cw + 0x2F88, Friend_data + (*(u8 *)&pNet->x0A + pNet->x24 * 7) * 0x30 + 8, 0x11);
            net_Delete_FriendData(Friend_data, (*(u8 *)&pNet->x0A + pNet->x24 * 7) & 0xFF, 0x32);
            kk = *(u8 *)&pNet->x0A;
            if (kk < 6) {
                pp = tl_member_buff + kk * 0x2FC;
                do {
                    memcpy(pp, tl_member_buff + (kk + 1) * 0x2FC, 0x2FC);
                    kk = (kk + 1) & 0xFF;
                    pp += 0x2FC;
                } while (kk < 6);
            }
            memset(tl_member_buff + (u8)kk * 0x2FC, 0, 0x2FC);
            get_friend_page_num(pNet);
            if (pNet->x24 >= pNet->x26) {
                pNet->x24 = pNet->x26 - 1;
            } else {
                q = (u8 *)&pNet->x0A;
                if (*q != 0) {
                    *q = *q - 1;
                }
            }
            cw[0x2C08] = 0;
            system_w.softkey = 0;
            pNet->step = 0xD;
            SetDialogData(0x17, 3);
            break;
        case 3:
            pNet->step = 2;
            break;
        }
        break;
    case 11:
        q = (u8 *)&a->x12;
        if (*q == 0) {
            switch (SaveNetFile_ForLobby()) {
            case 1:
            case -1:
                cw[0x2C08] = 1;
                system_w.softkey = 1;
                str_pause(0, 0);
                str_pause(1, 0);
                if (cw[0x35D5] != 0) {
                    str_fadein_vol(0, 0x1E, D_32D471[*(u8 *)0x3F3404 * 2]);
                }
                return 3;
            }
            break;
        }
        *q = *q - 1;
        break;
    case 12:
        a->x0C = 1;
        if (sw & 0x20) {
            cnWrap_SoundRequest(3);
            return 3;
        }
        break;
    case 13:
        a->x0C = 1;
        if (sw & 0x20) {
            pNet->step++;
            cnWrap_SoundRequest(0);
        }
        break;
    case 14:
        switch (getFriendNow(a, 6, 1)) {
        case 0:
            pNet->step = 2;
            break;
        }
        break;
    }
    return 2;
}

/* button help line of the plaza menus: which of the four buttons are shown for each menu / sub menu step (near-match) */

int mail_input(a, buf)
LB_NETW *a;
int buf;
{
    s8 r;

    Get_sw(0);
    Get_kb_input();
    switch (a->x05) {
    case 0:
        a->x05++;
        SoftKeyboard_pos_set(100.0f, 0x140);
        SoftKeyboard_set(1, 0xE, 0x7E, buf);
        break;
    case 1:
        r = SoftKeyboard_move(buf, *(s16 *)0x3F3710, *(s16 *)0x3F3714);
        switch (r) {
        case 1:
        case -1:
            a->x05++;
            break;
        }
        break;
    case 2:
        SoftKeyboard_exit();
        a->x05 = 0;
        return 1;
    }
    return 0;
}

void get_friend_page_num(a)
LB_NETW *a;
{
    a->x26 = net_Check_FriendSuu(Friend_data, 0x32);
    if (a->x26 % 7 != 0) {
        a->x26 = a->x26 / 7 + 1;
        return;
    }
    a->x26 = a->x26 / 7;
    if (a->x26 == 0) {
        a->x26 = 1;
    }
}

static int get_page_num(a, b)
s16 a;
s16 b;
{
    int r;

    if (a % b != 0) {
        return (s16)(a / b + 1);
    }
    r = (s16)(a / b);
    if (r == 0) {
        r = 1;
    }
    return r;
}

void plaza_searchAll(void)
{
    u16 sw = Get_sw2(0);
    LB_NETW *a = pNet;
    u8 *st = &a->step;
    int n;
    int n2;
    int t;
    LB_NETW *p;
    u8 *pa;
    int v;

    switch (*st) {
    case 0:
        *st = 5;
        SearchCondition.flag = 1;
        SetDialogData(0x18, 5);
        break;
    case 5:
        a->x0C = 1;
        switch (Lbc_ConditionSearch(&SearchCondition, 0)) {
        case 0:
            pNet->step++;
            pNet->x26 = *SearchResult / 7;
            pNet->x24 = 0;
            X0A = 0;
            if (*SearchResult % 7 != 0) {
                pNet->x26++;
            }
            break;
        case 1:
            SetDialogData(0x2B, 0);
            pNet->step = 7;
            break;
        }
        break;
    case 6:
        pNet->x28 = Get_sw_on2(0);
        if (sw & 0x40) {
            tl_exit_sub_menu(0);
            break;
        }
        if (sw & 0x200) {
            pNet->step = 8;
            cnWrap_SoundRequest(6);
            break;
        }
        if (sw & 0x20) {
            if (memcmp(SearchResult + (X0A + pNet->x24 * 7) * 0x5C + 4, my_user_id, 8) == 0) {
                cnWrap_SoundRequest(7);
                break;
            }
            pNet->step = 9;
            strcpy((char *)cw + 0x2F80, (char *)SearchResult + (X0A + pNet->x24 * 7) * 0x5C + 4);
            memset(cw + 0x2B9C, 0, 0x62);
            cnWrap_SoundRequest(6);
            break;
        }
        if (sw & 0x800) {
            if (pNet->x26 > 1) {
                t = pNet->x24 - 1;
                pNet->x24 = t;
                if ((s16)t < 0) {
                    pNet->x24 = pNet->x26 - 1;
                }
                X0A = 0;
                cnWrap_SoundRequest(1);
            }
        } else if (sw & 0x400) {
            n = pNet->x26;
            if (n > 1) {
                t = pNet->x24 + 1;
                pNet->x24 = t;
                if ((s16)t >= n) {
                    pNet->x24 = 0;
                }
                X0A = 0;
                cnWrap_SoundRequest(1);
            }
        } else {
            if (sw & 0x2000) {
                p = pNet;
                pa = (u8 *)&p->x0A;
                v = *pa;
                if (v == 0) {
                    if (p->x24 == p->x26 - 1) {
                        n = *SearchResult % 7;
                        if (n == 0) {
                            *pa = 6;
                        } else {
                            *pa = n - 1;
                        }
                    } else {
                        *pa = 6;
                    }
                } else {
                    *pa = v - 1;
                }
                cnWrap_SoundRequest(1);
                break;
            }
            if (sw & 0x1000) {
                X0A++;
                if (X0A >= 7) {
                    X0A = 0;
                }
                if (pNet->x24 == pNet->x26 - 1) {
                    n2 = *SearchResult % 7;
                    if (n2 != 0 && X0A >= n2) {
                        X0A = 0;
                    }
                }
                cnWrap_SoundRequest(1);
                break;
            }
        }
        break;
    case 7:
        a->x0C = 1;
        if (sw & 0x20) {
            pNet->step = 6;
            cnWrap_SoundRequest(0);
        }
        break;
    case 8:
        switch (Plaza_add_friend(SearchResult + ((*(u8 *)&a->x0A) + a->x24 * 7) * 0x5C + 4)) {
        case 0:
        case 1:
            pNet->step = 6;
            break;
        }
        break;
    case 9:
        switch (getUserInfo(a)) {
        case 0:
            pNet->step++;
            X12P = 0;
            break;
        case 1:
            SetDialogData_HTML(cw + 0x32D1);
            pNet->step = 7;
            break;
        }
        break;
    case 10:
        pNet->x28 = Get_sw_on2(0);
        if (sw & 0x800) {
            if (X12P == 0) {
                X12P = 2;
            } else {
                X12P = X12P - 1;
            }
            cnWrap_SoundRequest(1);
            break;
        }
        if (sw & 0x400) {
            t = X12P + 1;
            X12P = t;
            if ((u8)t > 2) {
                X12P = 0;
            }
            cnWrap_SoundRequest(1);
            break;
        }
        if (sw & 0x40) {
            pNet->step = 6;
            cnWrap_SoundRequest(3);
        }
        break;
    }
}

void plaza_searchMember(a)
LB_NETW *a;
{
    u16 sw = Get_sw2(0);
    int n;
    int t;

    switch (a->step) {
    case 0:
        a->step++;
        X06 = 0xFF;
        XA = 0;
        break;
    case 1:
        a->x28 = Get_sw_on2(0);
        if ((sw & 0x20) || kb_input_ck_enter() == 1) {
            if (X04 != 0 && X04 != 1) {
                a->step = 3;
                X06 = XA;
                XA = 6;
            } else {
                seekStr[0] = 0;
                a->step++;
            }
            cnWrap_SoundRequest(0);
        } else if (sw & 0x40) {
            tl_exit_sub_menu(0);
        } else if (sw & 0x400) {
            t = X04 + 1;
            X04 = t;
            if ((u8)t >= 4) {
                X04 = 0;
            }
            XA = 0;
            cnWrap_SoundRequest(1);
        } else if (sw & 0x800) {
            if (X04 == 0) {
                X04 = 3;
            } else {
                X04 = X04 - 1;
            }
            XA = 0;
            cnWrap_SoundRequest(1);
        }
        if (X04 != 0 && X04 != 1) {
            XA = Lb_cursorUD(XA, 5);
        }
        break;
    case 2:
        if (plaza_req_input(a, seekStr) == 1) {
            if (strlen(seekStr) != 0) {
                XA = 6;
                a->step++;
                break;
            }
            a->step = 1;
        }
        break;
    case 3:
        a->x28 = Get_sw_on2(0);
        if (sw & 0x20) {
            a->step = 5;
            SetDialogData(0x18, 5);
            cnWrap_SoundRequest(0);
            switch (X04) {
            case 1:
                strcpy(SearchCondition.s, seekStr);
                SearchCondition.len = strlen(seekStr);
                SearchCondition.flag = 1;
                a->x0E = 1;
                if (SearchCondition.len < 6) {
                    SetDialogData(0x1B, 3);
                    a->step = 4;
                }
                break;
            case 0:
                a->x0E = 1;
                if (strlen(seekStr) == 0) {
                    SetDialogData(0x1C, 3);
                    a->step = 4;
                    break;
                }
                strcpy(SearchCondition.s, seekStr);
                SearchCondition.len = strlen(seekStr);
                SearchCondition.flag = 2;
                break;
            case 2:
                a->x0E = 1;
                SearchCondition.s[0] = X06;
                SearchCondition.len = 1;
                SearchCondition.flag = 3;
                break;
            case 3:
                a->x0E = 1;
                ((u8 *)&SearchCondition)[4] = X06 * 4 + 1;
                ((u8 *)&SearchCondition)[5] = X06 * 4 + 4;
                ((u8 *)&SearchCondition)[1] = 1;
                ((u8 *)&SearchCondition)[0] = 6;
                break;
            }
        } else if (sw & 0x40) {
            a->step = 1;
            XA = X06;
            cnWrap_SoundRequest(3);
        }
        break;
    case 4:
        a->x0C = 1;
        if (sw & 0x20) {
            a->step = 0;
            XA = X06;
            cnWrap_SoundRequest(0);
        }
        break;
    case 5:
        a->x0C = 1;
        switch (Lbc_ConditionSearch(&SearchCondition, 1)) {
        case 0:
            if (*SearchResult == 0) {
                a->step = 7;
                SetDialogData(0x19, 3);
                break;
            }
            a->step = 6;
            a->x26 = *SearchResult / 7;
            a->x24 = 0;
            XA = 0;
            if (*SearchResult % 7 != 0) {
                a->x26++;
            }
            break;
        case 1:
            a->step = 7;
            SetDialogData(0x1A, 3);
            break;
        }
        break;
    case 6:
        a->x28 = Get_sw_on2(0);
        if (sw & 0x40) {
            tl_exit_sub_menu(0);
            break;
        }
        if (sw & 0x200) {
            a->step = 8;
            cnWrap_SoundRequest(6);
            break;
        }
        if (sw & 0x20) {
            if (memcmp(SearchResult + (XA + a->x24 * 7) * 0x5C + 4, my_user_id, 8) == 0) {
                cnWrap_SoundRequest(7);
                break;
            }
            a->step = 9;
            strcpy((char *)cw + 0x2F80, (char *)SearchResult + (XA + a->x24 * 7) * 0x5C + 4);
            memset(cw + 0x2B9C, 0, 0x62);
            cnWrap_SoundRequest(6);
            break;
        }
        if (sw & 0x800) {
            if (a->x26 > 1) {
                t = a->x24 - 1;
                a->x24 = t;
                if ((s16)t < 0) {
                    a->x24 = a->x26 - 1;
                }
                XA = 0;
                cnWrap_SoundRequest(1);
            }
        } else if (sw & 0x400) {
            n = a->x26;
            if (n > 1) {
                t = a->x24 + 1;
                a->x24 = t;
                if ((s16)t >= n) {
                    a->x24 = 0;
                }
                XA = 0;
                cnWrap_SoundRequest(1);
            }
        } else {
            if (sw & 0x2000) {
                if (XA == 0) {
                    if (a->x24 == a->x26 - 1) {
                        n = *SearchResult % 7;
                        if (n == 0) {
                            XA = 6;
                        } else {
                            XA = n - 1;
                        }
                    } else {
                        XA = 6;
                    }
                } else {
                    XA = XA - 1;
                }
                cnWrap_SoundRequest(1);
                break;
            }
            if (sw & 0x1000) {
                XA++;
                if (XA >= 7) {
                    XA = 0;
                }
                if (a->x24 == a->x26 - 1) {
                    n = *SearchResult % 7;
                    if (n != 0 && XA >= n) {
                        XA = 0;
                    }
                }
                cnWrap_SoundRequest(1);
                break;
            }
        }
        break;
    case 7:
        a->x0C = 1;
        if (sw & 0x20) {
            a->step = 1;
            XA = 0;
            cnWrap_SoundRequest(0);
        }
        break;
    case 8:
        switch (Plaza_add_friend(SearchResult + (XA + a->x24 * 7) * 0x5C + 4)) {
        case 0:
        case 1:
            a->step = 6;
            break;
        }
        break;
    case 9:
        switch (getUserInfo(a)) {
        case 0:
            a->step++;
            X12 = 0;
            break;
        case 1:
            SetDialogData_HTML(cw + 0x32D1);
            a->step = 11;
            break;
        }
        break;
    case 10:
        a->x28 = Get_sw_on2(0);
        if (sw & 0x800) {
            if (X12 == 0) {
                X12 = 2;
            } else {
                X12 = X12 - 1;
            }
            cnWrap_SoundRequest(1);
            break;
        }
        if (sw & 0x400) {
            t = X12 + 1;
            X12 = t;
            if ((u8)t > 2) {
                X12 = 0;
            }
            cnWrap_SoundRequest(1);
            break;
        }
        if (sw & 0x40) {
            a->step = 6;
            cnWrap_SoundRequest(3);
        }
        break;
    case 11:
        a->x0C = 1;
        if (sw & 0x20) {
            a->step = 6;
            cnWrap_SoundRequest(0);
        }
        break;
    }
}

/* button help line of the plaza menus: which of the four buttons are shown for each menu / sub menu step (near-match) */

int my_comment_input(buf)
int buf;
{
    s8 r;

    Get_sw(0);
    Get_kb_input();
    switch (pNet->x05) {
    case 0:
        pNet->x05++;
        SoftKeyboard_pos_set(100.0f, 0x140);
        SoftKeyboard_set(1, 0xE, 0x61, buf);
        break;
    case 1:
        r = SoftKeyboard_move(buf, *(s16 *)0x3F3710, *(s16 *)0x3F3714);
        switch (r) {
        case 1:
        case -1:
            pNet->x05++;
            break;
        }
        break;
    case 2:
        SoftKeyboard_exit();
        pNet->x05 = 0;
        return 1;
    }
    return 0;
}

void plaza_checkMyStatus(void) {
    int sw = Get_sw2(0) & 0xFFFF;
    s16 *p;
    int t;
    s16 v;

    switch (pNet->step) {
    case 0:
        pNet->x24 = 0;
        pNet->step++;
        CW->x30B4 = ClassInfo.plaza;
        CW->x30B6 = ClassInfo.lobby;
        break;
    case 1:
        pNet->x28 = Get_sw_on2(0);
        t = sw & 0xFFFF;
        if (t & 0x800) {
            p = &pNet->x24;
            if (*p == 0) {
                *p = 2;
            } else {
                *p = *p - 1;
            }
            cnWrap_SoundRequest(1);
        } else if (t & 0x400) {
            v = pNet->x24 + 1;
            pNet->x24 = v;
            if (v > 2) {
                pNet->x24 = 0;
            }
            cnWrap_SoundRequest(1);
        } else if (t & 0x40) {
            tl_exit_sub_menu(0);
        }
        break;
    }
}

int plaza_setMyComment(void) {
    u16 sw = Get_sw2(0);
    LB_NETW *n = pNet;
    u8 *stp = &n->step;

    switch (*stp) {
    case 0:
        (*stp)++;
        memcpy(CW->comment[game_w.master], D_3C73B4, 0x62);
        break;
    case 1:
        if (sw & 0x20) {
            (*stp)++;
            cnWrap_SoundRequest(0);
            break;
        }
        if (sw & 0x40) {
            memcpy(D_3C73B4, CW->comment[game_w.master], 0x62);
            return 3;
        }
        break;
    case 2:
        if (my_comment_input(CW->comment[game_w.master]) == 1) {
            KinshiYogo_chk(CW->comment[game_w.master]);
            memcpy(D_3C73B4, CW->comment[game_w.master], 0x62);
            pNet->step--;
        }
        break;
    }
    return 2;
}

/* original bytes: build/raw/Plaza_add_friend.inc (config/c_rawfuncs.txt); C near-match in lb_pz12_nm.c */
asm int Plaza_add_friend()
{
#include "Plaza_add_friend.inc"
}

int plaza_req_input(a, buf)
LB_NETW *a;
int buf;
{
    s8 r;

    Get_sw(0);
    switch (a->x05) {
    case 0:
        a->x05++;
        SoftKeyboard_pos_set(100.0f, 0x140);
        if (a->x04 == 1) {
            SoftKeyboard_set(0, 6, 6, buf);
        } else {
            SoftKeyboard_set(3, 0xF, 8, buf);
        }
        break;
    case 1:
        r = SoftKeyboard_move(buf, *(s16 *)0x3F3710, *(s16 *)0x3F3714);
        switch (r) {
        case 0:
            break;
        case 1:
            a->x05++;
            a->x06 = 0;
            break;
        case -1:
            a->x05++;
            a->x06 = 0;
            break;
        }
        break;
    case 2:
        SoftKeyboard_exit();
        a->x05 = 0;
        return 1;
    }
    return 0;
}

int getHandleFromID(a)
LB_NETW *a;
{
    switch (a->x04) {
    case 0:
        a->x04++;
        a->x06 = 0;
    case 1:
        a->x04++;
        strcpy(SearchCondition.s, CW->x2F80);
        SearchCondition.len = strlen(CW->x2F80);
        SearchCondition.flag = 1;
        break;
    case 2:
        switch (Lbc_ConditionSearch(&SearchCondition, 1)) {
        case 0:
            if (SearchResult[0] != 0) {
                memcpy(CW->x2F80 + 8, SearchResult + 0xC, 0x11);
                return 0;
            }
            SetDialogData(0x29, 3);
            return 1;
        case 1:
            SetDialogData(0x29, 3);
            return 1;
        }
        break;
    }
    return 2;
}

/* original bytes: build/raw/plaza_mailBox.inc; C near-match (4 off) in lb_pz14_nm.c */
asm int plaza_mailBox()
{
#include "plaza_mailBox.inc"
}

void plaza_setChatMode(a)
LB_NETW *a;
{
    u16 sw;
    int v;
    int t;
    u8 *e;

    sw = Get_sw2(0);
    switch (a->step) {
    case 0:
        a->step++;
        XAC = 0;
        v = Plaza_log_id_chk(chatLogBuff);
        X12C = v;
        a->x24 = 0;
        a->x26 = get_page_num((s16)v, 7);
        break;
    case 1:
        v = Plaza_log_id_chk(chatLogBuff);
        X12C = v;
        a->x26 = get_page_num((s16)v, 7);
        if (a->x24 >= a->x26) {
            a->x24 = a->x26 - 1;
            XAC = 0;
        } else {
            if (v % 7 < XAC) {
                XAC = XAC - 1;
            }
        }
        a->x28 = Get_sw_on2(0);
        if (sw & 0x100) {
            cnWrap_SoundRequest(6);
            a->step = 3;
            X13C = XAC;
            XAC = 0;
            chatListFlag = 0;
            a->idx = 0;
            break;
        }
        if (sw & 0x20) {
            cnWrap_SoundRequest(6);
            if (XAC == 0) {
                Lb_clearChatList();
                break;
            }
            e = *(u8 **)(chatLogBuff + (XAC - 1 + a->x24 * 7) * 4);
            if (Lb_checkChatID(e + 0x44) == 1) {
                Lb_clearChatID(e + 0x44);
                break;
            }
            if (Lb_addChatMember(e + 0x44, e + 0x4C) == -1) {
                a->step = 2;
                SetDialogData(0x2F, 3);
            }
        } else {
            if (sw & 0x40) {
                tl_exit_sub_menu(0);
                break;
            }
            if (sw & 0x2000) {
                if (X12C == 0) {
                    cnWrap_SoundRequest(7);
                } else if (XAC == 0) {
                    cnWrap_SoundRequest(1);
                    if (a->x24 == a->x26 - 1) {
                        t = X12C % 7;
                        if (t == 0) {
                            XAC = 7;
                        } else {
                            XAC = t;
                        }
                    } else {
                        XAC = 7;
                    }
                } else {
                    cnWrap_SoundRequest(1);
                    XAC = XAC - 1;
                }
                cnWrap_SoundRequest(1);
                break;
            }
            if (sw & 0x1000) {
                if (X12C == 0) {
                    cnWrap_SoundRequest(7);
                } else {
                    cnWrap_SoundRequest(1);
                    XAC++;
                    if (XAC >= 8 || XAC + a->x24 * 7 >= X12C + 1) {
                        XAC = 0;
                    }
                }
                cnWrap_SoundRequest(1);
                break;
            }
            if (sw & 0x800) {
                if (a->x26 > 1) {
                    XAC = 0;
                    t = a->x24 - 1;
                    a->x24 = t;
                    if ((s16)t < 0) {
                        a->x24 = a->x26 - 1;
                    }
                    cnWrap_SoundRequest(1);
                }
            } else {
                if ((sw & 0x400) && a->x26 > 1) {
                    XAC = 0;
                    t = a->x24 + 1;
                    a->x24 = t;
                    if ((s16)t >= a->x26) {
                        a->x24 = 0;
                    }
                    cnWrap_SoundRequest(1);
                }
            }
        }
        break;
    case 2:
        a->x0C = 1;
        if (sw & 0x20) {
            a->step = 1;
            cnWrap_SoundRequest(0);
        }
        break;
    case 3:
        switch (lb_chatMemberCheck()) {
        case 0:
        case 3:
            a->step++;
            break;
        }
        break;
    case 4:
        if (sw & 0x40) {
            XAC = X13C;
            a->step = 1;
            cnWrap_SoundRequest(3);
            break;
        }
        if (sw & 0x20) {
            cnWrap_SoundRequest(6);
            if (XAC == 0) {
                Lb_clearChatList();
                break;
            }
            Lb_clearChatMember((s8)(XAC - 1));
            a->step = 3;
            chatListFlag = 0;
            a->idx = 0;
            XAC = XAC - 1;
            break;
        }
        XAC = Lb_cursorUD(XAC, cw[0x32BE] + 1);
        break;
    }
}

int lb_chatMemberCheck(void) {
    u8 *p;
    s16 i;
    LB_NETW *n;

    switch (pNet->x04) {
    case 0:
        p = (u8 *)chatIDList + pNet->idx * 8;
        if ((s8)p[0] != 0) {
            memcpy(CW->x2F80, p, 8);
            pNet->x04++;
            break;
        }
        return 0;
    case 1:
        switch (Lbs_SeekId()) {
        case 0:
            if (ClassInfo.plaza == CW->x30B4 && CW->x30B6 == 0) {
                n = pNet;
                chatListFlag = chatListFlag | (1 << n->idx);
            } else {
                n = pNet;
                chatListFlag = chatListFlag & ~(1 << n->idx);
            }
            i = n->idx + 1;
            n->idx = i;
            if (i < 8) {
                pNet->x04 = 0;
                break;
            }
            return 0;
        case 1:
            n = pNet;
            chatListFlag = chatListFlag & ~(1 << n->idx);
            i = n->idx + 1;
            n->idx = i;
            if (i < 8) {
                pNet->x04 = 0;
                break;
            }
            return 0;
        }
        break;
    }
    return 2;
}

int Lb_checkChatID(id)
u8 *id;
{
    s8 i;
    u8 *p = (u8 *)chatIDList;

    for (i = 0; ; ) {
        if (memcmp(p, id, 8) == 0) {
            return 1;
        }
        i++;
        p += 8;
        if (i >= 7) {
            return 0;
        }
    }
}

