/* lb_gl02 - near-match fixes 0x005D4960-0x005D4A94: lb_goto_guest_room. Whole file in lb_l.c. */
#include "lobby_f.h"





void lb_goto_guest_room(PLW *pl, int no) {
    u8 *p;
    u8 v;
    u16 r;
    s32 *t;
    u8 m;
    u8 *c;
    switch (Lb_check_hotel(no)) {
    case 1:
        if (no == 0x55) {
            r = ran_suu(1);
            CW8(0x35D8) = (r % 27) * 2;
        }
        t = &lbs_command_jmp[0xB3];
        Gold_add(-t[no]);
        cnWrap_SoundRequest(8);
        p = D_3C7357 + no;
        v = *p;
        if (v < 0x96) {
            *p = v + 1;
        }
    case 2:
        ((u8 *)&lb_sys)[0x71] = 0;
        lb_sys.x03 = 5;
        F(s16, pl, 0x73A) = no;
        c = cw;
        c[0x35D7] |= (1 << (no - 0x51)) & 0xFF;
        lb_sys.x68 = 0x14;
        break;
    case 0:
        break;
    case 3:
        Lb_put_set01(0xA);
    }
}
