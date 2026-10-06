/* lb_dd12 - browser: check_rowspan2 0x00606070-0x0060615C. Hand-written from the asm (working copy in lb_dr2.c). */
#include "lobby_f.h"
extern u8 *bsw;

u8 *check_upTD_rowspan2();
int check_rowspan_sub();
int SetTableData();
void set_TH_TD_data_1st();
void init_td_data();
int check_rowspan2(u8 *prev, u8 *o) {
    u8 *cell;
    s32 temp_s3;
    u8 *w;
    u8 *c;
    u8 *r;
    for (;;) {
        init_td_data();
        if (SetTableData(4) < 0) {
            return -1;
        }
        w = bsw;
        c = (u8 *)((*(u16 *)(w + 0xD894) * 0x5C) + (int)w);
        cell = c + 0x24E0;
        temp_s3 = *(s32 *)(cell);
        if (temp_s3 == 0) {
            return -1;
        }
        w[0x18D] = 1;
        set_TH_TD_data_1st(cell, temp_s3);
        if (check_rowspan_sub(cell, temp_s3, prev) < 0) {
            return -1;
        }
        r = check_upTD_rowspan2(o);
        prev = r;
        if (r == 0) {
            return 0;
        }
    }
}
