/* lb_n04 - lobby senders 0x005D63B0-0x005D6420: Lb_send_data_to_myself. Whole file in lb_n.c. */
#include "lobby_f.h"








void Lb_send_data_to_myself(len, type, data)
int len;
s8 type;
u8 *data;
{
    if (Online_ck() != 0) {
        sendDat[0] = type;
        flMemcpy(sendDat + 1, data, len & 0xFF);
        Lb_check_receipt(cw + 0x440, sendDat);
    }
}
