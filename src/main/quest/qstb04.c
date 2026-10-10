/* qstb04 - f_quest0 0x002267F0-0x00226830: Modori_dama_ck. */
#include "quest.h"

extern u8 *mission_area;
extern s32 *quest_data_tbl[];
extern s32 Item_get_tbl[];
void *memset(void *, int, unsigned int);




#define QOFS(off) ((off) != 0 ? (void *)((off) + (int)mission_area) : 0)










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
