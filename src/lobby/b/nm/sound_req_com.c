#include "lobby_a.h"

void sound_req_com(s32 arg0, s32 arg1, int arg2) {
    if (em_frame_check((f32) arg1, 0) != 0) {
        Npc_se_req_com(arg0, arg2, arg0 + 0xAC, 2);
    }
}
