#include "lobby_s.h"
extern s32 armorIndex;
extern char shopList2[];
extern char D_3C7005[];
extern char shop_process2_help[];
extern char User_data[];
extern char shop_process2_help[];
extern char shop_process2_help[];
extern char User_data[];
extern char shop_process2_help[];
extern char shop_process2_help[];
extern char shop_process2_help[];
extern char D_3C7005[];
extern char D_3C7005[];
extern char shop_process2_help[];
extern char shop_process2_help[];
extern char shop_process2_help[];
extern char shop_process2_help[];
extern char shop_process2_help[];
extern char shop_process2_help[];
extern char shop_process2_help[];
extern char shop_process2_help[];
s32 lb_process_kyoukaListProg(void) {
    int var_at;
    int var_at_2;
    s32 temp_a1;
    s32 temp_a1_2;
    s32 temp_a1_3;
    s32 temp_v0;
    s32 temp_v1_2;
    s32 var_v0;
    s32 var_v0_4;
    int var_v0_2;
    int var_v0_3;
    s8 temp_v0_2;
    s8 temp_v0_3;
    u16 temp_v0_4;
    u16 temp_v1;
    int temp_s0;

    temp_a1 = lbShop.x70 * 0x28;
    temp_s0 = (int)&shopList2 + temp_a1;
    if (lbShop.key & 0x20) {
        if (F(s16, temp_s0, 0x24) == 0) {
            temp_a1_2 = armorIndex;
            if (*(s32 *)((int)&D_3C7005 + (temp_a1_2 * 6)) != 7) {
                var_v0 = F(s32, &shop_process2_help, 0x14);
                var_at = (int)&lbShop + 0x4C;
            } else {
                temp_v1 = F(u16, temp_s0, 0x26);
                switch (temp_v1) {                  /* switch 1; irregular */
                case 1:                             /* switch 1 */
                    if (Gun_option_ck(&User_data, (s16)temp_a1_2, 0x20) == 1) {
                        var_v0 = F(s32, &shop_process2_help, 0x4C);
                        var_at = (int)&lbShop + 0x4C;
                    } else {
                        var_v0 = F(s32, &shop_process2_help, 0x44);
                        var_at = (int)&lbShop + 0x4C;
                    }
                    break;
                case 3:                             /* switch 1 */
                    if (Gun_option_ck(&User_data, (s16)temp_a1_2, 0x10) == 1) {
                        var_v0 = F(s32, &shop_process2_help, 0x48);
                        var_at = (int)&lbShop + 0x4C;
                    } else {
                        var_v0 = F(s32, &shop_process2_help, 0x44);
                        var_at = (int)&lbShop + 0x4C;
                    }
                    break;
                default:                            /* switch 1 */
                    var_v0 = F(s32, &shop_process2_help, 0x44);
                    var_at = (int)&lbShop + 0x4C;
                    break;
                }
            }
            (*(s32 *)var_at) = var_v0;
            cnWrap_SoundRequest(0);
            lbShop.x6E = 0;
            lbShop.x84 = 2;
            return 0;
        }
        cnWrap_SoundRequest(7, temp_a1);
        goto block_64;
    }
    if (lbShop.key & 0x40) {
        cnWrap_SoundRequest(3, temp_a1);
        if (lbShop.x1C == 0) {
            lbShop.x70 = 0;
            lb_process_tag_decide01();
            return 3;
        }
        lbShop.x1C = 0;
        goto block_64;
    }
    if (lbShop.key & 0x200) {
        if (F(s16, temp_s0, 0x24) != 2) {
            cnWrap_SoundRequest(0xE, temp_a1);
            if (lbShop.x1C != 1) {
                lbShop.x1C = 1;
                lbShop.x6E = 0;
            } else {
                lbShop.x1C = 0;
            }
        } else {
            cnWrap_SoundRequest(7, temp_a1);
        }
    } else if (lbShop.key & 0x80) {
        temp_a1_3 = armorIndex;
        if (*(s32 *)((int)&D_3C7005 + (temp_a1_3 * 6)) != 7) {
            if (F(s16, temp_s0, 0x24) != 2) {
                cnWrap_SoundRequest(0xF, temp_a1_3);
                if (lbShop.x1C != 2) {
                    lbShop.x1C = 2;
                } else {
                    lbShop.x1C = 0;
                }
            } else {
                cnWrap_SoundRequest(7, temp_a1_3);
            }
        }
    } else if (lbShop.key & 0x2000) {
        if (lbShop.x1C == 0) {
            cnWrap_SoundRequest(1, temp_a1);
            temp_v0 = lbShop.x70 - 1;
            lbShop.x70 = temp_v0;
            if (temp_v0 < 0) {
                lbShop.x70 = (lbShop.count - 1);
            }
        }
    } else if (lbShop.key & 0x1000) {
        if (lbShop.x1C == 0) {
            cnWrap_SoundRequest(1, temp_a1);
            temp_v1_2 = lbShop.x70 + 1;
            lbShop.x70 = temp_v1_2;
            if (temp_v1_2 >= lbShop.count) {
                lbShop.x70 = 0;
            }
        }
    } else if (lbShop.key & 0x800) {
        if (lbShop.x1C == 1) {
            cnWrap_SoundRequest(1, temp_a1);
            var_v0_2 = 2;
            if (lbShop.tbl[(lbShop.cur) * 2] == 7) {
                var_v0_2 = 4;
            }
            temp_v0_2 = lbShop.x6E - 1;
            lbShop.x6E = temp_v0_2;
            if (((s8)temp_v0_2) < 0) {
                lbShop.x6E = (s8) (( (((s8)var_v0_2) << 0x38) >> 0x38) - 1);
            }
        }
    } else if ((lbShop.key & 0x400) && (lbShop.x1C == 1)) {
        cnWrap_SoundRequest(1, temp_a1);
        var_v0_3 = 2;
        if (lbShop.tbl[(lbShop.cur) * 2] == 7) {
            var_v0_3 = 4;
        }
        temp_v0_3 = lbShop.x6E + 1;
        lbShop.x6E = temp_v0_3;
        if (((s8)temp_v0_3) >= ((s8)var_v0_3)) {
            lbShop.x6E = 0;
        }
    }
block_64:
    if (lbShop.x1C != 0) {
        if (*(s32 *)((int)&D_3C7005 + (armorIndex * 6)) != 7) {
            goto block_67;
        }
        temp_v0_4 = F(u16, temp_s0, 0x26);
        switch (temp_v0_4) {                        /* switch 2 */
        case 0:                                     /* switch 2 */
            var_v0_4 = F(s32, &shop_process2_help, 0x28);
            var_at_2 = (int)&lbShop + 0x4C;
            goto block_84;
        case 1:                                     /* switch 2 */
            var_v0_4 = F(s32, &shop_process2_help, 0x2C);
            var_at_2 = (int)&lbShop + 0x4C;
            goto block_84;
        case 2:                                     /* switch 2 */
            var_v0_4 = F(s32, &shop_process2_help, 0x30);
            var_at_2 = (int)&lbShop + 0x4C;
            goto block_84;
        case 3:                                     /* switch 2 */
            var_v0_4 = F(s32, &shop_process2_help, 0x34);
            var_at_2 = (int)&lbShop + 0x4C;
            goto block_84;
        case 4:                                     /* switch 2 */
            var_v0_4 = F(s32, &shop_process2_help, 0x38);
            var_at_2 = (int)&lbShop + 0x4C;
            goto block_84;
        case 5:                                     /* switch 2 */
            var_v0_4 = F(s32, &shop_process2_help, 0x3C);
            var_at_2 = (int)&lbShop + 0x4C;
            goto block_84;
        case 6:                                     /* switch 2 */
            var_v0_4 = F(s32, &shop_process2_help, 0x40);
            var_at_2 = (int)&lbShop + 0x4C;
            goto block_84;
        }
    } else {
block_67:
        var_v0_4 = F(s32, &shop_process2_help, 0xC);
        var_at_2 = (int)&lbShop + 0x4C;
block_84:
        (*(s32 *)var_at_2) = var_v0_4;
    }
    lbShop.cur = (lbShop.x70 + (lbShop.x6C * 7));
    return 2;
}
