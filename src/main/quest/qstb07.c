/* qstb07 - 0x00229570-0x00229684: the stack of extra pick-up points (stiem_stack_tbl, quest_w+0x3B entries, at most
 * 20): ext_pick_point_fifo_ck frees the oldest entry that is still "open" when the stack is full (the loop is a
 * do-while behind `0 < n`), ext_pick_point_tbl_clr removes entry n and shifts the rest down. The s8 argument of
 * tbl_clr is widened once (`int n = (s8)arg`), not kept as s8. Whole part in f_quest_nm.c. */
#include "quest.h"

void Ext_pick_point_clr();

void ext_pick_point_fifo_ck(void)
{
    int n;
    int i;
    s8 *t;

    n = quest_w.x3B;
    if (n >= 20) {
        n--;
        i = 0;
        if (0 < n) {
            t = stiem_stack_tbl;
            do {
                if (!(StiEM_data[*t].x19 & 1)) {
                    break;
                }
                i++;
                t++;
            } while (i < n);
        }
        Ext_pick_point_clr(stiem_stack_tbl[i]);
    }
}

void ext_pick_point_tbl_clr(arg)
int arg;
{
    int n = (s8)arg;
    s8 *t;
    int last;

    last = quest_w.x3B - 1;
    if (n < last) {
        t = stiem_stack_tbl + n;
        do {
            n++;
            t[0] = t[1];
            t++;
        } while (n < last);
    }
    stiem_stack_tbl[n] = -1;
    quest_w.x3B--;
}
