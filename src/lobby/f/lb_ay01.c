/* lb_ay01 - item box yes/no 0x0060B110-0x0060B18C: yes_no_select. Whole file in lb_ay.c. */
#include "lobby_f.h"
typedef struct ITEMSLOT { u16 id; s16 num; } ITEMSLOT;
extern u8 *ib;
extern u8 User_data[];
extern ITEMSLOT D_3C7184[];
extern u8 D_3C7186[];
extern u8 D_3396D3[];
extern u8 D_3396D5[];
void se_req();
u8 *Get_equip_data_ptr();


void yes_no_select(u16 pad) {
    u8 *t;
    u8 *q;
    t = ib;
    q = t + 0x21;
    if (*q == 0) {
        if (pad & 0x400) {
            *q = 1;
            se_req(7, 0x16, 0);
        }
    } else if (pad & 0x800) {
        *q = 0;
        se_req(7, 0x16, 0);
    }
}
