/* G online (PC): lobby_func_005872E0, written from Monster Hunter G's code at lobby 0x5872E0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_005AC580();
s32 cnetGet_Room_LastRecvNumber();         /* extern */
extern u8 LobbyInfo;

void lobby_func_005872E0(void) {
    u8 g_frame[0x20] __attribute__((aligned(16)));   /* G's stack frame */
#define sp1E (*(u16 *)(g_frame + 0x1E))

    cnetGet_Room_LastRecvNumber(&sp1E);
    lobby_func_005AC580((s32) sp1E, &LobbyInfo + ((sp1E - 1) * 0x15C), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
}

