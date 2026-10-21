/* G online (PC): CallBack_Event_LobbyCommer, written from Monster Hunter G's code at lobby 0x587320 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
s32 Lb_put_chat();                 /* extern */
s32 cnLBS_Get_RoomLeaveUser();         /* extern */
s32 memcmp();               /* extern */
s32 memcpy();         /* extern */
s32 memset();        /* extern */
extern s8 lbCommer;

void CallBack_Event_LobbyCommer(void) {
    union { s8 v; u8 b[0x58]; } u_sp48;
#define sp48 (u_sp48.v)
    union { s32 v; u8 b[0x60]; } u_sp40;
#define sp40 (u_sp40.v)
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
                Lb_put_chat(2, &sp48);
            }
        } else {
            goto loop_4;
        }
    }
}

