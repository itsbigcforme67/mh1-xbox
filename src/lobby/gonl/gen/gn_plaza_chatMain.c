/* G online (PC): plaza_chatMain, written from Monster Hunter G's code at lobby 0x5732B0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Get_sw();
#ifdef __MWERKS__
s32 Get_sw_on2();
#else
unsigned short Get_sw_on2();   /* the real return type (tools/check_kr_returns.py) */
#endif
s32 Plaza_chat_move();
extern u8 Psw;
extern u8 pNet;
void plaza_chatMain(void) {
    u8 temp_a0;
    u8 *temp_a0_2;
    u8 *temp_a2;

    (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x20)) = Get_sw(0);
    temp_a2 = (*(void **)&pNet);
    temp_a0 = (*(u8 *)((u8 *)(temp_a2) + 0x23));
    switch (temp_a0) {                              /* irregular */
    case 0:
        (*(u8 *)((u8 *)(temp_a2) + 0x23)) = (u8) (temp_a0 + 1);
        /* fallthrough */
    case 1:
        (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x20)) = Get_sw_on2(0);
        if (Plaza_chat_move((s32) (*(u16 *)((u8 *)&Psw + 0x4)), M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */)) == -1) {
            temp_a0_2 = (*(void **)&pNet);
            (*(u8 *)((u8 *)(temp_a0_2) + 0x23)) = (u8) ((*(u8 *)((u8 *)(temp_a0_2) + 0x23)) + 1);
            return;
        }
        return;
    case 2:
        (*(u8 *)((u8 *)(temp_a2) + 0x23)) = (u8) (temp_a0 + 1);
        return;
    case 3:
        (*(u8 *)((u8 *)(temp_a2) + 0x22)) = (u8) ((*(u8 *)((u8 *)(temp_a2) + 0x22)) - 1);
        (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x23)) = 0U;
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x24)) = 0;
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x25)) = 0;
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2B)) = 0;
        (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x1C)) = 0;
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x32)) = 0;
        (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x20)) = 0;
        (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x1E)) = 0;
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x28)) = 0x10;
        (*(s8 *)((u8 *)((*(void **)&pNet)) + 0x2E)) = 1;
        break;
    }
}

