/* G online (PC): lb_send_my_status, written from Monster Hunter G's code at lobby 0x559A20 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 game_w;
extern u8 lb_sys;
s32 Lb_send_trade_startTU();    /* extern */
s32 lobby_func_0059AB90();           /* extern */
s32 lobby_func_0059AD00();                    /* extern */
extern u8 player_work;

void lb_send_my_status(s32 arg0) {
    s32 temp_a2;
    u8 temp_a0;

    if (((*(s8 *)((u8 *)&lb_sys + 0x19)) == 4) && ((lobby_func_0059AD00((*(u8 *)((u8 *)&game_w + 0x14))), temp_a0 = (*(u8 *)((u8 *)&game_w + 0x14)), (temp_a0 == 0x4D)) || (temp_a0 == 0x4C))) {
        lobby_func_0059AB90((u8 *)&player_work + ((*(u8 *)((u8 *)&game_w + 0x3A)) * 0xA00), arg0);
        temp_a2 = (*(u8 *)((u8 *)&game_w + 0x3A)) * 0xA00;
        if (((*(s32 *)((u8 *)&(*(u8 *)((u8 *)&player_work + 0x90B)) + temp_a2)) != 0) && ((*(s32 *)((u8 *)&(*(u8 *)((u8 *)&player_work + 0x15)) + temp_a2)) == 0x2D) && ((*(s32 *)((u8 *)&(*(u8 *)((u8 *)&player_work + 0x14)) + temp_a2)) == 0)) {
            Lb_send_trade_startTU((u8 *)&player_work + temp_a2, arg0, temp_a2);
        }
    }
}

