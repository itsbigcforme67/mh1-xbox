/* ud07 - f_ud 0x00273C20-0x0027432C: Quest_clear_bit_set, Quest_clear_bit_ck, Warehouse_search_space, Warehouse_equip_stack, Warehouse_equip_erase, Equip_idx_renew, Warehouse_equip, Warehouse_equip_out, equip_idx_ck, Set_equip_idx. Whole file in ud_nm.c. */
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






















void Quest_clear_bit_set(int n) {
    User_data->qclear[n / 32] |= 1LL << (n % 32);
}

int Quest_clear_bit_ck(int n) {
    return (User_data->qclear[n / 32] & (1LL << (n % 32))) != 0;
}

u8 Warehouse_search_space(void) {
    UDW *u = User_data;
    s16 i;
    for (i = 0; i < 64; i++) {
        if (u->ware[i].use == 0) {
            return i;
        }
    }
    return 0xFF;
}

u8 Warehouse_equip_stack(UDW *u, s8 kind, s16 id, s16 opt) {
    s16 i;
    for (i = 0; i < 64; i++) {
        if (u->ware[i].use == 0) {
            u->ware[i].use = 1;
            u->ware[i].kind = kind;
            u->ware[i].id = id;
            u->ware[i].opt = opt;
            return i;
        }
    }
    return 0xFF;
}

int Warehouse_equip_erase(UDW *u, u16 idx) {
    if (Now_equip_ck(u, idx) == 0) {
        u->ware[idx].use = 0;
        u->ware[idx].kind = 0;
        u->ware[idx].id = 0;
        u->ware[idx].opt = 0;
        return 1;
    }
    return 0;
}

static void Equip_idx_renew(UDW *u) {
    u8 m = 0;
    if (u->x01 == 0) { m |= 1; } else { m |= 2; }
    if (u->wkind == 7) { m |= 8; } else { m |= 4; }
    if (m != (m & *(&Armor_Head_Data[0][2] + u->armor[1] * 0x14))) { u->widx[2] = 0xFF; }
    if (m != (m & *(&Armor_Body_Data[0][2] + u->armor[2] * 0x14))) { u->widx[3] = 0xFF; }
    if (m != (m & *(&Armor_Waist_Data[0][2] + u->armor[4] * 0x14))) { u->widx[5] = 0xFF; }
    if (m != (m & *(&Armor_Arm_Data[0][2] + u->armor[3] * 0x14))) { u->widx[4] = 0xFF; }
    if (m != (m & *(&Armor_Leg_Data[0][2] + u->armor[0] * 0x14))) { u->widx[1] = 0xFF; }
}

int Warehouse_equip(UDW *u, u8 idx) {
    if (u->ware[idx].use == 0) {
        return 0;
    }
    switch (u->ware[idx].kind) {
    case 0: u->widx[1] = idx; break;
    case 2: u->widx[2] = idx; break;
    case 3: u->widx[3] = idx; break;
    case 4: u->widx[4] = idx; break;
    case 5: u->widx[5] = idx; break;
    case 6:
    case 7:
        if (idx == 0xFF) {
            return 0;
        }
        u->widx[0] = idx;
        break;
    default:
        return 0;
    }
    return 1;
}

int Warehouse_equip_out(UDW *u, u32 kind) {
    switch (kind) {
    case 0: u->widx[1] = 0xFF; break;
    case 2: u->widx[2] = 0xFF; break;
    case 3: u->widx[3] = 0xFF; break;
    case 4: u->widx[4] = 0xFF; break;
    case 5: u->widx[5] = 0xFF; break;
    case 6:
    case 7:
        return 0;
    default:
        return 0;
    }
    return 1;
}

static int equip_idx_ck(UDW *u, u8 idx, u8 k1, u8 k2) {
    if (idx == 0xFF) {
        return 0;
    }
    if (u->ware[idx].use != 0) {
        if (u->ware[idx].kind == k1 || u->ware[idx].kind == k2) {
            goto one;
        }
    }
    return 0;
one:
    return 1;
}

void Set_equip_idx(UDW *u) {
    if (equip_idx_ck(u, u->widx[0], 6, 7) == 1) {
        u->wkind = u->ware[u->widx[0]].kind;
        u->wid = u->ware[u->widx[0]].id;
        u->wopt = u->ware[u->widx[0]].opt;
    } else {
        u->wkind = 6;
        u->wid = 0x9B;
    }
    Equip_idx_renew(u);
    if (equip_idx_ck(u, u->widx[1], 0, 0) == 1) {
        u->armor[0] = u->ware[u->widx[1]].id;
    } else {
        u->armor[0] = 0;
    }
    if (equip_idx_ck(u, u->widx[2], 2, 2) == 1) {
        u->armor[1] = u->ware[u->widx[2]].id;
    } else {
        u->armor[1] = 0;
    }
    if (equip_idx_ck(u, u->widx[3], 3, 3) == 1) {
        u->armor[2] = u->ware[u->widx[3]].id;
    } else {
        u->armor[2] = 0;
    }
    if (equip_idx_ck(u, u->widx[4], 4, 4) == 1) {
        u->armor[3] = u->ware[u->widx[4]].id;
    } else {
        u->armor[3] = 0;
    }
    if (equip_idx_ck(u, u->widx[5], 5, 5) == 1) {
        u->armor[4] = u->ware[u->widx[5]].id;
    } else {
        u->armor[4] = 0;
    }
}
