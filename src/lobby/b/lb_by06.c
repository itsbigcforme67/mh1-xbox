/* lb_by06 - agent B promoted near-match 0x005C1B00-0x005C1B5C: CallBack_Event_LbsBinary (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
typedef struct { u8 pad0000[0x2C0C]; s8 x2C0C; u8 pad2C0D[0x24]; u8 x2C31; } CWS_CallBack_Event_LbsBinary;

void CallBack_Event_LbsBinary(CNET_RES res) {
    char sp20[784];
    u8 temp_a0;
    temp_a0 = ((CWS_CallBack_Event_LbsBinary *)cw)->x2C31;
    if (temp_a0 != 5) {
        if ((temp_a0 == 4) || (((CWS_CallBack_Event_LbsBinary *)cw)->x2C0C != 0)) {
            return;
        }
        cnLBS_Get_ChatBinary(sp20, ((CWS_CallBack_Event_LbsBinary *)cw));
        Lb_check_receipt(sp20, sp20 + 8);
    }
}
