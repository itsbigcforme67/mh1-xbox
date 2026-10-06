/* lb_gy01 - near-match fixes 0x005CA8C0-0x005CA9C0: lb_guild_check_keyQuest. Whole file in lb_y.c. */
#include "lobby_f.h"
extern u8 *flag_quest_tbl[];
extern u8 *flag_quest_tbl_local[];
extern u8 *quest_lv_tbl[];
extern u8 *quest_local_tbl[];
extern u8 lb_quest_clear[];
extern u8 lb_quest_info[];
extern u8 key_quest;
extern s8 key_quest_num;
extern u8 User_data[];
int Quest_clear_bit_ck();
int lb_key_quest_ck();
int Ex_quest_ck();
int Event_flag_ck();
void Event_flag_set();
int lb_get_quest_level();
u8 get_new_quest();
u8 get_flag_quest();
int lb_guild_check_keyQuest();
int Lb_guild_check_requireF();
s8 lb_set_key_quest_local(void);

int lb_guild_check_keyQuest(u32 a) {
    switch (a) {
    case 0:
        if (*(u8 *)0x3C733B >= 4) {
            return 1;
        }
        break;
    case 1:
        if (*(u8 *)0x3C733B >= 8) {
            return 1;
        }
        break;
    case 2:
        if (*(u8 *)0x3C733B >= 0xC) {
            return 1;
        }
        break;
    case 3:
        if (*(u8 *)0x3C733B >= 0x10) {
            return 1;
        }
        break;
    case 4:
        if (*(u8 *)0x3C733B >= 0x12) {
            return 1;
        }
        break;
    case 5:
        if (*(u8 *)0x3C733B >= 0x13) {
            return 1;
        }
        break;
    default:
        return 1;
    }
    return 0;
}
