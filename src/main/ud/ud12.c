/* ud12 - f_ud 0x002743E0-0x00274654: Seisan_ok_ck (can the player craft this armor/weapon from the held items). Whole file in ud_nm.c. */
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

int Seisan_ok_ck(u16 kind, s16 idx, int mode) {
    u8 *ent;
    s16 j;
    u8 any;
    s16 have;
    s16 t;
    u8 ok;
    u8 cnt;
    u8 *e;

    switch (kind) {
    case 0:
        ent = bou_sei_tbl + idx * 0x18;
        break;
    default:
    case 1:
        ent = buki_sei_tbl + idx * 0x18;
        break;
    }
    any = 0;
    cnt = 0;
    ok = 0;
    for (j = 0, e = ent; j < 4; j++, e += 4) {
        s16 tmp;
        if (*(u16 *)(e + 4) == 0 || *(s16 *)(e + 6) == 0) {
            ok |= 1 << j;
            continue;
        }
        switch (mode) {
        case 0:
            have = Ud_item_num_ck(*(u16 *)(e + 4));
            tmp = Ud_stock_item_num_ck3(*(u16 *)(e + 4));
            break;
        case 1:
            have = Ud_item_num_ck(*(u16 *)(e + 4));
            tmp = 0;
            break;
        case 2:
            have = 0;
            tmp = Ud_stock_item_num_ck(*(u16 *)(e + 4));
            break;
        }
        t = have + tmp;
        if (ent[0x14 + j] != 0) {
            cnt++;
            if (t > 0) {
                any = 1;
            }
        }
        if (!(t < *(s16 *)(e + 6))) {
            ok |= 1 << j;
        }
    }
    if ((u8)cnt == 0 || (u8)any != 0) {
        if (ok == 0xF) {
            return 2;
        }
        if ((u8)any != 0) {
            return 1;
        }
    }
    if (ent[1] != 0) {
        return 1;
    }
    return 0;
}
