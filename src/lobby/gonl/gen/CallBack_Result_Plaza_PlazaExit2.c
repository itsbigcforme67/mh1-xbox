/* G online (PC): CallBack_Result_Plaza_PlazaExit2, written from Monster Hunter G's code at lobby 0x57E790 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
s32 To_LogOut();     /* extern */
s32 cnLBS_Get_AllocationProgressCount(); /* extern */

void CallBack_Result_Plaza_PlazaExit2(s64 arg0) {
    union { s32 v; u8 b[0x4]; } u_sp1C;
#define sp1C (u_sp1C.v)
    union { s64 v; u8 b[0x10]; } u_sp10;
#define sp10 (u_sp10.v)
    u8 *temp_a0;
    u8 *temp_a2;
    u8 *temp_a3;

    temp_a2 = (*(void **)&cw);
    sp10 = arg0;
    if (((*(u8 *)((u8 *)(temp_a2) + 0xB99)) != 5) && (temp_a3 = temp_a2 + 0xBA9, ((*(u8 *)((u8 *)(temp_a2) + 0xBA9)) == 0x18))) {
        if ((s8) sp10 == 2) {
            if (((s64) ((s64) (((s8 *)&sp10)[1]) << 0x38) >> 0x38) == 0xB) {
                cnLBS_Get_AllocationProgressCount(&sp1C, (((s8 *)&sp10)[1]), temp_a2, temp_a3);
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
        To_LogOut(4, (((s8 *)&sp10)[1]), temp_a2, temp_a3);
    }
}

