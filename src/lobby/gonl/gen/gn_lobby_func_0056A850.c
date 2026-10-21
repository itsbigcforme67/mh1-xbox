/* G online (PC): lobby_func_0056A850, written from Monster Hunter G's code at lobby 0x56A850 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 CallBack_Result_Plaza_ReadAllocation2();
s32 Get_sw2();
s32 Lb_eat_to_bell();
s32 Lbs_LobbyExit();
s32 SetDialogData();
s32 cnWrap_SoundRequest();
s64 lobby_func_00569390();
extern u8 g_main_2EF390;
extern u8 cw;
extern u8 g_main_2EF390;
extern u8 ClassInfo;
extern s8 g_lb_5FBC10;
extern s32 g_main_3064AF;

s32 lobby_func_0056A850(void *arg0) {
    u8 g_frame[0x30] __attribute__((aligned(16)));   /* G's stack frame */
#define sp2C (*(s16 *)(g_frame + 0x2C))
#define sp2F (*(u8 *)(g_frame + 0x2F))
    s16 temp_v1_3;
    s16 temp_v1_4;
    s32 temp_a4;
    s32 temp_v0_2;
    s32 temp_v1_2;
    s32 var_a1;
    s8 *var_a0;
    s8 temp_a3;
    s8 temp_v1;
    u16 temp_v0;

    temp_v1 = (*(s8 *)((u8 *)(arg0) + 0xB));
    temp_a4 = Get_sw2(0) & 0xFFFF;
    switch (temp_v1) {                              /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        temp_v0 = (*(u16 *)((u8 *)(arg0) + 4));
        if (temp_v0 == 0) {
            SetDialogData(0x66);
            (*(s8 *)((u8 *)(arg0) + 0xB)) = 1;
            cnWrap_SoundRequest(3);
            goto block_41;
        }
        (*(u16 *)((u8 *)(arg0) + 4)) = (u16) (temp_v0 - 1);
        temp_v1_2 = temp_a4 & 0xFFFF;
        if (temp_v1_2 & 0x20) {
            temp_v1_3 = (*(s16 *)((u8 *)(arg0) + 2));
            if (temp_v1_3 == 7) {
                SetDialogData(0x66);
                (*(s8 *)((u8 *)(arg0) + 0xB)) = 1;
                cnWrap_SoundRequest(3);
                goto block_41;
            }
            if (temp_v1_3 == 0xF) {
                (*(s8 *)((u8 *)(arg0) + 0xB)) = 0;
                cnWrap_SoundRequest(0);
                return 0;
            }
            temp_a3 = (&g_lb_5FBC10)[temp_v1_3];
            if ((*(s8 *)((u8 *)(((*(s32 *)&g_main_2EF390) + 0x23F4 + 0x20 + ((((s8) (((*(s32 *)((u8 *)&g_main_3064AF + (ClassInfo * 0x15C))) - 0x31)))) * 0x10) + temp_a3)) + 0x22E)) != 0) {
                (*(s16 *)((u8 *)(arg0) + 0)) = (s16) temp_a3;
                (*(s16 *)((u8 *)(arg0) + 2)) = 0xF;
                cnWrap_SoundRequest(6);
            } else {
                cnWrap_SoundRequest(7);
            }
            goto block_41;
        }
        if (temp_v1_2 & 0x40) {
            temp_v1_4 = (*(s16 *)((u8 *)(arg0) + 0));
            if (temp_v1_4 != 0x10) {
                var_a1 = 0;
                var_a0 = &g_lb_5FBC10;
loop_19:
                if (*var_a0 != temp_v1_4) {
                    var_a1 = ((s8) ((var_a1 + 1)));
                    var_a0 += 1;
                    if (var_a1 < 0xF) {
                        goto loop_19;
                    }
                }
                (*(s16 *)((u8 *)(arg0) + 2)) = (s16) (((s8) (var_a1)));
                (*(s16 *)((u8 *)(arg0) + 0)) = 0x10;
            } else if ((*(s16 *)((u8 *)(arg0) + 2)) == 7) {
                SetDialogData(0x66);
                (*(s8 *)((u8 *)(arg0) + 0xB)) = 1;
            } else {
                (*(s16 *)((u8 *)(arg0) + 2)) = 7;
            }
            cnWrap_SoundRequest(3);
        } else {
            sp2C = (*(s16 *)((u8 *)(arg0) + 2)) >= 8;
            sp2F = (u8) ((s16) (*(s16 *)((u8 *)(arg0) + 2)) % 8);
            lobby_func_00569390(&sp2F, &sp2C, 8, 0x10, temp_a4);
            (*(s16 *)((u8 *)(arg0) + 2)) = (s16) (sp2F + (sp2C * 8));
        }
block_41:
    default:                                        /* switch 1 */
        return 2;
    case 1:                                         /* switch 1 */
        temp_v0_2 = Lbs_LobbyExit();
        switch (temp_v0_2) {                        /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            (*(s8 *)((u8 *)(arg0) + 0xB)) = (s8) ((*(s8 *)((u8 *)(arg0) + 0xB)) + 1);
            break;
        case 1:                                     /* switch 2 */
            Lb_eat_to_bell((*(s32 *)&cw) + 0xF75D);
            (*(s8 *)((u8 *)(arg0) + 0xB)) = 3;
            break;
        }
        goto block_41;
    case 2:                                         /* switch 1 */
        if (CallBack_Result_Plaza_ReadAllocation2() != 0) {
            goto block_41;
        }
        (*(s8 *)((u8 *)(arg0) + 0xB)) = 0;
        return 3;
    case 3:                                         /* switch 1 */
        if (temp_a4 & 0xFFFF & 0x20) {
            (*(s8 *)((u8 *)(arg0) + 0xB)) = 0;
            cnWrap_SoundRequest(0);
            return 3;
        }
        goto block_41;
    }
}

