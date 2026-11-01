/* G online (PC): lm_friend_list_trans, written from Monster Hunter G's code at lobby 0x58D000 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 DispButtonHelp();
s32 DispSceneSubTitle();
s32 Put_receive_mark();
s32 plaza_mailBoxTrans();
extern u8 pNet;
void lm_friend_list_trans(s32 unused_a0, s32 unused_a1, s32 unused_a2, s32 arg3) {
    plaza_mailBoxTrans(0xD8, 0x38, 1, arg3);
    DispSceneSubTitle(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    DispButtonHelp((*(void **)&pNet), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    Put_receive_mark(1);
}

