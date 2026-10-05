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
