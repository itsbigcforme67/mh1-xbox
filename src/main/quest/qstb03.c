/* SLPM_654.95 0x002269E0-0x00226C24: Stage_item_probability_get .. Quest_init. */
#include "quest.h"

extern u8 *mission_area;
extern s32 *quest_data_tbl[];
extern s32 Item_get_tbl[];
void *memset(void *, int, unsigned int);




#define QOFS(off) ((off) != 0 ? (void *)((off) + (int)mission_area) : 0)










/* 0x2269E0 */
s32 Stage_item_probability_get(int n)
{
    return Item_get_tbl[n];
}

/* 0x226A00: list `which` (0: model kinds, 1: QEM entries) of a monster
 * list header; 0 or -1 is returned as is */
s32 *Em_data_com_adrs_get(s32 *p, int which)
{
    s32 v = which == 0 ? p[2] : p[3];

    if (quest_w.no == 0) {
        return (s32 *)v;
    }
    if (v == 0 || v == -1) {
        return (s32 *)v;
    }
    return (s32 *)(v + (int)mission_area);
}

/* 0x226A60: the per-stage monster list of stage `id` in table p[idx]
 * (16-byte entries {stage, ?, kinds, QEMs}, ended by 0) */
s32 *Em_data_st_adrs_get(s32 *p, int id, int which, s8 idx)
{
    s32 *q;
    s32 v;

    if (quest_w.no == 0) {
        return 0;
    }
    p += idx;
    if (*p == 0) {
        return 0;
    }
    q = (s32 *)(*p + (int)mission_area);
    for (;;) {
        if (q[0] == 0) {
            return 0;
        }
        if (q[0] == id) {
            break;
        }
        q += 4;
    }
    v = which == 0 ? q[2] : q[3];
    if (v == 0 || v == -1) {
        return (s32 *)v;
    }
    return (s32 *)(v + (int)mission_area);
}

/* 0x226B10 */
void *Start_item_data_adrs_get(void)
{
    u8 *m = mission_area;
    if (quest_w.no == 0) {
        return quest_data_tbl[2];
    }
    return (void *)(((s32 *)m)[2] + (int)m);
}

/* 0x226B40: clear quest_w and point it at the free-hunt tables */
void Quest_init(void)
{
    memset(&quest_w, 0, 0x190);
    quest_w.x6C = quest_data_tbl[4];
    quest_w.x38 = -1;
    quest_w.x36 = -1;
    quest_w.x70 = quest_data_tbl[12];
    quest_w.x84 = quest_data_tbl[12];
    quest_w.x74 = quest_data_tbl[5];
    quest_w.x78 = quest_data_tbl[6];
    quest_w.x7C = quest_data_tbl[7];
    quest_w.x80 = quest_data_tbl[8];
    quest_w.x88 = quest_data_tbl[3];
    quest_w.x8C = quest_data_tbl[10];
    quest_w.x90 = quest_data_tbl[11];
    quest_w.x94 = (MISSION2 *)quest_data_tbl[0];
}
