/* G online (PC): lobby_func_0056C480, written from Monster Hunter G's code at lobby 0x56C480 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_0056D6E0();
s32 lobby_func_00571AF0();
void * memset();
extern u8 cw;
extern u8 lb_sys;
extern u8 pNet;
extern u8 tl_member_buff;
s16 net_Check_FriendSuu();        /* extern */
extern u8 g_main_302CB0;

void lobby_func_0056C480(void) {
    s32 temp_v0;
    s32 var_a2;
    s32 var_a4;
    s32 var_a5;
    u8 *var_a3;   /* (u16 *) */

    var_a5 = 0;
    var_a3 = &(*(u16 *)((u8 *)&lb_sys + 0xA0));
    var_a2 = 0;
    do {
        var_a4 = 0;
        if ((*(u16 *)var_a3) == 0) {
            do {
                temp_v0 = var_a4 + var_a2;
                if (temp_v0 < 0x65) {
                    (*(s8 *)((u8 *)(((*(s32 *)&cw) + (temp_v0 * 0x24A))) + 0x1265)) = 2;
                }
                var_a4 += 1;
            } while (var_a4 < 7);
        }
        var_a5 += 1;
        var_a3 += 2;
        var_a2 += 7;
    } while (var_a5 < 0xC);
    memset(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398)), 0, 0x618);
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2E)) = 1;
    (*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x608)) = net_Check_FriendSuu(&g_main_302CB0, 0x32);
    lobby_func_0056D6E0(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    lobby_func_00571AF0(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
}

