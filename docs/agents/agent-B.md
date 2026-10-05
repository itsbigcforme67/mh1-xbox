# Agent B notes: eft04, eft16, eft17 (game.bin effects)

All checked with `tools/check.py` per function and `tools/rebuild.sh game`
(game OK, byte-identical) after each file was registered.

## eft04 (0x0053FFD0-0x00542D34, monster attack effects, 9 types)
- Built: `src/game/eft/eft04.c` (eft04_move..eft04_e, rodata slot
  0x006853B0-0x00685404 for the eft04_i/eft04_m jump tables) and
  `eft04b.c` (eft04_pos_calc..Eft04_set_time). 13 of 14 functions.
- Near-match: `eft04_nm.c` holds the whole file. eft04_t is 6 instructions
  off: in the type 3 colour fade (`col = A<<24 | R<<16 | G<<8 | B` rebuilt
  from the bytes of an eft_rgba_linear result) the original schedules the
  `srl R` before the `andi A`; no expression form tried (operand order,
  temporaries, u8/u32 casts, separate r/g/b locals) reproduces it.
- Work piece is 0x30 bytes (EFT04_PIECE, file-local). Type 0 picks four
  distinct joints from eft04_em15_pos (0x14-byte entries).

## eft16 (0x0054D660-0x00550F68, hit blood/sparks, 15 types)
- Built: `eft16.c` (move, i) and `eft16b.c` (d..Eft16_set_impact, rodata
  0x00685730-0x00685760 for Eft16_set_impact's jump table). 12 of 13.
- Near-match: `eft16_nm.c`, eft16_m 10 instructions off: only the three
  stack spill slots (k = keyframe index copy, n = sprite count, and the
  compiler's i*5 induction variable) come out in a different order.
  Decl-position search over n/k found nothing.
- Character fields 0x3EC (u16 facing) and 0x4D8 (pointer, its +0xA0 holds a
  joint list) are read through file-local macros (CHR_ANG3EC/CHR_X4D8);
  not added to pl.h/em.h. Owner kind at +2 is read via PLW._pad002[0].

## eft17 (0x0053BA50-0x0053FFC4, breath/dust, 21 types): whole file matches
- `eft17.c`, rodata 0x00685230-0x006853A4. Type 8 is a separate path
  (ten bouncing rocks, eft17_i08/m08/t08).
- Note: eft17_m00 type 15 adds v[0] to all three position components
  (y and z too). That is what the original does (looks like a Capcom bug).

## Matching lessons (each confirmed by a match)
- `for (...; p++, i++)` vs `i++, p++` changes the order of the increments
  at the loop end (eft04_i, eft16_i, eft17_i00).
- Two keyframe reads `data[idx]`, `data[idx+1]`, then `idx += 2`: the
  original keeps an int copy `k = idx` and indexes `data[(s16)(k + 1)]`;
  plain `idx++` twice or `data[idx + 1]` (CSE'd to 4(addr)) do not match
  (eft04_m). A single read is `d = data[idx++]; f(lag, d, ...)` (eft04_m,
  eft16_m); writing it inline in the call changes argument load order.
- A `u16` flag tested with `if (mul)` skips the andi 0xFFFF that
  `if (mul != 0)` produces (eft04_t).
- Loops `for (i = 0; i < num; i++)` straight on the s16 count (no int copy
  `n = num`) fixed spill-slot order in eft17_m00.
- Empty cases are real: eft17_i00's top switch has `case 5: case 16:
  case 17: break;` (they branch to the end instead of default).
- One-case switches with default (`switch (x) { case 0: ...; default: ...}`)
  where an if/else gives the wrong branch layout (eft04_type8_init,
  eft04_t type 8, eft17_i/m/d/t dispatchers, eft16_col_type_sel).
- `switch (hit) { default: case 2: ...}` reproduces a leftover `li 2`
  with no compare (Eft16_set, Eft16_set_impact).
- Small (<=8 byte) tables must be declared with their size to get
  gp-relative access (eft04_type5_fade_data[2], eft17 lag tables [4]).
- Statement order inside a case matters for load scheduling even between
  calls (eft17_t00: `cl = ...; mats = mw->mat; flag |= 2;` in type 3,
  `flag |= 2` first in types 7 and 17).
- `(u8)(r - (u8)((s32)((u32)r >> 1) * t))` gives the srl + plain cvt
  pattern for halving a colour byte (eft16_t).
- An `||` of two equality tests compiled as beq/beq/b came from
  `switch ((s16)arg) { case 1: case 2: ...; default: ... }` (Eft17_set).

# Second assignment: per-monster AI files (game overlay)

All checked with check.py and `tools/rebuild.sh` (all modules OK).
- em29 (f_em_6140B0, 0x6140B0-0x6147C8): whole file matches, src/game/em/em29.c.
- em18 (f_em_5E6E00, 0x5E6E00-0x5E7918): whole file matches, em18b.c (b because
  g_em18_init belongs to agent C's em18).
- em19 (f_em_5E8060, 0x5E8060-0x5EB3A8): whole file matches, em19b.c.
- em10 (f_em_5ACC60, village NPC/trader, 26 functions): parked in
  src/game/em/em10_nm.c (not built). Everything matches except em10_turn_sub,
  10 instructions off (only temporaries a1/a2/a3 coloured differently; a
  search over decl order and types and a permuter run did not fix it). A split
  around it does not work: em_act00/02/03 call em10_search_set / em10_msg_set,
  and with those in another file MWCC reloads a0 (it only skips that for
  callees defined in the same file), so three more functions stop matching.
  Next: fix em10_turn_sub, then register em10 as one file
  (0x5ACC60-0x5AF528, rodata 0x688220-0x688248 and 0x688250-0x6882D4).
- Not started: f_em_58BA40 (em04, 42 functions), f_em_5873D0 (em03, 61).

Shared header edits (since the first assignment): em.h x05, x06, x13,
mode_old/x15_old, type, mat, x0E, x2D4-x2DA, x3C0, x3F4, x40C/x40E, x56A,
x616, x6E0/x6E2, x6FF, x734, home, x798, x7D6, x88B, x8BB, x8BD, x95C, x9E1
(some renamed by the coordinator since); pl.h talk (0x8C6), x8C7.

Lessons from this round (each confirmed by a match):
- Data tables point at some "static" functions (dummy_em_prog_ADDR, emNN_effect_move_ADDR);
  define those globally with the address-suffixed name or the link fails.
- A static callee in the same file lets the caller keep using a0; when the original
  doesn't reload a0 after a call, the callee was in the same file (em10_msg_set).
- `NPC_Message` had to be called without a prototype to get the original argument load order.
- Calls with an extra unused or constant argument show as a register set before the
  jal (em_act_search takes one argument; em_mahi_eff_set(em, 2); em_sleep_eff_set(em, 8, v, f)).
- One-case `switch` again and again where the original has beq/b instead of bne
  (em19 main, talk_move x2D6, to_normal flag).
- Loading table fields with separate symbol+offset addressing means each field is
  indexed separately, not through a pointer (em10_init); em10_init reads angle/act/pose
  from em10_start_pos41 for every stage (a Capcom bug kept as is).
- `(s32)((u32)u8 >> 3)` gives srl + plain cvt (em29_init).
