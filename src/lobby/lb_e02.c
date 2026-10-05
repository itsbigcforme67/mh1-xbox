/* lb_e02 - lobby members/cockpit/icons 0x005CB3F0-0x005CB468: Lb_frendlist_entry. Whole file in lb_e.c. */
#include "lobby.h"










void Lb_frendlist_entry(int a0, int a1) {
    pNet[7] = 5;
    pNet[5] = 0;
    cnLBS_Get_ConditionSearchUser(&SearchResult);
    memcpy(SearchResult + 4, (void *)a0, 8);
    memcpy(SearchResult + 0xC, (void *)a1, 0x12);
    cnWrap_SoundRequest(6);
}
