/* Shell work pool (SLPM_654.95 0x00158F20-0x00159378): 64 SHLW records (0x1A8 bytes each, shell_work)
 * kept as a free-list stack (shell_sp / shell_ctr) plus a doubly linked list of live entries (shell_w_top,
 * links +0xC prev / +0x10 next); same scheme as the set pool in src/main/set/setwork.c. Entries borrow
 * 512-byte blocks from the shared work heap. move_shell runs each live shell's move callback
 * (after counting down its +0x7B delay), trans_shell its draw callback while the owner (player or monster,
 * selected by +0x7A) is alive. */
#include "types.h"
#include "shell.h"
#include "game.h"
#include "pl.h"
#include "em.h"

extern SHLW shell_work[0x40];
extern SHLW **shell_sp;
extern s32 shell_ctr;
extern SHLW *shell_w_top;
void *memset(void *, int, unsigned long);
int get_start_heap(int);
u8 *get_heap_ptr(int);
void set_used_heap(int, int);
void clr_used_heap(int, int);

void init_shell_work(void) {
    int i;
    SHLW *p;
    memset(shell_work, 0, 0x3500);
    p = &shell_work[0x3F];
    shell_w_top = 0;
    shell_sp = (SHLW **)shell_work;
    for (i = 0; i < 0x40; i++) {
        *--shell_sp = p;
        p--;
    }
    shell_ctr = 0x40;
    game_w.shl10_num = 0;
    game_w.trap_num = 0;
}

void push_shell_work(SHLW *sh);

void clr_shell_work(void) {
    SHLW *p = shell_w_top;
    while (p != 0) {
        if (p->pad0 != 0 && p->x09 == 0) {
            SHLW *q = p;
            p = p->next;
            push_shell_work(q);
        } else {
            p = p->next;
        }
    }
}

SHLW *pull_shell_work(int n) {
    int start;
    SHLW *sh;
    if ((start = get_start_heap(n)) < 0) {
        return 0;
    }
    if (shell_ctr == 0) {
        return 0;
    }
    shell_ctr--;
    sh = *shell_sp++;
    sh->prev = 0;
    sh->next = shell_w_top;
    shell_w_top = sh;
    if (sh->next != 0) {
        sh->next->prev = sh;
    }
    sh->pad0 = 1;
    sh->senko = (struct SENKO *)get_heap_ptr(start);
    sh->heap_pos = start;
    sh->heap_n = n;
    set_used_heap(start, n);
    sh->xB4 = 0;
    sh->xC4 = 0;
    sh->x09 = 0;
    return sh;
}

void push_shell_work(SHLW *sh) {
    if (sh->prev == 0) {
        shell_w_top = sh->next;
    } else {
        sh->prev->next = sh->next;
    }
    if (sh->next != 0) {
        sh->next->prev = sh->prev;
    }
    shell_ctr++;
    *--shell_sp = sh;
    clr_used_heap(sh->heap_pos, sh->heap_n);
    memset(sh, 0, 0xC);
    sh->flag = 0;
    sh->x88 = 0;
    sh->x8C = 0;
    sh->prim_no = 0;
    sh->prim = 0;
}

void move_shell(void) {
    SHLW *p = shell_w_top;
    if (p != 0) {
        do {
            if (p->pad0 != 0) {
                if (p->x7B != 0) {
                    p->x7B--;
                } else {
                    p->move(p);
                }
            }
            p = p->next;
        } while (p != 0);
    }
}

void trans_shell(void) {
    SHLW *p = shell_w_top;
    if (p != 0) {
        do {
            if (p->x7A == 0) {
                PLW *pl = &player_work[p->em_no];
                if (p->pad0 != 0 && p->be_flag != 0 && pl->x01 != 0 && p->trans != 0) {
                    p->trans(p);
                }
            } else {
                EMW *em = &em_work[p->em_no];
                if (p->pad0 != 0 && p->be_flag != 0 && em->x01 != 0 && p->trans != 0) {
                    p->trans(p);
                }
            }
            p = p->next;
        } while (p != 0);
    }
}
