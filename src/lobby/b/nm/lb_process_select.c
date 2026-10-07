#include "lobby_s.h"
typedef struct { s16 a, b, c; } S3;
extern s32 armorIndex;
extern s8 randTblNo;
extern s32 shop_process2_help[];
extern u8 buki_sei_tbl[];
extern u8 D_3C7004[];
extern char User_data[];
extern char lb_armor2_sel2Prog[];
extern char Lb_put_shopYesNo[];
void lb_process_make_kyoukaList();
s32 lb_process_select(void) {
    S3 q;
    int c;
    u16 *sk;
    u8 *e;

    switch (lbShop.mode) {
    case 0:
        switch (lbShop.x1A) {
        case 0:
            c = lbShop.cur;
            sk = (u16 *)&((u8 *)shopList)[0x26];
            if (((s16 *)&((u8 *)shopList)[0x24])[c * 20] == 0) {
                lbShop.help = shop_process2_help[4];
                lbShop.x84 = 2;
                armorIndex = c;
                lbShop.x6E = 0;
                switch (*(u16 *)(buki_sei_tbl + sk[c * 20] * 0x18 + 4)) {
                case 0x73:
                    randTblNo = 0;
                    break;
                case 0x74:
                    randTblNo = 1;
                    break;
                case 0x76:
                    randTblNo = 2;
                    break;
                case 0x75:
                    randTblNo = 3;
                    break;
                case 0x72:
                    randTblNo = 4;
                    break;
                }
                lbShop.x1C = 0;
                cnWrap_SoundRequest(0, 2);
                return 1;
            }
            return 0;
        case 1:
            c = lbShop.cur;
            if (((s16 *)&((u8 *)shopList)[0x24])[c * 20] == 0) {
                armorIndex = c;
                lbShop.x84 = 0;
                lbShop.help = shop_process2_help[3];
                lb_process_make_kyoukaList(c);
                lbShop.x70 = 0;
                cnWrap_SoundRequest(0);
                lbShop.x1C = 0;
                if (lbShop.x1A == 1) {
                    q = *(S3 *)(D_3C7004 + armorIndex * 6);
                    *(S3 *)&lbShop.x54[0] = q;
                    if (lbShop.x54[1] == 7) {
                        *(S3 *)&lbShop.x5A[0] = q;
                    }
                }
                return 1;
            }
            return 0;
        }
        break;
    case 1:
        c = lbShop.cur;
        if (((s16 *)&((u8 *)shopList)[0x24])[c * 20] == 0) {
            lbShop.help = shop_process2_help[4];
            cnWrap_SoundRequest(0);
            lbShop.x1C = 0;
            lbShop.x84 = 2;
            lbShop.x5A[0] = 1;
            lbShop.x6E = 0;
            armorIndex = lbShop.cur;
            e = (u8 *)lbShop.tbl + lbShop.cur * 8;
            lbShop.x5A[1] = *(s32 *)e;
            *(s16 *)&lbShop.x5A[2] = *(s32 *)(e + 4);
            *(s16 *)&lbShop.x5A[4] = 0;
            lbShop.f30 = 0;
            lbShop.x18 = 0;
            if (Equip_ok_ck(User_data, lbShop.x5A) == 0) {
                lbShop.f30 = (void *)lb_armor2_sel2Prog;
                lbShop.f40 = (void *)Lb_put_shopYesNo;
                lbShop.x18 = 1;
                lbShop.x78 = 1;
                lbShop.help = shop_process2_help[9];
            }
            return 1;
        }
        return 0;
    default:
        return 0;
    }
    return 0;
}
