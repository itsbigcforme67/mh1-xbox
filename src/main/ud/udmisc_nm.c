/* udmisc_nm - f_ud tail (SLPM_654.95 0x00271FB0-0x00272400, main.bin): copy of the saved hunter data to and from the
 * memory-card slot block (option_w + 0x10, 0x480 bytes per slot), the pouch copy between User_data and a player work
 * (PLW +0x828), the hunter point to rank conversion (h_rank_tbl: ascending point limits ending in 9999999 = 0x98967F),
 * Set_userdata/Set_equip_data (fill a player work's name, pouch, equipment from the saved data) and Gold_add. */
#include "types.h"
#include "pl.h"
#include "ud.h"

extern u8 option_w[];
extern u32 h_rank_tbl[];
extern u8 Battle_type[];
extern PLW player_work[];

void *memcpy(void *, void *, int);
void Set_equip_idx();
void Set_equip_data();
u8 Get_weapon_id();

#define PLB(p, o) (*(u8 *)((u8 *)(p) + (o)))
#define PLH(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define PLW32(p, o) (*(s32 *)((u8 *)(p) + (o)))
#define UDB(p, o) (*(u8 *)((u8 *)(p) + (o)))
#define UDH(p, o) (*(s16 *)((u8 *)(p) + (o)))

void Load_userdata(int slot) {
    memcpy(User_data, option_w + (slot & 0xFF) * 0x480 + 0x10, 0x480);
}

void Save_userdata(int slot) {
    memcpy(option_w + (slot & 0xFF) * 0x480 + 0x10, User_data, 0x480);
}

/* the fields Set_equip_data touches, as overlays on the player work and the saved data */
typedef struct { s16 a, b, c; } EQS3;

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

void Set_equip_data(EQPL *pl, EQUD *u) {
    Set_equip_idx(u);
    pl->x11 = u->x01;
    pl->x353 = u->x02 + 1;
    pl->x34E = u->x03;
    pl->x5FC = u->x04;
    pl->x8D3 = u->a3D7;
    pl->wkind = Get_weapon_id(&u->wx3CC);
    *(EQS3 *)&pl->wid = *(EQS3 *)&u->wx3CC;
    pl->kind = Battle_type[pl->wkind];
    pl->x352 = u->a3D2;
    pl->x354 = u->a3D3;
    pl->x355 = u->a3D4;
    pl->x356 = u->a3D5;
    pl->x357 = u->a3D6;
}

typedef struct UDN { u8 pad[8]; s8 name[0x12]; } UDN;

void Set_userdata(PLW *pl) {
    s16 i;
    s16 j;
    UDW *u = User_data;

    i = 0;
    do {
        pl->name[i] = ((UDN *)u)->name[i];
        i++;
    } while (i < 0x12);
    for (j = 0; j < 20; j++) {
        pl->item[j].id = u[0].item[j].id;
        pl->item[j].num = u[0].item[j].num;
    }
    PLB(pl, 0x8D3) = UDB(u, 0x3D7);
    PLB(pl, 0x34C) = Get_weapon_id(u->item + 20);
    PLH(pl, 0x35E) = UDH(u, 0x3CC);
    PLH(pl, 0x360) = UDH(u, 0x3CE);
    PLH(pl, 0x362) = UDH(u, 0x3D0);
    Set_equip_data((void *)pl, (void *)u);
}

void ItemCopy_Pl2Ud(PLW *pl) {
    memcpy((u8 *)User_data + 0x37C, (u8 *)pl + 0x828, 0x50);
}

void ItemCopy_Ud2Pl(PLW *pl) {
    memcpy((u8 *)pl + 0x828, (u8 *)User_data + 0x37C, 0x50);
}

void Gold_add(int n) {
    User_data[0].gold = User_data[0].gold + n;
    if (User_data[0].gold > 9999999) {
        User_data[0].gold = 9999999;
    }
    if (User_data[0].gold < 0) {
        User_data[0].gold = 0;
    }
}

u8 Get_hunter_rank(UDW *u) {
    u32 *p = h_rank_tbl;
    u8 r = 0;
    u32 t = *p;

    if (t != 0x98967F) {
        do {
            if (u->point < t) {
                return r;
            }
            p++;
            r++;
            t = *p;
        } while (t != 0x98967F);
    }
    return r;
}

void Get_hunter_status(UDW *u, u8 *rank, u32 *cur, u32 *next) {
    u32 *p;
    u32 t;

    *rank = 1;
    *cur = u->point;
    t = h_rank_tbl[1];
    p = &h_rank_tbl[1];
    if (t != 0x98967F) {
        do {
            if (u->point < t) {
                break;
            }
            p++;
            (*rank)++;
            t = *p;
        } while (t != 0x98967F);
    }
    *next = t;
}
