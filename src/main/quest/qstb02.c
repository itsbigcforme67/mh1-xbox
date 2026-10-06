/* SLPM_654.95 0x00226900-0x00226914: Stage_data_get .. Stage_data_get. See f_quest0_nm.c. */
#include "quest.h"

extern u8 *mission_area;
extern s32 *quest_data_tbl[];
extern s32 Item_get_tbl[];
void *memset(void *, int, unsigned int);




#define QOFS(off) ((off) != 0 ? (void *)((off) + (int)mission_area) : 0)










/* 0x226900: stage n's 32-byte entry */
void *Stage_data_get(int n)
{
    return (u8 *)quest_w.x80 + n * 32;
}
