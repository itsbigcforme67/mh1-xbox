#include "lobby_a.h"
extern char ot5[];
extern char ot6[];
extern char ot7[];
extern char ot2[];
typedef struct { u8 pad0000[0x14]; int x0014; int x0018; int x001C; int x0020; } ARG_lbc_text_lobby_trans_arg0;
void lbc_text_lobby_trans(ARG_lbc_text_lobby_trans_arg0 *arg0) {
    int temp_a1_3;
    int temp_a1_4;
    int temp_a1;
    int temp_a1_2;

    temp_a1 = arg0->x0014;
    if ((temp_a1 != 0) && (F(s32, temp_a1, 0x14) != 0)) {
        add_prim2(&ot5, temp_a1, 0, 1);
    }
    temp_a1_2 = arg0->x0018;
    if ((temp_a1_2 != 0) && (F(s32, temp_a1_2, 0x14) != 0)) {
        add_prim2(&ot6, temp_a1_2, 0, 1);
    }
    temp_a1_3 = arg0->x001C;
    if ((temp_a1_3 != 0) && (F(s32, temp_a1_3, 0x14) != 0)) {
        add_prim2(&ot7, temp_a1_3, 0, 1);
    }
    temp_a1_4 = arg0->x0020;
    if ((temp_a1_4 != 0) && (F(s32, temp_a1_4, 0x14) != 0)) {
        add_prim2(&ot2, temp_a1_4, 0, 0x10);
    }
}
