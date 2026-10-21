/* G online (PC): plaza_ReibunEdit, written from Monster Hunter G's code at lobby 0x572E70 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Get_sw2();
s32 Get_sw_on2();
s32 tl_exit_sub_menu();
extern u8 pNet;
s32 Plaza_ReibunEdit_i();                     /* extern */
s32 Plaza_ReibunEdit_mv();                       /* extern */

void plaza_ReibunEdit(void) {
    s32 temp_s0;
    u8 temp_a0;
    u8 *temp_a0_2;

    temp_a0 = (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x23));
    temp_s0 = Get_sw2(0) & 0xFFFF;
    switch (temp_a0) {                              /* irregular */
    case 0:
        Plaza_ReibunEdit_i(temp_a0);
        temp_a0_2 = (*(void **)&pNet);
        (*(u8 *)((u8 *)(temp_a0_2) + 0x23)) = (u8) ((*(u8 *)((u8 *)(temp_a0_2) + 0x23)) + 1);
        return;
    case 1:
        (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x20)) = Get_sw_on2(0);
        if (Plaza_ReibunEdit_mv(temp_s0) & 0xFFFF & 0x40) {
            tl_exit_sub_menu(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        }
        return;
    }
}

