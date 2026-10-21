/* G online (PC): lobby_func_00571AF0, written from Monster Hunter G's code at lobby 0x571AF0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 lobby_func_00571A90();
void * memset();
extern u8 g_main_2EFBAC;
extern u8 g_main_2EFBAC;
void lobby_func_00571AF0(void) {
    (*(s8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x231)) = 0;
    (*(s8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x230)) = 0;
    (*(s8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x233)) = 0;
    (*(s8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x237)) = 0;
    (*(s8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x236)) = 0;
    (*(s8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x239)) = 0;
    memset((*(void **)&g_main_2EFBAC), 0, 0x9A);
    memset((*(void **)&g_main_2EFBAC) + 0x9A, 0, 0x5C);
    memset((*(void **)&g_main_2EFBAC) + 0x1AE, 0, 0x80);
    (*(s8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x23E)) = 1;
    (*(s8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x23D)) = 1;
    (*(s8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x23C)) = 1;
    lobby_func_00571A90();
}

