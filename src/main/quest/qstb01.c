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
#ifdef VERSION_MHG
    *((u8 *)&quest_w + QWO(0x154)) = 1;
#else
    *((u8 *)&quest_w + QWO(0x150)) = 1;
#endif
}

/* 0x226840: quests 0x66-0x6A (the "f_dra" quests) */
#ifdef VERSION_MH2
int Quest_f_dra_ck(int q)
{
    switch (q) {
    case 621:
    case 622:
    case 623:
    case 624:
        return 1;
    }
    return 0;
}
#else
int Quest_f_dra_ck(int unused)
{
    switch (quest_w.no) {
#ifdef VERSION_MHG
    case 3004: /* G: quest numbers moved */
    case 3005:
    case 3006:
    case 3007:
#else
    case 0x66:
    case 0x67:
    case 0x68:
    case 0x69:
    case 0x6A:
#endif
        return 1;
    }
    return 0;
}
#endif
