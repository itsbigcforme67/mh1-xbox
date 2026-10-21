/* G online (PC): put_mail_input_square, written from Monster Hunter G's code at lobby 0x578530 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 put_main_cursor();
s32 put_main_cursor2();
extern u8 g_main_2EFBAC;
extern u8 cw;
extern u8 g_main_2EFBAC;
extern u8 pNet;
s32 Put_F();                               /* extern */

void put_mail_input_square(s64 arg0, s64 arg1) {
    u8 g_frame[0xC0] __attribute__((aligned(16)));   /* G's stack frame */
#define spA0 (*(s32 *)(g_frame + 0xA0))
#define spB0 (*(s16 *)(g_frame + 0xB0))
#define spB2 (*(s16 *)(g_frame + 0xB2))
#define spB4 (*(s16 *)(g_frame + 0xB4))
#define spB6 (*(s16 *)(g_frame + 0xB6))
#define spB8 (*(s32 *)(g_frame + 0xB8))
    s16 var_s2;
    s32 var_s4;
    s32 var_v0;
    s8 temp_v1;
    u32 var_s3;
    u8 temp_a1;
    u8 *temp_a1_2;
    u8 *temp_a1_3;
    u8 *temp_v0;

    var_s4 = 0;
    if ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) == 0) {
        temp_v0 = (*(void **)&g_main_2EFBAC);
        temp_a1 = (*(u8 *)((u8 *)(temp_v0) + 0x230));
        if (temp_a1 != 2) {
            if ((*(s8 *)((u8 *)(temp_v0) + 0x236)) == 0) {
                var_s4 = 1;
            }
        } else {
            temp_v1 = (*(s8 *)((u8 *)(temp_v0) + 0x237));
            switch (temp_v1) {                      /* irregular */
            case 2:
            case 1:
            case 0:
                var_s4 = 2;
                break;
            case 3:
                var_s4 = 1;
                break;
            }
        }
        if (temp_a1 == 2) {
            var_s3 = 0;
            var_s2 = (((s16) (arg1))) + 0x2A;
            spA0 = (((s16) (arg0))) + 0x33;
            do {
                spB0 = (s16) spA0;
                if ((s32) (*(u8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x233)) >= 0) {
                    spB4 = spB0 + 0x9D;
                } else {
                    spB4 = spB0 + 0x151;
                }
                spB2 = var_s2;
                spB6 = spB2 + 0x16;
                if ((var_s4 == 2) && (var_s3 == (*(s8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x237)))) {
                    spB8 = 0xFF98DB98;
                } else {
                    spB8 = 0xFF32563C;
                }
                Put_F(&spB0);
                spB0 += 3;
                spB4 -= 3;
                spB2 += 2;
                spB6 -= 2;
                if ((var_s4 == 2) && (var_s3 == (*(s8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x237)))) {
                    spB8 = 0xFF26371C;
                } else {
                    spB8 = 0xFF051F05;
                }
                Put_F(&spB0);
                var_s3 += 1;
                var_s2 += 0x16;
            } while (var_s3 < 3U);
        }
        spB0 = (((s16) (arg0))) + 0x33;
        spB4 = spB0 + 0x151;
        if ((*(u8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x230)) == 2) {
            spB2 = (((s16) (arg1))) + 0x6C;
        } else {
            spB2 = (((s16) (arg1))) + 0x40;
        }
        spB6 = spB2 + 0x58;
        if (var_s4 == 1) {
            var_v0 = 0xFF98DB98;
        } else {
            var_v0 = 0xFF32563C;
        }
        spB8 = var_v0;
        Put_F(&spB0);
        spB0 += 3;
        spB4 -= 3;
        spB2 += 2;
        spB6 -= 2;
        if (var_s4 == 1) {
            spB8 = 0xFF26371C;
        } else {
            spB8 = 0xFF051F05;
        }
        Put_F(&spB0);
        if ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xFA61)) != 0) {
            temp_a1_2 = (*(void **)&g_main_2EFBAC);
            if ((*(u8 *)((u8 *)(temp_a1_2) + 0x230)) != 2) {
                if ((*(s8 *)((u8 *)(temp_a1_2) + 0x236)) == 1) {
                    put_main_cursor2(0xD8, 0x38, 7);
                }
            } else if ((*(s8 *)((u8 *)(temp_a1_2) + 0x237)) == 4) {
                put_main_cursor2(0xD8, 0x38, 7);
            }
        } else {
            temp_a1_3 = (*(void **)&g_main_2EFBAC);
            if ((*(u8 *)((u8 *)(temp_a1_3) + 0x230)) != 2) {
                if ((*(s8 *)((u8 *)(temp_a1_3) + 0x236)) == 1) {
                    put_main_cursor(7);
                }
            } else if ((*(s8 *)((u8 *)(temp_a1_3) + 0x237)) == 4) {
                put_main_cursor(7);
            }
        }
    }
}

