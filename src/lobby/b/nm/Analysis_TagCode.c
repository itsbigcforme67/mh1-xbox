#include "lobby_a.h"
extern s8 html_start_flag;
extern s8 html_tag_flag;
extern int dp;
extern char check_tag_table_3534[];
extern char tag_buffer[];
extern char tag_buffer[];
extern char tag_buffer[];
extern char tag_buffer[];
extern char tag_buffer[];
extern char tag_buffer[];
extern char tag_buffer[];
extern char tag_buffer[];
extern char tag_buffer[];
extern char tag_buffer[];
extern char tag_buffer[];
extern char tag_buffer[];
extern char tag_buffer[];
extern char tag_buffer[];
extern char tag_buffer[];
extern char tag_buffer[];
extern char tag_buffer[];
void Analysis_TagCode(void) {
    char sp38[8];   /* the asm clears 8 bytes at sp+0x38: an int here overflowed on the PC */
    int var_s1;
    s8 var_s0;
    s8 var_v1;
    int temp_a0;
    int temp_a0_2;
    int temp_a0_3;
    int temp_a0_4;
    int temp_a0_5;
    int temp_a0_6;
    int temp_a0_7;
    int temp_a0_8;
    int temp_a1;
    int temp_a1_2;
    int temp_v1;
    int temp_v1_2;
    int temp_v1_3;
    int temp_v1_4;

    var_s0 = 0;
    var_s1 = (int)&check_tag_table_3534;
loop_1:
    if (strstr(&tag_buffer, (*(s32 *)var_s1)) == 0) {
        var_s0 += 1;
        var_s1 += 4;
        if (var_s0 < 0xB) {
            goto loop_1;
        }
    }
    switch (var_s0) {
    case 1:
        temp_a1 = dp;
        var_v1 = 1;
        F(s32, temp_a1, 4) = (F(s32, temp_a1, 4) | 1);
block_54:
        html_start_flag = var_v1;
        break;
    case 4:
        temp_a1_2 = dp;
        var_v1 = 2;
        F(s32, temp_a1_2, 4) = (F(s32, temp_a1_2, 4) | 8);
        goto block_54;
    case 5:
        temp_a0 = dp;
        F(s32, temp_a0, 4) = (F(s32, temp_a0, 4) | 0x10);
        break;
    case 6:
        temp_a0_2 = dp;
        F(s32, temp_a0_2, 4) = (F(s32, temp_a0_2, 4) | 0x20);
        break;
    case 7:
        temp_a0_3 = dp;
        F(s32, temp_a0_3, 4) = (F(s32, temp_a0_3, 4) | 0x40);
        break;
    case 8:
        temp_a0_4 = dp;
        F(s32, temp_a0_4, 4) = (F(s32, temp_a0_4, 4) | 0x80);
        break;
    case 2:
        temp_a0_5 = dp;
        F(s32, temp_a0_5, 4) = (F(s32, temp_a0_5, 4) | 2);
        if ((F(s8, &tag_buffer, 5) >= 0x30) && (F(s8, &tag_buffer, 5) < 0x3A)) {
            F(u8, dp, 9) = (u8) (F(s8, &tag_buffer, 5) - 0x30);
        } else if ((F(s8, &tag_buffer, 5) >= 0x61) && (F(s8, &tag_buffer, 5) < 0x67)) {
            F(u8, dp, 9) = (u8) (F(s8, &tag_buffer, 5) - 0x57);
        } else {
            F(u8, dp, 9) = (u8) (F(s8, &tag_buffer, 5) - 0x37);
        }
        temp_v1 = dp;
        if (F(u8, temp_v1, 9) >= 0x10) {
            F(u8, temp_v1, 9) = 5U;
        }
        break;
    case 3:
        memset(sp38, 0, 8);
        temp_a0_6 = dp;
        F(s32, temp_a0_6, 4) = (F(s32, temp_a0_6, 4) | 4);
        if ((F(s8, &tag_buffer, 6) >= 0x30) && (F(s8, &tag_buffer, 6) < 0x3A)) {
            F(u8, dp, 8) = (u8) (F(s8, &tag_buffer, 6) - 0x30);
        } else if ((F(s8, &tag_buffer, 6) >= 0x61) && (F(s8, &tag_buffer, 6) < 0x67)) {
            F(u8, dp, 8) = (u8) (F(s8, &tag_buffer, 6) - 0x57);
        } else {
            F(u8, dp, 8) = (u8) (F(s8, &tag_buffer, 6) - 0x37);
        }
        temp_v1_2 = dp;
        if (F(u8, temp_v1_2, 8) >= 0x10) {
            F(u8, temp_v1_2, 8) = 7U;
        }
        break;
    case 10:
        memset(sp38, 0, 8);
        temp_a0_7 = dp;
        F(s32, temp_a0_7, 4) = (F(s32, temp_a0_7, 4) | 4);
        if ((F(s8, &tag_buffer, 2) >= 0x30) && (F(s8, &tag_buffer, 2) < 0x3A)) {
            F(u8, dp, 8) = (u8) (F(s8, &tag_buffer, 2) - 0x30);
        } else if ((F(s8, &tag_buffer, 2) >= 0x61) && (F(s8, &tag_buffer, 2) < 0x67)) {
            F(u8, dp, 8) = (u8) (F(s8, &tag_buffer, 2) - 0x57);
        } else {
            F(u8, dp, 8) = (u8) (F(s8, &tag_buffer, 2) - 0x37);
        }
        temp_v1_3 = dp;
        if (F(u8, temp_v1_3, 8) >= 0x10) {
            F(u8, temp_v1_3, 8) = 7U;
        }
        break;
    case 9:
        temp_a0_8 = dp;
        F(s32, temp_a0_8, 4) = (F(s32, temp_a0_8, 4) | 0x100);
        F(u8, dp, 0xA) = (u8) (F(s8, &tag_buffer, 3) - 0x30);
        temp_v1_4 = dp;
        if (F(u8, temp_v1_4, 0xA) >= 0xA) {
            F(u8, temp_v1_4, 0xA) = 9U;
        }
        var_v1 = 2;
        goto block_54;
    }
    html_tag_flag = var_s0;
}
