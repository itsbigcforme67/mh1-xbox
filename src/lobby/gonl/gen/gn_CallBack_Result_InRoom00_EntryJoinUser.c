/* G online (PC): CallBack_Result_InRoom00_EntryJoinUser, written from Monster Hunter G's code at lobby 0x583590 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 ClassInfo;
extern u8 cw;
s32 cnLBS_Get_MatchEntryJoinUser(); /* extern */

void CallBack_Result_InRoom00_EntryJoinUser(s64 arg0) {
    u8 g_frame[0x20] __attribute__((aligned(16)));   /* G's stack frame */
#define sp18 (*(s64 *)(g_frame + 0x18))
    u8 *temp_v0;

    sp18 = arg0;
    if ((s8) sp18 == 0) {
        temp_v0 = (*(void **)&cw);
        cnLBS_Get_MatchEntryJoinUser((*(u8 *)((u8 *)&ClassInfo + 0x8)), temp_v0 + 0xF752, temp_v0 + 0xF754);
        return;
    }
    (*(s16 *)((u8 *)((*(void **)&cw)) + 0xF752)) = 0;
    (*(s16 *)((u8 *)((*(void **)&cw)) + 0xF754)) = 0;
}

