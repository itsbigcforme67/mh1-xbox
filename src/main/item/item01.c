/* SLPM_654.95 0x0011CDF0-0x0011D328: init_item_work .. Item_preparation_adrs. See item_nm.c. */
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
IPREP *Item_preparation_adrs();
int Item_preparation_list_chk_0(IPREP *);











#define POP1(x) ((u8)(((x) & 0x55) + (((x) & 0xAA) >> 1)))
#define POP2(x) ((u8)(((x) & 0x33) + (((x) & 0xCC) >> 2)))
#define POP3(x) ((u8)(((x) & 0x0F) + (((x) & 0xF0) >> 4)))
#define POP8(x) POP3(POP2(POP1(x)))








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
        s16 t = b;

        b = a;
        a = t;
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
