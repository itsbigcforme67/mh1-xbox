/* em_cmd_r90 - near-match fixes: em_cmd_pl_ride_ck,area_route_rnd32. Whole file in em_cmd_nm.c. 0x00563080-0x005631E0: em_cmd_pl_ride_ck. Whole file in em_cmd_nm.c. */
#include "em_cmd.h"











































































































































u8 *em_cmd_pl_ride_ck(EMW *em, u8 *p)
{
  u8 *q;
  PLW *new_var;
  u16 ok;
  s8 i;
  u8 n;
  q = p;
  ok = 1;
  switch (*(q++))
  {
    case 0:
      n = *((u8 *) 0x3F34C3);
      new_var = player_work;
      for (i = 0; i < n; i++)
    {
      if (new_var[i].flag604 != 0)
      {
        ok = 0;
      }
    }

      if (ok)
    {
      if (ok)
      {
        for (;;)
        {
          if ((q[0] == 0x66) && (q[1] == 1))
          {
            break;
          }
          if ((q[0] == 0x66) && (q[1] == 2))
          {
            break;
          }
          q = cmd_end_search(em, q, 0x66, 2);
          if (!ok)
          {
            break;
          }
        }

      }
      q = next_cmd_search(em, q);
      if ((q[0] == 0x66) && (q[1] == 2))
      {
        q = next_cmd_search(em, q);
      }
    }
      ;
      break;

    case 1:
      q = else_ck(em, q, 0x66);
      break;

    case 2:
      break;

  }

  return q;
}
