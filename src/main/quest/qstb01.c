/* SLPM_654.95 0x00226830-0x00226840: Quest_failed_set .. Quest_failed_set. See f_quest0_nm.c. */
#include "quest.h"

extern u8 *mission_area;
extern s32 *quest_data_tbl[];
extern s32 Item_get_tbl[];
void *memset(void *, int, unsigned int);














/* 0x226830 */
void Quest_failed_set(void)
{
    *((u8 *)&quest_w + 0x150) = 1;
}
