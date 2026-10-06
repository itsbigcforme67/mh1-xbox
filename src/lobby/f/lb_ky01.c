/* lb_ky01 - offline guild key quest list 0x005CA230-0x005CA3D0: lb_set_key_quest_local (fills lb_quest_info+0x19 with the key quests the player can take).
   D_6EABD9 is an object of its own at lb_quest_info + 0x19 (config/lobby_aliases.txt): the original forms its address with lui/addiu and adds the index. Whole file in lb_y.c. */
#include "lobby_f.h"
extern u8 D_6EABD9[16];
extern u8 User_data[];
int Event_flag_ck();
int Quest_clear_bit_ck();
int Ex_quest_ck();

s8 lb_set_key_quest_local(void);
extern u8 D_6EABD9[16];
s8 lb_set_key_quest_local(void) {
    s8 n;
    if (Event_flag_ck(0x4C) == 0) {
        return 0;
    }
    n = 0;
    if (Quest_clear_bit_ck(0xAF) == 1 && Quest_clear_bit_ck(0xAE) == 1 && Quest_clear_bit_ck(0xAD) == 1 && Quest_clear_bit_ck(0xAC) == 1) {
        n = 1;
        D_6EABD9[0] = 0xAA;
    }
    if (Ex_quest_ck(User_data, 2) == 1) {
        D_6EABD9[n++] = 0xAF;
    }
    if (Ex_quest_ck(User_data, 1) == 1) {
        D_6EABD9[n++] = 0xAE;
    }
    if (Ex_quest_ck(User_data, 0) == 1) {
        D_6EABD9[n++] = 0xAD;
    }
    if (Quest_clear_bit_ck(0xAD) == 1) {
        D_6EABD9[n++] = 0xAC;
    }
    return n;
}
