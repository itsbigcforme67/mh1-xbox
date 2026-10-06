/* lb_gav01 - near-match fixes 0x005E9960-0x005E9A1C: BsWorkInitAll. Whole file in lb_av.c. */
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

void BsWorkInitAll(void) {
    int i;
    BSWK **h;
    BSWK **l;
    BSWK *w;
    int n;
    i = 0;
    h = Bs_work_line_head;
    l = Bs_work_line_last;
    do {
        h[0] = 0;
        l[0] = 0;
        i += 3;
        h[1] = 0;
        l[1] = 0;
        h[2] = 0;
        l[2] = 0;
        h += 3;
        l += 3;
    } while (i < 9);
    w = Bs_work;
    Bs_work_hit_head = 0;
    n = 0x200;
    Bs_work_hit_last = 0;
    Bs_work_free_head = Bs_work;
    if (n-- != 0) {
        do {
            memset(w, 0, 0x70);
            if (n != 0) {
                w->next = w + 1;
            }
            w += 1;
        } while (n-- != 0);
    }
}
