/* lb_z103 - auto-drafted 0x005D8410-0x005D845C: Lb_put_inputMsgForHTML (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern u16 System_timer;
extern char lit_1068_00665D70[];

void Lb_put_inputMsgForHTML(void) {
    if (System_timer & 0x20) {
        font_set_stack_no(3);
        flfntLocate(0xC8, 0x17C);
        font_set_palette(5);
        font_print(&lit_1068_00665D70);
    }
}
