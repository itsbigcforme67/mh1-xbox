#include "lobby_a.h"
s32 join_input_password(s32 arg0) {
    s8 sx1;
    u8 temp_v1;
    int temp_s1;

    Get_sw(0);
    Get_kb_input();
    temp_s1 = Lbs_GetRoomInfo(F(u8, pNet, 7));
    switch (F(s8, &lb_sys, 7)) {          /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        F(s8, &lb_sys, 7) = (s8) (F(s8, &lb_sys, 7) + 1);
        SoftKeyboard_pos_set(0x42A00000, 0x3A);
        SoftKeyboard_set(0, 6, 8, arg0);
        *(s8 *)0x3F36AB = 0;
    case 3:                                         /* switch 2 */
block_19:
    default:                                        /* switch 1 */
        return 2;
    case 1:                                         /* switch 1 */
        if ((sx1 = SoftKeyboard_move(arg0, *(s16 *)0x3F3710, *(s16 *)0x3F3714)) != 0) {
            F(s8, &lb_sys, 7) = (s8) (F(s8, &lb_sys, 7) + 1);
            goto block_19;
        }
        temp_v1 = F(u8, temp_s1, 0x10);
        switch (temp_v1) {                          /* switch 2; irregular */
        case 4:                                     /* switch 2 */
            Lb_put_set01(7);
block_15:
            SoftKeyboard_exit();
            F(s8, &lb_sys, 7) = 0;
            *(u8 *)0x3F36AB = 1;
            return 3;
        default:                                    /* switch 2 */
            Lb_put_set01(6);
            goto block_15;
        }
        break;
    case 2:                                         /* switch 1 */
        SoftKeyboard_exit();
        F(s8, &lb_sys, 7) = 0;
        *(u8 *)0x3F36AB = 1;
        return 0;
    }
}
