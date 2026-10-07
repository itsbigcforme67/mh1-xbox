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

# Third round: em04 (f_em_58BA40, 0x58BA40-0x58F4A0, monster kind 4, 42 functions)
39 of 42 functions match and are built (rebuild all OK): src/game/em/em04.c
(0x58BCC0-0x58D4D8, rodata 0x6865B0-0x686648), em04b.c (0x58D600-0x58E4F8, rodata
0x686650-0x6866F0), em04c.c (0x58F3E0-0x58F498). Shared declarations and EM04W in
include/em04.h. em04_nm.c holds the whole file including the three near-matches
(not built, stay asm):
- em04_act_set (0x58BA40): logic complete; only the copy of six floats from
  em05_rev_set_tbl_stNN rows differs: the original keeps `p += 2` as a real addiu
  between the three pairs, MWCC folds it into the load offsets for every source
  form tried (f32*, f32(*)[2], V2 struct copy).
- em_dm03 (0x58D4E0): 2 instructions: the constant 2 of the switch ladder sits in a1
  instead of v1.
- ef_move_sub (0x58E500, per-animation sound/effect script, 3.7 KB, generated from the asm
  with a script): the whole compare ladder uses a0 for constants and v1 for the value
  where mine uses v1/v0; same kind of difference as em_dm03 (an extra live value
  somewhere in the original?). Everything else in the 936 instructions is identical.
Shared header edits: em.h x39C, x1A8, dm_ang (0x3EC, same offset as PLW.dm_ang).
Lessons (each confirmed by a match):
- Switch compare ladders come out in REVERSE source order of the case labels; code
  blocks follow source order. Jump table entries then tell the source order: em_move00's
  table showed cases 9, 12, 10, 11 (check.py does not see this, only rebuild does).
- A callee called with a varying number of arguments (em04_act_set: 3 params, callers pass
  3 or 4) needs an unprototyped declaration `void f();` before a K&R definition
  (`void f(em, kind, no) EMW *em; int kind; u16 no; { ... }`); u16 `no` then keeps
  daddiu for constant assignments. A call (em, 6, 0, 1) loads the constant 1 in a3 and
  shares it with a compare constant.
- `if (a * b >= 0.0f) x = 0.0f;` compiles to bc1t + the store in the delay slot (the
  original's odd "bc1t to the next instruction" in em_dm01/02/die00). `< 0.0f` gives bc1f.
- Locals declared `FLMAT mat; f32 in[3]; f32 out[3]; s32 ang[3];` land at sp+0x20/0x60/0x70/0x80
  (later declarations get lower addresses; ang[0] doubles as temp in dm01).
- `(u16)ran_suu(1) & 0x7F`, `x = (u16)f(); y = x;` (local) avoids a reload of the first store.
- A u16-returning declaration for em_act_search plus u16 parameter in em_act_set stops MWCC
  from adding andi at the call.
- Statics called across a split need the suffixed global name (em_move00_0058C350 ...), and
  the jump-table order check is only done by rebuild.sh.

# em03 (f_em_5873D0, 0x5873D0-0x58B84C, monster kind 3, 61 functions)
57 of 61 match and are built (rebuild all OK): em03.c (0x5873D0-0x588344, rodata
0x686490-0x686500), em03b.c (0x588760-0x588BEC), em03c.c (0x588E90-0x58B84C, rodata
0x686500-0x686590); include/em03.h holds EM03W and the shared externs. em03_nm.c is the
whole file. Near-matches (stay asm): em_mv01/02/03 (0x588350, the "turn by at most 0x40"
block: the original keeps the angle difference in v0 and the old angle in v1 and uses `at`
for the constants, mine puts them in v1/a0; no source form tried (single expression,
locals, pointer, repeated expression) changes it) and em_mv12 (0x588BF0, 7 instructions,
the same a1/a2 swap of the turn code as em10_turn_sub; declaration order permutations do
not help).
Lessons:
- Functions called from a dispatcher with the monster work pointer in a1 (em_move00(em, w))
  have that second parameter even when unused, and the callees too (em_act*(em, w)): it shows
  as the jump-table base register being a2 instead of a1.
- `if (a || (b && c))` over u8 locals needs `(a & 0xFF)` tests to match the original's andi
  (em03_main); u8 locals alone are not re-masked.
- `for (pn = 0; pn < game_w.pl_num; pn++)` with `s8 pn` gives the dsll32/dsra32 pair and
  reloads pl_num every pass (em03_main); `if (pn == game_w.pl_num - 1) em->x839 = 1;` as the last
  statement compiles with the store in the branch delay slot (executed on both paths).
- A lone store before an if (`x8C3 = 0` in em03_init) belongs inside the preceding if when
  the original shows it after the bne with a nop in the delay slot.
- A generator (asm to C) is practical for the sound/effect script ef_move_sub: it tracks
  a0-a3 and f12 constants through addiu/daddu/lui/ori/mtc1 and emits sound_call(...) lines;
  the few blocks with control flow were then written by hand (per-animation switch with a
  reverse-ordered compare ladder, `case 0x3E9: break;` for the empty first case).

# Fourth round (em33, em09 started)
- em03 now one file (61/61, em03.c) and em33 one file (60/60, em33.c, written from em03 by renaming and fixing diffs).
  em03 turn block: `int d; d=(u16)((u16)Em_Calc_angY()-ang[1]); if (d<=0x8000){if(d<=0x3F)ang+=d;else ang+=0x40;}else if(d>0xFFC0)ang+=d;else ang-=0x40;` matches; mv12 needed decl order spd,fr,d,dd,h,ang with a local h=horm_ang.
- em_mahi_eff_set(em, 2) (2 args) fixes the a1/v1 constant-register swap in em04 dm03 (a compare constant is shared with a later call argument).
- check.py compares plain static names against the first matching address of any file: give statics their address suffix before trusting "OK" (em33 had hidden mismatches).
- em09 (f_em_5A81B0): src/game/em/em09.c WIP, not registered; first 7 functions done except em09_act_set (same unfolded-pointer-copy problem as em04_act_set), em09_status_ck/em09_dir_calc 2-8 instrs off (signed/unsigned compare forms).
- em04 near-matches left: act_set, ef_move_sub. em10_turn_sub still parked.

# Fifth round (policy: breadth first, park after ~10 min)
## em09 (f_em_5A81B0, 0x5A81B0-0x5ACC60, item thief, 52 functions): 47 match
em09.c (oikake_ck), em09b.c (next_act_set), em09c.c (item_theft ... ef_move_sub, 0x5A8540-0x5AC938, rodata
0x686B60-0x686C40), em09d.c (local_init, dummy). em09_nm.c = whole file. Rebuild OK.
Near-matches (stay asm): em09_act_set (same unfolded pointer copy as em04_act_set), em09_status_ck (6 instrs,
sltiu vs slti/andi on `(u8)(mode-4) < 3`), em09_dir_calc (4 instrs, register of the second temporary),
em09_effect_move (5 instrs, `mode == 4 || mode == 5` layout; `switch (mode) { case 5: case 4: ...}` is closest),
em09_material_sub (loop/pointer layout, ~90 instrs).
Lessons (each confirmed by a match):
- `a = b = x` stores b first: `em->ang[1] = ang[1] = expr;` fixed the store order (em09 dm01/die00).
- check.py prints "original calls em_act00" for address-suffixed statics (name noise only); only rebuild.sh
  tells. Statics that the asm of unmatched functions or other runs call must be global (ef_move_sub_005AB750).
- Per-case constant tests with `||` chains that come out as separate beq's need `switch (i) { case 2: case 3: ...}`
  (ladder = reverse source order).
- `if (r != -1) { B } else { A }` gives the layout `beq r,-1 -> A; B; b end; A:` (em12 mov01 case 1/3).
- `(f32)(u32)u8` gives the bltz unsigned fix-up (em12_init); `(u16)(u32)(f / 66.0f)` the 0x4F000000 test.
- calc_vec_ang(f32,f32,f32,f32) takes (x1, z1, x2, z2); its result needs `(u16)` then `+ 0x4000` then `(u16)`.
- A static empty function called only from one place stays a real `j` call only if it is global (em12 move02).
- Em_Yobi_Ck result: `int yobi = (u8)Em_Yobi_Ck(...)` gives andi then a direct test (em09_main).
- struct fields addressed as `w->yobi` (array member) are recomputed from w each time, a cast `(f32 *)((u8 *)w + 0x24)`
  is CSE'd into a saved register (em12_main).

## em12 (f_em_5AF530, 0x5AF530-0x5B5290, 66 functions): 64 match
em12.c (0x5AF530-0x5B06E0), em12b.c (0x5B0AE0-0x5B3A44), em12c.c (0x5B40D0-0x5B5248); rodata 0x6882E0-0x688318,
0x688340-0x6883CC, 0x688410-0x688430. em12_nm.c = whole file. Rebuild OK.
Near-matches (stay asm): em_mov01_005B06E0 (the first angle test `(u16)(horm_ang - ang[1]) < 0x3000` is in v0 with an
unfilled delay slot in the original; mine uses v1 and fills it; locals/casts/expression forms tried) and em12_main
(registers: the original keeps hit/idle/revived/boss_hit in s7/s6/s5/s0 and reads em_boss_tbl once into v0;
all 720 orders of the flag declarations tried with declbf, best 316 instrs off; logic complete).
Shared header edits: em.h x7A0 (struct PLW *), x94E (s16), x944 (struct EMW *).

## em27 (f_em_60D440, 0x60D440-0x6139D0, monsters 27/28/31, 70 functions): 68 match
em27a.c (0x60D440-0x612B94, rodata 0x689980-0x689B40), em27b.c (0x612DA0-0x613958, rodata 0x689B40-0x689C14),
em27c.c (dummy). em27_nm.c = whole file. Rebuild OK. The action setters/fly_adjy2 are in the older em27.c.
Near-matches (stay asm): em27_uvmove (0x612BA0: the original walks two induction pointers rooted at em with
constant 0x5F0/0x5C0 offsets; every form tried keeps the offsets folded into the pointers, 61 instrs off) and
em27_effect_move (7 instrs: eff in a2/v1 instead of v1/v0 around the trailing em27_uvmove call).
Lessons:
- `if (u8var != 0 && ...)` adds an andi, `if (u8var && ...)` does not (em27 atk02, `daddiu s0,zero,1` for u8).
- `if (a >= 2)` on a u16 result gives slti v1; `if (a > 1)` gives the original `slti at` (em27 act01).
- A `for`-less per-slot loop that touches the same field twice wants separate stores per branch (mv00: each
  branch stores ang[1] itself, a merged `v` variable adds a store).
- `(u16)x < 0x8001` must be `(u32)(u16)x < 0x8001` for sltu (em12 act02).
- Dispatchers that pass `w` along (jal without touching a1) are written `(EMW *em, EMW_W *w)` and call
  `em_actNN(em, w)`; the acts take (em, w) too, even when unused.
- Sparse empty `case`s listed first (`case 0: case 3: ... break;`) force the jump table the original has
  (em27_main Em_Dmg_Sys).
- Writing `x05++; x388 = 0; em_char_set(...)` (both stores before the call) puts the second store in the delay slot.
- check.py "move0x" noise: do not grep it away, one real diff hid there (em27 move06 is a one-case switch).

## em16 (f_em_5D0600, 0x5D0600-0x5D8198, monsters 16/13/30, 81 functions): 79 match
Sibling of em27 (same templates; em16_nm.c was written from em27_nm.c). em16a.c (0x5D0600-0x5D7404, rodata
0x688940-0x688B50), em16b.c (0x5D7610-0x5D8130, rodata 0x688B50-0x688C24), em16c.c (dummy). em16_nm.c = whole file.
Rebuild OK. Near-matches (stay asm): em16_uvmove and em16_effect_move (same two as em27, same diffs).
Lessons: a `u16 d` local is re-masked at every use (andi), an `int d` is not (em16 demo00); small tables
(`u16 st51_ang_tbl[3]`) need their size for gp-relative access (die04); K&R `void f(em) EMW *em; {}` lets callers pass
extra zero args (em16_to_normal(em, 0, 0)); `case 4: ... /* fallthrough */ case 3:` with the jump table sending 3 into the
middle of 4's code (demo00).
Shared header edit: em.h x95B (u8, boss flag).

# Sixth round: em01 AI (f_em_566630, 0x566630-0x57AB5C, monster kinds 1/6/8/11/14/15/17/21/22/26, 151 functions)
141 of 151 functions are linked (rebuild OK, all five modules byte-identical): src/game/em/em01_ai.c .. em01_aih.c
(8 matching runs, text 0x566630-0x57AB58, rodata 0x685B30-0x686078 for the jump tables of atk08, atk17/21/22, demo00-02,
move00-06, main and ef_move_sub). em01_ai_nm.c holds the WHOLE file (all 151 functions, the C that the PC runtime should
use). EM01W (per-monster work at EMW+0x444) is in that file (a superset of the one in em01.c).
Near-matches (stay asm, C in em01_ai_nm.c, all logically complete):
- em01_frame_reset (5 instrs, switch constants land in t0 instead of v1) and em01_reset_char_set (4, same): nothing tried
  (K&R params, u8/s8/u16 switch variable, EMF macro) changes the register of the ladder constants.
- em_mv03/em_mv05 (7/8: `sltiu at` vs v0 in the turn-window test and one nop before the final branch).
- em_fly09 (12: the `work08 -= 1` store/reload order), em_atk11 (the last `slt at`/`sw` delay-slot pair of case 3: the
  original uses `slt at` and keeps the work08 store in the delay slot, mine stores before the long float expression),
  em01_effect_move (a2/v1 instead of v1/v0), ground_land_eff_set/takeoff_eff_set (the `&v[1]` address register),
  em01_uvmove (61 instrs, same near-match as em16/em27_uvmove).
Mapping: em_actNN = action steps, em_mvNN walk/turn, em_flyNN flight, em_atkNN attacks (atk08/21 = "kyusyu" pursuit,
atk11 = dive), em_dmgNN damage, em_demoNN event demos (demo00/02 carry the partner x944), em_dieNN, em_moveNN dispatchers by
mode, em01_main (damage system + dispatch), ef_move_sub (per-animation sound/effect script, 685 sound_call calls).
Capcom bug kept: em_fly24 passes an uninitialized local to Em_Calc_angY.

## New tools (all in /tmp-independent form under tools/)
- `tools/draft.py` now resolves jump tables (jt_patch): m2c decompiles functions with switch tables.
- `tools/status.py FILE` (noise-aware check): prints per function OK / NOISE (only "calls X, original calls Y" static-name
  differences) / DIFF n; "true OK" = OK or NOISE. check.py itself reports NOISE functions as `--`, so genruns.py does not
  see them as matching; use tools/mkruns_nm.py instead.
- `tools/mkruns_nm.py NM.c PREFIX`: writes the matching runs (PREFIX.c, PREFIXb.c ...) of a near-match file (all true-OK
  functions that are consecutive by address; every function becomes global except those in KEEP) and prints the
  c_files.txt text lines. Rodata lines (jump tables) are still computed by hand: each run's tables are contiguous from the
  first to the end of the last table (16-byte aligned in between), see tools/rodata.py.
- `tools/genef.py`: asm -> C for the sound/effect script (ef_move_sub style: switch on the animation, sound_call/quake_call/
  frame_check/Eft20_set calls with constants, if-chains rebuilt from the branch structure with a small Quine-McCluskey).
  Its output for em01 matched on the first full build; reuse for em17/em20/em14/em15.

## Lessons from em01 (each confirmed by a match)
- m2c drops float arguments of calls (MWCC passes floats in $f12-14, m2c assumes the o32 registers): declare the
  prototype WITHOUT the float parameters in the m2c context and read the constants from the asm (tools/f12.py in the notes of
  this round lists f12/f13 and the integer constants of each jal, incl. delay slots).
- `if (a < K)` and `if (K < a)` compile differently for floats: original `c.lt.s f1,f0` + `bc1t` came from `!(K < d)`
  (atk12), `4000.0f < d` (demo00/02), `100.0f < x` (atk08); plain `d > K` / `!(d > K)` give `c.le` forms.
- `em->w->dang -= em->ang[1]` must be written `w->dang = w->dang - em->ang[1]` to load dang first.
- A one-case `switch (x734) { case 3: ...; default: break; }` reproduces `beq L; b end` (em01_main).
- `t == 1 || t == 2 || t == 3` (not `(u32)(t-1) < 2 || t == 3`) gives the `sltiu at` form (em_act01).
- Fields read through a struct member (`em->x2DE`) are loaded separately; a cast pointer (`EMF(em, u16, 0x2DE)`) gets CSE'd
  into a pointer register (reset_char_set). x2DE/x2E0 added to em.h; x38E retyped u8 (lbu in em01_main).
- `for (i = 0, t = em_work; i < 20; i++, t++) { if (...) goto found; } t = 0; found:` reproduces a search loop that sets
  the pointer to NULL when the list ends (demo02).
- Float local arrays for `em_sleep_eff_set(em, n, v, 1.6f)`: `v[1] = 10.0f; v[2] = 140.0f; v[0] = 0.0f;` in this order
  (the last store lands in the jal delay slot); two such calls need two arrays (declaration order = address order).
- A callee in the same file that is `static` keeps its callers' register use; if it must be global (called from asm or
  another run) check the run file again with tools/status.py (em_act_search2 had to stay static, KEEP in mkruns_nm.py).
