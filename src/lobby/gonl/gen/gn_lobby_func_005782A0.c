/* G online (PC): lobby_func_005782A0, written from Monster Hunter G's code at lobby 0x5782A0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 pNet;
s32 Lb_put_status(); /* extern */
s32 flfntLocate();                  /* extern */
s32 flfntSetSize();             /* extern */
s32 font_print_double(); /* extern */
s32 func_0012BDD0();                         /* extern */
s32 memcpy();            /* extern */
s32 put_main_cursor();                        /* extern */
extern u8 g_lb_636A00;
extern u8 g_lb_63E128;
extern u8 g_lb_63E2E0;
extern u8 g_lb_63E2E8;

void lobby_func_005782A0(void) {
    s32 var_a3;
    s32 var_s1;
    u8 *var_s0;   /* (s32 *) */
    s64 var_s2;
    s64 var_s3;
    u8 temp_v1;
    u8 *temp_a0;

    temp_a0 = (*(void **)&pNet);
    if ((*(u8 *)((u8 *)(temp_a0) + 0x23)) != 0) {
        put_main_cursor((*(u8 *)((u8 *)(temp_a0) + 0x2B)));
        flfntSetSize(0x12, 0x12);
        var_s3 = 0x8E;
        var_s2 = 0;
        var_s0 = &g_lb_636A00;
        do {
            temp_v1 = (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x32));
            if (temp_v1 == 0) {
                memcpy((*(s32 *)var_s0), &g_lb_63E128, 4);
                var_a3 = -1;
                var_s1 = 0;
            } else if (((s64) (var_s2 << 0x30) >> 0x30) == temp_v1) {
                memcpy((*(s32 *)var_s0), &g_lb_63E2E0, 4);
                var_a3 = -1;
                var_s1 = 4;
            } else {
                memcpy((*(s32 *)var_s0), &g_lb_63E2E8, 4);
                var_s1 = 0xA;
                var_a3 = 0xFF707070;
            }
            Lb_put_status(0xE2, (s64) ((((s64) (var_s3 << 0x30) >> 0x30) - 2) << 0x30) >> 0x30, 0x14, var_a3);
            flfntLocate(0xDE, var_s3);
            if (((s64) (var_s2 << 0x30) >> 0x30) == (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2B))) {
                font_print_double(0xDE, var_s3, 1, var_s1);
            } else {
                func_0012BDD0((*(s32 *)var_s0));
            }
            var_s0 += 4;
            var_s2 = (s64) ((var_s2 + 1) << 0x30) >> 0x30;
            var_s3 = (s64) ((var_s3 + 0x16) << 0x30) >> 0x30;
        } while (var_s2 < 7);
    }
}

