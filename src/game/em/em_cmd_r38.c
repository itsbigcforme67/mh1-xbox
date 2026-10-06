/* em_cmd_r38 - agent D 0x00565520-0x005655CC: else_ck (find the matching "else" command). Whole file in em_cmd_nm.c. */
#include "em_cmd.h"

u8 *else_ck(EMW *em, u8 *p, int code)
{
    int new_var;
    u8 c;

    c = code;
    if (p[0] == c && p[1] == 2) {
        p = next_cmd_search(em, p);
        goto done;
    }
    new_var = 2;
    for (;;) {
        if (p[new_var * 0] == c && p[1] == 2) {
            p = next_cmd_search(em, p);
            goto done;
        }
        p = cmd_end_search(em, p, code, 2);
    }
done:
    return p;
}
