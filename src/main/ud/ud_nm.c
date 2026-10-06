/* ud_nm - f_ud (SLPM_654.95 0x001723F0-0x00174E10, main.bin): user-data helpers (item pouch, stock, event
 * flags, hunter points, equipment values) as near-match C, not built; matching runs are built from it.
 * Field meanings are guesses. */
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

int Ud_item_stack(s16 id, s16 num) {
    UDW *u = User_data;
    u16 ret;
    s16 i;
    u8 *max;

    if (Item_data[(u16)id][0] == 5) {
        return Share_item_stack(&player_work[game_w.master], id, num);
    }
    max = &Item_data[(u16)id][3];
    if (*max == 0xFF) {
        num = 0xFF;
    }
    if (Ud_item_num_ck(id) == 0) {
        ret = 5;
        for (i = 0; i < 20; i++) {
            if (u->item[i].id == 0 && num > 0) {
                u->item[i].id = id;
                if ((s16)*max < num) {
                    num = *max;
                }
                u->item[i].num = num;
                ret = 0;
                break;
            }
        }
    } else {
        for (i = 0; i < 20; i++) {
            if (u->item[i].id == (u16)id) {
                s16 m = *max;
                if (num > 0 && !(u->item[i].num < m)) {
                    u->item[i].num = m;
                    ret = 3;
                } else if (num < 0 && (ret = 1, *max == 0xFF)) {
                } else {
                    u->item[i].num += num;
                    if (u->item[i].num <= 0) {
                        Ud_item_erase(i);
                        ret = 4;
                        u->item[i].id = 0;
                        u->item[i].num = 0;
                    } else if (m < u->item[i].num) {
                        u->item[i].num = m;
                        ret = 2;
                    } else {
                        ret = 1;
                    }
                }
                break;
            }
        }
    }
    return ret;
}

s16 Ud_item_num_ck(u16 id) {
    s16 i;
    UDW *u = User_data;
    for (i = 0; i < 20; i++) {
        if (u->item[i].id == id) {
            return u->item[i].num;
        }
    }
    return 0;
}

int Ud_item_num_ck2(u16 id)
{
  s16 i;
  int r;
  UDW *u = User_data;
  for (i = 0; i < 20; i++)
  {
    if (u->item[i].id == id)
    {
      u8 m = Item_data[id][3];
      if (m == 0xFF)
      {
        if ((m && m) && m)
        {
        }
        return 0xFF;
      }
      else
      {
        return (s16) (m - u->item[i].num);
      }
    }
  }

  return Item_data[id][3];
}

int Ud_item_num_ck3(u16 id) {
    s16 i;
    int new_var;
    s16 n = 0;
    UDW *u = User_data;
    for (i = 0; i < 20; i++) {
        u16 v = u->item[i].id;
        new_var = v == id;
        if (new_var) {
            u8 m = Item_data[id][3];
            if (m == 0xFF) {
                /* permuter-found filler: changes only the branch/delay-slot layout */
                if (((!v) && (!v)) && (!v)) {
                }
                return 0xFF;
            } else {
                return (s16)(m - u->item[i].num);
            }
        }
        if (v == 0) {
            n++;
        }
    }
    return n == 0 ? -1 : Item_data[id][3];
}

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

s16 Ud_u_item_stack(u16 id, u16 num) {
    UDW *u = User_data;
    int i;
    u8 *max = &Item_data[id][3];
    u8 m;
    UD_ITEM *p;

    if (Item_data[id][0] == 5) {
        return Share_item_stack(&player_work[game_w.master], id, (s16)num);
    }
    m = *max;
    if (m < num) {
        num = m;
    }
    if (m != 0xFF) {
        p = u->stock;
        for (i = 100; i != 0; i--, p++) {
            if (p->id == id) {
                s16 cur = p->num;
                u32 room = m - cur;
                if ((u32)cur < m) {
                    if (room >= num) {
                        p->num = cur + (s16)num;
                        return 0;
                    }
                    p->num = m;
                    num = num - (u16)room;
                }
            }
        }
    }
    p = u->stock;
    for (i = 100; i != 0; i--, p++) {
        if (p->id == 0) {
            p->id = id;
            p->num = num;
            return 0;
        }
    }
    return num;
}

void Event_flag_set(int n) {
    User_data->evflag[n / 16] |= (u16)(1 << (n & 0xF));
}

void Event_flag_clear(int n) {
    User_data->evflag[n / 16] &= (u16)~(1 << (n & 0xF));
}

int Event_flag_ck(int n) {
    return (User_data->evflag[n / 16] & (1 << (n & 0xF))) != 0;
}

extern u8 option_w[];
void Omake_flag_set(int n) {
    *(s16 *)(option_w + 0xFCC) |= (s16)(1 << n);
}

int Omake_flag_ck(int n) {
    return (*(s16 *)(option_w + 0xFCC) & (1 << n)) != 0;
}

