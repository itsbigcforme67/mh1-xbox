/* G online (PC): plaza_checkMyStatus, written from Monster Hunter G's code at lobby 0x570B30 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Eft25_set();
#ifdef __MWERKS__
s32 Get_sw2();
#else
unsigned short Get_sw2();   /* the real return type (tools/check_kr_returns.py) */
#endif
#ifdef __MWERKS__
s32 Get_sw_on2();
#else
unsigned short Get_sw_on2();   /* the real return type (tools/check_kr_returns.py) */
#endif
s32 tl_exit_sub_menu();
extern u8 pNet;
void plaza_checkMyStatus(void) {
    s32 temp_s0;
    u8 temp_a0;
    u8 *temp_a0_2;
    u8 *temp_a1;

    temp_a1 = (*(void **)&pNet);
    temp_a0 = (*(u8 *)((u8 *)(temp_a1) + 0x23));
    temp_s0 = Get_sw2(0) & 0xFFFF;
    switch (temp_a0) {                              /* irregular */
    case 0:
        (*(s16 *)((u8 *)(temp_a1) + 0x1C)) = 0;
        temp_a0_2 = (*(void **)&pNet);
        (*(u8 *)((u8 *)(temp_a0_2) + 0x23)) = (u8) ((*(u8 *)((u8 *)(temp_a0_2) + 0x23)) + 1);
        return;
    case 1:
        (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x20)) = Get_sw_on2(0);
        if (temp_s0 & 0xFFFF & 0x40) {
            tl_exit_sub_menu(0, M2C_ERROR(/* Read from unset register $a1 */), M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
            return;
        }
        (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x1C)) = Eft25_set((s32) (*(s16 *)((u8 *)((*(void **)&pNet)) + 0x1C)), 3, M2C_ERROR(/* Read from unset register $a2 */), M2C_ERROR(/* Read from unset register $a3 */));
        return;
    }
}

