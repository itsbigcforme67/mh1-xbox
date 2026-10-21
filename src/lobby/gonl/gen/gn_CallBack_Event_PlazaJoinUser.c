/* G online (PC): CallBack_Event_PlazaJoinUser, written from Monster Hunter G's code at lobby 0x587160 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_005E36D0();
s32 gonl_Get_PlazaJoinUser(); /* extern */
s32 cnetGet_Room_LastRecvNumber();         /* extern */
extern u8 PlazaInfo;

void CallBack_Event_PlazaJoinUser(void) {
    u8 g_frame[0x20] __attribute__((aligned(16)));   /* G's stack frame */
#define sp1E (*(u16 *)(g_frame + 0x1E))
    u8 *temp_v0;

    cnetGet_Room_LastRecvNumber(&sp1E);
    temp_v0 = &PlazaInfo + ((sp1E - 1) * 0x15C);
    gonl_Get_PlazaJoinUser(sp1E, temp_v0 + 6, temp_v0 + 0x12);
    lobby_func_005E36D0((s32) sp1E, &PlazaInfo + ((sp1E - 1) * 0x15C) + 0x14, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
}

