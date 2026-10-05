#include "lobby_a.h"
typedef struct { u8 pad0000[0x3]; u8 x0003; u8 pad0004[0x3]; u8 x0007; u8 pad0008[0x2]; u8 x000A; u8 pad000B[0x1]; u8 x000C; } ARG_put_mail_input_square_arg0;

void put_mail_input_square(ARG_put_mail_input_square_arg0 *arg0, int arg1, int arg2) {
    s32 sp98;
    s16 sp96;
    s16 sp94;
    s16 sp92;
    s16 sp90;
    s16 temp_s6;
    s32 var_s3;
    s32 var_v0;
    s32 var_v0_2;
    s32 var_v0_3;
    int temp_s7;
    u8 temp_a0;
    u8 temp_v1;

    var_s3 = 0;
    if (arg0->x000C == 0) {
        temp_a0 = arg0->x0003;
        if (temp_a0 < 7) {
            if (arg0->x000A == 0) {
                var_s3 = 1;
            }
        } else {
            temp_v1 = arg0->x000A;
            switch (temp_v1) {                      /* irregular */
            case 0:
                var_s3 = 2;
                break;
            case 1:
                var_s3 = 1;
                break;
            }
        }
        if (temp_a0 != 5) {
            if (temp_a0 == 6) {
                goto block_13;
            }
        } else {
block_13:
            if (arg0->x0007 == 4) {
                var_s3 = 0;
            }
        }
        temp_s6 = ( (arg1 << 0x30) >> 0x30) + 0x33;
        temp_s7 =  (arg2 << 0x30) >> 0x30;
        sp90 = temp_s6;
        sp94 = sp90 + 0x151;
        sp92 = temp_s7 + 0x2A;
        sp96 = sp92 + 0x16;
        if (var_s3 == 2) {
            var_v0 = 0xFFAA8820;
        } else {
            var_v0 = 0xFF501515;
        }
        sp98 = var_v0;
        if (arg0->x0003 >= 8) {
            Put_F(&sp90);
        }
        sp90 += 3;
        sp94 -= 3;
        sp92 += 2;
        sp96 -= 2;
        if (var_s3 == 2) {
            var_v0_2 = 0xFF4A2020;
        } else {
            var_v0_2 = 0xFF2A0000;
        }
        sp98 = var_v0_2;
        if (arg0->x0003 >= 8) {
            Put_F(&sp90);
        }
        sp90 = temp_s6;
        sp94 = sp90 + 0x151;
        sp92 = temp_s7 + 0x40;
        sp96 = sp92 + 0x58;
        if (var_s3 == 1) {
            sp98 = 0xFFAA8820;
        } else {
            sp98 = 0xFF501515;
        }
        Put_F(&sp90);
        sp90 += 3;
        sp94 -= 3;
        sp92 += 2;
        sp96 -= 2;
        if (var_s3 == 1) {
            var_v0_3 = 0xFF4A2020;
        } else {
            var_v0_3 = 0xFF2A0000;
        }
        sp98 = var_v0_3;
        Put_F(&sp90);
        if (F(u8, (u8 *)cw, 0x35D5) != 0) {
            if (arg0->x0003 < 6) {
                if (arg0->x000A == 1) {
                    put_main_cursor2(0xD8, 0x38, 7);
                }
            } else if (arg0->x000A == 2) {
                put_main_cursor2(0xD8, 0x38, 7);
            }
        } else if (arg0->x0003 < 6) {
            if (arg0->x000A == 1) {
                put_main_cursor(7);
            }
        } else if (arg0->x000A == 2) {
            put_main_cursor(7);
        }
    }
}
