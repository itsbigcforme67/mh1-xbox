/* lb_s15 - tag handlers with tag buffers 0x005FEE00-0x005FEE40: tagAct_141 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;
extern char input_type_dat[];

s32 tagAct_141(s32 arg0) {
    char sp10[0x108];

    get_tag_in_parameter(arg0, sp10, 0x100);
    F(s8, bsw, 0x4E4) = get_input_type(sp10, input_type_dat, 72);
    return 0;
}
