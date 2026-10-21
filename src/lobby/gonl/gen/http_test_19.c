/* G online (PC): http_test_19, written from Monster Hunter G's code at lobby 0x5A1C90 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 ParseReq;
void http_test_19(void) {
    (*(s8 *)&ParseReq) = 0;
}

