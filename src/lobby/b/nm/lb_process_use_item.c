#include "lobby_s.h"
extern char buki_sei_tbl[];
extern char kakou_tbl[];
extern char bou_sei_tbl[];
void lb_process_use_item(s32 arg0) {
    s32 var_s0;
    s32 var_s0_2;
    s32 var_s0_3;
    u16 temp_a0;
    u16 temp_a0_2;
    u16 temp_a0_3;
    int var_s1;
    int var_s1_2;
    int var_s1_3;

    switch (lbShop.mode) {       /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        switch (lbShop.x1A) {   /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            var_s0 = 0;
            var_s1 = (int)&buki_sei_tbl + (*(s32 *)((int)&shopList + 0x26 + (arg0 * 0x28)) * 0x18);
            do {
                temp_a0 = F(u16, var_s1, 4);
                if (temp_a0 != 0) {
                    Ud_item_stack(temp_a0,  ( -F(s16, var_s1, 6) << 0x30) >> 0x30);
                }
                var_s0 += 1;
                var_s1 += 4;
            } while (var_s0 < 4);
            return;
        case 1:                                     /* switch 2 */
            var_s0_2 = 0;
            var_s1_2 = (int)&kakou_tbl + (F(s32, ((int)lbShop.tbl + (lbShop.cur * 8)), 4) * 0x18);
            do {
                temp_a0_2 = F(u16, var_s1_2, 0);
                if (temp_a0_2 != 0) {
                    Ud_item_stack(temp_a0_2,  ( -F(s16, var_s1_2, 2) << 0x30) >> 0x30);
                }
                var_s0_2 += 1;
                var_s1_2 += 4;
            } while (var_s0_2 < 3);
            return;
        }
        break;
    case 1:                                         /* switch 1 */
        var_s0_3 = 0;
        var_s1_3 = (int)&bou_sei_tbl + (*(s32 *)((int)&shopList + 0x26 + (arg0 * 0x28)) * 0x18);
        do {
            temp_a0_3 = F(u16, var_s1_3, 4);
            if (temp_a0_3 != 0) {
                Ud_item_stack(temp_a0_3,  ( -F(s16, var_s1_3, 6) << 0x30) >> 0x30);
            }
            var_s0_3 += 1;
            var_s1_3 += 4;
        } while (var_s0_3 < 4);
        return;
    }
}
