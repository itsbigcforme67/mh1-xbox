/* G online (PC): lobby_func_0059F6C0, written from Monster Hunter G's code at lobby 0x59F6C0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

void * strcpy();
extern u8 g_main_2EFAE8;
extern u8 g_main_2EFAE8;
extern s32 g_lb_654E00;

void lobby_func_0059F6C0(void *arg0) {
    (*(s32 *)&g_main_2EFAE8) = 0xB000240A;
    strcpy(&g_lb_654E00, arg0);
}

