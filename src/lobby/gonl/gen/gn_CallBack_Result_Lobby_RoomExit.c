/* G online (PC): CallBack_Result_Lobby_RoomExit, written from Monster Hunter G's code at lobby 0x583340 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 To_LogOut();
extern u8 cw;
void CallBack_Result_Lobby_RoomExit(s64 arg0, s32 unused_a1, s32 unused_a2, s32 arg3) {
    u8 g_frame[0x20] __attribute__((aligned(16)));   /* G's stack frame */
#define sp18 (*(s64 *)(g_frame + 0x18))
    u8 *temp_a0;
    u8 *temp_a1;

    temp_a1 = (*(void **)&cw);
    sp18 = arg0;
    if (((*(u8 *)((u8 *)(temp_a1) + 0xB99)) != 5) && ((*(u8 *)((u8 *)(temp_a1) + 0xBA9)) == 0x28)) {
        (*(u8 *)((u8 *)(temp_a1) + 0xBA9)) = 0U;
        if ((s8) sp18 == 0) {
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA5F)) = 0;
            temp_a0 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_a0) + 0xB9D)) = (u8) ((*(u8 *)((u8 *)(temp_a0) + 0xB9D)) + 1);
            return;
        }
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA5F)) = 0;
        To_LogOut(4, (s32) temp_a1, (s32) (temp_a1 + 0xBA9), arg3);
    }
}

