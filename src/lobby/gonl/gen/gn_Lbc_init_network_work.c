/* G online (PC): Lbc_init_network_work, written from Monster Hunter G's code at lobby 0x57EE00 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Online_ck();
extern u8 cw;
extern u8 pNet;
s32 memset();           /* extern */
extern u8 lb_prim;
extern u8 text_lobby_trans_ot3;

void Lbc_init_network_work(void) {
    memset((*(void **)&pNet), 0, 0x38);
    (*(s32 **)((u8 *)((*(void **)&pNet)) + 0)) = &lb_prim;
    (*(s32 **)((u8 *)((*(void **)&pNet)) + 4)) = &(*(u8 *)((u8 *)&lb_prim + 0x20));
    (*(s32 **)((u8 *)((*(void **)&pNet)) + 8)) = &(*(u8 *)((u8 *)&lb_prim + 0x40));
    (*(s32 **)((u8 *)((*(void **)&pNet)) + 0xC)) = &(*(u8 *)((u8 *)&lb_prim + 0x60));
    (*(s32 **)((u8 *)((*(void **)&pNet)) + 0x10)) = &(*(u8 *)((u8 *)&lb_prim + 0x80));
    (*(s32 **)((u8 *)((*(s32 **)((u8 *)((*(void **)&pNet)) + 0x10))) + 0x14)) = &text_lobby_trans_ot3;
    (*(s32 *)((u8 *)((*(s32 **)((u8 *)((*(void **)&pNet)) + 0))) + 0x18)) = 0;
    (*(s32 *)((u8 *)((*(s32 **)((u8 *)((*(void **)&pNet)) + 4))) + 0x18)) = 1;
    (*(s32 *)((u8 *)((*(s32 **)((u8 *)((*(void **)&pNet)) + 8))) + 0x18)) = 2;
    (*(s32 *)((u8 *)((*(s32 **)((u8 *)((*(void **)&pNet)) + 0xC))) + 0x18)) = 3;
    (*(s32 *)((u8 *)((*(s32 **)((u8 *)((*(void **)&pNet)) + 0x10))) + 0x18)) = 5;
    if (((*(u8 *)((u8 *)((*(void **)&cw)) + 0xFA61)) == 0) && (Online_ck() == 1)) {
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x28)) = 0x10;
    }
}

