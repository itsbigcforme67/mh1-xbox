# Agent E notes (main module: game flow, stage, reward, quest)

Every "match" below was checked with tools/check.py and `tools/rebuild.sh main`
printing "main OK". Shared headers I touch: include/game.h (fields carved out of
padding, see commit messages), include/pl.h (x73A), new include/flow.h and
include/f_game.h.

## f_game (0x10F050-0x110F08, Game_task + game0..game13, game_core) - 10/12 built
Game mode machine. `Game_task(tsk)` runs setup steps (tsk+8), then calls
game0..game5 by `game_w.mode`; game_w.step (+1) and game_w.sub (+2) are the
step inside a mode. game12 is the sound/model loading sequence.
Built: f_game.c (game0, game10, game11, game12, game13, game1, game2, table
0x3580E0-0x35815C), f_gameb.c (game4, game5, game_core).
Near-match (f_game_nm.c, not built): game3 (10/150 instructions off, order of
the sprite-struct stores) and Game_task (the C is complete; ~90 of 788
instructions differ in real terms: the original keeps tsk->step in a1 and has
different delay-slot filling for the first switch).
Strings (SJIS UI text) are left in the original rodata and referenced as
`extern char lit_NNN_ADDR[]`, so the file registers only its jump tables.
Lessons:
- Named struct fields vs. byte macros: `*(u8*)((u8*)&game_w + o)` lets the
  compiler hoist the address into a register; the original re-loads `lui` per
  access. Give each field a name in game.h (game10, game11, game13).
- `game_w.step++` (not `= game_w.step + 1`) gives the original lbu/addiu/sb.
- Arrays of per-player bytes must be real arrays in the struct
  (`game_w.x1E8[i][k]`) to get the pointer + 8 induction of the original
  (game0), and `x40[i] = select_w.x5C[i]` with a 3-short struct gives lh/lh/lh.
- switch compare chains test cases in REVERSE source order; case bodies stay in
  source order. So write the cases in the order the bodies appear in the asm
  (Game_task).
- In a switch of `if (x) {...}` bodies the original ends each case with
  `return;` (game12), but `else if` chains + `break` where the first branch
  jumps straight to the end (game2).
- `if (x > 0x22550FF)` instead of `>= 0x2255100` stops the compiler sharing
  the lui of two constants (Game_task).

## f_stage (0x15C210-0x15F...): partly done; PAUSED here
Built (f_stage.c, 0x15C210-0x15C6A4, main OK): stage_mv_ck, clr_stg_work, clr_flash,
Stage_env_ck, Pile_on, stage_i. Functions in a file must be in address order
(stage_mv_ck first) or the build mismatches.
f_stage_nm.c (not built): stage_se_move (~100 instr off, register allocation: original
has p=s0.., cnt=s2, n=s1, pl=s3; declbf takes >15 min, run in background), plus
stage_m and move_stage written from the asm but NEVER compiled against the original.
Not started: trans_stage_sub, trans_stage (0x3B30 bytes, huge), spr_disp_sub,
stage_spr_disp (m2c draft via `python3 tools/draft.py main --file f_stage`).
Also not started: f_reward.s (24 fns), f_quest.s (83 fns).
Lessons: prototype float-argument callees (`f32 flSqrt(f32);`) or the arg goes to a0;
`dx=..; dz=..; flSqrt(dx*dx+dz*dz)` gives mula.s/madd.s; stage_mv_ck: use named PLW
fields (macros cast pointers and the compiler hoists addresses). After merging, GAME_W
x208 is pl_state, PLW 0x570 is work570 (s16, cast (u16) for lhu).

