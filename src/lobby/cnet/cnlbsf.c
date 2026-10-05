/* cnlbs, run 6: __cnet_SendSet_PersonalDataAge .. __cnet_SendSet_PersonalDataAge (lobby.bin 0x005A3730-0x005A3780): the matching functions of cnlbs_nm.c. */
#include "lbnet_proto.h"

void __cnet_SendSet_PersonalDataAge(void) {
    SetSendCommand(&send_work, 0xB8);
    SetSendData8(&send_work, CNW(u8, 0x1248));
    SetSendCommandLen(&send_work);
    Write_Socket(&send_work);
}
