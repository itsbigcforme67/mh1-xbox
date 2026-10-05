/* SLPM_654.95 0x00271FB0-0x00272010: Load_userdata .. Save_userdata. See udmisc_nm.c. */
#include "types.h"
#include "pl.h"
#include "ud.h"

extern u8 option_w[];
extern u32 h_rank_tbl[];
extern u8 Battle_type[];
extern PLW player_work[];

void *memcpy(void *, void *, int);
void Set_equip_idx();
u8 Get_weapon_id();

#define PLB(p, o) (*(u8 *)((u8 *)(p) + (o)))
#define PLH(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define PLW32(p, o) (*(s32 *)((u8 *)(p) + (o)))
#define UDB(p, o) (*(u8 *)((u8 *)(p) + (o)))
#define UDH(p, o) (*(s16 *)((u8 *)(p) + (o)))



/* the fields Set_equip_data touches, as overlays on the player work and the saved data */
typedef struct EQPL {
    u8 pad00[2];
    u8 kind;            /* 0x002 */
    u8 pad03[0xE];
    u8 x11;             /* 0x011 */
    u8 pad12[0x34C - 0x12];
    u8 wkind;           /* 0x34C */
    u8 pad34D;
    u8 x34E;            /* 0x34E */
    u8 pad34F[3];
    u8 x352;            /* 0x352 */
    u8 x353;            /* 0x353 */
    u8 x354;            /* 0x354 */
    u8 x355;            /* 0x355 */
    u8 x356;            /* 0x356 */
    u8 x357;            /* 0x357 */
    u8 pad358[6];
    s16 wid;            /* 0x35E */
    s16 wopt;           /* 0x360 */
    s16 wx362;          /* 0x362 */
    u8 pad364[0x5FC - 0x364];
    s32 x5FC;           /* 0x5FC */
    u8 pad600[0x8D3 - 0x600];
    u8 x8D3;            /* 0x8D3 */
} EQPL;

typedef struct EQUD {
    u8 pad00;
    u8 x01;
    u8 x02;
    u8 x03;
    s32 x04;
    u8 pad08[0x3CC - 8];
    s16 wx3CC;
    s16 wx3CE;
    s16 wx3D0;
    u8 a3D2;
    u8 a3D3;
    u8 a3D4;
    u8 a3D5;
    u8 a3D6;
    u8 a3D7;
} EQUD;







void Load_userdata(int slot) {
    memcpy(User_data, option_w + (slot & 0xFF) * 0x480 + 0x10, 0x480);
}

void Save_userdata(int slot) {
    memcpy(option_w + (slot & 0xFF) * 0x480 + 0x10, User_data, 0x480);
}
