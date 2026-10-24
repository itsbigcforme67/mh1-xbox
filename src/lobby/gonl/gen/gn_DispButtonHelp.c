/* G online (PC): DispButtonHelp, written from Monster Hunter G's code at lobby 0x5A0F60 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 SetTextureStage();
s32 flfntSetSize();
#ifdef __MWERKS__
s32 func_0012D000();
#else
unsigned short func_0012D000();   /* the real return type (tools/check_kr_returns.py) */
#endif
s32 lobby_func_005A0E80();
s32 put_button_help();
s32 reload_tex();
extern u8 g_main_2EFBAC;
extern u8 cw;
extern u8 g_main_2EFBAC;
extern u8 tl_member_buff;
extern void *g_lb_636B54;

void DispButtonHelp(void *arg0) {
    s32 temp_s0_10;
    s32 temp_s0_11;
    s32 temp_s0_12;
    s32 temp_s0_13;
    s32 temp_s0_14;
    s32 temp_s0_15;
    s32 temp_s0_16;
    s32 temp_s0_17;
    s32 temp_s0_18;
    s32 temp_s0_19;
    s32 temp_s0_20;
    s32 temp_s0_21;
    s32 temp_s0_2;
    s32 temp_s0_3;
    s32 temp_s0_4;
    s32 temp_s0_5;
    s32 temp_s0_6;
    s32 temp_s0_7;
    s32 temp_s0_8;
    s32 temp_s0_9;
    s32 var_s0;
    u16 temp_s0;
    u8 temp_a0;
    u8 temp_v0;
    u8 temp_v1;

    temp_s0 = (*(u16 *)((u8 *)(arg0) + 0x20));
    if ((*(u8 *)((u8 *)(arg0) + 0x2D)) == 0) {
        flfntSetSize(0x12, 0x12);
        reload_tex(1, 0x180);
        SetTextureStage(0x180);
        temp_v1 = (*(u8 *)((u8 *)(arg0) + 0x28));
        switch (temp_v1) {                          /* switch 1 */
        default:                                    /* switch 1 */
            put_button_help(0, 0, 2, func_0012D000(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */)) & 0xFFFF & 0x100 & 0xFFFF);
            put_button_help(1, 1, 3, func_0012D000(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */)) & 0xFFFF & 0x200 & 0xFFFF);
            return;
        case 0:                                     /* switch 1 */
            temp_a0 = (*(u8 *)((u8 *)(arg0) + 0x23));
            switch (temp_a0) {                      /* switch 2; irregular */
            case 1:                                 /* switch 2 */
            case 5:                                 /* switch 2 */
                temp_s0_2 = temp_s0 & 0xFFFF;
                put_button_help(1, 2, 3, temp_s0_2 & 0x200 & 0xFFFF);
                put_button_help(3, 4, 0, temp_s0_2 & 0x20 & 0xFFFF);
                put_button_help(2, 5, 1, temp_s0_2 & 0x40 & 0xFFFF);
                return;
            case 3:                                 /* switch 2 */
                temp_s0_3 = temp_s0 & 0xFFFF;
                put_button_help(1, 3, 3, temp_s0_3 & 0x200 & 0xFFFF);
                put_button_help(3, 4, 0, temp_s0_3 & 0x20 & 0xFFFF);
                put_button_help(2, 5, 1, temp_s0_3 & 0x40 & 0xFFFF);
                return;
            case 2:                                 /* switch 2 */
            case 4:                                 /* switch 2 */
                lobby_func_005A0E80(g_lb_636B54, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                return;
            }
            break;
        case 1:                                     /* switch 1 */
            temp_s0_4 = temp_s0 & 0xFFFF;
            put_button_help(2, 5, 1, temp_s0_4 & 0x40 & 0xFFFF);
            put_button_help(3, 4, 0, temp_s0_4 & 0x20 & 0xFFFF);
            return;
        case 14:                                    /* switch 1 */
            switch ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C))) { /* switch 3; irregular */
            case 3:                                 /* switch 3 */
                temp_s0_5 = temp_s0 & 0xFFFF;
                put_button_help(1, 0xE, 3, temp_s0_5 & 0x200 & 0xFFFF);
                put_button_help(2, 5, 1, temp_s0_5 & 0x40 & 0xFFFF);
                return;
            case 1:                                 /* switch 3 */
                temp_s0_6 = temp_s0 & 0xFFFF;
                put_button_help(1, 0x17, 3, temp_s0_6 & 0x200 & 0xFFFF);
                put_button_help(3, 0x19, 0, temp_s0_6 & 0x20 & 0xFFFF);
                put_button_help(2, 5, 1, temp_s0_6 & 0x40 & 0xFFFF);
                return;
            case 2:                                 /* switch 3 */
                temp_s0_7 = temp_s0 & 0xFFFF;
                put_button_help(3, 0x1A, 0, temp_s0_7 & 0x20 & 0xFFFF);
                put_button_help(2, 5, 1, temp_s0_7 & 0x40 & 0xFFFF);
                return;
            }
            break;
        case 3:                                     /* switch 1 */
            switch ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C))) { /* switch 4; irregular */
            case 4:                                 /* switch 4 */
            case 3:                                 /* switch 4 */
            case 1:                                 /* switch 4 */
                lobby_func_005A0E80(g_lb_636B54, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                return;
            case 2:                                 /* switch 4 */
                temp_s0_8 = temp_s0 & 0xFFFF;
                put_button_help(0, 6, 6, temp_s0_8 & 0x80 & 0xFFFF);
                put_button_help(1, 0xE, 3, temp_s0_8 & 0x200 & 0xFFFF);
                put_button_help(2, 5, 1, temp_s0_8 & 0x40 & 0xFFFF);
                put_button_help(3, 7, 0, temp_s0_8 & 0x20 & 0xFFFF);
                return;
            case 5:                                 /* switch 4 */
                temp_s0_9 = temp_s0 & 0xFFFF;
                put_button_help(3, 6, 6, temp_s0_9 & 0x80 & 0xFFFF);
                if ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xFA61)) == 0) {
                    put_button_help(0, 0x13, 2, temp_s0_9 & 0x100 & 0xFFFF);
                }
                put_button_help(1, 0xE, 3, temp_s0_9 & 0x200 & 0xFFFF);
                put_button_help(2, 5, 1, temp_s0_9 & 0x40 & 0xFFFF);
                return;
            case 12:                                /* switch 4 */
                temp_s0_10 = temp_s0 & 0xFFFF;
                put_button_help(2, 5, 1, temp_s0_10 & 0x40 & 0xFFFF);
                put_button_help(3, 4, 0, temp_s0_10 & 0x20 & 0xFFFF);
                return;
            }
            break;
        case 5:                                     /* switch 1 */
            switch ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A))) { /* switch 5; irregular */
            case 8:                                 /* switch 5 */
                if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136F)) != 2) {
                    put_button_help(1, 9, 3, temp_s0 & 0xFFFF & 0x200 & 0xFFFF);
                }
                temp_s0_11 = temp_s0 & 0xFFFF;
                put_button_help(3, 7, 0, temp_s0_11 & 0x20 & 0xFFFF);
                put_button_help(2, 5, 1, temp_s0_11 & 0x40 & 0xFFFF);
                return;
            case 11:                                /* switch 5 */
                temp_s0_12 = temp_s0 & 0xFFFF;
                put_button_help(1, 9, 3, temp_s0_12 & 0x200 & 0xFFFF);
                put_button_help(2, 5, 1, temp_s0_12 & 0x40 & 0xFFFF);
                put_button_help(0, 0x13, 2, temp_s0_12 & 0x100 & 0xFFFF);
                return;
            case 2:                                 /* switch 5 */
            case 6:                                 /* switch 5 */
                temp_s0_13 = temp_s0 & 0xFFFF;
                put_button_help(3, 4, 0, temp_s0_13 & 0x20 & 0xFFFF);
                put_button_help(2, 5, 1, temp_s0_13 & 0x40 & 0xFFFF);
                return;
            }
            break;
        case 6:                                     /* switch 1 */
            switch ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A))) { /* switch 6; irregular */
            case 2:                                 /* switch 6 */
                temp_s0_14 = temp_s0 & 0xFFFF;
                put_button_help(3, 4, 0, temp_s0_14 & 0x20 & 0xFFFF);
                put_button_help(2, 5, 1, temp_s0_14 & 0x40 & 0xFFFF);
                return;
            case 4:                                 /* switch 6 */
                temp_s0_15 = temp_s0 & 0xFFFF;
                put_button_help(3, 0x14, 0, temp_s0_15 & 0x20 & 0xFFFF);
                put_button_help(2, 5, 1, temp_s0_15 & 0x40 & 0xFFFF);
                return;
            case 6:                                 /* switch 6 */
                if ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1375)) == 0) {
                    var_s0 = temp_s0 & 0xFFFF;
                    put_button_help(1, 0x15, 3, var_s0 & 0x200 & 0xFFFF);
                } else {
                    var_s0 = temp_s0 & 0xFFFF;
                    put_button_help(1, 0x16, 3, var_s0 & 0x200 & 0xFFFF);
                }
                put_button_help(2, 5, 1, var_s0 & 0x40 & 0xFFFF);
                put_button_help(0, 0x13, 2, var_s0 & 0x100 & 0xFFFF);
                return;
            }
            break;
        case 4:                                     /* switch 1 */
            temp_v0 = (*(u8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x230));
            switch (temp_v0) {                      /* switch 7; irregular */
            case 0:                                 /* switch 7 */
                temp_s0_16 = temp_s0 & 0xFFFF;
                put_button_help(0, 0xF, 6, temp_s0_16 & 0x80 & 0xFFFF);
                put_button_help(1, 9, 3, temp_s0_16 & 0x200 & 0xFFFF);
                put_button_help(2, 5, 1, temp_s0_16 & 0x40 & 0xFFFF);
                put_button_help(3, 0xD, 0, temp_s0_16 & 0x20 & 0xFFFF);
                return;
            case 1:                                 /* switch 7 */
                temp_s0_17 = temp_s0 & 0xFFFF;
                put_button_help(0, 0xB, 6, temp_s0_17 & 0x80 & 0xFFFF);
                put_button_help(1, 9, 3, temp_s0_17 & 0x200 & 0xFFFF);
                put_button_help(2, 5, 1, temp_s0_17 & 0x40 & 0xFFFF);
                put_button_help(3, 0xC, 0xE, temp_s0_17 & 1);
                return;
            default:                                /* switch 7 */
                temp_s0_18 = temp_s0 & 0xFFFF;
                put_button_help(2, 5, 1, temp_s0_18 & 0x40 & 0xFFFF);
                put_button_help(3, 4, 0, temp_s0_18 & 0x20 & 0xFFFF);
                return;
            }
            break;
        case 7:                                     /* switch 1 */
            put_button_help(2, 5, 1, temp_s0 & 0xFFFF & 0x40 & 0xFFFF);
            return;
        case 8:                                     /* switch 1 */
        case 10:                                    /* switch 1 */
            temp_s0_19 = temp_s0 & 0xFFFF;
            put_button_help(2, 5, 1, temp_s0_19 & 0x40 & 0xFFFF);
            put_button_help(3, 0x10, 0, temp_s0_19 & 0x20 & 0xFFFF);
            return;
        case 9:                                     /* switch 1 */
            temp_s0_20 = temp_s0 & 0xFFFF;
            put_button_help(2, 5, 1, temp_s0_20 & 0x40 & 0xFFFF);
            put_button_help(3, 4, 0, temp_s0_20 & 0x20 & 0xFFFF);
            return;
        case 11:                                    /* switch 1 */
            temp_s0_21 = temp_s0 & 0xFFFF;
            put_button_help(1, 1, 3, temp_s0_21 & 0x200 & 0xFFFF);
            put_button_help(2, 5, 1, temp_s0_21 & 0x40 & 0xFFFF);
            break;
        }
    }
}

