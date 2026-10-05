#include "lobby_b.h"
extern s32 mission_area;
extern char CallBack_Result_ReadFileDownload[];
s32 Lbc_DownloadQuest(void) {
    s32 temp_a0;
    u8 temp_v1_2;
    int temp_v1;

    temp_v1 = (int)cw;
    temp_a0 = temp_v1 + 0x2C35;
    temp_v1_2 = F(u8, temp_v1, 0x2C35);
    switch (temp_v1_2) {                            /* irregular */
    case 0:
        F(u8, temp_v1, 0x2C35) = (u8) (temp_v1_2 + 1);
        F(s8, (u8 *)cw, 0x2C2F) = 0;
        CallBackWaitInit(temp_a0);
        F(s8, (u8 *)cw, 0x2C45) = 0xC;
        cnLBS_Read_FileDownload(mission_area, &CallBack_Result_ReadFileDownload);
block_12:
    default:
        return 2;
    case 1:
        Check_CallBackWait(temp_a0);
        goto block_12;
    case 2:
        F(u8, temp_v1, 0x2C35) = 0U;
        return 0;
    case 3:
        F(u8, temp_v1, 0x2C35) = 0U;
        return 1;
    }
}
