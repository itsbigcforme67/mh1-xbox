/* G online (PC): plaza_checkChatLog, written from Monster Hunter G's code at lobby 0x572F10 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Plaza_chatlog_mv();
s32 tl_exit_sub_menu();
extern u8 pNet;
s32 Get_sw2();                               /* extern */
s16 Get_sw_on2();                            /* extern */
s32 Plaza_chatlog_i();                        /* extern */

void plaza_checkChatLog(void) {
    s32 temp_s0;
    u8 temp_a0;
    u8 *temp_a0_2;
    u8 *temp_a0_3;

    temp_a0 = (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x23));
    temp_s0 = Get_sw2(0) & 0xFFFF;
    switch (temp_a0) {                              /* irregular */
    case 0:
        Plaza_chatlog_i(temp_a0);
        temp_a0_2 = (*(void **)&pNet);
        (*(u8 *)((u8 *)(temp_a0_2) + 0x23)) = (u8) ((*(u8 *)((u8 *)(temp_a0_2) + 0x23)) + 1);
        return;
    case 1:
        temp_a0_3 = (*(void **)&pNet);
        (*(s16 *)((u8 *)(temp_a0_3) + 0x20)) = Get_sw_on2(0);
        if (temp_s0 & 0xFFFF & 0x40) {
            Plaza_chatlog_i((u8) temp_a0_3);
            tl_exit_sub_menu(0);
            return;
        }
        Plaza_chatlog_mv(temp_s0);
        return;
    }
}

