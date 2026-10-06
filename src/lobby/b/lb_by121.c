/* lb_by121 - agent B 0x005C3960-0x005C3A40: lb_cat_material (Felyne cat model: which material parts get cleared per kind). */
#include "lobby_a.h"
typedef struct { u8 pad0[0x10]; s32 x10; } MATB;
void lb_cat_material(u8 *em, MATB *m, s32 mode) {
    u8 *ex = em + 0x444;
    if (mode == 5) {
        m->x10 = 0;
        return;
    }
    switch (*(u16 *)(ex + 0x28)) {
    case 0:
        switch (mode) { case 2: case 3: case 4: m->x10 = 0; break; }
        break;
    case 1:
        switch (mode) { case 1: case 3: case 4: m->x10 = 0; break; }
        break;
    case 2:
        switch (mode) { case 1: case 2: case 4: m->x10 = 0; break; }
        break;
    }
}
