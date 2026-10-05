/* cnlbs, run 43: cnetGet_Login_NoOfUserAccount .. cnetGet_Login_NoOfUserAccount (lobby.bin 0x005AA430-0x005AA43C): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

u8 cnetGet_Login_NoOfUserAccount(void) {
    return CNW(u8, 0x145E);
}
