/* lb_v12 - guild quest picker 0x005CA080-0x005CA228: get_new_quest (random not-yet-cleared quest of a level, offline table or online table). */
#include "types.h"
int ran_suu();
extern u8 *quest_lv_tbl[];
extern u8 *quest_local_tbl[];
int Quest_clear_bit_ck();
int lb_key_quest_ck();
int Online_ck();
u8 get_new_quest(a)
int a;
{
    u8 *tbl;
    int k;
    int i;
    int n;
    int v;
    u8 *e;
    if (a == -1) {
        return 0;
    }
    if (Online_ck() == 1) {
        tbl = quest_lv_tbl[a];
    } else {
        tbl = quest_local_tbl[a];
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
            e = tbl + (i & 0xFF);
            if (Quest_clear_bit_ck(*e) == 0 && (Online_ck() == 0 || *e < 0x83) && lb_key_quest_ck(*e) == 0) {
                if (*e != 0x90 || Quest_clear_bit_ck(0x94) == 1) {
                    return *e;
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
