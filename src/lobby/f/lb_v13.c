/* lb_v13 - guild flag quest picker 0x005C9DF0-0x005C9F80: get_flag_quest (random not-yet-cleared flag quest, skips the 0x67-0x6A block). */
#include "types.h"
int ran_suu();
extern u8 *flag_quest_tbl[];
extern u8 *flag_quest_tbl_local[];
int Quest_clear_bit_ck();
int Online_ck();
u8 get_flag_quest(a)
int a;
{
    u8 *tbl;
    int k;
    int i;
    int n;
    int v;
    u8 *e;
    u8 w;
    if (a == -1) {
        return 0;
    }
    if (Online_ck() == 1) {
        tbl = flag_quest_tbl[a];
    } else {
        tbl = flag_quest_tbl_local[a];
    }
    v = 0;
    if (*tbl != 0) {
        do {
            v = (v + 1) & 0xFF;
        } while (tbl[v] != 0);
    }
    n = v & 0xFF;
    v = (ran_suu(1) & 0xFFFF) % n;
    k = 0;
    i = v & 0xFF;
    if (0 < n) {
        do {
            w = i;
            if (w < 0x67 || w > 0x6A) {
                e = tbl + (i & 0xFF);
                if (Quest_clear_bit_ck(*e) == 0) {
                    if (*e != 0x90 || Quest_clear_bit_ck(0x94) == 1) {
                        return *e;
                    }
                }
            }
            i = (i + 1) & 0xFF;
            if (i >= n) {
                i = 0;
            }
            k = (k + 1) & 0xFF;
        } while (k < n);
    }
    return 0;
}
