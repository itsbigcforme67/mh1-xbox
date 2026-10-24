/* G online (PC): lb_check_chair, written from Monster Hunter G's code at lobby 0x559850 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_send_chair_status();
s32 memcmp();
extern u8 cw;
extern u8 lb_sys;

void lb_check_chair(void *arg0, u8 *arg1) {
    s32 temp_a0;
    s32 temp_v0;
    u8 temp_a2;

    temp_v0 = (*(s32 *)&cw);
    if (memcmp(temp_v0 + 0x1C0, temp_v0 + 3, 8) == 0) {
        temp_a2 = *arg1;
        temp_a0 = 1 << temp_a2;
        if ((*(u16 *)((u8 *)&lb_sys + 0x14)) & temp_a0) {
            Lb_send_chair_status(arg0, 0, (s32) temp_a2, M2C_ERROR(/* Read from unset register $a3 */));
            return;
        }
        (*(u16 *)((u8 *)&lb_sys + 0x14)) |= temp_a0;
        Lb_send_chair_status(arg0, 1, (s32) *arg1, M2C_ERROR(/* Read from unset register $a3 */));
    }
}

