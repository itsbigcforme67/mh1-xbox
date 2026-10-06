/* lb_by98 - agent B promoted near-match 0x005B8B90-0x005B8C70: check_top_information_level (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
typedef struct { u8 pad0[5]; u8 x05; u8 pad06[0x26]; } CNW5;
extern CNW5 CnetWork;

s32 check_top_information_level(s32 arg0) {
    switch (arg0 & 0xFF) {
    case 0:
        return 0;
    case 1:
        if (CnetWork.x05 == 1) {
            return 1;
        }
        break;
    case 2:
        if (CnetWork.x05 == 1) {
            return 1;
        }
        if (CnetWork.x05 == 3) {
            return 1;
        }
        break;
    case 3:
        if (CnetWork.x05 == 1) {
            return 1;
        }
        if (CnetWork.x05 == 3) {
            return 1;
        }
        if (CnetWork.x05 == 2) {
            return 1;
        }
        break;
    }
    return 0;
}
