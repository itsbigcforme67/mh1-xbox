/* Staff roll (credits) 0x2907C0-0x2907FC (rest in staff_nm.c): Staff_init,
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

