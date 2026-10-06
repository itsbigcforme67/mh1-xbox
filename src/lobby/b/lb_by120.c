/* lb_by120 - agent B 0x005C38D0-0x005C395C: lb_normal_material (NPC model material colour override table). */
#include "lobby_a.h"
typedef struct { s16 id; s16 mat; s32 col; } NMAT;
void lb_normal_material(u8 *em, s32 arg1, s32 arg2, int arg3) {
    NMAT *p = (NMAT *)F(int, em, 0x458);
    if (p != 0) {
        do {
            if (p->id == -1) break;
            if (p->id == (s16)arg3 && p->mat == arg2) {
                SetDiffuseColor(arg1 + 4, p->col);
            }
            p++;
        } while (p != 0);
    }
}
