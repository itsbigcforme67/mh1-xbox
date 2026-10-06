/* f_quest, first part (SLPM_654.95 main 0x002267F0-0x00226C24, the
 * g_Modori_dama_ck asm file): mission-data accessors and Quest_init.
 * Written from the asm for the PC port (agent A); NOT built for the PS2 and
 * not compared with check.py. Field meanings are guesses.
 *
 * With a quest loaded (quest_w.no != 0) the tables hold offsets into
 * mission_area (0 / -1 = none); in free hunts (no == 0) they come from
 * quest_data_tbl and hold pointers. */
#include "quest.h"

extern u8 *mission_area;
extern s32 *quest_data_tbl[];
extern s32 Item_get_tbl[];
void *memset(void *, int, unsigned int);

/* 0x2267F0: can the return ball (modori dama) be used here: not in the
 * base camp stage of the area (game_w+0x2F == +0x14) and the mission
 * does not forbid it (flag 0x1000). */
int Modori_dama_ck(void)
{
  unsigned char new_var;
  if ((*(((u8 *) (&game_w)) + 0x2F)) == (*(((u8 *) (&game_w)) + 0x14)))
  {
    return 0;
  }
  new_var = (quest_w.x40 & 0x1000) == 0;
  new_var = (quest_w.x40 & 0x1000) == 0;
  if ((!(&quest_w)) && (!(&quest_w)))
  {
  }
  return new_var;
}

/* 0x226830 */
void Quest_failed_set(void)
{
    *((u8 *)&quest_w + 0x150) = 1;
}

/* 0x226840: quests 0x66-0x6A (the "f_dra" quests) */
int Quest_f_dra_ck(int unused)
{
    switch (quest_w.no) {
    case 0x66:
    case 0x67:
    case 0x68:
    case 0x69:
    case 0x6A:
        return 1;
    }
    return 0;
}

#define QOFS(off) ((off) != 0 ? (void *)((off) + (int)mission_area) : 0)

/* 0x2268A0: the exits list of stage n */
void *Stage_mv_data_get(int n)
{
    if (quest_w.no == 0) {
        return (void *)quest_w.x7C[n];
    }
    {
        s32 v = quest_w.x7C[n];
        if (v == 0) {
            return 0;
        }
        return (void *)(v + (int)mission_area);
    }
}

/* 0x226900: stage n's 32-byte entry */
void *Stage_data_get(int n)
{
    return (u8 *)quest_w.x80 + n * 32;
}

/* 0x226920 */
void *Stage_item_data_get(int n)
{
    if (quest_w.no == 0) {
        return (void *)quest_w.x8C[n + 1];
    }
    {
        s32 v = quest_w.x8C[n + 1];
        if (v == 0) {
            return 0;
        }
        return (void *)(v + (int)mission_area);
    }
}

/* 0x226980 */
void *Stage_unique_data_get(int n)
{
    if (quest_w.no == 0) {
        return (void *)quest_w.x90[n];
    }
    {
        s32 v = quest_w.x90[n];
        if (v == 0) {
            return 0;
        }
        return (void *)(v + (int)mission_area);
    }
}

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
