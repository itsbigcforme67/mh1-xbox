/* lb_gz09 - browser table/tag handlers 0x006010F0-0x006011AC: tagAct_341, tagAct_342 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;
extern char tr_align_dat[];
extern char tr_valign_dat[];

s32 tagAct_341(int arg0) {
    char sp10[0x108];

    get_tag_in_parameter(arg0, sp10, 0x100);
    if (F(s8, bsw, 0x186) != -0xA) {
        return 0;
    }
    F(s8, bsw, 0xF16) = get_input_type(sp10, &tr_align_dat, 0x20);
    return 0;
}

s32 tagAct_342(int arg0) {
    char sp10[0x108];

    get_tag_in_parameter(arg0, sp10, 0x100);
    if (F(s8, bsw, 0x186) != -0xA) {
        return 0;
    }
    F(s8, bsw, 0xF17) = get_input_type(sp10, &tr_valign_dat, 0x20);
    return 0;
}
