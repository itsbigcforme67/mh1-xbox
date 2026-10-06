/* netdev10 - _decide_dialtype (SLPM_654.95 0x00238060-0x00238070): copies the dial parameter into MyDialType. Written new in this pass. */
#include "types.h"
extern s8 MyDialType;
extern s16 Ppp_dial_param;

int _decide_dialtype(void) {
    MyDialType = Ppp_dial_param;
    return 0;
}
