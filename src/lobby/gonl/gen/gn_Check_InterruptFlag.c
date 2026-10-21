/* G online (PC): Check_InterruptFlag, written from Monster Hunter G's code at lobby 0x585120 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 D_0035EFEB;
extern u8 cw;
extern u8 pNet;
s32 Init_InterruptFlag();                       /* extern */
s32 Lb_eat_to_bell();                     /* extern */
s32 Lbc_init_network_work();                    /* extern */
s32 Lbc_set_prim(); /* extern */
s32 NPCZoomInCameraCancel();                    /* extern */
s32 To_MatchingFailed();         /* extern */
s32 To_ReadyBattle();                     /* extern */
s32 lobby_func_0055A140();       /* extern */
s32 lobby_func_0057E3D0();       /* extern */
s32 lobby_func_00582C70();               /* extern */
extern u8 lb_sys;

s32 Check_InterruptFlag(void) {
    s8 temp_v1;
    u8 temp_v1_2;
    u8 temp_v1_3;
    u8 *temp_a1;

    temp_a1 = (*(void **)&cw);
    if ((*(s8 *)((u8 *)(temp_a1) + 0xB70)) != 0) {
        if ((*(s8 *)((u8 *)(temp_a1) + 0xB74)) != 0) {
            To_ReadyBattle(temp_a1);
            return 1;
        }
        temp_v1 = (*(s8 *)((u8 *)(temp_a1) + 0xB75));
        if (temp_v1 == 1) {
            To_MatchingFailed(0, temp_a1);
            Lb_eat_to_bell((*(void **)&cw) + 0xF75D);
            return 1;
        }
        if (temp_v1 == 2) {
            To_MatchingFailed(1, temp_a1);
            return 1;
        }
        temp_v1_2 = (*(u8 *)((u8 *)(temp_a1) + 0xB99));
        if (temp_v1_2 == 2) {
            if ((*(s8 *)((u8 *)(temp_a1) + 0xB71)) == 1) {
                lobby_func_0057E3D0(1, temp_a1);
                return 1;
            }
            goto block_24;
        }
        if ((temp_v1_2 == 3) && ((*(u8 *)((u8 *)(temp_a1) + 0xFA61)) == 1)) {
            temp_v1_3 = (*(u8 *)((u8 *)(temp_a1) + 0xFA5F));
            if (temp_v1_3 == 0) {
                if ((*(s8 *)((u8 *)(temp_a1) + 0xB72)) == 1) {
                    if ((temp_v1_3 != 0) && ((*(u8 *)((u8 *)(temp_a1) + 0xF751)) != 0)) {
                        lobby_func_0055A140(1, temp_a1);
                    }
                    lobby_func_00582C70(1);
                    return 1;
                }
                goto block_24;
            }
            if ((*(s8 *)((u8 *)(temp_a1) + 0xB73)) == 1) {
                if ((*(u8 *)((u8 *)(temp_a1) + 0xF751)) != 0) {
                    lobby_func_0055A140(1, temp_a1);
                    (*(u8 *)((u8 *)((*(void **)&cw)) + 0xF751)) = 0U;
                }
                NPCZoomInCameraCancel();
                Lb_eat_to_bell((*(void **)&cw) + 0xF75D);
                (*(s8 *)&D_0035EFEB) = 0;
                lb_sys = 0x17;
                (*(u8 *)((u8 *)&lb_sys + 0x8)) = 1;
                (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0;
                (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 0;
                Lbc_init_network_work();
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

