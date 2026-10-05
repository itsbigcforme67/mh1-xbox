/* lb_av02 - browser work pool (IPA static pull/push) 0x005E9DF0-0x005E9EC8: BsTextureLoad. Whole file in lb_av.c. */
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

void BsTextureLoad(int a, u8 *p) {
    u8 st;
    switch (a) {
    case 3:
        st = *p;
        switch (st) {
        case 0:
            load_file_mdl(data_load_ptr, 0x885);
            *p = *p + 1;
            break;
        case 1:
            BsTextureAdd(flCreateTextureFromTim2_mem(data_load_ptr, 0));
            *p = *p + 1;
            break;
        case 2:
            *p = st + 1;
            break;
        case 3:
            *p = st + 1;
            break;
        case 4:
            break;
        }
    }
}
