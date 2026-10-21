/* G online (PC): lobby_func_005782A0, written from Monster Hunter G's code at lobby 0x5782A0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_put_status();
s32 flfntLocate();
s32 flfntSetSize();
s32 font_print_double();
void * memcpy();
s32 put_main_cursor();
extern u8 pNet;
s32 func_0012BDD0();                      /* extern */
extern void *g_lb_636A00;
extern s32 g_lb_63E128;
extern s32 g_lb_63E2E0;
extern s32 g_lb_63E2E8;

void lobby_func_005782A0(void) {
    s32 var_a3;
    s32 var_s1;
    s32 var_s2;
    s32 var_s3;
    u8 temp_v1;
    void **var_s0;
    u8 *temp_a0;

    temp_a0 = (*(void **)&pNet);
    if ((*(u8 *)((u8 *)(temp_a0) + 0x23)) != 0) {
        put_main_cursor((s32) (*(u8 *)((u8 *)(temp_a0) + 0x2B)));
        flfntSetSize(0x12, 0x12);
        var_s3 = 0x8E;
        var_s2 = 0;
        var_s0 = &g_lb_636A00;
        do {
            temp_v1 = (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x32));
            if (temp_v1 == 0) {
                memcpy(*var_s0, &g_lb_63E128, 4);
                var_a3 = -1;
                var_s1 = 0;
            } else if ((((s16) (var_s2))) == temp_v1) {
                memcpy(*var_s0, &g_lb_63E2E0, 4);
                var_a3 = -1;
                var_s1 = 4;
            } else {
                memcpy(*var_s0, &g_lb_63E2E8, 4);
                var_s1 = 0xA;
                var_a3 = -0x8F8F90;
            }
            Lb_put_status(0xE2, (s32) (((s16) (((((s16) (var_s3))) - 2)))), 0x14, var_a3, var_s2);
            flfntLocate(0xDE, (s32) var_s3);
            if ((((s16) (var_s2))) == (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2B))) {
                font_print_double(0xDE, (s32) var_s3, 1, var_s1, *var_s0 + 4);
            } else {
                func_0012BDD0(*var_s0);
            }
            var_s0 += 4;
            var_s2 = ((s16) ((var_s2 + 1)));
            var_s3 = ((s16) ((var_s3 + 0x16)));
        } while (var_s2 < 7);
    }
}

