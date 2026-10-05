/* ud06 - f_ud 0x002731B0-0x00273B50: Get_atk_value, Get_equip_value. Whole file in ud_nm.c. */
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






















s16 Get_atk_value(u8 *p, u8 kind) {
    s16 r = 0;
    switch (kind) {
    case 0: if (p[0x35F] == 6) { r = Ken_data[*(u16 *)(p + 0x360)][0xB]; } break;
    case 1: if (p[0x35F] == 6) { r = Ken_data[*(u16 *)(p + 0x360)][0xC]; } break;
    case 2: if (p[0x35F] == 6) { r = Ken_data[*(u16 *)(p + 0x360)][0xD]; } break;
    case 3: if (p[0x35F] == 6) { r = Ken_data[*(u16 *)(p + 0x360)][0xE]; } break;
    case 4: if (p[0x35F] == 6) { r = Ken_data[*(u16 *)(p + 0x360)][0xF]; } break;
    case 5: if (p[0x35F] == 6) { r = Ken_data[*(u16 *)(p + 0x360)][0x10]; } break;
    case 6: if (p[0x35F] == 6) { r = Ken_data[*(u16 *)(p + 0x360)][0x11]; } break;
    }
    return r;
}

s16 Get_equip_value(u8 kind) {
    UDW *u = User_data;
    s16 r = 0;
    switch (kind) {
    case 0:
        if (u->wkind == 7) {
            u16 o = u->wopt;
            r += ((GE *)&Gun_data[0][8])[u->wid].v;
            r += *(s16 *)(Gun_Grow_Up_DATA[Gun_data[u->wid][2]] + (o & 0xF) * 0x18);
            if (o & 0x10) { r += Silencer_Grow_Up_Tbl[0]; }
            if (o & 0x20) { r += LBarrel_Grow_Up_Tbl[0]; }
        } else {
            r = ((KE *)&Ken_data[0][8])[u->wid].v;
        }
        break;
    case 1:
        r += *(&Armor_Head_Data[0][8] + u->armor[1] * 0x14);
        r += *(&Armor_Body_Data[0][8] + u->armor[2] * 0x14);
        r += *(&Armor_Arm_Data[0][8] + u->armor[3] * 0x14);
        r += *(&Armor_Waist_Data[0][8] + u->armor[4] * 0x14);
        r += *(&Armor_Leg_Data[0][8] + u->armor[0] * 0x14);
        if (u->wkind == 7) {
            r += *(&Gun_data[0][0xA] + u->wid * 0x14);
        } else {
            r += *(&Ken_data[0][0xA] + u->wid * 0x18);
        }
        break;
    case 2:
        r += (s8)*(&Armor_Head_Data[0][9] + u->armor[1] * 0x14);
        r += (s8)*(&Armor_Body_Data[0][9] + u->armor[2] * 0x14);
        r += (s8)*(&Armor_Arm_Data[0][9] + u->armor[3] * 0x14);
        r += (s8)*(&Armor_Waist_Data[0][9] + u->armor[4] * 0x14);
        r += (s8)*(&Armor_Leg_Data[0][9] + u->armor[0] * 0x14);
        break;
    case 3:
        r += (s8)*(&Armor_Head_Data[0][0xA] + u->armor[1] * 0x14);
        r += (s8)*(&Armor_Body_Data[0][0xA] + u->armor[2] * 0x14);
        r += (s8)*(&Armor_Arm_Data[0][0xA] + u->armor[3] * 0x14);
        r += (s8)*(&Armor_Waist_Data[0][0xA] + u->armor[4] * 0x14);
        r += (s8)*(&Armor_Leg_Data[0][0xA] + u->armor[0] * 0x14);
        break;
    case 4:
        r += (s8)*(&Armor_Head_Data[0][0xB] + u->armor[1] * 0x14);
        r += (s8)*(&Armor_Body_Data[0][0xB] + u->armor[2] * 0x14);
        r += (s8)*(&Armor_Arm_Data[0][0xB] + u->armor[3] * 0x14);
        r += (s8)*(&Armor_Waist_Data[0][0xB] + u->armor[4] * 0x14);
        r += (s8)*(&Armor_Leg_Data[0][0xB] + u->armor[0] * 0x14);
        break;
    case 5:
        r += (s8)*(&Armor_Head_Data[0][0xC] + u->armor[1] * 0x14);
        r += (s8)*(&Armor_Body_Data[0][0xC] + u->armor[2] * 0x14);
        r += (s8)*(&Armor_Arm_Data[0][0xC] + u->armor[3] * 0x14);
        r += (s8)*(&Armor_Waist_Data[0][0xC] + u->armor[4] * 0x14);
        r += (s8)*(&Armor_Leg_Data[0][0xC] + u->armor[0] * 0x14);
        break;
    case 6: if (u->wkind == 6) { r = *(&Ken_data[0][0xB] + u->wid * 0x18); } break;
    case 7: if (u->wkind == 6) { r = *(&Ken_data[0][0xC] + u->wid * 0x18); } break;
    case 8: if (u->wkind == 6) { r = *(&Ken_data[0][0xD] + u->wid * 0x18); } break;
    case 9: if (u->wkind == 6) { r = *(&Ken_data[0][0xE] + u->wid * 0x18); } break;
    case 10: if (u->wkind == 6) { r = *(&Ken_data[0][0xF] + u->wid * 0x18); } break;
    case 11: if (u->wkind == 6) { r = *(&Ken_data[0][0x10] + u->wid * 0x18); } break;
    case 12: if (u->wkind == 6) { r = *(&Ken_data[0][0x11] + u->wid * 0x18); } break;
    }
    return r;
}
