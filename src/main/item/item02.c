/* SLPM_654.95 0x0011D560-0x0011D59C: Item_preparation_rate .. Item_preparation_rate. See item_nm.c. */
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











#define POP1(x) ((u8)(((x) & 0x55) + (((x) & 0xAA) >> 1)))
#define POP2(x) ((u8)(((x) & 0x33) + (((x) & 0xCC) >> 2)))
#define POP3(x) ((u8)(((x) & 0x0F) + (((x) & 0xF0) >> 4)))
#define POP8(x) POP3(POP2(POP1(x)))








s8 Item_preparation_rate(a, b, mode)
int a;
int b;
int mode;
{
    IPREP *e = Item_preparation_adrs();

    if (e == 0) {
        return -1;
    }
    return Item_preparation_rate_0(e, mode);
}
