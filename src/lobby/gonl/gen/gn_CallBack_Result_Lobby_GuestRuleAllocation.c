/* G online (PC): CallBack_Result_Lobby_GuestRuleAllocation, written from Monster Hunter G's code at lobby 0x5826F0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_eat_to_bell();
s32 cnLBS_Get_ServerMessage();
extern u8 D_0035EFEB;
extern u8 cw;
extern s32 lb_sys;

void CallBack_Result_Lobby_GuestRuleAllocation(s64 arg0) {
    u8 g_frame[0x20] __attribute__((aligned(16)));   /* G's stack frame */
#define sp18 (*(s64 *)(g_frame + 0x18))
    u8 *temp_a0;
    u8 *temp_a1;

    temp_a1 = (*(void **)&cw);
    sp18 = arg0;
    if (((*(u8 *)((u8 *)(temp_a1) + 0xB99)) != 5) && ((*(u8 *)((u8 *)(temp_a1) + 0xBA9)) == 0x23) && ((s8) sp18 != 2)) {
        (*(u8 *)((u8 *)(temp_a1) + 0xBA9)) = 0U;
        switch ((s8) sp18) {                        /* irregular */
        case 0:
            temp_a0 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_a0) + 0xB9D)) = (u8) ((*(u8 *)((u8 *)(temp_a0) + 0xB9D)) + 1);
            return;
        case -1:
            cnLBS_Get_ServerMessage((*(void **)&cw) + 0xF75D);
            Lb_eat_to_bell((s32) ((*(void **)&cw) + 0xF75D), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            (*(s8 *)&D_0035EFEB) = 0;
            (*(s32 *)((u8 *)&lb_sys + 0x8)) = 1;
            lb_sys = 0x19;
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 0U;
            (*(s8 *)((u8 *)&lb_sys + 0x1C)) = 0;
            break;
        }
    }
}

