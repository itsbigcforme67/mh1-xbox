/* G online (PC): join_input_password, written from Monster Hunter G's code at lobby 0x58A310 by tools/mhg_gonl.py (an m2c draft with
 * G's names; docs/network.md 3.12). Not compared with anything; the G online PC build links it (ONLINE=1
 * tools/build_pc_mhg.sh). */
#include "../gonl.h"

s32 Get_sw();
#ifdef __MWERKS__
s32 SoftKeyboard_move();
#else
signed char SoftKeyboard_move();   /* the real return type (tools/check_kr_returns.py) */
#endif
s32 func_00220F40();
extern u8 g_main_2EFABC;
extern u8 D_0035EFEB;
extern u8 Psw;
extern u8 g_main_2EFABC;
extern u8 lb_sys;
extern u8 pNet;
#ifdef __MWERKS__
s32 Get_kb_input();                             /* extern */
#else
unsigned char Get_kb_input();                             /* extern */   /* the real return type (tools/check_kr_returns.py) */
#endif
s32 Lb_put_set01();                      /* extern */
void *Lbs_GetRoomInfo();                          /* extern */
s32 SoftKeyboard_exit();                        /* extern */
void SoftKeyboard_pos_set(f32, s16);

s32 join_input_password(void *arg0) {
    u8 temp_a0;
    u8 *temp_s0;

    Get_sw(0);
    Get_kb_input();
    temp_s0 = Lbs_GetRoomInfo((*(u8 *)((u8 *)((*(void **)&pNet)) + 0x28)));
    switch ((*(s8 *)((u8 *)&lb_sys + 0x1D))) {                           /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        (*(s8 *)((u8 *)&lb_sys + 0x1D)) += 1;
        SoftKeyboard_pos_set(80.0f, 0x3A);
        func_00220F40(0, 6, 8, arg0);
        (*(s8 *)&D_0035EFEB) = 0;
block_22:
    default:                                        /* switch 1 */
        return 2;
    case 1:                                         /* switch 1 */
        if ((((s8) (SoftKeyboard_move(arg0, (s32) (*(s16 *)&Psw), (s32) (*(s16 *)((u8 *)&Psw + 0x4)))))) != 0) {
            (*(s8 *)((u8 *)&lb_sys + 0x1D)) += 1;
            goto block_22;
        }
        temp_a0 = (*(u8 *)((u8 *)(temp_s0) + 0x16));
        switch (temp_a0) {                          /* switch 2; irregular */
        case 3:                                     /* switch 2 */
            if (((s32) (*(u16 *)((u8 *)(temp_s0) + 6)) >= (s32) (*(u16 *)((u8 *)(temp_s0) + 8))) || ((*(s8 *)&g_main_2EFABC) != 0)) {
                if (temp_a0 != 4) {
                default:                            /* switch 2 */
                    Lb_put_set01(8);
                } else {
                case 4:                             /* switch 2 */
                    Lb_put_set01(9);
                }
                SoftKeyboard_exit();
                (*(s8 *)((u8 *)&lb_sys + 0x1D)) = 0;
                (*(u8 *)&D_0035EFEB) = 1;
                return 3;
            }
            goto block_22;
        }
        break;
    case 2:                                         /* switch 1 */
        SoftKeyboard_exit();
        (*(s8 *)((u8 *)&lb_sys + 0x1D)) = 0;
        (*(u8 *)&D_0035EFEB) = 1;
        return 0;
    }
}

