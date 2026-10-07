/* lb_c510 - agent C round 5 0x005B2370-0x005B256C: tk_lever_ck (up/down repeat lever with wrap or clamp; gotos reproduce the original block order; locals declared in reverse register order). */
#include "lobby_a.h"
extern char wait_157[4];
extern char D_38A82E[2];
s32 tk_lever_ck(u8 *p, u8 max, s32 mode) {
    u16 *wait;
    s32 up;
    s32 down;
    s32 m;
    u16 t;

    m = mode & 0xFF;
    if (((m == 0) | (m == 2)) != 0) {
        up = 0x2000;
        down = 0x1000;
        wait = (u16 *)wait_157;
    } else {
        up = 0x800;
        down = 0x400;
        wait = (u16 *)D_38A82E;
    }
    if (tk_sw_on_ck(up) != 0) {
        *wait = 0x14;
        goto dec;
    }
    if (tk_sw_on_ck(down) != 0) {
        *wait = 0x14;
        goto inc;
    }
    t = *wait;
    if (t != 0) {
        *wait = (t & 0xFFFF) - 1;
        return 0;
    }
    if (tk_sw_new_ck(up) != 0) {
        *wait = 0xA;
        goto dec;
    }
    if (tk_sw_new_ck(down) != 0) {
        *wait = 0xA;
        goto inc;
    }
    return 0;
inc:
    if (m < 2) {
        if (*p == max) {
            *p = 0;
        } else {
            *p = *p + 1;
        }
    } else {
        if (*p == max) {
            return 0;
        }
        *p = *p + 1;
    }
    cnWrap_SoundRequest(1);
    return 1;
dec:
    if (m < 2) {
        if (*p == 0) {
            *p = max;
        } else {
            *p = *p - 1;
        }
    } else {
        if (*p == 0) {
            return 0;
        }
        *p = *p - 1;
    }
    cnWrap_SoundRequest(1);
    return 1;
}