## f_reward (0x290E80-0x293B68): 19 of 24 functions built, 2 near-matches
Built (main OK): f_reward.c (key_quest_ck .. gold_main, tables 0x3865A0/0x3865D0), f_reward2.c
(gold_disp, result_init, result_main, tables 0x3866F0/0x386710), f_reward3.c (result_disp, error_disp,
add_disp, reward_init, table 0x3868F0), f_rewardb.c (reward_cursor_mv), f_rewardc.c (disp_reward).
Shared declarations are in include/reward.h (REWARD_W, QUEST_WR, RESULT_W, string externs).
Near-matches (not built, still compile): reward_mv in f_reward_nm.c (9 of 351 instr off: the original keeps
the constant 2 in a2 and the masked key-repeat value in a0), reward_key_repeat in f_rewardb_nm.c (all 29
diffs are register names: original keeps the work pointer in a2), reward_itembox in f_rewardd_nm.c
(~115 of 314 off, register allocation of hoisted sprite-field addresses; logic believed complete).
Shared header edits: game.h (x08, x0D5, x21A, reward_item[16] of PL_ITEM at 0x128; PL_ITEM typedef
guarded by PL_ITEM_DEFINED, same guard in pl.h), pl.h (work91E now u8: lbu in result_init), plf.h
(Pl_item_stack now returns int).
Lessons:
- A C file may contain only ONE contiguous rodata range per `main:rodata` pair of lines; jump tables that
  have strings between them in the original must live in different C files (the linker packs a file's
  tables together). Tables separated only by alignment padding (e.g. 0x3865CC-0x3865D0) can share a range.
  Symptom was a MISMATCH with later data shifted by 0x20.
- The END in `main START END file` is the next function's start (end of last instruction), not the last
  instruction address; a wrong END shifts every later function by the alignment (0x10).
- Old-style (K&R) definitions give the callee-side narrowing of u8/s16/s8 params (`andi a1,0xFF`),
  and are the way to call a function with fewer arguments than it defines (movie_add_ck(no) leaves a1
  untouched): `int f(no, set) int no; int set; {` plus `int f();` earlier in the file.
- `n * -10` gives neg;sll;subu (original) while `-n * 10` does not (result_disp).
- Global struct array element field `game_w.reward_item[i].num` folds the field offset into the symbol
  (lui game_w+0x12A), a cast pointer does not: use a real array of structs in the global struct.
- Case blocks that end in `se_req(...)` where the original fills the delay slot with a store: write
  the store BEFORE the call (reward_mv).
- An empty `case 2: break;` forces the extra compare in a switch whose original has it (disp_reward).

## f_quest (0x226C30-...): started, 14 of 83 built
Source of truth is src/main/quest/f_quest_nm.c (all functions written so far, in address order);
matching runs are extracted into f_quest.c, f_questb.c .. f_queste.c with `python3 tools/split_runs.py
f_quest_nm.c src/main/quest/f_quest ':A-B' 'b:C-D' ...` and registered in config/c_files.txt (END = next
function's start). Types in include/quest.h (QUEST_W, QEM mission enemy entry (0x3C bytes), MISSION).
Built: Quest_error_set2/error_set, Quest_restart .. Quest_remuneration_calc, Quest_condition_judging,
Quest_next_em_clr. Near-matches (nm only): Quest_start (16 off, schedule/reg), Quest_retire_set (15),
Quest_pl_stage_init (11), Em_direct_set (53, register numbering), Quest_next_em_set (written, never
matched: first diff is loop pointer/register shape; not registered). Next: Quest_str_get onward
(asm is in asm/main/text/Quest_next_em_set.s after the rebuild).
Lessons: `if ((q = f()) != 0 && ...)` tests v0 directly (plain `q = f(); if (q ...)` copies first);
a prototype with an s8 last parameter changes argument evaluation order to left-to-right
(Em_data_st_adrs_get); `x > 2` gives slti $at where `x >= 3` does not; `(u8 *)arr + i*2` folds the
array offset into the symbol, `&arr[i].f` does not; `v == 5 || v == 6 || v == 7` reproduces the
original sltiu range test; check.py shows "1/N differ" for functions whose only difference is a
relocation: trust `tools/rebuild.sh main` OK.
