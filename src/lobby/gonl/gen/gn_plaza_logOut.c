/* G online (PC): plaza_logOut, written from Monster Hunter G's code at lobby 0x5731B0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

#ifdef __MWERKS__
s32 Get_sw2();
#else
unsigned short Get_sw2();   /* the real return type (tools/check_kr_returns.py) */
#endif
s32 Lb_select();
s32 SetDialogData();
s32 SetDialogYesNo();
s32 To_LogOut();
s32 fade_set();
s32 tl_exit_sub_menu();
extern u8 pNet;
s32 Fade_busy_ck();                                 /* extern */

void plaza_logOut(void) {
    s32 temp_v0;
    u8 temp_v1;
    u8 *temp_a1;
    u8 *temp_a2;
    u8 *temp_v1_2;

    Get_sw2(0);
    temp_a1 = (*(void **)&pNet);
    temp_v1 = (*(u8 *)((u8 *)(temp_a1) + 0x23));
    temp_a2 = temp_a1 + 0x23;
    switch (temp_v1) {                              /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        (*(u8 *)((u8 *)(temp_a1) + 0x23)) = (u8) (temp_v1 + 1);
        SetDialogData(0x35, (s32) temp_a1, (s32) temp_a2, M2C_ERROR(/* Read from unset register $a3 */));
        SetDialogYesNo(1, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        return;
    case 1:                                         /* switch 1 */
        (*(s8 *)((u8 *)(temp_a1) + 0x2D)) = 1;
        temp_v0 = Lb_select(1, (s32) temp_a1, (s32) temp_a2, M2C_ERROR(/* Read from unset register $a3 */));
        switch (temp_v0) {                          /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            temp_v1_2 = (*(void **)&pNet);
            (*(u8 *)((u8 *)(temp_v1_2) + 0x23)) = (u8) ((*(u8 *)((u8 *)(temp_v1_2) + 0x23)) + 1);
            fade_set(1);
            return;
        case 3:                                     /* switch 2 */
            tl_exit_sub_menu(1, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            return;
        }
        break;
    case 2:                                         /* switch 1 */
        if ((Fade_busy_ck() & 0xFF) != 1) {
            To_LogOut(1, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            return;
        }
        return;
    default:                                        /* switch 1 */
        To_LogOut(1, (s32) temp_a1, (s32) temp_a2, M2C_ERROR(/* Read from unset register $a3 */));
        break;
    }
}

