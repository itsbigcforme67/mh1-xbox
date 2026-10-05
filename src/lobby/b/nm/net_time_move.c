#include "lobby_a.h"
extern s32 net_time_flag;
typedef struct { u8 pad0000[0x1]; s8 x0001; u8 pad0002[0x2]; u8 x0004; u8 x0005; u8 pad0006[0x12]; int x0018; } ARG_net_time_move_arg0;
void net_time_move(ARG_net_time_move_arg0 *arg0) {
    s16 temp_v0;
    s16 temp_v1_3;
    u8 temp_v1;
    u8 temp_v1_2;
    u8 temp_v1_4;
    int temp_a2;

    temp_v1 = arg0->x0004;
    temp_a2 = arg0->x0018;
    switch (temp_v1) {                              /* switch 1; irregular */
    case 2:                                         /* switch 1 */
        break;
    case 0:                                         /* switch 1 */
        temp_v1_2 = arg0->x0005;
        switch (temp_v1_2) {                        /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            arg0->x0005 = (u8) (temp_v1_2 + 1);
            F(s16, temp_a2, 4) = 0x78;
            break;
        case 1:                                     /* switch 2 */
            if (F(s8, (u8 *)cw, 0x2C30) != 0) {
                arg0->x0005 = 0U;
            } else {
                temp_v1_3 = F(s16, temp_a2, 4) - 1;
                F(s16, temp_a2, 4) = temp_v1_3;
                if (((s16)temp_v1_3) == 0) {
                    arg0->x0004 = 1U;
                    arg0->x0005 = 0U;
                } else if (*(u16 *)0x3F3714 != 0) {
                    arg0->x0005 = 0U;
                }
            }
            break;
        }
        break;
    case 1:                                         /* switch 1 */
        temp_v1_4 = arg0->x0005;
        switch (temp_v1_4) {                        /* switch 3; irregular */
        case 0:                                     /* switch 3 */
            arg0->x0005 = (u8) (temp_v1_4 + 1);
            F(s16, temp_a2, 4) = 0x10;
            arg0->x0001 = 1;
            /* fallthrough */
        case 1:                                     /* switch 3 */
            temp_v0 = F(s16, temp_a2, 4) - 1;
            F(s16, temp_a2, 4) = temp_v0;
            if (((s16)temp_v0) == 0) {
                arg0->x0005 = (u8) (arg0->x0005 + 1);
            }
            net_time_str(0x3F800000, 1, 2, temp_a2);
            break;
        case 2:                                     /* switch 3 */
            if (F(s8, (u8 *)cw, 0x2C30) != 0) {
                arg0->x0004 = 0U;
                arg0->x0005 = 0U;
                arg0->x0001 = 0;
            } else {
                if (*(u8 *)0x3F3714 != 0) {
                    arg0->x0004 = 0U;
                    arg0->x0005 = 0U;
                    arg0->x0001 = 0;
                }
                net_time_str(0x3F800000, 1, 2, temp_a2);
            }
            break;
        }
        break;
    }
    if (net_time_flag == 0) {
        cnWrap_PushWork((u8 *)arg0);
    }
}
