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
