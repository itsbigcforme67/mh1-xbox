/* G online (PC): Lb_send_data_to_myself, written from Monster Hunter G's code at lobby 0x59B2E0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_check_receipt();
s32 Online_ck();
s32 flMemcpy();
extern u8 my_user_id;
extern s8 sendDat;

void Lb_send_data_to_myself(s32 arg0, s8 arg1, void *arg2) {
    if (Online_ck() != 0) {
        sendDat = arg1;
        flMemcpy(&(*(u8 *)((u8 *)&sendDat + 0x1)), arg2, arg0 & 0xFF);
        Lb_check_receipt((s32) &my_user_id, &sendDat, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    }
}

