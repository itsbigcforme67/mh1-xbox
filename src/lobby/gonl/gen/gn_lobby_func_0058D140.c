/* G online (PC): lobby_func_0058D140, written from Monster Hunter G's code at lobby 0x58D140 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"
extern u8 g_main_2EF06C[];      /* HAND-FIXED */

s32 DispFrameList();
s32 Lb_put_status();
void * memcpy();
extern u8 pNet;
extern u8 statData;
extern void *g_lb_636A00;
extern s32 g_lb_63E8C0;
extern s32 g_lb_63E8C8;
extern s32 g_lb_63E8D0;
extern u8 my_user_mini_data;

void lobby_func_0058D140(void) {
    s32 var_a3;
    s32 var_s2_2;
    s32 temp_v1;
    s32 var_s0_2;
    s32 var_s2;
    void **var_s0;

    var_s2 = 0;
    var_s0 = &g_lb_636A00;
    do {
        if ((*(u8 *)((u8 *)(&my_user_mini_data) + 2)) == 0) {
            memcpy(*var_s0, &g_lb_63E8C0, 4);
        } else if ((*(u8 *)((u8 *)(&my_user_mini_data) + 2)) == (((s16) (var_s2)))) {
            memcpy(*var_s0, &g_lb_63E8C8, 4);
        } else {
            memcpy(*var_s0, &g_lb_63E8D0, 4);
        }
        var_s2 = ((s16) ((var_s2 + 1)));
        var_s0 += 4;
    } while (var_s2 < 7);
    DispFrameList(&(*(s32 *)((u8 *)&statData + 0x20)), *(void **)g_main_2EF06C, (s32) (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x29)));
    var_s2_2 = 0x68;
    var_s0_2 = 0;
    do {
        var_a3 = -1;
        if ((*(u8 *)((u8 *)(&my_user_mini_data) + 2)) == 0) {

        } else {
            var_a3 = -1;
            if ((*(u8 *)((u8 *)(&my_user_mini_data) + 2)) == (((s16) (var_s0_2)))) {

            } else {
                var_a3 = -0x8F8F90;
            }
        }
        temp_v1 = ((s16) (var_s0_2));
        if (temp_v1 != 0) {
            Lb_put_status(0x194, var_s2_2, 0x14, var_a3, (s32) (((s16) (temp_v1))));
            var_s2_2 = (s32) (((s16) ((var_s2_2 + 0x16))));
        }
        var_s0_2 = ((s16) ((var_s0_2 + 1)));
    } while (var_s0_2 < 7);
}

