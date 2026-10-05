# Agent C notes: monster (em) action files in game.bin

Reference for decompiling monster code. Everything here was checked with
tools/check.py and `tools/rebuild.sh game` (game OK) unless marked as a guess.

## Monster file structure (seen in f_em07/08/...)

Each small emNN file holds the action setters for one monster kind:
- emNN_act_act_set / move_act_set / fly_act_set / atk_act_set(EMW *em, u16 no, u16 arg):
  set em->act_spd (0x930) = 1.0f, sometimes tweak `no` or the per-monster
  work, then call em_act_set2(em, group, no, arg) with group 0/1/2/3.
- emNN_act_set(EMW *em, int kind, u16 no, u16 arg): runs
  em_cdm_act_flag_ck(em) when em->x8C3 == 0, then `switch ((u16)kind)`
  dispatches to the four setters above; kinds 4-6 call em_act_set2 directly.
- Some files add fly_adjy2_init / fly_adjy2_subx / suby / subz (file
  statics, renamed with their address by the split) / emNN_fly_adjy2, and
  senkai_* (turning) helpers.

Prototypes that matched:
- `void em_act_set2(EMW *, int group, u16 no, u16 arg);` (u16 so callers
  pass u16 params on without re-masking; with int params they get masked).
- `void em_cdm_act_flag_ck(EMW *);`
- `f32 CalcDistanceXZ(f32 *, f32 *); u16 Em_Calc_angY(f32 *, f32 *);`

## EMW fields added (include/em.h)

Fields used straight from EMW in the em files (so common to all monsters):
- 0x008 work08 (s32, em08 stores a turn time), 0x1A0 chr_spd0 (frame step,
  as in PLW), 0x1C4 x1C4, 0x302 x302 / 0x792 x792 (s16; em08 tests
  x302 < 10% of x792, maybe hit points, a guess), 0x388 x388 (cleared by
  act setters), 0x3B8 adj_y / 0x3BC adj_z (fly height/depth step),
  0x617 x617 (s8, -1 = none), 0x827/0x828/0x829 (bytes),
  0x882 x882, 0x940 area (EM_AREA*, +8 = per-stage point list
  EM_STG_POS {s16 stg; f32 (*pos)[3];}).

- 0x444 `ex[]`: per-monster work area. Its layout differs per monster
  (em07 keeps a distance float at +0x10 and a flag at +0x16; em08 uses +0xD
  and +0x34). Each emNN.c defines its own `EMNNW` struct and casts
  `(EMNNW *)em->ex`. The end of the area (0x50C) is a guess.
- 0x881 x881: non-zero when there is a target (value 7 seen in em08).
- 0x8C3 x8C3, 0x930 act_spd (guess at meaning), 0x934 tgt_pos[3].

## Matching lessons

- em07_move_act_set: the angle window test
  `(0 <= a && a <= 0x4000) || (a >= 0xC000 && a <= 0xFFFF)` on a u16 `a`
  must be written `0 <= a`; `a >= 0` gives bltz, `a > -1` is 2 off.
  The original has slt at, a, zero.
- em07_act_set: `kind` is an int param switched as `(u16)kind`; with a u16
  param MWCC reuses the masked value for the em_act_set2 call.

- Statics in em files keep their original names (fly_adjy2_suby...). The
  split names them with an address suffix only because several files have
  one; tools/check.py tries every candidate address, so the plain name
  matches. Use suffixed names temporarily if you want -v to diff against
  the right copy.
- fly_adjy2_suby: a statement before an `if` can show up in the delay slot
  of the if's branch (`ret = 2;` before `if (adj_y > -50) adj_y -= 1;`),
  so a delay-slot assignment on a plain (non-likely) branch runs on both
  paths and belongs before the if.
- Loops that are written `if (t > v && v != 0) i++; else ...` inside a
  do/while(i != 0) (flag loop, fly_adjy2_suby/subz).
