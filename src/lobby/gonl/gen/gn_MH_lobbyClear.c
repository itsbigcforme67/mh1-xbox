/* G online (PC): MH_lobbyClear, written from Monster Hunter G's code at lobby 0x57D230 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Lb_clearChatList();
void * memset();
extern u8 BsProxyUrlstr;
extern u8 cw;
extern u8 lb_gbyte_2EF2A4;
extern u8 lb_sys;
extern s32 lbCommer;

void MH_lobbyClear(void) {
    memset((*(void **)&cw) + 0x38C, 0, 0x398);
    memset((*(void **)&cw) + 0xB64, 0, 0xA);
    memset(&lbCommer, 0, 0x398);
    (*(s8 *)((u8 *)((*(void **)&cw)) + 3)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xFA5F)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB6E)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xF751)) = 0;
    (*(s8 *)((u8 *)((*(void **)&cw)) + 0xB6F)) = 0;
    memset(&(*(s32 *)((u8 *)&BsProxyUrlstr + 0x110)), 0, 0xC0);
    Lb_clearChatList(M2C_ERROR(/* Read from unset register $a0 */), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
    (*(u8 *)((u8 *)&lb_sys + 0x93)) = (*(u8 *)&lb_gbyte_2EF2A4);
}

