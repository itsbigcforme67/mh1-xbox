/* lb_by23 - agent B promoted near-match 0x005C4CF0-0x005C4D3C: lb_npc_ef_move (first drafted by tools/lbauto.py). */
#include "lobby_a.h"
typedef struct { u8 pad[0x2A]; s8 eff; } LBEFW;
void ef_move_sub_005C49F0();

void lb_npc_ef_move(u8 *em) {
    LBEFW *w = (LBEFW *)(em + 0x444);

    switch (w->eff) {
    case 0:
        w->eff++;
        break;
    case 1:
        ef_move_sub_005C49F0(em, w);
        break;
    }
}
