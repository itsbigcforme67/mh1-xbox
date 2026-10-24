/* G online (PC): CallBack_Event_LobbyCommer, written from Monster Hunter G's code at lobby 0x587320 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_put_chat();
s32 memcmp();
void * memcpy();
void * memset();
extern u8 cw;
s32 cnLBS_Get_RoomLeaveUser();         /* extern */
extern s8 lbCommer;

void CallBack_Event_LobbyCommer(void) {
    u8 g_frame[0xA0] __attribute__((aligned(16)));   /* G's stack frame */
#define sp40 (*(s32 *)(g_frame + 0x40))
#define sp48 (*(s8 *)(g_frame + 0x48))
    s32 var_a0;
    s32 var_s1;
    s8 *var_a1;
    s8 *var_s0;
    u8 *temp_v0;

    memset(&sp40, 0, 0x5C);
    cnLBS_Get_RoomLeaveUser(&sp40);
    temp_v0 = (*(void **)&cw);
    if (((*(s8 *)((u8 *)(temp_v0) + 3)) == 0) || (memcmp(temp_v0 + 3, &sp40, 8) < 0)) {
        memset((*(void **)&cw) + 3, 0, 8);
        memcpy((*(void **)&cw) + 3, &sp40, 8);
    }
    var_s1 = 0;
    var_s0 = &lbCommer;
loop_4:
    if (memcmp(var_s0, &sp40, 8) != 0) {
        var_s1 += 1;
        var_s0 += 0x5C;
        if (var_s1 >= 0xA) {
            var_a0 = 0;
            var_a1 = &lbCommer;
loop_7:
            if (*var_a1 == 0) {
                memcpy(&lbCommer + (var_a0 * 0x5C), &sp40, 0x5C);
            } else {
                var_a0 += 1;
                var_a1 += 0x5C;
                if (var_a0 < 0xA) {
                    goto loop_7;
                }
            }
            if (((*(u8 *)((u8 *)((*(void **)&cw)) + 0xFA61)) != 0) && (sp48 != 0)) {
                Lb_put_chat(2, (s32) &sp48, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            }
        } else {
            goto loop_4;
        }
    }
}

