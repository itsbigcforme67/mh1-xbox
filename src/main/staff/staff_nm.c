/* Staff roll (credits), SLPM_654.95 main 0x2907C0-0x290C60: Staff_init,
 * Staff_main (page step machine with fades, skip with the pad), logo_disp
 * (draws a logo sprite from logo_tbl) and staff_disp (draws one page of
 * staff_tbl: text lines or logos). Field meanings are guesses. */
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

void Staff_init(void)
{
    staff_w.step = 0;
    staff_w.x1 = 0;
    staff_w.x4 = 0;
    staff_w.page = 0;
    staff_w.timer = 0;
    staff_w.x8 = 0;
    staff_w.xA = 0;
}

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
        } else if (Psw[4] & 0x8000) {
            w->step = 4;
            fade_set(1);
        }
        break;
    case 2:
        if (Fade_busy_ck(1) != 1) {
            w->page++;
            if (w->page >= 0x22) {
                w->step++;
                str_stop(0);
                return 0;
            }
            w->step = 0;
            w->timer = 0;
        } else if (Psw[4] & 0x8000) {
            w->step = 4;
        }
        break;
    case 3:
        return 0;
    case 4:
        if (Fade_busy_ck(1) != 1) {
            str_stop(0);
            return 0;
        }
        break;
    }
    staff_disp(w->page);
    return 1;
}

typedef struct {
    s16 x, y, w, h;     /* 0x00 */
    u32 col;            /* 0x08 */
    s16 u, v, u2, v2;   /* 0x0C */
} SPR;

void logo_disp(x, y, no)
s16 x;
s16 y;
u8 no;
{
    SPR spr;
    int i = no * 5;
    s16 w, h;

    SetFilterMode(1);
    reload_tex(1, logo_tbl[i + 4] + 0xEA);
    SetTextureStage(logo_tbl[i + 4] + 0xEA);
    spr.x = x;
    spr.y = y;
    w = logo_tbl[i + 2];
    spr.w = w;
    h = logo_tbl[i + 3];
    spr.h = h;
    spr.col = -1;
    spr.u = logo_tbl[i];
    spr.v = logo_tbl[i + 1];
    spr.u2 = spr.u + spr.w;
    spr.v2 = spr.v + spr.h;
    flps0008(&spr, -1, h, w);
}

void staff_disp(page)
u8 page;
{
    STAFF_LINE *e;
    s16 y;
    s16 size;
    s16 x;

    e = staff_tbl[page];
    if (e->x != 0x3E7) {
        do {
            if (e->y != 0) {
                y = e->y;
            }
            switch (e->kind) {
            default:
            case 0:
                if (e->size == 0) {
                    size = 0x12;
                } else {
                    size = e->size;
                }
                x = e->x;
                if (e->kind == 1) {
                    x = e->x - (size / 2 * strlen(e->str) >> 1);
                }
                flfntSetSize(size, size);
                font_print_ex(x, y, e->col, lit_403_00386408, e->str);
                break;
            case 2:
                logo_disp(e->x, y, e->size);
                break;
            }
            e++;
            y = y + (s16)(size + 4);
        } while (e->x != 0x3E7);
    }
}
