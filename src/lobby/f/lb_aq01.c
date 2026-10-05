/* lb_aq01 - tag handlers 604, 145 0x005FEF00-0x005FEF70: tagAct_145. Whole file in lb_aq.c. */
#include "lobby_f.h"
extern u8 *bsw;
void tagoutprintf3();
void tagoutprintf6();
void set_align_data();
void get_tag_in_parameter();
int get_numeric_parameter2();

s32 tagAct_145(s32 arg0) {
    u8 sp10[0x108];
    u16 *p;
    u16 v;
    get_tag_in_parameter(arg0, sp10, 0x100);
    *(u16 *)(bsw + 0x4E8) = get_numeric_parameter2(sp10);
    p = (u16 *)(bsw + 0x4E8);
    v = *(u16 *)(bsw + 0x4E8);
    if (v != 0) {
        if ((v & 0xFFFF) > 0x100) {
            *p = 0x100;
        }
    } else {
        *p = 0x100;
    }
    return 0;
}
