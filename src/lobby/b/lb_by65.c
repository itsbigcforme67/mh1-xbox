/* lb_by65 - agent B promoted near-match 0x005B9460-0x005B952C: lbc_browser_04 (first drafted by tools/lbauto.py). */
#include "lobby_f.h"
extern u8 * pNet;
extern char tmpPersonalData[];
extern char BrPersonalData[];
typedef struct { u8 pad0000[0x2C43]; u8 x2C43; } CWS_lbc_browser_04;

void lbc_browser_04(void) {
    s32 temp_v0;

    F(s8, pNet, 0x11) = 1;
    temp_v0 = MainBrowser();
    switch (temp_v0) {
    case 0:
        break;
    case 1:
        MainBsDispose();
        if (memcmp(&tmpPersonalData, &BrPersonalData, 0x1D0) != 0) {
            F(s8, (u8 *)cw, 0x2C3C) = 1;
        } else {
            F(s8, (u8 *)cw, 0x2C3C) = 0;
        }
        ((CWS_lbc_browser_04 *)cw)->x2C43 = (u8) (((CWS_lbc_browser_04 *)cw)->x2C43 + 1);
        fade_set(1);
        break;
    case -1:
        MainBsDispose();
        ((CWS_lbc_browser_04 *)cw)->x2C43 = (u8) (((CWS_lbc_browser_04 *)cw)->x2C43 + 1);
        fade_set(2);
        break;
    }
}