- Shell08_set_ang takes 6 arguments (em, joint, a, b, ang1, ang2); the last two sit in $t0/$t1 and the delay slot.

# em02 AI (f_em_57F1E0, 0x57F1E0-0x587390, 66 functions): 63 linked
src/game/em/em02_ai.c .. em02_aid.c (4 runs, rodata 0x686270-0x686488), em02_ai_nm.c = whole file. Rebuild OK (all five modules).
Near-matches (stay asm): em_mv01_005800C0 (7: same turn test as em01 mv03), em02_uvmove (61, same as em01; note em02's timer
advances by 2), em02_effect_move (6, same a2/v1 register difference as em01_effect_move).
Notes:
- em_atk05 is shared by actions 5, 10, 11 and takes a third argument (0/1/2) that picks the shell angle; the dispatcher
  em_move03 passes it. A switch whose compare ladder reads 0,1,2 means the source order is 2,1,0 (atk09), and 2,1,0 means 0,1,2.
- em_uvset (static leaf called many times from ef_move_sub): the caller does NOT reload a2/a3 between calls, so the callee
  must not modify them (write the index as `*(u8 *)((u16)idx + (u32)em + 0x5F8)` to keep a2 intact). tools/genef.py keeps
  a2/a3 across em_uvset calls so the generated constants are right.
- ef_move_sub_00583BF0 only matches as a file-static function but is called by asm (em02_effect_move): it is defined static
  and config/game_aliases.txt (new, added to the link by tools/build.py when it exists) gives the asm its address.
  Callees of ef_move_sub that precede or follow it in the file (move_default, quake_call, sound_call*, em_uvset) must also
  stay static (MKRUNS_KEEP in tools/mkruns_nm.py).
