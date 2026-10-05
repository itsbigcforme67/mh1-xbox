/* SLPM_654.95 0x00226B40-0x00226C24: Quest_init .. Quest_init. See f_quest0_nm.c. */
#include "quest.h"

extern u8 *mission_area;
extern s32 *quest_data_tbl[];
extern s32 Item_get_tbl[];
void *memset(void *, int, unsigned int);














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
