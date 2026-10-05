#include "lobby_a.h"
typedef struct { u8 pad0000[0x452]; u8 x0452; } ARG_set_se_type_arg0;

void set_se_type(ARG_set_se_type_arg0 *arg0) {
    s8 var_a0;
    u8 temp_a1;

    temp_a1 = arg0->x0452;
    switch (temp_a1) {                              /* irregular */
    case 0xD:
    case 0xB:
    case 0x9:
    case 0x8:
        var_a0 = 0x14;
        break;
    case 0x1A:
        var_a0 = 0x16;
        break;
    case 0x1C:
    case 0x1B:
        var_a0 = 0x18;
        break;
    case 0x37:
        var_a0 = 0x1A;
        break;
    case 0x12:
        var_a0 = 0x1C;
        break;
    case 0x1D:
    case 0x13:
        var_a0 = 0x1E;
        break;
    case 0x38:
        var_a0 = 0x20;
        break;
    case 0x39:
        var_a0 = 0x22;
        break;
    default:
        var_a0 = 0x14;
        break;
    }
    F(s8, ((u8 *)arg0 + 0x444), 0x2C) = var_a0;
}
