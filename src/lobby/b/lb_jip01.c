/* lb_jip01 - agent C 0x005B08F0-0x005B0A7C: join_input_password (guild room password entry; inner switch with shared exit block via goto). */
#include "lobby_b.h"
extern void SoftKeyboard_pos_set(f32, int);
s32 join_input_password(s32 arg0) {
    s8 sx1;
    u8 v;
    u8 *room;

    Get_sw(0);
    Get_kb_input();
    room = (u8 *)Lbs_GetRoomInfo(pNet->sel);
    switch (lb_sys.x07) {
    case 0:
        lb_sys.x07 = lb_sys.x07 + 1;
        SoftKeyboard_pos_set(80.0f, 0x3A);
        SoftKeyboard_set(0, 6, 8, arg0);
        *(s8 *)0x3F36AB = 0;
        break;
    case 1:
        if ((sx1 = SoftKeyboard_move(arg0, *(s16 *)0x3F3710, *(s16 *)0x3F3714)) != 0) {
            lb_sys.x07 = lb_sys.x07 + 1;
            break;
        }
        v = room[0x10];
        switch (v) {
        case 4:
            Lb_put_set01(7);
            goto fin;
        case 3:
            break;
        default:
            Lb_put_set01(6);
fin:
            SoftKeyboard_exit();
            lb_sys.x07 = 0;
            *(u8 *)0x3F36AB = 1;
            return 3;
        }
        break;
    case 2:
        SoftKeyboard_exit();
        lb_sys.x07 = 0;
        *(u8 *)0x3F36AB = 1;
        return 0;
    }
    return 2;
}
