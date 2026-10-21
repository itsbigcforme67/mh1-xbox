/* G online (PC): lbc_browser_04, written from Monster Hunter G's code at lobby 0x57CA40 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 MainBrowser();
s32 fade_set();
s32 memcmp();
extern u8 cw;
extern u8 pNet;
s32 MainBsDispose();                            /* extern */
extern u8 BrPersonalData;
extern u8 tmpPersonalData;

s32 lbc_browser_04(void) {
    s32 temp_v0;

    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x31)) = 1;
    temp_v0 = MainBrowser();
    switch (temp_v0) {                              /* irregular */
    case 1:
        MainBsDispose();
        if (memcmp(&tmpPersonalData, &BrPersonalData, 0x1D0) != 0) {
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA4)) = 1;
        } else {
            (*(s8 *)((u8 *)((*(void **)&cw)) + 0xBA4)) = 0;
        }
        fade_set(1);
        return 0;
    case -1:
        MainBsDispose();
        fade_set(2);
        return 0;
    case -100:
        MainBsDispose();
        fade_set(2);
        return 1;
    case 0:
        /* fallthrough */
    default:
        return 2;
    }
}

