/* G online (PC): _sub_ReceiveJoinUser, written from Monster Hunter G's code at lobby 0x5ACAE0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 CnetSys_w;
s32 __cnet_Recv_PieceJoinUserMH(); /* extern */
extern u16 g_lb_72E07E;

void _sub_ReceiveJoinUser(s32 arg0) {
    u8 g_frame[0x30] __attribute__((aligned(16)));   /* G's stack frame */
#define sp20 (*(u16 *)(g_frame + 0x20))
#define sp22 (*(u8 *)(g_frame + 0x22))
#define sp24 (*(u8 *)(g_frame + 0x24))
#define sp26 (*(u8 *)(g_frame + 0x26))
#define sp2E (*(u16 *)(g_frame + 0x2E))
    u8 *temp_s0;

    if ((*(s8 *)((u8 *)&CnetSys_w + 0xFEC)) == 0) {
        __cnet_Recv_PieceJoinUserMH(&sp2E, 4, &sp20);
        temp_s0 = arg0 + ((sp2E - 1) * 0x164);
        (*(u16 *)((u8 *)(temp_s0) + 8)) = sp2E;
        (*(u16 *)((u8 *)(temp_s0) + 0xA)) = sp20;
        (*(u16 *)((u8 *)(temp_s0) + 0xC)) = sp22;
        (*(u16 *)((u8 *)(temp_s0) + 0xE)) = sp24;
        (*(u16 *)((u8 *)(temp_s0) + 0x10)) = sp26;
        (*(s32 *)((u8 *)(temp_s0) + 0)) = (s32) ((*(s32 *)((u8 *)(temp_s0) + 0)) | 1);
        g_lb_72E07E = sp2E;
    }
}

