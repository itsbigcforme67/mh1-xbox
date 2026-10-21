/* G online (PC): lobby_func_005774D0, written from Monster Hunter G's code at lobby 0x5774D0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 pNet;
extern u8 tl_member_buff;
s32 Put_page_num(); /* extern */
s32 lobby_func_00576A70();                      /* extern */
s32 lobby_func_00576CB0();                      /* extern */
s32 lobby_func_00576F20();                    /* extern */
s32 lobby_func_00577370();                    /* extern */
s32 put_main_cursor();                        /* extern */

void lobby_func_005774D0(void) {
    if ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) == 0) {
        switch ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A))) { /* irregular */
        case 4:
            put_main_cursor((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1372)));
            lobby_func_00576A70();
            Put_page_num(0x1E0, 0x128, (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1364)), (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1366)), 0);
            return;
        case 2:
            if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) != 1) {
                put_main_cursor((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)));
            } else {
                put_main_cursor(7);
            }
            lobby_func_00576CB0();
            return;
        case 6:
            if ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1375)) == 0) {
                lobby_func_00576F20((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)));
                return;
            }
            lobby_func_00577370((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)));
            break;
        }
    }
}

