/* G online (PC): check_sender0, written from Monster Hunter G's code at lobby 0x59A820 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_send_pl_status();                        /* extern */

void check_sender0(void) {
    Lb_send_pl_status();
}

