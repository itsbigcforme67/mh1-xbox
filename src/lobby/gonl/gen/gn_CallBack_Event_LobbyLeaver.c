/* G online (PC): CallBack_Event_LobbyLeaver, written from Monster Hunter G's code at lobby 0x587480 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_put_chat();
s32 lobby_func_00595530();
s32 memcmp();
void * memcpy();
void * memset();
extern u8 cw;
extern u8 game_w;
extern u8 lb_sys;
s32 Lb_send_myChair();                          /* extern */
s32 cnLBS_Get_RoomLeaveUser();         /* extern */
extern u8 lbCommer;
extern u8 player_work;

void CallBack_Event_LobbyLeaver(void) {
    u8 g_frame[0xA0] __attribute__((aligned(16)));   /* G's stack frame */
#define sp40 (*(s32 *)(g_frame + 0x40))
    u8 *var_s1;   /* (s32 *) */
    s32 temp_s1;
    s32 var_s0;
    s32 var_s0_2;
    s32 var_s1_2;
    u8 *temp_a1;
    u8 *temp_a2;
    u8 *temp_v1;
    u8 *temp_v1_2;
    u8 *temp_v1_3;
    u8 *temp_v1_4;

    cnLBS_Get_RoomLeaveUser(&sp40);
    var_s0 = 0;
    var_s1 = &lbCommer;
loop_1:
    if (memcmp(var_s1, &sp40, 8) == 0) {
        temp_s1 = var_s0 * 0x5C;
        memset(&lbCommer + temp_s1, 0, 0x5C);
        temp_v1 = (*(void **)&cw);
        if ((*(u8 *)((u8 *)(temp_v1) + 0xFA61)) != 0) {
            temp_v1_2 = temp_v1 + temp_s1;
            if ((temp_v1_2 + 0x38C) != NULL) {
                Lb_put_chat(8, (s32) (temp_v1_2 + 0x394), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            }
        }
        temp_v1_3 = &player_work + (var_s0 * 0xA00);
        (*(s32 *)((u8 *)(temp_v1_3) + 0xB4)) = 0;
        (*(s32 *)((u8 *)(temp_v1_3) + 0xB0)) = 0;
        (*(s32 *)((u8 *)(temp_v1_3) + 0xAC)) = 0;
        (*(s32 *)((u8 *)(temp_v1_3) + 0x808)) = 0;
        (*(s32 *)((u8 *)(temp_v1_3) + 0x804)) = 0;
        (*(s32 *)((u8 *)(temp_v1_3) + 0x800)) = 0;
        (*(s16 *)((u8 *)(temp_v1_3) + 0xE)) = 0;
        (*(s32 *)((u8 *)(temp_v1_3) + 0xA4)) = (s32) (0 & 0xFFFF);
        (*(s16 *)((u8 *)&lb_sys + 0x14)) = 0;
        if ((lobby_func_00595530((s32) (*(u8 *)((u8 *)(temp_v1_3) + 0x14)), (s32) (*(u8 *)((u8 *)(temp_v1_3) + 0x15)), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */)) == 1) && ((*(u8 *)((u8 *)&game_w + 0x14)) == 0x4D)) {
            Lb_send_myChair();
        }
    } else {
        var_s0 += 1;
        var_s1 += 0x5C;
        if (var_s0 < 0xA) {
            goto loop_1;
        }
    }
    if (memcmp((*(void **)&cw) + 3, &sp40, 8) == 0) {
        var_s0_2 = 0;
        var_s1_2 = 0;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 3)) = 0;
        do {
            temp_a1 = (*(void **)&cw);
            temp_a2 = temp_a1 + var_s1_2;
            if (((*(s8 *)((u8 *)(temp_a2) + 0x38C)) != 0) && (((*(s8 *)((u8 *)(temp_a1) + 3)) == 0) || (memcmp(temp_a1 + 3, temp_a2 + 0x38C, 8) < 0))) {
                memset((*(void **)&cw) + 3, 0, 8);
                temp_v1_4 = (*(void **)&cw);
                memcpy(temp_v1_4 + 3, temp_v1_4 + var_s1_2 + 0x38C, 8);
            }
            var_s0_2 += 1;
            var_s1_2 += 0x5C;
        } while (var_s0_2 < 0xA);
    }
}