- `w->adj_tm += (s16)f;` (s16 field, float) matched; `(int)f` adds a
  sign-extension, `(int)f + x` swaps the addu operands.
- em08_senkai_pos_no: three induction forms for one index in the original
  (pointer, byte offset, i*8) came from `for (i...; p++, i++)` with
  `p->stg == -1 || em->area->stg_pos[i].stg == em->stg` (base re-read in
  the source, hoisted by the compiler). Unsigned loop counters (sltiu)
  mean `u32 i`.
- Switches with `default: return;` and a call after the switch: put the
  default last when the original's out-of-range branch goes to a
  `b epilogue` stub just before the call.

## Files

- em07 (0x599ED0-0x59A23C, 5 functions): all match. src/game/em/em07.c,
  jump table slot 0x686860-0x68687C.
- em08 (0x5A7380-0x5A7F68, 11 functions): 10 match, built as
  src/game/em/em08.c (0x5A7380-0x5A7DDC) with rodata 0x686AB0-0x686B5C
  (three jump tables, the 12-byte gap between the 2nd and 3rd is the
  object's alignment). em08_senkai_pos_no is 2 instructions off (s0/s2
  swapped in the second loop's setup); whole file in em08_nm.c.
- em16 (0x5E65D0-0x5E6C74, 10 functions): all match. Setters take
  (em, no, arg) but call `em_act_set(em, group, no)` (3 args). The
  fly_adjy2 sub-functions set adj_y/adj_z straight from the table's first
  row at time 0 or 1, and subz zeroes adj_z when x74C & 0xF000000F.
  rodata 0x688ED0-0x688F30.
- em27 (0x6139D0-0x6140A4, 10 functions): all match; generated from em16.c
  (em_act_set2 with 4 args instead of em_act_set, attack actions 0-13).
  rodata 0x689C20-0x689C80.
- Lesson (em16_act_act_set): a single `if (no == 1)` compiled as
  `beq ==1 -> body; b end` is a one-case `switch (no) { case 1: ... }`.
- Lesson: `em->x883 != -1` on a u8 field keeps the -1 compare (lbu then
  li -1), as in the original; no cast needed.
- em21 (0x60C3A0-0x60D3FC, 11 functions): 10 match, built as em21.c
  (0x60C3A0-0x60D26C), rodata 0x6898D0-0x68997C. em21_senkai_pos_no is the
  same code as em08's and the same 2 instructions off (em21_nm.c).
  Tail functions (fly_adjy2_init .. senkai_pos_no) are byte-identical to
  em08's apart from relocations, so they were copied.
- Lessons (em21): `game_w.stage == em->stg` (global first) gives the
  original's load order; the "on my own stage" fallback check used by most
  fly/attack cases is a macro in em21.c (EM21_STAGE_CK). A `goto` from
  move case 0 into case 3 (laid out after case 1) matched. `x / 7` on an
  int from a float shows up as the 0x92492493 multiply.
- em02 (0x57E120-0x57EF94, 13 functions): all match. rodata
  0x6861F0-0x68626C. Adds senkai_player / senkai_target (turn ang[1]
  toward a player / the target by at most w->turn, angle left in w->dang)
  and fly_adjy (table fly_adjy_hosei_tbl in main's small data, timed by
  EMW+0x19C, applies speed_add). Lessons:
  - `if (arg)` on a u16 param tests the register as is; `arg != 0` adds
    an andi (em02_act_act_set).
  - `a = w->dang & 0xFFFF;` (explicit mask on a u16 field) gives the
    original's andi in the delay slot (senkai_*); `a <= w->turn` gives the
    `slt at` form.
  - `((PLW *)player_work)[n].stg` keeps the 0x736 offset in the load; plain
    `player_work[n].stg` folds it into the symbol address.
  - `u16` return type + `u16 ret` local avoids a sign extension
    (senkai_target).
  - suby: `int i;` declared before the table pointer fixed an a2/a3 swap.
  - suby variant: the loop test uses `v > 0.0f` and a `v < 0` branch
    (`w->adj_tm = 0; ret = 2; em->adj_y = 0.0f; break;`).
- em14 (0x5C1260-0x5C2634, 13 functions): all match. rodata
  0x688610-0x68869C. senkai_target / fly_adjy / fly_adjy2 subs are em02's
  code with a different EMNNW layout (fly_adjy table here is a normal
  global, not small data). Lessons:
  - Jump-table switches: every case that shares the "just call" target must
    be listed (`case 1: case 2: ... break;`), or MWCC builds a compare
    chain instead of a table.
  - `em->work08 = (int)((w->dist = CalcDistanceXZ(...)) / 30.0f) + 30;`
    (assignment used as a value) avoids reloading w->dist (atk 15-17).
  - tossin_move: `a > 0xFFC0` / `a <= 0x3F` forms as in the original.
- Coordinator note: helper functions with address-suffixed names in the
  split (fly_adjy2_subx_...) are file statics; keep them `static`.
- em15 (0x5CF0F0-0x5D04EC, 13 functions): 12 match, built as em15.c
  (0x5CF0F0-0x5D0358), rodata 0x688830-0x688940; em15_senkai_pos_no is
  em08's code, same 2-instruction near-match (em15_nm.c). Lessons:
  - A run of `==` tests on one byte that jump to the same place, ordered
    high to low, is a `switch` with stacked case labels (fly 7, stg check).
  - Address-taken local read after a byte store is reloaded each time in
    the original when read through a pointer variable (`h = &hit[1]; *h`)
    and the call result is held first (`d = CalcDistanceXZ(...)` before
    the tests).
  - `if ((d = ...) > 5000) ... else if (d < 2500)` with a local d (no reload
    of the stored field).
- em17 (0x5D81A0-0x5D9BC4, 13 functions): 12 match, built as em17.c
  (setters, 0x5D81A0-0x5D8C64, rodata 0x688C30-0x688CAC) and em17b.c
  (senkai_target .. fly_adjy2, 0x5D9570-0x5D9BC4). em17_senkai_sub (the
  flying bank/turn/climb routine, 574 instructions; em20 and em01 have
  near-identical copies) is 17 instructions off: only a0/a1 swapped for
  ang[2] vs bank_max in the "turn_left > 0x8000" and "turn_left == 0"
  branches. Whole file in em17_nm.c. Lessons:
  - `a = (a < 0x8000) ? a : (u16)(0x10000 - a);` gives the original's
    `slt at` + empty-then layout (an if-statement is 10 instructions off).
  - act_set calls Online_ck/act_ck (s16) before the dispatch; `(u8)arg`.
  - em17_act_act_set: small gp-relative tables st58_dir/st64_dir/st75_dir
    are declared `u16 x[4]` (8 bytes) so MWCC uses gp addressing.

## Update: senkai_pos_no solved

The 2-instruction senkai_pos_no near-match (em08/em15/em21) is fixed by
walking the four points with a pointer in the distance loop:
`for (q2 = pos, i = 0; i < 4; i++, q2++) dist[i] = CalcDistanceXZ(em->pos, *q2);`
(the original sets the walking pointer before the counter). em08, em15 and
em21 are now whole files (0x5A7380-0x5A7F6C, 0x5CF0F0-0x5D04EC,
0x60C3A0-0x60D3FC); their _nm.c files are gone. Found while matching
em20_ground_point_search, which has the same loop (there the pointer is
also assigned before the em_pl_pos_set call).
- em17_senkai_sub now matches: the bank value `b = em->ang[2]` is declared
  in block scope inside each branch (`int b = em->ang[2];`), which changes
  MWCC's register colouring (found by the permuter as an inline accessor,
  then rewritten as block-scoped locals). em17 is one whole file again
  (0x5D81A0-0x5D9BC4); em17b.c and em17_nm.c are gone.
