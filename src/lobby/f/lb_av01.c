/* lb_av01 - browser work pool (IPA static pull/push) 0x005E9A20-0x005E9D44: bs_pul_wk, bs_psh_wk, BsWorkPull, BsWorkPush, BsWorkMove, BsWorkTrans, BsTextureFreeAll. Whole file in lb_av.c. */
#include "lobby_f.h"
extern BSWK *Bs_work_line_last[9];
extern BSWK *Bs_work_line_head[9];
extern BSWK Bs_work[0x200];
extern BSWK *Bs_work_hit_head;
extern BSWK *Bs_work_hit_last;
extern BSWK *Bs_work_free_head;
extern s32 Bs_tex_handle[0x18];
extern s8 Bs_tex_handle_ptr;
extern s32 data_load_ptr;
void flReleasePaletteHandle();
void flReleaseTextureHandle();
void flCompact();
void load_file_mdl();
int flCreateTextureFromTim2_mem();
int BsTextureAdd();

static BSWK *bs_pul_wk(void) {
    BSWK *n;
    n = Bs_work_free_head;
    if (n != 0) {
        Bs_work_free_head = n->next;
    }
    return n;
}

static void bs_psh_wk(BSWK *w) {
    memset(w, 0, 0x70);
    w->next = Bs_work_free_head;
    Bs_work_free_head = w;
}

BSWK *BsWorkPull(int line, int tail) {
    BSWK *w;
    BSWK *p;
    int off;
    w = bs_pul_wk();
    if (w == 0) {
        return 0;
    }
    w->x03 = line;
    if (tail == 0) {
        p = Bs_work_line_head[line];
        w->next = p;
        Bs_work_line_head[line] = w;
        w->prev = 0;
        if (p == 0) {
            Bs_work_line_last[line] = w;
            return w;
        }
        p->prev = w;
        return w;
    }
    p = Bs_work_line_last[line];
    w->prev = p;
    Bs_work_line_last[line] = w;
    w->next = 0;
    if (p == 0) {
        Bs_work_line_head[line] = w;
        return w;
    }
    p->next = w;
    return w;
}

void BsWorkPush(BSWK *w) {
    BSWK *p;
    BSWK *n;
    p = w->prev;
    if (p == 0) {
        p = w->next;
        Bs_work_line_head[w->x03] = p;
        if (p == 0) {
            Bs_work_line_last[w->x03] = 0;
        } else {
            p->prev = 0;
        }
    } else {
        n = w->next;
        if (n == 0) {
            Bs_work_line_last[w->x03] = p;
            p->next = 0;
        } else {
            p->next = n;
            w->next->prev = w->prev;
        }
    }
    bs_psh_wk(w);
}

void BsWorkMove(int line) {
    BSWK *n;
    BSWK *w;
    w = Bs_work_line_head[line];
    if (w != 0) {
        do {
            n = w->next;
            if (w->x00 != 0) {
                ((void (*)(BSWK *))w->task)(w);
            }
            w = n;
        } while (n != 0);
    }
}

void BsWorkTrans(int line) {
    BSWK *n;
    BSWK *w;
    w = Bs_work_line_head[line];
    if (w != 0) {
        do {
            n = w->next;
            if (w->x01 != 0) {
                ((void (*)(BSWK *))w->trans)(w);
            }
            w = n;
        } while (n != 0);
    }
}

void BsTextureFreeAll(void) {
    int i;
    s32 *p;
    s32 v;
    u32 h;
    u16 t;
    i = 0;
    p = Bs_tex_handle;
    if (0 < Bs_tex_handle_ptr) {
        do {
            v = *p;
            if (v != 0) {
                h = (u32)(v & 0xFFFF0000) >> 0x10;
                if (h != 0) {
                    flReleasePaletteHandle(h);
                }
                t = *p;
                if (t != 0) {
                    flReleaseTextureHandle(t);
                }
                *p = 0;
            }
            i += 1;
            p += 1;
        } while (i < Bs_tex_handle_ptr);
    }
    Bs_tex_handle_ptr = 0;
    flCompact();
}
