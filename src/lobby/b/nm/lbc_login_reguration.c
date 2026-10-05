#include "lobby_a.h"
extern u8 dod_new_reguration_agree_type;
extern char first_url[];
extern char lit_452_0065E820[];
void lbc_login_reguration(void) {
    u8 temp_a1;
    int temp_a2;
    int temp_a3;

    temp_a2 = (int)cw;
    temp_a1 = F(u8, temp_a2, 0x2C34);
    temp_a3 = temp_a2 + 0x2C34;
    switch (temp_a1) {                              /* irregular */
    case 0:
        F(u8, temp_a2, 0x2C34) = (u8) (temp_a1 + 1);
        fade_set(2, temp_a1, temp_a2, temp_a3);
        strcpy(&first_url, &lit_452_0065E820);
        To_BootUpBrowser();
        return;
    case 1:
        if (F(u8, temp_a2, 0x2C44) == 2) {
            F(u8, temp_a2, 0x2C34) = (u8) (temp_a1 + 1);
            F(u8, (u8 *)cw, 0x2C44) = 0U;
            fade_set(1, temp_a1, temp_a2, temp_a3);
            return;
        }
        lbc_browser(2, temp_a1, temp_a2, temp_a3);
        return;
    case 2:
        if (dod_new_reguration_agree_type == 0) {
            To_LogOut(1, temp_a1, temp_a2, temp_a3);
            return;
        }
        F(u8, temp_a2, 0x2C34) = (u8) (temp_a1 + 1);
        CallBackWaitInit(1, temp_a1, temp_a2, temp_a3);
        cnLBS_Send_RegurationAgree(0);
        return;
    case 3:
        Check_CallBackWait(temp_a1, temp_a2, temp_a3);
    }
}
