/* lb_c515 - agent C round 5 0x005BC6C0-0x005BC9A8: Lbc_GuestReadRoom (guest view of a room: read rule allocation + member list; u16 prototypes make the call sites mask the room number, one shared return 2; locals declared in reverse register order). */
#include "lobby_a.h"
typedef struct { u8 pad0[0x54]; u8 n; } RRH;
extern RRH RoomRule;
extern u8 RoomInfo[];
extern u8 join_member[];
extern char CallBack_Result_Lobby_GuestRoomMember[];
extern char CallBack_Result_Lobby_GuestRuleAllocation[];
typedef struct { u8 pad0[0x2C35]; u8 step; } CWS_gr;
#define CWX ((CWS_gr *)cw)
int cnLBS_Get_RoomRuleAllocation(u16, u8 *);
int cnLBS_Read_RoomMemberList(u16, char *);
s32 Lbc_GuestReadRoom(s32 arg0) {
    u8 buf[0x294B0];
    s32 off;
    s32 r;
    u8 st;
    u8 *stp;
    s32 j;
    u8 t8;
    u8 *s;
    u8 *d;
    u8 *s2;
    u8 *d2;
    u8 *jm;
    s32 i;

    st = CWX->step;
    stp = &CWX->step;
    switch (st) {
    case 0:
        *stp = st + 1;
        memset(&RoomRule, 0, 0x29555);
        off = (arg0 & 0xFFFF) * 0x15C;
        strcpy((char *)&RoomRule + 0x12, (char *)RoomInfo + off + 0x14);
        ((u8 *)&RoomRule)[0x52] = RoomInfo[off + 0x11];
        break;
    case 1:
        *stp = st + 1;
        CallBackWaitInit();
        cw[0x2C45] = 0x13;
        cnLBS_Read_RoomRuleAllocation(((arg0 & 0xFFFF) + 1) & 0xFFFF, CallBack_Result_Lobby_GuestRuleAllocation, 0x13);
        break;
    case 2:
        Check_CallBackWait();
        break;
    case 3:
        *stp = st + 1;
        r = (arg0 & 0xFFFF) + 1;
        cnLBS_Get_RoomRuleAllocation(r, buf);
        *(u8 *)&RoomRule = buf[0];
        ((u8 *)&RoomRule)[1] = buf[1];
        RoomRule.n = buf[3];
        i = 0;
        if (0 < RoomRule.n) {
            s = buf;
            d = (u8 *)&RoomRule;
            do {
                strcpy(d + 0x56, s + 5);
                d[0x98] = s[0x46];
                d[0x97] = s[0x47];
                t8 = s[0x48];
                d[0x9A] = t8;
                d[0x99] = t8;
                j = 0;
                if (0 < d[0x97]) {
                    s2 = s;
                    d2 = d;
                    do {
                        strcpy(d2 + 0xBD, s2 + 0x69);
                        j++;
                        s2 += 0x41;
                        d2 += 0x41;
                    } while (j < d[0x97]);
                }
                i++;
                s += 0x14A5;
                d += 0x14A8;
            } while (i < RoomRule.n);
        }
        CallBackWaitInit();
        cw[0x2C45] = 0x14;
        cnLBS_Read_RoomMemberList(r, CallBack_Result_Lobby_GuestRoomMember);
        break;
    case 4:
        Check_CallBackWait();
        break;
    case 5:
        *stp = 0;
        memset(join_member, 0, 0xBF0);
        i = 0;
        jm = join_member;
        do {
            cnLBS_Get_RoomMemberList(i & 0xFFFF, jm + 0x280, jm + 0x288, jm + 0x29A);
            i++;
            jm += 0x2FC;
        } while (i < 4);
        cw[0x2C3A] = 1;
        cw[0x32BF] = 3;
        F(s8, pNet, 6) = 2;
        return 0;
    }
    return 2;
}
