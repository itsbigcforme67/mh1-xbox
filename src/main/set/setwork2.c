/* Set-object work pool (SLPM_654.95 0x00155140-0x001554D8 (init_set_work .. trans_set)): the pool of 128 SETW records (0x40 bytes each) kept as a free-list stack plus a doubly linked list of live entries (set_w_top, links at +0xC prev / +0x10 next). move_set / trans_set run each live entry's callbacks (+0x20 move, +0x14 draw). Whole file in setwork_nm.c. */
/* NOT BUILT (near-matches get_start_heap, get_heap_ptr, init_set_work; whole file kept). Set-object work pool (SLPM_654.95 0x00154F20-0x001554D8): a 256-byte "work heap" of
 * 512-byte blocks (get_start_heap finds a free run, set_used_heap / clr_used_heap mark it)
 * and the pool of 128 SETW records (0x40 bytes each) kept as a free-list stack plus a
 * doubly linked list of live entries (set_w_top, links at +0xC prev / +0x10 next).
 * move_set / trans_set run each live entry's callbacks (+0x20 move, +0x14 draw). */
#include "types.h"
#include "set.h"
extern u8 work_heap[0x100];
extern u8 *work_heap_area;
extern u8 set_work[0x2000];
extern SETW **set_sp;
extern s32 set_ctr;
extern SETW *set_w_top;
void *memset(void *, int, unsigned long);
void init_set_work(void) {
    int i;
    SETW *p;
    memset(set_work, 0, 0x2000);
    set_w_top = 0;
    p = (SETW *)(set_work + 0x1FC0);
    set_sp = (SETW **)set_work;
    for (i = 0; i < 0x80; i++) {
        *--set_sp = p;
        p--;
    }
    set_ctr = 0x80;
}
void clr_set_work(void) {
    SETW *p = set_w_top;
    while (p != 0) {
        if (p->pad0 != 0 && p->flag3E == 0) {
            SETW *q = p;
            p = p->next;
            push_set_work(q);
        } else {
            p = p->next;
        }
    }
}
SETW *pull_set_work(int n) {
    int start;
    SETW *sw;
    if ((start = get_start_heap(n)) < 0) {
        return 0;
    }
    if (set_ctr == 0) {
        return 0;
    }
    set_ctr--;
    sw = *set_sp++;
    sw->prev = 0;
    sw->next = set_w_top;
    set_w_top = sw;
    if (sw->next != 0) {
        sw->next->prev = sw;
    }
    sw->pad0 = 1;
    sw->u.work = n > 0 ? (void *)get_heap_ptr(start) : (void *)-1;
    sw->heap_pos = start;
    sw->heap_n = n;
    sw->flag3E = 0;
    set_used_heap(start, n);
    return sw;
}
void push_set_work(SETW *sw) {
    if (sw->prev == 0) {
        set_w_top = sw->next;
    } else {
        sw->prev->next = sw->next;
    }
    if (sw->next != 0) {
        sw->next->prev = sw->prev;
    }
    set_ctr++;
    *--set_sp = sw;
    if (sw->heap_n > 0) {
        clr_used_heap(sw->heap_pos, sw->heap_n);
    }
    memset(sw, 0, 8);
}
void move_set(void) {
    SETW *p = set_w_top;
    if (p != 0) {
        do {
            if (p->pad0 != 0) {
                p->move(p);
            }
            p = p->next;
        } while (p != 0);
    }
}
void trans_set(void) {
    SETW *p = set_w_top;
    if (p != 0) {
        do {
            if (p->pad0 != 0 && p->be_flag != 0) {
                if (p->trans != 0) {
                    p->trans(p);
                }
            }
            p = p->next;
        } while (p != 0);
    }
}
