/* G online (PC): lb_set_chair, written from Monster Hunter G's code at lobby 0x5598F0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_pl_to_chair();
extern u8 game_w;
s32 Lb_get_plID();                        /* extern */
s32 Lb_send_chair_release();              /* extern */
extern s32 lb_sys;
extern s32 player_work;

void lb_set_chair(s32 unused_a0, void *arg1) {
    u8 temp_a2;
    u8 temp_s0;
    u8 temp_s0_2;

    if ((*(u8 *)((u8 *)(arg1) + 0)) == 0) {
        temp_s0 = (*(u8 *)((u8 *)&game_w + 0x3A));
        if ((temp_s0 == (Lb_get_plID(arg1 + 2) & 0xFF)) && (lb_sys == 0x1A)) {
            lb_sys = 0;
            (*(s32 *)((u8 *)&lb_sys + 0x8)) = 0;
        }
        return;
    }
    temp_a2 = (*(u8 *)((u8 *)(arg1) + 1));
    temp_s0_2 = (*(u8 *)((u8 *)&game_w + 0x3A));
    (*(u16 *)((u8 *)&lb_sys + 0x14)) |= 1 << temp_a2;
    if (temp_s0_2 == (Lb_get_plID(arg1 + 2, temp_a2) & 0xFF)) {
        if (((*(u8 *)((u8 *)&game_w + 0x14)) == 0x4D) && (lb_sys == 0x1A)) {
            Lb_pl_to_chair(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            return;
        }
        Lb_send_chair_release((u8 *)&player_work + ((temp_s0_2 & 0xFF) * 0xA00));
    }
}

