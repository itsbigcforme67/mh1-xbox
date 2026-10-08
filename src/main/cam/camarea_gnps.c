/* camarea_gnps - camera areas (SLPM_654.95 0x00223870-0x00223978): get_near_point_sub, the nearest end of a rail section for the
 * projected parameters t[0..n-1] (-1 before the start, 1 past the end, 0 inside: out gets the clamped parameter). The new_var temps
 * are what the permuter found (a copy of t[0] and of v - seg[3] before the store). Whole file in camarea_nm.c. */
#include "types.h"

s32 get_near_point_sub(f32 *out, f32 *coef, f32 *seg, f32 *t, s32 n) {
  s32 r;
  f32 best;
  f32 new_var;
  f32 v;
  f32 new_var2;
  new_var2 = t[0];
  v = new_var2;
  if (v < 0.0f)
  {
    *out = 0.0f;
    r = -1;
    best = -v;
  }
  else
    if (!(v <= seg[3]))
  {
    best = v - seg[3];
    r = 1;
    *out = seg[3];
  }
  else
  {
    *out = v;
    return 0;
  }
  if (n == 1)
  {
    return r;
  }
  for (n--; n != 0; n--)
  {
    t++;
    v = *t;
    if (v < 0.0f)
    {
      if (!(best <= (-v)))
      {
        best = -v;
        r = -1;
        *out = 0.0f;
      }
    }
    else
      if (!(v <= seg[3]))
    {
      if (!(best <= (v - seg[3])))
      {
        new_var = v - seg[3];
        *out = seg[3];
        r = 1;
        best = new_var;
      }
    }
    else
    {
      *out = v;
      return 0;
    }
  }

  return r;
}
