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

## eft02 (0x27D6E0-0x27EF58) - 13/14 match
Hit sparks and blood. eft02.c (move/i/m/d/e, 0x27D6E0-0x27DDB8, jump tables
0x384170-0x3841F0 incl. alignment pad) and eft02b.c (8 spawners,
0x27E940-0x27EF58, table 0x384220-0x384240) are built. eft02_t stays asm:
src/main/eft/eft02_nm.c (whole file) is 12 instructions off, all in case
9-11 (a1/a2 swap for the clay index temp and where `col = -1` is
scheduled); 20 minutes of permuter found nothing better.
- Float constants that are one ulp above the obvious literal come from
  folded expressions: 0x39D1B718 = `0.4f / 1000.0f` (0.0004f gives ...717),
  0x3C23D70B = `0.1f * 0.1f` (0.01f gives ...70A). Found by compiling the
  candidates with MWCC.
- `mw->clay + ew->timer / 2 + 97` (pointer + index, then constant), not
  `&mw->clay[97 + t/2]`, matches the add order.
- A u8 field read into a u32 local (`k = ew->arg` before a call) explains a
  value kept in a saved register and converted with the unsigned sequence.
- Eft_rendope_set takes a u16 (callers pass the u16 flags without andi).
Shared header: pl.h carves PLW+0x3EC (u16 x3EC) from _pad3D2.

## set13 (0x1569E0-0x158F18) - 6/10 match so far
Sun glare / lens flare (guess from sun_pos_tbl and camera maths).
set13.c (Set13_set/set2/move/i, 0x1569E0-0x157080) and set13b.c (d/e,
0x158130-0x158188) are built. set13_m (4.3 KB) and set13_trans (3.1 KB) not
attempted yet (fused mula.s/madd.s maths, an inlined angle helper).
src/main/set/set13c.c (not registered): set13_hit_calc matches,
set13_disp_pos_calc is 13 off (the original loads dir[1], dir[2] before the
first store; no natural source found yet).
- `PLW *pl = &player_work[game_w.master];` as an initialiser (not a later
  statement) gives the original's early address computation (set13_i).
- `if ((sw = pull_set_work(0)) != 0)` tests v0 before the copy to s0
  (Set13_set2, Eft02_set2), where `sw = ...; if (sw != 0)` tests s0.
- set13_hit_calc: `p = (f32 *)((u8 *)p + 8)` keeps two separate +8 steps
  that `p += 2` lets the compiler merge (found from a permuter hint).

## eft06 (0x102BD0-0x105B10) - 17/19 match
Hit sparks with ten types; pieces are 0x38 bytes (EFT06_PIECE).
eft06.c (move, i, init_subs, pw_die_ck, continue, type_ck; 0x102BD0-
0x103A38, tables 0x3579C0-0x357A08), eft06b.c (d/e) and eft06c.c (se_req,
set_com, Eft06_set/set2/set_hit) built. eft06_m (4.8 KB) and eft06_t
(2.2 KB) still asm, not attempted.
- eft06_i: the main loop must index `w[i]` (the compiler's own pointer
  induction then increments first); `p++` in the for header put the
  pointer increment last.
- Eft06_set takes the float scale as its LAST parameter: argument set-up
  order of the recursive call follows parameter order.
- eft06_continue's count parameter is s16 (int gave an extra sign-extend).

set13c.c (set13_hit_calc, 0x158E00-0x158F18) is now built too; the
near-match set13_disp_pos_calc moved to src/main/set/set13_nm.c.

## eft13 (0x105B10-0x109E28) - 13/19 match
Dust, splashes and debris (35 types). Built: eft13.c (move), eft13b.c
(d/e), eft13c.c (se_req, water_ck, set_sub), eft13d.c (eft13_set,
Eft13_set_scl), eft13e.c (Eft13_set_em/_em_scl/_pos/_pos2, water_set; jump
table 0x357D70-0x357D98). Not attempted (big): eft13_i, _m, _t, _set_pos,
_set_sub_em, _set_pos_em.
- Main calls game.bin's Eft08_set/Eft08_set2 by address with no symbol:
  call them as func_544C90 / func_544D20 (config/main_undefined_funcs_auto.txt).
  check.py shows these calls as differing ("original calls ?"); the build
  links them correctly.
- eft13_water_set: `sc = 2.7f * scale` into a new local gives the
  original's const*reg multiply order; reusing the parameter swaps it.
  Statement order inside cases matters (case 2 has `pos[1] += 5` before
  the scale, case 8 after).
- A parameter only passed on to other functions: declaring it s16 makes
  MWCC re-extend it at each call to a function defined in ANOTHER file,
  while a callee defined earlier in the same file is trusted. After the
  split, eft13_set/Eft13_set_scl need `int j` to keep the raw pass-through.
- game_w+0x1E is read as a u16 here (`*(u16 *)&game_w.x1E`); game.h names
  it as a u8 (eft12), left unchanged.

## eft20 (0x218590-0x21D464) - 9/13 match
Monster dust/debris, sibling of eft13. Built: eft20.c (move, se_req),
eft20b.c (d/e), eft20c.c (water_ck), eft20d.c (Eft20_set/_set2/_set_pl,
water_set). Not attempted (big): eft20_i (4.3 KB), _m (4.6 KB), _t (5.1 KB),
_pos_set (3.5 KB). Same lessons as eft13 (func_544C90 calls, `sc` local).
