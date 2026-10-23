/* G online (PC): id_select_02, written from Monster Hunter G's code at lobby 0x5BA760 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

#ifdef __MWERKS__
s32 Get_sw2();
#else
unsigned short Get_sw2();   /* the real return type (tools/check_kr_returns.py) */
#endif
s32 Lb_select();
s32 SetDialogYesNo();
s32 SetHelpLineMsg();
s32 SetSceneTitle();
s32 fade_set();
extern u8 ret_stat_0038A900;
s32 Fade_busy_ck();                      /* extern */

void id_select_02(void *arg0) {
    s32 temp_v0;
    u8 temp_a0;

    Get_sw2(0);
    (*(s8 *)((u8 *)(arg0) + 0x2D)) = 1;
    temp_a0 = (*(u8 *)((u8 *)(arg0) + 0x23));
    switch (temp_a0) {                              /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        (*(u8 *)((u8 *)(arg0) + 0x23)) = (u8) (temp_a0 + 1);
        SetDialogYesNo(0, 1, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        return;
    case 1:                                         /* switch 1 */
        temp_v0 = Lb_select((s32) temp_a0, 1, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        switch (temp_v0) {                          /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            fade_set(1);
            (*(u8 *)((u8 *)(arg0) + 0x23)) = (u8) ((*(u8 *)((u8 *)(arg0) + 0x23)) + 1);
            return;
        case 3:                                     /* switch 2 */
            (*(s8 *)((u8 *)(arg0) + 0x22)) = 1;
            (*(u8 *)((u8 *)(arg0) + 0x23)) = 0U;
            SetSceneTitle(4, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            SetHelpLineMsg(1, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            return;
        }
        break;
    case 2:                                         /* switch 1 */
        if ((Fade_busy_ck(temp_a0, 1) & 0xFF) != 1) {
            (*(s8 *)&ret_stat_0038A900) = 0;
        }
        return;
    }
}

