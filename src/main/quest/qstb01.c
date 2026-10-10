/* SLPM_654.95 0x00226830-0x002268A0: Quest_failed_set .. Quest_f_dra_ck. */
#include "quest.h"

extern u8 *mission_area;
extern s32 *quest_data_tbl[];
extern s32 Item_get_tbl[];
void *memset(void *, int, unsigned int);




#define QOFS(off) ((off) != 0 ? (void *)((off) + (int)mission_area) : 0)










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
