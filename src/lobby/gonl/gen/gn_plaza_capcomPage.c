/* G online (PC): plaza_capcomPage, written from Monster Hunter G's code at lobby 0x572FC0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Fade_busy_ck();
s32 Get_sw2();
s32 fade_set();
s32 lobby_bgm_set2();
s32 lobby_func_0057C7E0();
void * strcpy();
s32 tl_exit_sub_menu();
extern u8 cw;
extern u8 jtbl_2033;
extern u8 pNet;
s32 Lbc_SendBrowserResult(); /* extern */
s32 To_BootUpBrowser();                         /* extern */
extern u8 first_url;
extern u8 lit_2032_0065DD40;

void plaza_capcomPage(void) {
    s32 temp_a4;
    s32 temp_v0;
    u8 temp_a1;
    u8 *temp_a2;
    u8 *temp_a3;
    u8 *temp_v1;
    u8 *temp_v1_2;
    u8 *temp_v1_3;

    temp_a2 = (*(void **)&pNet);
    temp_a4 = Get_sw2(0) & 0xFFFF;
    temp_a1 = (*(u8 *)((u8 *)(temp_a2) + 0x23));
    temp_a3 = temp_a2 + 0x23;
    switch (temp_a1) {                              /* switch 1 */
    case 0:                                         /* switch 1 */
        (*(u8 *)((u8 *)(temp_a2) + 0x23)) = (u8) (temp_a1 + 1);
        fade_set(1);
        (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB70)) = 0;
        return;
    case 1:                                         /* switch 1 */
        if ((Fade_busy_ck() & 0xFF) != 1) {
            temp_v1 = (*(void **)&pNet);
            (*(u8 *)((u8 *)(temp_v1) + 0x23)) = (u8) ((*(u8 *)((u8 *)(temp_v1) + 0x23)) + 1);
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x31)) = 1;
            strcpy(&first_url, &lit_2032_0065DD40);
            To_BootUpBrowser();
            fade_set(2);
            return;
        }
    default:                                        /* switch 1 */
        return;
    case 2:                                         /* switch 1 */
        (*(s8 *)((u8 *)(temp_a2) + 0x31)) = 1;
        if ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xBA8)) == 2) {
            temp_v1_2 = (*(void **)&pNet);
            (*(u8 *)((u8 *)(temp_v1_2) + 0x23)) = (u8) ((*(u8 *)((u8 *)(temp_v1_2) + 0x23)) + 1);
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x24)) = 0;
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xBA8)) = 0U;
            fade_set(2);
            lobby_bgm_set2(0x4B);
            if ((*(u8 *)((u8 *)((*(void **)&cw)) + 0xBA4)) != 0) {
                (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x23)) = 4U;
            }
            return;
        }
        lobby_func_0057C7E0(2, (s32) temp_a1, (s32) temp_a2, (s32) temp_a3);
        return;
    case 3:                                         /* switch 1 */
        tl_exit_sub_menu(1, (s32) temp_a1, (s32) temp_a2, (s32) temp_a3);
        return;
    case 4:                                         /* switch 1 */
        temp_v0 = Lbc_SendBrowserResult(&jtbl_2033, temp_a1, temp_a2, temp_a3, temp_a4);
        switch (temp_v0) {                          /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB70)) = 1;
block_22:
            (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x23)) = 3;
            break;
        case 1:                                     /* switch 2 */
            temp_v1_3 = (*(void **)&pNet);
            (*(u8 *)((u8 *)(temp_v1_3) + 0x23)) = (u8) ((*(u8 *)((u8 *)(temp_v1_3) + 0x23)) + 1);
            fade_set(2);
            return;
        }
        break;
    case 5:                                         /* switch 1 */
        (*(s8 *)((u8 *)(temp_a2) + 0x2D)) = 1;
        if (temp_a4 & 0xFFFF & 0x20) {
            goto block_22;
        }
        break;
    }
}

