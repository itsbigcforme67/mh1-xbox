/* ud01 - f_ud 0x002723F0-0x00272770: stop_level, Hunter_point_add_sub, Hunter_point_add. Whole file in ud_nm.c. */
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






















void stop_level(void *p, int n) {
    *(u32 *)((u8 *)p + 0x1C) = D_0035178C[n] - 1;
}

int Hunter_point_add_sub(UDW *p, int add) {
    u8 rank = p->rank;
    u16 ret = 0;

    if (add < 0) {
        u32 cur = p->point;
        if (cur < (u32)flAbs((f32)add)) {
            p->point = 0;
            ret = 1;
            goto clamp;
        }
    }
    p->point += add;
clamp:
    if (p->point > 9999999) {
        p->point = 9999999;
    }
    if (p->point < D_0035178C[rank]) {
        p->point = D_0035178C[rank];
        p->rank = Get_hunter_rank(User_data);
        return 1;
    }
    if (rank >= 20) {
        return 2;
    }
    while (1) {
        u32 pt = p->point;
        if (pt < *(u32 *)(h_rank_tbl + rank * 4)) {
            p->rank = Get_hunter_rank(User_data);
            return (u8)ret;
        }
        switch (rank + 1) {
        case 5:
            if (Quest_clear_bit_ck(0x27) == 1) { rank++; } else { stop_level(p, 5); ret = 3; }
            break;
        case 9:
            if (Quest_clear_bit_ck(9, pt) == 1) { rank++; } else { stop_level(p, 9); ret = 4; }
            break;
        case 13:
            if (Quest_clear_bit_ck(0x65) == 1) { rank++; } else { stop_level(p, 0xD); ret = 5; }
            break;
        case 17:
            if (Quest_clear_bit_ck(0x4F) == 1) { rank++; } else { stop_level(p, 0x11); ret = 6; }
            break;
        case 19:
            if (Quest_clear_bit_ck(0x61) == 1) { rank++; } else { stop_level(p, 0x13); ret = 7; }
            break;
        case 20:
            if (Quest_clear_bit_ck(0x6B) == 1) { rank++; } else { stop_level(p, 0x14); ret = 8; }
            break;
        default:
            rank++;
            break;
        }
        if ((u8)ret) {
            p->rank = Get_hunter_rank(User_data);
            return (u8)ret;
        }
    }
}

void Hunter_point_add(int n) {
    Hunter_point_add_sub(User_data, n);
}
