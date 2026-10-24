/* G online (PC): gnet_620A_ans, written from Monster Hunter G's code at lobby 0x5E3940 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 _cnet_Return_CallBack();
s32 lobby_func_005E3BE0();
void * strcpy();
extern u8 CnetSys_w;
extern s32 CnetSys_g;

void gnet_620A_ans(s32 unused_a0, s32 unused_a1, s32 arg2, s32 arg3) {
    u8 g_frame[0x120] __attribute__((aligned(16)));   /* G's stack frame */
#define sp10 (*(s32 *)(g_frame + 0x10))
#define sp11E (*(u16 *)(g_frame + 0x11E))
    u8 *temp_v1;   /* (s32 *) */

    if ((*(s8 *)((u8 *)&CnetSys_w + 0xFEC)) == 0) {
        lobby_func_005E3BE0(&sp11E, &sp10, arg2, arg3);
        (*(u16 *)((u8 *)&(*(u16 *)((u8 *)&CnetSys_w + 0x426C)) + (sp11E * 0x164))) = sp11E;
        strcpy((u8 *)&CnetSys_g + ((sp11E - 1) * 0x164) + 0x44D2, &sp10);
        temp_v1 = (u8 *)&(*(s32 *)((u8 *)&CnetSys_w + 0x4264)) + (sp11E * 0x164);
        (*(s32 *)temp_v1) |= 8;
    }
    _cnet_Return_CallBack(0x13, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
}

