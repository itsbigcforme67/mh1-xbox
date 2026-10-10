/* qstb05 - 0x002268A0-0x00226900: Stage_mv_data_get (the exits list of stage n). With a quest loaded the table
 * holds offsets into mission_area (0 = none), in free hunts (quest_w.no == 0) pointers. `p += n; *p` (pointer
 * advanced in its register) is what gives the original addu. */
#include "quest.h"

void *Stage_mv_data_get(int n)
{
    s32 v;
    s32 *p;
    if (quest_w.no == 0) {
        p = quest_w.x7C;
        p += n;
        return (void *)*p;
    }
    p = quest_w.x7C;
    p += n;
    v = *p;
    if (v == 0) {
        return 0;
    }
    return (void *)(v + (int)mission_area);
}
