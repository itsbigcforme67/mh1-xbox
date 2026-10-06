/* lb_by126 - agent B 0x0053D280-0x0053D3C8: Lb_put_job_limit (shop: warning text when the hunter's job cannot equip the item). */
#include "lobby_s.h"
extern char User_data[];
extern char lit_543_00655878[];
extern char *shop_warning[];
typedef struct { u8 x0; s8 kind; s16 id; u8 x4[4]; } EQB;
void Lb_put_job_limit(u16 kind, s16 id) {
    EQB q;
    u8 b;
    u8 v;

    if (kind != 7 && kind != 6) {
        q.kind = kind;
        q.id = id;
        b = Get_equip_bit(User_data, &q);
        v = 0xFF;
        if ((b & 3) != 3) {
            if (b & 1) {
                switch (b & 0xC) {
                case 4:
                    v = 7;
                    break;
                case 8:
                    v = 5;
                    break;
                default:
                    v = 3;
                    break;
                }
            } else {
                switch (b & 0xC) {
                case 4:
                    v = 6;
                    break;
                case 8:
                    v = 4;
                    break;
                default:
                    v = 2;
                    break;
                }
            }
        } else if ((b & 0xC) != 0xC) {
            if (b & 4) {
                v = 1;
            } else {
                v = 0;
            }
        }
        if (v != 0xFF) {
            font_set_palette(5);
            flfntLocate(0x1D0, 0x11B);
            font_print(lit_543_00655878, shop_warning[v]);
        }
    }
}
