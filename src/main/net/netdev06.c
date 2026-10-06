/* netdev06 - _reset_dialtype (SLPM_654.95 0x00238010-0x00238060): flips Ppp_dial_param between 0 and 1 against MyDialType (tone / pulse
   dialing retry; a guess from the names); returns 0 if flipped, -1 if it had already been tried. Written new in this pass. */
#include "types.h"
extern u8 MyDialType;
extern s16 Ppp_dial_param;
int _reset_dialtype(void) {
    if (MyDialType == 1) {
        if (Ppp_dial_param == 1) {
            Ppp_dial_param = 0;
            return 0;
        }
    } else if (Ppp_dial_param == 0) {
        Ppp_dial_param = 1;
        return 0;
    }
    return -1;
}
