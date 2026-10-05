/* SLPM_654.95 0x00274960-0x00274E10: Equip_ok_ck .. Ex_quest_ck. See ud_nm.c. */
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






















int Equip_ok_ck(UDW *u, UD_WARE *w) {
    u8 m = 0;
    if (u->x01 == 0) { m |= 1; } else { m |= 2; }
    if (u->wkind == 7) { m |= 8; } else { m |= 4; }
    switch (w->kind) {
    case 2: if (m == (m & Armor_Head_Data[w->id][2])) { break; } return 0;
    case 3: if (m == (m & Armor_Body_Data[w->id][2])) { break; } return 0;
    case 5: if (m == (m & Armor_Waist_Data[w->id][2])) { break; } return 0;
    case 4: if (m == (m & Armor_Arm_Data[w->id][2])) { break; } return 0;
    case 0: if (m == (m & Armor_Leg_Data[w->id][2])) { break; } return 0;
    case 6:
    case 7:
    default: break;
    }
    return 1;
}

int Get_equip_bit(UDW *u, UD_WARE *w) {
    switch (w->kind) {
    case 2: return Armor_Head_Data[w->id][2];
    case 3: return Armor_Body_Data[w->id][2];
    case 5: return Armor_Waist_Data[w->id][2];
    case 4: return Armor_Arm_Data[w->id][2];
    case 0: return Armor_Leg_Data[w->id][2];
    case 6:
    case 7: return 0xF;
    default: return 0;
    }
}

void wyvern_kill_cnt_up(UDW *u, int n) {
    if (Online_ck() != 0) {
        if (n == 0) {
            u->wyv_kill[0]++;
            if (u->wyv_kill[0] > 100) {
                u->wyv_kill[0] = 100;
            }
        } else {
            u->wyv_kill[1]++;
            if (u->wyv_kill[1] > 100) {
                u->wyv_kill[1] = 100;
            }
        }
    }
}

void Gunner_wasure_ck(UDW *u) {
    s16 i;
    u8 *p;
    if (u->wkind == 7) {
        i = 0;
        p = (u8 *)u;
        for (; i < 20; i++, p += 4) {
            u16 id = *(u16 *)(p + 0x37C);
            if (id != 0 && *(s16 *)(p + 0x37E) > 0 && Item_data[id][1] == 2) {
                if (!(*(u32 *)(Gun_data[u->wid] + 0x10) & (1 << *(s16 *)(Item_data[id] + 8)))) {
                    continue;
                }
                return;
            }
        }
        set01_set2(lit_1515_003735D0);
    }
}

int Ex_quest_ck(UDW *u, u8 n) {
    s16 j;
    u16 *t;
    u16 id;
    u8 *w;

    if (n > 3) {
        return 0;
    }
    t = ((u16 **)ex_equip_tbl)[n];
    id = *t;
    while (id != 0xFF) {
        j = 0;
        w = (u8 *)u;
        for (; j < 64; j++, w += 6) {
            if (w[0x44] != 0 && w[0x45] == id && *(u16 *)(w + 0x46) == t[1]) {
                return 1;
            }
        }
        t += 2;
        id = *t;
    }
    return 0;
}
