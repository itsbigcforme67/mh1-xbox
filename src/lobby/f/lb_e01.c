/* lb_e01 - lobby members/cockpit/icons 0x005CB310-0x005CB354: Lobby_quest_print. Whole file in lb_e.c. */
#include "lobby_f.h"










#ifdef VERSION_MH2
void flfntSetSize();
void Lobby_quest_print(void) {
    flfntSetSize(0x11, 0x12);
    flfntLocate(0x13B, 0x18E);
    font_print(*(char **)0x80CA98);
}
#else
void Lobby_quest_print(void) {
    font_set_palette(5);
    flfntLocate(0x167, 0x8E);
    font_print_uf(lit_254_00664B00);
    font_set_palette(0);
    lb_put_room_member_005CB220();
}
#endif
