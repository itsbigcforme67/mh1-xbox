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
