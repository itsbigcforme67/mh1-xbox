/* G online (PC): getHandleFromID, written from Monster Hunter G's code at lobby 0x57E840 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Fade_busy_ck();
s32 Lbc_init_network_work();
s32 fade_set();
s32 lobby_func_005690D0();
s32 lobby_func_00580FA0();
s32 lobby_func_00581210();
extern u8 D_0035EFEB;
extern u8 cw;
extern u8 pNet;
extern s32 network_work;

s32 getHandleFromID(s32 unused_a0, s32 unused_a1, s32 arg2, s32 arg3) {
    s32 temp_v0;
    s32 temp_v0_2;
    u8 temp_a0;
    u8 *temp_a1;
    u8 *temp_v1;
    u8 *temp_v1_2;
    u8 *temp_v1_3;

    temp_a1 = (*(void **)&cw);
    temp_a0 = (*(u8 *)((u8 *)(temp_a1) + 0xB9C));
    switch (temp_a0) {                              /* irregular */
    case 0:
        (*(s8 *)((u8 *)(temp_a1) + 0xB70)) = 1;
        (*(s8 *)&D_0035EFEB) = 1;
        if (!(Fade_busy_ck() & 0xFF)) {
            temp_v1 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_v1) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_v1) + 0xB9C)) + 1);
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 0;
            Lbc_init_network_work(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            fade_set(0xC);
        }
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
block_21:
    default:
        return 2;
    case 1:
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        temp_v0 = lobby_func_00581210((s32) temp_a0, (s32) temp_a1, arg2, arg3);
        if ((temp_v0 != 0) && (temp_v0 != 1)) {

        } else {
            temp_v1_2 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_v1_2) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_v1_2) + 0xB9C)) + 1);
        }
        goto block_21;
    case 2:
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        temp_v0_2 = lobby_func_00580FA0((s32) temp_a0, (s32) temp_a1, arg2, arg3);
        if ((temp_v0_2 != 0) && (temp_v0_2 != 1)) {

        } else {
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB70)) = 1;
            temp_v1_3 = (*(void **)&cw);
            (*(u8 *)((u8 *)(temp_v1_3) + 0xB9C)) = (u8) ((*(u8 *)((u8 *)(temp_v1_3) + 0xB9C)) + 1);
        }
        goto block_21;
    case 3:
        return lobby_func_005690D0(&network_work, (s32) temp_a1, arg2, arg3);
    }
}

