/* lb_e09 - lobby member status 0x005CB780-0x005CB82C: Lb_PlStatusSet. Whole file in lb_e.c. */
#include "lobby_f.h"

void Lb_PlStatusSet(int id)
{
    s16 *new_var;
    s16 *mini;
    u8 *pl;
    int uid;
    s16 a2;
    s16 new_var2;
    s16 a1;
    mini = (s16 *)GetAdrsMiniData();
    uid = id & 0xFF;
    a2 = mini[4];
    a1 = mini[5];
    pl = (u8 *)(&player_work[uid]);
    new_var2 = mini[6];
    *((s16 *)(pl + 0x35E)) = a2;
    *((s16 *)(pl + 0x360)) = a1;
    new_var = (s16 *)(pl + 0x362);
    *new_var = new_var2;
    pl[0x352] = ((u8 *)mini)[0xE];
    pl[0x353] = ((u8 *)mini)[0xF];
    pl[0x354] = ((u8 *)mini)[0x10];
    pl[0x355] = ((u8 *)mini)[0x11];
    pl[0x356] = ((u8 *)mini)[0x12];
    pl[0x357] = ((u8 *)mini)[0x13];
    Skill_set_PL(pl, a1, a2, uid * 0xA00);
    Lb_get_comment((u8 *)&lb_player[uid] + 0x24);
}
