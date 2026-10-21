/* G online (PC): lbc_login_error, written from Monster Hunter G's code at lobby 0x57B560 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Fade_busy_ck();
s32 Get_sw2();
s32 Lb_eat_to_bell();
s32 Lbc_init_network_work();
s32 Lbc_set_prim();
s32 To_LogOut();
s32 cnWrap_SoundRequest();
s32 fade_set();
s32 text_lobby_trans_ot0();
extern u8 cw;
extern u8 pNet;
void lbc_login_error(void) {
    s32 temp_a2;
    s32 temp_v1_2;
    u8 temp_a1;
    u8 *temp_a1_2;
    u8 *temp_a1_3;
    u8 *temp_v1;

    temp_a1 = (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9C));
    temp_a2 = Get_sw2(0) & 0xFFFF;
    switch (temp_a1) {                              /* irregular */
    case 0:
        Lbc_init_network_work(1, (s32) temp_a1, temp_a2, M2C_ERROR(/* Read from unset register $a3 */));
        Lbc_set_prim((s32) text_lobby_trans_ot0, 0, 0, 0);
        temp_v1 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_v1) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_v1) + 0xB9C)) + 1);
        fade_set(2);
        (*(s32 *)((u8 *)((*(void **)&cw)) + 0xBB4)) = 0x258;
        Lb_eat_to_bell((s32) ((*(void **)&cw) + 0xF75D), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        return;
    case 1:
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        temp_a1_2 = (*(void **)&cw);
        (*(s32 *)((u8 *)(temp_a1_2) + 0xBB4)) = (s32) ((*(s32 *)((u8 *)(temp_a1_2) + 0xBB4)) - 1);
        temp_a1_3 = (*(void **)&cw);
        temp_v1_2 = (*(s32 *)((u8 *)(temp_a1_3) + 0xBB4));
        if (temp_v1_2 > 0) {
            if ((temp_v1_2 < 0x1E0) && (temp_a2 & 0xFFFF & 0x20)) {
                (*(u8 *)((u8 *)(temp_a1_3) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_a1_3) + 0xB9C)) + 1);
                cnWrap_SoundRequest(0, (s32) temp_a1_3, temp_a2, M2C_ERROR(/* Read from unset register $a3 */));
                fade_set(1);
                return;
            }
            return;
        }
        (*(u8 *)((u8 *)(temp_a1_3) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_a1_3) + 0xB9C)) + 1);
        fade_set(1);
        return;
    case 2:
        if ((Fade_busy_ck() & 0xFF) != 1) {
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x31)) = 1;
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9B)) = 4;
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9C)) = 1U;
            return;
        }
        break;
    default:
        To_LogOut(4, (s32) temp_a1, temp_a2, M2C_ERROR(/* Read from unset register $a3 */));
        break;
    }
}

