#include "lobby_s.h"
extern s32 armorIndex;
extern s8 randTblNo;
extern char shop_process2_help[];
extern char buki_sei_tbl[];
extern char shop_process2_help[];
extern char D_3C7004[];
extern char shop_process2_help[];
extern char User_data[];
extern char lb_armor2_sel2Prog[];
extern char Lb_put_shopYesNo[];
extern char shop_process2_help[];
s32 lb_process_select(void) {
    int sp18;
    s32 temp_a0;
    s32 temp_a2;
    s8 var_v0;
    u16 temp_a0_2;
    u16 temp_a3;
    int temp_a1;
    int temp_a2_2;
    int temp_v1;
    int temp_v1_2;

    var_v0 = 1;
    switch (lbShop.mode) {       /* switch 1; irregular */
    case 0:                                         /* switch 1 */
        switch (lbShop.x1A) {   /* switch 2; irregular */
        case 0:                                     /* switch 2 */
            temp_a0 = lbShop.cur * 0x28;
            if (*(s32 *)((int)&shopList + 0x24 + temp_a0) == 0) {
                lbShop.help = F(s32, &shop_process2_help, 0x10);
                temp_a3 = *(s32 *)((int)&shopList + 0x26 + temp_a0);
                lbShop.x84 = 2;
                temp_a2 = temp_a3 * 0x18;
                armorIndex = lbShop.cur;
                lbShop.x6E = 0;
                temp_a0_2 = F(u16, ((int)&buki_sei_tbl + temp_a2), 4);
                switch (temp_a0_2) {                /* switch 3; irregular */
                case 0x73:                          /* switch 3 */
                    randTblNo = 0;
                    break;
                case 0x74:                          /* switch 3 */
block_20:
                    randTblNo = var_v0;
                    break;
                case 0x76:                          /* switch 3 */
                    randTblNo = 2;
                    break;
                case 0x75:                          /* switch 3 */
                    var_v0 = 3;
                    goto block_20;
                case 0x72:                          /* switch 3 */
                    var_v0 = 4;
                    goto block_20;
                }
                lbShop.x1C = 0;
                cnWrap_SoundRequest(0, 2);
                return 1;
            }
            return 0;
        case 1:                                     /* switch 2 */
            if (*(s32 *)((int)&shopList + 0x24 + (lbShop.cur * 0x28)) == 0) {
                armorIndex = lbShop.cur;
                lbShop.x84 = 0;
                lbShop.help = F(s32, &shop_process2_help, 0xC);
                lb_process_make_kyoukaList(lbShop.cur);
                lbShop.x70 = 0;
                cnWrap_SoundRequest(0);
                lbShop.x1C = 0;
                if (lbShop.x1A == 1) {
                    temp_a2_2 = (int)&lbShop + 0x54;
                    temp_v1 = (int)&D_3C7004 + (armorIndex * 6);
                    F(s16, &sp18, 0) = (s16) F(s16, temp_v1, 0);
                    F(s16, &sp18, 2) = (s16) F(s16, temp_v1, 2);
                    F(s16, &sp18, 4) = (s16) F(s16, temp_v1, 4);
                    F(s16, &lbShop, 0x54) = (s16) F(s16, &sp18, 0);
                    F(s16, temp_a2_2, 2) = (s16) F(s16, &sp18, 2);
                    F(s16, temp_a2_2, 4) = (s16) F(s16, &sp18, 4);
                    if (F(u8, &lbShop, 0x55) == 7) {
                        temp_a1 = (int)&lbShop + 0x5A;
                        F(s16, &lbShop, 0x5A) = (s16) F(s16, &sp18, 0);
                        F(s16, temp_a1, 2) = (s16) F(s16, &sp18, 2);
                        F(s16, temp_a1, 4) = (s16) F(s16, &sp18, 4);
                    }
                }
                return 1;
            }
            return 0;
        }
        break;
    case 1:                                         /* switch 1 */
        if (*(s32 *)((int)&shopList + 0x24 + (lbShop.cur * 0x28)) == 0) {
            lbShop.help = F(s32, &shop_process2_help, 0x10);
            cnWrap_SoundRequest(0);
            lbShop.x1C = 0;
            lbShop.x84 = 2;
            F(s16, &lbShop, 0x5A) = 1;
            lbShop.x6E = 0;
            armorIndex = lbShop.cur;
            temp_v1_2 = (int)lbShop.tbl + (lbShop.cur * 8);
            F(s8, &lbShop, 0x5B) = (s8) F(s32, temp_v1_2, 0);
            F(s16, &lbShop, 0x5C) = (s16) F(s32, temp_v1_2, 4);
            F(s16, &lbShop, 0x5E) = 0;
            lbShop.f30 = (int (*)())0;
            lbShop.x18 = 0;
            if (Equip_ok_ck(&User_data, (int)&lbShop + 0x5A) == 0) {
                lbShop.f30 = (int (*)())lb_armor2_sel2Prog;
                lbShop.f40 = (void (*)())Lb_put_shopYesNo;
                lbShop.x18 = 1;
                lbShop.x78 = 1;
                lbShop.help = F(s32, &shop_process2_help, 0x24);
            }
            return 1;
        }
        return 0;
    default:                                        /* switch 1 */
        return 0;
    }
}
