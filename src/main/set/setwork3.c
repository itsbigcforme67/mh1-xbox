/* setwork3 - get_heap_ptr (SLPM_654.95 0x00154FF0-0x00155000). Whole file in setwork_nm.c. */
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
u8 *get_heap_ptr(int n) {
    return (u8 *)((int)work_heap_area + (n << 9));
}
