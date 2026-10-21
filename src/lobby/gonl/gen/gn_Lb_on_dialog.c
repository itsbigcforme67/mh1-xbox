/* G online (PC): Lb_on_dialog, written from Monster Hunter G's code at lobby 0x5795B0 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 DispNameAndIDonDialog();
s32 lobby_func_005790B0();
extern u8 g_main_2EFBAC;
extern u8 cw;
extern u8 g_main_2EFBAC;
extern u8 pNet;
extern u8 tl_member_buff;
s32 Online_ck();                                    /* extern */
extern s32 g_main_302290;
extern s32 g_main_302CB0;
extern s32 lb_sys;

void Lb_on_dialog(void) {
    u8 temp_a0;
    u8 *temp_a1;
    u8 *temp_a2;
    u8 *temp_a2_2;
    u8 *temp_v0;
    u8 *temp_v0_2;

    if ((Online_ck() != 0) && (((*(u8 *)((u8 *)((*(void **)&cw)) + 0xFA61)) == 0) || (lb_sys == 0x10) || (lb_sys == 5))) {
        if ((Online_ck() == 1) && ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x615)) == 1)) {
            DispNameAndIDonDialog(0x98, (*(void **)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x600)) + 8, (*(void **)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x600)));
            return;
        }
        temp_a0 = (*(u8 *)((u8 *)((*(void **)&pNet)) + 0x28));
        switch (temp_a0) {                          /* switch 1; irregular */
        case 3:                                     /* switch 1 */
            switch ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C))) { /* switch 2; irregular */
            case 14:                                /* switch 2 */
                temp_a2 = (u8 *)&g_main_302CB0 + (((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60F)) + ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x604)) * 7)) * 0x30);
                DispNameAndIDonDialog(0x98, temp_a2 + 8, temp_a2);
                return;
            case 17:                                /* switch 2 */
                temp_v0 = (*(void **)&cw);
                DispNameAndIDonDialog(0x98, temp_v0 + 0xEEF, temp_v0 + 0xEE7);
                return;
            case 12:                                /* switch 2 */
                if ((*(u8 *)((u8 *)((*(void **)&g_main_2EFBAC)) + 0x233)) == 0xA) {
                    lobby_func_005790B0();
                    return;
                }
                break;
            }
            break;
        case 14:                                    /* switch 1 */
            switch ((*(s8 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60C))) { /* switch 3; irregular */
            case 4:                                 /* switch 3 */
                temp_a2_2 = (u8 *)&g_main_302290 + ((*(s16 *)((u8 *)(&(*(u8 *)((u8 *)&tl_member_buff + 0x1398))) + 0x60A)) * 0x18);
                DispNameAndIDonDialog(0x98, temp_a2_2 + 8, temp_a2_2);
                return;
            case 6:                                 /* switch 3 */
                temp_v0_2 = (*(void **)&cw);
                DispNameAndIDonDialog(0x98, temp_v0_2 + 0xEEF, temp_v0_2 + 0xEE7);
                return;
            }
            break;
        case 4:                                     /* switch 1 */
            temp_a1 = (*(void **)&g_main_2EFBAC);
            if (((*(u8 *)((u8 *)(temp_a1) + 0x230)) == 2) && ((*(u8 *)((u8 *)(temp_a1) + 0x233)) == 0xA)) {
                lobby_func_005790B0();
            }
            break;
        }
    }
}

