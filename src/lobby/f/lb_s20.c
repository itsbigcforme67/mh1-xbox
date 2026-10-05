/* lb_s20 - tag handlers with tag buffers 0x00600AA0-0x00600AE0: tagAct_322 (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern int bsw;
extern char tr_valign_dat[];

s32 tagAct_322(s32 arg0) {
    char sp10[0x108];

    get_tag_in_parameter(arg0, sp10, 0x100);
    F(s8, bsw, 0xF17) = get_input_type(sp10, tr_valign_dat, 32);
    return 0;
}
