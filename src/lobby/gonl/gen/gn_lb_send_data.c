/* G online (PC): lb_send_data, written from Monster Hunter G's code at lobby 0x59B140 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Online_ck();
s32 cnLBS_Send_ChatBinary();
s32 flMemcpy();
extern u8 cw;
s32 Lbs_CheckMatchingFlag();                     /* extern */
extern s8 sendDat;

void lb_send_data(s32 arg0, s8 arg1, void *arg2) {
    s32 temp_a0;
    s32 temp_a2;
    u8 temp_a0_2;

    if ((Online_ck() != 0) && (temp_a0 = arg1 & 0xFF, ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xFA61)) != 0))) {
        if (temp_a0 != 3) {
            if (Lbs_CheckMatchingFlag(temp_a0) != 1) {
                goto block_5;
            }
        } else {
block_5:
            temp_a0_2 = (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB99));
            if ((temp_a0_2 != 2) && (temp_a0_2 != 3)) {
                return;
            }
            temp_a2 = arg0 & 0xFF;
            sendDat = arg1;
            if (temp_a2 != 0) {
                flMemcpy(&(*(u8 *)((u8 *)&sendDat + 0x1)), arg2, temp_a2);
            }
            cnLBS_Send_ChatBinary();
        }
    }
}

