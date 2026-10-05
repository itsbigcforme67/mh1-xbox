/* lb_bz96 - lobby UI/client 0x005C45B0-0x005C4660: lb_npc_chr_sub (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
typedef struct { u8 pad0000[0x2E4]; u16 x02E4; u16 x02E6; u8 pad02E8[0x4]; s16 x02EC; s16 x02EE; u8 pad02F0[0xC]; u8 x02FC; u8 x02FD; u8 pad02FE[0x2]; u16 x0300; u8 pad0302[0x108]; u8 x040A; } ARG_lb_npc_chr_sub_arg0;

void lb_npc_chr_sub(ARG_lb_npc_chr_sub_arg0 *arg0) {
    u8 temp_v0;
    u8 temp_v1;

    if (softdip_ck(0x28) == 0) {
        cpRotMatrix((u8 *)arg0 + 0xA0, (u8 *)arg0 + 0x20);
        if (arg0->x040A == 0) {
            temp_v0 = arg0->x02FC;
            if (temp_v0 == 0) {
                arg0->x02FC = (u8) (temp_v0 + 1);
                frame_init((u8 *)arg0, arg0->x02E4, arg0->x02EC, 0);
            }
            temp_v1 = arg0->x02FD;
            if ((temp_v1 == 0) && (arg0->x0300 > 1)) {
                arg0->x02FD = (u8) (temp_v1 + 1);
                frame_init((u8 *)arg0, arg0->x02E6, arg0->x02EE, 1);
            }
            frame_move((u8 *)arg0);
        }
    }
}