void Set_mini_data_to_pl(s8 *key, u8 *pl) {
    s8 id;
    u8 *m;
    s16 w;

    id = Get_pl_id(key);
    m = room_member_mini_data[id];
    *(s32 *)(pl + 0x5FC) = *(s32 *)(m + 4);
    pl[0x11] = m[3];
    pl[0x34E] = m[0x14];
    w = *(s16 *)(m + 8);
    *(s16 *)(pl + 0x35E) = w;
    *(s16 *)(pl + 0x360) = *(s16 *)(m + 0xA);
    *(s16 *)(pl + 0x362) = *(s16 *)(m + 0xC);
    pl[0x8D3] = m[0x16];
    *(s8 *)(pl + 0x34C) = Get_weapon_id(m + 8, w);
    memcpy(pl + 0x352, m + 0xE, 6);
    strcpy((char *)pl + 0x8D4, room_member_handle[id]);
}

s8 Get_pl_id(s8 *key) {
    s8 i;
    s8 j;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 6; j++) {
            if (room_member_id[i][j] != key[j]) {
                break;
            }
        }
        if (j == 6) {
            return i;
        }
    }
    return 0;
}

void Copy_user_id(u8 no)
{
  u8 *d = (((u8 *) (&game_w)) + 0x1E8) - (-(no * 8));
  d[0] = my_user_id[0];
  d[1] = my_user_id[1];
  d[2] = my_user_id[2];
  d[3] = my_user_id[3];
  d[4] = my_user_id[4];
  d[5] = my_user_id[5];
  d[6] = my_user_id[6];
  d[7] = my_user_id[7];
}

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

s16 Get_bowgun_atk(u8 *w) {
    u16 o;
    int off;
    s16 r;

    if (w[1] != 7) {
        return -1;
    }
    off = *(u16 *)(w + 2) * 0x14;
    o = *(u16 *)(w + 4);
    r = ((GE *)(&Gun_data[0][8] + off))->v + *(s16 *)(Gun_Grow_Up_DATA[*(&Gun_data[0][2] + off)] + (o & 0xF) * 0x18);
    if (o & 0x10) { r += Silencer_Grow_Up_Tbl[0]; }
    if (o & 0x20) { r += LBarrel_Grow_Up_Tbl[0]; }
    return r;
}

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

int Now_equip_ck(UDW *u, int idx);

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

int Now_equip_ck(UDW *u, int idx)
{
  if (u->widx[0] == idx)
  {
    return 1;
  }
  if (u->widx[1] == idx)
  {
    return 1;
  }
  if (u->widx[2] == idx)
  {
    return 1;
  }
  if (u->widx[3] == idx)
  {
    return 1;
  }
  if (u->widx[4] == idx)
  {
    if (idx)
    {
      return 1;
    }
    else
    {
      return 1;
    }
  }
  return u->widx[5] == idx;
}

int Warehouse_space_ck(UDW *u, int idx) {
    return u->ware[idx].use == 0;
}

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
    e = ent;
    for (j = 0; j < 4; j++, e += 4) {
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
    if ((u8)cnt != 0 || (u8)any != 0) {
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

int gun_check(UDW *u, s16 i) {
    return u->ware[i].kind == 7;
}

void Gun_level_up(UDW *u, s16 i) {
    if (gun_check(u, i) != 0) {
        u16 o = u->ware[i].opt;
        int l = o & 0xF;
        u16 v;
        if (l >= 4) {
            v = 4;
        } else {
            v = (l + 1) & 0xFFFF;
        }
        u->ware[i].opt = (o & 0x70) | (v & 0xFFFF);
    }
}

int Get_Gun_level(UDW *u, s16 i) {
    if (gun_check(u, i) == 0) {
        return 0;
    }
    return u->ware[i].opt & 0xF;
}

int Gun_option_ck(UDW *u, s16 i, int mask) {
    int r;
    if (gun_check(u, i) == 0) {
        r = 0xFF;
    } else {
        r = (u->ware[i].opt & mask) != 0;
    }
    return r;
}

void Gun_barrel_set(UDW *u, s16 i, int off) {
    if (gun_check(u, i) != 0) {
        if (off == 0) {
            u->ware[i].opt |= 0x20;
            u->ware[i].opt &= 0xFFEF;
            return;
        }
        u->ware[i].opt &= 0xFFDF;
    }
}

void Gun_Silencer_set(UDW *u, s16 i, int off) {
    if (gun_check(u, i) != 0) {
        if (off == 0) {
            u->ware[i].opt |= 0x10;
            u->ware[i].opt &= 0xFFDF;
            return;
        }
        u->ware[i].opt &= 0xFFEF;
    }
}

void Gun_Scope_set(UDW *u, s16 i, int off) {
    if (gun_check(u, i) != 0) {
        if (off == 0) {
            u->ware[i].opt |= 0x40;
            return;
        }
        u->ware[i].opt &= 0xFFBF;
    }
}

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
