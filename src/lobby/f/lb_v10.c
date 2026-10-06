/* lb_v10 - lobby member stage change 0x005C5980-0x005C5AB4: lb_set_pl_stage (stores the member's stage, drops a held target, logs a leave in chat). */
#include "lobby_f.h"
extern char lit_476_0065ED10[];
void lb_set_pl_stage(id, stg)
int id;
u8 *stg;
{
    char buf[0x120];
    u8 uid;
    PLW *pl;
    PLW *me;
    uid = id;
    pl = &player_work[uid];
    me = &player_work[game_w.master];
    pl->stg = *stg;
    PLU8(pl, 0x8EC) = 0;
    PLU8(pl, 0x90F) = 0;
    if (me->x3B0 != 0 && ((u16 *)me->x3B0)[6] == uid && ((u8 *)me->x3B0)[0x1E] == 0) {
        me->x3B0 = 0;
    }
    if (*stg == 0) {
        if (CW8(0x35D5) != 0) {
            if (lbCommer[uid].name[0] != 0) {
                memset(buf, 0, 0x120);
                buf[0x11F] = 6;
                buf[0x11E] = 6;
                buf[0x11D] = 6;
                sprintf(buf + 0x1C, lit_476_0065ED10, lbCommer[uid].name);
                Chat_log_add(0, buf);
            }
        }
        Lb_clearChatMember((s8)id);
    }
}
