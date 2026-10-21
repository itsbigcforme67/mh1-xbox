/* G online (PC): lobby_func_0057B800, written from Monster Hunter G's code at lobby 0x57B800 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Check_CallBackWait();
s32 lobby_func_0057B770();
s32 lobby_func_005C08B0();
s32 lobby_func_005C09F0();
s32 sub_main_func_004C8F50();
void * sub_main_func_004C8F90();
extern u8 CnetWork;
extern u8 cw;

s32 lobby_func_0057B800(void) {
    s32 temp_s0;
    s32 var_s0;
    s32 var_s1;
    u8 temp_a0;
    u8 temp_a0_2;
    u8 *temp_a1;
    u8 *temp_a2;

    temp_a1 = (*(void **)&cw);
    temp_a0 = (*(u8 *)((u8 *)(temp_a1) + 0xB9D));
    switch (temp_a0) {                              /* switch 1; irregular */
    case 0x0:                                       /* switch 1 */
        (*(u8 *)((u8 *)(temp_a1) + 0xB9D)) = (u8) (temp_a0 + 1);
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 5;
        (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA68)) = 0xE10;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA65)) = 0;
        lobby_func_005C08B0((s32) (*(u8 *)((u8 *)&CnetWork + 0x5)), (s32) lobby_func_0057B770);
block_26:
        return 2;
    case 0x1:                                       /* switch 1 */
        Check_CallBackWait();
        goto block_26;
    case 0x2:                                       /* switch 1 */
        temp_a0_2 = (*(u8 *)((u8 *)(temp_a1) + 0xB9E));
        switch (temp_a0_2) {                        /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            (*(u8 *)((u8 *)(temp_a1) + 0xB9D)) = 0U;
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9E)) = 0U;
            return 0;
        case 3:                                     /* switch 2 */
        case 2:                                     /* switch 2 */
        case 1:                                     /* switch 2 */
            var_s0 = temp_a0_2 - 1;
            var_s1 = 2;
block_21:
            temp_s0 = sub_main_func_004C8F50(var_s1);
            temp_a2 = sub_main_func_004C8F90(var_s1, var_s0);
            (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA68)) = 0xE10;
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA65)) = 0;
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 6;
            lobby_func_005C09F0((s32) (*(u8 *)((u8 *)&CnetWork + 0x5)), (s32) (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9E)), temp_a2, temp_s0 & 0xFFFF, (s32) lobby_func_0057B770);
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 1U;
            goto block_26;
        case 4:                                     /* switch 2 */
            var_s1 = 1;
            var_s0 = 0;
            goto block_21;
        default:                                    /* switch 2 */
            (*(u8 *)((u8 *)(temp_a1) + 0xB9D)) = 0U;
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9E)) = 0U;
            return 1;
        }
        break;
    case 0x63:                                      /* switch 1 */
        (*(u8 *)((u8 *)(temp_a1) + 0xB9D)) = 0U;
        (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9E)) = 0U;
        return 1;
    default:                                        /* switch 1 */
        (*(u8 *)((u8 *)(temp_a1) + 0xB9D)) = 0U;
        (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9E)) = 0U;
        return 1;
    }
}

