/* G online (PC): transOtSelectHandleName, written from Monster Hunter G's code at lobby 0x5BACA0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 DispNameAndIDonDialog();
s32 font_set_stack_no();
extern u8 User_data;
extern u8 cw;
extern u8 pNet;
s32 DispDialogData();                           /* extern */

void transOtSelectHandleName(void *arg0) {
    u8 *temp_a0;
    u8 *temp_a1;

    font_set_stack_no((*(s32 *)((u8 *)(arg0) + 0x18)));
    if ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) != 0) {
        DispDialogData();
        temp_a1 = (*(void **)&pNet);
        if ((*(u8 *)((u8 *)(temp_a1) + 0x22)) == 2) {
            temp_a0 = ((*(u8 *)((u8 *)(temp_a1) + 0x29)) * 8) + (*(s32 *)&cw);
            if ((*(s8 *)((u8 *)(temp_a0) + 0xB)) != 0) {
                DispNameAndIDonDialog(0x86, &(*(u8 *)((u8 *)&User_data + 0x8)), temp_a0 + 0xB);
            }
        }
        (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) = 0U;
    }
}

