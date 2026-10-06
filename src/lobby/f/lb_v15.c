/* lb_v15 - hotel/guild help line 0x005CBF30-0x005CC584: Lb_put_help (room price lines, rule message, then lb_put_sprite). */
#include "lobby_f.h"
extern char D_6EAE5A[];
extern char D_6EAE78[];
extern u8 room_price[];
extern u8 lb_rule_msg_etc[];
extern char lit_427_00664C20[];
extern char lit_428_00664C30[];
extern char lit_430_00664C40[];
void Lb_put_new_mail();
int Lb_check_hotel();
void Lb_put_gold();
void lb_put_sprite();
int strlen();
void Lb_put_help(a)
u8 *a;
{
    PLW *pl = &player_work[*(u8 *)0x3F34C1];
    switch (lb_sys.x68) {
    case 0:
    case 0x27:
    case 7:
    case 8:
        break;
    default:
        return;
    }
    if (Online_ck() == 1) {
        Lb_put_new_mail(0x1A2, 0x1C);
    }
    font_set_stack_no(F(int, a, 0x18));
    font_set_palette(0);
    flfntSetSize(0x14, 0x14);
    if ((s8)SoftKeyboard_alive_check() == 0) {
        flfntSetSize(0x14, 0x14);
        if (D_6EAE5A[0] != 0 || lb_sys.x68 == 8) {
            if (F(u8 *, pl, 0x878) != 0) {
                switch (F(u16, F(u8 *, pl, 0x878), 2)) {
                case 0x13:
                    if (PLU8(pl, 0x14) == 1) {
                        return;
                    }
                    switch (Lb_check_hotel(0x52)) {
                    case 0:
                        D_6EAE5A[0] = 0;
                        break;
                    case 1:
                    case 3:
                        flfntSetSize(0x14, 0x14);
                        flfntLocate((s16)(strlen(D_6EAE5A) * 10 + 0x50), 0x1F);
                        font_print(lit_427_00664C20, F(char *, room_price, 4));
                    case 2:
                        flfntSetSize(0x14, 0x14);
                        flfntLocate(0x30, 0x1F);
                        font_print(lit_428_00664C30, D_6EAE5A);
                        Lb_put_gold();
                    }
                    switch (Lb_check_hotel(0x53)) {
                    case 0:
                        D_6EAE78[0] = 0;
                        break;
                    case 1:
                    case 3:
                        flfntSetSize(0x14, 0x14);
                        flfntLocate((s16)(strlen(D_6EAE78) * 10 + 0x50), 0x39);
                        font_print(lit_427_00664C20, F(char *, room_price, 8));
                    case 2:
                        flfntSetSize(0x14, 0x14);
                        flfntLocate(0x30, 0x39);
                        font_print(lit_428_00664C30, D_6EAE78);
                    }
                    break;
                case 0x14:
                    if (PLU8(pl, 0x14) == 1) {
                        return;
                    }
                    switch (Lb_check_hotel(0x54)) {
                    case 0:
                        D_6EAE5A[0] = 0;
                        break;
                    case 1:
                    case 3:
                        flfntLocate((s16)(strlen(D_6EAE5A) * 10 + 0x50), 0x1F);
                        flfntSetSize(0x14, 0x14);
                        font_print(lit_427_00664C20, F(char *, room_price, 0xC));
                    case 2:
                        flfntLocate(0x30, 0x1F);
                        flfntSetSize(0x14, 0x14);
                        font_print(lit_428_00664C30, D_6EAE5A);
                        Lb_put_gold();
                    }
                    switch (Lb_check_hotel(0x55)) {
                    case 0:
                        D_6EAE78[0] = 0;
                        break;
                    case 1:
                    case 3:
                        flfntLocate((s16)(strlen(D_6EAE78) * 10 + 0x50), 0x39);
                        flfntSetSize(0x14, 0x14);
                        font_print(lit_427_00664C20, F(char *, room_price, 0x10));
                    case 2:
                        flfntLocate(0x30, 0x39);
                        flfntSetSize(0x14, 0x14);
                        font_print(lit_428_00664C30, D_6EAE78);
                    }
                    break;
                case 0x12:
                    if (PLU8(pl, 0x14) == 1) {
                        return;
                    }
                    if (Lb_check_hotel(0x51) == 0) {
                        D_6EAE78[0] = 0;
                        return;
                    }
                    flfntSetSize(0x14, 0x14);
                default:
                    flfntLocate(0x30, 0x1F);
                    if (lb_sys.x68 == 8 || lb_sys.x68 == 0x28) {
                        if (F(u16, *(u8 **)((int)pl + 0x878), 2) != 6 || F(u8, cw, 0x32C5) != 0) {
                            if (PLU8(pl, 0x14) == 1) {
                                return;
                            }
                            font_set_palette(6);
                            font_print(lit_429_00664C38, F(char *, lb_rule_msg_etc, 0x1C));
                        } else {
                            font_set_palette(0);
                            font_print(lit_429_00664C38, F(char *, lb_rule_msg_etc, 0x2C));
                        }
                    } else {
                        if (PLU8(pl, 0x14) == 1) {
                            return;
                        }
                        font_print(lit_428_00664C30, D_6EAE5A);
                    }
                    sprintf(D_6EAE78, lit_430_00664C40);
                }
            } else if (lb_sys.x68 == 8) {
                flfntLocate(0x30, 0x1F);
                font_set_palette(6);
                font_print(lit_429_00664C38, F(char *, lb_rule_msg_etc, 0x1C));
            }
        }
        lb_put_sprite();
    }
}
