#include "lobby_f.h"
extern u8 * pNet;
s32 Lb_pl_status_m(s32 arg0) {
    s32 temp_v1;
    void *temp_v1_2;

    temp_v1 = arg0 & 0xFFFF;
    if (temp_v1 & 0x40) {
        cnWrap_SoundRequest(3);
        return 1;
    }
    if (temp_v1 & 0xC00) {
        temp_v1_2 = pNet;
        F(u8, temp_v1_2, 3) = (u8) (F(u8, temp_v1_2, 3) ^ 1);
        cnWrap_SoundRequest(1);
    }
    if (lb_player[F(u8, pNet, 8)].x24 == 0) {
        cnWrap_SoundRequest(3);
        return 1;
    }
    return 0;
}
