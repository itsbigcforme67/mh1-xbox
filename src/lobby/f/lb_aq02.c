/* lb_aq02 - tag handlers 604, 145 0x005FF9C0-0x005FFA20: tagAct_604. Whole file in lb_aq.c. */
#include "lobby_f.h"
extern u8 *bsw;
void tagoutprintf3();
void tagoutprintf6();
void set_align_data();
void get_tag_in_parameter();
int get_numeric_parameter2();

s32 tagAct_604(int arg0, s32 arg1) {
    (bsw + bsw[0xE96C])[0xE96D] = 0;
    bsw[0xE96C]++;
    tagoutprintf3(arg1);
    return 0;
}
