# Agent E notes (main module: game flow, stage, reward, quest)

Every "match" below was checked with tools/check.py and `tools/rebuild.sh main`
printing "main OK". Shared headers I touch: include/game.h (fields carved out of
padding, see commit messages), include/pl.h (x73A), new include/flow.h and
include/f_game.h.

## f_game (0x10F050-0x110F08, Game_task + game0..game13, game_core) - 11/12 built
Game mode machine. `Game_task(tsk)` runs setup steps (tsk+8), then calls
game0..game5 by `game_w.mode`; game_w.step (+1) and game_w.sub (+2) are the
step inside a mode. game12 is the sound/model loading sequence.
Built: f_game.c (game0, game10..13, game1, game2, game3, table
0x3580E0-0x35815C), f_gameb.c (game4, game5, game_core).
game3 MATCHES now: the sprite-struct stores must be in this order: w,h,x0,y0,z0,w0,one0,one1,alpha,kind,col,z (found by brute-forcing the permutations of the last six statements, ~1 s each with tools/align.py).
Near-match (f_game_nm.c, not built): Game_task (the C is complete; ~90 of 788
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

## f_stage (0x15C210-0x160E??): every function written, 6 of 11 built
Built (main OK): f_stage.c (stage_mv_ck .. stage_i), f_stageb.c (stage_se_move, stage_m, move_stage, trans_stage_sub; f_stagec.c was merged into it).
stage_set_set is in src/main/stage/stage_set.c (agent A). Source of truth for the rest: src/main/stage/f_stage_nm.c (functions in
address order, brace on its own line so tools/split_runs.py can parse them; the file is compiled but not linked).
Near-matches:
- stage_m MATCHES now (main OK): `pos[1] = (65.0f + it->pos[1]) + (f32)(int)((r & 0x3F) - 0x20);` -- the extra `(int)` cast (a no-op) is what puts the cvt before the add and gives `add.s f0,f0,f2`. Lesson: a redundant `(f32)(int)` cast changes MWCC's float expression scheduling.
- spr_disp_sub (colour lerp, 0x1608C0): 67/123. static (see lesson) helps stage_spr_disp, the byte shuffling schedule differs.
- stage_spr_disp (sky gradient from the sun angle + flash overlay, 0x160AB0, 1844 bytes = same size): ~132/461, the colour table loads
  (8 packed colours built from bytes) use different temp registers.
- trans_stage now lives in src/main/stage/trans_stage.c (the ONE definition; it replaced agent A's trans_stage_nm.c, and the PC runtime links it via tools/build_pc.sh; the helpers light_set/get_tex_num/trans_stage_sub are stubs/copies in src/pc/rt/rt_main.c). A call-trace comparison of both versions (32-bit freestanding harness, mocked fl*/flmat* that hash every matrix op, all 88 stages x 5 timer values) was identical except stage 0x28, where the asm falls through from the case-0x28 body into the 0x3B code (layer drawn twice), so mine is kept. The 32-bit sysroot (build/sysroot32) does not exist in this worktree, so tools/build_pc.sh was NOT run; only syntax/-m32 -c checks of the changed files. trans_stage (0x15CD90, 15152 bytes = EXACTLY the original size): written as two passes (stage clays with per-stage UV scroll
  / rotation, then the set objects from the setNN_pos_tbl tables). The first pass (0x15CE90-0x15F700) is instruction-identical except
  registers of the prologue; the second pass differs only in which s-register each per-case local lives in (the original has
  block-local variables per case; mine are function-level). The two jump tables lit_1784_0035B9D0 / lit_1785_0035B9A0
  (main:rodata 0x35B9A0-0x35B9F4) will have to be registered together with the file that holds trans_stage once it matches.
Lessons:
- `static` on a leaf helper defined earlier in the same file makes MWCC keep values in caller-saved registers across the call
  (stage_spr_disp keeps 8 colours in t1..t8 across spr_disp_sub calls); a non-static helper does not.
- A single-case `switch (stage) { case 0x4F: ... }` gives `beq; b end`; two separate `&&` conditions do not (stage_m).
- Variables declared last get the lowest saved register (s0), declared first the highest: declaring `best,px,pz,i,pl,cnt,n,p`
  produced the original allocation of stage_se_move.
- `cnt = 3` that is never set before a `switch (...) {case 3: ...}` test is shared with the compare constant `addiu s2,zero,3`:
  write the assignment only in the cases that have it (stage_se_move case 1).
- Generating the symbolic listing of a giant function: the small script used for trans_stage tracks lui/ori/mtc1 constants and prints
  every call with its argument registers, which is far easier to read than m2c output when floats are passed in f12-f14 (m2c's
  context mode puts them in a1-a3).
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

## f_quest (0x226C30-0x22C66C): all 83 functions written, 52 built
Source of truth is src/main/quest/f_quest_nm.c (every function, in ADDRESS order: split_runs needs that; all
non-function lines, typedefs and prototypes, are at the top). The matching runs are extracted into
f_quest.c, f_questb.c ... f_questq.c and registered in config/c_files.txt. Re-extract after any change with the
new helper tools/genruns.py (see its header: reorders the nm file into address order, writes every run of
consecutive OK functions with split_runs and prints the config lines; the quest config lines are f_quest[f-r]). It strips `static`
from the generated files because the nm file needs `static` on leaf helpers (see lessons) but the linked files must export them.
`tools/rebuild.sh` printed OK for all five modules after the last change. Other helpers added: tools/permdecl.py (brute-force /
hill-climb the order of a function's local declarations, with a mini file that also contains static helpers), tools/symdump.py.
Types in include/quest.h: QUEST_W, QEM (mission enemy entry, 0x3C bytes), QCMD (condition-program command, 8 bytes),
STIEM (pick-up point, 0x1C bytes, StiEM_data[20]), MISSION.
Near-matches (nm only), distance in instructions of the whole function:
- quest_condition_prog (0x22A410, 3420 bytes): whole interpreter written, size equals the original; 532/855 differ,
  mostly shifted register names and the 4-byte `p += 4` vs `p++` choice in case 0xA. A switch on cmd+2 (jump table lit_2397).
- Quest_net_sub (switch on quest_w.x181, table lit_3028): 179/205 (a nop in a branch delay slot differs).
- remuneration_item_set: 51/460, only temp register numbers differ (the pick loop uses a1/a2/a3 in another order).
- quest_item_ck2 109/114 (original uses 5 s-registers, mine 6), Item_regained 158/189, stolen_item_stack 145/145
  (original keeps its args in a3/t0 across the call, i.e. it uses an IPA-like "callee clobbers only a few regs"
  schedule that I could not trigger), Share_item_stack 86/127, Net_Share_item_stack 71/117, Share_item_num_ck 61/86,
  em_work_serch 75/86 and em_work_serch2 43/93 (only `slt/bne` vs `bltz` for `x3A >= 0`), Quest_str_get 15/21,
  str_gattai (varargs: the compiler knows `va_start` but I could not get the original's "(8-n)*8" prologue),
  Em_hagi_point_cnt_ck 20/50, station_em_set 12/85, quest_enemy_ck_sub/_sub2 (the original keeps a `beq 0x63; b` pair).
New lessons (function that shows it):
- Loops that scan the same table twice use two separate pairs of locals in the original (station_em_set: i,g for the first loop and
  j,h for the second); declare them all at function level and let tools/permdecl.py find the order (it matched station_em_set and
  Ext_pick_point_set, whose only difference was the order of `i` and `s = StiEM_data`).
- `static` on a leaf callee defined earlier in the same file makes MWCC keep the caller's values in t-registers across the call
  (stolen_item_stack: 145/145 -> 7/138 diffs; stage_spr_disp). A K&R `static s16 f(item) u16 item;` was needed there, a prototype-style
  definition masked the argument at the call site.
- check.py "1/N differ" is NOT always a relocation: quest_failed_ptr_set was a real `addiu a3,8` vs 16 (s16* stride) and
  quest_item_ck a real lhu/lh. Look at `-v` before registering; the rebuild is the final judge.
- A function that is K&R-defined stays unprototyped in the split files, but one with a prototype-style definition
  (stolen_item_stack(int,s16), em_work_serch2(s16,s16)) needs the same prototype at the top of every split file, or
  its callers (Item_stolen) get different argument conversions and the rebuild fails.
- Loops: write `for (;;) { if (e->id < 0) break; ... e++; }` to get the original's test-at-top loop with a back jump
  (Em_direct_set neighbours: enemy_insurance_sub, em_next_tbl_ck, quest_em_init_sub, quest_enemy_ck_sub).
- `while ((v = *p) != 0)` plus `tbl += k; base = *tbl;` (reuse the parameter register): em_data_st_adrs_set.
- Local declaration order decides callee-saved register numbering: the LAST declared local gets s0, the first the highest
  (quest_em_die: e, em, hp -> s2, s1, s0). Block-scoped temporaries keep short-lived values out of s-registers
  (quest_condition_prog frame size).
- `if (x == 0) continue; break;` vs `if (x) break; continue;` emits different branch polarity; the original's
  `bnez give; nop; b next` needs the first form (remuneration_item_set).
- `r = 0xFFFE; r = r & 0xFFFF;` reproduces `ori; andi` for a constant that is later masked (Ext_pick_point_ck2).
- A sparse `switch` with ~25 cases is a compare chain in REVERSE source order, bodies in source order
  (remuneration_item_set); a `switch (x) { case 0: case 1: case 2: ...}` gives `beq 2; beq 1; beqz` where `||` would give
  a sltiu range test (Quest_net_sub).
- struct field used with `lhu` in the asm needs an unsigned type (STIEM.cnt u16), `lh` a signed one.
- unused-argument trick: Item_stolen(pl, item, num) has a first argument that is never read; q_net_send_em_capture calls
  net_send_sys(6, master) although m2c shows one argument.
Shared header edits: include/pl.h (PL_ITEM share[4] at 0x8F4), include/game.h (area_mdlw[10] -> [9] because Item_stolen
stores at game_w+0xCC/0xCE: new fields xCC, xCE; reward_item[16] -> [32] and x1A8/x1AC), include/em.h (x876 u8 at 0x876,
x88D now s8 as proved by lb in Em_hagi_point_cnt_ck).
