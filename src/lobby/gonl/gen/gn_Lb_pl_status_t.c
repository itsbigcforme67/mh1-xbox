/* G online (PC): Lb_pl_status_t, written from Monster Hunter G's code at lobby 0x58E090 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 font_set_stack_no();
extern u8 pNet;
s32 Disp_FriendListEntry();          /* extern */
s32 Lb_PlayerStatus();           /* extern */
#ifdef __MWERKS__
s32 SoftKeyboard_alive_check();                     /* extern */
#else
signed char SoftKeyboard_alive_check();                     /* extern */   /* the real return type (tools/check_kr_returns.py) */
#endif
extern s32 lb_player;

void Lb_pl_status_t(void *arg0) {
    s32 var_s0;
    u8 temp_a2;
    u8 *temp_a0;
    u8 *temp_v1;
    u8 *temp_v1_2;

    var_s0 = 0;
    if (SoftKeyboard_alive_check() == 0) {
        font_set_stack_no((*(s32 *)((u8 *)(arg0) + 0x18)));
        temp_v1 = (*(void **)&pNet);
        if ((*(u8 *)((u8 *)(temp_v1) + 0x2D)) == 0) {
            if ((*(u8 *)((u8 *)(temp_v1) + 0x22)) == 1) {
                goto block_4;
            }
        } else {
block_4:
            var_s0 = 0x80;
        }
        temp_v1_2 = (*(void **)&pNet);
        temp_a2 = (*(u8 *)((u8 *)(temp_v1_2) + 0x29));
        Lb_PlayerStatus((u8 *)&lb_player + (temp_a2 * 0x38), ((*(u8 *)((u8 *)(temp_v1_2) + 0x23)) | (var_s0 & 0xFF)) & 0xFF, temp_a2);
        temp_a0 = (*(void **)&pNet);
        if ((*(u8 *)((u8 *)(temp_a0) + 0x23)) == 0) {
            Disp_FriendListEntry(0x110, (*(u8 *)((u8 *)(temp_a0) + 0x22)));
        }
    }
}

