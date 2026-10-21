/* G online (PC): Lb_put_inputMsgForHTML, written from Monster Hunter G's code at lobby 0x59DA30 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 flfntLocate();
s32 font_set_palette();
s32 font_set_stack_no();
extern u8 System_timer;
extern u8 lb_rule_member;
s32 func_0012BDD0();                         /* extern */

void Lb_put_inputMsgForHTML(void) {
    if ((*(u16 *)&System_timer) & 0x20) {
        font_set_stack_no(5);
        flfntLocate(0xC8, 0x17C);
        font_set_palette(5);
        func_0012BDD0((*(s32 *)((u8 *)&lb_rule_member + 0x1C)));
    }
}

