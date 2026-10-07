/* NOT BUILT. plaza_capcomPage (0x599280): 6/117 differ, only a/sw register rotation (original: pNet a2, q a3, sw t0). Standalone file for tools/check.py --at plaza_capcomPage=0x599280 */
#include "lobby_a.h"
extern char first_url[];
typedef struct { u8 p0[3]; u8 x3; u8 p4[8]; s8 xC; } PNC;
void plaza_capcomPage() {
    u16 sw;
    PNC *a;
    u8 *q;
    u8 st;

    sw = Get_sw2(0);
    a = (PNC *)pNet;
    q = &a->x3;
    st = *q;
    switch (st) {
    case 0:
        *q = st + 1;
        fade_set(1);
        F(s8, (u8 *)cw, 0x2C08) = 0;
        break;
    case 1:
        if ((Fade_busy_ck() & 0xFF) != 1) {
            pNet[3]++;
            F(s8, pNet, 0x11) = 1;
            strcpy(first_url, "lbs://lbs/04/DATABASE.HTM");
            To_BootUpBrowser();
            fade_set(2);
        }
        break;
    case 2:
        if (F(u8, (u8 *)cw, 0x2C44) == 2) {
            *q = st + 1;
            F(s8, pNet, 4) = 0;
            F(u8, (u8 *)cw, 0x2C44) = 0;
            fade_set(2);
            lobby_bgm_set2(0x48);
            if (F(u8, (u8 *)cw, 0x2C3C) != 0) {
                F(u8, pNet, 3) = 4;
            }
            return;
        }
        lbc_browser(2);
        F(s8, pNet, 0x11) = 1;
        return;
    case 3:
        tl_exit_sub_menu(1);
        break;
    case 4:
        switch (Lbc_SendBrowserResult()) {
        case 0:
            F(s8, (u8 *)cw, 0x2C08) = 1;
            F(s8, pNet, 3) = 3;
            break;
        case 1:
            pNet[3]++;
            fade_set(2);
            break;
        }
        break;
    case 5:
        a->xC = 1;
        if (sw & 0x20) {
            F(s8, pNet, 3) = 3;
        }
        break;
    }
}
