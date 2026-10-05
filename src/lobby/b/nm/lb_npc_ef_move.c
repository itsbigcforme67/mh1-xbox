#include "lobby_a.h"

void lb_npc_ef_move(int arg0) {
    s8 temp_a2;
    int temp_a1;

    temp_a2 = F(s8, arg0, 0x46E);
    temp_a1 = arg0 + 0x444;
    switch (temp_a2) {                              /* irregular */
    case 0:
        F(s8, temp_a1, 0x2A) = (s8) (temp_a2 + 1);
        return;
    case 1:
        ef_move_sub_005C49F0(temp_a1, temp_a2);
    }
}
