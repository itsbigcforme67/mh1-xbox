/* G online (PC): Plaza_chat_move, written from Monster Hunter G's code at lobby 0x58EC80 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 ChatKinsoku_chk();
s32 Lb_send_chat();
#ifdef __MWERKS__
s32 SoftKeyboard_move();
#else
signed char SoftKeyboard_move();   /* the real return type (tools/check_kr_returns.py) */
#endif
extern u8 Psw;
extern u8 game_w;
s32 Plaza_chatlog_i();                          /* extern */
s32 SoftKeyboard_exit();                     /* extern */

s32 Plaza_chat_move(void) {
    u8 g_frame[0x50] __attribute__((aligned(16)));   /* G's stack frame */
#define sp10 (*(s8 *)(g_frame + 0x10))
    s32 temp_v1_2;
    s32 var_a0;
    u16 temp_v1;

    var_a0 = -1;
    temp_v1 = (*(u16 *)((u8 *)&Psw + 0x4));
    sp10 = 0;
    if (!(temp_v1 & 0x4000)) {
        var_a0 = ((s8) (SoftKeyboard_move(&sp10, (s32) (*(s16 *)&Psw), (s32) (((s16) ((s32) temp_v1))))));
    }
    temp_v1_2 = ((s8) (var_a0));
    if (temp_v1_2 == -1) {
        SoftKeyboard_exit(var_a0);
        Plaza_chatlog_i();
        return -1;
    }
    if (temp_v1_2 != 0) {
        if ((sp10 != 0) && (temp_v1_2 > 0) && (ChatKinsoku_chk(&sp10) != 0)) {
            Lb_send_chat((s32) (*(u8 *)((u8 *)&game_w + 0x3A)), (s32) &sp10, 0, M2C_ERROR(/* Read from unset register $a3 */));
        }
        SoftKeyboard_exit();
        Plaza_chatlog_i();
        return -1;
    }
    return 0;
}

