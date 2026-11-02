/* SLPM_654.95 0x0011D840-0x0011DB04: Item_preparation_list_chk_0 .. Item_preparation_get. See item_nm.c. */
#include "gwx.h"
#include "types.h"
#include "itemprep.h"
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

#define PLB(p, o) (*(u8 *)((u8 *)(p) + MH2_OFS(p, o)))

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
IPREP *Item_preparation_adrs();
int Item_preparation_list_chk_0(IPREP *);











#define POP1(x) ((u8)(((x) & 0x55) + (((x) & 0xAA) >> 1)))
#define POP2(x) ((u8)(((x) & 0x33) + (((x) & 0xCC) >> 2)))
#define POP3(x) ((u8)(((x) & 0x0F) + (((x) & 0xF0) >> 4)))
#define POP8(x) POP3(POP2(POP1(x)))








#ifdef VERSION_MHG
int func_0010DD20(PLW *, IPREP *);

int Item_preparation_list_chk_0(IPREP *e) {
    s8 b;

    if (func_0010DD20(&player_work[game_w[GWX(0xD1)]], e) != 1) {
        return 0;
    }
    b = e->bit;
    return (User_data[IPREP_LIST + (b >> 3)] & (1 << (b & 7))) != 0;
}
#else
int Item_preparation_list_chk_0(IPREP *e) {
    s8 b = e->bit;

    return (User_data[IPREP_LIST + (b >> 3)] & (1 << (b & 7))) != 0;
}
#endif

void Add_to_Item_preparation_list_0(IPREP *e) {
    s8 b = e->bit;

    User_data[IPREP_LIST + (b >> 3)] |= (1 << (b & 7)) & 0xFF;
}

int Item_preparation_list_chk(void) {
    IPREP *e = Item_preparation_adrs();
    s8 bit;

    if (e == 0) {
        return 0;
    }
    bit = e->bit;
    return (User_data[IPREP_LIST + (bit >> 3)] & (1 << (bit & 7))) != 0;
}

int Item_preparation_list_search(idx, dir, oa, ob)
s8 *idx;
int dir;
u16 *oa;
u16 *ob;
{
    IPREP *e;
    u16 (*t)[2];
    int n;
    int i;

    i = *idx;
    if ((s8)dir == 0) {
        t = &Item_preparation_tbl2[(s8)i];
        e = Item_preparation_adrs((s16)t[0][0], (s16)t[0][1]);
        if (e != 0 && Item_preparation_list_chk_0(e) == 1) {
            *oa = t[0][0];
            *ob = t[0][1];
            return (int)e;
        }
        dir = 1;
    }
    n = IPREP_NUM;
    do {
        i = (s8)(i + dir);
        if (i > IPREP_NUM - 1) {
            i = 0;
        } else if (i < 0) {
            i = IPREP_NUM - 1;
        }
        t = &Item_preparation_tbl2[(s8)i];
        e = Item_preparation_adrs((s16)t[0][0], (s16)t[0][1]);
        if (e != 0 && Item_preparation_list_chk_0(e) == 1) {
            *idx = i;
            *oa = t[0][0];
            *ob = t[0][1];
            return (int)e;
        }
        n--;
    } while (n != 0);
    return 0;
}

#if !defined(G_PART)
int Item_preparation_check_list(s16 n) {
    return (User_data[IPREP_LIST + (n >> 3)] & (1 << (n & 7))) != 0;
}
#endif

#if !defined(G_PART)
IPREP *Item_preparation_get(s16 n) {
    return &Item_preparation_tbl_00[n];
}
#endif

#ifdef VERSION_MHG
/* G 0x10DD20 (no symbol in the split, so its asm sits inside the neighbour): may the hunter use this recipe?
 * Recipes flagged 1 (or no recipe) need the mixing skill 0xA8 or at least one of item 6 (the combo manual). The PC
 * build had no-op stand-ins that returned 0 here, so no recipe was offered in the mix menu. */
s32 Pl_Skill_ck(PLW *, int);
#if !defined(G_PART) || G_PART == 2
int func_0010DD20(PLW *pl, IPREP *e) {
    if (e != 0 && (e->flags & 1) == 0) {
        return 1;
    }
    if (Pl_Skill_ck(pl, 0xA8) == 1) {
        return 1;
    }
    return (s16)Pl_item_num_ck(pl, 6) != 0;
}
#endif
#endif
