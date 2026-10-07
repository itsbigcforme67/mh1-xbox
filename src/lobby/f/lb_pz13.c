/* lb_pz13 - lobby.bin 0x005950C0-0x00595F64: plaza_checkFriend(), friend list menu (online/plaza). Matches (rebuild OK). */
#pragma readonly_strings on
#include "lbui_proto.h"
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
extern u8 Friend_data[];
extern u8 tl_member_buff[];
#define XAF (*(u8 *)&a->x0A)
#define X06F (*(u8 *)&a->x06)
#define X0DF (*(u8 *)&a->x0D)
#define X12F (*(u8 *)&a->x12)
#define X13F (*((u8 *)a + 0x13))
typedef struct { u8 b[0x9A]; } BLK9A;
typedef struct { u8 b[0x2FC]; } BLK2FC;
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
