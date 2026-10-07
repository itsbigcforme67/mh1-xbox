/* lb_pz06 - lobby.bin 0x00594D70-0x00594FD0: getFriendNow(a, n, cnt), looks up the friend names one by one through the condition search (a->x06 is read as u8). All exits go through one `return 2`; the one-case switch on the search result is the original's. */
#pragma readonly_strings on
#include "lbui_proto.h"
extern u8 Friend_data[];
extern u8 tl_member_buff[];
int Lbc_ConditionSearch();
int strlen();

#define X6 (*(u8 *)&a->x06)
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
