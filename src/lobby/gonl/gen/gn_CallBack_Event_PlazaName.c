/* G online (PC): CallBack_Event_PlazaName, written from Monster Hunter G's code at lobby 0x587120 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_005E3480();
s32 cnetGet_Room_LastRecvNumber();         /* extern */
extern s32 PlazaInfo;

void CallBack_Event_PlazaName(void) {
    u8 g_frame[0x20] __attribute__((aligned(16)));   /* G's stack frame */
#define sp1E (*(u16 *)(g_frame + 0x1E))

    cnetGet_Room_LastRecvNumber(&sp1E);
    lobby_func_005E3480((s32) sp1E, (s32) ((u8 *)&PlazaInfo + ((sp1E - 1) * 0x15C) + 0x1A), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
}