- tools/mkruns_nm.py recognises function-defining macros (#define NAME(NAME, ...)) automatically now.
- `em->mode != 6 || em->x15 != 0` guards two cases of the em02 effect script (genef flags unknown branches with #error).

# em07 AI (f_em_58F4A0, 0x58F4A0-0x599EC8, 75 functions): 70 linked
src/game/em/em07_ai.c .. em07_aif.c (6 runs, rodata 0x6866F0-0x686710, 0x686710-0x6867D8, 0x686840-0x686860),
em07_ai_nm.c = whole file. Rebuild OK. Near-matches (stay asm): em07_act_sub (21, branch layout of `x == 0 || idx == 0xFF`),
em_mv02 (7, sltiu at/v0), em07_main (the `idx*8 + em` addu operand order), em_uvmove (61, same as em01/02), em07_effect_move (6).
Notes:
- The state functions take only (EMW *em); the second m2c argument is a jump-table address left in a1. Declare
  `EM07W *w = (EM07W *)em->ex;` inside the function.
- ef_move_sub here is a sparse switch compiled as a compare ladder, no jump table: tools/genef.py takes `-` as the table
  label and reads `addiu $4,$0,CASE; beq $3,$4,.Lxxx` pairs. The empty case (0x42A) must sit in sorted position in the
  source or the ladder loses a compare. SOUND5=1 makes genef pass the extra arguments of sound_call (frame, se, joint, vol)
  and sound_call_mov/mov2 (frame, frame2, se, joint, vol). shell05_set4 takes 3 arguments, Shell22_set3(em, 4, w->x1A)
  is followed by `x1A++; x1A &= 3`.
- A shared tail (`goto blk41` in em07_main case 12) is needed so the same `em07_act_set(em, 4, 3, 2)` is emitted once.
- `x == 0 || x == 1` style turn tests: see EM07_TURN; the +0x8000 variant turns away from the target.
- sound_call_mov2 had to be global (called from a run file before it), the rest of the sound/effect helpers are static (KEEP).
- Watch out for scripted file edits: `s.index('int em07_act_sub')` matched the prototype and cut 130 lines (recovered from the draft).

# em08 AI (f_em_59A280, 0x59A280-0x5A7380, 122 functions): 109 linked
src/game/em/em08_ai.c .. em08_ail.c (12 runs, rodata 0x686880-0x686898, 0x6868A0-0x6868E0, 0x686920-0x686A28, 0x686A90-0x686AB0),
em08_ai_nm.c = whole file. Rebuild OK (all five modules). em.h: x7E0/x7E4 (f32) carved from padding (em08 water depth/surface).
Near-matches (stay asm): em08_init (3: eft09_set arg in the delay slot), em08_act_sub (8), em_mv02 (7), em_fly03/09 (4, sltiu at/v0),
em_dmg00 (1, addu operand order), em_demo00 (17, a2/a3 swap), em08_main (the `idx*8 + em` addu), em08_uvmove (61, same as
em01/02/07), em08_effect_move (6), hire_move_sub1/sub2 (100/180, static callees with custom register convention) and hire_move
(depends on them; excluded with MKRUNS_EXCLUDE), em21_target_ang_calc (29).
Notes:
- Conversion pipeline that worked (a 6900-line m2c draft -> 109/122 in one session): /tmp scripts patched the draft: arg0->em,
  M2C_FIELD(arg1,..)->w->name via a struct, missing float args filled in from the asm with f12.py (em_frame_check, Eft20_set,
  xang_calc_target ...), `temp = em->x05; switch (temp)` -> `switch (em->x05)`, `return;` -> `break;` when the switch is the last
  statement of the function (that single change fixed ~55 functions), then hand-fixes of what the diff showed.
- Compound conditions that m2c prints as goto soup (em08_main damage cases) are chains `else if (!(mode == 4 && x15 == 6) && ... && x388 == 1)`.
- `x = a ? b : c` written as `if (cond) x = b; else x = c` stores constants directly (no movz/movn); the ternary gives movz/movn.
  For constants stored to a float field MWCC uses lui+sw (no fp register).
- Sparse `switch` with a final `beq; nop; b end`: a switch with one case plus default (stage 0x36) or a ladder (kind in 1,6,8,0xB,...).
- A flag variable assigned 0/1 that is compiled with daddiu and tested without andi: `u8 flag` gives daddiu (but adds an andi on test).
- A static function with custom register arguments (hire_move_sub1/2 take $t4-$t6): its caller can only be linked together with it.
- `em08_fly_adjy2` returns u8 in em08.c but the asm tests v0 directly: declare it `int` where the caller does not mask.
- genef.py: `-` as table label for ladder switches, Code_Make/hire_req_set/atk_shell_set/em08_vib_set handled, SOUND5 only for em07.

# Porting tools for sibling AI files (handed over; agent D now owns em14/15/17/20/21)
tools/port_em.py ASMNAME PFX OUT.c turns the m2c draft of one AI file into a first-pass C file (about 2 minutes; f12.py runs per
function), tools/protos.py regenerates its forward prototypes, tools/fixlib.py has rep()/sub1()/ensure_spd() for hand-fix scripts
that survive a re-run (never use s.index('name(') on a file that has prototypes: it hits the prototype), tools/mkruns_nm.py takes
MKRUNS_EXCLUDE for functions that must stay asm (callers of static callees with custom register args). tools/sibcmp.py shows
opcode-identical sibling functions (em20/em01 66, em17/em20 55, em21/em08 49, em14/em20 48). em14 was started and dropped.
Hand fixes that were always needed: to_normal-like functions with extra args, the uvmove/sound_call helper block copied from
em01/em08, ef_move_sub via genef.py, EMxxW field names (dang/has_tgt/dist) for the TURN macros.
- Note: tools/mkruns.py is main's tool (fully matching runs by check.py); mkruns_nm.py is agent B's (split an NM file by status.py).

# em10 AI (f_em_5ACC60, 0x5ACC60-0x5AF528, 26 functions): 24 linked
em10.c, em10b.c (act00-02), em10c.c (act04..), rodata 0x688220-0x688248 (em10) and 0x688250-0x6882D4 (em10c); em10_nm.c = whole file.
Stay asm: em10_turn_sub (10/42), em_act03_005AE060 (the original does not reload a0 after em10_msg_set(em, n) before em_act_set(em, 0, n);
ours reloads it, +16 bytes). Lesson: tools/check.py status of em10_nm.c said 25/26 OK because the static em_act03/em_move00 share a name with
globals elsewhere; give such statics an address suffix (em_act03_005AE060) before trusting OK. A C run that ends before a 4-byte function
pad must end at the last function's real end (0x5AE05C), not at the next function.

# uvmove solved (em01/02/07/08/16/27)
The old "61 instrs off" near-match of every uvmove was the pointer form: MWCC only keeps the original induction-pointer layout when the arrays are
real struct members of EMW (em.h now has f32 uv[4][3] at 0x5C0, u16 uvtm[4] at 0x5F0, u8 uvty[4] at 0x5F8). Write em->uv[i][0], em->uvtm[i], em->uvty[i];
`((u32)em->uvtm[i] >> 1)` gives srl; the switch source order is 0,1,2,3,0xFF (ladder compares in reverse). Lesson: when the original keeps
`lhu x(t2)` with an unfolded constant offset, the data is a struct field array, not a cast pointer. Linked as tiny runs em01_uv.c .. em27_uv.c
(500 bytes each); do NOT regenerate runs from the *_nm.c files: the run files carry hand edits (static ef_move_sub etc.). em*_effect_move
(6 instrs, eff register a2/v1 vs v1/v0) is still open.

# em_taisei_nm.c (agent C's file, finished by B)
Written: em_eye_dmg_act_set (matches; no `default:` label: a jump-table switch whose holes jump to the end needs `break` instead),
Em_Dmg_Sys (10 instrs off), Em_Taisei_Damage_Check now matches (`u8 t = x & ~4; u8 u; u = t & 0xFF;` gives the extra andi).
EMW.hagi is now EM_HAGI hagi[8] (hp s16, cnt u8): real struct member arrays keep the em+const offsets unfolded (same lesson as uvmove).
Not linked yet because the file still has two near-matches inside (Em_Taisei_Ck, Em_Dmg_Sys); the matching ones could be split out with mkruns_nm.py.

# sltiu at/v0 near-matches solved (em01 mv03/mv05, em02 mv01, em07/em08 mv02) + tools/appendfn.py
`d < 0x11C8U || d >= 0xEE39U` gives `sltiu v0`; the original `sltiu at` comes from writing the first test as `d <= 0x11C7U` (second stays `>=`).
The three `return;` in the turn function must be an if / else if / else chain ending in `break;` (a shared `return` stub adds a nop before the b).
tools/appendfn.py RUN.c NM.c fname appends a function that now matches to its neighbouring run file; then extend that run's end in config/c_files.txt
(end = function start + size). A function that is static in the nm file must become global (address-suffixed name) when asm callers remain.
WARNING: align.py hides differences in lui constants (a float constant 110.0f vs 48.0f looked like a match); check.py or a rebuild is the arbiter.

# Sixth round (post-outage)
- Merged main (only config/c_files.txt conflicted; union + no duplicates); rebuild all five OK.
- All unregistered em text left in game.yaml (f_em_55B060, 5B5290, 5C2A80, 5D9EE0, 5EBA10, 5FFFD0) is agent D's (em14/15/17/20/21).
  Agent B's remaining em work is only the parked near-matches (em10_turn_sub, em04 act_set/ef_move_sub, em03 mv, em09, em12).
- em10_turn_sub: four more declaration/type forms retried (u32/s32/u16 d, tgt as u32, no tgt local): still 10 instrs off (a1/a2/a3 colouring), parked.

# Lobby overlay (lobby.bin, links at 0x533980 like game) - agent B, 0x533980-0x5C4E60

Findings from the first pass (5 Oct 2026). Byte/structural comparison of every
lobby function against matched game/main code (opcode+register shape, immediates
ignored) found essentially nothing shared: only ~60 tiny coincidences (accessors,
5-instruction wrappers). The lobby is its own code base, so nothing is reused
from game.bin. What does repeat is *inside* the lobby network layer: 335 of the
935 cnlbs functions fall into 82 identical-shape families (see below).

## Area map (vram, size)
- 0x533980-0x53E848 (43 KB) lobby town game logic (Capcom):
  - 0x533A00-0x535238 lb_talk: NPC talk start/choosers, Lb_event_* (reward talks),
    lb_talk_init, Lb_put_npc_default. MATCHED (src/lobby/lb/lb_talk.c, rodata 0x654AD0).
  - 0x535240-0x536708 lb_mix: forge/item shop (Lb_mix, list build, select, buy/sell).
  - 0x536708-0x53856C shop engine (Lb_shop_move step machine, list/help/tag drawing).
  - 0x53856C-0x53C21C lb_process (weapon/armor forge menu), 0x53C21C-0x53D7D0 lb_armor
    (armor shop), then f_sound/f_em10/f_em09/f_move (0x53D7D0-0x53E848, small).
- 0x53E848-0x590D40 (330 KB) NOT Capcom code: Sony/third-party libraries compiled with
  GCC: sceHTTP client (0x53E848-0x549A30), MD5/digest, then an SSL/crypto stack
  (ASN1, BER, BIO, BN, X509, EVP, RSA/DSA/DH, SSL2/3/TLS1, OP_, R_ eitems, sk_, ...).
  Skip (the brief says skip GCC library code).
- 0x590D40-0x5C4E60 (207 KB, 935 functions) "cnlbs" - the lobby client:
  - 0x590D40-0x5A2A20 (73 KB) lobby/plaza UI: Lb_eat (eat scene), dialog/window/button
    drawing (SetDialogData, Draw_menu_square, DispButtonHelp ...), Lbs_plaza menus
    (plaza_*: friends, mail, chat log, search), npc movement scripts (npcMv*, npcCat*,
    npcPig*, lb_npc_*_move).
  - 0x5A2A20-0x5AE320 (45 KB, ~370 funcs) cnLBS network protocol: cnLBS_* (start a
    request in a CnetSys_w.bg slot), __cnet_SendReq_* (build packet in send_work),
    _cnet_RecvFromLbs_* (reply handlers), __cnet_bgProg_* (multi-step jobs),
    SetSendData*/GetRecvData*, lbs_encode_ex.
  - 0x5AE320-0x5B1E74 lbs_encode_ex and friends, 0x5B1E74-0x5C4E60 lobby client state
    machine: lm_* menus, lbc_* (login/browser/top menu/in plaza/in lobby), CallBack_Result_*,
    Split_TagCode, server_select_*. Agent F takes 0x5C4E60 to the end.

## Lobby status and lessons (agent B)
Source layout: `src/lobby/lb/` (town game logic) and `src/lobby/cnet/` (network layer).
Shared lobby headers: `include/lobby.h` (lb_pit/lb_sys/lbShop/LB_NPCW...), `include/lbnet.h`
(CnetSys_w, send_work, burst/bg slots), `include/lbnet_proto.h` (generated K&R declarations).
- lb_talk.c (0x533A00-0x535238): whole file matches, rodata jump table 0x654AD0.
- cnlbs (0x5A2A20-0x5AE320 network protocol): `src/lobby/cnet/cnlbs_nm.c` holds all ~300 functions
  written so far in address order; `tools/lbregister.sh` (uses tools/lbruns.py) cuts it into runs
  of contiguous matching functions (cnlbs.c, cnlbsb.c, ...) and registers them in c_files.txt.
  Add new functions with `tools/lbmerge.py src/lobby/cnet/cnlbs_nm.c NEW.c` (sorts by address,
  refreshes lbnet_proto.h). CnetSys_w fields live in `config/lbnet_fields.txt`
  (`tools/lbfields.py` regenerates the struct in lbnet.h).
- Near-match files (not built): src/lobby/lb/lb_mix_nm.c (forge shop, 0x535240-0x536708),
  lb_shop_nm.c (shop engine, Lb_shop_move is 4 instructions off), lb_em10_nm.c / lb_em09_nm.c /
  lb_em04_nm.c (NPC sound scripts, only the effect_move wrappers are 6 instructions off).
- tools: lbconv.py (m2c -> closer-to-C draft with lobby struct names), check.py now infers the
  module from the path (src/lobby/..) and has `--at NAME=ADDR` for functions whose name exists
  several times (static `sound_call` etc.).

Lessons that were each confirmed by a match:
- The lobby code uses K&R function definitions (`int f(idx, d) int idx; char *d; {`). With an ANSI
  prototype definition `(int idx)` the same body compiles differently (e.g. cnLBS_Get_PlazaName:
  `base + (u16)(idx-1)*0x164` is only produced by the K&R form). Params narrower than int
  (`s8 val`) must also be K&R-declared, and calls through unprototyped declarations pass
  nothing for forgotten arguments (stale registers in the original: m2c shows them as junk).
- `(u8 *)&CnetSys_w + 0x1234` arithmetic is common-subexpression-eliminated by MWCC (one address
  register kept across calls); the original did not, because it used struct members. Name the
  field in CNET_SYS instead (`&CnetSys_w.field`).
- A struct copy `*dst = CnetSys_w.field;` generates the original's copy loops; the element type
  decides the loop (u8 blob = byte pairs, s16 blob = halfword pairs, s32 blob = words).
- `switch (x) { case 0: case 3: ... }` (labels ascending) gives the compare order 3 then 0 that the
  original has (Lb_event_market); a trailing `return;`/`break;` in the last case adds a jump the
  original lacks (drop it).
- A by-value struct param (`CNET_RES res`, 8 bytes in a0) is spilled and read in place; do not
  copy it to a local first.
- Compare chains of a switch are in REVERSE source order of the case labels (lb_em* ef_move_sub).
- Statics with the same name in several files (sound_call): check with `--at`.

## Lobby: where I stopped and what is next (agent B)
Done and byte-matching (registered, lobby rebuild OK): lb_talk.c (0x533A00-0x535238) and 42 runs of
`src/lobby/cnet/cnlbs*.c` cut from cnlbs_nm.c (about 290 network-layer functions: the cnLBS_* request
starters, __cnet_SendReq_*, _cnet_RecvFromLbs_* handlers, the table getters, GetRecvData*/SetSendData*).
Near-matches kept in cnlbs_nm.c (a handful of instructions off, mostly register allocation or stack
layout): __cnetSub_Return_BgProcess (the done callback gets the slot pointer in a2 in a way I could
not reproduce), cnLBS_RecvData / __cnetSub_RecvThreeData / __cnet_RecvFromLbs helpers, the four
GetRecvData{String,Option,Option3} and SetSend{StringData,StringData2,EncodeStringData} (the
original recomputes `len & 0xFFFF` instead of CSE-ing it), the Match* handlers (2 nops of
alignment), cnLBS_Get_CurrentPlace, cnLBS_Get_GameServerAddress.
Not written yet in 0x590D40-0x5AE320: ~55 net functions (condition search, personal data
registration, the bgProg_* multi-step jobs, personal record tables, TopInformation/WarningMessage,
RuleControl, MemberSub/InOut/ReceiveJoinUser), and everything below:
- 0x590D40-0x5A2A20 (73 KB): UI/town code (Lb_eat, dialog drawing, Lbs_plaza menus, npc move
  scripts). Many reference string literals; MWCC puts <= 8 byte literals into .sdata (gp-relative)
  whereas the original keeps them in .rodata, so functions with short string literals do not match
  (cnLBS_Send_LoginUserAccount, __cnet_SendReq_EchoPacket are the two cases in the net layer);
  needs a compiler flag/pragma that is not known yet.
- 0x5AE320-0x5B1E74: lbs_encode_ex/write_col_numeric/read_col_numeric/mmbbc_encode (bit encoders),
  the item shop copy of the forge code (Lb_shop, lb_shop_select, ... 0x5AE8D0-0x5AFFA0, structurally
  the same as src/lobby/lb/lb_mix_nm.c), Lb_join / lb_select_* (room join menus).
- 0x5B1E74-0x5C4E60: login/browser/plaza/lobby state machines (lbc_*, lm_*, CallBack_*).
- The lb_* near-match files (lb_mix_nm.c, lb_shop_nm.c, lb_em*_nm.c) still need one more tuning
  round; K&R definitions (see lessons) were not yet tried on all of them.
- Static helpers (LOCAL symbols in docs/survey/mh1_symbols.csv, e.g. write_col_numeric/read_col_numeric)
  must be `static` in the near-match file: MWCC then does inter-procedural register allocation for
  their callers (mmbbc_encode keeps values in t0/t1 across the calls). tools/lbruns.py strips `static` in
  the run files (asm callers need the symbol); modifying the parameter itself (`buf += n - 1;`)
  instead of a new pointer variable fixed write_col_numeric's register allocation.
- `tools/lbfieldcheck.py`: tools/check.py ignores relocation addends, so a mistyped field in
  config/lbnet_fields.txt (e.g. `u8 *name` parsed as 1 byte) only shows in the rebuild; the checker
  compiles the header and verifies every CnetSys_w field offset (lbregister.sh runs it).

# Lobby round 2 (agent B, 5 Oct 2026): net layer finished, town NPC scripts, tools
New/changed tools: `tools/lbregister.sh` now handles two families (cnet/cnlbs_nm.c -> cnlbs*.c runs, lb/lbnpc_nm.c -> lbnpc*.c runs).
`tools/lbruns.py` verifies every generated run file with check.py (a function that stops matching inside its run, e.g. because a
`static` helper is not in the same run, is demoted and left in asm) and keeps `static` helpers static when all callers are in the run.
`tools/lbmerge.py NM.c NEW.c include/lbnpc_proto.h lbnpc.h` merges functions into any near-match file (proto header + base include).
`config/lbnet_rodata.txt` (START END FUNCTION) gives string literals / jump tables a rodata slot in the run file that holds FUNCTION.
`tools/lbconv.py` now names gp-relative globals of main.bin from config/symbols/main.txt (lobby gp = 0x38EB70).
Net layer (0x5A2A20-0x5AE320) status: all functions written except __cnet_bgProg_ReadRoomRule (2 KB, 19-state job with 8x-unrolled
table clears; asm read, not written). Matching and linked: condition search, personal data (bgProg_RegistPersonalData), room rule
set job (bgProg_RoomSetRule), InOutRoomMember, RuleControl, CheckCheckSum, personal record, patch, top information BattleResult etc.
Near-match (cnlbs_nm.c, not linked): bgProg_Read{Plaza,Lobby,Room}Allocation (19-23 instrs: register colouring in the check loop),
MatchOpponentInfo/Status (3), Warning/TopInformation recv (stack/regs), SendReq_ConditionSearchUser (3: loop init order).
Town NPC scripts (0x59DB40-0x5A2A20, src/lobby/lb/lbnpc_nm.c, headers include/lbnpc.h): all npcMv*, npc_move_common, lb_npc_*_move,
npcCat*, npcPig* written; linked in lbnpc*.c runs: all except npcPigSLEEP/TOPL/EXIT/WALK2 and lb_npc_old_guild (2 instrs, register
of a constant), npcCatWAITER (2.4 KB, jump table, not written). Pig/cat helpers use em.h names (x05 step, x15 action, work08 timer,
x194 anim wait) and LB_NPCMV (ex area: route list, idx, f0F, kind 0x0E, x26/x28, x2D).
Lessons (each confirmed by a match):
- SHORT STRING LITERALS (see BRIEF.md): `#pragma readonly_strings on` fixes the .sdata/.rodata mismatch.
- MWCC unrolls a plain counted loop 8x: do not write the unrolled body by hand (CheckCheckSum `for (i = 0; i < size; i++) acc += *p++;`,
  RuleControl 3-byte element copy, table clears). The preheader test `slt at,zero,n` is the loop's own, so write no outer `if (n > 0)`.
- `x >= C` vs `x > C-1`: the compare result goes to `at` (original) or into the value register; `if (n > 2)` instead of `n >= 3` fixed
  PersonalRecordHeader/Data and MemberSub (and `i = k + 1; if (count < i || i > 10)` style tests).
- m2c lists the labels of a ladder switch sorted by value; the asm compare ladder (beq chain) runs in REVERSE source order, so read the
  `addiu t,0,imm; beq x,t,L` sequence (script: /tmp ladder.py idea) and write the labels reversed. A group of case labels whose block
  is shared must be written in that reversed order too (npcMvTOPL: 14 empty cases that `break` come first).
- A `return;` that m2c shows after the last statement of a case is usually not in the source: the original branches threaded straight
  to the epilogue (bgtz -> end). If the compare ladder / bgtz goes to the epilogue use `break` / nothing; an extra `b end; nop` in
  your output means one `return;` too many.
- `if (a == 2 || a == 0)` gives `beq a,2,L; bnez a,else` (father_move); a `switch` with the same labels gives `beq zero..; b default`.
- One-case switches again (`switch (em->x05) { case 0: ... }`) give the `beqz / b end` shape; `case 1: break;` after case 0 when the
  original ladder tests 1 first.
- A local `LB_NPCMV *mv = (LB_NPCMV *)em->ex;` at the top makes the original's early `addiu a2,s0,0x444`.
- Functions called with an extra constant argument (Lb_pl_chr_set0 has 5 args, Lb_Pl_basic_flagset(em, 1, 0, 0), Lb_act_set(em, 0, act,
  idx)) show the extra zero registers; m2c drops a0 (em) and shifts the others.
- A float argument needs a prototype (frame_check2(EMW *, f32, int)); the f32 goes in $f12 regardless of position.
- Struct member arrays keep the `symbol+const` base (lb_sys.x88[idx] = 1 gives lui/addiu of lb_sys+0x88 plus idx); a separate extern
  symbol (D_3E4C05, in config/lobby_undefined_syms_auto.txt) is needed where the original loads `0(reg)` from symbol+0x15 plus offset.
- `u16 t = x - 1; x = t; if ((s16)t <= 0)` gives andi + dsll32/dsra32 (pig ATACK).
- check.py ignores relocation addends: burst[7] vs burst[9] (0xF34 vs 0xF7C) and rseq vs rseq2 only showed in the rebuild.
- Register colouring at the start of a function (em saved in s1 before the loads of player_work/x05, vs after in the original):
  npcPigSLEEP/TOPL/WALK2 and ReadXAllocation are still open; declaration order, scoped locals and extra K&R params did not help.

# Lobby UI (agent B): src/lobby/lb/lbui_nm.c, include/lbui.h (0x590D40-0x59DB40, plaza/dialog UI)
Started the UI region: ~55 functions written and linked (lbui*.c runs): dialog data/titles/help line setters, tl_menu cursors,
plaza_backToServer/checkChatLog/logOut/ReibunEdit/checkMyStatus, chat id lists, mail/comment/request input, page numbers,
scene titles, SetDialogData, plaza_selectMenu (5 instrs off, parked) ... Not started: Lb_eat/event_eat_* (0x590D40-0x591600), Draw_menu_square,
draw_dialog_square, DispDialogData, DispButtonHelp/put_button_help, plaza_enterLobby/movePlaza (+Trans), plaza_searchAll/Member,
plaza_mailBox(+Trans), plaza_setChatMode(+Trans), Plaza_add_friend, plaza_checkFriend (3.7 KB), disp_status, put_member_info, Lb_put_new_mail.
Parked near-matches: set_dialog_square (36, op order), plaza_selectMenu (5, `addu` operand order), plaza_chatMain (1), plaza_setMyComment (14),
Lb_addChatMember (56), Lb_clearChatMember (20).
Data structs (guesses): LB_NETW (pNet window state: idx/depth 2/step 3/x04/x05/x06/sel 7/menu 8/cur 9/x0C/x10/x24/x26/x28), LB_CW (cw chat work, accessed
through the CW macro because lobby.h declares cw as u8 *), LB_DIALOG, LB_TXT (x,y,string entries of text_lobby_msg), LB_SCOND, LB_PINFO.
More lessons (each confirmed by a match):
- A string literal shared by many functions of the original (lit_193_0065DBE8 "%s%s") must NOT be compiled into the run objects (each object
  would get its own copy and everything after shifts): declare `extern char lit_...[]` and keep the string in the asm data.
- Repeated `return 0;` in a switch is not in the source when the original has ONE `daddu v0,zero,zero` at the end: use `break` and a single
  final `return 0;`, with only the special cases (`return 1;`) inside (mail_input, my_comment_input, getHandleFromID `return 2` after the switch).
- A pointer loaded in each branch (`n = pNet;` repeated in the if and the else) is CSE'd at the merge point in the original; a single hoisted
  `n = pNet` is scheduled too early (lb_chatMemberCheck).
- `x >= 2` on a u8 global: write `x > 1`; `if ((u16)sw & 0x20)` gives andi 0xFFFF + andi; `s16 v = x24 + 1; x24 = v; if (v > 2)`.
- Unprototyped callees take stale extra arguments (disp_status has 8 args: a4..a7 are my_user_mini_data, pNet->x24, 3, D_3C73B4); Draw_menu_square(x,y,w,h,flag,color)
  ends with 1, 0xFF2A0000 (window) or 0, 0 (Tex variant).
- Static (LOCAL in docs/survey/mh1_symbols.csv) helpers: tl_menu_cursor_up/down are static so plaza_selectMenu reads a stale t1 after calling them.
- Integer arithmetic `master + (int)cw + 0x2BFE` fixes the operand order of the addu that `cw[...]` produces the other way round.

# Lobby round 3 (agent B, 5 Oct 2026): client/UI region 0x590D40-0x5C4E60, automation
Status of 0x590D40-0x5C4E60 (207 KB): ~75 KB matched and linked (lobby rebuild OK), ~98 KB compilable near-match C, ~34 KB with no C yet
(mostly big UI drawing / plaza functions and the login/logout state machines). All five modules OK.
Where the C lives:
- `src/lobby/b/lb_bzNN.c` (registered as `b/lb_bzNN`): ~140 runs of functions that came out byte-identical from the auto pipeline
  (`tools/lbauto.py` drafts, see agent-F.md) plus hand fixes. New tools for that pipeline:
  `tools/lbe2.py` (second chance for drafts that did not compile: void * -> u8 *, jump-table reads `*((int)&TBL + i*4)` ->
  `((int *)&TBL)[i]`, call tables -> `((int (**)())&TBL)[i]()`, s64 -> long long, redeclared externs dropped), `tools/lbcb.py`
  (CallBack_Result_*: by-value `CNET_RES res` parameter spilled to the stack, stale temp args dropped), `tools/lbcws.py` (cw accessed through a
  per-function struct), `tools/lbfld.py/lbfld2.py` (F(T,&lb_sys,off) -> typed members of include/lobby_b.h), `tools/lbvar.py` (`>= C` -> `> C-1`),
  `tools/lbtail.py` (drop the `return;` m2c puts at the end of the last case), `tools/lbblock.py` (`block_N: default: return X;` -> break + return).
  `tools/lbf_merge.py` now takes LBFL (function list file), LBDIR (output dir under src/lobby), FORCE_OK (names check.py cannot verify).
- `src/lobby/b/nm/NAME.c`: one near-match draft per function (compiles, not linked). Many are still m2c "int mode" code (pointers as int);
  measure closeness with `python3 tools/align.py FILE FUNC | grep -c '^replace\|^insert\|^delete'` (hunks), the instruction count
  from check.py is misleading when one missing/extra instruction shifts everything.
- UI region: `src/lobby/lb/lbui_nm.c` + `include/lbui.h` (eat scene: Lb_eat 17 instrs off (a final `b end; nop` after case 7 not reproduced),
  event_eat_rcpt/set_msg match, event_eat_trans_ot0 9 off (evaluation order of the two flfntLocate arguments), lb_eat_set 61 off);
  item shop `src/lobby/lb/lbshop2_nm.c` + `include/lbshop2*.h` (new family in tools/lbregister.sh): Lb_shop_init_member, Lb_shop, lb_shop_select,
  lb_shop_decide, lb_shop_listIcon match; lb_shop_put_itemDetail 6 off, CheckItemPrice_005AFEE0 24 off, tag_decide/item_select/checkMax parked.
- npcCatWAITER written (lbnpc_nm.c, 379 instrs off: register allocation of the four callee-saved pointers, the logic is complete),
  __cnet_bgProg_ReadRoomRule written (cnlbs_nm.c, 80 hunks, state machine complete), plaza_chatMain still 1 off (`addu` operand order),
  lb_npc_old_guild 2 off (register of the constant 0x69).
Lessons (each confirmed by a match):
- Typed globals matter: `lb_sys`/`pNet`/`lb_pit` as struct members (`lb_sys.x06`) give the original `lui %hi(lb_sys+6)` per access;
  `*((s8 *)&lb_sys + 6)` and F() macros make MWCC hoist the address into a saved register. include/lobby.h LB_SYS and include/lobby_b.h
  LBSYS_B carry all offsets seen in the asm. A 1-byte symbol+offset access can also be done with a local overlay struct
  (`((LBS1 *)&lb_sys)->x01++`).
- The client work `cw` is a struct pointer in the original: three separate `cw->x2C31 / cw->x2C45` reads in one function (no temp variable) give the
  original register allocation and the `addiu a2,a1,0x2C45` pointer; a temp `u8 *p = cw` does not (tools/lbcws.py).
- Callbacks `CallBack_Result_*(CNET_RES res)`: 8-byte struct by value, spilled with sd, read with lb 0x18(sp).
- `if (!(c)) return a; return K;` / `return c ? K : a;` removes a stray nop that `if (c) a = K; return a;` produces (lm_place_mv).
- `if (x == 3) {..; return 0x40;} return 0;` compiles with `bne/b` only as a single-case `switch (x) { case 3: ...; default: return 0; }` (lm_*_mv).
- A tail call with a constant argument (`str_stop(0)`, `fade_set(1)`, `To_LogOut(1)`): m2c drops the delay-slot `addiu a0`; also the delay-slot store before
  `Init_InterruptFlag` is dropped (add `cw->x2C3F = 0` before the call).
- A dead counter (`k++` never read) is removed by MWCC; the original keeps an index only when it is used (`eat_data_name[k]`/`eat_data_type[k]`).
- K&R params keep the callee-side narrowing: `connect_ps2(a, b, c) int a; s16 b; s16 c; { struct {int a; s16 b; s16 c;} t; ... CpInetTcpOpen(&t); }`
  spills the three arguments as one struct (4/4/2 bytes); a local array bigger than the passed size is real (Lbc_SendMiniData: `char sp10[0x20]`
  passed to memcmp/memcpy with 0x40).
- Constant-folded counts: `pages = cnt / 7; if (cnt % 7) pages++` with cnt = 20 is `3`.
- Calls with stale argument registers: Lb_draw_square has six args (x, y, w, h, 0xFF602020, 1), Lb_put_itemIcon four; cnLBS_Read_RoomRule* take a third
  (callback) argument that the matched 2-argument definitions pass through to __cnetSub_Set_BgProcess unchanged.
- `u16` K&R params (`CheckItemPrice_005AFEE0(id, qty) u16 id; s16 qty;`) avoid the `andi` that a prototype adds at the call sites.
- `Lb_eat`-style switch with an empty middle case: the jump table has 9 entries when cases 2 and 8 are absent but the highest label is 7 + `case 8: break;`.
Not started / still asm in this region: draw_dialog_square, Draw_menu_square, DispButtonHelp, plaza_checkFriend (3.7 KB), plaza_searchMember,
plaza_mailBox, the plaza *Trans functions (most have int-mode drafts in b/nm), Lb_put_new_mail, disp_status, lbc_login_*, lbc_logout_*, tk_logout,
Lb_menu_move_Core/DispLobbyMenu, test_server_sel_disp, Display_StringData/Analysis_TagCode, lb_npc_init/lb_npc_trans/lb_npc_item_trans.

# Lobby round 4 (agent B, 5 Oct 2026): 0x533980-0x5C4E60, near-match files turned into linked runs

Result: lobby (and all five modules) rebuild OK. About 60 new small linked runs plus the existing families grew; see `config/c_files.txt`
(`b/lb_by01..60`, `lb/lbmix*`, `lb/lbshp*`, `lb/lbem04/09/10*`, `lb/lbui*` incl. DispButtonHelp 1796 bytes, `cnet/cnlbs*`).
Not linked, near-match C that compiles (`src/lobby/b/nm/NAME.c`, `lb/*_nm.c`, `cnet/cnlbs_nm.c`, run `python3 tools/check.py FILE`):
the rest of the 0x537860-0x53D800 shop / armor / process family (lb_process_*, shop_armor*, lb_armor*, Lb_put_*; typed with include/lobby_s.h),
draw_dialog_square (8 off), Draw_menu_square, tk_logout (44 off), plaza_mailBox, plaza_checkFriend/searchMember/searchAll (int-mode drafts),
lbc_login_*/lbc_logout_*, Display_StringData, Analysis_TagCode, lb_npc_trans/init, Lb_menu_move_Core, DispLobbyMenu, disp_status.
Holdouts after a real attempt: npcCatWAITER, __cnet_bgProg_ReadRoomRule (no c_rawfuncs fallback needed: the family runs already link around an
unmatched function, which stays asm), plaza_chat* done; lb_npc_old_guild (the constant register of 0x69, 2 off), lb_mix_decide, Draw_menu_square
(x and y live on the stack in the original), connecting_00, cmcs_00, check_erase_dialog, event_eat_trans_ot0 (flfntLocate argument order),
_cnet_RecvFromLbs_MatchJoin/PlSide/Opponent* (res and the byte variable share one stack slot), value_result (64-bit locals).
Third-party code, not decompiled: 0x53E848-0x590D40 = Sony HTTP client (sceHTTP*, 0x53E848-0x54A000), HTTPS/SSL glue and the SSLeay-style crypto
library (ASN1, BER, BN, BIO, X509, EVP, DES, RC2/RC4, MD2/MD5/SHA1, DH/RSA/PK, SSLv2/v3/TLS1, PEM) up to 0x590D40.

New tools (all in tools/, see the docstrings): lbpromote.sh NAME... (matching b/nm/NAME.c -> linked run lb_byNN.c), lbregister.sh (now also lb_mix,
lb_shop, lb_em04/09/10 families and the jump-table slots; one slot per object, tables only separated by alignment are merged), lbjt2.py (jump tables of
functions that are already C), lbblk2.py, lbstale.py, lbderef.py, lbundef.py, lbshopfld.py, lbglob.py, lbcbnm.py, autodecl.py / permdecl.py /
permlines.py / permcases.py (brute force declaration order, statement order, case order; permuter.py does not work for lobby: the repo path has a
space), al3.py-style filtering of align.py output is in the scratchpad only. check.py now ignores a `_XXXXXXXX` address suffix on callee names (check.py
compares callee names with docs/survey/mh1_symbols.csv, so address-suffixed statics showed as differences); rebuild.sh is the proof. Tools that read
check.py output must read stderr too (compile errors go there).

Lessons (each confirmed by a match):
- CallBack_Event_* take `CNET_RES res` by value as well (frame 32 instead of 16 even if res is unused).
- `ok = f(x) > 0; if (!ok) ...` gives `slt v0` + bne (lb_mix_checkItemMake); `qty <= cnt` instead of `cnt >= qty` gives `slt at` (Lb_mix_item_checkMax).
- Prototype `int CheckItemPrice(int id, int qty)` at the call site while the definition is K&R u16/s16 (no andi at the call).
- `for (i = 0, p = x; ...)` instead of `p = x; for (i = 0; ...)` moves an addiu (Lb_mix_item_checkMax).
- A struct copy of three s16 (`*(S3 *)d = *(S3 *)t`) compiles to lh/lh/lh then sh/sh/sh (cnLBS_Get_CurrentPlace); a 20-byte struct of five f32 copies as
  lwc1/swc1 (draw_dialog_square: `*(F5 *)&sp = *(F5 *)t`).
- m2c `return;` in the middle of a switch whose cases return values = `r = ..; break;` and `return r;` after the switch (cnLbc_CheckInFloorOrder).
- `block_N: default: return X;` in m2c output = `break` + `return X;` after the switch (lbblk2.py fixed ten functions at once).
- Switch with the ladder in reverse source order: write the cases ascending when the compare ladder is descending (connect_10, DispButtonHelp inner switches).
- Stack slot sharing: `char buf[9]` instead of 8 moves a stack local (cnet __cnet_Recv_SearchUser); declaration order of scalars (autodecl.py) fixes saved-register
  numbering (id_select_00, Get_ServerColor).
- `v = (u16)Get_sw2(0)` keeps both masks of `v & 0xFFFF & 0x100`; `u16 pad = Get_sw2(0)` too (DispHelpLine, plaza_selectMenu); a u16 member loaded with lhu keeps
  `& 0xFFFF` as well (DispButtonHelp), a masked call result does not.
- `off = a->cur * 0x24; ... (off + tbl + 2)` fixes addu operand order (plaza_chatMain, plaza_selectMenu).
- Typed `lbShop` (include/lobby_s.h, struct LB_SHOP from include/lobby.h) was worth more than anything else for the shop family: one tools/lbshopfld.py pass.
  A table symbol that the original reaches gp-relative must be declared with a small size (`extern s32 shop_process00_tag[1]`).
- Struct arguments `put_button_help(int, int, int, u16)`: the u16 prototype adds the `andi 0xFFFF` at the call sites.
Header edits (all proven by a match): include/lobby_a.h and include/lobby_f.h: LBPLAYER.x24 (s8) carved out of _pad14 (Lb_pl_status_m); include/lobby_a.h
was regenerated from lobby_f.h by tools/lbauto.py (it only picked up fields already merged into lobby_f.h); new include/lobby_s.h (lobby_a.h plus typed
lbShop, shopList, lb_pit).

# Lobby round 5 (agent B, 6 Oct 2026): 0x533980-0x5C4E60, about 17 KB / 95 more functions linked
Result: lobby (and all five modules) rebuild OK. New linked runs: `b/lb_by61..by118+` (b/lb_by*.c), `b/lbarm01` (Lb_armor_shop), `b/lbsnd01-03`
(npc sound helpers). Newly linked: shop/armor/process family (Lb_make_mySrcEquip, lb_put_shopHelp, lb_put_mk_tags, Lb_shop_sw, Lb_shop_item_checkMax,
Lb_armor_shop, lb_process_decide, lb_process_tag_decide01, random_stack, lb_armor_decide/itemBuy, armor_set_myArmor, armor_shop(2)_trans, Lb_shop_move_x/xR,
Lb_shop_init/talk, lb_shop_put_shopHelp), login/logout machines (lbc_login_patch/error/reguration/finish_after, lbc_logout_01, lbc_top_menu_01,
lbc_in_plaza_03/04, lbc_in_lobby_00_05/03_01/03_02, lbc_game_ready_02/04, lbc_browser_04, check_warning_level, check_top_information_level,
lbc_text_lobby_trans, To_LogOut, To_EnterRoom, Lbc_ReserveRoom, Lbs_GuestEnterRoom, get_next_server, set_event_npc, server_select_sub_02/04, disconnect,
lobby_return_to_lobby), callbacks (Plaza/Lobby/Room ReadAllocation, ReadCurrentPlace, Lobby/InRoom JoinUser, GuestRuleAllocation), net_ToNetworkLobby,
Lbs_GetLobbyMemberList, DispLobbyMenu, cnWrap_SetFontSize, internet_to_modem, Lb_npc_mv.
Still near-match (b/nm or lb/*_nm.c): draw_dialog_square (8 off: the last two Put_sprite_rotate float operand registers), Draw_menu_square (not
retried), tk_logout (case-local re-reads of COM_R_No_Logout, 8 hunks), plaza_mailBox and the plaza_*/disp_status family (still int-mode drafts),
lbc_logout_00 (7 off, store scheduling of the COM_R_No_x block), lb_select_room_list (1 off, delay slot), lbc_in_lobby_03_00 (7), lbc_admin_message_00/01,
item_to_stack (1: addu operand order), check_erase_dialog, lb_armor_itemBuy/lb_put_shopList (cast CSE, see below).
Matching lessons (each confirmed by a match):
- `return;` vs `break;` at the end of a switch case that ends in an if-block decides whether the false branch jumps straight to the function end
  (lbc_in_plaza_03, server_select_sub_02/04, lobby_return_to_lobby). m2c writes `return;` everywhere; try `break;` first. Brute force 2^n over the tail
  statements found lobby_return_to_lobby (tools below). A mid-switch `if (x != 2 && x != 0)` is `switch (x) { case 0: case 2: break; default: ... }`
  (armor_shop_trans, Lb_shop_talk: write the labels in the reverse of the ladder order).
- State machines on the client work: `typedef struct { u8 pad0[0x2C34]; u8 x2C34; ...} CWS_x; #define CWX ((CWS_x *)cw)` and `switch (CWX->x2C34) {...
  CWX->x2C34++; ...}`, NO local copy of cw: gives the original `lw cw` per access (lbc_login_reguration, lbc_in_lobby_03_02, lbc_logout_01...).
  `u16 sw = Get_sw2(0); ... (sw & 0x60)` reproduces the original double mask; `s32 sw = Get_sw2(0) & 0xFFFF; ... sw & 0xFFFF & 0x20` also; try both.
- Stale arguments: the m2c drafts pass register leftovers (`Check_CallBackWait(&jtbl_xxx, temp_a1)`); drop them (CallBackWaitInit(), Check_CallBackWait(),
  fade_set(1), Fade_busy_ck(), Lbc_init_network_work(), To_TopMenu()). One pass of tools below did this automatically (lbc_login_patch matched with just that).
- gp-relative vs lui/addiu access of the same symbol: a data symbol of 8 bytes or less is gp-relative; declare it with its real size (`extern s16 Vs_Cnt_0;`,
  `extern char ot5[4]`, `extern CNW CnetWork` with a struct bigger than 8 bytes for the lui form). `extern char X[]` (unsized) always gives lui/addiu.
- Typed globals beat F(T, &sym, off): a struct typedef for CnetWork (`CnetWork.x05`), ClassInfo (`.x2`, `.x6`, `.xA`), RoomInfo/PlazaInfo/LobbyInfo arrays of
  0x15C-byte records, ... removes the hoisted `addiu s0, sym` register (lbc_login_finish_after, check_warning_level, lbc_top_menu_01, Plaza ReadAllocation).
  A repeated reload like `i == ClassInfo.x2` / `i < ClassInfo.x2` must stay two separate reads.
- Callee prototypes with narrow parameters change the call site: `int cnLBS_Get_PlazaStatus(u16 id, void *p);` declared in the file (not in the header) makes MWCC
  convert `i + 1` at every call (`addiu v0,s0,1; andi a0,v0,0xFFFF` each time); with a K&R declaration the converted value is shared in a saved register.
  Also fixes To_EnterRoom (`int n` + u16 prototypes). Where lobby_a.h declares the function K&R (flfntLocate) rename it around the include:
  `#define flfntLocate flfntLocate_hdr / #include "lobby_a.h" / #undef flfntLocate / void flfntLocate(int, s16);` (disp_string_handle: not yet a match).
- A temp pointer `u8 *st = (u8 *)cw + 0x2C35; switch (*st) { ... (*st)++ ...}` is real in some functions (Lbc_ReserveRoom) where the m2c draft names
  `temp_a1 = cw + 0x2C35`; use it when the original keeps `addiu a1,cw,0x2C35` and `sb v0,0(a1)`.
- `(int)&SYM + K` as a call argument must be `&SYM[K]` (Lbs_GuestEnterRoom: one `lui/addiu` pair against SYM+K, not SYM then +K).
- `x > 0` vs `0 < x` selects `slt at,zero,x; beq` instead of `blez` (lbc_game_ready_02, get_next_server); `n >= m` vs `m <= n` swaps `slt v0`/`slt at`.
- Struct copy of three s16: `typedef struct { s16 a, b, c; } S3; *(S3 *)(pl + 0x35E) = *(S3 *)m.wp;` (Lb_set_mini_data_to_pl, Lb_make_mySrcEquip).
  A store of constant 1 into a 3-s16 block via sb is `lbShop.x5A[0] = 1`, not sh (Lb_make_mySrcEquip: final `x54 = 1` is a byte store).
- Several `lbShop.tbl + cur*8` loads: `e = (s32 *)(lbShop.cur * 8 + (int)lbShop.tbl)`, or `lbShop.tbl[lbShop.cur * 2]` / `[... + 1]` (random_stack, lb_armor_decide).
  Permuter hint accepted (lb_armor_decide): `new_var2 = lbShop.cur;` before an unrelated store changes the load order.
- Unmatchable without the callee in the same translation unit: functions that hold a value in a1..a3 across a call (Lbs_ExitAndEnterPlaza,
  Lbc_SetPropaty, CallBack_Event_ChatMessage/TU keep `&msg[0x11D]` in a1 across get_font_col): MWCC knew the callee clobbers fewer registers, so the original
  source file contained the callee. These stay near-match (not a missing trick).
- Permuter: works from a symlinked path without spaces (`ln -s <worktree> /tmp/x; cd /tmp/x; PERM_ASM_DIR=<snapshot of asm/> python3 tools/perm.py lobby FUNC FILE -j1`),
  redirect output to a file (it prints one line per iteration). A near-match file that contains K&R forward declarations of the form `void f(em);`
  makes the permuter's base compile fail (pycparser): cut the function into a small file first. Found cnWrap_SetFontSize (`s = size; w = (u32)s;`) and
  the `ClassInfo[8]` re-read in CallBack_Result_InRoom00_JoinUser.
New helper scripts (kept in the scratchpad, not committed; the ideas are enough to redo them): stale-argument dropper, `(int)&SYM + K` -> `&SYM[K]`,
comparison mirroring (WARNING: its operand regex mis-parses `a + b * 7 >= c`; I reverted three bad rewrites, check semantics), CnetWork struct typing,
return/break toggler (single and pair), gp-size fixer. They only keep a rewrite when check.py's diff count falls.
No shared header was changed this round (lobby_p.h was tried and removed). New C files are all under src/lobby/b/ and registered in config/c_files.txt.

# Lobby round 6 (agent B, 6 Oct 2026): village vs online classification of the unlinked functions in 0x533980-0x5C4E60
How: a call graph of lobby.bin (jal/j targets, lui+addiu/ori function addresses in code, and function-pointer tables in data: a table counts as
reached when a reached function loads an address inside it), rooted at Local_main (0x5D8680, F's range; its step table runs init_pre, init_init, init,
Lb_move_common, exit, event), plus the offline menu entries Lb_Menu_Init, Lb_menu_move_Core, DispLobbyMenu, Disp_lb_menu. Third-party HTTP/SSL/crypto
(0x53E848-0x590D40) skipped. Caveat: the village and the online lobby share one code path switched by Online_ck(), so "reached" does not prove an
offline run executes it. Names with a plaza/cnet/Lbc/lbc/lm_/lb_select/join/mail/server flavour that the graph reaches only through the shared
walking loop are listed as online. Size in bytes after the name.
VILLAGE (offline reaches it; the first group is the shop / item process / armor / mix / eat / NPC / dialog / menu family):
npcCatWAITER 2448, disp_status 2008, shop_select_items 1940, Draw_menu_square 1896, lb_process_drawHelp 1692, lb_process_kyoukaListProg 1504, lb_process_make_kyoukaList 1396, Display_StringData 1380, DispDialogData 1312, draw_dialog_square 1248, lb_npc_init 1228, shop_process_after 1144, lb_process_set_weaponList 1120, Lb_menu_move_Core 1120, event_eat_trans_ot0 980, lb_npc_trans 968, Lb_process_shop 960, lb_mix_put_itemDetail 948, lb_process_select 932, lb_npc_old_guild 864, Analysis_TagCode 856, lb_npc_item_trans 856, lb_process_set_armorList 848, shop_armor2_question 848, lb_shop_item_select 848, lb_mix_item_select 800, lb_shop_put_itemDetail 752, Put_page_num 748, lb_shop_tag_decide 720, lb_armor_put_itemDetail 712, lb_mix_tag_decide 704, shop_armor_question 704, Analysis_StringData 672, npcPigTOPL 628, Lb_shop_trans2 620, lb_put_shopList 612, shop_armor2_stack 596, npcPigWALK2 556, Lb_npc_mk 556, lb_armor_tag_decide01 548, Disp_lb_menu 524, Lb_eat 508, lb_mix_makeMixList 500, lb_armor_tag_decide00 476, Lb_put_armorIcon 472, lb_mix_decide 440, lb_process_use_item 428, item_to_stack 412, lb_eat_set 408, npcPigSLEEP 364, Lb_put_itemIcon 364, lb_npc_move 336, Lb_put_job_limit 328, Lb_put_materialItem 304, npcPigEXIT 304, Lb_gh_board_trans 300, set_se_type 296, Split_TagCode 272, lb_set_npc 260, lb_armor2_listItem 252, put_main_cursor2 228, lb_cat_material 224, value_result 216, Lb_pl_status_i 208, CheckItemPrice_005AFEE0 192, Lb_gh_board 192, put_main_cursor 180, set_dialog_square 172, lb_normal_material 140, Lb_ck_menu 124, Lb_set_mini_data 244
ONLINE (reached via shared code but only meaningful with a network, or not reached at all):
plaza_checkFriend 3748, plaza_searchMember 2524, Lb_join 2372, plaza_mailBox 2200, __cnet_bgProg_ReadRoomRule 2040, plaza_searchAll 1664, lb_select_tag 1504, plaza_mailBoxTrans 1432, plaza_setChatModeTrans 1352, plaza_setChatMode 1332, plaza_searchMemberTrans 1320, plaza_enterLobbyTrans 1272, Plaza_add_friend 1220, lb_select_set_data 1144, plaza_enterLobby 1072, lbc_login_init 1068, plaza_checkFriendTrans 1028, __cnet_bgProg_ReadRoomAllocation 1004, lm_room_member_mv 996, plaza_movePlaza 988, lm_member_list_mv 912, __cnet_bgProg_ReadPlazaAllocation 876, __cnet_bgProg_ReadLobbyAllocation 876, lbc_logout_00 868, lbc_login_id_select 860, plaza_trans_ot0 856, Lb_put_new_mail 840, put_mail_input_square 836, tcp_init 820, plaza_setMyCommentTrans 808, test_server_sel_disp 808, Lbc_ConditionSearch 764, internet_lobby_act 756, Lbc_GuestReadRoom 744, internet_browser 720, CallBack_Result_LoginLobbyServer 700, CallBack_Result_Plaza_LobbyMember 692, server_select_05 680, lbs_encode_ex 668, lb_select_trans 648, Lbc_GetRoomRule 644, CallBack_Event_RecvMail 636, tk_logout 616, getFriendNow 608, CallBack_Event_RoomLeaver 608, plaza_disp_mail 604, plaza_movePlazaTrans 600, lm_member_trans 584, lbc_login_users_personal_data 548, select_ps2 540, lbc_game_ready_00 540, lbc_login_warning_message 532, lbc_login_finish 520, lbc_login_top_information 520, _cnet_RecvFromLbs_MatchOpponentInfo 512, _cnet_RecvFromLbs_MatchOpponentStatus 504, cmcs_04 504, lb_select_room 492, put_member_info 472, plaza_capcomPage 468, __cnet_SendReq_ConditionSearchUser 460, net_time_move 456, plaza_setMyComment 444, Lbc_SetRoomRule 436, plaza_moveMain 428, server_select_00 424, Lbs_plaza_trans 416, server_select_01 408, Lb_on_dialog 404, join_input_password 396, lbc_in_lobby_03_00 396, transSelectHandleName 396, net_time_str 392, lbc_admin_message_00 380, lm_place_trans 364, disp_lm_room_member 356, CallBack_Event_LobbyCommer 348, lobby_client_admin_message 340, Lb_clearChatMember 336, id_select_01 328, _cnet_RecvFromLbs_RequestWarningMessage 320, lb_select_room_list 316, DispNameAndIDonDialog 296, __cnetSub_Return_BgProcess 296, CallBack_Event_LobbyLeaver 292, Lb_addChatMember 284, disp_string_handle 284, check_room_require 280, lb_put_room_member_005B1930 276, _cnet_RecvFromLbs_AnswerTopInformation 272, nwDispStr_Html 264, CallBack_Event_MatchEntryUser 264, CallBack_Event_RoomCommer 256, CallBack_Event_MatchStart 252, Lbs_ExitAndEnterPlaza 244, disp_string_id 240, lb_put_comment 236, cmcs_02 228, CallBack_NoticeUserMiniData 228, cnLBS_RecvData 212, __cnetSub_RecvThreeData 212, check_halfcode 212, __cnet_Recv_UserIDandHandle 200, connecting_10 196, lbc_admin_message_01 196, cmcs_01 188, Lbc_SetPropaty 188, CallBack_Event_ChatMessageTU 172, _cnet_RecvFromLbs_MatchPlSide 164, _cnet_RecvFromLbs_MatchGameServerAddr 164, SetSendStringData2 148, SetSendEncodeStringData 148, Get_ServerName 148, _cnet_RecvFromLbs_MatchBattleCode 140, _cnet_RecvFromLbs_MatchGameRule 140, mmbbc_encode 140, net_Check_FriendData 140, connecting_00 140, cnLBS_Get_GameServerAddress 136, _cnet_RecvFromLbs_MatchJoin 132, _cnet_Return_CallBack 132, _cnet_RecvFromLbs_NoticePatchStart 116, CallBack_Event_ChatMessage 116, tk_logout_message_sub 112, GetRecvDataString 108, transOtSelectHandleName 108, __cnet_Recv_PatchData 92, SetSendStringData 88, cmcs_00 88
(The list is as of the start of round 6; round 6 linked many of the VILLAGE entries, see "Lobby round 6: results" below.)
UNDECIDED (not reached by the graph, probably called from tables built at run time): ef_move_sub_0053E360 1064, create_server_table 900, ef_move_sub_005C49F0 760, tk_dialog_mv02 680, tk_lever_ck 508, check_erase_dialog 48
 (ef_move_sub_* are effect movers: village-likely; tk_dialog_mv02/tk_lever_ck/check_erase_dialog are talk-window helpers; create_server_table is online.)

## Lobby round 6: results (agent B, 6 Oct 2026)
Linked this round (all rebuild OK): village side: Lb_eat, lb_mix_tag_decide (lbui/lb_mix family runs), Lb_ck_menu (by119), lb_normal_material (by120),
lb_cat_material (by121), set_se_type (by122), Lb_gh_board_trans / Lb_gh_board / Lb_pl_status_i (by123), put_main_cursor / put_main_cursor2 (by124),
Lb_put_armorIcon (by125), Lb_put_job_limit (by126), Lb_npc_mk (by127), lb_armor_tag_decide01 (by128), lb_armor_tag_decide00 (by129), item_to_stack (by130),
shop_armor2_stack (by131), lb_armor2_listItem (by134). Online side: lb_select_room_list (by132), check_erase_dialog (by133).
Near-matches left (village): draw_dialog_square (8, float operand regs of `20.0f * h`), Draw_menu_square,
lb_process_use_item (14, base of shopList+0x26), shop_armor_question (55), lb_process_select, lb_armor_put_itemDetail (kind/id registers), value_result,
Lb_put_materialItem (17, id/need saved registers), event_eat_trans_ot0 (9), lb_mix_decide (4), lb_mix_put_itemDetail (6), lb_shop_put_itemDetail (6),
lb_npc_old_guild (2), lb_npc_move (21: tail layout), npcPigTOPL (14). Their best C is in lb/*_nm.c or b/nm/*.c.
New matching lessons (each confirmed by a match):
- Declaration order picks saved registers: the FIRST declared local gets the HIGHEST s-register (lb_armor_tag_decide00: `int i; EQREC *r; LB_SHOPITEM *sl;
  SHTBL *t;` gives i=s3, r=s2, sl=s1, t=s0). tools/declbf.py only handles ANSI one-line signatures (K&R drafts: convert first).
- Loop strength reduction: write `shopList[i].x`, `D[i].y`-style pointer variables initialised at the top (`sl = shopList; r = D;`, `i++, r++, sl++`) and let a
  table that the original loads late (after a call) be a pointer assigned just before the loop (lb_armor_tag_decide00).
- Entry address operand order (`addu v0,v0,a0` vs `addu v0,a0,v0`): `u16 *lp = (u16 *)lbShop.list; int c = lbShop.cur; lp[c * 20 + 0x13]` gives the original
  order and load order (item_to_stack, shop_armor2_stack); the one-expression forms do not.
- A switch whose ladder is plain compares (no range trick) with several labels per target is a `switch` in the source, not `||` (that gives sltiu range
  tests): lb_cat_material (`case 2: case 3: case 4:`, ladder = reverse label order). A trailing `return;` in the last case (not `break`) fixes the
  extra `b end` (Lb_eat case 6/7).
- Params: `int kind` (not `s16 kind`) when the original keeps the raw a-register and converts at each use (`(s16)kind`, `(u8)kind`); declare the callee's
  narrow parameters locally so the caller sign-extends (Lb_put_armorIcon(int,int,int,s16,s16)). `(u8)kind` in one use and `kind & 0xFF` in the other stops MWCC
  from CSE-ing the mask into a saved register (Lb_put_armorIcon).
- 12-byte rectangle records on the stack: `typedef struct { s16 v[6]; } R12; R12 a = lit_3380;` copies with ld + lwc1 like the original (put_main_cursor).
  An 8-byte local struct for Get_equip_bit's out parameter (`{u8 x0; s8 kind; s16 id; u8 x4[4];}`) reproduces the frame (Lb_put_job_limit, lb_armor_tag_decide01).
  Small globals (shop_armor01_tag, 8 bytes) must be declared with their size to get the gp form.
- `i = (s16)n + lbShop.x6C * 7;` (explicit cast, plain `int` sum) matched lb_armor2_listItem where `i = lbShop.x6C*7; i += n` did not.
- `get_quest_info()` is called with no argument in lb_select_room_list (the m2c draft passed a stale 7).
- Permuter on these tiny near-matches (connecting_00, cmcs_00...) found nothing below the base score in 5-10 minutes each (base scores include relocation noise); not worth the CPU.

# Lobby round 7 (agent B, 6-7 Oct 2026): village first
Linked (all in src/lobby/b/lb_by135-152.c, registered in config/c_files.txt as `lobby ... b/lb_byNNN`; rebuild OK for all five modules):
- by135 lb_set_npc, by136 lb_npc_move, by137 lb_put_shopList, by138 shop_armor2_question, by139 Lb_shop_trans2, by140 tk_lever_ck,
  by141 Put_page_num, by142 lb_mix_item_select, by143 lb_shop_item_select, by144 Lb_process_shop, by145 npcPigEXIT, by146 lb_mix_makeMixList,
  by147 npcCatWAITER (2448 bytes, jump table rodata 0x65DEA0-0x65DEC8), by148 npcPigWALK2, by149 npcPigTOPL, by150 npcPigSLEEP,
  by151 Lb_menu_move_Core (village / lobby start menu, two jump tables 0x65E6A0-0x65E720), by152 Disp_lb_menu.
  All village (NPC placement/walk/serve, shop list/detail/select, forge, armor shop, talk lever, pig NPCs, start menu). None is online-only,
  though Lb_menu_move_Core's pages 8-15 are the online menu entries.
Biggest lesson of the round: the m2c-style drafts were far closer than their check.py counts said. Run `python3 tools/align.py FILE FUNC --module lobby`
(not the "N/M differ" figure, which counts every shifted instruction) and fix what the asm really does: m2c drafts passed junk arguments
(`Lb_get_angle(pl->pos, player_work, off)` is really `Lb_get_angle(em, pl->pos)`; `pl_flag_set(pl, ..)` is really `(em, ..)`; `Menu_x_i(lbmw)` is
`Menu_x_i()` with a0 left over; a "stage4" second argument was a stale register) and a stray `int off` variable. Removing those, plus `break` for a
trailing `return;`, turned 28-379 differing lines into 0 for npcPig*/npcCatWAITER/Lb_menu_move_Core.
New matching lessons (each confirmed by a match):
- A callee with narrow parameters (`flfntLocate(int x, s16 y)`, `Lb_put_icon_free(s16, s16, int, int, int)`, `Lb_mix_item_checkMax(u16 id, s8 qty)`)
  declared in the file makes the CALLER narrow each argument at the call and stops MWCC from CSE-ing the mask / sign extension into a
  saved register (lb_put_shopList y, Put_page_num x/y, lb_mix_item_select / lb_shop_item_select id). The headers declare these K&R, so
  rename the header declaration first: `#define flfntLocate flfntLocate_hdr` before the include, `#undef` after, then declare yours.
  Writing `(s16)x - 0x18` inline in each argument (no x/y temporaries) matched Put_page_num; a temporary made the allocator swap s0/s2.
- `u16 key = lbShop.key` loaded first and a separate `int k = key & 0xFFFF;` (not `key = key & 0xFFFF`, which the compiler folds away) gives
  the original `lhu` ... `andi` pair; put the mask inside the `if (armor_shop_r == 0)` block when the original does (shop_armor2_question).
  Same for lb_mix_item_select (`keys = lbShop.key;` first, `k = keys & 0xFFFF;` after the id load).
- A shared `return 2;` after an if/else block: when the original's `addiu v0,zero,2` sits at the very end after the `return 0` path, the
  source is `if (a == 0) { ... (no return) } else { ... return 0; } return 2;` (shop_armor_question / shop_armor2_question).
- Last case of a switch ending in `return;` adds a stray `b`: use `break` (npcPigEXIT). A switch whose "matched" cases all go straight to
  `return;` with the real work AFTER the switch gives `beq ...; b skip; L: b end` (lb_npc_move: the 9 slot kinds that skip ground snap).
- `if (cond1 == 0 || cond2) { state = 1; } else { state = 0; }` lays out the else (state = 0) after the then, the original order, where
  `if (A && B) state = 0 else state = 1` does not (lb_mix_makeMixList). `md->price > funds` gives `slt at` where `funds < md->price` gave `slt v0`.
- Declaration order picks saved registers in reverse of first use: lb_mix_makeMixList wanted `num, rt, idx, sl, rec, md, j, no, tb` (the old
  nm file had them in the opposite order and every s register was permuted). A 10th local that does not fit (8 s regs + fp) is spilled to the
  stack (the 8-bit `cnt` read: `sw v0,0xA0(sp); lw v0,0xA0(sp)`).
- Loop counters: `i = 0` moved INTO the `if` that guards the loop changes which s register `i` gets (lb_mix_item_select).
- `(u8)kind` in all four uses (not `kind & 0xFF`, not mixed) was needed in lb_armor_put_itemDetail to keep the mask un-CSE'd.
- gp-relative globals must be declared with a size <= 8 (wait_157[4], D_38A82E[2], r_no_process, armor_shop_tmp is not gp).
- ANSI vs K&R: `s32 f()` vs `s32 f(void)` made no difference to codegen here.
Near-matches (village) after this round, with the real remaining difference (counts exclude relocation noise):
- lb_mix_decide (4): `lui s0; sll; addiu` order of `mixData + cur` and the `sll v0,a0,9` register for `&player_work[idx]`.
- lb_mix_put_itemDetail / lb_shop_put_itemDetail (6): `(s16)have` sign-extension goes to t0 in the original, v1 here, plus lw/sll order in case 1.
  Permuter (-j1, 15 min on a standalone copy) found nothing.
- lb_npc_old_guild (2): the constant 105 uses a2 (the register holding `mv`) in the original, v1 here.
- draw_dialog_square (8), event_eat_trans_ot0 (9): see round 6; event_eat: the x load after the y load only fails for the three
  `y = M[1] + 0x16; flfntLocate(M[0], y)` sites that are followed by a one-argument font_print.
- lb_process_use_item (14): base `shopList + 0x26` indexing is right, but n*40 uses a0 as the destination in mine, v1/a1 in the original.
- Lb_put_materialItem (17): saved-register order of id/need (s3/s1); compare forms tried.
- shop_armor_question (3): cur/tbl load order and register of the entry address; everything after matches.
- lb_armor_put_itemDetail (about 10, prologue only): body matches.
- lb_shop_tag_decide: the original shares `cnt` between the two branches (`pages = cnt/7` after the if/else) and its loop in mode 0 uses 5 s regs
  (the Item array pointer `it` is not live there); not reproduced.
- lb_process_select: the original keeps the mode in v1 and constant 1 in v0; mine swaps them; not reproduced.
Online-flavoured functions that the round-6 graph listed as village but are really the in-game web browser: Analysis_TagCode,
Analysis_StringData, Split_TagCode, tk_dialog_mv02 (nwDispStr_Html). Not attempted.

More lessons from the second half of round 7:
- A function whose conditions are `x68 != 0x11 || s6 == 6 || s6 == 7` first and the real work after: write the leave-branch as the then-part
  (`if (cond) { leave; return; } work`), not `if (!cond-ish) { work } else { leave }`; the original lays the leave block out first
  (npcCatWAITER cases 5, 6, 7).
- `((s32 **)&((u8 *)tbl)[0x3C])[stage]` keeps `tbl+0x3C` as one address constant (`addiu v1,v1,sym+0x3C`) instead of folding 0x3C into the lw offset.
- Declaring `PLW *pl` before a K&R-style `int off` or using `pl = &player_work[game_w.master]` matters: m2c's `(u8 *)player_work + off` form gives a0/a1 swaps.
- `int n; page = (u32)n >> 3; sel = n % 8;` reproduces `srl` + the signed-mod fix-up of Disp_lb_menu (the nm had `u8 n` and `& 7`).
- `u16 r = Menu_xxx_mv(keys)` for int-returning callees gives the `andi v0,v0,0xFFFF` after each call and `daddiu` for a u16 default constant;
  keep the *_i handlers `int` (their result is copied with `daddu s0,v0,zero`, no mask).
- Tried and not solved (register/ordering only): lb_npc_init (typed near-match now in b/nm/lb_npc_init.c; the original clears the 32 flag bytes
  at em+0x4E6 with a counter in a0 and `em+a0` recomputed per iteration, my loops always strength-reduce it), lb_eat_set (the original keeps a dead
  `k++` counter in s18: 7 saved registers), lb_process_select, lb_shop_tag_decide (shared `cnt` after the if/else), draw_dialog_square
  (float registers f1/f2 swapped on the `0.025f * (20.0f * tw)` expression, expression order did not help), ef_move_sub_0053E360 (compare ladder uses
  a1/v1 where mine uses v1/v0, 17 instructions of register names only), set_dialog_square (the original keeps `addiu $11,$11,0x28` between the three
  table rows, MWCC folds pointer steps into offsets in every form I tried).
PC build (tools/build_pc.sh, docs/pc.md "Village"): functions I matched whose PC version is a near-match or stand-in: lb_set_npc and lb_npc_move (stand-ins in
lb_village_nm.c), Lb_menu_move_Core (src/lobby/b/nm/Lb_menu_move_Core.c, m2c draft that passes lbmw to the *_i handlers; lb_by151.c calls them without
arguments as the asm does), Disp_lb_menu (lb_menu_nm.c, matched form differs only by `int n` / `n % 8`), npcPigEXIT/WALK2/TOPL/SLEEP and npcCatWAITER
(lbnpc_nm.c: corrected in place, see above; PC behaviour changes where the drafts passed wrong arguments), lb_mix_item_select / lb_mix_makeMixList
(lb_mix_nm.c) and lb_shop_item_select (lbshop2_nm.c) unchanged. The by files themselves are not compiled by build_pc.sh.

# Lobby round 8 (agent B, 7 Oct 2026): village
Linked: lb_process_make_kyoukaList (lb_by155, village forge/upgrade list; all five modules OK). Its nm copy stays in src/lobby/b/nm/ because build_pc.sh compiles it.
Near-matches (best C in src/lobby/b/nm/ unless noted):
- lb_process_kyoukaListProg: 3 hunks (about 12 instructions); only the 0x400 branch (lim s8 conversion registers) differs.
- shop_select_items: 1 hunk; same s8 `lim`/`x6E + 1` compare as above. Best form is not in the repo (lb_shop_nm.c holds the older m2c draft): `lim = x6E-table == 7 ? 4 : 2` ternary, `lbShop.x6E++; n = lbShop.x6E; if (lim <= n)`, other branches use `x1C == 0 || x8E == 0 || (x8E == 3 && x1C != 2)` as the if condition, `x > 1` instead of `>= 2`, s8 local for x6C.
- lb_npc_item_trans: first version written (arrays of rotation/offset floats, em_frame_check2 case 0x2AD returns early); constant stores scheduled differently (about 20 hunks).
- lb_npc_trans, disp_status, lb_process_drawHelp: first hand-written C, register allocation far off (30-85 hunks); lb_process_select, lb_eat_set, event_eat_trans_ot0, lb_process_use_item unchanged.
Lessons: a `switch (x) { case 0: ... }` single-case form reproduces `beqz; b` layouts (lb_npc_item_trans); `if (a == 0 || b != 7)` first gives "then" block before the switch (kyoukaListProg tail); `price > money` (not `money < price`) fixes the load order of the `sltu` compare; `sl++; sl++;` keeps real pointer increments where `sl += 2` folds into offsets (make_kyoukaList); hill-climbing over statement order (random move/swap, score = align hunks) found make_kyoukaList's store order after hand tries failed.

## Lobby round 8, part 2 (agent B, after the crash): raw-byte holdout files
Linked (rebuild OK for all five modules) as files whose only PS2 content is an `asm` stub fed by the original bytes (config/c_rawfuncs.txt,
tools/b_rawwrap.py wraps a C definition as `#ifdef __MWERKS__ asm ... #else C #endif`). These are NOT true matches; the near-match C stays beside them
(in the by file under `#else`, or in the nm file named in the file's header comment) and is what build_pc.sh uses:
by156 shop_select_items (1 hunk: `slt at` vs `slt v0` in the x6E+1 compare), by157 lb_process_kyoukaListProg (same 1 hunk), by158 lb_process_use_item
(shopList+0x26 base ordering), by159 Lb_put_itemIcon (first real C: sprite struct as s16[10], Item_data rows through `(&Item_data[0][5])[id*16]`; schedule/reg diffs),
by160 value_result, by162 lb_armor_put_itemDetail, by163 shop_armor_question, by164 lb_process_select, by165 lb_process_drawHelp, by166 disp_status,
by167 lb_npc_trans, by168 lb_npc_init, by169 lb_eat_set, by170 event_eat_trans_ot0, by171 set_dialog_square, by172 draw_dialog_square, by173 Draw_menu_square,
by174 lb_npc_old_guild, by175 lb_shop_put_itemDetail, by176 lb_shop_tag_decide, by177 Lb_put_materialItem, by178 lb_mix_put_itemDetail, by179 lb_mix_decide.
All village. tools/b_covered.py lists the functions in my range with no linked run (mostly online, plus lb_npc_item_trans, ef_move_sub_*, DispDialogData,
Display_StringData, tk_dialog_mv02, DispNameAndIDonDialog). Lesson: `if (lim <= ++lbShop.x6E)` with an s8 lim fixes the registers of the s8 `x6E++` compare
but the compare result still goes to `at`; `if (--lbShop.x6E < 0)` (pre-decrement inside the condition) removes the reload after the store.

# Lobby round 9 (agent B): real matches from the raw-linked village functions
Range now 0x533980-0x5AB000 (agent C took 0x5AB000 up; by159/167/168/175/176 sit there and were left as they were). Lobby 29.202% -> 29.605% (progress.py).
Real matches (C compiled to the original bytes, removed from c_rawfuncs.txt or new; all five modules OK):
- shop_armor_question (lb_by163): load `kind = lbShop.tbl[c*2]` THEN `id = lbShop.tbl[c*2+1]` with `c = lbShop.cur` first (pointer `e` form never gave the load order).
- lb_armor_put_itemDetail (lb_by162): declare/assign `e` (tbl entry) before `ud` (User_data row), and read `kind` before `id` in the mode 0 branch (the last load fills the branch delay slot).
- DispDialogData (lb_by180, new, 1312 bytes): header edits LB_DIALOG x06 s16->u16 (lhu proven) and yesno u8->s8 (lb proven). `flfntLocate` redeclared with s16 params
  (rename-the-header trick) so `y` is not re-extended; `Sel_csr_disp(x, (s16)(y-2), w, 0x18, 0xB0008000)` takes FIVE arguments; `nwDispStr_Html(100.0f, 60.0f, 1.0f, htmlStr)`
  passes floats in f12-f14 (m2c showed them as integers); `if (html != 1) {lines; switch} else {html}` layout, `do {...} while (*p)` text loop.
- DispNameAndIDonDialog (lb_by181, new): `s16 y` as ANSI parameter, callee `font_print_double(int, s16, int, int, char *)` / `Draw_square(int, s16, ...)` redeclared
  narrow so each call narrows its own argument; strings are the extern literals lit_226/227.
- Lb_on_dialog (lb_by182, new): a two-case `switch` on `step` gave the ladder in the wrong order; `if (step == 10) {...return;} if (step == 13) {...}` matched. x0A is read as u8 here (`(u8)n->x0A`).
Still near-matches (best C in the by file under `#else` or the nm file):
- shop_select_items (2 instructions) and lb_process_kyoukaListProg (2): `slt v0` vs `slt at` in the x6E+1 compare. `lim <= ++lbShop.x6E`, `lim > n` forms give `at` with the right registers; the `n < lim`
  forms give `slt v0` but swap the lim/n registers (9 instructions). Tried about 40 spellings (temps, casts, ternary, `!`, `== 0`, int result) and a 1000-iteration permuter run.
- lb_process_use_item (14): the original builds `shopList + 0x26` as an absolute constant (0x66DBC6) and adds `n*40`: `sll v1,a0,2; addu v1,v1,a0; ...; sll a1,v1,3; addu a0,a0,a1`; mine overwrites a0.
  An alias `shopList_26 = 0x66DBC6` in config/lobby_aliases.txt gives the lui/addiu form but not the register choice (not committed).
- lb_npc_old_guild (2): mv's live range has a hole in the original (a2 reused for the 105 compare constant); local init in case 0x64 only made it worse.
- lb_mix_decide (4), lb_mix_put_itemDetail (6), Lb_put_materialItem (17), value_result (the original keeps `v` in a0 and re-masks u16 after each op), ef_move_sub_0053E360 (17, switch value in a1): no change.
Remaining unwritten village functions in my range: plaza_* (online), ef_move_sub_0053E360 (has C), Lb_put_new_mail, plaza_capcomPage, put_member_info, Lbs_plaza_trans. Above 0x5AB000 (agent C now): lb_npc_item_trans, Display_StringData.

# Lobby round 10 (agent B): lobby tail 0x5EE618-end
Real matches: eft25_t (lb_ge2505.c, rodata 0x6686A0-0x668704) and tagAct_500-504 (lb_aq03.c). Lobby 29.605% -> see progress.py.
eft25_t lessons: call prototypes with float args must be real (`flmatMakeTrans(u8 *, f32, f32, f32)`; K&R promotes floats to double); one `mat` buffer (not mat+mrv),
`make_mat_srt(sc, rot, prim+8, mode & 0xFFFF, mat)` takes 5 args and rot is f32[3] with only rot[2] written; no `if (type < 9U)` wrapper (the switch range check is the only one);
`flSetRenderState(0x19, (int)tr)` flips the delay-slot fill; prototype `eft_trans_sub_col(int, u8 *, int, u16, int)` avoids a re-extension of the u16 opt; colour `(g&0xFF) | ((r&0xFF)<<8 | (alpha<<24 | (b&0xFF)<<16))`
(right-nested or); decl order x10, tex, opt, mode gave s6..s3; r,g,b declared b,r,g; eft_mdlw[0] (20-byte table) instead of a gp-addressed pointer; E25 f5/f6 are u8 (lbu) in lb_e25.c/lb_ge2505.c only.
tagAct_500: `buf[(*(s32 *)(bsw + 4))++] = 60;` fixed the register order.
eft25_m: `long k` made `(f32)k` call __floatdisf and a callee-saved f20; with `int k` and real prototypes (eft_vec_linear(f32, f32 *, f32 *) etc.) the frame matches (320) and about 316 align lines remain
(the original spills par and tbl and keeps &v[1], &v[2] in s5/s6; mine keeps par in s5). Hill-climbs over declaration order and prologue statement order only got 332 -> 316.
Still near: itembox_cursor_mv (2: daddiu for 9), sellout (34: w in a1 not a2; permuter 1 hour no gain), Disp_lb_item_box, pickup, Disp_TABLE_Line 2 (arg load order), BsBody00_ReqSrc (5, delay slot),
Plaza_chatlog_mv 5, lb_process_kyoukaListProg (lim extension lands in v0 not v1; separate int temp did not help).

## Lobby round 10b (agent B)
BsBody00_ReqSrc matched (lb_au07.c): the original is `if (hide) { switch (MMBB_LOGIN) { case 2: case 1: ...; break; default: break; } } else {...}` (no early returns; the switch exit jumps straight to the epilogue).
Owner paused browser work mid-round. Plaza_chatlog_mv still 5 (u8 PZ_TOP decrement lands in v0 not in place), itembox_cursor_mv still 2 (daddiu 9; tried int/u8/u16/long/ternary forms). tagAct_600/601 drafted in scratch only: bsw field reads must be `(bsw + bsw[0xE96C])[0xE96C]` with the first read not CSE'd; not finished.

# Main module 0x24A240-0x2814E0 (agent B, 7 Oct 2026)
Map (all functions in this range that were not yet in a linked run; E's sk13-18/20 and D's pl_snd01 are merged first):
- 0x24A240-0x2542E0 player sound script (pl_snd01, linked by D). 0x254300-0x25F980 PS2 kernel stubs, sceSif/Fs/Tty/Timer/Deci2 SDK: skipped (not Capcom).
- 0x25F980-0x262A90 soft keyboard (sk_*, DispSoftkeyboard): chat/name text entry, offline and online. 0x263140-0x2671E0 candidate table, hard keyboard (hk_*), kbd_*, cmd_*.
- 0x2671E0-0x26D2F0 online: ms_network_bb_*, net file, net_connect_draw. 0x26D310 disp_spr_sub (11.9 KB, network connection screen sprites), 0x2703F0-0x271050 ncm message drawing,
  0x2712C0 server_connect / connect_error: online, last.
- 0x271EA0-0x273B50 user data (Set_userdata, Ud_item_stack, Set_mini_data_to_pl, Get_bowgun_atk): single player. 0x274EC0-0x27BF80 pit menu: list/page select, item valid check,
  frame/list/message drawing, chat log, NPC messages, player status / equipment windows (single player; chat log is shared with online).
- 0x27C020-0x27CA10 Softkey app glue and Sony base64/scf/rtc library (skipped). 0x27CF10-0x27D4A0 cnLBS file download (online). 0x27F1C0-0x2814E0 memory card (mc*), save selection UI.
Linked this round (all five modules OK): ListSelect, PageSelect, Item_valid_chk (pit04/05); chat24-43: Chat_init, ChatKinsoku_chk, Menu_chatlog_mv, Put_receive_mark, Join_pl_chk, Put_PageArrow,
disp_cursorC, Disp_help_mess, NPC_Message, PutSpriteDiv3, Put_mini_sight, Disp_NPC_message, Get_equip_icon_uv, PutButtonICON, ChatLogAdd_Q, Pit_disp_chat_log, EquipmentDescriptionWindowA,
EquipmentCompareWindowA, Monster_list_search, Chat_move; ud13 Set_mini_data_to_pl; hk19 cmd_delete; sk21/23/24 disp_keybase, sk_board_ptr_replace, key_mask_check.
Header edit: include/menu.h PitMenu.x0C s16 -> u16 (Chat_init/Menu_chatlog_mv store the chained `x0F = x0C = 0` through andi 0xFFFF). Aliases added to config/main_aliases.txt:
flfntLocate_i, Equip_moji_color_rare_i, Put_PageArrow_s, EquipmentDescriptionWindowA_s (same function, a second prototype so one TU can call it narrowed or raw; tools/mkruns3.py strips `_i`/`_s` when comparing call names).
Matching lessons (each confirmed by a match):
- Compare ladder of beq with a trailing `beq; nop; b` = a `switch` with `break` (yn_mask_char_check, mh_char_make_check); `r = 0` before the switch.
- A shared `li v0,1` return label at the end of a function = `switch` whose cases `break` and a final `return 1` (Item_valid_chk); the early exits `return 0`.
- Stray extra argument in a call (`se_req(7, 0x16, 0, v)`) put the variable in t0 instead of a3 (ListSelect/PageSelect). A call with FEWER args than the callee has: leave the arguments out
  (sk_backspace(1) etc. in SoftKeyboard_move), declare the callee `void f();`.
- `(u8)n` at every use (not `n & 0xFF`) keeps the mask un-CSE'd (Put_receive_mark, Disp_help_mess: `(u8)kind`, `(u16)id`).
- Parameters of the callee decide narrowing at the call: Put_PageArrow / EquipmentDescriptionWindowA take raw ints in their own definition but their callers narrow (two prototypes via alias).
  `u8` as last parameter of Put_PageArrow and return type u8 of EquipmentDescriptionWindowA removed the masks around `pages`.
- A float parameter shows as `mov.s $f20,$f12` in the prologue: Put_mini_sight(f32 scale, s16 ofs, int col). Hoisted `x0 = 1.25f * scale` goes after the last call before the loop.
- `int t = u1 - d` (int temp) instead of s16 saves the second sign extension (PutSpriteDiv3). Chained store `a = b = 0` stores the right-hand variable first.
- Struct with u8/u16 views of the same bytes (`KM`: `k->v.w`, `k->v.s.lo`) stops MWCC from forming a `k+2` pointer register (key_mask_check, EQD in EquipmentDescriptionWindowA).
- Per-branch stores `*(u8 **)lpSKey = X` in each case (not one temp `b` stored after the switch) (sk_board_ptr_replace). Reading `lpSKey->field` through a local pointer lets MWCC keep the pointer;
  reading through the global every time reloads it like the original (setup_rw_sub, disp_keybase: no `sc` temp). int instead of u16 for a position variable kills `andi` copies (cmd_delete).
  Declaration order is reverse of register order (cmd_delete: `p, s, n, pos` gave s0 = pos).
- A compare `if (0 < x)` / `x > left` vs `left < x` flips the `slt` result register (NPC_Message `PitMenu.x08 > left`). Both-branch values assigned once: `if (c == 0xA) { s += 1; } else {...}` order.
- Colour built from three sines: `(R | 0xFF000000 | G) | B` with each channel `(((s8)(K * s) + C) & 0xFF) << n` (EquipmentCompareWindowA); tried 7 orders with a script, four match.
- Shift/sign helpers: `u8 pg = page` extra copy lets `page &= 3` stay in its register; locals of struct/array size decide the frame (buf[0x40] vs [0x20] in Chat_move/Disp_NPC_message/ItemListWindow).
- Permuter (-j1, 7 min per function) found zero scores for Monster_list_search and Chat_move (applied with tools/permapply.py; formatting of those two functions is the permuter's).
Near-matches left (real remaining difference in instructions, C in the *_nm.c files): PlayerEquipmentWindow 2 (init store order), server_connect 2, connect_error 4, net_overlay_request 4,
hk_key_kata_hira 5 (empty then-block layout), sk_pltchange 5 (saved register order f/e), Chat_log_add / Plaza_chat_log_add 6 (a0 in the jal delay slot), HardKeyboard_move 7, hk_key_backspace 8,
disp_keybase2 9, sk_key_repeat 11 (return type is s16), sk_palette_cursor_set 11, hk_key_end 13, hk_cursor_mv 15, setup_rw_sub 29, Get_bowgun_atk 27, ItemListWindow 62,
ng_word_sub 67 (8 saved registers vs 7), PlayerStatusWindow 88 (pl/tab/t/noRank register order), SoftKeyboard_move 38 (callee prototypes now K&R; layout of the timer decrement block), equip_exp_core 965,
DispFrameMessageA 606, DispFrameListA 348. mc_act_unformat (11) needs mc_unformat/mc_act_return in the SAME translation unit (the original keeps a0 across both calls: MWCC register info of an earlier callee).
Not started: online code (disp_spr_sub, net_connect_draw, ms_network_*, ncm_*), mc disp/low, Ud_item_stack / Ud_u_item_stack.

# Main module round 2 (agent B, 7 Oct 2026): whole files, single player first
Method (the lesson from agent E, applied to f_chat, mc, hk, ud): one source file = one translation unit, address order, file-static
functions `static` (the symbol table marks them LOCAL), matched bodies and near-match C side by side, unmatched functions as raw `asm`
stubs (config/c_rawfuncs.txt) so the file still links. Helpers in tools/b_tu/ (subst2.py puts the matched bodies into the nm file,
mergetu.py merges several nm files, mkstatic.py, gen2.py writes src + c_files/c_rawfuncs lines, trywith.py / hunks.sh / stperm.py
try a rewrite and count real differences, pq.sh runs the permuter on one function of a scratch TU) and tools/b_od.py (original
disassembly of one function from disc/, with callee names). B_SCRATCH (default build/b_scratch) holds the scratch files. The all-C versions of the four TUs (the C
for the raw-stubbed functions, best known state) are kept in src/main/tu/*_all.c (not built; check.py compiles them).
Registered TUs (all five modules OK):
- chat/f_chat 0x2755D0-0x27BF80 (pit-menu windows, chat log, reibun): replaces chat01-44, chatb01, chatc01. Raw stubs: DispFrameMessageA,
  disp_chat_log_sub, ItemListWindow, PlayerStatusWindow, equip_exp_core, slash_level_bar, ng_word_sub.
- mc/f_mc 0x27EF60-0x280EF0 (card access + act layer): replaces mclow*/mcact*. Raw stub: McActAvailSet.
- hk/f_hk 0x264180-0x267198 (hardware keyboard). Raw stubs: HardKeyboard_move, hk_kbd_input, hk_kbd_input_sub, hk_cursor_mv,
  hk_key_r_cursor, hk_key_kata_hira, roma_ck_sub, kbd_disp_input.
- ud/f_ud 0x271FB0-0x274E10 (user data): ONE rodata slot 0x3734F0-0x3735D0 (the three old slots were one object). Raw stubs: Set_userdata,
  Ud_item_stack, Ud_u_item_stack, Get_bowgun_atk, Gun_level_up, Gun_option_ck.
New real matches (rebuild OK): Chat_log_add, Plaza_chat_log_add, chat_log_add, sword_zokusei, DispFrameListA, mc_check_card, mc_delete_dir,
mc_act_unformat, hk_key_backspace, hk_key_end, kbd_insert.
Link rules learned:
- Statics that the original DATA refers to by name (mc_act_jmp table, hk key tables) need an absolute alias in config/main_aliases.txt
  (C static + alias of the same name is fine). A static function that stays a raw stub but is called by a matched function changes how the
  caller compiles: make that stub non-static and drop its alias (roma_ck_sub / hk_roma_ck lost 24 bytes otherwise; link then fails on
  `small-data section too large`, which is just a shifted .sdata).
- A file with rodata (jump tables, short strings) can only be one TU when ALL its rodata comes from C: sk/f_sk would also need the tables and
  strings that belong to still-raw functions (0x36E580-0x36E5F8, plus lit_628_0036E5C0 "\x81\x40"), so sk was NOT converted.
- build/raw/NAME.inc is opened through wibo, which is case-insensitive: chat_log_add and Chat_log_add collided (the stub for the static one is
  called chat_log_add_277D30 in c_rawfuncs.txt). K&R parameter lists on an `asm` stub do not compile: write the ANSI header.
- tools/align.py ignores relocations completely, check.py checks call names but not addends, rebuild.sh is the only judge.
Matching lessons (function that shows it):
- Typed struct for a frame/window descriptor (FRL: x,y,colw,h,cols,rows,pal,mode,list,col; DispFrameListA) instead of F8()/F16() byte
  macros: the macros make MWCC CSE `fr+4`, `fr+5` address constants and spill them (frame 400 vs 320). A `switch (mode & 3)` with default
  first, then case 1, case 2 reproduces the `beq; nop` ladder; `u8` locals for the uv constants (daddiu + andi); `(alpha & 0xFF) << 24`;
  `for (j = rows; j > 0; j--) { ...; tl++; py += h; }` (increment as a statement inside) fixed the last scheduling hunk.
- Labels inside nested blocks reproduce original block order: mc_check_card (`again:`/`retry:` live inside `case -2: if (type == 2) {...}`,
  `done:` after the switch), mc_delete_dir (`err:` inside the then-block of the GetDir test, `rm_self:`/`rm:` after the r==1 block of case 2,
  `if (xA0 == 0) goto next; return 0;`). sceMcGetDir's 4th parameter `unsigned` flips the load order of cnt/port.
- if/else instead of `x = 0x16; if (c) x = 0x1E;` removes the extra nop before the loop (chat_log_add). `u8 *p` taken from the
  global (`new_var = (char *)lpSKey + 0x158;`) kept apart from the later reads: hk_key_backspace and hk_key_end (found by the permuter in
  7 minutes each; hand variants never produced it).
- int, not u16, for parameters whose original has no andi (kbd_insert pos/max), `/ 2` not `>> 1` for the bgez fix-up, one `(int)strlen()` for
  a signed compare; call with 5 arguments where the original loads t0 (font_print_double2(…, buf)); `buf[0x20]` not [0x40] from the frame size.
- `slash_level_bar(u8 *d, s16 y, f32 x)`: m2c's `(u8 *)(s32)(4.0f + ...)` first argument is really the float x in f12 (EABI passes it
  independently of a0/a1); the two flps0009 calls draw a triangle (6 x s16 + colour at +12).
Near-matches left (hunks = real differences after tools/align.py, best C in src/main/tu/{chat,mc,hk,ud}_all.c):
DispFrameMessageA 135 (struct FRM written; y0 and lh swap register vs spill, 9 spill slots in the original), equip_exp_core 183,
PlayerStatusWindow 45 (second `andi` of the tab index found: `[(u8)tab]`; saved-register order of pl/tab/noRank still differs),
ItemListWindow 20-31, slash_level_bar 55, ng_word_sub 28 (8 saved registers vs 7: the sign extension of `mode` lands in a new register),
disp_chat_log_sub 5 (else-block c/l registers), Set_userdata 8 (the original unrolls the name copy 6x, mine strength-reduces),
Ud_u_item_stack 15, Ud_item_stack 35, Get_bowgun_atk 7, Gun_level_up 3 / Gun_option_ck 3 (then-block out of line + `slti v1` +
`daddiu` in the delay slot: same layout class as HardKeyboard_move and hk_key_kata_hira, 2 hunks each; never reproduced),
hk_cursor_mv 4, hk_key_r_cursor 6, McActAvailSet 9, roma_ck_sub 31, hk_kbd_input*/kbd_disp_input untouched.
sk (not a TU, see above) in its own scratch TU: sk_zen_han_chg matches only with the statics around it; sk_pltchange 3, sk_backspace 3, setup_rw_sub 5,
setup_rw_moji 5, Han2zen 4, disp_keybase2 4, sk_palette_cursor_set 6, sk_key_repeat 6.
Not started: online code (server_connect, net_overlay_request, AnswerFileDownloadHeader, disp_spr_sub, ncm_*).

# Main module round 3 (agent B, 7 Oct 2026): sk stubs, chat near-matches
New real matches (rebuild OK, all five modules): setup_rw_moji, sk_palette_cursor_set, sk_backspace (sk/f_sk raw stubs replaced by C).
Lessons (function that shows it):
- Long runs of `a.x = b.x = ...` that look like m2c copies are chained STRUCT assignments (setup_rw_moji): `RWP(t1) = RWP(t2) = ... = *f;` with
  `typedef struct { s16 a, b; } RW2` makes MWCC store, reload from the last store and store again (the original "sh; lh; sh" pattern), the leftmost
  target is stored last. All address constants (lui/addiu) then get hoisted with the 24-register spill pattern. A `char *k = (char *)lpSKey;` local
  (type char *, not u8 * / void * / s32: the pointer type changes sq vs sw for its spill slot, only char * gave sq) and
  `(u8 *)&free_rw_tbl[0][2] + idx * 0xC` (symbol+8 kept as a separate constant, not folded into lh) fixed the last hunks.
- `SKB(0x24) = palette_set_tbl[(u8)f * 2]` (explicit u8 cast on a u8 local) reproduces the redundant `andi` (sk_palette_cursor_set).
- A callee prototype with a u16 parameter makes the caller emit `andi 0xFFFF` on the argument; the original had none, so declare the parameter
  `int` in the TU's own prototype (sk_letlenB(void *, int)); int, not u16, for the local n; `x = x - len` instead of `x -= len` (sk_backspace).
- Declaration order picks saved registers: `int len; u8 *s; int n;` gave the original s0/s1/s2 assignment in sk_backspace. tools: build/b_scratch/dperm.py
  style random shuffles of the declaration block (about 3 s per try under load) found 31 -> 20 for ItemListWindow; try this before the permuter.
- PlayerStatusWindow 45 -> 33: the casts `(u8)pt`, `(u8)FS16(..)`, `(u8)(s16)f32` were wrong (original loads lw/lh/lhu and uses dsll32+dsra32 for the
  s16 conversion); the skill loop is `do { q = pl + i; if (F8(q,0x910) == 0) break; ...; i++; } while (i < 5)` with `u32 i`. Remaining diff is the
  saved-register assignment (pl=s0, t=s1, tab=s2, noRank=s3 in the original).
- Pattern seen in server_connect and net_overlay_request: the compare-ladder constants of a switch land in a0 (original) instead of v0 (mine); no
  source variation tried (ret init, local copy of the switch value, extra parameter, (int) cast) changed it.
- align.py prints nothing, so scripts counted 0 hunks, when the scratch file does not compile (C89: a statement before declarations). Always
  confirm a 0 with `tools/check.py FILE -v | grep NAME`.
- tools/b_tu/hunks.sh breaks on paths with spaces (quote $S); cc1.sh has a stray `$` in the default output name; set B_SCRATCH to an absolute path
  and call tools/align.py directly.
Near-matches now: sk_key_repeat 3 (original keeps the masked hold value in a1: the two copies `h` live in different registers in mine), disp_keybase2 4,
setup_rw_sub 5 (the original hoists all five table address constants to the top), sk_pltchange 3 (f/e saved-register swap, decl shuffles of all 6
lines tried), Han2zen 4 (`c = *src++` temp register layout), dakuten_ck 5 (the original does `t += 2` in the compare's delay slot and reads t[0]
after it), ng_word_sub 28 (mode is sign-extended in place in s0 in the original), ItemListWindow 20, PlayerStatusWindow 33.
