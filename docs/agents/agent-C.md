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
- Monster init files em02_init.c, em04_init.c, em09_init.c (+ Em09_item_sub),
  em18_init.c, em19_init.c (+ em19_act_set): all match. Common pattern:
  quest 0 places the monster by spawn slot em->x13 (stage 15: 3-bit grid
  around (9500, 9200); otherwise a fixed spot or stage_start_pos[stage]),
  then em_char_set(em, 1), x388 = 0, em_act_set(em, 0, 1), hit points via
  em_hp_vital_set, and the work block's home position. New fields: EMW
  x13, x1B, x40C/x40E, x56A, x734, x765, x7EE, x88B, x9E1; GAME_W x218
  (carried-over hit points, em02). quest_w is declared file-locally
  (QUEST_W {s16 no at +8}), as in tutorial.c.
- Lessons: `(int)((u32)em->x13 >> 3)` for the original's srl (a u8 >> 3
  is an int shift, sra); `((f32 *)stage_start_pos)[stage * 3]` for the
  x term matched where `stage_start_pos[stage][0]` added an andi.
- g_Em_Master_Change (0x5395F0-0x53BA4C, 48 functions): 45 match, built as
  em_master.c (0x539AC0-0x539C90), em_master_b.c (0x539D00-0x53B6D0),
  em_master_c.c (0x53B8A0-0x53BA4C); whole file in em_master_nm.c.
  Near-matches: Em_Master_Change (network master hand-over; logic written,
  register allocation far off, a permuter run did not help), Em_Taisei_Set
  (23 off: the original loads all four table pointers before storing),
  em_hagitori_lv_up (19 off, register choice).
- g_Em_Taisei_Damage_Check (0x559260-0x55B054, 14 functions): 10 written,
  9 built as em_taisei.c (em_eye_dmg_reset_act_set) and em_taisei_b.c
  (stock/timer functions, Em_Damage_Stock); em_taisei_nm.c holds the file.
  Em_Taisei_Ck is 2 off (two saved registers swapped), Em_Taisei_Damage_Check
  10 off; em_eye_dmg_act_set (per-monster reaction to eye damage, 0xA10)
  and Em_Dmg_Sys (0x7A4) are not written yet.
- New shared header include/em_sys.h (EM_TAISEI_DATA, EM_SMELL, status
  tables). Many EMW fields added through a carve script (gen/carve.py in my
  scratchpad, not committed): mostly xNNN names for flags and counters used
  by this code; named ones: boss (0x9D4), taisei (0x7D3 status bits),
  *_tol tolerances, hungry/thirst (+max), dmg[8] (0x766), hagi[8][8]
  (0x308). GAME_W: pl_num (0xD3), pl_state[4] (0x208).
- Lessons:
  - `x = x + n` with an int n leaves n alone; `x += n` on an s16 field
    sign-extends n first (*_stock_set).
  - Early `return` inside an if-body gives a `b epilogue` stub; the outer
    test written as a nested if branches straight to the end
    (em_no_floor_ck).
  - `pl = &player_work[i];` inside the loop body (not a walking pointer in
    the for header) for player loops (em_no_battle_area_ck).
  - A struct table pointer used once at the end is still loaded at the top:
    declare it as an initialised local (`EM_TAISEI_DATA *d = tbl[kind];`).
  - game_w+0x1E is read as a u16 frame counter here; the existing u8 x1E
    field (eft12) was left alone and read through `*(u16 *)&game_w.x1E`.

## f_em (em_core_nm.c) - paused here
- src/game/em/em_core_nm.c holds the whole f_em file (0x533A00-0x5395F0),
  not registered yet. 52 of 53 functions written, 43 match (incl.
  cmd_target_kind_set and target_kind_set, 2464/2288 bytes, jump tables
  lit_1844/1845/1846_00685110, lit_2001-2003; em_type_act_set uses
  lit_1223_00685080).
- Near-matches: em_eye_search_set 9 off, senko_ck 32, smell_ck 57,
  em_act_search 13, Em_Hate_Add 1, em_range_set 38, em_char_set 206,
  em_hate_suu_set (u8 params kept raw, needs conflicting prototypes),
  neck_ang_set just written (350 off, first draft).
- Not written: em_neck_move_sub (0x534730, 1952 bytes; m2c draft via
  `tools/draft.py game em_neck_move_sub`; fields neck_tgt/neck_ang/neck[4]/
  neck_spd/neck_lock/neck_st and EM_NECK are already in place).
- Next: write em_neck_move_sub, then split em_core_nm.c into matching runs
  (em_core.c, em_core_b.c ...), register in config/c_files.txt (plus the
  rodata jump tables), rebuild, commit. Then em_eye_dmg_act_set and
  Em_Dmg_Sys in em_taisei_nm.c.
- Lessons from this file: `switch (flag) { case 0: ... }` reproduces the
  beqz/b-stub shape of `if (flag == 0)` tests (em_hungry_ck, case 1 of the
  target functions); u16 action params must stay u16 down the chain
  (em_act_set -> em_act_set_sub -> act_set) or andi masks appear; a cross-
  case fallback is a `goto` to a label inside case 0 of the inner switch;
  `- -b` gave the add.s operand order in xang_calc_*; `*nest++ == stg` in the
  nest-stage loops; block-scoped VEC/FLMAT locals per case set the frame.

## f_em split (third session)
- em_core_nm.c (whole f_em) is now split into linked runs, game OK:
  em_core.c, em_core_b..g.c (0x534100-0x53948C; rodata 0x685080-0x68510C in
  em_core_d, 0x685110-0x685224 in em_core_g). 43 functions linked.
- Still near-match only (in em_core_nm.c): em_eye_search_set (9 off), senko_ck
  (32), em_char_set (206), neck_ang_set (350, first draft), smell_ck (57),
  em_act_search (13), Em_Hate_Add (1), em_range_set (38), em_hate_suu_set.
  em_neck_move_sub (0x534730) not written yet.
- The split files were generated by a script: whole nm file with the
  non-run function bodies replaced by prototypes.

## f_em wrap-up (fourth session, worker C on Sonnet)
- em_neck_move_sub written in em_core_nm.c (near-match; register allocation of
  the saved vars differs, structure follows the asm: neck_st 0..3 state machine).
  Em_Hate_Add now matches: `h = (s32 *)em->x918 + pl;` (the `&em->x918[n]` form
  swaps the addu operands) and is linked (em_core_f.c starts at 0x536440).
  em_act_search 9 off (loop temps), em_eye_search_set 9 off (scheduling only).

## f_menu (main.bin 0x127440-0x134950, "pit menu") - src/main/menu/
- Whole file as near-match C in src/main/menu/menu_nm.c (not built); linked
  runs are menuNN.c, made from it with tools/mkrun.py (copies the preamble +
  named functions). include/menu.h has PIT_W (lpPit, 0x90 bytes) and PIT_MENU
  (PitMenu); field names are guesses, offsets are exact. Unknown parts of
  game_w/option_w/quest_w are reached with the FLD macros of include/flow.h.
- Linked (main OK, all five modules OK): menu01..menu18.c, 40 functions: select_yes_no..
  player_name_id_print, PitWork_init, menu_init/menu_move/menu_retire_i (+ jump table
  0x35A330), juchu_chk..Menu_item_i, mix_item_chk..Menu_mix_i, Menu_data_i, Menu_status_*,
  Menu_equipment_*, menu_option_i, ItemPickingDeclaration, ItemStockRequest,
  lb_item_stock_mv, map_move, MapSignRequest, Menu_chatcnfg_i, boss_icon_color,
  wyvern_area, DispWholeMap, WyvernAreaMove, Pit_disp_menu_status, disp_retire,
  trans_pit_0/1/2/1_lb/2_lb, Item_box_get_efct/_item.
- Near-match in menu_nm.c (written, not linked): Pit_init, Pit_reset (andi of zero
  reg not reproduced), pit_key_repeat (26 off), menu_retire_mv (4), Menu_item_mv (2,
  probably only relocs), Menu_data_mv (12), menu_data_mix_sub (4), menu_data_monster_sub,
  menu_equip_get_equip (7), menu_option_mv (37), item_stock_mv (204, register
  allocation), map_sign_move (4), Menu_chatcnfg_mv, menu_chcnfg_sendpl/reibun,
  maru_disp_sub, camp_disp_sub, Pit_mv_lb (14), Pit_mv (31), Menu_mix_mv (479, saved
  register order only).
- Also written as near-match: efct_circle (48 off), disp_map_sign (OK, not linked yet), enemy_on_map (119/229, stack/register order).
- Not written yet (all display code): Pit_disp_*, disp_* (map, item, vital, gauges,
  chat), font_print_quest_*, quest_condition_print, efct_circle, enemy_on_map,
  player_on_map, disp_map_sign, disp_whole_map/partial_map/map, put_mix_material,
  trans_box, Pit_effect_move, mix_effect_set, pef_get_*. They use string literals and
  float tables, so linking them also needs main:rodata ranges.
- Linking pitfalls found: a function with a `switch` jump table needs its
  `main:rodata` range in c_files.txt (menu03); an `if/else if/else` chain that
  `return`s in each branch vs one `r = ...; return r;` changes where the epilogue
  label sits (boss_icon_color: use a result variable); a struct field of the wrong
  signedness only shows as a one-instruction diff (PitMenu.x10 lb vs lbu in
  trans_pit_1) that check.py hides because it ignores relocations - always
  rebuild after adding a run. Fields accessed twice through FLD8(game_w, off)
  get CSE'd into one pointer; give the field a real name (game.h x1E7) instead.
- check.py shows `--` for calls to functions whose symbol it cannot resolve
  (func_5BD520 etc.); those are not real differences.
- Lessons (function that shows it):
  * `return x ? 1 : 0;` after an early `return 0;` gives the extra nop/branch
    shape of a bool return (Cockpit_menu_chk, mix_item_chk).
  * `switch (r) { case 0: case 1: ...; case 2: case 3: ... case 5: }` - the
    order of stacked case labels changes the compare chain (ItemStockRequest:
    `case 0: case 1:` matches, `case 1: case 0:` does not).
  * An s16 field returned/assigned as `return lpPit->key = 0;` reproduces the
    dsll32/dsra32/andi tail (pit_key_repeat, not matched yet).
  * `PitMenu.x10 = (u16)0`-style andi of a zero register in Pit_init/Pit_reset
    not reproduced (those two stay near-match).
  * Pointer + index order of an addu (`(s32 *)em->x918 + pl` vs `&x[n]`).
  * `max = cond == 0 ? 3 : 7;` picks movn vs movz (Menu_item_mv).
- Near-match status: see menu_nm.c per function (check.py).


