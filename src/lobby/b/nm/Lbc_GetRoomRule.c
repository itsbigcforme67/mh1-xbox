#include "lobby_a.h"
extern char CallBack_Result_RuleAllocation[];
extern char RoomRule[];
extern char RoomRule[];
extern char RoomRule[];
extern char RoomRule[];
extern char RoomRule[];
extern char RoomRule[];
extern char RoomRule[];
s32 Lbc_GetRoomRule(u8 arg28) {
    int var_a1;
    int var_a3;
    int var_s1;
    int var_s3;
    s32 temp_a0_3;
    s32 temp_a1;
    s32 temp_v0;
    s32 var_a0;
    s32 var_s0;
    s32 var_s5;
    s32 var_t1;
    int var_a2;
    int var_s2;
    int var_s4;
    int var_t0;
    u8 temp_a0_2;
    u8 temp_v0_2;
    int temp_a0;

    temp_v0 = cnLbc_CheckInFloorOrder(2);
    temp_a0 = (int)cw;
    temp_a1 = temp_a0 + 0x2C35;
    temp_a0_2 = F(u8, temp_a0, 0x2C35);
    switch (temp_a0_2) {                            /* irregular */
    case 0:
        F(u8, temp_a0, 0x2C35) = (u8) (temp_a0_2 + 1);
block_24:
    default:
        return 0;
    case 1:
        F(u8, temp_a0, 0x2C35) = (u8) (temp_a0_2 + 1);
        temp_a0_3 = CallBackWaitInit(temp_a0_2, temp_a1) & 0xFFFF;
        F(s8, (u8 *)cw, 0x2C45) = 0x10;
        cnLBS_Read_RoomRuleAllocation(temp_a0_3, &CallBack_Result_RuleAllocation);
        goto block_24;
    case 2:
        Check_CallBackWait(temp_a0_2, temp_a1);
        goto block_24;
    case 3:
        F(u8, temp_a0, 0x2C35) = 0U;
        F(s8, (u8 *)cw, 0x2C3A) = 0;
        F(s8, (u8 *)cw, 0x32BF) = 1;
        cnLBS_Get_RoomRuleAllocation(temp_v0 & 0xFFFF, &arg28);
        memset(&RoomRule, 0, 0x29555);
        F(u8, &RoomRule, 0) = arg28;
        F(u8, &RoomRule, 1) = arg28;
        F(u8, &RoomRule, 0x54) = arg28;
        var_s0 = 0;
        if (F(u8, &RoomRule, 0x54) > 0) {
            var_s4 = (int)&arg28;
            var_s3 = (int)&RoomRule;
            do {
                strcpy(var_s3 + 0x56, var_s4 + 5);
                F(u8, var_s3, 0x98) = (u8) F(u8, var_s4, 0x46);
                F(u8, var_s3, 0x97) = (u8) F(u8, var_s4, 0x47);
                temp_v0_2 = F(u8, var_s4, 0x48);
                F(u8, var_s3, 0x9A) = temp_v0_2;
                F(u8, var_s3, 0x99) = temp_v0_2;
                var_s5 = 0;
                if (F(u8, var_s3, 0x97) > 0) {
                    var_s2 = var_s4;
                    var_s1 = var_s3;
                    do {
                        strcpy(var_s1 + 0xBD, var_s2 + 0x69);
                        var_s5 += 1;
                        var_s2 += 0x41;
                        var_s1 += 0x41;
                    } while (var_s5 < F(u8, var_s3, 0x97));
                }
                var_a0 = 0;
                var_t0 = var_s4;
                var_a3 = var_s3;
loop_17:
                var_t1 = 0;
                var_a2 = var_t0;
                var_a1 = var_a3;
                F(u8, (var_s3 + var_a0), 0x8DD) = (u8) F(u8, &((u8 *)var_s4)[var_a0], 0x889);
loop_18:
                var_t1 += 1;
                F(u8, var_a1, 0x8FD) = (u8) F(u8, var_a2, 0x8A9);
                F(u8, var_a1, 0x8FE) = (u8) F(u8, var_a2, 0x8AA);
                F(u8, var_a1, 0x8FF) = (u8) F(u8, var_a2, 0x8AB);
                var_a2 += 3;
                var_a1 += 3;
                if (var_t1 < 0x20) {
                    goto loop_18;
                }
                var_a0 += 1;
                var_t0 += 0x60;
                var_a3 += 0x60;
                if (var_a0 < 0x20) {
                    goto loop_17;
                }
                var_s0 += 1;
                var_s4 += 0x14A5;
                var_s3 += 0x14A8;
            } while (var_s0 < F(u8, &RoomRule, 0x54));
        }
        F(s8, pNet, 6) = 3;
        return 1;
    }
}
