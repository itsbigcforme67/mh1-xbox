/* G online (PC): CallBack_Event_RoomPasswordInfo, written from Monster Hunter G's code at lobby 0x5877D0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 cnLBS_Get_RoomPasswordInfo();    /* extern */
s32 cnetGet_Room_LastRecvNumber();         /* extern */
extern s32 g_lb_6DE120;

void CallBack_Event_RoomPasswordInfo(void) {
    u8 g_frame[0x20] __attribute__((aligned(16)));   /* G's stack frame */
#define sp1E (*(u16 *)(g_frame + 0x1E))

    cnetGet_Room_LastRecvNumber(&sp1E);
    cnLBS_Get_RoomPasswordInfo(sp1E, (u8 *)&g_lb_6DE120 + ((sp1E - 1) * 0x15C) + 0x17);
}

