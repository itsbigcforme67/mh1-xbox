/* SLPM_654.95 0x002269E0-0x002269F8: Stage_item_probability_get .. Stage_item_probability_get. See f_quest0_nm.c. */
#include "quest.h"

extern u8 *mission_area;
extern s32 *quest_data_tbl[];
extern s32 Item_get_tbl[];
void *memset(void *, int, unsigned int);














/* 0x2269E0 */
s32 Stage_item_probability_get(int n)
{
    return Item_get_tbl[n];
}
