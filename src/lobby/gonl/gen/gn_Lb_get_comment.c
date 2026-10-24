/* G online (PC): Lb_get_comment, written from Monster Hunter G's code at lobby 0x56C2B0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

void * memset();
extern u8 cw;
s32 Lb_get_plID();                                  /* extern */
s32 Lbc_RequestNetComment();                 /* extern */

s32 Lb_get_comment(s32 arg0) {
    s32 temp_a1;
    u8 *temp_v0;

    temp_a1 = Lb_get_plID(arg0) & 0xFF;
    if (temp_a1 != 0xFF) {
        temp_v0 = temp_a1 + (*(s32 *)&cw);
        if ((*(u8 *)((u8 *)(temp_v0) + 0xB5A)) == 0) {
            (*(u8 *)((u8 *)(temp_v0) + 0xB5A)) = 0xFFU;
            memset((*(s32 *)&cw) + (temp_a1 * 0x62) + 0x724, 0, 0x62);
            Lbc_RequestNetComment(arg0);
            return 1;
        }
    }
    return 0;
}

