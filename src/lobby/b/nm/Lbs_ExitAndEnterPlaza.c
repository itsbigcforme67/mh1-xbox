#include "lobby_a.h"
extern char CallBack_Result_Plaza_PlazaExit2[];
extern char CallBack_Result_Plaza_PlazaEntry2[];
typedef struct { u8 pad0[0x2C35]; u8 x2C35; u8 pad2C36[0xF]; s8 x2C45; } CWS_ee;
#define CWX ((CWS_ee *)cw)
s32 Lbs_ExitAndEnterPlaza(arg0)
int arg0;
{
    switch (CWX->x2C35) {
    case 0:
        CWX->x2C35++;
        CallBackWaitInit();
        CWX->x2C45 = 9;
        cnLBS_PlazaExit(&CallBack_Result_Plaza_PlazaExit2);
        break;
    case 1:
        Check_CallBackWait();
        break;
    case 2:
        CWX->x2C35++;
        CallBackWaitInit();
        CWX->x2C45 = 4;
        cnLBS_PlazaEntry(arg0 & 0xFFFF, &CallBack_Result_Plaza_PlazaEntry2);
        break;
    case 3:
        Check_CallBackWait();
        break;
    case 4:
        CWX->x2C35 = 0;
        return 0;
    case 5:
        CWX->x2C35 = 0;
        return 1;
    }
    return 2;
}
