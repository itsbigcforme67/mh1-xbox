/* em_cmd_r37 - agent D 7 Oct 0x005641A0-0x00564204: em_cmd_position_set. Whole file in em_cmd_nm.c. */
#include "em_cmd.h"











































































































































u8 *em_cmd_position_set(EMW *em, u8 *p)
{
  f32 (*row)[3];
  f32 *v;
  row = em_cmd_pos_tbl[em->kind];
  v = row[*p];
  em->pos[0] = v[0];
  em->pos[1] = v[1];
  em->pos[2] = v[2];
  row = &em->pos;
  em->x5A0[0] = (*row)[0];
  em->x5A0[1] = (*row)[1];
  em->x5A0[2] = (*row)[2];
  return p + 1;
}
