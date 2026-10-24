/* G online (PC): Lbs_MatchStart (G lobby 0x583A20; HAND-FIXED: m2c could not take the leaf apart): the room owner
 * starts the match (cnLBS_MatchStart, 6508) and gets 1. docs/network.md 3.12. */
#include "../gonl.h"

int cnLBS_MatchStart();

s32 Lbs_MatchStart(void)
{
    cnLBS_MatchStart();
    return 1;
}
