/* G online (PC): CallBack_Event_RoomCommer, written from Monster Hunter G's code at lobby 0x587970 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 cw;
s32 Lb_put_chat();                 /* extern */
s32 cnLBS_Get_RoomLeaveUser();              /* extern */
s32 memcpy();              /* extern */
s32 memset();        /* extern */

void CallBack_Event_RoomCommer(void) {
    union { s8 v; u8 b[0x44]; } u_sp4C;
#define sp4C (u_sp4C.v)
    union { s8 v; u8 b[0x58]; } u_sp38;
#define sp38 (u_sp38.v)
    union { s8 v; u8 b[0x60]; } u_sp30;
#define sp30 (u_sp30.v)
    s32 temp_s0;
    s32 var_a3;
    u8 *temp_a0;
    u8 *var_a1;

    memset(&sp30, 0, 0x5C);
    cnLBS_Get_RoomLeaveUser(&sp30);
    temp_a0 = (*(void **)&cw);
    var_a3 = 0;
    var_a1 = temp_a0;
loop_1:
    if ((*(s8 *)((u8 *)(var_a1) + 0x21C)) == 0) {
        temp_s0 = var_a3 * 0x5C;
        memset(temp_a0 + temp_s0 + 0x21C, 0, 0x5C, var_a3);
        memcpy((*(void **)&cw) + temp_s0 + 0x21C, &sp30, 8);
        memcpy((*(void **)&cw) + temp_s0 + 0x224, &sp38, 0x10);
        memcpy((*(void **)&cw) + temp_s0 + 0x238, &sp4C, 0x40);
    } else {
        var_a3 += 1;
        var_a1 += 0x5C;
        if (var_a3 < 4) {
            goto loop_1;
        }
    }
    if (((*(u8 *)((u8 *)((*(void **)&cw)) + 0xFA61)) != 0) && (sp38 != 0)) {
        Lb_put_chat(4, &sp38);
    }
}

