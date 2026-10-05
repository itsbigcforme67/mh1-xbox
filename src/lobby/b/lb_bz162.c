/* lb_bz162 - lobby UI/client 0x005C3A40-0x005C3AD8: lb_npc_init_sub (first drafted by tools/lbauto.py). */
#include "lobby_b.h"
extern int lb_npc_prog_tbl[];
extern int lb_npc_trans[];

void lb_npc_init_sub(u8 *em) {
    s16 id;

    *(int *)(em + 0x3CC) = lb_npc_prog_tbl[em[2]];
    (**(void (***)())(em + 0x3CC))();
    if (em[1] != 0) {
        *(s16 *)(em + 0x568) = get_prim();
        id = *(s16 *)(em + 0x568);
        if (id != -1) {
            *(void **)(em + 0x564) = get_prim_ptr(id);
            *(u8 **)(*(u8 **)(em + 0x564) + 0x18) = em;
            *(int **)(*(u8 **)(em + 0x564) + 0x14) = lb_npc_trans;
        }
    }
}
