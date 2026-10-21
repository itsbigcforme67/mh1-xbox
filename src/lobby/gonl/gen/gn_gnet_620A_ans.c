/* HAND-FIXED (the id store is a halfword). G online (PC): gnet_620A_ans, written from Monster Hunter G's code at lobby 0x5E3940 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 _cnet_Return_CallBack();
s32 lobby_func_005E3BE0();
extern u8 CnetSys_w;
s32 strcpy(); /* extern */
extern u8 CnetSys_g;

void gnet_620A_ans(void) {
    u8 g_frame[0x120] __attribute__((aligned(16)));   /* G's stack frame */
#define sp10 (*(s32 *)(g_frame + 0x10))
#define sp11E (*(u16 *)(g_frame + 0x11E))
    u8 *temp_v1;   /* (s32 *) */
    s32 temp_a3;

    if ((*(s8 *)((u8 *)&CnetSys_w + 0xFEC)) == 0) {
        lobby_func_005E3BE0((s32) &sp11E, (s32) &sp10);
        temp_a3 = sp11E * 0x164;
        (*(s16 *)((u8 *)&(*(u8 *)((u8 *)&CnetSys_w + 0x426C)) + temp_a3)) = sp11E;   /* HAND-FIXED: sh */
        strcpy((u8 *)&CnetSys_g + ((sp11E - 1) * 0x164) + 0x44D2, &sp10, &(*(u8 *)((u8 *)&CnetSys_w + 0x426C)), temp_a3, sp11E);
        temp_v1 = (u8 *)&(*(u8 *)((u8 *)&CnetSys_w + 0x4264)) + (sp11E * 0x164);
        (*(s32 *)temp_v1) |= 8;
    }
    _cnet_Return_CallBack(0x13);
}

