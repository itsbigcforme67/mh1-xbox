/* lb_by97 - agent B promoted near-match 0x005B7C90-0x005B7DC8: check_warning_level (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
typedef struct { u8 pad0[5]; u8 x05; u8 pad06[0x26]; } CNW5;
extern CNW5 CnetWork;

s32 check_warning_level(s32 arg0) {
    switch (arg0 & 0xFF) {
    case 0:
        return 1;
    case 1:
        if (CnetWork.x05 == 0) {
            return 1;
        }
        break;
    case 2:
        if (CnetWork.x05 == 0) {
            return 1;
        }
        if (CnetWork.x05 == 3) {
            return 1;
        }
        break;
    case 3:
        if (CnetWork.x05 == 0) {
            return 1;
        }
        if (CnetWork.x05 == 3) {
            return 1;
        }
        if (CnetWork.x05 == 2) {
            return 1;
        }
        break;
    case 4:
        if (CnetWork.x05 == 0) {
            return 1;
        }
        if (CnetWork.x05 == 3) {
            return 1;
        }
        if (CnetWork.x05 == 2) {
            return 1;
        }
        if (CnetWork.x05 == 1) {
            return 1;
        }
        break;
    }
    return 0;
}
