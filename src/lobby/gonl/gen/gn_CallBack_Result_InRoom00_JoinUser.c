/* G online (PC): CallBack_Result_InRoom00_JoinUser, written from Monster Hunter G's code at lobby 0x583500 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 ClassInfo;
extern u8 cw;
s32 cnLBS_Get_mhLobbyJoinUser(); /* extern */
extern u16 g_lb_6DDFCA;
extern s32 g_lb_6DE120;

void CallBack_Result_InRoom00_JoinUser(s64 arg0) {
    u8 g_frame[0x20] __attribute__((aligned(16)));   /* G's stack frame */
#define sp18 (*(s64 *)(g_frame + 0x18))
    u8 *temp_v0;

    sp18 = arg0;
    if ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xB99)) != 5) {
        if ((s8) sp18 == 0) {
            temp_v0 = (u8 *)&g_lb_6DE120 + (((*(u8 *)((u8 *)&ClassInfo + 0x8)) - 1) * 0x15C);
            cnLBS_Get_mhLobbyJoinUser((*(u8 *)((u8 *)&ClassInfo + 0x8)) & 0xFFFF, temp_v0 + 6, temp_v0 + 0x12);
            return;
        }
        (*(u16 *)((u8 *)&g_lb_6DDFCA + ((*(u8 *)((u8 *)&ClassInfo + 0x8)) * 0x15C))) = 0;
    }
}

