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

void init_item_work(void) {
    int i;
    ITEMW *p;

    memset(item_work, 0, 0x1500);
    item_w_top = 0;
    p = &item_work[63];
    item_sp = (void **)item_work;
    for (i = 0; i < 0x40; i++) {
        *--item_sp = p--;
    }
    item_ctr = 0x40;
}

void clr_item_work(void) {
    ITEMW *p;
    ITEMW *q;

    p = item_w_top;
    if (p != 0) {
        do {
            if (p->used != 0 && (q = p, p->x42 == 0)) {
                p = p->next;
                push_item_work(q);
            } else {
                p = p->next;
            }
        } while (p != 0);
    }
}

void push_item_work(ITEMW *it) {
    if (it->prev == 0) {
        item_w_top = it->next;
    } else {
        it->prev->next = it->next;
    }
    if (it->next != 0) {
        it->next->prev = it->prev;
    }
    item_ctr++;
    *--item_sp = it;
    memset(it, 0, 8);
}

void move_item(void) {
    ITEMW *p;

    p = item_w_top;
    if (p != 0) {
        do {
            if (p->used != 0) {
                p->func(p);
            }
            p = p->next;
        } while (p != 0);
    }
}

void item_check(void) {
    ITEMW *it;
    PLW *pl;
    int i;
    f32 dx, dy, dz;
    f32 dist;

    PLB(&player_work[0], 0x602) = 0;
    it = item_w_top;
    PLB(&player_work[1], 0x602) = 0;
    PLB(&player_work[2], 0x602) = 0;
    PLB(&player_work[3], 0x602) = 0;
    if (it != 0) {
        do {
            it->near_pl = 0xFF;
            pl = player_work;
            i = 0;
            do {
                if (it->used != 0 && it->on != 0 && it->got == 0) {
                    dx = pl->pos[0] - it->pos[0];
                    dy = flAbs(pl->pos[1] - it->pos[1]);
                    dz = pl->pos[2] - it->pos[2];
                    dist = flSqrt(dz + (dz + dx * dx));
                    if (dy <= 60.0f && dist <= 100.0f) {
                        PLB(pl, 0x602) = 1;
                        if (PLB(pl, 0x601) != 0 && PLB(pl, 0x603) == 0) {
                            it->got = 1;
                            pl->work603 = 1;
                            se_req2(1, 0x22, 0, &pl->pos[0], 1, 0);
                        }
                    }
                    if (dy <= 300.0f && dist <= 800.0f) {
                        it->near_pl = pl->id;
                    }
                }
                i++;
                pl++;
            } while (i < 4);
            it = it->next;
        } while (it != 0);
    }
}

int Item_preparation_one_ck(s16 a) {
    IPREP *p;
    int i;
    int n;

    if (*(u8 *)((u8 *)Item_preparation_tbl + (a + a)) != 0) {
        return 1;
    }
    for (i = 0; i < a; i++) {
        n = *(u8 *)((u8 *)Item_preparation_tbl + (i + i));
        if (n != 0) {
            p = &Item_preparation_tbl_00[*(u8 *)((u8 *)Item_preparation_tbl + (i + i) + 1)];
            if (n > 0) {
                do {
                    if (p->a == a) {
                        return 1;
                    }
                    n--;
                    p++;
                } while (n > 0);
            }
        }
    }
    return 0;
}

IPREP *Item_preparation_adrs(a, b)
s16 a;
s16 b;
{
    IPREP *p;
    int n;

    if (a > b) {
        s16 t = a;

        a = b;
        b = t;
    }
    n = *(u8 *)((u8 *)Item_preparation_tbl + (a + a));
    if (n == 0) {
        return 0;
    }
    p = &Item_preparation_tbl_00[*(u8 *)((u8 *)Item_preparation_tbl + (a + a) + 1)];
    if (n != 0) {
        do {
            if (p->a == b) {
                return p;
            }
            n--;
            p++;
        } while (n != 0);
    }
    return 0;
}

s16 Item_preparation(int pl, int a, int b, int mode) {
    IPREP *e;
    s16 res;

    e = Item_preparation_adrs(a, b);
    if (e == 0) {
        return -1;
    }
    if ((s16)(s8)Item_preparation_rate(a, b, mode) >= (ran_suu(1) & 0xFFFF) % 101) {
        res = e->res;
    } else {
        res = -1;
    }
    if (mode == 0) {
        Pl_item_stack(pl, a & 0xFFFF, -1);
        Pl_item_stack(pl, b & 0xFFFF, -1);
    } else {
        Ud_item_stack(a & 0xFFFF, -1);
        Ud_item_stack(b & 0xFFFF, -1);
    }
    return res;
}

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
        rate = rate + pre_manual_rate_tbl[i];
        if (rate >= 0x65) {
            rate = 0x64;
        }
        return rate;
    }
    return 0x64;
}

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

#define POP1(x) ((u8)(((x) & 0x55) + (((x) & 0xAA) >> 1)))
#define POP2(x) ((u8)(((x) & 0x33) + (((x) & 0xCC) >> 2)))
#define POP3(x) ((u8)(((x) & 0x0F) + (((x) & 0xF0) >> 4)))
#define POP8(x) POP3(POP2(POP1(x)))

int Item_preparation_list_num(void) {
    return (u8)((u8)((u8)((u8)((u8)((u8)((u8)((u8)(POP8(User_data[0x3D8]) + POP8(User_data[0x3D9])) + POP8(User_data[0x3DA])) +
                                         POP8(User_data[0x3DB])) + POP8(User_data[0x3DC])) + POP8(User_data[0x3DD])) +
                   POP8(User_data[0x3DE])) + POP8(User_data[0x3DF])) + POP8(User_data[0x3E0] & 3));
}

int Item_preparation_list_chk_0(IPREP *e) {
    s8 b = e->bit;

    return (User_data[0x3D8 + (b >> 3)] & (1 << (b & 7))) != 0;
}

void Add_to_Item_preparation_list_0(IPREP *e) {
    s8 b = e->bit;

    User_data[0x3D8 + (b >> 3)] |= (1 << (b & 7)) & 0xFF;
}

int Item_preparation_list_chk(void) {
    IPREP *e = Item_preparation_adrs();
    s8 bit;

    if (e == 0) {
        return 0;
    }
    bit = e->bit;
    return (User_data[0x3D8 + (bit >> 3)] & (1 << (bit & 7))) != 0;
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
    n = 0x42;
    do {
        i = (s8)(i + dir);
        if (i > 0x41) {
            i = 0;
        } else if (i < 0) {
            i = 0x41;
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

int Item_preparation_check_list(s16 n) {
    return (User_data[0x3D8 + (n >> 3)] & (1 << (n & 7))) != 0;
}

IPREP *Item_preparation_get(s16 n) {
    return &Item_preparation_tbl_00[n];
}
