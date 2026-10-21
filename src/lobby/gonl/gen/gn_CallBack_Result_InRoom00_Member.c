/* G online (PC): CallBack_Result_InRoom00_Member, written from Monster Hunter G's code at lobby 0x583460 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 To_LogOut();
void * memset();
extern u8 cw;
s32 cnLBS_Get_RoomMemberList(); /* extern */

void CallBack_Result_InRoom00_Member(s64 arg0) {
    u8 g_frame[0x40] __attribute__((aligned(16)));   /* G's stack frame */
#define sp38 (*(s64 *)(g_frame + 0x38))
    s32 var_s0;
    s32 var_s1;
    u8 *temp_a1;
    u8 *temp_v0;

    temp_a1 = (*(void **)&cw);
    sp38 = arg0;
    if ((*(u8 *)((u8 *)(temp_a1) + 0xB99)) != 5) {
        memset(temp_a1 + 0x21C, 0, 0x170);
        var_s1 = 0;
        if ((s8) sp38 == 0) {
            var_s0 = 0;
            do {
                temp_v0 = (*(void **)&cw) + var_s0;
                cnLBS_Get_RoomMemberList(var_s1 & 0xFFFF, temp_v0 + 0x21C, temp_v0 + 0x224, temp_v0 + 0x238);
                var_s1 += 1;
                var_s0 += 0x5C;
            } while (var_s1 < 4);
            return;
        }
        To_LogOut(4, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    }
}

