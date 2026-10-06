/* lb_by122 - agent B 0x005C3AE0-0x005C3C08: set_se_type (NPC sound-effect type per NPC kind). */
#include "lbnpc_proto.h"
void set_se_type(EMW *em) {
    u8 *ex = em->ex;

    switch (*((u8 *)em + 0x452)) {
    case 8:
    case 9:
    case 0xB:
    case 0xD:
        ex[0x2C] = 0x14;
        break;
    case 0x1A:
        ex[0x2C] = 0x16;
        break;
    case 0x1B:
    case 0x1C:
        ex[0x2C] = 0x18;
        break;
    case 0x37:
        ex[0x2C] = 0x1A;
        break;
    case 0x12:
        ex[0x2C] = 0x1C;
        break;
    case 0x13:
    case 0x1D:
        ex[0x2C] = 0x1E;
        break;
    case 0x38:
        ex[0x2C] = 0x20;
        break;
    case 0x39:
        ex[0x2C] = 0x22;
        break;
    default:
        ex[0x2C] = 0x14;
        break;
    }
}
