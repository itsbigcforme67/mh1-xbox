/* lb_dd09 - browser: get_input_tag_sp_type 0x00603020-0x006030D0 (maps an <input> key code to a type). u8 locals give the daddiu constants. Hand-written from the asm. */
#include "lobby_f.h"
u8 get_input_tag_sp_type(p)
u8 *p;
{
    u8 v;
    u8 c;
    c = *p;
    v = 0;
    switch (c) {
    case 8:
        v = 3;
        break;
    case 11:
        v = 2;
        break;
    case 9:
    case 10:
        v = 1;
        break;
    case 40:
        v = 4;
        break;
    case 44:
        v = 1;
        break;
    case 45:
        v = 2;
        break;
    case 46:
        v = 3;
        break;
    case 47:
        v = 0x11;
        break;
    }
    *p = 0;
    return v;
}
