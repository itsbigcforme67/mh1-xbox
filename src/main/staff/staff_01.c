/* SLPM_654.95 0x00290800-0x002909B8: Staff_main .. Staff_main. See staff_nm.c. */
#include "types.h"

typedef struct {
    u8 step;    /* 0x0 */
    u8 x1;      /* 0x1 */
    u8 _pad2[2];
    u8 x4;      /* 0x4 */
    u8 page;    /* 0x5 */
    s16 timer;  /* 0x6 */
    s16 x8;     /* 0x8 */
    s16 xA;     /* 0xA */
} STAFF_W;

extern STAFF_W staff_w;
extern u16 Psw[];
extern char lit_403_00386408[];
typedef struct {
    s16 x;      /* 0x0 (0x3E7 ends a page) */
    s16 y;      /* 0x2 (0: continue below the previous line) */
    u8 kind;    /* 0x4: 0 text, 1 centered text, 2 logo */
    u8 col;     /* 0x5 */
    u8 size;    /* 0x6: font size, or logo number */
    u8 _pad7;
    char *str;  /* 0x8 */
} STAFF_LINE;

extern STAFF_LINE *staff_tbl[];
extern s16 logo_tbl[];

u8 Fade_busy_ck();
void fade_set();
void str_stop();
void SetFilterMode();
void reload_tex();
void SetTextureStage();
void flps0008();
void flfntSetSize();
void font_print_ex();
int strlen();
void staff_disp();



typedef struct {
    s16 x, y, w, h;     /* 0x00 */
    u32 col;            /* 0x08 */
    s16 u, v, u2, v2;   /* 0x0C */
} SPR;



int Staff_main(void)
{
    STAFF_W *w = &staff_w;

    switch (w->step) {
    case 0:
        if (Fade_busy_ck(1) != 1) {
            w->step++;
            fade_set(2);
        }
        return 1;
    case 1:
        w->timer++;
        if (w->timer >= 0xD2) {
            w->step++;
            fade_set(1);
        } else if (Psw[2] & 0x8000) {
            w->step = 4;
            fade_set(1);
        }
        break;
    case 2:
        if (Fade_busy_ck() != 1) {
            w->page++;
            if (w->page >= 0x22) {
                w->step++;
                str_stop(0);
                return 0;
            }
            w->step = 0;
            w->timer = 0;
        } else if (Psw[2] & 0x8000) {
            w->step = 4;
        }
        break;
    case 3:
        return 0;
    case 4:
        if (Fade_busy_ck() != 1) {
            str_stop(0);
            return 0;
        }
        break;
    }
    staff_disp(w->page);
    return 1;
}
