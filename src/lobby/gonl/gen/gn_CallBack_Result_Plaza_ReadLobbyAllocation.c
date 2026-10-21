/* G online (PC): CallBack_Result_Plaza_ReadLobbyAllocation, written from Monster Hunter G's code at lobby 0x57FDE0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
extern u8 lb_quest_all;
s32 To_LogOut();         /* extern */
s32 cnLBS_Get_LobbyMemberList(); /* extern */
s32 lobby_func_0058BAA0();                /* extern */

void CallBack_Result_Plaza_ReadLobbyAllocation(s64 arg0) {
    u8 g_frame[0x40] __attribute__((aligned(16)));   /* G's stack frame */
#define sp38 (*(s64 *)(g_frame + 0x38))
    u8 *var_s0;   /* (s32 *) */
    s32 var_s1;
    u8 *temp_a0;
    u8 *temp_a1;

    temp_a1 = (*(void **)&cw);
    sp38 = arg0;
    if (((*(u8 *)((u8 *)(temp_a1) + 0xB99)) != 5) && ((*(u8 *)((u8 *)(temp_a1) + 0xBA9)) == 0xE)) {
        (*(u8 *)((u8 *)(temp_a1) + 0xBA9)) = 0U;
        if ((s8) sp38 == 0) {
            var_s1 = 0;
            var_s0 = &(*(u8 *)((u8 *)&lb_quest_all + 0xA0));
            do {
                cnLBS_Get_LobbyMemberList(var_s1 & 0xFFFF, var_s0, var_s0 + 8, var_s0 + 0x1C);
                lobby_func_0058BAA0(var_s0 + 0x1C);
                var_s1 += 1;
                var_s0 += 0x5C;
            } while (var_s1 < 0xA);
        } else {
            To_LogOut(4, temp_a1, temp_a1 + 0xBA9);
        }
        temp_a0 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_a0) + 0xB9D)) = (u8) ((*(u8 *)((u8 *)(temp_a0) + 0xB9D)) + 1);
    }
}

