/* G online (PC): BsStrtblGet, written from Monster Hunter G's code at lobby 0x5B6B90 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 g_lb_6387E0;
extern u8 bs_strtbl_cap_dialog;
extern u8 bs_strtbl_html;
extern u8 bs_strtbl_mmbb_dialog;
extern u8 bs_strtbl_mmbb_ng_msg;
extern u8 bs_strtbl_titlebar;
extern u8 bs_strtbl_toolmenu;

s32 BsStrtblGet(u32 arg0, s32 arg1) {
    switch (arg0) {
    case 2:
        return *(&bs_strtbl_html + (arg1 * 4));
    case 3:
        return *(&bs_strtbl_toolmenu + (arg1 * 4));
    case 4:
        return *(&bs_strtbl_titlebar + (arg1 * 4));
    case 5:
        return *(&bs_strtbl_mmbb_dialog + (arg1 * 4));
    case 6:
        return *(&bs_strtbl_mmbb_ng_msg + (arg1 * 4));
    case 7:
        return *(&bs_strtbl_cap_dialog + (arg1 * 4));
    case 8:
        return *(&g_lb_6387E0 + (arg1 * 4));
    default:
        return 0;
    }
}

