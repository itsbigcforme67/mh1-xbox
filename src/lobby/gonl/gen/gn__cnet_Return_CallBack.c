/* G online (PC): _cnet_Return_CallBack, written from Monster Hunter G's code at lobby 0x5E13A0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 __cnetSub_Return_BgProcess();
s32 __cnet_Recv_ServerMessage();
s32 _cnetEvent_JumpCallBack();
extern u8 CnetSys_w;

void _cnet_Return_CallBack(s32 arg0) {
    u8 g_frame[0x20] __attribute__((aligned(16)));   /* G's stack frame */
#define sp18 (*(s8 *)(g_frame + 0x18))

    switch ((*(u16 *)((u8 *)&CnetSys_w + 0xFE6))) {                           /* irregular */
    case 1:
        /* fallthrough */
    case 16:
        if (arg0 != 0) {
            _cnetEvent_JumpCallBack(arg0);
        }
        return;
    case 2:
        if ((*(s8 *)((u8 *)&CnetSys_w + 0xFEC)) == 0) {
            sp18 = 0;
        } else {
            sp18 = -1;
            __cnet_Recv_ServerMessage();
        }
        __cnetSub_Return_BgProcess((s32) *(s64 *)&sp18, 1, 0);
        return;
    }
}

