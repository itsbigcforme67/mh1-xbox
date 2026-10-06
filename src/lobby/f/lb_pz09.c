/* lb_pz09 - lobby.bin 0x005961D0-0x00596850: plaza_searchAll(), friend-less 'search all members' page: result paging (7 rows per page, x24 page, x26 pages), add friend, user info. Statement-level lessons: pNet is cached in a before the first call only (Get_sw2 first), the 0x1000 branch needs its own n2 for the register order. */
#pragma readonly_strings on
#include "lbui_proto.h"
#define X0A (*(u8 *)&pNet->x0A)
#define X12 (*(u8 *)&pNet->x12)
extern u8 my_user_id[];
int Plaza_add_friend();
int getUserInfo();

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
            X12 = 0;
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
            pNet->step = 6;
            cnWrap_SoundRequest(3);
        }
        break;
    }
}
