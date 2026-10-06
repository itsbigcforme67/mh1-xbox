/* lb_by59 - agent B promoted near-match 0x0053D3D0-0x0053D444: Lb_put_my_job (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
extern char *my_job_str[2];
extern char lit_551_00655880[];

void Lb_put_my_job(void) {
    font_set_palette(5);
    flfntSetSize(0x12, 0x12);
    flfntLocate(0x124, 0x3C);
    if (*(u8 *)0x3C738D == 7) {
        font_print(lit_551_00655880, my_job_str[0]);
    } else {
        font_print(lit_551_00655880, my_job_str[1]);
    }
}
