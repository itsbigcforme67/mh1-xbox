/* G online (PC): lbc_login_reguration, written from Monster Hunter G's code at lobby 0x57A8F0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Check_CallBackWait();
s32 To_LogOut();
s32 fade_set();
s32 lobby_func_0057C7E0();
extern u8 cw;
s32 To_BootUpBrowser();                         /* extern */
s32 strcpy();               /* extern */
extern u8 first_url;
extern u8 lit_452_0065E820;

void lbc_login_reguration(void) {
    u8 temp_a1;
    u8 *temp_v1;

    temp_v1 = (*(void **)&cw);
    temp_a1 = (*(u8 *)((u8 *)(temp_v1) + 0xB9C));
    switch (temp_a1) {                              /* irregular */
    case 0:
        (*(u8 *)((u8 *)(temp_v1) + 0xB9C)) = (u8) (temp_a1 + 1);
        fade_set(2);
        strcpy(&first_url, &lit_452_0065E820);
        To_BootUpBrowser();
        return;
    case 1:
        if ((*(u8 *)((u8 *)(temp_v1) + 0xBA8)) == 2) {
            (*(u8 *)((u8 *)(temp_v1) + 0xB9C)) = (u8) (temp_a1 + 1);
            (*(u8 *)((u8 *)((*(void **)&cw)) + 0xBA8)) = 0U;
            fade_set(1);
            return;
        }
        lobby_func_0057C7E0();
        return;
    case 3:
        Check_CallBackWait();
        return;
    default:
        To_LogOut(4);
        return;
    }
}

