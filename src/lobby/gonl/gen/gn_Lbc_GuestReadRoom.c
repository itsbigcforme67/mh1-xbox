/* G online (PC): Lbc_GuestReadRoom, written from Monster Hunter G's code at lobby 0x582410 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). HAND-FIXED: m2c took G's rule-table stack buffer (sp+0x80) for a stack argument
 * (the copy overran the frame): a static buffer, as Lbc_GetRoomRule. */
#include "../gonl.h"

s32 Check_CallBackWait();
void * memset();
void * strcpy();
extern u8 RoomInfo;
extern u8 cw;
extern u8 pNet;
s32 cnLBS_Get_RoomMemberList(); /* extern */
s32 cnLBS_Get_RoomRuleAllocation();    /* extern */
s32 cnLBS_Read_RoomMemberList();  /* extern */
s32 lobby_func_005BCEE0(); /* extern */
extern u8 CallBack_Result_Lobby_GuestRoomMember;
extern u8 CallBack_Result_Lobby_GuestRuleAllocation;
extern s32 g_lb_6DE120;
extern u8 RoomRule;
extern s32 join_member;

s32 Lbc_GuestReadRoom(s32 arg0, s32 unused_a1, s32 arg2, s32 arg3) {
    static u8 rbuf[0x2A000];          /* G: 0x294A4 bytes at sp+0x80 */
    u8 *var_s1_2;   /* (s32 *) */
    s32 temp_s0;
    s32 temp_s6;
    s32 var_s0;
    s32 var_s0_2;
    s32 var_s5;
    u8 *var_s1;
    u8 *var_s2;
    u8 *var_s3;
    u8 *var_s4;
    u8 temp_a0;
    u8 temp_v0_2;
    u8 *temp_a1;
    u8 *temp_v0;

    temp_v0 = (*(void **)&cw);
    temp_a0 = (*(u8 *)((u8 *)(temp_v0) + 0xB9D));
    temp_a1 = temp_v0 + 0xB9D;
    switch (temp_a0) {
    case 0:
        (*(u8 *)((u8 *)(temp_v0) + 0xB9D)) = (u8) (temp_a0 + 1);
        memset(&RoomRule, 0, 0x29555);
        temp_s0 = (arg0 & 0xFFFF) * 0x15C;
        strcpy(&(*(s32 *)((u8 *)&RoomRule + 0x12)), (u8 *)&g_lb_6DE120 + temp_s0 + 0x1A);
        (*(u8 *)((u8 *)&RoomRule + 0x52)) = *(&(*(u8 *)((u8 *)&RoomInfo + 0x13)) + temp_s0);
    default:
block_21:
        return 2;
    case 1:
        (*(u8 *)((u8 *)(temp_v0) + 0xB9D)) = (u8) (temp_a0 + 1);
        (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA68)) = 0xE10;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA65)) = 0;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0x23;
        lobby_func_005BCEE0(((arg0 & 0xFFFF) + 1) & 0xFFFF, &CallBack_Result_Lobby_GuestRuleAllocation, 0x23);
        goto block_21;
    case 2:
        Check_CallBackWait((s32) temp_a0, (s32) temp_a1, arg2, arg3);
        goto block_21;
    case 3:
        (*(u8 *)((u8 *)(temp_v0) + 0xB9D)) = (u8) (temp_a0 + 1);
        temp_s6 = (arg0 & 0xFFFF) + 1;
        cnLBS_Get_RoomRuleAllocation(temp_s6 & 0xFFFF, rbuf);
        RoomRule = rbuf[0];
        (*(u8 *)((u8 *)&RoomRule + 0x1)) = rbuf[1];
        (*(u8 *)((u8 *)&RoomRule + 0x54)) = rbuf[3];
        var_s0 = 0;
        if ((s32) (*(u8 *)((u8 *)&RoomRule + 0x54)) > 0) {
            var_s4 = rbuf;
            var_s3 = &RoomRule;
            do {
                strcpy(var_s3 + 0x56, var_s4 + 5);
                (*(u8 *)((u8 *)(var_s3) + 0x98)) = (u8) (*(u8 *)((u8 *)(var_s4) + 0x46));
                (*(u8 *)((u8 *)(var_s3) + 0x97)) = (u8) (*(u8 *)((u8 *)(var_s4) + 0x47));
                temp_v0_2 = (*(u8 *)((u8 *)(var_s4) + 0x48));
                (*(u8 *)((u8 *)(var_s3) + 0x9A)) = temp_v0_2;
                (*(u8 *)((u8 *)(var_s3) + 0x99)) = temp_v0_2;
                var_s5 = 0;
                if ((s32) (*(u8 *)((u8 *)(var_s3) + 0x97)) > 0) {
                    var_s2 = var_s4;
                    var_s1 = var_s3;
                    do {
                        strcpy(var_s1 + 0xBD, var_s2 + 0x69);
                        var_s5 += 1;
                        var_s2 += 0x41;
                        var_s1 += 0x41;
                    } while (var_s5 < (s32) (*(u8 *)((u8 *)(var_s3) + 0x97)));
                }
                var_s0 += 1;
                var_s4 += 0x14A5;
                var_s3 += 0x14A8;
            } while (var_s0 < (s32) (*(u8 *)((u8 *)&RoomRule + 0x54)));
        }
        (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA68)) = 0xE10;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA65)) = 0;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0x24;
        cnLBS_Read_RoomMemberList(temp_s6 & 0xFFFF, &CallBack_Result_Lobby_GuestRoomMember);
        goto block_21;
    case 4:
        Check_CallBackWait((s32) temp_a0, (s32) temp_a1, arg2, arg3);
        goto block_21;
    case 5:
        (*(u8 *)((u8 *)(temp_v0) + 0xB9D)) = 0U;
        memset(&join_member, 0, 0x170);
        var_s0_2 = 0;
        var_s1_2 = &join_member;
        do {
            cnLBS_Get_RoomMemberList(var_s0_2 & 0xFFFF, var_s1_2, var_s1_2 + 8, var_s1_2 + 0x1C);
            var_s0_2 += 1;
            var_s1_2 += 0x5C;
        } while (var_s0_2 < 4);
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA2)) = 1;
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x27)) = 2;
        return 0;
    }
}

