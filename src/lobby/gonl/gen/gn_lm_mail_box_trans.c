/* G online (PC): lm_mail_box_trans, written from Monster Hunter G's code at lobby 0x58CF00 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 DispButtonHelp();
s32 DispSceneSubTitle();
s32 Put_receive_mark();
s32 lobby_func_00574FC0();
extern u8 pNet;
void lm_mail_box_trans(s32 unused_a0, s32 unused_a1, s32 unused_a2, s32 arg3) {
    lobby_func_00574FC0(0xD8, 0x38, 1, arg3);
    DispSceneSubTitle(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    DispButtonHelp((*(void **)&pNet), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    Put_receive_mark(1);
}

