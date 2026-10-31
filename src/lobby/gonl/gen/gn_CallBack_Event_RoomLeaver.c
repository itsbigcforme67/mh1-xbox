/* G online (PC): CallBack_Event_RoomLeaver, written from Monster Hunter G's code at lobby 0x587A80 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"
extern u8 cw;

s32 Lb_put_chat();               /* extern */
s32 cnLBS_Get_RoomLeaveUser();         /* extern */
s32 memcmp();             /* extern */
s32 memcpy();            /* extern */
s32 memset();        /* extern */

void CallBack_Event_RoomLeaver(void) {
    u8 sp60[0x5C] __attribute__((aligned(16)));   /* HAND-FIXED: a user record, 0x5C bytes: as an s32 the memset below wiped the return address (Xbox crash, 30 Oct) */
    s32 var_s0;
    s32 var_s0_2;
    s32 var_s1;
    s32 var_s1_2;
    s32 var_s2;
    s32 var_s3;
    s32 var_s3_2;
    u8 *temp_a0;
    u8 *temp_v0;
    u8 *temp_v1;
    u8 *temp_v1_2;

    var_s3 = 4;     /* HAND-FIXED: G leaves s3 as the caller had it when the leaver is not found; 4 = nothing to move */
    memset(&sp60, 0, 0x5C);
    cnLBS_Get_RoomLeaveUser(&sp60);
    var_s1 = 0;
    var_s0 = 0;
    do {
        temp_a0 = (*(void **)&cw);
        if (((*(s8 *)((u8 *)(temp_a0) + 0xB74)) == 0) && (memcmp(temp_a0 + var_s0 + 0x21C, &sp60, 8) == 0)) {
            temp_v1 = (*(void **)&cw);
            if ((*(u8 *)((u8 *)(temp_v1) + 0xFA61)) != 0) {
                temp_v1_2 = temp_v1 + var_s0;
                if ((*(s8 *)((u8 *)(temp_v1_2) + 0x224)) != 0) {
                    Lb_put_chat(5, temp_v1_2 + 0x224);
                }
            }
            memset((*(void **)&cw) + var_s0 + 0x21C, 0, 0x5C);
            var_s3 = var_s1;
        }
        var_s1 += 1;
        var_s0 += 0x5C;
    } while (var_s1 < 4);
    var_s0_2 = var_s3;
    var_s3_2 = var_s3 + 1;
    if (var_s0_2 < 4) {
        var_s2 = var_s0_2 * 0x5C;
        var_s1_2 = var_s3_2 * 0x5C;
        do {
            if (var_s3_2 >= 4) {
                memset((*(void **)&cw) + var_s2 + 0x21C, 0, 0x5C);
            } else {
                temp_v0 = (*(void **)&cw);
                memcpy(temp_v0 + var_s2 + 0x21C, temp_v0 + var_s1_2 + 0x21C, 0x5C);
            }
            var_s0_2 += 1;
            var_s2 += 0x5C;
            var_s1_2 += 0x5C;
            var_s3_2 += 1;
        } while (var_s0_2 < 4);
    }
}

