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
