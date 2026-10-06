/* lbsnd03 - npc sound request helper ashi_sd_req_005C4980 (agent B) */
#include "lobby.h"
#include "em.h"
int em_frame_check();
void Npc_se_req();
int ran_suu();
void ashi_sd_req_005C4980(EMW *em) {
    u8 *npc;

    npc = (u8 *)em + 0x444;
    if (em_frame_check(em, 0) != 0) {
        Npc_se_req(em, *(s8 *)(npc + 0x2C) + ((u16)ran_suu(1) & 1), (u8 *)em + 0xAC, 2);
    }
}