## f_menu display half (fifth session, worker C on Sonnet)
- All 87 functions of f_menu (main 0x127440-0x134950) now have C. The display half
  (map, item, vital, chat, quest text, effects: ~60 functions) is in
  src/main/menu/menu_disp_nm.c (near-match, not built); the first half stays in menu_nm.c.
  Runs are made from them with `tools/mkrun.py` (menu_nm.c) or the new `tools/mkrun2.py`
  (keeps declarations/#defines that sit between functions, needed for menu_disp_nm.c).
  A run file must also be checked with `tools/check.py` on its own: helpers defined earlier
  in the same TU get inlined by MWCC, so a run that only has a prototype can differ.
- Linked this session (all five modules OK): menu19 disp_map_sign; menu20 font_print_quest_
  money/lv/target/Bdragon/BBQquest; menu21 Pit_disp_item_mix + put_mix_material; menu22
  wyvn_efct_ripple; menu05 extended to include Menu_item_mv; menu24 menu_retire_mv; menu25
  map_sign_move; menu26 menu_equip_get_equip (+ jump table 0x35A3A0); menu27 Menu_data_mv;
  menu28 pit_key_repeat; menu29 disp_item_sub_normal; menu30 disp_item_sub_select_ex; menu31
  disp_item_stock; menu32 disp_monster_list; menu33 Pit_disp_data; menu34 Pit_effect_move;
  menu35 disp_mix_list; menu36 pef_get_scale.
- Near-match, how far off (check.py counts; many "reloc" lines are not real):
  disp_needle 4 (float reg order of one constant), Pit_disp_chat_cnfg 3, Pit_mv 4 / Pit_mv_lb 3
  (needs `int pit_key_repeat(u16,u16)` prototype and decl order sw,hold,now; remaining diff is
  `lhu a0` + move-to-saved-reg order), menu_chcnfg_reibun 3 (s0 copy of sw kept by the original),
  menu_data_monster_sub 5, menu_data_mix_sub 4, maru_disp_sub 11 (store order of the struct),
  camp_disp_sub (float vs constant folding of 12.8f/16.0f: original does not fold),
  font_print_quest_time 29 (original does /30 then /60 as two divides, MWCC fuses mine),
  disp_map 41, disp_others_info 25, Pit_disp_item_list 85, player_on_map 92, trans_box,
  disp_item_sub_select (968 insns, jump table 0x35A5B0; structure transcribed from the asm),
  disp_whole_map/disp_partial_map (one extra saved register), player_info_sub, mix_effect_set,
  Pit_disp_pit_effect (madd.s chains are plain `a*b + c*d` in C), gage_disp, bar_disp (66),
  disp_timer, disp_pl_vital, disp_pachinger, disp_cannon, disp_item, disp_item_icon, disp_name.
  Written from the asm but only checked for compiling: lb_disp_chat_cnfg_sendpl, disp_menu,
  disp_option, Pit_disp_menu_equipment, pef_get_scale/alpha, disp_slash_level, disp_gun_load_mess.
- Shared-header / tool edits: include/game.h (GAME_W x21D carved from pad), include/menu.h
  (PIT_W x85 s8, x86 s16 carved from pad), tools/draft.py (DRAFT_CTX=file passes m2c
  --context so float/int args are typed), tools/setup_split.py (`$` in static data names such as
  btn_item_sel$3617 is now `_`, so C can name them; config/symbols/*.txt regenerated).
- Lessons (function that shows it):
  * `x >= N` compiles `slti v1,...`; `x > N-1` compiles `slti at,...` (map_sign_move,
    disp_item_stock, disp_monster_list, disp_mix_list): use the form the original has.
  * Repeated float subexpressions: write them inline, not via temporaries (pef_get_scale matched
    only with `(f32)(t - ta)` spelled out twice; `-(f32)x` vs `(f32)(-x)` also matters).
  * A `return` in each switch case vs `break` changes code layout (Pit_disp_data: breaks).
  * `if (a != X) { return value; } return 0;` order vs `if (a == X) return 0; return value;`
    changes where the delay-slot addiu lands; `u + idx*6 + 0x44` vs `&u[0x44 + idx*6]`
    (menu_equip_get_equip).
  * One `r` result variable assigned in each branch gives the shared epilogue that the
    original has when `return 0` sits in a branch delay slot (pit_key_repeat).
  * `(u16)(sw & 0xFFBF)` keeps the second andi; `sw & 0xFFBF & 0xFFFF` merges to one (Menu_data_mv).
  * Compute a masked copy of the argument inside the branch that uses it (not at the top) to
    avoid it being hoisted into the first delay slot (menu_data_monster_sub).
  * Typed u8/s16 field widths show as lbu/lhu in single instructions: if the original has lhu,
    do not cast to (u8) (disp_item_sub_normal); a `u32 k` loop counter gives bne instead of bgtz.
  * Struct with two views of the same words (PFLPS2 with u16 uv[4]) instead of casting
    `((s16 *)&q.uv0)[1]` avoids extra pointer registers (disp_monster_list).
  * A function parameter may be passed through untouched (a0) when the callee takes two
    args (Reibun_select_mv(sw, idx)); check prototype arity from the asm.
  * MWCC emits `madd.s/msub.s/adda.s/mula.s` for `a*b + c*d`; flSinCos(ang, &sin, &cos) with
    sin at the higher stack address (declare `f32 s, c;` in that order).


## Sixth session (worker C on Sonnet): f_ud, f_chat, f_sk, f_hk (main.bin) written in C
- Every function of the four files has C now (breadth first, per the 5 Oct policy):
  src/main/ud/ud_nm.c (f_ud 0x1723F0-0x174E10, 48 funcs), src/main/chat/chat_nm.c (f_chat 0x1755D0-0x17BF80,
  70 funcs), src/main/sk/sk_nm.c (f_sk 0x15FA90-0x162A90, 46), src/main/hk/hk_nm.c (f_hk 0x164180-0x167198, 46).
  None of this C is Capcom bytes. Matching runs are split out with `tools/mkruns.py NM.c OUTDIR PREFIX FIRST "comment"`
  (new: groups address-contiguous fully matching functions, one file per run via mkrun2.py, prints the
  c_files.txt lines; check each run file with check.py on its own before registering).
  Linked, all five modules OK: ud01-ud08, chat*, sk*, hk* (config/c_files.txt; the run numbers shift whenever more
  functions match: regenerate with mkruns.py and verify each run file with check.py before registering; a run whose
  static helper is not part of it fails, e.g. Init_reibun alone).
- Status (check.py, fully matching / near-match): ud 37/11, chat 31/38, sk 20/26, hk 21/25 (functions linked are the
  address-contiguous matching runs: 47 c_files.txt lines for ud/chat/sk/hk; the ud Gun_* group, 9 matching functions, is
  parked behind gun_check, see below). Many of the "near" ones
  are only a register swap or a delay slot; most of the rest are float-heavy UI code (see below).
- New shared header include/ud.h (UDW: User_data layout: point 0x1C, evflag 0x24, ware[64] 0x44, stock[100] 0x1C4,
  qclear[8] 0x354, rank 0x37B, item[20] 0x37C, wkind 0x3CD, wid 0x3CE, wopt 0x3D0, armor[5] 0x3D2, widx[6] 0x456,
  wyv_kill 0x45C). include/menu.h edited (pad carve only): PIT_CHAT (chat log entry, 0x5D bytes) and PitMenu fields
  x04, x06/x07, x0E, x19, logtop 0x1E, lognum 0x1F, logscr 0x20, x21, x22, log[64] at 0x23.
- tools/symsz.sh NAME: prints a symbol's address and size from config/symbols (sdata items <= 8 bytes are gp-relative:
  declare them with their size, e.g. `extern u8 btn_menu_sub[8];`, or the compiler uses lui/addiu).
- Lessons (function that shows it):
  * A static function defined EARLIER in the same file keeps its caller's argument registers alive (the callee's clobber
    set is known): Set_equip_idx/Gun_* use a0/a1 after calling gun_check/equip_idx_ck/Equip_idx_renew. They must be
    `static` and in the same run as their callers, or the call costs the saved registers. Consequence: a run with such
    a static helper cannot be linked unless EVERY function between the helper and its callers matches (gun_check
    group is parked because Gun_level_up and Gun_option_ck are 9 and 15 instructions off).
  * An unused static is dropped by the compiler: a run holding only the static helper fails the link.
  * `UDW *u = User_data;` as a local (global declared `extern UDW User_data[];`) gives the original's single `lui/addiu`
    base register across loops (Ud_item_num_ck); declaration order of `u` and the index decides which of a1/a2 is which
    (tools/declbf.py found them in seconds).
  * `u16 ret = 0; ... ret = 5;` makes constants load with `daddiu` instead of `addiu` (Hunter_point_add_sub); a long long
    local does too but adds more. Return `(u8)ret`.
  * `if (x) { ...; break; } return 0;` inside a switch case, with ONE shared `return 1` after the switch, matched
    Equip_ok_ck where `if (m == x) return 1; return 0;` per case became xor/sltiu. A `goto` to a shared `return 1`
    fixed equip_idx_ck the same way.
  * `1LL << (n % 32)` on a u32 array gives the original's lwu/dsllv (Quest_clear_bit_ck/set).
  * Struct-offset array tables: `((GE *)&Gun_data[0][8])[id].v` (a typedef'd view starting at the field) folds +8 into the
    symbol like the original; `Gun_data[id][8]` keeps the displacement (Get_equip_value).
  * Registers named with a `.sdata` extern of size > 8 are not gp-relative; Psw/PitMenu need a typed global (struct with
    the real fields) for the original's per-field `lui at; lhu lo(at)` (chat_sw_set matched only that way).
  * m2c output of small functions can be wrong about argument passing (it reads `$a0..` that were never set as
    arguments): check the asm prologue before trusting a prototype. For jump-table functions m2c can be fed a temp asm
    with the table renamed `jtbl_...` and appended as `.rodata` (see how equip_exp_core was drafted; recipe in
    /tmp notes: rename the lit_NNN symbol used by the `lui/addiu/jr` sequence, append `glabel jtbl_lit_NNN` with its
    `.word .L...` entries).
  * After a run is linked the remaining asm of that file is re-split into new files (e.g. asm/main/text/
    EquipmentDescriptionWindow.s holds the rest of f_chat): use `grep -rn "glabel NAME" asm/main/text`.
- Near-match list with how far off (see per-function check.py output; counts are instructions that differ):
  ud: Ud_item_stack 150, Ud_u_item_stack 75, Ud_item_num_ck2 19 / ck3 15 (u16 id param, decl order),
  Get_bowgun_atk 25 (id*0x14 scheduled earlier), Now_equip_ck (two extra nops in the original), Seisan_ok_ck 50,
  Set_mini_data_to_pl 17 (s1/s0 swap), Copy_user_id 8 (sym+0x1E8 folding), Gun_level_up 9, Gun_option_ck 15.
  chat/sk/hk: not worked through per function; most diffs are float constants (the C uses literals, the original reads
  its own literal pool: linking such a function needs `extern f32 lit_NNNN[]` pool reads, not done), prim struct
  layouts of the local PFLP4/PFLP8 stack structs (original keeps several separate stack variables), and loops.
- f_menu quick pass (retry of the 2-5 instruction near-matches): nothing new matched in the time box
  (menu_data_mix_sub/monster_sub/chcnfg_reibun: declbf finds no better order; Pit_mv, disp_needle etc. untouched).
- tools/perm.py on a function inside a whole-file nm.c gave scores around 1300 for a 15-instruction diff (the context
  is the whole file): not useful here; use try.py-style variant lists (small script comparing check.py output) or
  declbf.py instead.
- Not done: f_menu quick pass beyond pef_get_alpha (2, float temp reg), Pit_disp_chat_cnfg (2, lui/ori register of the /3
  magic number after PitMenu.x1B became s8), disp_needle (4), Pit_mv/Pit_mv_lb. Float-literal functions (most of the f_chat
  UI) cannot be linked until the literals are read from the original pool (`extern f32 lit_NNNN[]` with the right NNNN).

# select.bin and yn.bin overlays (agent C, 5 Oct 2026, second assignment)

Both overlays are built through config/c_files.txt like game: `select START END NAME` is
src/select/NAME.c, `yn START END NAME` is src/yn/NAME.c; jump tables need a
`select:rodata START END NAME` line (exact table end, no padding). Strings and other data stay as
asm (declared `extern char lit_NNN_ADDR[]`). Shared declarations: include/select.h (select only;
it carries its own partial SYS_W / SEL_W / EDIT_W / DEMO_W layouts, so do not include flow.h or
f_game.h in the same file).

## select.bin (0x533A00-0x538580): 44 functions
- select00.c Init_task, demo.c (title/logo/opening movie: Demo_task, demo_task_sub, violence_logo,
  capcom_logo, middle_logo, c_disp, title_disp, opening_demo) all match.
- edit_nm.c holds the whole f_disp.s file (character edit + continue screens); matching runs are
  split out as edit00..edit08.c (tools/mkruns_mod.py does the split) and linked. What stays asm
  is listed in the status table at the end of this section.
- Lessons (function that shows it):
  - Unused leading arguments: callers pass leftover registers. `McCardOperation()` and
    `system_w_set()` with no arguments matched (Init_task); `param_change_sub(w, btn, p, max, se)`
    is called with w as an unused first argument (param_change_00536280).
  - Float parameter order: `SoftKeyboard_pos_set(int, f32)` needs a real prototype, otherwise a
    float passed to an unprototyped call is promoted to double (edit_trans).
  - A global that the original reloads after a store through a pointer: `*(volatile u16 *)&Psw[4]`
    gave the two loads (roll_move).
  - `u32` in the cast `(f32)(u32)x` produces the bltz/srl unsigned-to-float sequence (arrow_disp).
  - `if (0 <= n)` gives slt+bne instead of bltz (cmn_mongon_check_filter); switch with cases
    written in ascending order is tested in descending order (demo_task_sub, disp_check).
  - Statement order of struct stores can be brute forced: permute lines with itertools and
    keep the one that compares OK (title_disp, ~40k compiles at 0.05 s each, found in seconds).
  - tools/draft.py now resolves switch tables (lit_NNN_ADDR in the same overlay) so m2c
    drafts functions with jump tables (Edit_task, Cont_task).
- disp_edinfo matched with `(0x280u - len * 10) >> 1` (unsigned constant, not a (u32) cast of the
  whole difference); status: select 38 of 44 functions linked (tools/progress.py: 50% by bytes; the 44th is a nop).
- Not linked (near-match, logic complete, in edit_nm.c): edit_pl_init_new / edit_pl_init (original
  reads stage_start_pos x/y/z through three separate symbols D_2F2620/24/28 that only exist as
  auto-generated undefined symbols; our C uses stage_start_pos[n][i] = one base register),
  disp_edit_spr (register allocation, 4 saved regs vs 6), disp_color, Edit_task, Cont_task (big state machines, only drafted from
  m2c and cleaned, not tuned), cmn_mongon_check_sub, cmn_mongon_set (hand unrolled copy loops).

## yn.bin (0x533A00-0x53C800): 104 functions
Linked and checked (yn OK): yn_sd, yn_mc, nc00-nc04 (network config helpers incl. yn_hard_*),
ui00-ui06 (string/draw helpers, key repeat, scecom reboot), misc00/01. Counts (progress.py): 11.5%.
- The Sony library part (sce_callback.s, sce_cbfunc.s, sceNetcnfif*, about 45 functions from
  libnetcnfif) is compiled with GCC, not MWCC: an m2c draft compiled with MWCC differs in every
  instruction (checked with sce_callback, sce_call_rpc). No C is provided for it; keep as asm or
  replace with the SDK source/own implementation in the port (the PS2 network adapter code is not
  needed on Xbox anyway).
- src/yn/netcnf_nm.c: all 28 Capcom network config functions (yn_netcnf_*, yn_hard_*, yn_utf8_to_sjis,
  yn_sjis_to_utf8, module_load/unload). They compile; the ones that match are linked as nc00-04.
  Near-matches (not linked): work_to_ifc/dev, dev_to_work, setup_devwork, set_current, get_num/list,
  net_allload, magicno_check_sub, pastdata/pastproxy_check, utf8/sjis converters (m2c-derived,
  structure guessed: the ifc struct is 0x1330 bytes, dev 0x1320; module_load is 0x1CC in the
  original and 0x13C here, so its real structure is different).
- src/yn/ui_nm.c: the 57 UI functions of f_yn_535340.s from tools/draft2c.py. 34 compile; 19 are
  wrapped in `#if 0 /* name: m2c draft ... */` (yn_set_main compiles but is far off; big
  switch-heavy ones like yn_select_provider, the yn_*_font_sub family, yn_dialog_*, yn_keyboard_init,
  yn_sprite_draw_each are still raw m2c). tools/ifdef0.py disables failing functions,
  tools/ifdef1.py re-enables one after you fix it.
- Tools added: tools/draft2c.py (m2c drafts of a whole asm file as compilable K&R-style C, gp
  globals named, M2C_FIELD macro from include/yn.h), tools/mkruns_mod.py (split matching runs of a
  *_nm.c for any module), tools/mkrun2.py now also parses K&R definitions.
- Lessons: old-style (K&R) definitions `void f(a, b)\nint a;\n{` make small passthrough wrappers
  match (yn_printf, yn_set_pal: callers pass wider/other types, and the sign-extension of an s8
  parameter happens inside the callee); a float-taking tail call needs `void flfntSetZ(f32)` so
  `yn_set_z(f32 z) { flfntSetZ(z); }` becomes a plain `j` (yn_set_z); m2c loses trailing arguments
  of calls (module_load takes 5, yn_netcnf_num_to_ip 6 values): compare the asm when a call has
  fewer arguments than expected; a gp global holding a work pointer (`ynw`) is accessed as
  `((STRUCT *)ynw)->field` with the cast repeated at each use to get the original reloads
  (yn_key_repeat).

# Third assignment: unowned main code 0x100000-0x1A0000

Candidates (asm files still without C; sizes are the whole asm file; owned elsewhere are skipped:
player f_pl 0x134000-0x15B000 (F), hit 0x111000-0x125000 and camera/weapon (D), f_game/f_stage/f_quest (E)):
- 0x15AED0 f_trans (sprite list, 0x430), 0x1611F0 g_system_w_init (0x290), g_Put_sprite_rotate (0x8E0), f_calcpoint (0x9B0),
  g_get_prim_ptr (0x5B0), g_RollView (0xEF8: view, em_work, smell/smoke/senko stacks, ground data, fms), f_set (0x318)
- f_font (0x11B4), g_font_set_stack_no (0x77C, softreset/Get_sw...), f_disp_162DB0 (0xA60 loading screens)
- 0x11E910 f_release (0xC8), g_load_texlist (0x348), f_load (0x6D0), f_ioread (0xE44), g_cpAng2Rad (0x114C vector math),
  f_yure (0x7F0), f_get (0x1ED0 model work), set_used_clay (0xC74), g_armor_model_free (0xC6C incl. Scheduler/Tsk_*)
- f_option (0xC40), Pit_* leftovers of the menu pass (menu_* near-matches), g_Fade_task (0x438), f_em (0x3428), f_tri (0x14B0)
- 0x16B060.. f_flps2 / flib graphics library (0x2890+, ~0x28000 total, probably Capcom/Sony mix: left for last),
  0x18D9D0.. pl* AHI/AMO model loader library, 0x196390.. sceCd*/libc (GCC or Sony libs, not Capcom: skip).
Status per file below as it is done.

## Sprite/system/prim files (src/main/sprite, src/main/prim)
- sprite/trans2.c (f_trans 0x15AED0, 4/4 match): sort_sub must be `static` and defined before its caller:
  MWCC then keeps the caller's loop variable in a0/t3 across the call (it knows the leaf callee's register use).
  `(*s & 8) / 8` (signed divide) gives the bgez/addiu 7/sra sequence; flSetRenderState(int, int) with `(int)m` fixed an a0/a1 order diff.
  Statement-level locals: `u8 *p = s + sort_no[i]*0x50;` before the flag test fixed the delay-slot placement.
- sprite/sysw.c (0x1611F0, 11/11): ran_suu needs `u32 v` locals declared r, v, p order; init_std_rate wants the constant in a local `g`.
- sprite/putspr.c, putspr2.c (Put_2TF, Put_F, Paint_square, Put_megaphone, stage_w_init, stage_fog_set match):
  struct copy of 12/20 bytes compiles to lwc1/swc1 pairs; Paint_square statement order found with tools/stperm.py
  (new tool: brute-forces the order of N single-line statements); stage_w_init needs its own struct with real fields
  (a FLD macro with a base pointer makes the compiler CSE `&stage_work` and emit different code).
  Not matched: Put_sprite_rotate (0x15B300, big, 4 temp pointers), Draw_square (src/main/sprite/putspr_nm.c, 76/78 differ).
- prim/prim2.c (0x169300, 10/10): get_prim_ptr takes int here (header says s16; callers extend). add_prim returns the slot
  (header says void; not changed, prim2.c has its own PRIM typedef).

## Progress, third assignment (main 0x100000-0x1A0000)
Linked (all verified with a full `tools/rebuild.sh main` OK; main went from 124108 to ~150000 bytes matched):
- sprite/trans2 (0x15AED0 sprite list draw), sprite/sysw (system work helpers, ran_suu, gauss table), sprite/putspr+putspr2
  (Put_2TF, Put_F, Paint_square, Put_megaphone, stage_w_init, stage_fog_set), sprite/calcpoint
- prim/prim2 (prim pools, add_prim, draw_prim, SetDiffuseColor)
- emw/emw01-02, gmat01-02 (view accessors, em_work push/pull, smell/smoke/senko/ear/yobi stacks, ground material data),
  emw/emu01-02 (em_init, em_die, em_erase, enemy_mv in f_em)
- cp/cp01-02 (vector/angle math: 24 functions), load/lf01-03 (file/model loaders, Meltw LZ decompressor, link file accessors),
  model/hp01-04 (model slot heaps), model/gm01-05 (model work setup/free, Attribute_from_amo), fade/fd01-02 (screen fade),
  font/fs1_01, fs2_01-03, gfs01 (render state cache, font setup, all_reset/softreset, pad accessors), sys/io01-04
  (ACRMain main loop, system_w_set, InitSystemData), sys/tsk01-02 (task scheduler), sys/vw01-02 (View_move, set_aov)
- New headers: include/mdlw.h (model work), include/sysw.h (system_w fields). prim.h/em.h not changed.
Near-matches kept in *_nm.c (not built), with the reason:
- sprite/putspr_nm.c Draw_square (76/78; saved-reg pointer temps); Put_sprite_rotate (0x15B300) not attempted (big).
- emw/emwork_nm.c em_work_set (48/100 register allocation), pull_enemy_work (29/45 block layout);
  emw/groundmat_nm.c GetPlayerDiffuseData (reg alloc), GetPlayerShagamiData, fmsInitialize/fmsAllocMemory (scheduling).
- cp/cpmath_nm.c NormalClipCheckF3 (64/101), PointHitCheckF3 (41/55, original keeps st non-constant), parts_chg, parts_init (not done).
- load/loadf_nm.c load_armor_model (17/32), load_texlist/load_texlist_pl (reg alloc 18/51, 16/58); mkMaterial/mkModel*/Sethierarchy
  and the f_get functions with string literals (system_error calls) not done.
- model/heap_nm.c get_start_material/hierarchy/clay/mdlw (19/54: the summing loop and the exit shape differ).
- model/getm_nm.c model_work_set2 (41/64), release_model, model_work_free (21/49, odd (s16) extension of tex_n).
- font/disp1_nm.c disp_load_msg (16/50), Ck_hankaku (5/34); disp2_nm.c Start_item_init (10/58), Disp_button (6/110), Put_comment (2/77);
  font/fontst2_nm.c han2zen (2/38: slti into at); sys/ioread_nm.c ioread_sub (original keeps a dead load), ioRead, setBGcolor, InitCommonWork (25/109);
  sys/tsk_nm.c Scheduler (2/94), sys/view_nm.c set_viewproj (43/53, the original keeps a stack copy of the proj_tbl row).
Not attempted (jump tables, varargs or string literals): Game_clear_ck, Disp_NowLoading2, font_print/font_print2/font_print_ex, MakeMediaVersion,
  ioRead_sub/ioRead2, SpritePut, Pl_model_id_set/armor_create_model (PLW offsets), the f_flps2/flib graphics library (MWCC, ~0x28000 bytes, left for last),
  sceCd*/libc (Sony/GCC).
Lessons (function that shows it):
- check.py masks relocation addends: two stores `sb v, system_w+0x2E` / `+0x3E` with the same value compare equal even when the order is wrong;
  ALWAYS run tools/rebuild.sh main before committing a run (InitRenderState).
- MWCC unrolls constant-trip `for` loops (full unroll when the count is small, 8x otherwise). `i = 0; do { ... i += 8; } while (i < 0x20);` gives the
  plain loop of the original (em_init hagi loop, smell_init stacks). A one-case `switch` (case X: return 1; default: return 0;) gives the
  original's branchy code (kb_input_ck_enter).
- A static leaf defined before its caller in the same file lets MWCC keep caller values in a0/t3 across the call (trans2.c sort_sub).
- `long` is 64 bit in this compiler (GetLinkFileSize returning long gives the dsll32/dsra32 truncation at the caller).
- 20/0x1C byte struct copies of s16/float structs compile to lwc1/swc1 pairs (Put_2TF).
- Statement order of single-line stores: tools/stperm.py FILE FUNC FIRST LAST (new; brute force, max ~7 lines) and the random sampler idea
  (shuffle N orders) when the line count is too big.
- mkruns_mod.py: FORCE_OK=name,name env marks functions whose only diffs are jal targets in another module (overlay calls such as func_53A190);
  mkrun2.py cannot split empty function bodies (put a comment inside) nor macro-generated functions.
- After splitting a *_nm.c into runs, run check.py on every run file: sibling functions need prototypes (cp02 SetVector lost its float prototype).

- fl/pv01-03 (0x192E30 plFCVSetBaseAddress, 0x192FA0 plGetFcurveTime, 0x193150-0x193344 plvec length/normalize/inner/outer/plane): match.
  plmatCopy33 (in plvec_nm.c) sits inside another asm file's range and is not linked.

## Fourth assignment: unowned main code 0x100000-0x1A0000, second pass (Sonnet worker C, 5-6 Oct 2026)
Done this pass (all rebuilt byte-identical; nm = near-match file kept for the functions that still differ):
- sound/bgm_nm.c: stage BGM server and setters (bgm01-02 + rev01 linked; em_status_ck, lobby_bgm_set, stage_bgm_set stay nm, 4-15 diffs).
- aq/aq_nm.c: f_aq network session layer (aq01-06, 17 of 27 functions; AQ_init, get_AQdata, self_data_ctrl, set_other_data,
  AQ_data_put, pl_data_put, pl_AQ_put, item_ans_send, host_change near-matches 1-18 diffs).
- sk/cmd_nm.c: soft keyboard conversion commands (cmd01-04, 17 of 23).
- net/connect_nm.c (f_connect), net/netbgm_nm.c (net bgm/MMBB menus), net/netwk_nm.c (g_network_work_init: step machines,
  pad helpers, Ncm_spr_* request bits), net/ncmreq_nm.c (the five *_disp_req queues), net/netname_nm.c, net/ms_nm.c (f_ms),
  menu/pit_nm.c (g_load_pit). New include/netcw.h (net_common_w) and include/main.h (m2c draft header).
- mc/mccard_nm.c (the memory card screens, 0x281BC0-0x2860D0) is 95% byte-identical already (static-name noise only) but cannot be
  linked in runs: mc_r_no_set is a file static called by every screen, and decode_to_ck/check_sum_ck are statics of the same
  original object (their a0 preservation is why callers do not reload a0). It links only as one file with the mcsave range.
New tool tools/linkruns.py NM.c DIR/PREFIX [--dry]: builds maximal runs of matching functions (OK or only call-name noise),
writes PREFIXNN.c with mkrun2, registers them in config/c_files.txt together with one main:rodata line for the switch jump tables
of the run (tables must be adjacent; two lines for one object do not link). Run `tools/rebuild.sh main` first so asm/ reflects
c_files.txt (carved tables vanish from the data asm), then linkruns, then rebuild. mkrun2 cannot split empty bodies: put a comment.
Lessons (function that showed it):
- A jump table gives every case body: dump the table (asm/main/data/data/*.rodata.s, dlabel lit_N_ADDR), group labels by body, emit
  `case N:` for every id of a label in address order. A 122-entry request switch (Ncm_mssage_disp_req) matched at once this way;
  the parameter must be `u8 id` and `switch (id)`; holes and ids past the end need no label (netwk/ncmreq_nm.c is generated code).
- A compare chain lists case labels in reverse source order; labels with identical bodies in two places (ms_network_net_file
  cases 5 and 6) are two separate source cases, MWCC merges the bodies.
- `if (n > 0)` gives blez but `if (0 < n)` gives `slt at,zero,n; beq` (Set_KouhoTable); `x >= 2` gives `slti v,` but `x > 1` gives
  `slti at` (cmd_muhenkan; for unsigned `u32 c; c > 1` gives sltiu at); strlen is unsigned.
- A one-test `if (f() == 1) { ... }` whose original is `beq v0,1,L; nop; b end; nop; L:` is a one-case `switch (f()) { case 1: ... break; }`
  (cnnect_err_set, connect_error). `if (x == 0x4E || x == 0x6E) {...}` rather than a switch gives the original chain (cmd_henkan).
- `x = x + 1` on a byte global reloads x when written `COM_R_No_0++` after other stores (connect_error) and uses the cached
  switch value when written `x = x + 1`; try both.
- Struct array element fields give `sym+4` as a relocation addend plus one `addu` for the index (net_swdata3: `Psw[i].x04`, with
  34-byte elements); a byte-offset macro on a u8 array folds the offset into the load instead.
- Passing a 0x2C-byte constant struct by value: copy it to a local first (`cfg = lit; f(&cfg)` gives the lq/sq copy then pointer).
- K&R definition `int f(a, b, c) int a; ... {` keeps later calls with fewer arguments legal (send_my_data called with 2 args).
- Locals: MWCC gives the lowest stack address to the LAST declared local; temps declared first end up higher (Net_disp_net_name:
  declare the big buffer last). Saved registers: first declared gets the highest s-register in several functions; declperm.py tries
  all orders but is slow (kill it with pkill -f "declperm.py src" if it hangs).
- `if (a != 0) return; ...` vs `switch (a) { case 0: ...}` and `return c ? 1 : 2` vs `c == 0 ? 2 : 1` pick movz/movn.
- Unprototyped callee with 5 args from m2c (flSndSetRev) needs the extra `0, 0`; m2c drops trailing zero arguments.
Not done in this range (needs a vendor library or is huge): ADX/CRI (0x101000-0x117E50, SJ*, svm_*, sdr_*), sce*/libc (0x154000-0x160000),
IME dictionary (g_dic_open 20 KB, f_kh, f_wd, f_api, Overlay_reset/kh_learn), f_flps2 graphics library, f_disp_26CD60 (13.6 KB),
f_ncm text drawing, f_ms patch download (ms_net_patch_set 6.9 KB), f_mcsls/f_ave/f_wait/f_prot/CpInet network core.

### Late additions (second pass, end)
- Also linked: net/aqcmd01-03 (AQ command lists), plus fixed-and-linked nm files for plmem, amo2, netfile2, tex, staff and Scheduler.
- mcsls_nm.c stays near-match only (not linked). The r0 state handlers and app queues (vram 0x230CD0-0x232494) are NOT done; the MCSLS player array sits at +0x4C with a 0x3C stride. m2c output needs hand typing with that struct.
- tools/linkruns.py: always run tools/rebuild.sh first, otherwise carved jump tables vanish from asm and the link fails.

## Fifth assignment: main, both halves (Sonnet worker C, 5 Oct 2026)
Fixed the menu03/menu04 range overlap (menu_retire_i belongs to menu04; menu03 now ends at 0x1287F0). Rebuild all five OK after every step.

Done (all rebuilt byte-identical; `_nm.c` keeps the functions that still differ):
- net/mcsls_r0_nm.c + mcsls_r01/r02: the r0 state handlers and application queues (0x230C10-0x231D40): 17 of 19 functions linked
  (mcsls_r0_pingpong 83 diffs and mcsls_recv 450 diffs stay nm: pingpong min/max registers, recv has a different loop
  nesting at the start and 2 more saved registers). New include/mcsls.h with the session layout (CNMSG queue, MCSPL 0x3C bytes per
  player at +0x4C, MCSLS). net/mcsls_nm.c (0x232494-0x232C50) rewritten on it: 9 of 12 linked (mcsls_t01-03); send_size_get (13 diffs,
  saved-register order) and send_command_app_data (73, the tag byte is masked earlier) stay nm.
- option/option_nm.c: the whole OPTION screen (0x1267C0-0x127440), 11 functions, linked as option/option01.c.
- item/item_nm.c: the 64-slot field item pool and the item-preparation (mix recipe) tables (0x11CDF0-0x11DB00): 13 of 17 linked.
  Item_preparation (29 diffs, saved register order), Item_preparation_rate_0 (14), Item_preparation_adrs (3), list_num (popcount macro,
  the original hoists &User_data) stay nm.
- ud/udmisc_nm.c (0x271FB0-0x272400): Load/Save_userdata, ItemCopy_*, Gold_add, Get_hunter_rank/status linked (udmisc01/02);
  Set_equip_data (5 diffs) and Set_userdata (100 diffs, the original keeps &User_data and the player work in s0/s1) stay nm.
  include/ud.h: carved UDW.gold (0x20) out of the padding.
- game/flow_nm.c (0x110F10-0x111B1C): game_init, stage_load, player_all_load, load_eft, st_model_load, swset_w_init, init_light_work,
  the motion loaders linked (flow01/02). load_shadow (2 diffs, instruction order of a dsra32), init_pl_work (77: block layout of the
  `be_flag = 1 / else be_flag = 0` test) and round_init (260: the original uses 6 saved registers, mine 9; the monster placement loops need
  strength-reduced induction pointers) stay nm. include/flow.h: carved STGW.stage (0x02), x34, x38 out of the padding.
  `STGW *sw = &stage_work;` as a local (declared after the int it is used with) is what makes MWCC keep the base in a saved register (st_model_load).
- Near-match fixes linked: GetRailCamPos, cam_rail_move_0 (camarea02, camr2n01), tri_in_check (tri01).
Unmatched but understood: wall_act_ck/wall_vec_set (pl_nm.c, 1 diff each: `addu v0,v0,s1` operand order of an index add), disp_needle
(menu_disp_nm.c, 4 diffs: which float register holds the constant), pef_get_alpha (2), menu_data_mix_sub (4), menu_data_monster_sub (5).

Lessons (function that showed it):
- check.py compares only the original size, so a run file can pass check.py and still be longer: always run check.py on the generated run file
  (linkruns copies declarations but a callee defined earlier in the nm file is not visible: Item_preparation_rate returned
  an s8 call result and got an extra dsll32/dsra32 until `s8 Item_preparation_rate_0();` was declared in the run file). A rebuild MISMATCH with
  "built N bytes, want M" and a shift of the next asm function means an object is longer than its range.
- Compare chains: a `switch` gives `beq x,k; nop` chains; `if (a != 7 && a != 6 && a != 5)` gives packed beq with a delay-slot instruction.
  param_change_00126B30: `switch (row) { case 5: case 6: case 7: break; default: ... }`. A one-case `switch` gives `beq v1,v0,L; b end` (disp_option_sub_menu).
  Chain tests run in reverse source order, so the source cases are ascending when the chain tests descending (Option_task).
- K&R definitions keep a u16/s8 parameter unnarrowed: `void option_main_menu(t, pad) OPTTSK *t; u16 pad; {` with a `()` forward declaration
  matches the original's `daddu s0,a1` where a prototype with u16 gives andi at every call. Same for a callee called with fewer or more
  arguments than it takes (Item_preparation_rate calls `Item_preparation_adrs()` without arguments: the registers pass through).
- An s16 variable in a register is normalized (dsll32/dsra32) at each use, an int variable only where it is cast: disp_option_menu uses `int y`
  with prototype `flfntLocate(s16, s16)` and gets the original per-call conversion with a plain `addiu` for `y += 12`.
- `if (x > 0xC8)` gives `slti at,x,201; bne at` where `x >= 0xC9` gives `slti v0` (mcsls_app_que_send, app_push_is_ready); `i > 0x41` gives `slti at`.
- `if (!x) continue; return i;` gives sltu/xori/bnez, `if (x == 0) {} else return` does not (mcsls_calc_master_id).
- `if (a || b) { ...returns... } return 0;` puts `return 0` last (mcsls_get_error_code).
- `a + b` operand order: `(u8 *)(i * 16) + (int)ptr` swaps the add (GetRailCamPos); `int t = f() & 0xFFFF; if (t + s >= N)` (tri_in_check); `*(u8 *)((u8 *)tbl + (a + a))`
  gives `addu v0,a,a` for a 2-byte element index (Item_preparation_one_ck).
- `option_w[6]`-style repeated accesses are hoisted into one register only when a local `s8 *w = option_w;` exists (option_sub_menu).
- Declaration order decides saved registers; tools/declbf.py handles plain declarations, a hand-made permutation loop (scratch script, 4-5 names, 120 tries) the rest.
  The register of a stack temp follows the LAST declared stack object (mcsls_recv: `u8 pad[12]` declared before the u16 temp and the queue struct, CNMSGB = queue + 10 bytes).
- Bit counting written as nested `((u8)((x & 0x55) + ((x & 0xAA) >> 1)))` macros matches in shape (Item_preparation_list_num) but the original loads all the bytes through one hoisted base.
- Item pool free-list push `*--item_sp = p--` in a `for (i < 0x40)` loop reproduces the 8x unrolled original (init_item_work); declare `int i; ITEMW *p;` in that order.

### Unwritten Capcom functions in main (no C anywhere), by area, largest first (sizes in bytes; after this pass)
- game flow, stage load (0x110000): round_init 1036, init_pl_work 520, stage_load 224, st_model_load 220, load_eft 216, load_shadow 196, game_init 140, swset_w_init 104, player_all_load 88, em_motion_load 60 (17 functions, 3 KB)
- lights, model loading, ioRead (0x11DB10-0x125000): yure_move_hair 1864, mkModel4 1184, ioRead_sub 1140, flash_move 1048, mkModel 1040, armor_create_model 964, Pl_model_id_set 944, mkModel3 896, Pl_light_set 792, parts_init 748 (36 functions, 17 KB)
- player select / debug enemy select (0x14E0C0): em_select 600, sel_default_set 540, disp_em_select 392, player_sel 344, Plsel_task 248 (8 functions, 2.5 KB)
- sound requests (0x159500): se_req2 872, armor_sd_req 736, snd_joint_load 568, snd_joint_load_pl 396 (18 functions, 4 KB)
- sprites and fonts: SpritePut 2048, Put_sprite_rotate 840, font_print_sp 972, font_sp_ck 564, font_print2 372 (8 functions, 5 KB)
- enemy model/ride (0x109E30): mlCalcTransEM 1712, em_ride_sub 1632, em_search_set 564 (16 functions, 5 KB)
- quest and shared items (0x226A00): quest_condition_prog 3420, Item_regained 756, Quest_next_em_set 576, Share_item_stack 508, Net_Share_item_stack 468 (16 functions, 8 KB)
- network core (0x22E000-0x233000): mcsls_move 632, mcsls_init 556, CnInetMcsReceive 484, module_load 460, module_loadhigh 424 (46 functions, 5 KB); prot_00/prot_01 (0x2381F0) 2796 and 3260, InetDisconnectAll 1588
- Ncm text (0x26CD60-0x271000): disp_spr_sub 11912, net_connect_draw 1420, Ncm_br_mc_mssage_disp 784, ncm_str_disp_sub 624, Ncm_menu_disp 624 (13 functions, 17 KB); DispFrameMessageA 3316 (0x276170)
- net patch/DNAS (0x28A170-0x28D000): nb_flps0009 788, PatchExecCS 740, net_flps0008 724 (11 functions, 4.6 KB); reward_itembox 1240, staff_disp 372 (0x290AE0)
- flPS2 clay/dma/file (0x16AEC0-0x170000): flPS2ConvClayData 4536, flPS2SetMaterialData 1580, flPS2StoreImageB 1312, flPS2VIF1MakeLoadImage 1196 (35 functions, 19 KB, Capcom's own flPS2 layer)
- not listed: ADX/CRI/sce/SJ/newlib/mpv (0x100008-0x117E50 front, 0x170000-0x21F000 vendor), IME (0x23E500-0x24A240, agent E), memory card (0x2814E0-0x2862F0, agent E).
Written but not matching (`_nm.c`): see tools/ scan idea: `for f in src/main/*/*_nm.c; check.py -v` and sort by differing instructions; 95 functions are within 8 diffs.

## Sixth assignment: main leftovers and small near-match sweep (Sonnet worker C)
Linked (all rebuild OK): ud/udb01-02 (gun_check, Equip_ok_ck, Get_equip_bit, wyvern_kill_cnt_up, Gunner_wasure_ck, Ex_quest_ck),
quest/qstb01-03, sound/sndb01-02 + sndc01-04 (Snd_init, se_req, Code_Make, Pl/Em/Npc_se_req2, snd_joint_load_pl, pack loaders),
chat/chatb01 + chatc01, fl/plvecb01, em/femb01, menu/pitx01, sk/cmdy01 + skx01, net/aqcmdx01 + cngmsgx01, cam/camr2x01,
plsel/plsel01-02 (player_sel, player_wait, em_select; debug player/monster select, plsel_nm.c).
Near-match still: se_req2 (7: `vol` in v1 not a3), armor_sd_req (original 5 saved regs), snd_joint_load (15), disp_em_select (68, regs),
sel_default_set (129, regs), wall_act_ck/wall_vec_set (1: index add operand order, not fixed by 15 variants), load_shadow (2).
Lessons:
- check.py cannot see switch case ORDER or the data a case uses: Equip_ok_ck/Get_equip_bit matched under check.py but the jump table
  differed (case blocks in source order 2,3,5,4,0). Always rebuild before trusting a run.
- Compare operand order picks the slt destination: `v[j]->time > pivot` gives `slt at` where `pivot < v[j]->time` gives v1 (AQQuickSortSub);
  `rp->sec > target` (cam_rail_move_sub); `wr + n > m->cap` (CngNet_MSG_Write); `n*3+3 <= a` (cmd_next_kouho).
- `(u8 *)(i * 4) + (int)ptr` swaps the addu operands (UseItemChk); `lpSKey[(n - x) + 0x358]` (cmd_prev_bun); `(v + (int)base)` (Em_data_com_adrs_get).
- K&R definition `int f(p, f, e) int p;` + local `u8 q = p;` stops the re-mask of a u8 param passed on (palette_ng_sub2).
- `u8 *m = mission_area;` declared first hoists the gp load into the branch delay slot (Start_item_data_adrs_get).
- Calls whose callee takes s16 args load with lh: prototype SoftKeyboard_move(s8 *, s16, s16) (Reibun_Edit_Core).
- Local `u8 *sw = select_w;` keeps the base in a saved register (player_sel/player_wait). tools/flipcmp.py tries operand flips per function.
- The unnamed 0x24A240+ and 0x1C0000-0x230000 runs linked here are now agent D's range; all were committed before the hand-over.

## Seventh assignment: main coverage sweep (Sonnet worker C, 5 Oct 2026)
Range: all of main except agent D (0x1C0000-0x230000, 0x24A240-0x2814E0) and agent E (IME, memory card). All five modules rebuild OK.
Tools added: tools/mk1.py (write a run file from a *_nm.c by brace matching: `mk1.py NM.c OUT.c "header" func...`; keeps the
non-function chunks, K&R definitions are NOT recognised: delete the stray copy by hand), include/va.h (original va_start expansion).
The per-function scripts used (try replacements, brute-force declaration moves) were throw-away; the permuter (tools/perm.py, -j1,
one at a time) found 8 matches in the end: pef_get_alpha, Pit_disp_chat_cnfg, Pl_item_num_ck3, menu_chcnfg_reibun, frame_move,
menu_data_mix_sub, hit_point_cbd, Pl_scope_ck/silencer/barrel, Taru_ok_ck, afs_file_length. Its junk edits (`if (p && p) {}`,
`new_var`, `do {} while (0)`) are kept with a comment because they give the original bytes; none changes the logic.
Linked (matched C, all rebuild OK): sys/adxs01-05 (0x100380-0x101E38: ADX server, file load queue, BGM streams, effect work pool,
effect draw helpers), em/emsrch01-03 + emmk01 (durability slots, joint accessors, enemy_mk, ride_ofs_calc, em_effect_pull),
font/fsp01-02 + fprint01 + dsp03 (font_sp_ck, print helpers, Start_item_init), sys/view201, hit/hitid01, hit/hitpk01, hit/hit3
(hit_point_cbd), model/mkm01, crmdl01-02, light01-03, net/cnmsg01+03 (Inet message queue, CngNetTimeGet, mcsls accessors),
net/cpinet01-06 + 07-11 (CpInet* wrappers), plsel/plsel00, reward/rwkey01-02, set/setwork2 (init_set_work), frame f_frameb
(frame_move), pl/pl_wall, pl_demo, pl_itemck, pl_ammo, pl_flagck, weapon/wtrans01, menu37-39, sys/ior01, tex/reltex, sys/adx_err.
hit/hit2all.c: the whole 0x28CE00-0x290560 range is one file with three raw functions (hit_sphr_sphr2, hit_cap_cap2_m,
hit_cap_cap3_m via config/c_rawfuncs.txt) so the static hit_point_sphr keeps its calling convention and hit_cap_sphr2_m (1060) and
hit_line_sphr2 (408) link. progress.py counts raw functions as done (9048 bytes), they are NOT matched.
Not linked / near-match now (logic complete): hit_cap_cap2_m 41/1253 and hit_cap_cap3_m 90/945 (only f20/f22 and i/j register swap,
dsw moves of the declarations do not fix it), hit_hit_sub_em 106/802, hit_hit_sub_pl 2, hit_calc_shl 4, egg_com_ck 4, Pit_mv 4 and
Pit_mv_lb 3 (need `int pit_key_repeat(u16,u16)` seen by the caller, split file), load_bin 8, eft_rgba_linear 179, em_search_set 58,
get_start_material/hierarchy/clay/mdlw/heap (5 functions, the sum loop: original keeps j in a register, ours folds j=0),
Sethierarchy (127), crmdl_nm.c pl/weapon/em/npc/set/edit_create_model + release_enemy_model, font_print_sp 167/245 (fsp_nm.c),
CpInetTcpOpen/Close/Delete + ProblemEnable, CCnNetMsg_CnReadSeek 5, CpInetDnsLookUp 2, ioread_sub (dead stores kept by the original).
Lessons (function that shows it):
- Early exits as `if (A && B) {} else { return; } stmt;` or `if (r >= 0) {} else { return r; } return f();` give the original's
  `bgez; nop; b end` stub layout (trans_pl_sub, CpInetTcpGetOption). `return;` in place of `break;` at the end of a case adds the
  `b end` stub (load_task). `if (x == 0) {} else {}` with the flag compare as `== 0` gives the original block order (pl_flag_ck).
- A one-case switch with a default (`switch (v) { case 5: ...; default: ... }`) is the original for `beq; nop; b else` (CpInetTcpInitialize);
  `switch (r) { case -33: case -1: r = -1; }` for the error mapping in the CpInetDns* wrappers.
- `if ((r = f()) < 0)` (assignment in the condition) tests v0 before copying to the saved register (pull_eft_work, CnInetMcsReceive);
  `while (ok && size())` not `while (ok != 0 && size() != 0)` (CnInetMcsReceive).
- `kind - 6` range tests: `(u32)(kind - 6) <= 2` writes sltiu into `at` (enemy_mk); `x <= 0x9F` vs `x < 0xA0` too (font_print2).
- MWCC unrolls `for (i = 0; i < 64; i++) a[i].k = -1;` 8 times itself (load_work_init); `*--sp = p--` pointer stacks likewise.
- Varargs: `ap = (char *)__builtin_next_arg(parm) - (__builtin_args_info(2) >= 8 ? 0 : (8 - __builtin_args_info(2)) * 8)` is the
  original va_start (include/va.h); callers pass named args via real prototypes.
- (s16)f() results of an int-typed callee: declare the function `int` and write `return (s16)Ave_X();` (no re-extension at callers,
  CpInetTcpNbCallEnd); `s16` returns make every caller extend.
- Far (non-gp) 1/4 byte globals must be declared with a size > 8 (`extern s32 Inet_interface_status[4];`) to get lui/addiu.
- Unsigned/int/s16 of loop variables decides extensions: armor_model_free uses `int i` with an s16 field read, release_stage_model
  `s16 i` with a separate `s16 t = i` for the calls.
- `r = *t++; g = *t++; b = *t;` (not t[0..2]) gives the original's advanced pointer (setBGcolor).
- Several statements `a = x; b = y` at the top of a function change which saved register a variable gets; a stray initialisation
  `s16 cur = 0` at the declaration hoists the `li` into the prologue (font_print_sp, not fixed).

## Eighth assignment: near-match sweep, ranges 0x100000-0x160000 and 0x230000-0x23E500 (Sonnet worker C, 5 Oct 2026)
Linked (main rebuild OK): net/ave01 + ave02 (the 48 "Ave_" IOP network RPC wrappers, 0x233B10-0x23525C; Ave_TcpSend 360 bytes and
Ave_TcpRecv 288 stay near-matches in ave_nm.c: loop shape / double s16 normalisation), net/cpinet12 (CpInetDnsLookUp), set/setwork3
(get_heap_ptr), pl/plx01 (Sansai_talk_ck), pl/plx02 (Get_Use_itemnum). Outside my new range (0x160000-0x1C0000, now agent E, all committed):
font/disp103 (Ck_hankaku), fl/tx07 (flPS2GetPaletteVramBlock), fl/flm03 (flvecCopy).
Still near-match: load_bin 5 (saved register order of part/file), em_search_set 57, stick_pow_get 1 (r tested masked, returned unmasked),
flPS2SearchVramSpace 3 (extra nop), CpInetDnsLookUp done, CpInetTcpOpen/Close/Delete, flPS2DmaAdd*Tag 16-18 (schedule), CCnNetMsg_CnReadSeek 5
(rd/size register swap), getsh_nm.c (get_start_material/hierarchy/clay/mdlw: MWCC unrolls the byte-sum loop 8x, the original does not).
Library, not worth matching: sceNetGlue* + ipaddr_from_string (0x236B70-0x237800, Sony netglue), sceUsbKb* (0x23BE10-0x23D870), flPS2Dma* (hardware
DMA with pcpyld / inline asm, 0x16E790-0x16F0F0).
Lessons:
- A switch whose LAST case falls out (`case -2: r = -3;` with no break, then `return r;`) gives the original's "last block has no `b end`" layout
  (CpInetDnsLookUp, flPS2GetPaletteVramBlock with `int r;` declared first).
- Result of an s16-returning callee kept in `int r` and function returning `int` avoids the second dsll32/dsra32 at the return (Ave_GetOpt);
  `if (r > len) {} else memcpy(...)` gives `slt at`; `if (0 <= r) {...}` gives `slt at,r,zero; bne` instead of bltz (Ave_DnsLookUp).
- Struct assignment of a 3-float struct gives lwc1 x3/swc1 x3 in the original order (flvecCopy); 11-float struct likewise (Ave_PppStatus).
- Rounding `(16 - n % 16) + n` (operand order) for RPC sizes (Ave_SifCallRpc). `(s16)!f()` in an int function = sltu/xori/dsll32/dsra32.
- `for (i = 0; i < n + 1 ...)` with `i = 0` first, and `c <= 0x9F` instead of `c < 0xA0` (Ck_hankaku); `(int)base + (n << 9)` order (get_heap_ptr).
- declbf killed by a timeout leaves the file in a worse permutation: always `git diff` the nm file after a killed run.
- mk1.py silently overwrites an existing run file: check `ls` for the name first.

## Ninth assignment: near-match pass + remaining-area list (Sonnet worker C, 5 Oct 2026)
Ranges: main 0x100000-0x160000 and 0x230000-0x23E500. Linked (all five modules rebuild OK): net/cnmsg02 (CCnNetMsg_CnReadSeek),
net/ave03 (Ave_TcpRecv), net/cpinet13 (CpInetInterfaceProblemEnable), menu/menu40 (disp_needle), item/item01 extended to 0x11D328
(Item_preparation_adrs), fade/fd03 (Fade_busy_ck). No shared header edits.
Lessons (function that shows it):
- Result of an assignment as the return value: `int f(int on) { return g[0] = !on; }` keeps the value in v0 (the plain `void` version used v1)
  (CpInetInterfaceProblemEnable).
- Param copies: when the original keeps `s0 = a2` raw and a second register holds the normalised value, the param is an `int` and there is a
  separate `s16 l = len;` used for compares and arguments, with `len = (s16)n;` where the original normalises (Ave_TcpRecv). `if (n <= len)`
  and `if (0 < len)` give `slt at` where `!(len < n)` and `len > 0` give v0 / blez; `len > 0x3CA` instead of `>= 0x3CB` sets `at`.
- An int-to-float cast of a nested int expression: `k = k / 5 * 5; flSinCos((f32)k * C - D, ...)` gives the original's register order for
  the cvt (a single expression `(f32)(k / 5 * 5) * C` swaps f1/f2) (disp_needle).
- Swap of two s16 parameters: `s16 t = b; b = a; a = t;` (the other direction `t = a; a = b; b = t` swaps two instructions) (Item_preparation_adrs).
- `if (A && B) {} else { return 0; } i = x - 1; return (p != q) ? 1 : 2;` gives the early-exit stub layout and the branch sense (Fade_busy_ck);
  a pointer `FADE_ENT *d = &fade_data[w->cur - 1]` stops the (cur-1)*28 being folded into the symbol offset.
- MWCC does not unroll `if (n > 0) do { sum += a[pos + j]; j++; } while (j < n);` but unrolls the equivalent `for`/`while` (get_start_* in
  getsh_nm.c: do-while form is 16 of 52 instructions off, the original is a non-unrolled loop with an explicit `slt at,j,n` test that no source
  form reproduced). The first search loop is `if (pos < N) { p = &heap[pos]; do { if (*p == 0) break; pos++; p++; } while (pos < N); }`.
- `goto test;` / `top:` / `test: if (len > 0) goto top;` reproduces a loop whose test is at the bottom entered by `b test` (Ave_TcpSend, still 76/90 off:
  an extra callee-saved register for the normalised len).
- Also linked later in this pass: pl/plx03 (pl_work_clr), pl/plx04 (stick_pow_get, rodata 0x35A7B0-0x35A7C8), sys/adxs06 (load_bin), net/cpinet14+15
  (CpInetTcpClose/Delete), item/item04 (Item_preparation_rate_0), pl/plx05 (pad_timer_calc_sub).
- A K&R definition `f(pl, no, prog) PLW *pl; int no; ...` keeps `no` raw in its saved register where the u8 prototype normalises it (pl_work_clr); when a
  shared header declares the prototype, put the function in its own run file and rename the name around the include
  (`#define pl_work_clr pl_work_clr_proto_unused` before the headers, `#undef` after, then `void pl_work_clr();`): no header edit (plx03/plx04).
- A function whose result variable is u8 and whose original returns it unmasked but tests it masked: give the definition a `u8` return type and test
  `(r & 0xFF)`; `int r` makes the constant loads addiu where the original has daddiu (stick_pow_get).
- `0 <= t` (not `t >= 0`) gives `slt at,t,zero; bne` and `if (..) { r = f(); *p = -1; r = g(r); } else { *p = -1; } return r;` the original layout (CpInetTcpClose).
- `if (mode == 0) { A } else { return 0x64; } if (rate > 0x64) rate = 0x64; return rate;` puts the else stub before the join (Item_preparation_rate_0);
  `rate += tbl[i]` on a u8 keeps the value in its saved register.
- The permuter's junk: `st = part;` (dead store before a call, load_bin) and `if (pl && pl && pl) {}` (pad_timer_calc_sub) are kept with comments. 330 s runs, one at a time,
  solved both after 240 s runs had found nothing.
- NEW C written (no earlier C existed) and linked, all `rebuild OK`: net/cpinet16-26 (CpInetTcpGetStatus, CpInetPppGetDns, CpInetPppGetATScript, CpInetHttpInitialize,
  CpInetHttpResolvCacheInitialize, http_wait_thread/CpInetHttpSignalThread, CpInetDelayThread, InetDnsCacheInitialize, InetConnectAll, wait/signal_http_static_sema,
  CpInetPppGetStatus), net/netdev01-11 (DeviceGetOptionalStatus, bind_rpc_blocking, set_device_no, _device_check, _reset_recognize, _reset_dialtype,
  search_sif_call_rpc, DeviceSelectInitialize, rpc_initialize, _decide_dialtype, InetConnectAllCore), model/light04 (light_move), model/yure01 (yure_move).
  All are from m2c drafts (tools/draft.py) fixed by hand; the field names are guesses from use.
- Lessons from the new C:
  * A m2c `switch` whose cases map a state code to a state code is a jump table when the case values are dense (CpInetTcpGetStatus 12 cases at 0x36CE20, CpInetPppGetStatus
    two tables): register `main:rodata` with ONE range per object, from the first table to the end of the last (0x36CEA0-0x36CF08, the 8 bytes between the tables
    are the 16-byte alignment). Two rodata lines for one object gave a link that was 16 bytes too long ("MISMATCH built N want M").
  * `default:` written between the numeric cases keeps the original block order (CpInetTcpGetStatus, CpInetPppGetStatus); `case 2: case 3:` listed in ascending order
    for a compare chain that tests 3 first (DeviceGetOptionalStatus), `if (k != 3 && k != 2) return;` gives a different layout than the switch.
  * `for (;;) { if (p->id == 0) break; ...; p++; }` gives the original's test-at-top loop where `for (p = x; p->id != 0; p++)` is inverted (set_device_no).
  * An 8-argument callee (InetConnectAllCore) passes args 5-8 through; a callee with `(void)` prototype that m2c shows with fewer args often has more: check the
    registers a0-a3, t0-t3 in the caller (InetConnectAll passes eight pointers into InetSys).
  * `int f(...) { r = (s16)call(); if (r >= 0) {} else { return r; } ...; return r; }`: a function that ends with the call result still in v0 returns int r
    (CpInetPppGetDns, CpInetTcpGetStatus); with a void return the stub layout differs.
  * Far (non-gp) globals again need a declared size > 8 (`extern s32 Inet_http_static_sema[4]`), near ones must NOT (PppRecognize u8 stays gp).
  * Sony-sample style code (module_load/module_loadhigh/module_unload with 8-nop gaps) and the USB keyboard files look like library code: not attempted.
  * declbf found the register order for light_move (w, p, q, i, in, out) after 5 minutes; start it in the background and poll.
Near-match status now: load_bin 5 (part/file saved registers swapped; declaration order irrelevant), stick_pow_get 3 (masks the test, ours masks
the return instead; int r, (u8)r, copies did not help), Pit_mv 4 / Pit_mv_lb 3 (of which 2 and 1 are the real now/hold register copy, the rest are
cosmetic func_NNNN names of other modules; the caller needs `int pit_key_repeat(u16,u16)` which both nm and a split file have), hit_hit_sub_pl 2 (one
add.s operand order: constant first in ours, no source form changes it), hit_calc_shl 4 and egg_com_ck 4 (a `nop` before the final `b end`),
CpInetTcpOpen 12 (original copies the pointer: `daddu v0,a0`), CpInetTcpClose/Delete 19, em_dur_set 17, load_shadow 2 (order of `li t0,0x900` and
dsra32), release_model 7 (prologue schedule), menu_data_monster_sub 5 (m and the &lpPit->x82 pointer swap registers), pad_timer_calc_sub 2 (original
has `addiu v1,gp,off; lhu 0(v1)` for the gp array; no declaration or cast form gives it), Sel_back_disp 7 (statement order [w,x,h,y,v,col,u2,v2,u]
is the best of 120; the col expression is scheduled differently), Ave_TcpSend 76, em_search_set 57, eft_rgba_linear 179, font_print_sp 167.
Permuter (-j1, 240 s each) found nothing better for: load_shadow, Item_preparation_adrs (solved by hand), hit_hit_sub_pl, stick_pow_get, egg_com_ck,
menu_data_monster_sub, load_bin, hit_calc_shl.

Update (later in the same pass): load_bin, stick_pow_get, pad_timer_calc_sub, CpInetTcpClose/Delete, Pl_hold_item_ck, Item_preparation_adrs, Item_preparation_rate_0,
disp_needle, Fade_busy_ck, pl_work_clr and Ave_TcpRecv from the list above are now LINKED (see the file names in the first lines of this section); the near-match
numbers quoted for them are the state before they were solved. Still near-match: Pit_mv 4 / Pit_mv_lb 3, hit_hit_sub_pl 2, hit_calc_shl 4, egg_com_ck 4, CpInetTcpOpen 3
(`o++; o--` junk gets it to 3: the original copies the pointer with `daddu v0,a0`), em_dur_set 17, load_shadow 2, release_model 7, menu_data_monster_sub 5, Sel_back_disp 7,
get_start_* 16 each (do-while form), light_init (written, 146/146: the original copies the three floats with `lwc1 0; lwc1 4; addiu a0,8; lwc1 0` and keeps `light_work`
in s0), DeviceUpdateStatus (written, 12/83 with declbf; in build/scr only, not committed), rpccall_end 2 (`lui v0` instead of `lui at` for the semaphore id).

### Unmatched Capcom code left in my ranges (after this pass; the network device/IOP helpers and light_move/yure_move listed above are now done), largest first. Sizes in bytes; "C" = C exists in a *_nm.c (written, not matching),
"-" = no C yet. Library (not worth matching) is noted separately.
- 0x15C000 trans_stage 15152 (C, stage model transform), 0x10C000 em_material_sub 7500 (-, called from weapon3_nm.c only)
- effects: eft06_m 4848, eft13_m 2688, eft13_set_pos 2512, eft13_set_pos_em 2720, eft13_set_sub_em 1388, eft13_i 1924 (all C, far off), eft_rgba_linear 944
- set13: set13_m 4264, set13_trans 3100 (C)
- hit (0x113000-0x11D000): PushAdjust3 4024, hit_hit_sub_em 3208 (106 off), sphr_face_o3/o4 1936/2016, GetEyeHitLine 2568, GetWallHitLine 2096, GetWallHitBitPl/Em
  2196/2164, GetWallHitBit2 1668, GetGroundHitStatusAreaEm/Pl 1768/1408, GetGroundHitArea* ~1000 each, GetFloorSlide 1088, HitWallPlayer 868 (all C)
- menu/HUD (0x127000-0x134000): disp_item_sub_select 3872, trans_box 2556, Menu_mix_mv 2120, player_info_sub 1836, disp_whole_map 1564, Pit_mv 1528 (4 off),
  disp_partial_map 1252, item_stock_mv 1160, disp_pachinger 1092, gage_disp 812, disp_timer 756 (all C)
- game flow: Game_task 3096 (C), round_init 1036 and init_pl_work 520 (C, ~260 off), load_shadow 196 (2 off), em_move 2648 (C), mlCalcTransEM 1712 (-?), em_ride_sub 1632
- player (0x136000-0x14C000): basic_com_ck 2368, pl_move_sub 2144, timer_calc_sub_pl 1700, sougun_adj_sub 972, gun_adj_sub 928, pl_mv021 988, egg_com_ck 916 (4 off)
- model/light/io (0x11D000-0x125000): yure_move_hair 1864, mkModel4 1184, ioRead_sub 1140, flash_move 1048, mkModel 1040, Pl_model_id_set 944, armor_create_model 964
- body hit / items: body_hit_sub_em/new/body_hit 1220/964/864, Pl_item_stack 1020, Pl_horm_adj 680, Pl_box_select 780
- sound/sprite: se_req2 872 (7 off), armor_sd_req 736, snd_joint_load 568, SpritePut 2048, Put_sprite_rotate 840
- network 0x230000-0x23E500: prot_01 3260, prot_00 2796, InetDisconnectAll 1588, mcsls_recv 1876, mcsls_r0_pingpong 884, mcsls_move 632, mcsls_init 556,
  CpInetPppStart 720, DeviceLoadDriver* (~1.6 KB, IOP module loading), SetResult_Read 1396 and cnv_keycode/vblank_e_handler/push_repbuf (USB keyboard, no C)
- omake/ncm menus 0x23A000: disp_mode_menu 992, mode_sel 824, Sel_menu_disp 792, disp_omake_menu 656, npc_move 936, npc_trans 576
- Library (skip): sceNetGlue* / ipaddr_from_string / InetIPAddrFromString-like Sony netglue (0x236B70-0x237800), sceUsbKb* (0x23BE10-0x23D870), flPS2Dma*, ADX/CRI
  (0x100008-0x117E50 front part), Sony sce/newlib/SJ/mpv (0x170000+, not mine).

## Tenth assignment: small near-matches, field checks, device/session code (Sonnet worker C, 6 Oct 2026)
Ranges: main 0x100000-0x160000 and 0x230000-0x23E500. Linked, all `tools/rebuild.sh` OK for all five modules (all in `config/c_files.txt`):
pl/pl_itemck extended down to 0x152BC0 (Pl_item_num_ck2), pl/plx07 (Pl_vital_calc_item), pl/plx08 (rate_g_calc), set/set13c extended to 0x158DB0
(set13_disp_pos_calc), hit/shit2a (BlockPlaceCgeck, GroundFieldInCheck, WallFieldInCheck, AreaFieldInCheck), net/netdev12-17
(DeviceModuleInitialize_blocking, DeviceLoadDriver2, DeviceLoadDriver, DeviceRollbackDriver, prot_02, InetDnsGetIPAddress), net/mcsls_i01 (mcsls_init),
net/mcsls_m01 (mcsls_move). Header edit: include/mcsls.h carved pad14/pad28/pad3C/pad44/pad16A into dt, t_prev, t_sec, nsent_prev, nrecv_prev, x16A
(proven by mcsls_init/mcsls_move; no other header touched). shit2.c (all five grid helpers) stays for the PC build; shit2a.c is the linked run.
Lessons (function that shows it):
- `if (x < 8.0f || (z = p[2]) < 8.0f) return 0;` gives the original's single shared return-0 stub where nested `if (!(x < 8)) { z = ..; if (z < 8) return 0; ..}`
  adds a second one (AreaFieldInCheck, Ground/WallFieldInCheck); the second value is loaded inside the condition.
- Two divisions feeding one result: write the integer conversions first (`iz = (int)(z / cz); ix = (int)(x / cx);`) and combine afterwards; the original
  does both divides before the compares (BlockPlaceCgeck). A random order of the declaration lines found the register assignment (all 8 locals matter).
- `0 <= f()` (not `f() >= 0`) gives `slt at,v0,zero; bne` for a negative-result test; `x16A >= w` vs `w <= x16A` flips `slt v0` to `slt at` (mcsls_move).
- A switch whose compare ladder is in descending order needs the cases written in the reverse of the ladder: DeviceLoadDriver2 wants `case 2: break; case 1: {..} case 3: break;`
  (the ladder tests 3, 1, 2); DeviceLoadDriver wants `switch (sub) { case 4: ..; default: ..; }` for an `if (sub == 4) else` pair that compiles as `beq; nop; b`.
- A shared `return 0` at the very end of a function with an exhaustive switch: `case 1` bodies that `goto z;` to a final `z: return 0;` reproduces
  `b end; nop` (DeviceRollbackDriver); with `default: return 0;` plus an inner `return 0;` the two returns do not merge. A case that ends with `*b = 0;` then `break;`
  to a final `return 0;` is the original's layout (InetDnsGetIPAddress).
- Four-way float copy `out[i] = base[i] + d * dir[i]`: load the three direction values into locals first and make `a = d * x; b = d * y; c = d * z;` temporaries,
  then add the base (set13_disp_pos_calc). `(f32)(f10 + dt)` with the stored value read back from the struct (`mcsls_w.dt`) keeps one mov out (mcsls_move).
- Ternary instead of if for `v = (D < C) ? D + 1 : C; store v` removed a nop (prot_02). `int r = (s16)t; t = (s16)(r / 2);` keeps the parameter in its own register
  and `t <= 1` instead of `t < 2` selects `slt at` (rate_g_calc). Junk `if ((pl && pl) && pl) {}` before `return 0xFF;` as in Pl_item_num_ck3 (Pl_item_num_ck2).
- Same-file IPA: when a callee defined earlier in the same original file is tiny, MWCC keeps the caller's loop variable in a caller-saved register (ioRead uses a2 across
  `jal ioread_sub`). ioread_sub (0x11FA30) keeps its two dead loads and a dead `li`, which no C form reproduces, so ioRead/ioread_sub/ioRead2 stay unmatched.
Parked near-matches (not built, kept in the nm files): hit_hit_sub_pl 2 (add.s operand order, `def + 80.0f` is canonicalised), hit_calc_shl 4 / egg_com_ck 4
(extra `b end` stub, the original does not thread it), load_shadow 2 (li t0 / dsra32 order), Pit_mv 4 / Pit_mv_lb 3 (now/hold copy register), CpInetTcpOpen 3,
DeviceUpdateStatus 11 (netdev_nm.c), InetDnsSetAll 21 and CpInetPppStart ~18 (netdev2_nm.c, new C), InetIPAddrFromString 2 (netdev2_nm.c, new C; also
ipaddr_from_string 0x002368E0 is the same text), release_model 7, menu_data_monster_sub 5 (declbf/declhill/permuter found nothing for these), se_req2 7, pl_light_change
(new C in no file yet: logic as in the m2c draft; the original keeps the stage 12/13/14/28/30 test as five separate compares), parts_init (m2c draft: three loops,
the 21-iteration one is unrolled 7x by MWCC; our version differs in register allocation of w/q/i).

## Eleventh assignment: single player first (Sonnet worker C, 6 Oct 2026)
Remaining unmatched functions in my ranges: 236 functions, 208 KB (list from config/c_files.txt + c_rawfuncs; sizes in bytes).
Split by caller: single player = reached from Game_task/round_init/em/pl/menu/village paths; online = cp/net/inet/mcsls/ppp/USB-keyboard-for-chat stack.

Single player (work these first, largest first):
- stage/models: trans_stage 15152, em_material_sub 7500, set13_m 4264, set13_trans 3100, mkModel4/mkModel/mkModel3, armor_create_model, Pl_model_id_set
- effects: eft06_m 4848, eft13_m 2688, eft13_set_pos_em 2720, eft13_set_pos 2512, eft13_i 1924, eft13_set_sub_em 1388, eft_rgba_linear 944
- collision: PushAdjust3 4024, hit_hit_sub_em 3208, GetEyeHitLine 2568, GetWallHitBitPl/Em 2196/2164, GetWallHitLine 2096, sphr_face_o3/o4, GetGroundHit* family, hit_calc_shl 1368, hit_hit_sub_pl 1072, GetFloorSlide, HitWallPlayer
- game flow: Game_task 3096, em_move 2648, mlCalcTransEM 1712, em_ride_sub 1632, round_init 1036, init_pl_work 520, load_shadow 196
- player: basic_com_ck 2368, pl_move_sub 2144, timer_calc_sub_pl 1700, body_hit_*, Pl_item_stack, pl_mv021, sougun/gun_adj_sub, egg_com_ck, pl_egg*, pl_at*
- menus/HUD: disp_item_sub_select 3872, trans_box 2556, Menu_mix_mv 2120, player_info_sub 1836, disp_whole_map, Pit_mv 1528, Pit_mv_lb 416, disp_partial_map, item_stock_mv, disp_pachinger, gage_disp, disp_timer
- sound/sprites: SpritePut 2048, se_req2 872, Put_sprite_rotate 840, armor_sd_req, snd_joint_load
- omake/mode menus at 0x23A000-0x23E000 (called from the title/mode select, not the net): disp_mode_menu, mode_sel, Sel_menu_disp, disp_omake_menu, npc_move/npc_trans, Sel_back_disp, key_rept_du
- library, skip: _start/_root/_exit, Adx_init (CRI), ioRead/ioread_sub/ioRead2 (known unmatched), MakeMediaVersion

Online (last): prot_01 3260, prot_00 2796, mcsls_recv 1876, mcsls_r0_pingpong 884, mcsls_send_command_app_data, mcsls_send_size_get, InetDisconnectAll 1588,
CpInetPppStart 720, CpInetTcpOpen, Ave_TcpSend, DeviceUpdateStatus, InetDnsSetAll, InetIPAddrFromString, ipaddr_from_string, module_load/loadhigh/unload,
SetResult*/rpccall_end/USB keyboard (cnv_keycode, vblank_e_handler, push/pop/clear_repbuf, usbkbdm_*, getPS2KbData, usbKbConnectChk), Menu_chatcnfg_mv,
menu_chcnfg_sendpl, lb_disp_chat_cnfg_sendpl (chat config UI).

## Twelfth pass (Sonnet worker C, after the network cut-off)
Linked: egg_com_ck (pl/plegg.c, 0x14A6B0-0x14AA44): `return;` instead of `break;` after the dash branch (early-return form). The single-player / online split of the
remaining list is the "Eleventh assignment" section above. load_shadow: `(s16)(i + 0x127)` is far worse (23 off); stays at 2 off. Remaining near-matches unchanged.

## Thirteenth pass (Sonnet worker C, single-player list, all `tools/rebuild.sh` OK)
Linked (main module, single player): Put_sprite_rotate (sprite/putspr3, 0x15B300), flash_move (model/light05, 0x11DE60), Pl_model_id_set
(model/crmdl03 + rodata 0x358480-0x358498), parts_chg + yure_init (cp/cp03, 0x121280-0x121380), player_init0 (pl/plx09), hit_hit_sub_pl
(hit/hite, 0x113E50), St_pick_ck2 (pl/plx10), pl_light_ck (pl/plx11), pl_egg03 (pl/plegg2). No header edits. No online code touched.
Lessons (function that shows it):
- A struct/array local with `addiu v0,sp,off; sh r,0(v0)` stores is just `a = b = x` chained assignments (the inner one is stored first); declare the
  bigger local FIRST to get the lower stack slot (Put_sprite_rotate: TRI before SPR).
- A counted loop is NOT unrolled when written `i = 0; do { ... i++; } while (i < n);` (yure_init: orig is a single 10-store loop), but a `for` is
  unrolled 4-8x; and a `for` whose body has 9 statements is not unrolled while one with 8 is (the unroller has a body-size limit, parts_init).
- `j = sum = 0; for (; j < n; j++)` keeps `slt at,j,n` with j in a register (no folding of the first test), `for (j = 0; ...)` folds it (get_start_*, 7 off).
- Switch whose compare ladder is 2,1,0 wants the cases written 0,1,2 (flash_move, parts_chg wants 0x12 before 0xE: the reverse of the ladder).
- `(f32)p[2]` of a u8 gives the unsigned-convert branch only with `(u32)`: `1.0f / (u32)p[2]` (flash_move). Pointer walks that must not fold
  (`d[1] = a[0]; d[2] = a[1]; a++; a++; d[3] = a[0];`) need `a++; a++;`, not `a += 2` (flash_move).
- `f32 k = 80.0f; pw - pw * def / (def + k)` keeps the operand order of add.s that `80.0f + def` / `def + 80.0f` flip (hit_hit_sub_pl).
- `for (i = 0, mx = tbl; i < 6; i++)` (init inside the for) and `eq = ..; ` before it fixes the order of `daddu s2,zero` vs `addiu s0` (player_init0).
- The order of the `return` stubs at the end of a function is the textual order of the return statements: `if (n > 0) {..} else { return 0xFFFE; } return r;`
  puts the 0xFFFE stub first (St_pick_ck2). An `int r` assigned `(u16)f()` gives `andi s0,v0,0xFFFF` once and a plain `daddu v0,s0` at the return.
- `*(u8 *)((int)e + 0x612)` for the second use stops MWCC making a hoisted `addiu s3,s0,0x612` pointer that the original does not have (pl_light_ck).
- `v = (arg1 == 1) ? 4 : 0x72;` instead of `v = 0x72; if (arg1 == 1) v = 4;` moves the int-to-float `mtc1` to the join point like the original (pl_egg03).
Near-matches after this pass (all in the *_nm.c files): armor_create_model (written in the thirteenth pass, instructions identical, only the s-register
numbering of i/p2/p4/off differs: orig i=s2 p2=s1 p4=s7 off=s6 id=s3 h=s4; declbf/declhill found nothing; the s16-id version needs `int id = (s16)mdl[i]`,
`(u32)(id - 13) <= 1`, the model_work_set2 arg `(u16)(skin + sex * 4)` and explicit pointer counters `p2 += 2; p4 += 4; off += 2`), get_start_material and
friends 7 off (j and the pointer IV swap a2/a3), key_rept_du 5 (idx4 and the `on` pointer swap t0/a3), em_dur_set 8 (the `n * 2` is computed before the table
address in the original), weapon_create_model/edit_create_model 10, Sel_back_disp (the OR order of the colour; best expression shape gets 2 lines off),
pl_egg05 32 (the original does not fill two branch delay slots and has a `nop` before an aligned block), parts_init (original unrolls the 21-iteration
loop 7x and keeps nine stores per iteration, ours is not unrolled: the unroll limit), mode_sel (the original has `nop nop` padding in front of case 0).
Also linked later in the pass: pl_mv060 (pl/plx12, 0x13EA40), edit_create_model (model/crmdl04, 0x124F80), load_texlist (load/lf04, 0x11E9E0). More lessons:
- `if (c) v = 12; else v = 8;` (not `v = c ? 12 : 8;`) when the original does the int-to-s16 extension of v after the branch (pl_mv060); the opposite
  holds where the ternary moved the `mtc1` to the join (pl_egg03): try both.
- When the saved-register numbering of two pointer/index variables is swapped, drop the explicit induction variable and write the expression in terms
  of the loop counter: `model_work_set(.., (s16)(10 + i * 0x32), ..)` instead of `y += 0x32` (edit_create_model); `mem_tex[base++] = ..` instead of a
  `dst = &mem_tex[base]` pointer (load_texlist, which also moves the pointer set-up behind the `0 < n` guard as in the original).
- Do not use an automatic statement swapper on code with side effects: a trial swap of two Pl_item_stack calls and of a load call and a field read
  "improved" the diff count while changing the behaviour (both reverted).
Still near-match, tried this pass without success: Pit_mv 4 / Pit_mv_lb 3 (the original loads `now` into a0 and copies it to the saved register; assignment
inside the argument, int/u16/u32 types, a hold variable and statement orders all give the same 3), load_shadow 2, menu_data_monster_sub 5, pl_dm008/pl_at012
(the original re-copies a0 from s0 in the first call block), Pl_slash_lv_ck, se_req2 7 (permuter ran 15 minutes), em_dur_set 8, release_model 7.

## Fourteenth pass (Sonnet worker C, after the crash restart)
Remaining unmatched main-module functions in my ranges are all written in C now (nm files) except network/library code (prot_00, npc_trans is C but 30 off,
sceUsbKb*, _start). Linked (all five `tools/rebuild.sh` OK): Pit_disp_menu_equipment (menu/menu41.c, 0x1337A0) and release_tex_sub (tex/reltex.c, now
0x11E910-0x11E9D8 with release_texture). No header edits. Lessons:
- Pit_disp_menu_equipment: `u32 v` (not int) for `(f32)v` gives the unsigned-convert branch; EquipmentDescriptionWindow takes a 5th int argument (original sets t0=0),
  so the prototype in menu_disp_nm.c was widened (the menuNN.c files that call it with four args still match, the extra register is only set by this caller);
  assigning `q.s[0]` before `q.s[2]` fixed the constant load order.
- release_tex_sub: `for (i = 0; i < n; i++) { p = &mem_tex[start]; ...; start++; }` (pointer recomputed from a counter that is bumped at the end) puts the address
  computation behind the guard and keeps the loop in registers; pointer-walk and pre-loop pointer forms hoist it. Same idea as load_texlist.
- tools/mkone.py NM.c OUT.c FUNC HEADER builds a one-function file from an nm file in one step.
Still near-match, tried again: Pl_light_set 5 (lp/pb swap s6/s7, all 24 orders of the four pointer locals tried), WallHitInit/GroundHitInit 6 (decl order, block scopes,
`int e = -1`: no change; tweak.py and a 15 minute permuter run found nothing), key_rept_du 5, em_dur_set 4-17, pl_dm008 2 (if/else forms are worse: 6-11),
atck_data_set_shl2 4 hunks (the original copies 24 bytes as three 8-byte lw/sw pairs; 8-byte s32 struct, u32 pair and loop forms all give lwc1/swc1 or a rolled loop),
aan_ofs_calc ~31 (the original keeps `aan` copy in v0 and consumes a0).

## Lobby overlay, 0x5AB000-0x5C4E60 (agent C, 6-7 Oct 2026)
Map (checked with tools/lbleft.py-style counting + jal/data-reference scan of lobby.bin; 100 functions, 46 KB were still asm before this pass):
- 0x5AB000-0x5AE320: cnLBS network protocol (Match*, Patch*, bg-process return, GetRecvData*, SetSend*): online only.
- 0x5AE320-0x5AE8D0: lbs_encode_ex / write_col_numeric / mmbbc_encode (bit encoders for the network strings): online only.
- 0x5AE8D0-0x5AFFA0: item shop (lbshop2). Village AND online: B's note says the town code switches on Online_ck();
  CheckItemPrice_005AFEE0 (shop price check, village shop) is the only unlinked function here.
- 0x5AFFA0-0x5B2E90: Lb_join (guild counter "join a room"), lb_select_*: room list/select dialogs, reached from lb_check_status
  through Lbc_ReadRoomInfo (network): online.
- 0x5B2E90-0x5B4F80: lm_* lobby menus (member list, room member): online.
- 0x5B4F80-0x5B9030: connecting_NN, tcp_init, server_select_NN, cmcs_NN, internet_lobby_act, lbc_login_*: online login.
- 0x5B9030-0x5C2000: CallBack_Result_* / CallBack_Event_* (login, plaza member, mail, room events), Lbc_* room rules,
  lbc_logout/admin message, Analysis_TagCode/Display_StringData (HTML-ish tag text in the lobby browser): online.
- 0x5C2000-0x5C31A0: id_select/handle name selection, test_server_sel_disp, Get_ServerName: online.
- 0x5C31A0-0x5C4E60: village NPC code: lb_npc_item_trans (NPC carried item draw) and ef_move_sub_005C49F0 (NPC sound
  script) are the village functions that were left; the rest of the range is already linked (B's lb_by*/lb_bz*).
Local_main (0x5D8680, F's range) reaches almost everything through function-pointer tables, so a plain jal walk from it finds nothing;
the village/online split above is by caller (jal and pointer-table scan) and name.

Linked this pass (rebuild OK, all five modules):
- lb_vs01 (b/lb_vs01.c, 0x5C49F0-0x5C4CE8) ef_move_sub_005C49F0: village NPC sound script. Compare chain 2B6..2,1 = labels written ascending
  (case 1 is an explicit empty case); `ashi_sd_req_005C4980(em, f32)` needs an ANSI prototype (float in f12, em in a0, first call leaves a0).
- lb_vs02 (b/lb_vs02.c, 0x5C31A0-0x5C34F8) lb_npc_item_trans: village NPC item draw. Lessons (each confirmed by the match):
  * `kind == 0`, `ex[0xE] == 3` tests written as nested ONE-CASE switches (beq; b end), not &&.
  * `em_frame_check2(em, 80.0f, 0)`: the callee takes (EMW *, f32, int); with a0 untouched the compiler keeps em in a0 and puts the
    ex pointer in a1.
  * `case 0x2AD: if (check != 0) return; case 0x2AB: case 0x2AC: ...` (fall into the body).
  * Two FLMAT locals: declare `m` before `jm` (stack order); decl order of mw/c/mat/n/i found with tools/lbdbf.py.
  * The order of the 14 constant stores + the clay pointer decides the constant-load scheduling. A hill-climb over single-statement
    moves (one round, ~200 compiles) went 20 -> 0 after a group permutation (720 orders) got it to 20. Scripts were scratch (not committed):
    permute groups, then try every "move one line to another position"; only use it for independent stores (not for calls).
Near-matches left in the range, all online, mostly 100+ instructions off (b/nm/*.c, cnet/cnlbs_nm.c): Lb_join 306/593, lb_select_room,
lm_member_trans, lm_room_member_mv, server_select_*, tcp_init, internet_lobby_act, lbc_login_*, CallBack_*; small ones that stay stuck on scheduling
(statement-order hill-climb found nothing): cmcs_00 4, connecting_00 3, transOtSelectHandleName 4, net_Check_FriendData 5, _cnet_RecvFromLbs_MatchPlSide 2
(original: the by-value result record is at sp+24 and the received byte at sp+31, which overlap; union/1-byte struct forms tried, no match),
MatchOpponentInfo 3. CheckItemPrice_005AFEE0 (village shop): 24 off with `&&`, 32 off with a one-case switch (right size, wrong register use);
the original has both price branches as explicit slt/beq/b blocks where ours folds the first one into xori.

## Lobby online round (agent C, 6 Oct 2026, evening): cnet protocol, login, callbacks, menus
Lobby 30.75% -> 31.64% (rebuild OK, all five modules). Matched this round (new files): lb_ss01/02 (server_select_00/01), lb_lg01
(lbc_login_finish), lb_ila01 (internet_lobby_act, 756 B), lb_id01 (id_select_01), lb_cb01..04 (CallBack_Event_LobbyLeaver/LobbyCommer/
RoomCommer/MatchStart), lb_nt01/02 (net_time_str/move), lb_gs01 (Get_ServerName), lb_dsi01 (disp_string_id), lb_crr01 (check_room_require,
+ rodata 0x65E240), lb_jip01 (join_input_password), lb_lmp01 (lm_place_trans), lb_tos01/lb_tsh01 (trans(Ot)SelectHandleName); and in
cnet/cnlbs_nm.c (registered through tools/lbreg_cnet.sh = lbregister.sh restricted to the cnet family, which regenerates cnlbs*.c runs):
_cnet_RecvFromLbs_MatchBattleCode/GameRule/GameServerAddr, _cnet_Return_CallBack, _cnet_RecvFromLbs_NoticePatchStart, SetSendStringData,
SetSendStringData2, SetSendEncodeStringData, GetRecvDataString, __cnet_Recv_PatchData, __cnet_Recv_UserIDandHandle, cnLBS_RecvData stays 4 off.
No shared-header edits. Run `tools/lbregister.sh` only after checking `git status`: it regenerates lb/lbnpc, lbui... runs too and broke the link
once when main had promoted functions out of those nm files (use tools/lbreg_cnet.sh for cnet only).

New scratch tools (tools/): lbvariants.py (try whole-function variants from stdin, `=====` separated), lbperm.py (all orders of a statement window),
lbhill.py (move-one-line hill climb), lbdeclrand.py (random declaration orders), lbreg.py (check OK, write src/lobby/b/NAME.c, register, overlap
check, delete the nm draft). Many nm drafts in b/nm are STALE (already matched by agent B in lb_by*; e.g. Lb_ck_menu, Lb_gh_board*, lb_select_room_list,
lb_npc_*, set_se_type): lbreg.py refuses them with OVERLAP. check.py cannot see relocation addends: check_room_require compiled OK with
`lb_pit.step` (offset 0xA) instead of `lb_pit.x08`; only tools/rebuild.sh caught it. Always rebuild before trusting a new match.

Lessons (each from a function that matched):
- m2c "switch ... irregular / goto block_NN" drafts are really if/else-if chains (server_select_00/01: `if (x == 0) {...} else if (x == 1 || x == 2) {...}`;
  a callee called with no arguments in the original must be written with none (get_next_server()) or the compiler sets a0/a1).
- Empty `if (x == 2) {}` is dropped by the compiler; the original keeps the compare: write `if (x == 2) { return; }` (NoticePatchStart).
  Early-return form `if (a) { if (b) f(); return; } if (c) {...}` instead of else-if (_cnet_Return_CallBack: avoids the jump-threaded `b end`).
- A switch whose last case just falls to common code AFTER the switch: `switch (r) { case 1: break; case 0: return; case -1: ...return; }`
  puts the ladder in reverse label order (-1,0,1) and the body after the switch (cmcs_04, tcp_init case 1). A one-case `switch (x) { case 0x1031: ... }`
  gives `beq; b end` (cmcs_04). A shared exit block inside a nested switch needs `goto` (join_input_password: case 4 -> `goto fin`, default falls into fin).
- Struct-by-value result records: CNET_RES locals in sibling blocks end at sp+24 while a u8 sits at +31 (MatchJoin/MatchPlSide/MatchOpponentInfo): still
  unsolved, see docs above (frame hole of 8/16 bytes below the record). Same hole for the 0x120 chat record in CallBack_Event_ChatMessage(TU).
- `sprintf(buf, fmt, h, m, s)` with dead m, s: the original keeps them because they are call arguments (net_time_str); a dead local would be removed.
- u16 params: `void f(w, src, len) u16 len;` K&R made `w->total += len` unmasked (SetSendStringData); with `int len` the compiler masks and CSEs.
- `(u16)(src[0] << 8) | src[1]` keeps the inner andi (GetRecvDataString); `((x << 8) & 0xFFFF)` loses it.
- CNW(T, off) pointer arithmetic on CnetSys_w makes the compiler cache the base in a saved register; use the named field (CnetSys_w.patch_ptr).
- `x > C-1` and `0 < n` instead of `x >= C` / `n > 0` (UserIDandHandle: n > 3, 0 < n) to get slt+at.
- Arguments of a call evaluated from a local temp (`p = CNWP(..); GetRecvDataString(p, ..)`) schedule differently from the inline expression (MatchBattleCode).
- stack frames: a CNET_CHAT-like 0x120 struct plus sprintf needs the 16-byte outgoing area (variadic call): lm_place_trans needed the 5-float
  record declared BEFORE the char buffer (later declaration = lower address).
- Register naming: `s32 sw = Get_sw2(0) & 0xFFFF;` (id_select_01) vs `u16 sw = Get_sw2(0);` (Lb_gh_board): which one matches depends on whether the
  original masks again at use; try both. `cw[0xB + n * 8]` single index expression (not `(n*8) + cw`) gives the original operand order for addu.
- Static callee knowledge: when the original TU defines a small `static` helper above the caller, MWCC keeps caller values in a-registers across
  the call (a2 survives CallBackWaitInit in Lbs_ExitAndEnterPlaza). A non-static helper in the same file does NOT do this. All the "stp"-style
  register differences (lbc_login_warning_message, lbc_login_init, lbc_in_lobby_03_00, Lbc_SetPropaty, Lbs_ExitAndEnterPlaza) point at one original
  translation unit that starts at 0x5B7020 (internet_connect_minimum_cleanup, CallBackWaitInit, Check_CallBackWait are its static helpers) and runs at least to
  lbc_game_ready_00 (0x5BDD50). Lbs_ExitAndEnterPlaza matches (OK in check.py) when those three statics are in the same file in front of it,
  but a file must be one contiguous run, so every function in 0x5B7020-0x5B9EE4 would have to match at once (still unmatched there: lbc_login_init,
  lbc_login_users_personal_data, lbc_login_top_information, CallBack_Result_LoginLobbyServer, warning_message 6 off, id_select 10 off).
  Everything else in that range is already matched in separate files.
- Near-matches kept in b/nm (all logic complete): lbc_login_warning_message 6, lbc_login_id_select 10, lbc_logout_00 7 (store-order of three
  3F33F1/COM_R_No_* blocks), lm_member_list_mv 191 (regs), lbc_login_init (regs, frame OK), create_server_table 51, CallBack_Event_RoomLeaver 22,
  MatchEntryUser 39, disp_lm_room_member 10, tcp_init 8, cmcs_04 4, server_select_05 48, disp_string_handle 7, cmcs_00/01/02, connecting_00/10,
  cnLBS_Get_GameServerAddress 3, cnLBS_RecvData 4, MatchJoin/PlSide/OpponentInfo (record hole).
- Browser/HTML functions (internet_browser, Analysis_TagCode, nwDispStr_Html, Display_StringData, lbc_admin_message_*, check_halfcode) were skipped on
  the coordinator's instruction (browser work paused).

## Lobby online round 3 (agent C, 7 Oct 2026): literal addresses were the culprit
Matched (rebuild OK): lbc_login_id_select (lb_lid01), lbc_logout_00 (lb_lo01), connecting_00/10, cmcs_00/01/04, tcp_init, CallBack_Result_Plaza_LobbyMember,
net_Check_FriendData, __cnet_SendReq_ConditionSearchUser: 11 functions. No shared-header edits.
- Hypothesis "one TU from 0x5B7020 with static helpers" tested (scratch TU with internet_connect_minimum_cleanup/CallBackWaitInit/Check_CallBackWait/
  check_warning_level static, then lbc_login_init/warning_message/id_select/logout_00): NO change in any diff count. So static helpers do not explain
  those near-misses (they still help Lbs_ExitAndEnterPlaza). Do not spend more time on that TU idea.
- THE finding: `*(u8 *)0x3F33F1 = x` (a literal address) schedules differently from the same store through the symbol the address belongs to.
  Most "store order / lui at placement" near-misses (logout_00 7, connecting_00 3, cmcs_00 4, cmcs_04 4, tcp_init 8, LobbyMember 7) were this.
  Look the address up in config/symbols/main.txt (script: size covers the address) and use the symbol with its field: 0x3F33F1 = game_w.step,
  0x3F34C3 = game_w.pl_num, 0x3F34C1 = game_w.master, 0x3A6E94 = net_common_w.timer (include/netcw.h), 0x4E36F4 = ConnWork.sock (word at +4 of
  ConnWork, a 0x2C-byte object; +0x24 is a s16 status), 0x4E4723 = InetGame+3 (extern u8 InetGame[0x14]), 0x39DAD0/2/4 = PitMenu x10/x12/x14.
  `*(u8 *)((u8 *)SYM + off)` is NOT enough in every case; a struct field or SYM[index] form worked (tcp_init needed a small struct for ConnWork so the
  address of the status field is not CSE'd into a saved register).
- id_select_01: two index variables that share a name in different blocks get registers by use count; declaring SEPARATE locals (ix, of) for the second
  block fixed the s0/s1 swap (lbc_login_id_select).
- tcp_init: `SecCunt = 0x3C` placed BEFORE `TryCunt = TryCunt + 1` gives the original's delay-slot store; `x >= 0x15` must be `x > 0x14` (slti into at).
- cmcs_00: stores in source order Vs_Cnt_0, Vs_Cnt_1, flag. cmcs_01: ANSI prototype `s32 connect_ps2(s32, u16, s32)` changes the argument load order.
- net_Check_FriendData / ConditionSearchUser: the loop `i = 0` init lands in the beq delay slot when written as `n = x & 0xFF; i = 0; if (0 < n) { p = ..; do {..} while (i < n); }`
  (FriendData) and `if (0 < n) { i = 0; e = ..; do {..} while (i < n); }` (ConditionSearchUser). Try both placements of the init.
- MatchJoin/PlSide/OpponentInfo (record hole): one more attempt. In the original the 8-byte result record sits at sp+24 and the received byte at sp+31
  (overlapping it); frame is 32. Any form that makes the byte part of the record (union, cast of &res+7) makes MWCC allocate a quad-aligned 48-byte frame
  with a saved s0, so that is not it. Unused extra locals are dropped. Still 2 instructions off (stack slot 16 vs 24).
- Still near-matches in the range: lbc_login_warning_message 6 (sw lands in a2 and cw in t0, original has them the other way; declaration order, separate locals,
  masks, case-4 reuse all tried), lm_*_mv (register allocation), disp_string_handle 7 (sel ternary layout), disp_lm_room_member 10, create_server_table 45 (was 51
  after LbsInfoWork fields), RoomLeaver 22 (all 120 declaration orders tried), cnLBS_Get_GameServerAddress 3 (OR-chain temps), cnLBS_RecvData 4 (an extra nop
  from branch-target alignment), CheckItemPrice_005AFEE0 24 (original keeps both price compares as explicit slt/beq blocks, ours folds to xori; if/else, early
  return, flag variable, cost variable all tried; User_gold in place of the literal made it worse).
