/* G online (PC): lobby_func_005E3050, written from Monster Hunter G's code at lobby 0x5E3050 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

extern u8 CnetSys_w;

void lobby_func_005E3050(void *arg0) {
    u8 g_frame[0x20] __attribute__((aligned(16)));   /* G's stack frame */
#define sp18 (*(s8 *)(g_frame + 0x18))
#define sp19 (*(s8 *)(g_frame + 0x19))
    u8 temp_a0;
    u8 temp_a2;

    temp_a0 = (*(u8 *)((u8 *)(arg0) + 0x23));
    switch (temp_a0) {                              /* irregular */
    case 1:
        (*(u8 *)((u8 *)(arg0) + 0x22)) = (u8) ((*(u8 *)((u8 *)(arg0) + 0x22)) + 1);
        (*(u8 *)((u8 *)(arg0) + 0x23)) = 0U;
        temp_a2 = (*(u8 *)((u8 *)(arg0) + 0x24));
        sp18 = 2;
        sp19 = 0xA;
        (*(s16 *)((u8 *)&CnetSys_w + 0x1032)) = (*(u16 *)((u8 *)&CnetSys_w + 0x43B4)) * temp_a2;
        (*(s32 (**)(s64, s8 *, u8, void *))((u8 *)(arg0) + 4))(*(s64 *)&sp18, &sp18, temp_a2, arg0);
        return;
    case 2:
        (*(s8 *)((u8 *)(arg0) + 0x21)) = 2;
        (*(u8 *)((u8 *)(arg0) + 0x22)) = 0U;
        return;
    }
}

