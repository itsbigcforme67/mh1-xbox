/* lb_e07 - lobby members/cockpit/icons 0x005CCF60-0x005CCFE4: Lb_num_to_str. Whole file in lb_e.c. */
#include "lobby_f.h"













void Lb_num_to_str(int n, char *out) {
    char buf[0x20];
    char *p;
    char c;
    sprintf(buf, lit_688_00664CB0, n);
    *out = 0;
    c = buf[0];
    p = buf;
    if (c != 0) {
        do {
            strcat(out, *(char **)((u8 *)lb_num_str + (c - 0x30) * 4));
            p++;
            c = *p;
        } while (c != 0);
    }
}
