#include "lobby_a.h"
extern char temp_a2[];
extern char sp10[];
extern char temp_a2[];
extern char sp10[];
extern char temp_a2[];
extern char temp_a2[];
extern char temp_a2[];
void CallBack_Result_ReadFileDownload(int arg0) {
    int sp1C;
    s32 sp18;
    long long sp10;
    int temp_a2;

    temp_a2 = (int)cw;
    sp10 = arg0;
    if ((F(u8, temp_a2, 0x2C31) != 5) && (F(u8, temp_a2, 0x2C45) == 0xC)) {
        if ((s8) sp10 == 0) {
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
