/* G online (PC): Lbc_GetRoomRule, written from Monster Hunter G's code at lobby 0x581E70 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Check_CallBackWait();
s32 cnLbc_CheckInFloorOrder();
void * memset();
void * strcpy();
extern u8 arg_sp70;
extern u8 cw;
extern u8 pNet;
s32 cnLBS_Get_RoomRuleAllocation();    /* extern */
s32 lobby_func_005BCEE0();        /* extern */
extern u8 CallBack_Result_RuleAllocation;
extern u8 RoomRule;

s32 Lbc_GetRoomRule(s32 unused_a0, s32 arg1, s32 arg2, s32 arg3, u8 arg_sp70) {
    s32 temp_v0;
    s32 var_a0;
    s32 var_a5;
    s32 var_s0;
    s32 var_s5;
    u8 *var_a1;
    u8 *var_a2;
    u8 *var_a3;
    u8 *var_a4;
    u8 *var_s1;
    u8 *var_s2;
    u8 *var_s3;
    u8 *var_s4;
    u8 temp_a0_2;
    u8 temp_v0_2;
    u8 *temp_a0;
    u8 *temp_a0_3;

    temp_v0 = cnLbc_CheckInFloorOrder(2, arg1, arg2, arg3);
    temp_a0 = (*(void **)&cw);
    temp_a0_2 = (*(u8 *)((u8 *)(temp_a0) + 0xB9D));
    switch (temp_a0_2) {                            /* irregular */
    case 0:
        (*(u8 *)((u8 *)(temp_a0) + 0xB9D)) = (u8) (temp_a0_2 + 1);
        /* fallthrough */
    case 1:
        temp_a0_3 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_a0_3) + 0xB9D)) = (u8) ((*(u8 *)((u8 *)(temp_a0_3) + 0xB9D)) + 1);
        (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA68)) = 0xE10;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA65)) = 0;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0x20;
        lobby_func_005BCEE0(temp_v0 & 0xFFFF, &CallBack_Result_RuleAllocation);
block_23:
    default:
        return 0;
    case 2:
        Check_CallBackWait((s32) temp_a0_2, temp_a0 + 0xB9D, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        goto block_23;
    case 3:
        (*(u8 *)((u8 *)(temp_a0) + 0xB9D)) = 0U;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA2)) = 0;
        cnLBS_Get_RoomRuleAllocation(temp_v0 & 0xFFFF, &arg_sp70);
        memset(&RoomRule, 0, 0x29555);
        RoomRule = arg_sp70;
        (*(u8 *)((u8 *)&RoomRule + 0x1)) = arg_sp70;
        (*(u8 *)((u8 *)&RoomRule + 0x54)) = arg_sp70;
        var_s0 = 0;
        if ((s32) (*(u8 *)((u8 *)&RoomRule + 0x54)) > 0) {
            var_s4 = &arg_sp70;
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
                var_a0 = 0;
                var_a4 = var_s4;
                var_a3 = var_s3;
loop_16:
                var_a5 = 0;
                var_a2 = var_a4;
                var_a1 = var_a3;
                (*(u8 *)((u8 *)(&var_s3[var_a0]) + 0x8DD)) = (u8) (*(u8 *)((u8 *)(&var_s4[var_a0]) + 0x889));
loop_17:
                var_a5 += 1;
                (*(u8 *)((u8 *)(var_a1) + 0x8FD)) = (u8) (*(u8 *)((u8 *)(var_a2) + 0x8A9));
                (*(u8 *)((u8 *)(var_a1) + 0x8FE)) = (u8) (*(u8 *)((u8 *)(var_a2) + 0x8AA));
                (*(u8 *)((u8 *)(var_a1) + 0x8FF)) = (u8) (*(u8 *)((u8 *)(var_a2) + 0x8AB));
                var_a2 += 3;
                var_a1 += 3;
                if (var_a5 < 0x20) {
                    goto loop_17;
                }
                var_a0 += 1;
                var_a4 += 0x60;
                var_a3 += 0x60;
                if (var_a0 < 0x20) {
                    goto loop_16;
                }
                var_s0 += 1;
                var_s4 += 0x14A5;
                var_s3 += 0x14A8;
            } while (var_s0 < (s32) (*(u8 *)((u8 *)&RoomRule + 0x54)));
        }
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x27)) = 3;
        return 1;
    }
}

