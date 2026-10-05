/* lb_z96 - auto-drafted 0x00604D20-0x00604F20: init_table_data, init_tr_data, init_td_data, set_valign_data (first drafted by tools/lbauto.py). */
#include "lobby.h"
extern u8 * bsw;

void init_table_data(void) {
    memset(bsw + 0xDFC, 0, 0x114);
    F(s32, bsw, 0xE0C) = -1;
    F(s32, bsw, 0xE08) = -1;
    F(s16, bsw, 0xE00) = 2;
}

void init_tr_data(void) {
    F(s8, bsw, 0xF16) = 0;
    F(s8, bsw, 0xF17) = 0;
    F(s32, bsw, 0xF18) = -1;
}

void init_td_data(void) {
    memset(bsw + 0xF10, 0, 0x10C);
    F(s32, bsw, 0xF18) = -1;
    F(s8, bsw, 0x18A) = 0;
    F(s8, bsw, 0x18C) = 0;
}

void set_valign_data(u8 *arg0) {
    s32 temp_a1_2;
    s32 temp_a1_4;
    s32 var_v1;
    s32 var_v1_2;
    u8 temp_a1;
    u8 temp_a1_3;

    if (F(u8, arg0, 0x50) & 1) {
        F(s16, arg0, 0x3C) = 0;
        return;
    }
    if (F(u8, arg0, 0x48) == 0) {
        temp_a1 = F(u8, arg0, 0x4B);
        switch (temp_a1) {                          /* switch 1; irregular */
        case 3:                                     /* switch 1 */
            F(s16, arg0, 0x3C) = (s16) ((F(u16, bsw, 0xD8D4) + F(u16, arg0, 0x1E)) - F(u16, arg0, 0x38));
            return;
        case 2:                                     /* switch 1 */
            F(s16, arg0, 0x3C) = 0;
            return;
        default:                                    /* switch 1 */
            temp_a1_2 = (F(u16, bsw, 0xD8D4) + F(u16, arg0, 0x1E)) - F(u16, arg0, 0x38);
            var_v1 = temp_a1_2 >> 1;
            if (temp_a1_2 < 0) {
                var_v1 = (temp_a1_2 + 1) >> 1;
            }
            F(s16, arg0, 0x3C) = (s16) var_v1;
            return;
        }
    } else {
        temp_a1_3 = F(u8, arg0, 0x4B);
        switch (temp_a1_3) {                        /* switch 2; irregular */
        case 3:                                     /* switch 2 */
            F(s16, arg0, 0x3C) = (s16) ((F(u16, bsw, 0xD8D4) + F(u16, arg0, 0x40)) - F(u16, arg0, 0x38));
            return;
        case 2:                                     /* switch 2 */
            F(s16, arg0, 0x3C) = 0;
            return;
        default:                                    /* switch 2 */
            temp_a1_4 = (F(u16, bsw, 0xD8D4) + F(u16, arg0, 0x40)) - F(u16, arg0, 0x38);
            var_v1_2 = temp_a1_4 >> 1;
            if (temp_a1_4 < 0) {
                var_v1_2 = (temp_a1_4 + 1) >> 1;
            }
            F(s16, arg0, 0x3C) = (s16) var_v1_2;
            return;
        }
    }
}
