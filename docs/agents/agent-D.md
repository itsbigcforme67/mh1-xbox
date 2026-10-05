# Agent D notes (main-module effect and set files)

Every "match" below was checked with tools/check.py (all functions OK) and
`tools/rebuild.sh main` printing "main OK" (byte-identical).

## eft26 (0x27CA50-0x27CF10) - 7/7 match
Marker model spinning and bobbing above the player PLW+0x3B0 points at,
tinted from col_tbl by the target's +0x8ED. What +0x3B0 points to is a guess.
- `if (x == 0) n = 0; else n = y;` gave the original branch layout; the
  ternary and the `!= 0` if/else did not (eft26_m).
- Reads a lobby.bin byte by absolute address (0x6EAED6, no relocation in the
  original), written as `*(s8 *)0x6EAED6`.

## eft01 (0x101E40-0x102BC8) - 9/9 match
Shadows: skinned shadow model (kinds 0/3, 3 snaps every bone to the ground
through SetSkinTransKKK), two foot blobs (1), five joint blobs (2, one prim
each) and the monster blob (4, enemy_shadow_size; the lobby copy when
game_w+0x1DC is set).
- References to overlay tables from main (enemy_shadow_size in game.bin,
  enemy_shadow_size_lb in lobby.bin) are written as externs D_63BC40 /
  D_610300 (names from config/main_undefined_syms_auto.txt); they link.
- SetSkinTransKKK: `g = GetGroundHit(v); y = g + (5 + 0.3*h)` with a local
  g; writing the call inside the expression swapped the add.s operands.
- eft01_m: a table indexed `type02_tbl[i]` in a loop, not a walking pointer
  (the pointer version increments in the wrong order).
- eft01_t: declbf found the saved-register order (mw, mats, chr, cl, tbl, i);
  stack matrices declared in reverse of their stack order.

## set21 (0x225FC0-0x2267EC) - 12/12 match (first try)
A model held between a monster's joints 6 and 9; thrown when animation 0x432
hits frame 48, flies 10 frames to a per-stage spot (stages 0x51-0x55), Eft13
puff, stays 300 frames. Small per-stage tables declared with their real
sizes so the s16 angle tables are gp-relative.
