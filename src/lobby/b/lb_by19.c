/* lb_by19 - agent B promoted near-match 0x005BB9A0-0x005BBA24: CallBack_Result_ReadFileDownload (first drafted by tools/lbauto.py). */
#include "lobby_b.h"

void CallBack_Result_ReadFileDownload(CNET_RES res) {
    int sp1C;
    s32 sp18;
    int temp_a2;

    temp_a2 = (int)cw;
    if ((F(u8, temp_a2, 0x2C31) != 5) && (F(u8, temp_a2, 0x2C45) == 0xC)) {
        if (res.val == 0) {
            F(u8, temp_a2, 0x2C35) = (u8) (F(u8, temp_a2, 0x2C35) + 1);
            cnLBS_Get_FileDownloadInfo(&sp18, &sp1C, temp_a2);
            if (sp18 != 0) {
                F(s8, (u8 *)cw, 0x2C2F) = 1;
            }
        } else {
            F(u8, temp_a2, 0x2C35) = 3U;
        }
    }
}
