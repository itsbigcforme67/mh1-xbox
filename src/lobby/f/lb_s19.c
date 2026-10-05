/* lb_s19 - tag handlers with tag buffers 0x00600A60-0x00600AA0: tagAct_321 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;
extern char tr_align_dat[];

s32 tagAct_321(s32 arg0) {
    char sp10[0x108];

    get_tag_in_parameter(arg0, sp10, 0x100);
    F(s8, bsw, 0xF16) = get_input_type(sp10, tr_align_dat, 32);
    return 0;
}
