/* G online (PC): Lb_eat_to_bell, written from Monster Hunter G's code at lobby 0x59F3B0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 dialogData;
extern u8 htmlStr;
s32 set_dialog_square();                 /* extern */

void Lb_eat_to_bell(s32 arg0) {
    (*(s32 *)&htmlStr) = arg0;
    (*(s8 *)((u8 *)&dialogData + 0x12)) = 1;
    set_dialog_square(0x1F4);
}

