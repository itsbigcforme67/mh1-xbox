/* lbsnd02 - npc sound request helper sound_req_com (agent B) */
#include "lobby.h"
#include "em.h"
int em_frame_check(EMW *, f32, int);
void Npc_se_req_com();
void sound_req_com(EMW *em, int frame, int se) {
    if (em_frame_check(em, (f32)frame, 0) != 0) {
        Npc_se_req_com(em, se, (u8 *)em + 0xAC, 2);
    }
}
