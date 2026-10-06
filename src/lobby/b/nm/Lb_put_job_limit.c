#include "lobby_a.h"
extern char User_data[];
extern char lit_543_00655878[];
extern char shop_warning[];
void Lb_put_job_limit(s8 arg0, s16 arg1) {
    s16 sp2A;
    s8 sp29;
    int sp28;
    s32 temp_a0;
    s32 temp_a0_2;
    s32 temp_a2;
    s32 temp_a2_2;
    s32 var_s0;

    temp_a2 = arg0 & 0xFFFF;
    if ((temp_a2 != 7) && (temp_a2 != 6)) {
        sp29 = arg0;
        sp2A = arg1;
        temp_a2_2 = Get_equip_bit(&User_data, &sp28, temp_a2) & 0xFF;
        var_s0 = 0xFF;
        if ((temp_a2_2 & 3) != 3) {
            temp_a0 = temp_a2_2 & 0xC;
            if (temp_a2_2 & 1) {
                switch (temp_a0) {                  /* switch 1; irregular */
                case 4:                             /* switch 1 */
                    var_s0 = 7;
                    break;
                case 8:                             /* switch 1 */
                    var_s0 = 5;
                    break;
                default:                            /* switch 1 */
                    var_s0 = 3 & 0xFF;
                    break;
                }
            } else {
                temp_a0_2 = temp_a2_2 & 0xC;
                switch (temp_a0_2) {                /* switch 2; irregular */
                case 4:                             /* switch 2 */
                    var_s0 = 6;
                    break;
                case 8:                             /* switch 2 */
                    var_s0 = 4;
                    break;
                default:                            /* switch 2 */
                    var_s0 = 2;
                    break;
                }
            }
        } else if ((temp_a2_2 & 0xC) != 0xC) {
            var_s0 = 1;
            if (temp_a2_2 & 4) {

            } else {
                var_s0 = 0;
            }
        }
        if ((var_s0 & 0xFF) != 0xFF) {
            font_set_palette(5, 3, temp_a2_2);
            flfntLocate(0x1D0, 0x11B);
            font_print(&lit_543_00655878, *(s32 *)((int)&shop_warning + ((var_s0 & 0xFF) * 4)));
        }
    }
}
