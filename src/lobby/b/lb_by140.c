/* lb_by140 - agent B 0x005B2370-0x005B256C: tk_lever_ck (talk-window up/down lever: stick or buttons with auto-repeat timer, moves *p within 0..max, wraps in mode 0/1). */
#include "lobby_a.h"
extern char wait_157[4];
extern char D_38A82E[2];
int tk_sw_on_ck();
int tk_sw_new_ck();
s32 tk_lever_ck(u8 *p, u8 max, s32 mode) {
    u16 *w;
    int dn;
    int up;
    int m;
    int t;

    m = mode & 0xFF;
    if ((m == 0) | (m == 2)) {
        dn = 0x2000;
        up = 0x1000;
        w = (u16 *)wait_157;
    } else {
        dn = 0x800;
        up = 0x400;
        w = (u16 *)D_38A82E;
    }
    if (tk_sw_on_ck(dn) != 0) {
        *w = 0x14;
        goto down;
    }
    if (tk_sw_on_ck(up) != 0) {
        *w = 0x14;
        goto upk;
    }
    t = *w;
    if (t != 0) {
        *w = (t & 0xFFFF) - 1;
        return 0;
    }
    if (tk_sw_new_ck(dn) != 0) {
        *w = 0xA;
        goto down;
    }
    if (tk_sw_new_ck(up) != 0) {
        *w = 0xA;
        goto upk;
    }
    return 0;
upk:
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
down:
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
