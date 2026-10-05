/* lb_b01 - lobby quest money / guild end 0x005C5D50-0x005C5F30: Lb_check_money, get_quest_info, Lb_get_quest_str, Lb_get_quest_str2, Lb_back_money, lb_guild_end. Whole file in lb_b.c. */
#include "lobby_f.h"
#define USER_GOLD (*(s32 *)0x3C6FE0)







int Lb_check_money(u32 n) {
    LBQUEST *q;
    s32 fee;
    if (n >= 0xC8) {
        q = get_quest_info();
    } else {
        q = lb_quest_all[n];
    }
    fee = q->fee;
    if (USER_GOLD < fee) {
        return 1;
    }
    Gold_add(-fee);
    cnWrap_SoundRequest(8);
    return 0;
}

LBQUEST *get_quest_info(void) {
    s32 *p = (s32 *)mission_area;
    return (LBQUEST *)(*p + (int)p);
}

int Lb_get_quest_str(int n) {
    u8 *qi = (u8 *)get_quest_info();
    int base = mission_area;
    int v = *(s32 *)(*(s32 *)(qi + 0x18) + base + n * 4);
    return v + base;
}

int Lb_get_quest_str2(int n) {
    LBQUEST *q = lb_quest_all[mhRule.quest];
    if (mhRule.quest >= 0xC8) {
        return Lb_get_quest_str(n);
    }
    return *(s32 *)(*(s32 *)((u8 *)q + 0x18) + n * 4);
}

void Lb_back_money(void) {
    u16 n = *(u16 *)0x3F33DC;
    LBQUEST *q;
    if (n >= 0xC8) {
        q = get_quest_info();
    } else {
        q = lb_quest_all[n];
    }
    Gold_add(q->fee);
    cnWrap_SoundRequest(8);
}

void lb_guild_end(void) {
    CW8(0x2C08) = 1;
    ((u8 *)&lb_sys)[0x87] = 0x14;
    lb_sys.x6C = 0;
    ((u8 *)&lb_sys)[6] = 0;
    lb_sys.x68 = 0;
    Lbc_init_network_work(1);
    NPCZoomInCameraCancel();
}
