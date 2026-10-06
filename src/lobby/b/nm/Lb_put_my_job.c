#include "lobby_a.h"
extern u8 my_job_str[8];
extern char lit_551_00655880[];
extern char lit_551_00655880[];
void Lb_put_my_job(void) {
    font_set_palette(5);
    flfntSetSize(0x12, 0x12);
    flfntLocate(0x124, 0x3C);
    if (*(u8 *)0x3C738D == 7) {
        font_print(&lit_551_00655880, my_job_str);
        return;
    }
    font_print(&lit_551_00655880, (*(s32 *)(my_job_str + 4)));
}
