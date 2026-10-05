#ifndef PLEQUIP_H
#define PLEQUIP_H
/* Equipment data tables as row structs, for src/main/pl/pl71.c only (it does not include plf.h,
 * which declares Ken_data as bytes). Strides from the original index math. Field meaning is
 * partly a guess: only the ones the Get_equip_* helpers read are named. */
#include "types.h"
typedef struct PL_EQUIP { u8 _00; u8 kind; u16 id; } PL_EQUIP;   /* equipment slot: kind 0 leg, 2 head, 3 body, 4 arm, 5 waist, 6 melee, 7 gun */
typedef struct EQ_ARMOR { u8 _00[3]; u8 rare; s32 price; u8 _08[8]; char *name; } EQ_ARMOR;   /* 0x14 */
typedef struct EQ_GUN { u8 type; u8 rare; u8 _02[2]; s32 price; u8 _08[4]; char *name; s32 ammo_mask; } EQ_GUN;   /* 0x14 */
typedef struct EQ_KEN { u8 type; u8 rare; u8 sharp; u8 _03; s32 price; u8 _08[0xC]; char *name; } EQ_KEN;   /* 0x18 */
extern EQ_GUN Gun_data[];
extern EQ_ARMOR Armor_Leg_Data[];
extern EQ_ARMOR Armor_Head_Data[];
extern EQ_ARMOR Armor_Body_Data[];
extern EQ_ARMOR Armor_Arm_Data[];
extern EQ_ARMOR Armor_Waist_Data[];
extern EQ_KEN Ken_data[];
extern u8 Battle_type[0x7C];
#endif
