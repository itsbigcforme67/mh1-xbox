/* G online (PC): lobby_func_0056AFF0, written from Monster Hunter G's code at lobby 0x56AFF0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s64 lobby_func_00569320();
s32 lobby_func_0059F740();
extern u8 PlazaInfo;
extern u8 pNet;
extern u8 ClassInfo;

void lobby_func_0056AFF0(void) {
    u32 temp_s0;

    temp_s0 = ClassInfo - 1;
    (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x27)) = (u8) ((*(s32 *)((u8 *)&(*(u8 *)((u8 *)&PlazaInfo + 0x5B)) + (temp_s0 * 0x15C))) - 0x31);
    lobby_func_0059F740();
    (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x1C)) = (s16) (((ClassInfo - 1) / 7) + (temp_s0 >> 0x1F));
    (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2B)) = (s8) ((s32) temp_s0 % 7);
    (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x1E)) = lobby_func_00569320((s64) (*(s16 *)((u8 *)&ClassInfo + 0x2)), 7);
}

