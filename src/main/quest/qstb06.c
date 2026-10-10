/* qstb06 - 0x00226920-0x002269E0: Stage_item_data_get, Stage_unique_data_get (same offset-or-pointer tables as
 * Stage_mv_data_get, see qstb05.c). */
#include "quest.h"

void *Stage_item_data_get(int n)
{
    s32 v;
    s32 *p;
    if (quest_w.no == 0) {
        p = quest_w.x8C;
        p += n + 1;
        return (void *)*p;
    }
    p = quest_w.x8C;
    p += n + 1;
    v = *p;
    if (v == 0) {
        return 0;
    }
    return (void *)(v + (int)mission_area);
}

void *Stage_unique_data_get(int n)
{
    s32 v;
    s32 *p;
    if (quest_w.no == 0) {
        p = quest_w.x90;
        p += n;
        return (void *)*p;
    }
    p = quest_w.x90;
    p += n;
    v = *p;
    if (v == 0) {
        return 0;
    }
    return (void *)(v + (int)mission_area);
}
