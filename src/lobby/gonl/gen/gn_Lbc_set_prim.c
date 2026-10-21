/* G online (PC): Lbc_set_prim, written from Monster Hunter G's code at lobby 0x57A0A0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 pNet;
extern u8 lb_prim;

void Lbc_set_prim(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    (*(s32 **)((u8 *)((*(void **)&pNet)) + 0)) = &lb_prim;
    (*(s32 *)((u8 *)((*(s32 **)((u8 *)((*(void **)&pNet)) + 0))) + 0x14)) = arg0;
    (*(s32 **)((u8 *)((*(void **)&pNet)) + 4)) = &(*(u8 *)((u8 *)&lb_prim + 0x20));
    (*(s32 *)((u8 *)((*(s32 **)((u8 *)((*(void **)&pNet)) + 4))) + 0x14)) = arg1;
    (*(s32 **)((u8 *)((*(void **)&pNet)) + 8)) = &(*(u8 *)((u8 *)&lb_prim + 0x40));
    (*(s32 *)((u8 *)((*(s32 **)((u8 *)((*(void **)&pNet)) + 8))) + 0x14)) = arg2;
    (*(s32 **)((u8 *)((*(void **)&pNet)) + 0xC)) = &(*(u8 *)((u8 *)&lb_prim + 0x60));
    (*(s32 *)((u8 *)((*(s32 **)((u8 *)((*(void **)&pNet)) + 0xC))) + 0x14)) = arg3;
}

