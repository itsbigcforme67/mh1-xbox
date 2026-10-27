/* G online (PC): Lbc_GetRoomRule (G lobby 0x581E70; HAND-FIXED: m2c took G's 0x294A4-byte stack buffer at sp+0x70 for a
 * stack argument, so the draft copied the rule table over its own frame). Written from G's asm after the MH1 PC version
 * (src/lobby/f/lb_online_nm.c): the rules of this player's room. Step 1 asks the server (G's rule allocation, 0x5BCEE0:
 * the 66xx requests), step 3 copies the rule table (0x14A5 bytes an entry on the wire side, 0x14A8 in RoomRule) into
 * RoomRule and returns 1. G differs from MH1 in: cw offsets (step cw+0xB9D, wait kind cw+0xBA9 = 0x20, cw+0xBA2), no
 * MH1 cw+0x32BF store, and the pNet flag at G's +0x27 (MH1 +6). docs/network.md 3.12. */
#include "../gonl.h"

int cnLbc_CheckInFloorOrder();
int Check_CallBackWait();
int lobby_func_005BCEE0(int room, void *cb);           /* G's cnLBS_Read_RoomRuleAllocation (src/lobby/cnet/cnet_g.c) */
int cnLBS_Get_RoomRuleAllocation(int unused, void *d);
void CallBack_Result_RuleAllocation();
void *memset();
char *strcpy();
extern u8 *cw;
extern u8 *pNet;
extern u8 RoomRule[];

s32 Lbc_GetRoomRule(void)
{
    extern u8 lb_room_rule_buf[];       /* G keeps it on its 0x29520-byte stack frame (sp+0x70); one shared buffer (lb_online_nm.c) */
    u8 *buf = lb_room_rule_buf;
    int room = cnLbc_CheckInFloorOrder(2) & 0xFFFF;
    u8 *stp = cw + 0xB9D;
    int i, j, a, t;
    u8 *s, *d;

    switch (*stp) {
    case 0:
        *stp = 1;
        /* fallthrough (G 0x581EE0) */
    case 1:
        *stp += 1;
        *(s32 *)(cw + 0xFA68) = 0xE10;  /* CallBackWaitInit */
        cw[0xFA65] = 0;
        cw[0xBA9] = 0x20;
        lobby_func_005BCEE0(room, (void *)CallBack_Result_RuleAllocation);
        break;
    case 2:
        Check_CallBackWait();
        break;
    case 3:
        *stp = 0;
        cw[0xBA2] = 0;
        cnLBS_Get_RoomRuleAllocation(room, buf);
        memset(RoomRule, 0, 0x29555);
        RoomRule[0] = buf[0];
        RoomRule[1] = buf[1];
        RoomRule[0x54] = buf[3];
        s = buf;
        d = RoomRule;
        for (i = 0; i < RoomRule[0x54]; i++) {
            strcpy((char *)d + 0x56, (char *)s + 5);
            d[0x98] = s[0x46];
            d[0x97] = s[0x47];
            d[0x9A] = d[0x99] = s[0x48];
            for (j = 0; j < d[0x97]; j++)
                strcpy((char *)d + 0xBD + 0x41 * j, (char *)s + 0x69 + 0x41 * j);
            for (a = 0; a < 0x20; a++) {
                d[0x8DD + a] = s[0x889 + a];
                for (t = 0; t < 0x20; t++) {
                    d[0x60 * a + 0x8FD + 3 * t] = s[0x60 * a + 0x8A9 + 3 * t];
                    d[0x60 * a + 0x8FE + 3 * t] = s[0x60 * a + 0x8AA + 3 * t];
                    d[0x60 * a + 0x8FF + 3 * t] = s[0x60 * a + 0x8AB + 3 * t];
                }
            }
            s += 0x14A5;
            d += 0x14A8;
        }
        pNet[0x27] = 3;
        return 1;
    }
    return 0;
}
