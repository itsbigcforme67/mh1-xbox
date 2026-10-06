/* lbsnd01 - npc sound request helper sound_call_005C48C0 (agent B) */
#include "lobby.h"
#include "em.h"
int em_frame_check(EMW *, f32, int);
void Npc_se_req();
void sound_call_005C48C0(EMW *em, int frame, int se) {
    if (em_frame_check(em, (f32)frame, 0) != 0) {
        Npc_se_req(em, se, (u8 *)em + 0xAC, 2);
    }
}