- em20 (0x5FCDC0-0x5FFCB4, 18 functions): 17 match, built as em20.c
  (0x5FCDC0-0x5FD898, rodata 0x6894E0-0x689600) and em20b.c
  (0x5FDA10-0x5FFCB4, rodata 0x689620-0x68965C). em20_act_set is 1
  instruction off (`kind = 3` loads with daddiu in the original, i.e. a
  16-bit type, but a u16 `kind` makes MWCC reuse the masked switch value
  for the em_act_set2 call); whole file in em20_nm.c. senkai_sub2/sub3 are
  senkai_sub with parts removed (turn base 0x100, no sinking, climb toward
  tgt_pos[1]). New: GAME_W x2E (include/game.h). Lessons:
  - ground_point_search: same pointer-walk loop as senkai_pos_no, with the
    pointer assigned before the em_pl_pos_set call; reading em->x617 twice
    instead of keeping it in a local fixed the register choice; `q = pos[n]`
    taken before the stores.
  - xang_set_pl: `a = 0x10000 - calc_vec_ang(...); a = (u16)(a - ang[0]);`
- em01 (0x57AB60-0x57DE24, 19 functions): all match, rodata
  0x686080-0x6861EC. Generated from em20's code: senkai_sub* add
  `if (!(flags & 8))` around the turn-rate update (and sub1
  `!(flags & 0x10)` before the animation change), em01_demo_senkai_target
  is senkai_target without the "no target" exit. New EMW field x8D4[4]
  (per-player value, em01 atk 4). Lesson (atk 4): a plain
  `if (A) no = 0x18; else if (B) no = 4; else no = 0x19;` chain puts each
  `no = ...` in the branch delay slot (overwritten anyway on the other
  path), so it looks like "statement before the if" but is not.

