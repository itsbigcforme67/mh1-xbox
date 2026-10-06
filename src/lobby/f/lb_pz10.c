/* lb_pz10 - lobby.bin 0x00596850-0x0059722C: plaza_searchMember(a), member search by name / handle / class / level range (x04 = search kind, X06 = saved cursor), then the same result paging and add-friend steps as plaza_searchAll. Header edit: LB_NETW.x0E carved from _pad0E. SearchCondition case 3: the four stores are written as u8 array stores in the order [4],[5],[1],[0] (found by permutation). */
#pragma readonly_strings on
#include "lbui_proto.h"
#define XA (*(u8 *)&a->x0A)
#define X12 (*(u8 *)&a->x12)
#define X04 (*(u8 *)&a->x04)
#define X06 (*(u8 *)&a->x06)
extern u8 my_user_id[];
extern char seekStr[];
int Plaza_add_friend();
int getUserInfo();
int kb_input_ck_enter();
int plaza_req_input();
int Lb_cursorUD();
int strlen();

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
