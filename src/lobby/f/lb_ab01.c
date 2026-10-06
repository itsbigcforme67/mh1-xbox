/* lb_ab01 - lobby 0x005CD6D0-0x005CD7CC: Lb_load_player_all (load every member model once, then the four NPC models; lb_sys.x70 = done).
   The flag pointers (c = cw + 0x2C06 / cw + i + 0x2BFE) are separate from the compared byte: that gives the original addiu before the lb. Whole file in lb_ab.c. */
#include "lobby_f.h"
extern PLW player_work[];
void Lb_player_load();
void com_motion_load();
void Lbc_connect();
void npc_create_model();

void Lb_load_player_all(void) {
    u32 i;
    u8 *pl;
    s8 *c;
    if (lb_sys.x70 == 0) {
        c = (s8 *)(cw + 0x2C06);
        if (*(s8 *)(cw + 0x2C06) == 0) {
            *c = 1;
            com_motion_load(1);
        }
        i = 0;
        pl = (u8 *)player_work;
        do {
            if (*pl != 0) {
                c = (s8 *)(cw + i + 0x2BFE);
                if (*(s8 *)(cw + i + 0x2BFE) == 0) {
                    *c = 1;
                    Lb_player_load(pl);
                    Lbc_connect();
                }
            }
            i += 1;
            pl += 0xA00;
        } while (i < 8U);
        npc_create_model(0);
        Lbc_connect();
        npc_create_model(1);
        Lbc_connect();
        npc_create_model(2);
        Lbc_connect();
        npc_create_model(3);
        Lbc_connect();
        lb_sys.x70 = 1;
    }
}
