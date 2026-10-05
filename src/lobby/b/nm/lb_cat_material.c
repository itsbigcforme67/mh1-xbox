#include "lobby_a.h"
typedef struct { u8 pad0000[0x10]; s32 x0010; } ARG_lb_cat_material_arg1;

void lb_cat_material(s32 arg0, ARG_lb_cat_material_arg1 *arg1, s32 arg2) {
    u16 temp_v1;

    if (arg2 == 5) {
        arg1->x0010 = 0;
        return;
    }
    temp_v1 = F(u16, (arg0 + 0x444), 0x28);
    switch (temp_v1) {                              /* irregular */
    case 0:
        if ((arg2 != 4) && (arg2 != 3) && (arg2 != 2)) {
            return;
        }
        arg1->x0010 = 0;
        return;
    case 1:
        if ((arg2 != 4) && (arg2 != 3) && (arg2 != 1)) {
            return;
        }
        arg1->x0010 = 0;
        return;
    case 2:
        if ((arg2 != 4) && (arg2 != 2) && (arg2 != 1)) {
            return;
        }
        arg1->x0010 = 0;
        return;
    }
}
