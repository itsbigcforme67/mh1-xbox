/* G online (PC): lobby_func_0056B0C0, written from Monster Hunter G's code at lobby 0x56B0C0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

#ifdef __MWERKS__
s32 Get_sw2();
#else
unsigned short Get_sw2();   /* the real return type (tools/check_kr_returns.py) */
#endif
#ifdef __MWERKS__
s32 Get_sw_on2();
#else
unsigned short Get_sw_on2();   /* the real return type (tools/check_kr_returns.py) */
#endif
s32 Lb_eat_to_bell();
s32 SetDialogData();
s32 cnWrap_SoundRequest();
s64 lobby_func_00569320();
s64 lobby_func_00569390();
s32 lobby_func_0056AF20();
s32 lobby_func_00580A40();
s32 lobby_func_0059F740();
s32 tl_exit_sub_menu();
extern u8 g_main_2EF390;
extern u8 PlazaInfo;
extern u8 cw;
extern u8 g_main_2EF390;
extern u8 pNet;
extern u8 ClassInfo;

void lobby_func_0056B0C0(void) {
    s16 temp_a2;
    s16 temp_a3_2;
    s32 temp_a1;
    s32 temp_a1_2;
    s32 temp_a1_3;
    s32 temp_a2_2;
    s32 temp_s0;
    s32 temp_s1;
    s32 temp_v0_2;
    u8 temp_a0;
    u8 temp_a0_2;
    u8 temp_a3;
    u8 *temp_a4;
    u8 *temp_v0;
    u8 *temp_v1;
    u8 *temp_v1_2;
    u8 *temp_v1_3;

    temp_a4 = (*(void **)&pNet);
    temp_s1 = Get_sw2(0) & 0xFFFF;
    temp_a2 = (*(s16 *)((u8 *)(temp_a4) + 0x1C));
    temp_a3 = (*(u8 *)((u8 *)(temp_a4) + 0x2B));
    temp_a0 = (*(u8 *)((u8 *)(temp_a4) + 0x23));
    temp_a1 = temp_a2 * 7;
    temp_s0 = temp_a3 + temp_a1;
    switch (temp_a0) {                              /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x20)) = Get_sw_on2(0);
        temp_a1_2 = temp_s1 & 0xFFFF;
        (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x1E)) = lobby_func_00569320((s32) (*(s16 *)((u8 *)&ClassInfo + 0x2)), 7, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        if (temp_a1_2 & 0x20) {
            if (lobby_func_0056AF20(temp_s0 + 1, 0, (s32) *(&(*(u8 *)((u8 *)&PlazaInfo + 0x5B)) + (temp_s0 * 0x15C)), M2C_ERROR(/* Read from unset register $a3 */)) == 0) {
                (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x23)) = 2U;
                cnWrap_SoundRequest(7, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                return;
            }
            if (ClassInfo == (temp_s0 + 1)) {
                (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x23)) = 2U;
                SetDialogData(0x16, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                cnWrap_SoundRequest(7, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
                return;
            }
            temp_v1 = (*(void **)&pNet);
            (*(u8 *)((u8 *)(temp_v1) + 0x23)) = (u8) ((*(u8 *)((u8 *)(temp_v1) + 0x23)) + 1);
            SetDialogData(0xF, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            cnWrap_SoundRequest(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            return;
        }
        if (temp_a1_2 & 0x40) {
            tl_exit_sub_menu(0, temp_a1_2, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            return;
        }
        temp_v0 = (*(void **)&pNet);
        lobby_func_00569390(temp_v0 + 0x2B, temp_v0 + 0x1C, 7, (s32) (*(s16 *)((u8 *)&ClassInfo + 0x2)), temp_s1);
        temp_v1_2 = (*(void **)&pNet);
        temp_a3_2 = (*(s16 *)((u8 *)(temp_v1_2) + 0x1C));
        temp_a2_2 = (*(u8 *)((u8 *)(temp_v1_2) + 0x2B)) + (temp_a3_2 * 7);
        temp_a1_3 = temp_a2_2 * 0x15C;
        temp_a0_2 = (*(&(*(u8 *)((u8 *)&PlazaInfo + 0x5B)) + temp_a1_3) - 0x31) & 0xFF;
        if (temp_a0_2 != (*(u8 *)((u8 *)(temp_v1_2) + 0x27))) {
            (*(u8 *)((u8 *)(temp_v1_2) + 0x27)) = temp_a0_2;
            lobby_func_0059F740((*(s32 *)&g_main_2EF390) + 0x23F4 + 0x20 + ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x27)) << 8) + 0x288, temp_a1_3, temp_a2_2, (s32) temp_a3_2);
            return;
        }
        return;
    case 1:                                         /* switch 1 */
        temp_v0_2 = lobby_func_00580A40(temp_s0 + 1, 0, (s32) temp_a2, (s32) temp_a3);
        switch (temp_v0_2) {                        /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            (*(s32 *)((u8 *)((*(void **)&pNet)) + 0x14)) = 3;
            break;
        case 1:                                     /* switch 2 */
            temp_v1_3 = (*(void **)&pNet);
            (*(u8 *)((u8 *)(temp_v1_3) + 0x23)) = (u8) ((*(u8 *)((u8 *)(temp_v1_3) + 0x23)) + 1);
            Lb_eat_to_bell((*(s32 *)&cw) + 0xF75D, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            break;
        }
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 1;
        return;
    case 2:                                         /* switch 1 */
        (*(s8 *)((u8 *)(temp_a4) + 0x2D)) = 1;
        if (temp_s1 & 0xFFFF & 0x20) {
            (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x23)) = 0U;
            cnWrap_SoundRequest(6, temp_a1, (s32) temp_a2, (s32) temp_a3);
            return;
        }
        break;
    default:                                        /* switch 1 */
        (*(u8 *)((u8 *)(temp_a4) + 0x23)) = 0U;
        break;
    }
}

