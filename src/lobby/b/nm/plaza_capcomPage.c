#include "lobby_a.h"
extern char jtbl_2033[];
extern char first_url[];
extern char jtbl_2033[];
extern char lit_2032_0065DD40[];
void plaza_capcomPage(void) {
    s32 temp_t0;
    s32 temp_v0;
    u8 temp_a1;
    int temp_a2;
    int temp_a3;
    int temp_v1;
    int temp_v1_2;

    temp_a2 = (int)pNet;
    temp_t0 = Get_sw2(0) & 0xFFFF;
    temp_a1 = F(u8, temp_a2, 3);
    temp_a3 = temp_a2 + 3;
    switch (temp_a1) {                              /* switch 1 */
    case 0:                                         /* switch 1 */
        F(u8, temp_a2, 3) = (u8) (temp_a1 + 1);
        fade_set(1, temp_a1, temp_a2, temp_a3);
        F(s8, (u8 *)cw, 0x2C08) = 0;
        return;
    case 1:                                         /* switch 1 */
        if ((Fade_busy_ck(&jtbl_2033, temp_a1, temp_a2, temp_a3) & 0xFF) != 1) {
            temp_v1 = (int)pNet;
            F(u8, temp_v1, 3) = (u8) (F(u8, temp_v1, 3) + 1);
            F(s8, pNet, 0x11) = 1;
            strcpy(&first_url, &lit_2032_0065DD40, 1);
            To_BootUpBrowser();
            fade_set(2);
            return;
        }
    default:                                        /* switch 1 */
        return;
    case 2:                                         /* switch 1 */
        if (F(u8, (u8 *)cw, 0x2C44) == 2) {
            F(u8, temp_a2, 3) = (u8) (temp_a1 + 1);
            F(s8, pNet, 4) = 0;
            F(u8, (u8 *)cw, 0x2C44) = 0U;
            fade_set(2, temp_a1, temp_a2, temp_a3);
            lobby_bgm_set2(0x48);
            if (F(u8, (u8 *)cw, 0x2C3C) != 0) {
                F(u8, pNet, 3) = 4U;
            }
            return;
        }
        lbc_browser(2, temp_a1, temp_a2, temp_a3);
        F(s8, pNet, 0x11) = 1;
        return;
    case 3:                                         /* switch 1 */
        tl_exit_sub_menu(1, temp_a1, temp_a2, temp_a3);
        return;
    case 4:                                         /* switch 1 */
        temp_v0 = Lbc_SendBrowserResult(&jtbl_2033, temp_a1, temp_a2, temp_a3);
        switch (temp_v0) {                          /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            F(s8, (u8 *)cw, 0x2C08) = 1;
block_22:
            F(s8, pNet, 3) = 3;
            break;
        case 1:                                     /* switch 2 */
            temp_v1_2 = (int)pNet;
            F(u8, temp_v1_2, 3) = (u8) (F(u8, temp_v1_2, 3) + 1);
            fade_set(2, 1U);
            return;
        }
        break;
    case 5:                                         /* switch 1 */
        F(s8, temp_a2, 0xC) = 1;
        if (temp_t0 & 0xFFFF & 0x20) {
            goto block_22;
        }
        break;
    }
}
