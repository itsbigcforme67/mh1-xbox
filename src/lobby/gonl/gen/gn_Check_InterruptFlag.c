/* G online (PC): Check_InterruptFlag, written from Monster Hunter G's code at lobby 0x585120 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_eat_to_bell();
s32 Lbc_init_network_work();
s32 Lbc_set_prim();
s32 NPCZoomInCameraCancel();
s32 To_ReadyBattle();
s32 lobby_func_0055A140();
s32 lobby_func_0057E3D0();
s32 lobby_func_00582C70();
extern u8 D_0035EFEB;
extern u8 cw;
extern u8 pNet;
s32 Init_InterruptFlag();                       /* extern */
s32 To_MatchingFailed();         /* extern */
extern s32 lb_sys;

s32 Check_InterruptFlag(s32 arg0, s32 unused_a1, s32 arg2, s32 arg3) {
    s8 temp_v1;
    u8 temp_v1_2;
    u8 temp_v1_3;
    u8 *temp_a1;

    temp_a1 = (*(void **)&cw);
    if ((*(s8 *)((u8 *)(temp_a1) + 0xB70)) != 0) {
        if ((*(s8 *)((u8 *)(temp_a1) + 0xB74)) != 0) {
            To_ReadyBattle(arg0, (s32) temp_a1, arg2, arg3);
            return 1;
        }
        temp_v1 = (*(s8 *)((u8 *)(temp_a1) + 0xB75));
        if (temp_v1 == 1) {
            To_MatchingFailed(0, temp_a1);
            Lb_eat_to_bell((s32) ((*(void **)&cw) + 0xF75D), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            return 1;
        }
        if (temp_v1 == 2) {
            To_MatchingFailed(1, temp_a1);
            return 1;
        }
        temp_v1_2 = (*(u8 *)((u8 *)(temp_a1) + 0xB99));
        if (temp_v1_2 == 2) {
            if ((*(s8 *)((u8 *)(temp_a1) + 0xB71)) == 1) {
                lobby_func_0057E3D0(1, (s32) temp_a1, arg2, arg3);
                return 1;
            }
            goto block_24;
        }
        if ((temp_v1_2 == 3) && ((*(u8 *)((u8 *)(temp_a1) + 0xFA61)) == 1)) {
            temp_v1_3 = (*(u8 *)((u8 *)(temp_a1) + 0xFA5F));
            if (temp_v1_3 == 0) {
                if ((*(s8 *)((u8 *)(temp_a1) + 0xB72)) == 1) {
                    if ((temp_v1_3 != 0) && ((*(u8 *)((u8 *)(temp_a1) + 0xF751)) != 0)) {
                        lobby_func_0055A140(1, (s32) temp_a1, arg2, arg3);
                    }
                    lobby_func_00582C70(1, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                    return 1;
                }
                goto block_24;
            }
            if ((*(s8 *)((u8 *)(temp_a1) + 0xB73)) == 1) {
                if ((*(u8 *)((u8 *)(temp_a1) + 0xF751)) != 0) {
                    lobby_func_0055A140(1, (s32) temp_a1, arg2, arg3);
                    (*(u8 *)((u8 *)((*(void **)&cw)) + 0xF751)) = 0U;
                }
                NPCZoomInCameraCancel();
                Lb_eat_to_bell((s32) ((*(void **)&cw) + 0xF75D), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                (*(s8 *)&D_0035EFEB) = 0;
                lb_sys = 0x17;
                (*(s32 *)((u8 *)&lb_sys + 0x8)) = 1;
                (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0;
                (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 0;
                Lbc_init_network_work(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                Lbc_set_prim(0, 0, 0, 0);
                Init_InterruptFlag();
                (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
                return 1;
            }
            goto block_24;
        }
        goto block_24;
    }
block_24:
    return 0;
}

