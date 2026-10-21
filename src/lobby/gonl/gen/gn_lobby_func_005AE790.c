/* G online (PC): lobby_func_005AE790, written from Monster Hunter G's code at lobby 0x5AE790 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 CnetSys_w;
s32 __cnetSub_Get_RestBgWork();                     /* extern */
s32 cnLBS_Read_RoomRuleCaption(); /* extern */
s32 cnLBS_Read_RoomRuleChoiceCount(); /* extern */
s32 cnLBS_SendMessage();    /* extern */
s32 strlen();                                   /* extern */
extern u8 g_lb_72E068;
extern u8 g_lb_72E069;
extern u8 g_lb_72E06A;
extern u8 lobby_func_005AE9D0;
extern u8 lobby_func_005AEA10;
extern u8 lobby_func_005AEA50;

void lobby_func_005AE790(void *arg0) {
    if (__cnetSub_Get_RestBgWork() >= 3) {
        (*(u8 *)((u8 *)(arg0) + 0x22)) = (u8) ((*(u8 *)((u8 *)(arg0) + 0x22)) + 1);
        if (((*(s8 *)((u8 *)&CnetSys_w + 0x12CA)) != 0) && (g_lb_72E068 == 1)) {
            cnLBS_SendMessage(&(*(s8 *)((u8 *)&CnetSys_w + 0x12CA)), strlen(&(*(s8 *)((u8 *)&CnetSys_w + 0x12CA))) & 0xFFFF, &lobby_func_005AE9D0);
        } else {
            (*(u8 *)((u8 *)(arg0) + 0x23)) = (u8) ((*(u8 *)((u8 *)(arg0) + 0x23)) | 1);
        }
        if (((*(s8 *)((u8 *)&CnetSys_w + 0x130B)) != 0) && (g_lb_72E069 == 1)) {
            cnLBS_Read_RoomRuleCaption(&(*(s8 *)((u8 *)&CnetSys_w + 0x130B)), strlen(&(*(s8 *)((u8 *)&CnetSys_w + 0x130B))) & 0xFFFF, &lobby_func_005AEA10);
        } else {
            (*(u8 *)((u8 *)(arg0) + 0x23)) = (u8) ((*(u8 *)((u8 *)(arg0) + 0x23)) | 2);
        }
        if (((*(s8 *)((u8 *)&CnetSys_w + 0x1314)) != 0) && (g_lb_72E06A == 1)) {
            cnLBS_Read_RoomRuleChoiceCount(&(*(s8 *)((u8 *)&CnetSys_w + 0x1314)), strlen(&(*(s8 *)((u8 *)&CnetSys_w + 0x1314))) & 0xFFFF, &lobby_func_005AEA50);
            return;
        }
        (*(u8 *)((u8 *)(arg0) + 0x23)) = (u8) ((*(u8 *)((u8 *)(arg0) + 0x23)) | 4);
    }
}

