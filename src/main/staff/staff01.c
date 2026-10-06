/* SLPM_654.95 0x00290AE0-0x00290C54: staff_disp .. staff_disp. See staff_nm.c. */
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
void flfntSetSize(u8, u8);
void font_print_ex(s16, s16, int, char *, ...);
u32 strlen();
void staff_disp();
void logo_disp(s16, s16, u8);



typedef struct {
    s16 x, y, w, h;     /* 0x00 */
    u32 col;            /* 0x08 */
    s16 u, v, u2, v2;   /* 0x0C */
} SPR;



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
                y = (int)e->y;
            }
            switch (e->kind) {
            case 1:
            case 0:
            default:
                if (e->size == 0) {
                    size = 0x12;
                } else {
                    size = e->size;
                }
                if (e->kind == 1) {
                    x = e->x - (size / 2 * strlen(e->str) >> 1);
                } else {
                    x = e->x;
                }
                flfntSetSize(size, size);
                font_print_ex(x, y, e->col, lit_403_00386408, e->str);
                break;
            case 2:
                logo_disp(e->x, y, e->size);
                break;
            }
            y += (s16)(size + 4);
            e++;
        } while (e->x != 0x3E7);
    }
}
