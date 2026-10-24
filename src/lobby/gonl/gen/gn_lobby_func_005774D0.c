/* G online (PC): lobby_func_005774D0, written from Monster Hunter G's code at lobby 0x5774D0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Put_page_num();
s32 lobby_func_00576A70();
s32 lobby_func_00576CB0();
s32 lobby_func_00576F20();
s32 lobby_func_00577370();
s32 put_main_cursor();
extern u8 pNet;
extern u8 tl_member_buff;

void lobby_func_005774D0(s32 unused_a0, s32 arg1, s32 arg2, s32 arg3) {
    if ((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x2D)) == 0) {
        switch ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A))) { /* irregular */
        case 4:
            put_main_cursor((s32) (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1372)), arg1, arg2, arg3);
            lobby_func_00576A70(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            Put_page_num(0x1E0, 0x128, (s32) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1364)), (s32) (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1366)), 0);
            return;
        case 2:
            if ((*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136B)) != 1) {
                put_main_cursor((s32) (*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1373)), arg1, arg2, arg3);
            } else {
                put_main_cursor(7, arg1, arg2, arg3);
            }
            lobby_func_00576CB0(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            return;
        case 6:
            if ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x1375)) == 0) {
                lobby_func_00576F20((s32) (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)), arg1, arg2, arg3);
                return;
            }
            lobby_func_00577370((s32) (*(u8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x18))) + 0x136A)), arg1, arg2, arg3);
            break;
        }
    }
}

