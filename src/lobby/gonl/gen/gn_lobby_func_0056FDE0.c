/* G online (PC): lobby_func_0056FDE0, written from Monster Hunter G's code at lobby 0x56FDE0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_00559FF0();
extern u8 g_main_2EFBC0;
extern u8 g_main_2EFBC0;
s8 Lb_back_money();                              /* extern */

void lobby_func_0056FDE0(void) {
    u8 g_frame[0xD0] __attribute__((aligned(16)));   /* G's stack frame */
#define sp50 (*(s8 *)(g_frame + 0x50))
#define spC0 (*(s32 *)(g_frame + 0xC0))
    u8 *var_t2;   /* (s32 *) */
    u8 *var_t3_3;   /* (s32 *) */
    s16 temp_a7;
    s16 temp_a7_2;
    s16 temp_a7_3;
    s32 var_s2;
    s32 var_s3;
    s32 var_t1;
    s32 var_t1_2;
    s32 var_t1_3;
    s32 var_v1;
    s8 *var_a2;
    s8 *var_a4;
    s8 *var_s1;
    s8 var_a6;
    u8 *temp_a3;
    u8 *var_a1;
    u8 *var_a5;
    u8 temp_s0;
    u8 *var_t2_2;
    u8 *var_t2_3;
    u8 *var_t3;
    u8 *var_t3_2;

    temp_s0 = *(*(u8 **)&g_main_2EFBC0);
    if (temp_s0 != 0) {
        var_s3 = 0;
        if ((s32) temp_s0 > 0) {
            var_s2 = 0;
            var_s1 = &sp50;
            do {
                var_s3 += 1;
                *var_s1 = Lb_back_money((((s16) (lobby_func_00559FF0((*(u8 **)&g_main_2EFBC0) + var_s2 + 4)))) & 0xFFFF);
                var_s2 += 0x10;
                var_s1 += 1;
            } while (var_s3 < (s32) temp_s0);
        }
        var_a5 = (*(u8 **)&g_main_2EFBC0);
        var_a6 = 1;
        var_a4 = &sp50;
        temp_a3 = var_a5;
        do {
            var_v1 = 0;
            if ((s32) temp_s0 > 0) {
                var_a2 = &sp50;
                var_a1 = temp_a3;
                do {
                    var_t3 = var_a5 + 4;
                    if (*var_a2 == var_a6) {
                        var_t2 = &spC0;
                        var_t1 = 4;
                        do {
                            var_t1 -= 1;
                            temp_a7 = (*(s16 *)((u8 *)(var_t3) + 2));
                            (*(s16 *)((u8 *)(var_t2) + 0)) = (s16) (*(s16 *)((u8 *)(var_t3) + 0));
                            var_t3 += 4;
                            (*(s16 *)((u8 *)(var_t2) + 2)) = temp_a7;
                            var_t2 += 4;
                        } while (var_t1 > 0);
                        var_t3_2 = var_a1 + 4;
                        var_t2_2 = var_a5 + 4;
                        var_t1_2 = 4;
                        do {
                            var_t1_2 -= 1;
                            temp_a7_2 = (*(s16 *)((u8 *)(var_t3_2) + 2));
                            (*(s16 *)((u8 *)(var_t2_2) + 0)) = (s16) (*(s16 *)((u8 *)(var_t3_2) + 0));
                            var_t3_2 += 4;
                            (*(s16 *)((u8 *)(var_t2_2) + 2)) = temp_a7_2;
                            var_t2_2 += 4;
                        } while (var_t1_2 > 0);
                        var_t3_3 = &spC0;
                        var_t2_3 = var_a1 + 4;
                        var_t1_3 = 4;
                        do {
                            var_t1_3 -= 1;
                            temp_a7_3 = (*(s16 *)((u8 *)(var_t3_3) + 2));
                            (*(s16 *)((u8 *)(var_t2_3) + 0)) = (s16) (*(s16 *)((u8 *)(var_t3_3) + 0));
                            var_t3_3 += 4;
                            (*(s16 *)((u8 *)(var_t2_3) + 2)) = temp_a7_3;
                            var_t2_3 += 4;
                        } while (var_t1_3 > 0);
                        var_a5 += 0x10;
                        *var_a2 = *var_a4;
                        *var_a4 = var_a6;
                        var_a4 += 1;
                    }
                    var_v1 += 1;
                    var_a2 += 1;
                    var_a1 += 0x10;
                } while (var_v1 < (s32) temp_s0);
            }
            var_a6 += 1;
        } while (var_a6 < 6);
    }
}

