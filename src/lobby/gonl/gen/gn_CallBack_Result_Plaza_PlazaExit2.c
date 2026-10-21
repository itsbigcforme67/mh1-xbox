/* G online (PC): CallBack_Result_Plaza_PlazaExit2, written from Monster Hunter G's code at lobby 0x57E790 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 To_LogOut();
extern u8 cw;
s32 cnLBS_Get_AllocationProgressCount(); /* extern */

void CallBack_Result_Plaza_PlazaExit2(s64 arg0) {
    u8 g_frame[0x20] __attribute__((aligned(16)));   /* G's stack frame */
#define sp10 (*(s64 *)(g_frame + 0x10))
#define sp1C (*(s32 *)(g_frame + 0x1C))
    u8 *temp_a0;
    u8 *temp_a2;

    temp_a2 = (*(void **)&cw);
    sp10 = arg0;
    if (((*(u8 *)((u8 *)(temp_a2) + 0xB99)) != 5) && ((*(u8 *)((u8 *)(temp_a2) + 0xBA9)) == 0x18)) {
        if ((s8) sp10 == 2) {
            if ((((s8) ((s32) (((s8 *)&sp10)[1])))) == 0xB) {
                cnLBS_Get_AllocationProgressCount(&sp1C, (((s8 *)&sp10)[1]), temp_a2, temp_a2 + 0xBA9);
                return;
            }
            return;
        }
        if ((s8) sp10 == 0) {
            (*(u8 *)((u8 *)(temp_a2) + 0xBA9)) = 0U;
            temp_a0 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_a0) + 0xB9D)) = (u8) ((*(u8 *)((u8 *)(temp_a0) + 0xB9D)) + 1);
            return;
        }
        To_LogOut(4);
    }
}

