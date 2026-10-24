/* G online (PC): Lb_send_pl_pos, written from Monster Hunter G's code at lobby 0x59A660 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lb_send_data();
extern u8 lbSendInterval;
extern u8 game_w;
extern u8 lb_sys;
s64 lobby_func_0059A590();            /* extern */
extern f32 lastSend;
extern u8 player_work;

void Lb_send_pl_pos(void) {
    u8 g_frame[0x50] __attribute__((aligned(16)));   /* G's stack frame */
#define sp48 (*(s16 *)(g_frame + 0x48))
#define sp4A (*(s16 *)(g_frame + 0x4A))
#define sp4C (*(u8 *)(g_frame + 0x4C))
    s32 temp_a1;
    s8 temp_v1;
    u8 temp_a2;
    u8 *temp_s2;

    temp_a2 = (*(u8 *)((u8 *)&game_w + 0x3A));
    temp_a1 = temp_a2 * 0xA00;
    temp_v1 = (*(s8 *)((u8 *)&lb_sys + 0x85)) - 1;
    (*(s8 *)((u8 *)&lb_sys + 0x85)) = temp_v1;
    temp_s2 = &player_work + temp_a1;
    if (((((s8) ((s32) temp_v1))) <= 0) && ((*(s8 *)((u8 *)&lb_sys + 0x19)) == 4) && ((sp48 = (s16) (s32) (*(f32 *)((u8 *)(temp_s2) + 0xAC)), sp4A = (s16) (s32) (*(f32 *)((u8 *)(temp_s2) + 0xB4)), sp4C = (*(u8 *)((u8 *)(temp_s2) + 0x736)), ((((s8) (lobby_func_0059A590(&sp48, temp_a1, temp_a2)))) == 0)) || ((*(u8 *)((u8 *)(temp_s2) + 0x15)) == 0x2C))) {
        (*(s8 *)((u8 *)&lb_sys + 0x86)) = 0;
        (*(s8 *)((u8 *)&lb_sys + 0x85)) = (s8) (*(u16 *)&lbSendInterval);
        lastSend = (f32) sp48;
        (*(f32 *)((u8 *)&lastSend + 0x8)) = (f32) sp4A;
        (*(s16 *)((u8 *)&lastSend + 0x12)) = (s16) sp4C;
        lb_send_data(5, 0, &sp48, M2C_ERROR(/* Read from unset register $a3 */));
    }
}

