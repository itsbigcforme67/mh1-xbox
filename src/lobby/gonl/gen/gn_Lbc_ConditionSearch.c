/* G online (PC): Lbc_ConditionSearch, written from Monster Hunter G's code at lobby 0x57FF60 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
s32 CallBackWaitInit();     /* extern */
s32 Check_CallBackWait();                    /* extern */
s32 cnLBS_ConditionSearchUser(); /* extern */
extern u8 CallBack_Result_ConditionSearchUser;

s32 Lbc_ConditionSearch(void *arg0, s64 arg1, s32 arg3) {
    s8 sp11;
    union { s8 v; u8 b[0x230]; } u_sp10;
#define sp10 (u_sp10.v)
    s32 var_a2;
    s32 var_a3;
    s32 var_t1;
    s32 var_t1_2;
    s32 var_t1_3;
    s32 var_t1_4;
    s32 var_t1_5;
    s32 var_t1_6;
    s32 var_t1_7;
    s32 var_t1_8;
    s64 temp_v1_3;
    s64 var_a1;
    s8 *var_t2;
    s8 *var_t2_2;
    s8 *var_t2_3;
    s8 *var_t2_4;
    s8 *var_t2_5;
    s8 *var_t2_6;
    s8 *var_t2_7;
    s8 *var_t2_8;
    s8 *var_v0;
    s8 temp_a3;
    s8 temp_a3_2;
    s8 temp_a3_3;
    s8 temp_a3_4;
    s8 temp_a3_5;
    s8 temp_a3_6;
    s8 temp_a3_7;
    s8 temp_a3_8;
    s8 temp_v0;
    u8 temp_v1_2;
    u8 *temp_a0;
    u8 *temp_a0_2;
    u8 *temp_a0_3;
    u8 *temp_v1;
    u8 *var_a0;
    u8 *var_t0;
    u8 *var_t1_9;
    u8 *var_t2_9;
    u8 *var_t3;
    u8 *var_t3_2;
    u8 *var_t3_3;
    u8 *var_t3_4;
    u8 *var_t3_5;
    u8 *var_t3_6;
    u8 *var_t3_7;
    u8 *var_t3_8;

    var_a0 = arg0;
    var_a1 = arg1;
    var_a3 = arg3;
    temp_v1 = (*(void **)&cw);
    temp_v1_2 = (*(u8 *)((u8 *)(temp_v1) + 0xB9D));
    switch (temp_v1_2) {                            /* irregular */
    case 0:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = (u8) (temp_v1_2 + 1);
        temp_v1_3 = ((s8) (var_a1));
        sp10 = 0x64;
        var_a2 = 0;
        sp11 = (s8) var_a1;
        if (temp_v1_3 > 0) {
            var_a1 = temp_v1_3 - 8;
            if (temp_v1_3 >= 9) {
                var_v0 = &sp10;
                do {
                    var_t2 = var_v0 + 4;
                    var_t1 = 0x22;
                    var_t3 = var_a0;
loop_10:
                    var_t1 -= 1;
                    temp_a3 = (*(s8 *)((u8 *)(var_t3) + 1));
                    (*(s8 *)((u8 *)(var_t2) + 0)) = (*(s8 *)((u8 *)(var_t3) + 0));
                    var_t3 += 2;
                    (*(s8 *)((u8 *)(var_t2) + 1)) = temp_a3;
                    var_t2 += 2;
                    if (var_t1 > 0) {
                        goto loop_10;
                    }
                    var_t3_2 = var_a0 + 0x44;
                    var_t2_2 = var_v0 + 0x48;
                    var_t1_2 = 0x22;
loop_12:
                    var_t1_2 -= 1;
                    temp_a3_2 = (*(s8 *)((u8 *)(var_t3_2) + 1));
                    (*(s8 *)((u8 *)(var_t2_2) + 0)) = (*(s8 *)((u8 *)(var_t3_2) + 0));
                    var_t3_2 += 2;
                    (*(s8 *)((u8 *)(var_t2_2) + 1)) = temp_a3_2;
                    var_t2_2 += 2;
                    if (var_t1_2 > 0) {
                        goto loop_12;
                    }
                    temp_a0 = var_a0 + 0x88;
                    var_t2_3 = var_v0 + 0x8C;
                    var_t1_3 = 0x22;
                    var_t3_3 = temp_a0;
loop_14:
                    var_t1_3 -= 1;
                    temp_a3_3 = (*(s8 *)((u8 *)(var_t3_3) + 1));
                    (*(s8 *)((u8 *)(var_t2_3) + 0)) = (*(s8 *)((u8 *)(var_t3_3) + 0));
                    var_t3_3 += 2;
                    (*(s8 *)((u8 *)(var_t2_3) + 1)) = temp_a3_3;
                    var_t2_3 += 2;
                    if (var_t1_3 > 0) {
                        goto loop_14;
                    }
                    var_t3_4 = temp_a0 + 0x44;
                    var_t2_4 = var_v0 + 0xD0;
                    var_t1_4 = 0x22;
loop_16:
                    var_t1_4 -= 1;
                    temp_a3_4 = (*(s8 *)((u8 *)(var_t3_4) + 1));
                    (*(s8 *)((u8 *)(var_t2_4) + 0)) = (*(s8 *)((u8 *)(var_t3_4) + 0));
                    var_t3_4 += 2;
                    (*(s8 *)((u8 *)(var_t2_4) + 1)) = temp_a3_4;
                    var_t2_4 += 2;
                    if (var_t1_4 > 0) {
                        goto loop_16;
                    }
                    temp_a0_2 = temp_a0 + 0x88;
                    var_t2_5 = var_v0 + 0x114;
                    var_t1_5 = 0x22;
                    var_t3_5 = temp_a0_2;
loop_18:
                    var_t1_5 -= 1;
                    temp_a3_5 = (*(s8 *)((u8 *)(var_t3_5) + 1));
                    (*(s8 *)((u8 *)(var_t2_5) + 0)) = (*(s8 *)((u8 *)(var_t3_5) + 0));
                    var_t3_5 += 2;
                    (*(s8 *)((u8 *)(var_t2_5) + 1)) = temp_a3_5;
                    var_t2_5 += 2;
                    if (var_t1_5 > 0) {
                        goto loop_18;
                    }
                    var_t3_6 = temp_a0_2 + 0x44;
                    var_t2_6 = var_v0 + 0x158;
                    var_t1_6 = 0x22;
loop_20:
                    var_t1_6 -= 1;
                    temp_a3_6 = (*(s8 *)((u8 *)(var_t3_6) + 1));
                    (*(s8 *)((u8 *)(var_t2_6) + 0)) = (*(s8 *)((u8 *)(var_t3_6) + 0));
                    var_t3_6 += 2;
                    (*(s8 *)((u8 *)(var_t2_6) + 1)) = temp_a3_6;
                    var_t2_6 += 2;
                    if (var_t1_6 > 0) {
                        goto loop_20;
                    }
                    temp_a0_3 = temp_a0_2 + 0x88;
                    var_t2_7 = var_v0 + 0x19C;
                    var_t1_7 = 0x22;
                    var_t3_7 = temp_a0_3;
loop_22:
                    var_t1_7 -= 1;
                    temp_a3_7 = (*(s8 *)((u8 *)(var_t3_7) + 1));
                    (*(s8 *)((u8 *)(var_t2_7) + 0)) = (*(s8 *)((u8 *)(var_t3_7) + 0));
                    var_t3_7 += 2;
                    (*(s8 *)((u8 *)(var_t2_7) + 1)) = temp_a3_7;
                    var_t2_7 += 2;
                    if (var_t1_7 > 0) {
                        goto loop_22;
                    }
                    var_t3_8 = temp_a0_3 + 0x44;
                    var_t2_8 = var_v0 + 0x1E0;
                    var_t1_8 = 0x22;
loop_24:
                    var_t1_8 -= 1;
                    temp_a3_8 = (*(s8 *)((u8 *)(var_t3_8) + 1));
                    (*(s8 *)((u8 *)(var_t2_8) + 0)) = (*(s8 *)((u8 *)(var_t3_8) + 0));
                    var_t3_8 += 2;
                    (*(s8 *)((u8 *)(var_t2_8) + 1)) = temp_a3_8;
                    var_t2_8 += 2;
                    if (var_t1_8 > 0) {
                        goto loop_24;
                    }
                    var_a2 += 8;
                    var_a0 = temp_a0_3 + 0x88;
                    var_a3 = var_a2 < var_a1;
                    var_v0 += 0x220;
                } while (var_a3 != 0);
            }
            if (var_a2 < temp_v1_3) {
                var_t2_9 = (var_a2 * 0x44) + (s32)&sp10;   /* HAND-FIXED: sp + 0x10 */
                do {
                    var_t0 = var_t2_9 + 4;
                    var_a3 = 0x22;
                    var_t1_9 = var_a0;
loop_30:
                    var_a1 = (s64) (*(s8 *)((u8 *)(var_t1_9) + 0));
                    var_a3 -= 1;
                    temp_v0 = (*(s8 *)((u8 *)(var_t1_9) + 1));
                    (*(s8 *)((u8 *)(var_t0) + 0)) = (s8) var_a1;
                    var_t1_9 += 2;
                    (*(s8 *)((u8 *)(var_t0) + 1)) = temp_v0;
                    var_t0 += 2;
                    if (var_a3 > 0) {
                        goto loop_30;
                    }
                    var_a2 += 1;
                    var_a0 += 0x44;
                    var_t2_9 += 0x44;
                } while (var_a2 < temp_v1_3);
            }
        }
        CallBackWaitInit(var_a0, (s8) var_a1, var_a2, var_a3);
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0xE;
        cnLBS_ConditionSearchUser(&sp10, &CallBack_Result_ConditionSearchUser);
block_40:
    default:
        return 2;
    case 1:
        Check_CallBackWait(temp_v1 + 0xB9D);
        goto block_40;
    case 2:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = 0U;
        return 0;
    case 3:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9D)) = 0U;
        return 1;
    }
}