## Summary (end of assignment)

All 11 assigned files decompiled: em07, em08, em16, em27, em21, em02, em14,
em15, em17, em01 fully match (whole files); em20 matches 17/18 (em20_act_set
1 instruction off, em20_nm.c, a 15-minute permuter run found nothing).
153 of 154 functions byte-match; every registered file passes
`tools/rebuild.sh game` (game OK).

## Second assignment: g_em* gap files, f_em.s, Em_Master_Change, Em_Taisei_Damage_Check

- horm (turn-to-face-player) code: em01_horm.c, em14_horm.c, em17_horm.c,
  em20_horm.c, all match. em01 checks em_frame_check2 before re-picking the
  turn animation; the others only test x194. em17 uses animation 2 instead
  of 3. em14_horm.c also has em14_suna_ck / em14_sasari_ck. The stretch
  after each horm file holds the next monster's local_area_move_init
  (em15/em17/em21), kept in the same C file because the real boundaries
  are unknown.
- local_area_move_init (em08/em14/em27 standalone, em15/em17/em21 in the
  horm files): `em->stay_tm = emNN_stay_timer_tbl[stg]; em->runaway_tm =
  emNN_runaway_timer_tbl[stg];` (EMW 0x94A/0x94C, new).
- New EMW fields: horm_ang (0x3A4), x3F4, stay_tm, runaway_tm, x95D.
- Lesson (horm_main): `((a + 0x200) & 0xFFFF) < 0x400` on a u32 gives the
  unsigned sltiu; `(u16)(a + 0x200) < 0x400` promotes to int (slti).
- em19_flyinit.c (em19_fly_adjy2_init, right before fly.c), em19_move.c
  (em19_move_sub, em19_dir_adj, em19_rate_add_calc,
  em20_local_area_move_init) and em15_senkai.c (em15_senkai_target = em02's
  code) all match. EMW 0x3B4 rate_x: rate vector x; with adj_y (0x3B8) and
  adj_z (0x3BC) it forms a f32[3] that em19_rate_add_calc copies and
  rotates by ang[1] before adding to pos.
