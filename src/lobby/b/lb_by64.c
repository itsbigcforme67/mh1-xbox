/* lb_by64 - agent B promoted near-match 0x005B7B90-0x005B7C8C: lbc_login_reguration (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern u8 dod_new_reguration_agree_type;
extern char first_url[];
extern char lit_452_0065E820[];
typedef struct { u8 pad0[0x2C34]; u8 x2C34; u8 pad2C35[0xF]; u8 x2C44; } CWS_reg;
#define CWX ((CWS_reg *)cw)

void lbc_login_reguration(void) {
    switch (CWX->x2C34) {
    case 0:
        CWX->x2C34++;
        fade_set(2);
        strcpy(&first_url, &lit_452_0065E820);
        To_BootUpBrowser();
        return;
    case 1:
        if (CWX->x2C44 == 2) {
            CWX->x2C34++;
            CWX->x2C44 = 0;
            fade_set(1);
            return;
        }
        lbc_browser(2);
        return;
    case 2:
        if (dod_new_reguration_agree_type == 0) {
            To_LogOut(1);
            return;
        }
        CWX->x2C34++;
        CallBackWaitInit();
        cnLBS_Send_RegurationAgree(0);
        return;
    case 3:
        Check_CallBackWait();
    }
}
