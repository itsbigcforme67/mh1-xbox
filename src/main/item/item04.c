/* item04 - Item_preparation_rate_0 (SLPM_654.95 0x0011D450-0x0011D554): success rate of a preparation recipe (clamped to 100). Whole file in item_nm.c. */
/* item_nm - dropped item work (SLPM_654.95 0x0011CDF0-0x0011D940, main.bin): the 64-slot field item pool
 * (free-slot stack, doubly linked list of live items, per-frame move and "is a player close enough to pick it up"
 * check) and the item preparation (mix recipe) tables: lookup of a recipe by its two ingredients, success rate,
 * the "known recipes" bit list in User_data+0x3D8 (66 bits). Field names are guesses from use. */
#include "types.h"
#include "pl.h"
typedef struct ITEMW {
    u8 used;                /* 0x00 slot in use */
    u8 on;                  /* 0x01 active (can be picked up) */
    u8 pad02[0xA];
    struct ITEMW *prev;     /* 0x0C */
    struct ITEMW *next;     /* 0x10 */
    u8 pad14[4];
    u8 near_pl;             /* 0x18 id of the player that can see it, 0xFF none */
    u8 got;                 /* 0x19 already picked up */
    u8 pad1A[2];
    void (*func)(struct ITEMW *); /* 0x1C per-frame handler */
    f32 pos[3];             /* 0x20 */
    u8 pad2C[0x16];
    u8 x42;                 /* 0x42 */
    u8 pad43[0x54 - 0x43];
} ITEMW;
/* one recipe: ingredient b (a is the table index), result item, list bit */
typedef struct IPREP {
    s16 a;                  /* 0x00 other ingredient */
    s16 res;                /* 0x02 result item */
    s8 x04;                 /* 0x04 rate table index */
    s8 bit;                 /* 0x05 bit of the known-recipe list */
} IPREP;
extern ITEMW item_work[64];
extern ITEMW *item_w_top;
extern s32 item_ctr;
extern void **item_sp;
typedef struct IPTBL {
    u8 n;                   /* number of recipes whose first ingredient is the index */
    u8 idx;                 /* first recipe in Item_preparation_tbl_00 */
} IPTBL;
extern IPTBL Item_preparation_tbl[];
extern IPREP Item_preparation_tbl_00[];
extern u16 Item_preparation_tbl2[][2];
extern u8 item_pre_rate_tbl[8];
extern u8 pre_manual_rate_tbl[8];
extern u16 mix_manual_tbl[];
extern u8 User_data[];
extern u8 game_w[];
#define PLB(p, o) (*(u8 *)((u8 *)(p) + (o)))
void *memset(void *, int, int);
f32 flAbs(f32);
f32 flSqrt(f32);
void se_req2();
s16 Pl_item_num_ck();
void Pl_item_stack();
void Ud_item_stack();
int ran_suu();
void push_item_work(ITEMW *);
s8 Item_preparation_rate();
s8 Item_preparation_rate_0();
IPREP *Item_preparation_adrs();
int Item_preparation_list_chk_0(IPREP *);
s8 Item_preparation_rate_0(IPREP *e, int mode) {
    u8 rate;
    u32 i;
    u16 *p;
    PLW *pl;

    if (mode == 0) {
        rate = item_pre_rate_tbl[e->x04];
        if (Item_preparation_list_chk_0(e) == 1) {
            rate += 5;
        }
        i = 0;
        p = mix_manual_tbl;
        pl = &player_work[game_w[0xD1]];
        do {
            if ((s16)Pl_item_num_ck(pl, *p) == 0) {
                break;
            }
            i++;
            p++;
        } while (i < 5);
        rate += pre_manual_rate_tbl[i];
    } else {
        return 0x64;
    }
    if (rate > 0x64) {
        rate = 0x64;
    }
    return rate;
}
