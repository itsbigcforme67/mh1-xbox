/* work in progress; functions move to pl_nm.c once they compile */
#include "types.h"
#include "plequip.h"

void *Get_equip_data_ptr(PL_EQUIP *e) {
    switch (e->kind) {
    case 0:
        return &Armor_Leg_Data[e->id];
    case 2:
        return &Armor_Head_Data[e->id];
    case 3:
        return &Armor_Body_Data[e->id];
    case 4:
        return &Armor_Arm_Data[e->id];
    case 5:
        return &Armor_Waist_Data[e->id];
    case 6:
        return &Ken_data[e->id];
    case 7:
        return &Gun_data[e->id];
    default:
        return 0;
    }
}

u8 Get_weapon_id(PL_EQUIP *e) {
    if (e->kind == 6) {
        return Ken_data[e->id].type;
    }
    if (e->kind == 7) {
        return Gun_data[e->id].type;
    }
    return 0;
}

u8 Get_weapon_job(PL_EQUIP *e) {
    return Battle_type[(u8)Get_weapon_id(e)];
}

u8 Get_weapon_job2(u8 kind, u16 id) {
    u8 j;
    if (kind == 6) {
        return Battle_type[Ken_data[id].type];
    }
    if (kind == 7) {
        j = Battle_type[Gun_data[id].type];
        if (j == 5) {
            j = 1;
        }
        return j;
    }
    return 0;
}

s32 Get_equip_price(u8 kind, u16 id) {
    switch (kind) {
    case 0:
        return Armor_Leg_Data[id].price;
    case 2:
        return Armor_Head_Data[id].price;
    case 3:
        return Armor_Body_Data[id].price;
    case 4:
        return Armor_Arm_Data[id].price;
    case 5:
        return Armor_Waist_Data[id].price;
    case 6:
        return Ken_data[id].price;
    case 7:
        return Gun_data[id].price;
    default:
        return 9999999;
    }
}

u32 Get_equip_kaitori(u8 kind, u16 id) {
    u32 p;
    switch (kind) {
    case 0:
        p = Armor_Leg_Data[id].price;
        break;
    case 2:
        p = Armor_Head_Data[id].price;
        break;
    case 3:
        p = Armor_Body_Data[id].price;
        break;
    case 4:
        p = Armor_Arm_Data[id].price;
        break;
    case 5:
        p = Armor_Waist_Data[id].price;
        break;
    case 6:
        p = Ken_data[id].price;
        break;
    case 7:
        p = Gun_data[id].price;
        break;
    default:
        return 0;
    }
    return p >> 1;
}

s32 Get_equip_name(u8 kind, u16 id) {
    switch (kind) {
    case 0:
        return (s32)Armor_Leg_Data[id].name;
    case 2:
        return (s32)Armor_Head_Data[id].name;
    case 3:
        return (s32)Armor_Body_Data[id].name;
    case 4:
        return (s32)Armor_Arm_Data[id].name;
    case 5:
        return (s32)Armor_Waist_Data[id].name;
    case 6:
        return (s32)Ken_data[id].name;
    case 7:
        return (s32)Gun_data[id].name;
    default:
        return 0;
    }
}

u8 Get_equip_rare(u8 kind, u16 id) {
    switch (kind) {
    case 0:
        return Armor_Leg_Data[id].rare;
    case 2:
        return Armor_Head_Data[id].rare;
    case 3:
        return Armor_Body_Data[id].rare;
    case 4:
        return Armor_Arm_Data[id].rare;
    case 5:
        return Armor_Waist_Data[id].rare;
    case 6:
        return Ken_data[id].rare;
    case 7:
        return Gun_data[id].rare;
    default:
        return 0;
    }
}
