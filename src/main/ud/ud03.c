/* ud03 - f_ud 0x00272BF0-0x00272D58: Ud_item_search_space, Ud_item_erase, Ud_stock_item_num_ck, Ud_stock_item_num_ck3. Whole file in ud_nm.c. */
#include "types.h"
#include "game.h"
#include "pl.h"
#include "ud.h"

extern u8 Item_data[327][16];
extern u8 h_rank_tbl[];
typedef struct { s16 v; u8 _p[0x12]; } GE;
typedef struct { s16 v; u8 _p[0x16]; } KE;
extern u8 Ken_data[][0x18];
extern u8 Gun_data[][0x14];
extern u8 Armor_Head_Data[][0x14];
extern u8 Armor_Body_Data[][0x14];
extern u8 Armor_Waist_Data[][0x14];
extern u8 Armor_Arm_Data[][0x14];
extern u8 Armor_Leg_Data[][0x14];
extern u8 *Gun_Grow_Up_DATA[];
extern s16 Silencer_Grow_Up_Tbl[];
extern s16 LBarrel_Grow_Up_Tbl[];
extern u8 room_member_mini_data[][0x40];
extern char room_member_handle[][0x11];
extern s8 room_member_id[][8];
extern s8 my_user_id[];
extern u8 ex_equip_tbl[];
extern u8 bou_sei_tbl[];
extern u8 buki_sei_tbl[];
extern char lit_1515_003735D0[];
s8 Get_weapon_id();
void *memcpy(void *, const void *, int);
char *strcpy(char *, const char *);
int Online_ck();
s8 Get_pl_id(s8 *);
u8 set01_set2(char *);

u8 Get_hunter_rank();
int Quest_clear_bit_ck();
f32 flAbs(f32);
void stop_level(void *, int);
int Share_item_stack(void *, s16, s16);
int Ud_item_erase(s16);
s16 Ud_item_num_ck(u16);

extern u32 D_0035178C[];
















extern u8 option_w[];












int Now_equip_ck(UDW *u, int idx);






















int Ud_item_search_space(void) {
    s16 i;
    for (i = 0; i < 20; i++) {
        if (User_data->item[i].id == 0) {
            return 1;
        }
    }
    return 0;
}

int Ud_item_erase(s16 i) {
    UDW *u = User_data;
    if (i >= 20) {
        return 2;
    }
    u->item[i].num = 0;
    if (u->item[i].id == 0) {
        return 0;
    }
    u->item[i].id = 0;
    return 1;
}

s16 Ud_stock_item_num_ck(u16 id) {
    s16 i;
    UDW *u = User_data;
    for (i = 0; i < 100; i++) {
        if (u->stock[i].id == id) {
            return u->stock[i].num;
        }
    }
    return 0;
}

s16 Ud_stock_item_num_ck3(u16 id) {
    s16 i;
    UDW *u = User_data;
    s16 n = 0;
    for (i = 0; i < 100; i++) {
        if (u->stock[i].id == id) {
            n += u->stock[i].num;
        }
    }
    return n;
}
