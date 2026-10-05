/* lb_bz09 - lobby UI/client 0x005B2C20-0x005B2CA4: cnLbc_LoadNetModel, cnLbc_LoadModelWait, cnWrap_IsBBConnect, cnWrap_GetConnectType, cnWrap_IsAccessMemoryCard, cnLbc_SetIspRestTime, lm_place_i (first drafted by tools/lbauto.py). */
#include "lobby_f.h"

void cnLbc_LoadNetModel(void) {

}

s32 cnLbc_LoadModelWait(void) {
    return 1;
}

s32 cnWrap_IsBBConnect(void) {
    return 1;
}

s32 cnWrap_GetConnectType(void) {
    return 1;
}

s32 cnWrap_IsAccessMemoryCard(void) {
    return *(s8 *)0x3F36CC != 0;
}

void cnLbc_SetIspRestTime(s32 *arg0) {
    *arg0 = 0x1A5E0;
}

s32 lm_place_i(void) {
    return Online_ck() != 1;
}
