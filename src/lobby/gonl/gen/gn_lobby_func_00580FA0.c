/* G online (PC): lobby_func_00580FA0, written from Monster Hunter G's code at lobby 0x580FA0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Check_CallBackWait();
s32 lobby_func_00580EC0();
s32 lobby_func_005C05B0();
s32 lobby_func_005C0620();
s32 lobby_func_005C0740();
extern u8 g_main_2EF390;
extern u8 cw;
extern u8 g_main_2EF390;
s32 cnLBS_Get_ServerMessage();    /* extern */

s32 lobby_func_00580FA0(void) {
    u8 g_frame[0x20] __attribute__((aligned(16)));   /* G's stack frame */
#define sp1F (*(u8 *)(g_frame + 0x1F))
    s32 var_a1;
    u8 temp_v0;
    u8 temp_v1;
    u8 temp_v1_3;
    u8 *temp_a0;
    u8 *temp_a0_2;
    u8 *temp_v1_2;

    temp_a0 = (*(void **)&cw);
    sp1F = 0;
    temp_v1 = (*(u8 *)((u8 *)(temp_a0) + 0xB9D));
    switch (temp_v1) {                              /* switch 1; irregular */
    case 0x0:                                       /* switch 1 */
        (*(u8 *)((u8 *)(temp_a0) + 0xB9D)) = (u8) (temp_v1 + 1);
        (*(u8 *)((u8 *)((*(void **)&cw)) + 0x101A)) = 0U;
        (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA68)) = 0xE10;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA65)) = 0;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0x1A;
        lobby_func_005C05B0((s32) lobby_func_00580EC0);
block_31:
    default:                                        /* switch 1 */
        return 2;
    case 0x1:                                       /* switch 1 */
        Check_CallBackWait();
        goto block_31;
    case 0x2:                                       /* switch 1 */
        lobby_func_005C0620(&sp1F, NULL);
        if (sp1F == 0) {
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 0U;
            return 0;
        }
        temp_v1_2 = (*(void **)&cw);
        (*(u8 *)((u8 *)(temp_v1_2) + 0xB9D)) = (u8) ((*(u8 *)((u8 *)(temp_v1_2) + 0xB9D)) + 1);
        (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9E)) = sp1F;
        (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9F)) = 0U;
        goto block_31;
    case 0x3:                                       /* switch 1 */
        (*(u8 *)((u8 *)(temp_a0) + 0xB9D)) = (u8) (temp_v1 + 1);
        (*(s32 *)((u8 *)((*(void **)&cw)) + 0xFA68)) = 0xE10;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA65)) = 0;
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA9)) = 0x1B;
        temp_a0_2 = (*(void **)&cw);
        temp_v1_3 = (*(u8 *)((u8 *)(temp_a0_2) + 0xB9F));
        switch (temp_v1_3) {                        /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            var_a1 = (*(s32 *)&g_main_2EF390) + 0x23F4 + 0x20 + 0x9C8;
block_24:
            (*(u8 *)((u8 *)(temp_a0_2) + 0x101A)) = (u8) ((*(u8 *)((u8 *)(temp_a0_2) + 0x101A)) + 1);
            lobby_func_005C0740((s32) (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9F)), var_a1);
            goto block_31;
        case 1:                                     /* switch 2 */
            var_a1 = (*(s32 *)&g_main_2EF390) + 0x23F4 + 0x20 + 0x9C8 + 0x500;
            goto block_24;
        case 2:                                     /* switch 2 */
            var_a1 = (*(s32 *)&g_main_2EF390) + 0x23F4 + 0x20 + 0x9C8 + 0xA00;
            goto block_24;
        default:                                    /* switch 2 */
            (*(u8 *)((u8 *)(temp_a0_2) + 0xB9F)) = 0U;
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9E)) = 0U;
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 0U;
            return 1;
        }
        break;
    case 0x4:                                       /* switch 1 */
        Check_CallBackWait();
        goto block_31;
    case 0x5:                                       /* switch 1 */
        temp_v0 = (*(u8 *)((u8 *)(temp_a0) + 0xB9F)) + 1;
        (*(u8 *)((u8 *)(temp_a0) + 0xB9F)) = temp_v0;
        if ((temp_v0 & 0xFF) >= (s32) (*(u8 *)((u8 *)(temp_a0) + 0xB9E))) {
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 0U;
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9E)) = 0U;
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9F)) = 0U;
            return 0;
        }
        (*(u8 *)((u8 *)((*(void **)&cw)) + 0xB9D)) = 3U;
        goto block_31;
    case 0x63:                                      /* switch 1 */
        (*(u8 *)((u8 *)(temp_a0) + 0xB9D)) = 0U;
        cnLBS_Get_ServerMessage((*(void **)&cw) + 0xF75D, temp_a0 + 0xB9D);
        return 1;
    }
}

