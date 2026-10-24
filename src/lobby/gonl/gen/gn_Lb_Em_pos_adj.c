/* G online (PC): Lb_Em_pos_adj, written from Monster Hunter G's code at lobby 0x567D40 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

void Lb_Em_pos_adj(void *arg0) {
    u16 temp_a1;
    u8 *temp_a2;

    temp_a1 = (*(u16 *)((u8 *)(arg0) + 0x472));
    temp_a2 = arg0 + 0x444;
    if (temp_a1 != 0) {
        (*(s16 *)((u8 *)(temp_a2) + 0x2E)) = (s16) (temp_a1 - 1);
        if ((s32) temp_a1 > 0) {
            (*(f32 *)((u8 *)(arg0) + 0xAC)) = (f32) ((*(f32 *)((u8 *)(arg0) + 0xAC)) + (*(f32 *)((u8 *)(temp_a2) + 0xC)));
            (*(f32 *)((u8 *)(arg0) + 0xB0)) = (f32) ((*(f32 *)((u8 *)(arg0) + 0xB0)) + (*(f32 *)((u8 *)(temp_a2) + 0x10)));
            (*(f32 *)((u8 *)(arg0) + 0xB4)) = (f32) ((*(f32 *)((u8 *)(arg0) + 0xB4)) + (*(f32 *)((u8 *)(temp_a2) + 0x14)));
        }
    }
}

