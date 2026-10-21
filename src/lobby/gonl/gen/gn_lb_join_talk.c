/* G online (PC): lb_join_talk, written from Monster Hunter G's code at lobby 0x58B7B0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 NPC_Message();
extern u8 lb_sys;
extern s32 lb_pit;

void lb_join_talk(void) {
    u8 *temp_a1;

    temp_a1 = (*(s32 *)((u8 *)&lb_pit + 0x4)) + ((*(s8 *)((u8 *)&lb_pit + 0x8)) * 8);
    if (((*(s8 *)((u8 *)&lb_sys + 0x1C)) != 0xE) && ((*(s8 *)((u8 *)&lb_sys + 0x1C)) != 0x12) && ((*(s8 *)((u8 *)&lb_sys + 0x1C)) != 2) && ((*(s8 *)((u8 *)&lb_sys + 0x1C)) != 9) && ((*(s8 *)((u8 *)&lb_sys + 0x1C)) != 7)) {
        return;
    }
    (*(s8 *)((u8 *)&lb_pit + 0xB)) = NPC_Message((*(void **)((u8 *)(temp_a1) + 4)), lb_pit, (s32) (*(u16 *)((u8 *)(temp_a1) + 0)), (s32) (*(s8 *)((u8 *)&lb_pit + 0x9)));
}

