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

## Update 5 Oct 2026 (second pass): newly matched, main OK
- stage_m, game3 (see above); f_quest: em_work_serch, ext_pick_point_tbl_clr_ex, quest_enemy_ck_sub, quest_enemy_ck_sub2
  (now in f_questj/m/p, config ranges widened). Game_task: still 690 diffs (a local copy of tsk->step does not change the a1/a0 choice).
- Lesson (slt vs bltz): `if (0 > x)` gives the original `slt at,x,zero; bne`, while `x < 0` gives `bltz`; likewise `if (0 < n)` gives
  `slt at,zero,n; beq`, `n >= 1` gives `blez` (em_work_serch, ext_pick_point_tbl_clr_ex). Write the constant on the left.
- Lesson (`beq X; nop; b Y` pairs): an `if (v == K) {A} else {if (v == k2) {B}}` whose original has the two bodies out of line is a
  one-case switch: `switch (v) { case K: A; break; default: B; }` (quest_enemy_ck_sub/_sub2). Not yet working for quest_em_init_sub2
  (`bne v0,zero; nop; b` for `if (quest_w.no == 0) {} else {...}`; tried switch forms, `;` in the then part).
- Lesson (float add order): a redundant `(f32)(int)(...)` cast changed MWCC's evaluation order (stage_m).
- Lesson: tools/align.py output lines are indented ("   - "); count real diffs with `grep -c '^   [-+]'`. For small files the
  permutation of N independent statements can be brute-forced with align.py (~1 s per try; game3).
- quest_item_ck3, quest_supplies_get now match: reading consecutive u16/s16 fields of a pointer must be written `p++; id = *p; p++; num = *p;`
  (or `*p++`), not `p[1]` / `p += 2` (the compiler then loads both before bumping p; the original bumps in between).
- Still parked: Quest_retire_set (6 instr: `bne; nop` empty delay slot and -1/7 register order; switch form gets the -1 register right but wrong branch),
  Quest_str_get, Em_hagi_point_cnt_ck, quest_em_init_sub2 (see above).

## Assignment 3 (6 Oct): unowned main code from 0x1A0000 (candidate list)
0x1A0000-0x218000 is Sony/CRI/newlib library code (skipped, GCC). 0x22C670-0x22F7A0 AQ/net, 0x22F800-0x24A0F0
network/inet/Ave/mcsls (online; skipped for now). Candidates (vram, bytes of uncovered code):
staff 0x2907C0 (1.6K), movie_* 0x22F7B0 (1.5K), evdemo 0x2862F0 (1.7K), npc 0x23D870 (3.2K), Select_task/omake 0x23A0F0 (7.4K),
mc* memory card 0x27EF60-0x2862F0 (28K, ~135 functions), IME dictionary 0x23E500-0x24A240 (47K, Japanese input, low priority),
f_sound 0x24A250 (41K, sound requests), net file load/save 0x2869A0-0x28BEC0 (21K, online-ish).
Order: staff, movie, evdemo, npc, select/omake, mc.

### staff (0x2907C0-0x290AE0, credits) and movie (0x22F7B0-0x22FDD0, Sofdec wrapper)
- staff: Staff_init + logo_disp built (src/main/staff/staff.c, staffb.c; main OK). Staff_main 2/110 off (original passes `1` in a0 reused from the
  switch compare, mine reloads it in the call delay slot), staff_disp ~75/95 off (register choice; the loop test reloads `e->x`): both in staff_nm.c.
- movie: everything except movie_draw built (src/main/movie/movie.c; movie_nm.c has all). movie_draw 13/96 off: store order of the sprite fields.
- Lessons: (1) a global struct accessed many times in one function wants `SFD_W *w = &sfd_work;` as local to get the original `lui s0` base register
  (movie_server/stop/exit); declare it late (assign after the first calls) if the original materializes it late (movie_start).
  (2) `memset(p, 0, (u32)n)`: the cast changes arg load order (movie_start). (3) a K&R `s8 no` param gives dsll32/dsra32 but the original used the raw
  register: use `int no`. (4) struct-copy loop of 19 words = assignment of a 19-word local struct (movie_server, local frame 0x50).
  (5) END of a `main` range in c_files.txt: use the exact end of the last function (size from symbols), 0x290AE0 vs 0x290AD8 gave MISMATCH.

### evdemo (0x2862F0-0x28699C): 6/6 built (src/main/evdemo/evdemo.c), main OK
Event demo slots (3 per quest): EvDemoInitialize/evdemo_init_sub/EvDemoMove/check000/event000/evdemo_camera_request. The demo tables evdemo_NN stay in
the original data. Lessons: `EVENT_DEMO *e = &event_demo;` local base pointer again gives the original lui s0 (EvDemoInitialize, EvDemoMove);
`*(int *)slot = 0` is the original's word clear of {active, step, timer}; a store that sits in the delay slot after a `jal` in the asm listing
(check.py -v hides nops: look at the other column) was written AFTER the call in the source (event000 case 2).

### omake / mode select (0x23A0F0-0x23BE10): 14 of 20 built (omakeb/c/d/e.c, rodata lit_727_0036D1E0), main OK
Built: Select_task, csub00, ck_start_sw, init_mode_sel, sel_sel_sub, mode_sel_end, mode_sel_exit, Sel_csr_disp, omake_check, omake_init, omake_main,
omake_play, omake_exit, Omake_task. Near-match (omake_nm.c holds all 20 in address order): mode_sel 182/206 (nested sel compares: original keeps
the constant 1 in v0 not a0, block layout differs), key_rept_du (gp-relative key_timer/key_wait: check.py cannot verify, tried as KT{on,cnt}[2] struct,
31/60 off), disp_mode_menu 223/248, Sel_menu_disp 144/206 and Sel_back_disp 14/36 (original keeps &spr.field addresses in registers = separate local
variables, not a struct), disp_omake_menu 153/164.
Lessons:
- check.py masks relocation ADDENDS: `Psw[4]` vs `Psw[2]` looked identical. Psw = 0x3F3710: Psw[0] = held, Psw[2] = pressed (byte 4); only the rebuild
  catches it (omake_play case 2 needed Psw[0]). (staff_nm.c had the same mistake: fixed to Psw[2].)
- `r = 0; if (c) r |= 1;` gives `ori v0,v0,1` (ck_start_sw); `if (--x <= 0)` on an s16 gives the dsll32/dsra32 re-sign-extension (csub00);
  hoisted locals `int held = Psw[0]; int st = tsk->step; int push = Psw[2];` give the lhu/lbu/lhu order and leave `st` in a3 (Select_task);
  `u8 Fade_busy_ck();` prototype gives the andi 0xFF; a K&R `u16 a; int k = a & 0xFFFF;` keeps both masks (omake_main);
  `x > 1` instead of `x >= 2` gives `slti at` (omake_play); extra call args that only look like args in m2c (decide_se(1,3)) are stale registers.
- functions whose K&R header + params take >= 6 lines are not found by tools/split_runs.py: put `s16 x, y, w, h;` on one line.
- globals sized <= 8 bytes (key_wait, key_timer) are gp-relative (`addiu v1,gp,-17760`): declare them with their real size (`s16 key_wait[2]`).

### mc save image helpers (0x2814E0-0x281C00): 9 of 12 built (src/main/mc/mcsaveb/c/d.c), main OK
Written in mcsave_nm.c (all 12). Built: check_sum_set/ck, mc_copy_opt_only, mc_copy_patch, user_data_clr, User_data_init, save_data_sub, card_data_init.
Near-match: encode_data_002814E0 (38/48; original keeps buf in s0, advances it in place and has the checksum pointer in t0), decode_data (11/42;
local declaration order found by permuting: sum? see the file), user_data_copy2 (35/51; original reads data_load_ptr before the `if` and
keeps the raw slot in s1), decode_to_ck matches ONLY with `static decode_data` earlier in the file (the compiler then knows a0 survives the call), so
it is not linked. Save image layout is described at the top of mcsave_nm.c (scrambled u16 stream, key = key*0xB0 % 65363).

### npc (0x23D870-0x23E500): 8 of 10 built (npcb.c, npcc.c), main OK
NPC = a player-like work block (struct NPCW in npc_nm.c; fields named only as far as used). Built: npc_init_sub, npc_init, npc_die, npc_erase, npc_mv, npc_chr_sub,
npc_mk, npc_effect_move (npc_effect_move = prog->init2-style call through the table at +0x3CC; the init writes game overlay function func_53A190, which check.py
cannot verify: the rebuild did). Near-match: npc_trans 149/152 (too many live values: the original uses 7 s-registers, mine 9; locals order rad,m3,m4,m2 gave
the right stack offsets), npc_move 193/234 (original hoists `lbu kind` into the delay slot of the first branch).
Lessons: (1) raw `*(u8*)((u8*)p+off)` accessors on a parameter pointer change the codegen too (address CSE: `addiu v1,s0,764; sb v0,0(v1)`): a local view struct
with named fields at the right offsets fixes it (npc_chr_sub 6/44 -> OK). Generate the struct from an offset table with padding. (2) assigning an integer
bit pattern to an f32 field converts it: write 2.0f/1.0f (npc_init). (3) pointer tables of u16 are walked with `q++` (2 bytes), a `[][2]` s16 table gives
`sll 2; lh` (npc_init). (4) a struct field 0x18 that is a pointer covers 0x18-0x1B: check overlapping field offsets before building a view struct.
(5) the first call arg of K&R-declared callees may carry stale registers in m2c output (npc_init_sub(p, 1) was really npc_init_sub(p)); a 5th pl_chr_set arg (t0) is real.

### mc low level (0x27EF60-0x27FDF0): 13 of 15 built (mclowb/d/c.c), main OK
PS2 memory card step machines (MCW work struct in mclow_nm.c). Built: MemcardInit, McReadClock, mc_sync, mc_check_file, mc_read_file, mc_mkdir, mc_create_file,
mc_write_file, mc_attr_file, mc_format, mc_unformat, mc_get_dir. Not written yet: mc_check_card (0x27F1C0). Near-match: mc_delete_dir (58/116, block layout of the
shared error exit). Lessons: (1) m2c drops trailing call args: sceMcGetDir takes 6 (port,0,path,0,1,table), sceMcSetFileInfo 5; check the asm for t0/t1 setup, and look
at which symbol the last arg is (mc_attr_file passes info_attr, the others mc_dir: the rebuild caught it, check.py cannot). (2) the original `default: return -1;`
reached from several exits = `break;` in every case and one `return -1;` after the switch; the shared error block lives INSIDE case 0 as a label (`err:`)
and later cases `goto err` (mc_read_file/mc_write_file/mc_create_file). (3) <=8 byte globals are gp-relative: declare `u8 keep_rtc[8]`. (4) the weekday formula:
`(day + (year + year/4 - year/100 + year/400 + (mon*13+8)/5)) % 7` with `u16 year` (McReadClock, found by trying ~20 parenthesisations with a loop).
(5) Beware overlapping struct fields when sizing arrays: state[3]/info[3], not [4].

### Assignment 4 (this pass): memory card UI, McAct layer, player sound script, net file code

New lessons (function that shows it):
- `mc_r_no_set` must be `static` and defined before its callers (CardAtld00/01, CardOptsv00/03 only match then: the compiler keeps a0/a1
  alive across the call and even reuses the constant argument register for the following store, `w->msg = 5`). Because of that the Card*
  functions cannot be linked until EVERY function between the helper and the callers matches (mc_sel_ck 68/118 and mc_remove_ck are the
  blockers; Atld11/Optsv06/Cmsv03/Cmsv09/Conld03/Onsv102/Onsv104 additionally need `decode_to_ck` defined in the same file).
- A float-first prototype `void Disp_button(float, int, int, int, int)` is needed to get f12 and the integer args right (K&R puts the float in a0/a1 as double).
- ANSI definitions with `s16` params (`mc_mes_disp(int, s16 y, int)`, `mc_ok_ck`) do NOT narrow in the callee and make the CALLER narrow int expressions
  (`mc_mes_disp(...) + 0x12` passed as y); K&R definitions narrow in the callee. `flfntLocate(s16, s16)` prototype gives raw s16 locals without re-extension.
- Case bodies in a `switch` are laid out in SOURCE order, the compare chain in reverse order: `case 2` before `case 1` in the dispatcher source
  reproduces a jump table whose labels are not monotonic (CardCmsv/CardOfsv0); a `default:` that shares the body of the last case is written
  `case N: default:`; `switch (x) {default: ...; case 5: ...; case 6: ...; case 0: break;}` reproduces `beq 0; beq 6; beq 5; <default code first>` (trans_card_0).
- `if (a) {x; break} else {y; break}` style: `if (xA0 == 0) {step++;} else {step += 2; break;} case 0xB:` (fall-through from an if) gives the original layout (mc_act_save).
- `switch (op) {case 6: case 7: case 8: call(); break;}` stops MWCC from turning `==8||==7||==6` into a range check (McOperationSet); keep the
  `&trans_card_0` address in a local so the lui/addiu is shared.
- `(s16)(timer-1)` pattern: `t = w->timer - 1; w->timer = t; if ((s16)t <= 0)` (CardCmsv01).
- mc_act_* with a single `case 0:` switch (`switch (w->astep) {case 0: ...}`) gives the `beq; b end` pair of the original (mc_act_format).
- Calls to a function with fewer args than its definition (mc_sel_ck 4 args, 5th = stale t0) cannot be written when a prototype is in scope;
  the C uses 0 for the missing arg (near-matches CardOptsv02/Cmsv01/Ofsv001/Conld01).

What was done in this pass:
- McAct* layer (0x27FDF0-0x280EF0), include/mcw.h (MCW work struct, MCFILE tables). Built: mcactb/c/d/e.c (24 of 27 functions, main OK).
  Near-matches in mcact_nm.c: mc_act_save (4 instr: addu operand order of f+slot*16), mc_act_unformat (original calls mc_unformat() without
  args and keeps a0 alive), McActAvailSet (50/63, register allocation). mc_check_card is in mclow_nm.c (29/142).
- disp_savesel* (0x280EF0): mcdisp_nm.c, near-match (font_print_ex takes extra printf args: slot number, name, sex string, play time h:m).
- Card screens (0x281BC0-0x2860D0): src/main/mc/mccard_nm.c, ALL 87 functions written (CARDW struct documented at the top): 63 match
  instruction for instruction, the rest are near-matches (listed in the commit message / by tools/check.py). NOT linked (see the lesson above).
  McCardOperation(op) 2 instr off. Messages ids are numbers; the flow is: Atld = auto load, Optsv = options save, Cmsv = common save,
  Conld = continue load, Onsv1/Ofsv0 = online/offline save, Easysv = easy save; results of McActResult: 0 ok, -255 no card, -254 unformatted,
  -253 no file, -252 not enough space, -256/-251 other errors.
- Player sound wrappers (0x24A240-0x24A790): src/main/pl/pl_snd_nm.c. sound_call*/yoroi_sd_req/move_default/pl_local_init match; wall_sd_req
  (48/73, induction variable layout), ashi_sd_req (static, unused: not checkable), ashi_eft_req (jump table lit_178_0036E0C0) near-match.
- ef_move_sub_0024A790 (0x9B50 bytes, the per-motion sound/effect script of the player: ~140 motions x footstep/armor/sound_call calls, then a
  switch on pl->kind with weapon motions 1002..1427): written by a generator script from the disassembly (case bodies are regular), then
  hand-fixed (Code_Make with unset register arguments is written `Code_Make(STALE...)`, STALE = -1 = no sound). Same TU as the wrappers.
- Net file code (0x2869A0-0x28BEC0, online-only): netfile_nm.c (NetFileLoad by hand, 3124 of 3136 bytes) and netfile2_nm.c (everything else,
  cleaned m2c output; sizes within 1% of the original). Not linked.
- Method for the long tail: `tools/draft.py` (m2c) + a cleaner (types, remove stray args, `(s64)..<<0x30>>0x30` -> `(s16)`), then compile with
  tools/check.py. m2c's pointer increments on typed pointers are in BYTES: rewrite them with a (u8 *) cast.

### Status of assignment 3 (end of this pass) and what is left
Done (built, main OK): staff (2/4 functions), movie (8/9), evdemo (6/6), omake/mode select (14/20), mc save helpers (9/12), npc (8/10), mc low level (13/15).
Written but not built (near-match files): staff_nm.c, movie_nm.c (movie_draw), omake_nm.c (6 functions), mcsave_nm.c, npc_nm.c (npc_trans, npc_move), mclow_nm.c (mc_delete_dir).
Not started (in order of usefulness): McAct*/mc_act_* (0x27FDF0-0x280EF0, drafts via tools/draft.py work, jump table mc_act_jmp), disp_savesel* (0x280EF0),
mc_*_ck and trans_card_0 (0x281BC0-0x2822B0), the ~90 CardAtld/CardOptsv/CardCmsv/CardConld/CardOnsv/CardOfsv/CardEasysv step functions (0x2822B0-0x2860D0,
mostly 100-400 bytes each), net file load/save (0x2869A0-0x28BEC0), player sound wrappers sound_call*/wall_sd_req/ashi_sd_req/yoroi_sd_req (0x24A2A0-0x24A790, 1.3K,
frame_check takes a float in f12), the IME/dictionary engine (0x23E500-0x24A240, 258 functions, low value for the port), and all network code (0x22C670-0x23A0F0, 0x22F800
on: AQ, Ave, mcsls, Inet; Sony/Capcom online stack, skipped on purpose because the port has no online mode).
Library code 0x1A0000-0x218000 (newlib, libm, Sony sce*, CRI Sofdec/ADX) is GCC-built: not matchable with MWCC.
Check list when continuing: always run `tools/rebuild.sh main` after registering: check.py masks relocation addends (wrong Psw index, wrong table symbol, gp-relative
globals) and absolute calls into other modules.

## Update 5 Oct 2026 (third pass): IME engine, memory card chain

### IME / dictionary engine (0x23E500-0x24A240), src/main/ime/ime_nm.c: all 264 functions written, 142 linked
"Ask" Japanese input method used by the name entry (kana to kanji: roman input, bunsetu segmentation, candidate lists, dictionary
pages read from disc through FAskRom_*, learning). Source of truth is src/main/ime/ime_nm.c (every function, address order, brace on its
own line); the matching runs are extracted into ime<letters>.c (imeb.c .. imeau.c) and registered in config/c_files.txt by
`python3 tools/relink_runs.py src/main/ime/ime_nm.c src/main/ime/ime 23E500` followed by `tools/rebuild.sh main` (main OK, 47 runs).
Data structures (all named by offset, guesses): HCHAR (28-byte edit character, hchar[80]), BS (bunsetu candidate), KH (kanji candidate
chain), CH (dictionary entry hit), PWM/KL (temporary lists), PAGE (dictionary page cache, 10 pages of 0x400), ENTID, NODE (temp word hash),
SYNR/SRCH (search results), WD (word record). Dictionary entry format: u16 little-endian length, u8 key length, key, then word records
(attr, rtime, kind[, extra], kanji bytes). api_* functions take a pointer to the request body; the command id is a[-1]
(api_funcent dispatches through the table D_0034ABEC).
Not matching yet (near-matches, logic believed complete): the long ones (henkan, ch_check, setu_point, josi_match, to_roman, set_num,
trans_roman, dic_snssyn/main_snssyn, pword_list ...) and many small ones that differ by one scheduling detail (see tools/check.py -v).
Lessons (function that shows it):
- `slti at,x,K; bne at` (the compare lands in `at`) is what MWCC emits for `x <= K-1` / `x > K-1`; `slti v0` is `x < K` / `x >= K`.
  m2c always prints `< K`/`>= K`, so when the original has `at`, write `<=`/`>` with K-1 (next_wd: `(int)(*p) <= 0x38`, McCardOperation
  `w->rno > 1`, CardCmsv04 `edit_w[1] > 2`).
- `if ((b = f()) == 0) return -1;` (assignment inside the condition) gives the original `bne v0,..; daddu s3,v0` (bs_check).
- `while (n-- != 0) { ... }` is the original shape of the count-down loops (take_kouho); `-(x != N)` in m2c output is really
  `if (x != N) return -1; return 0;` (write_temp, write_page, read_index, 3 functions fixed by that).
- Loop-invariant `if (p < end) { do {...} while (p < end); }` is how MWCC compiles `while`; the shape with the exit test at the top is
  `for (;;) { n = k->next; if (n == 0) break; k = n; }` (kh_endof).
- Walking records: keep ONE pointer and advance it in place (`p[1] = 0; p += 2; if (*p < 0xC) p++; p = next_wd(p, end);`) instead of a
  second `q = p + 2` variable (clear_rtime, max_rtime).
- Loads of the arguments of an `int *a` request: the compiler emits them in the REVERSE order of the source statements: write `p = a[0];
  q = a[1];` to get `lw 4(a0)` first (api_khshort, api_movekh, api_moveblk, api_khhenkan, api_henkan).
- Using the unmodified parameter later (`return srch_ucode(x)` after `c = x & 0xFFFF`) keeps the register (to_ucode).
- ANSI `u16` second parameter gives `andi 0xFFFF` at the use (ext_jis); `int ret` of a u8-looking function: do not cast (`return m;`).
- check.py judges the whole nm file, but callees defined EARLIER in the same file change the callers' register allocation, so a function
  can be OK in the nm file and wrong in its own run file (tmp_getsyn: hashfunc defined above it). tools/relink_runs.py checks every run
  file separately and drops those functions; always use it (and rebuild) instead of genruns alone. genruns/split_runs now accept K&R
  heads up to 12 lines and two-letter run suffixes (GENRUNS_SKIP env var = names to treat as not matching).
- Names with only a symbol difference in check.py (`calls encode_data_002814E0, original calls encode_data`, `func_534650`) are fine for the
  rebuild; they count as 1 differing instruction.

### Memory card chain: one translation unit 0x2814E0-0x2862F0
The save helpers (0x2814E0-0x281C00), mc_* UI helpers, all 90 Card* step functions and McCardOperation are ONE original source file:
decode_data, decode_to_ck and mc_r_no_set are LOCAL (static) in the symbol table. The Card* callers only match when `decode_to_ck` and
`mc_r_no_set` are `static` and defined above them in the SAME file (the compiler then knows a0 survives the call), so the region can
only be linked as a single C file in which EVERY function matches. src/main/mc/mccomb_nm.c is the experiment (mcsave_nm.c + mccard_nm.c
concatenated, static decode_to_ck): all functions match except mc_sel_ck (68/118, see below) and five that only differ by symbol name
(CardOptsv08, CardCmsv08, CardOnsv103, CardOfsv008, CardEasysv01: fine for the rebuild).
This pass: mc_remove_ck, encode_data, decode_data, user_data_copy2 now match (mcsaveb.c now links 0x2814E0-0x281740 with encode_data,
static decode_data, check_sum_*, decode_to_ck: it must become part of the big file later). Fixes found on the way: a stale-register call
`mc_sel_ck(w,221,136,&port)` with an unset t0 is really `..., 1)` when a constant 1 is already in t0 for a compare (CardOptsv02, Cmsv01,
Conld01, Ofsv001); `t = w->timer - 1; w->timer = t; if ((s16)t <= 0)` (Optsv02); user_data_copy2 needs `(u8)slot` in the offset but
`(slot & 0xFF)` in the shift; `seek_dic` is K&R so a 64-bit argument is passed unchanged (read_page: `((s64)p->id << 10) + 0x3400`).
mc_sel_ck: tried declaration order, K&R/ANSI, int vs s16 params, an explicit/implicit y0, for/do loops. The original keeps hide in s7, the
y+18 value in fp and w,x,sel,y in s3..s0 (y shares s0 with the loop counter); mine puts y first. Permuter: tools/perm.py cannot read files
with K&R definitions (it turns them into declarations), so run it on a small standalone file (header + the one ANSI function).
- mc_sel_ck update: the permuter found `w->csr[1] = (y0 = y1) + 0x24;` (68 -> 35 of 116 differing); now only the callee-saved register
  assignment differs (original: y0 in fp, hide in s7, w/x/sel/y in s3..s0). A 25 minute permuter run on top of that found nothing better.
  Once mc_sel_ck matches, build ONE run file from src/main/mc/mccomb_nm.c (regenerate: mcsave_nm.c + mccard_nm.c, static decode_to_ck)
  for 0x2814E0-0x2862F0 and drop the mcsaveb/c/d lines from config/c_files.txt.
- Tools added: tools/relink_runs.py (verify runs per file + rewrite config), GENRUNS_SKIP / GENRUNS_KEEP_STATIC in genruns.py. Comparison form
  brute force (`<`/`<=`, `>=`/`>` with K+-1) found to_ucode; a LOCAL helper may only stay `static` in a run file when ALL its callers are C in the
  same run (ins_bsmem, exist_kouho); the others (getbit, kh_append...) are called from asm and give undefined references.

### Session notes (mc chain linked, IME small near-matches)
- Memory card chain: mc_sel_ck still 35/116 (the old permuter run in /tmp/perm_sel.log never beat base score 255; no permuter is running).
  Extra tries (no y0 variable, loop counter = y) were worse. The rest of 0x2814E0-0x2862F0 IS linked now: 21 run files src/main/mc/mccombb..v
  (from mccomb_nm.c via `RELINK_KEEP=decode_data python3 tools/relink_runs.py src/main/mc/mccomb_nm.c src/main/mc/mccomb 2814E0 b`),
  plus the jump tables of trans_card_0, CardAtld, CardOptsv, CardCmsv, CardConld, CardOnsv1, CardOfsv, CardOfsv (main:rodata lines found with
  a small script scanning lui/addiu pairs; relink_runs/genruns do NOT write main:rodata lines, add them by hand). 20 Card* steps that need
  `static mc_r_no_set`/`decode_to_ck` in the same file as the callers stay asm (Atld01/11, Cmsv00/02/03/05/09, Conld00/03, Ofsv000/002/003/005/009,
  Onsv102/104/106, Optsv00/03/06). They link only once mc_sel_ck matches (one big file).
- IME matches this pass (now linked): is_shift (`u8 lo = c` BEFORE the call), tmp_touroku (`alloc_record(need = newwdlen(w))`), FAskRom_Write (the
  original memcpy's `n`, not `len`: an original bug), kh_mergesort (`if ((k = null_kouho(..)) != 0)`), is_kata (K&R `u16 c`),
  to_zenkaku_spec (`(*k & 0xFF) == (c & 0xFF)`, `(u16)c & 0x100`), isnum (`for (v = *p; v != 0; v = *++p) if ((v & 0xFF) < 0x30 || (v & 0xFF) > 0x39)`).
- Tried without success (parked): calc_pulen (empty-if layout), kstrncpy (sltu/xori on *src), not_bhead (return layout), srch_ucode, change_kind
  (original hoists the shifted kind out of the loop), free_entid_tab (sign-extended index), clear_allrtime. The bigger ones (henkan 74 real diffs,
  ch_check 219, setu_point 74, josi_match 67, to_roman 99, set_num 61) were not attempted.
- Tool pitfall: a variant-testing script that rewrites the source file must compute the new text BEFORE opening the file for writing.

### Session notes (mc chain fully linked; IME retries)
- Memory card chain 0x2814E0-0x2862E4 is now ONE C file, src/main/mc/mccomb.c (rodata slot 0x384EF0-0x38506C), all 90 Card* steps linked.
  mc_sel_ck still does not match as C (best near-match 31/116 in mccomb_nm.c: only the s0-s4 allocation differs; the original shares y's
  register with the loop counter i and keeps y1 separate in s4; ~30 min of variants: chain assignment `y1 = y0 = y + 0x12`, statement orders,
  for/do/while, declaration order permutations, none moved it). What worked instead: a new INCLUDE_ASM equivalent. config/c_rawfuncs.txt
  (`main VRAM SIZE NAME`) makes tools/build.py write build/raw/NAME.inc (.word lines of the ORIGINAL bytes read from disc/, never committed)
  and mccomb.c has `asm int mc_sel_ck(...) { #include "mc_sel_ck.inc" }`. mwccps2 accepts `.word` inside `asm` functions. Raw words need no
  relocations because the link is byte-identical. tools/check.py does not generate the .inc: run tools/build.py (or rebuild.sh) first if you
  compile mccomb.c by hand. To un-asm it later, replace the asm body with C and delete the c_rawfuncs.txt line.
- IME matches this pass (linked): srch_ucode (`*(u16*)p > (u16)code` as the break test, no `c` variable), calc_pulen (`if (key[n]==0) return n;
  return n + 1;`), change_kind (`u16 k = (kind & 0xFFFF) << 12` hoisted out of the loop), free_entid_tab (`s64 i = (int)id; &entid_tab[i]`).
- Closer but not matching: henkan 97/260 (sel/cur are `int`, `bs_prefer` returns `int` NOT s16, top-level test is `mode != 3 || ikkatsu == 0`,
  first loop must be do/while; rest is register allocation of h/cur/len in the second loop, original h=s1 cur=s5 len=s0),
  set_num 42/139 (K&R head with `s16 n`, direct `num_chars[k][d*2]` indexing (reloads the table pointer after each store), `n > 13`,
  `(u32)(g-1) > 1`, `r / 4`, k chosen with `if (d <= 0) k=1; else if (d < 4) k=kind-1; else k=1`), kstrncpy 3/47 (`while (*src && n > 0)`;
  original loads *src straight into a0 for is_kanji), not_bhead 7/50 (layout only), setu_point (original: u8 `a`, `p->id == (u32)-1`
  is addiu+dsrl32 not ori/dsll/ori), josi_match, ch_check, to_roman untouched beyond a look (all control-flow/allocation differences).
- tools/relink_runs.py takes 3-4 minutes on ime_nm.c; run it in the background.

### Session notes (IME leftovers, yn overlay 11.5% -> about 55%)
Workflow that worked (yn): scratch copy of an nm file with only the target function (tools/check.py runs in under a second on it), try several
source forms in one go, `tools/align.py FILE FUNC` for the real differences, then link as a small run file `src/yn/ncNN.c/uiNN.c`
(`yn START END name` in config/c_files.txt, END = START + size from check.py; one `yn:rodata START END name` line per jump table, the table symbol
sizes are in config/symbols/yn.txt). `tools/rebuild.sh yn` takes about 30 s and must print OK. New near-match drafts for yn live in
src/yn/ui2_nm.c (UI text/draw functions, own header) next to ui_nm.c and netcnf_nm.c; the linked run files are generated from them.
IME (src/main/ime): kstrncpy (`int c; while ((c = *src) && n > 0) ... is_kanji(c)`), not_bhead (it is a `switch (c & 0xFF)` with 13 case labels
returning 1; the ladder of beq in the asm was a switch, not an if chain). Still near-match: set_num (42/139: the k selection layout; original has
`k = 1` hoisted into the delay slot of the first branch and keeps kind-1 in the delay slot of the second), henkan, ch_check (u32 n, u32 e, `*(s8 *)p`
instead of a char variable, register assignment differs: orig pos=s5 end=s0 n=s3 p=s2 h=s4 kind=s6 k=s1), setu_point (u8 a/c/t, `p->id == (u32)-1`),
josi_match (c in v1/k in t0 in the original; a1/d reloaded with andi at each use), to_roman. A 15 minute permuter run on josi_match only found
cosmetic rewrites (best 435 of base 1115, not a match).
yn lessons (each shown by the named function):
- A `switch` whose ladder leaves a stray `addiu reg,zero,N` (a dead constant load) has an extra `case N:` merged with `default:` in the source:
  yn_netcnf_dev_to_work, yn_netcnf_work_to_dev (`case 0: default:`), yn_setup_allwork (`case 1: default:`).
- Strength reduction: write `arg0 + i * 0x1340 + K` indexing and let MWCC build the pointer induction variables (net_allload, search_usr_name);
  hand-written running pointers give different registers.
- Big offsets (> 0x7FFF) from a work pointer: use a struct with the field at that offset (`NCW`, `IFCW`, `WRKW` in netcnf_nm.c); M2C_FIELD with
  the same offset makes lui/ori/addu instead of lui/addu/lw.
- Early returns that share the epilogue: `break` out of the switch and `return 0;` after it (file_search, set_main); `||` for the two button tests
  (`if ((pad & 0x20) || (pad & 0x40))`) gives the shared block that m2c prints as a goto.
- `-(a != b)` is `if (a != b) return -1; return 0;` (pastproxy_check). A copy of a 520 byte block is a struct assignment of `struct { s32 w[130]; }`.
- A 16-bit compare of an int local needs the ints, not s64: write m2c's `(s64)(x << 0x30) >> 0x30` as `(s16)x`, keep the loop counter an `int` and
  cast with `(s8)(i + 1)` where m2c shows the 0x38 shifts (hard_more_font_sub, help_font).
- Table switch vs ladder: yn_button_font only became a jump table once the table pointer was a local (`char **tbl = yn_button_mes_tbl`).
- Locals that are not used must still be sized to match the frame: ifc/dev/name in pastdata_check (`struct { IFCW x; u8 pad[0x30]; }`, `name[0x200]`).
- yn_center_x returns s16 in its own file (uc00.c) but its callers do not sign-extend, so every other file declares it `int`.
- The yn SCE library functions (sceNetcnfif*, sce_*) were built by gcc (sd/ld saves, absolute addressing): they cannot match with MWCC.
Linked in yn: netcnf (init1/exit, pastproxy, ip_check, set_current, get_num, get_list, net_allload, magicno_check_sub, ip_to_num, search_usr_name,
setup_devwork, work_to_dev, dev_to_work, work_to_ifc, pastdata_check), ui (title_font, backup_allwork, proxy_wk_load/save, center_x, strconv, hard_more/
prname/adname font subs, dialog_draw, shot_cancel, button_draw/font, message_font_sub, hard_font_sub, memcard_font_sub, dialog_font_sub/without_yesno/
memcard/ip_sub/font/setting, help_font, id_pw_font_sub, ipadrs_font_sub, keyboard_init, setup_allwork, file_search, set_main).
Near-match / open in yn (check.py differing instructions): message_font 23/170 (arg0 pointer lands in a0 instead of a1), port_font_sub 51/151
(registers), dialog_font_once 9/29 (arg0*8 register), connect_font_sub 4/141 (operand order of one addu), sprite_draw_sub (argument evaluation order;
the draft in ui2_nm.c is complete), strconv2 45/78 (the third byte copy is not merged in the original), utf8_to_sjis/sjis_to_utf8 (~45 diffs, registers),
module_load/unload (empty loops with 8 nops in the original), mc_device_check_all, auto_connect (no draft), sprite_draw/sprite_draw_each (no draft),
select_provider (5.5 KB; the m2c draft keeps `ynw` in callee-saved temps, the original reloads it everywhere: replace every `temp = ynw` alias by `ynw`
and rewrite case by case).

### Session notes (yn / select, 5 Oct 2026, second pass)
Linked this pass (each checked with check.py and `tools/rebuild.sh` printing OK for all five modules):
- select: edit_pl_init_new (edit08.c, 0x534820-0x5349CC) and edit_pl_init (edit09.c, 0x534A80-0x534C20). Remaining select asm: disp_edit_spr, disp_color, Edit_task, Cont_task, cmn_mongon_check_sub, cmn_mongon_set.
- yn: yn_sprite_draw (ui30.c, 0x537770-0x538114, jump table lit_3823 at 0x540BE0), yn_auto_connect (ui31.c, 0x535DC0-0x5360D8,
  jump table lit_664 at 0x540B70).
Shared header edit: include/select.h `edit_top[]` -> `edit_top[2]` (the symbol is 8 bytes; a sized array makes MWCC use gp-relative
sdata access, which the original has).
Lessons (function that shows it):
- Position table read through three symbols: edit_pl_init(_new) reads `stage_start_pos` x/y/z as `stage_start_pos[stage*3]`,
  `D_2F2624[stage*3]`, `D_2F2628[stage*3]` (the two extra names are the auto-generated undefined symbols; declare them `extern f32 X[];`).
- `EDIT_W *e = &edit_w;` as a local makes MWCC hoist the base the way the original does (edit_pl_init_new), a bare `&edit_w + 4` does not.
- A call that passes fewer arguments than m2c shows: pl_create_model(id) and weapon_create_model(a, b, 0) take exactly what the original
  loads; extra stage*12 arguments were guesses of m2c.
- switch(x) { case 1: A; default: B; case -2: ...; case 0: ... } where case 1 falls into the default body when its test fails and the
  tests of the other cases are laid out AFTER case 1's body is an if/else-if chain in C: `if (r == 1) { ...; if (ok) break; }
  else if (r == -2) return -2; else if (r == 0) break;` followed by the default body (yn_auto_connect, yn_select_provider case 18).
- `x >= 9` vs `x <= 8` flips slt into `at` (yn_select_provider: `(v >= 6 && v <= 8)`), a `(cond) ? 4 : 2` assigned to a field gets the
  delay-slot constant load the original has where an `if` + local does not (select_provider case 1).
- Operand order of `index*20 + base`: a named int local (`off = b * 0x14; M2C_FIELD(off + (int)ynw, ...)`) gives `addu idx, base`;
  an inline expression does not (auto_connect, select_provider case 18).
- Per-case local `u16 pad;` in a big switch gives the register choices of the original better than one function-wide variable.
- m2c's `yn_cur2_sd(ptr + 0xD)`: read-modify-write of a field and pass its address: `t = ynw; p = t + o; *p = (t[o] + n) % n; call(p)`.
- yn_sprite_draw: compares `x > 2` / `x > 3` (not >= 3 / >= 4) and the inner switch cases listed 0, 1, 2 (ladder tests in reverse).
Not linked (near-match, own notes):
- yn_select_provider (5536 bytes): full C in src/yn/ui3_nm.c, 216 of 1384 instructions differ, no structural difference left. What
  differs is register choice (case 3 loop pointer a1/a2, case 4 hoisted masks a0/t0/a2/a3, case 12 pad in a1 and the digit loop) and
  the delay slot of the -2 compare after yn_mc_gmfile_check/save (the original copies the result to s0 in the delay slot).
- yn_sprite_draw_each (draft in ui2_nm.c, 96 of 152) and yn_sprite_draw_sub (needs it): frame size and the stack block layout
  (rect at +0xA0, col at +0xA8, uv at +0xAC) are right; the s16 loads into v1/v0/a3/a2 and the u1/v1 temporaries take other registers.
  declbf over the six s16 temporaries did not help (best 94).
- yn_dialog_font_once (9/29): the original builds `sll arg0*8` before loading ynw and the table address; tried local table pointer,
  `&((YMSG *)tbl[i])[arg0]`, an `off` local: all keep the order below.
- yn_connect_font_sub stays at 4 differing instructions (register of the ynw load before `lb 12(...)` and one addu operand order).
- yn_utf8_to_sjis 7 of 76 (u16 code, `code = src[1] << 8; src += 2; code += *src; src += 1;` is the form that reproduces the lazy
  pointer increments; the original masks after the add, not after the shift, and keeps hi bits in the same register as the byte).
- module_load/unload: `asm { nop; ... }` inside the loops compiles but the loop is not rotated like the original (the original is
  `b test; nop*8; test: call; bltz body`), skipped. The gcc-built sce* functions in yn stay asm (0x53B2E8 on, see config/symbols/yn.txt).
- select: disp_edit_spr 123 of 214 after int i / s16 y, the `case 3: w[6]; case 4: w[7]; case 5: colour` order and `*(u32 *)(w + 8)`;
  the original keeps only s0-s3 (task in s0 shared with the menu pointer), mine allocates six. cmn_mongon_set 94/96 (hand-unrolled
  copy loops with separate out/in cursors), disp_color needs `s.x = 96.0f` style float stores (the original converts floats to s16 with
  cvt.w.s for every field) and was not rewritten; Edit_task/Cont_task untouched.

### Session notes (select overlay + yn leftovers, 5 Oct 2026, third pass)
Linked this pass (rebuild.sh OK for all five modules): select cmn_mongon_set (edit10), Edit_task + Cont_task (edit04, now
0x535F30-0x538028 with rodata 0x53B8E0-0x53B964; edit05 was merged into it), disp_edit_spr (edit11, jump table 0x53B660-0x53B678);
yn yn_sprite_draw_each (ui32), yn_dialog_font_once (ui33). select is at about 87%.
Shared-header edits (all proven by matched loads/stores): select.h EDIT_W x3C/x3D are u8 (lbu in Edit_task); SoftKeyboard_move is
`s8 (s8 *, s16, s16)` (lh loads of Psw, s8 return); SEL_W got `xB6` carved out of padding (Edit_task/Cont_task store the slot at
select_w+0xB6); font_print_ex is now `void (s16, s16, int, char *, ...)` (see lessons).
New config file: config/select_aliases.txt (`roll_move = 0x00536470;`) because roll_move is file-static in edit04.c.
Lessons (each shown by the named function):
- A file-static (or just earlier-defined, leaf) callee tells MWCC which registers it clobbers, so the caller keeps loop variables in
  a0/a1 across `roll_move(pl, i)` calls. Edit_task/Cont_task only matched after `static void roll_move` in the same file; asm callers
  then need a `name = addr;` alias in config/<module>_aliases.txt.
- An unprototyped call passes an s16 local after a lazy sign extension and CSEs that extension across calls (extra callee-saved
  register). With a prototype whose parameter is s16 the compiler converts per call and keeps no copy: disp_edit_spr only matched after
  font_print_ex got `s16 x, s16 y`; Edit_task needed `edit_pl_init_new(PLW *, s16, s16)` to pass i raw.
- Repeated reads of a global u16 array (Psw) are re-read, not CSE'd, when stores through u16 fields sit between them: reading through
  `*(volatile u16 *)&Psw[i]` at the top of Edit_task reproduces that (only there; later in the function the original does CSE).
  Operand order of `a == b` follows the load order in the original: `PSWV(1) == PSWV(0)` loaded Psw[1] first.
- check.py ignores both relocation addends and jump-table contents; Edit_task passed check.py with the wrong Psw index and
  disp_edit_spr with its switch cases in the wrong order (cases 5, 3, 4 sit in the order 2, 5, 3, 4 in memory). Always run rebuild.sh.
- `all_model_free(t->step++)` gives the original delay-slot store; `if (++e->x38 >= 0x3C)` gives `andi 0xFFFF; slti` without `at`;
  `A || B` conditions that share one body (cancel) must be written once: `else if (((p & 0x20) && x == 1) || (p & 0x40))`.
- `for (i = 0; i < 2; i++) f(&player_work[i])` instead of a `pl++` pointer swaps which of i/pl gets the lower saved register.
- Declaration order mattered a lot in Cont_task: tools/declhill.py (hill-climb over the declaration order, ~5 min) found the order
  that tools/declbf.py cannot reach with 8 variables. tools/vt.py + tools/mkscratch.py test source variants on one function with the
  other functions as K&R declarations (`DECLS="void f(int, s16)" python3 tools/mkscratch.py nm.c scratch.c FUNC ...`).
  tools/cc.sh compiles and disassembles a file (micro experiments), tools/vtry.py tries whole-function variants.
Near-match left (not linked):
- cmn_mongon_check_sub 136/155: callee-saved allocation differs (mine strength-reduces `flt + pos` into a pointer and needs s7; the
  original recomputes it from sp each outer iteration and uses s0-s6). The rest of the structure (do/while nest, `idx = pos` copy,
  `look(&flt[idx])`, found/c handling) is right.
- disp_color 269/293: contents match the original instruction by instruction in both loops (per-case `s.w`, u32 colour bytes,
  `s.x = 0.8f * fx0` with a float local so the cvt stays; a `flfntLocate(int, s16)` prototype was tried and left out), but the original keeps the first loop's test at the
  bottom with the s16 copy of i computed there and the pointer hoists in an out-of-line preheader after the second loop; mine rotates
  the loop. Not found how to provoke that (while/do forms, int/s16 variants, goto-free).
- yn_utf8_to_sjis and yn_sjis_to_utf8 stay 2 instructions off each (`andi t4, t4, 0xFFFF` is done in place before `sra` in the original,
  in a fresh register in mine; u16/int/u32 code, (u16) casts, `&= 0xFFFF`, separate out variable all tried). The matching shapes are
  `code = (src[1] << 8) + *(src += 2); src += 1;` and three `*dst++` stores.
- yn_sprite_draw_sub 15/119 (scheduling of the first call's two byte loads), yn_connect_font_sub 3/141 (ynw/(i+1) register pair),
  yn_select_provider still 216/1384: the permuter at -j1 managed only 51 iterations in 25 minutes (about 30 s per candidate on the
  5.5 KB function) and found nothing better than the base.

## Assignment 4 (main 0x2862F0 to end of Capcom code)
Code/library map: 0x2862F0-0x293B68 is ALL Capcom code (82 functions, last is reward_itembox); .text ends there. No sce*/adx/sfd/libc
code in this range (that sits below 0x28xxxx in the 0x200000s and in the 0x1A0000-0x1C0000 libc block). After it comes data (0x2E5F00 on).
Layout: evdemo 2862F0 | net file code 2869A0-28BEC0 | net_flps/nb_flps 28BEC0-28C750 | Patch* 28C750-28CC40 | hit 28CC40-2907B4 |
staff 2907C0 | power off 290C60 | reward/result 290E50-293B68.
Linked this pass (rebuild OK x5): hit2.c (whole hit file as one unit, 3 raw holdouts hit_sphr_sphr2 18/64, hit_cap_cap2_m 41/1253, hit_cap_cap3_m 90/945
via config/c_rawfuncs.txt; the split files lost the "static callee" scheduling so hit_cap_sphr2_m and hit_line_sphr2 only match inside one file),
hit_point_cbd (hit3.c), net save code (netfile2c-l: decode/encode_data, mc_bs_chg, check_data_cn_file, Net_Icon_Data_Load, dialog_limit_disp,
SaveGameFileNet2, SaveNetFile, SaveNetFile_ForLobby, SaveNetFileBr, NetAutoLoad), power01 (ps2HddPowerOffSet, PowerOffThread, PowerOffHandler,
Quest_price_return), patch01/02 (PatchInitCS, PatchLoadinDNAS_Init/Main).
Header edits (proven by matched loads): netcw.h gained x7A (s16), x7C (s8, lb), x7D, x8C, x8D[5]; new include/netfile2.h; config/main_aliases.txt `_gp`.
Lessons: (1) a single `case 0:` plus `default:` switch produces the `beq/nop/b` double jump of the original (SaveNetFile_ForLobby case 5, PowerOffThread);
`return` vs `break`, and case order = reverse of the compare ladder, decide everything else. (2) `(int)ptr + 0x12000` loads ptr before the constant (Net_Icon_Data_Load).
(3) `sub++; timer = N;` vs `timer = N; sub++` change which is loaded first; try both (NetAutoLoad, SaveNetFile). (4) `x ^ 1` form `((a & 1) != 0) ^ 1` gives sltu+xori.
(5) hit_point_cbd: store order n[0], n[1], n[2] lets d[2] stay in a register. (6) sceDevctl takes 6 args; `&_gp` gives `addiu v1, gp, 0`.
Near-matches left (not linked): NetFileLoad (netfile2m_nm.c, step in a2 vs a1 cascades), NetFileCreate (m2c only, 6780 B), PatchExecCS (patch03_nm.c),
net_flps0008/0004, nb_flps0009 (not started), staff_disp (75/93, original has a case-0 stub + default path I could not reproduce),
reward_mv 9/351 and reward_key_repeat 29/42 (pointer in a2 vs a0), reward_itembox 115/312, hit_cap_cap2_m/cap3_m, hit_sphr_sphr2.

## Assignment 5: main 0x160000-0x1C0000 (Capcom vs library map, shader packet area linked)
Map (function names from docs/survey/mh1_symbols.csv, MWCC = Capcom, GCC = Sony/newlib/CRI, not matchable):
0x160000-0x16A000 sprite/font/weapon/player/enemy draw code (MWCC, mostly unmatched, see agent-D.md) | 0x16A860-0x16AEC0 fms/groundmat |
0x16AEC0-0x170000 fl clay/DMA/file/texture-from-file (MWCC, 35 functions, 19 KB) | 0x170000-0x175000 fl math (fcv, flmat, flvec, quat, motion) |
0x175000-0x177570 flps00xx sprite prims (GS 64-bit packets) | 0x177570-0x179DD0 render state + texture registers | 0x179DD0-0x17BE20 flPS2SetShaderParam (8 KB nested
switch) | 0x17BE20-0x187C50 shader DMA packet builders flPS2AddMatrix_NNNN (LINKED, see below) | 0x187D10-0x18BD90 texture/palette/VRAM | 0x18BD90-0x18F0B0 flInitialize,
draw buffers, plmem, pad | 0x18F0C0-0x193350 AAN/AMO model+motion conversion | 0x193350-0x195000 FOV clip, TIM2, plXXX | 0x195000-0x1B9E60 Sony libs, newlib, CRI
(GCC, skipped) | 0x1B9EC0-0x1BD660 Capcom net sync (net_send/receive_pl/em/sys/chat/host, mwInit) | 0x1BD670-0x1BDB68 flSfd* | 0x1BDB68 on CRI CFT (GCC).

### flps/fladdm.c (0x17BE20-0x187C50): 78 of 81 flPS2AddMatrix packet builders + 19 VU0 asm helpers, main OK x5
Every flPS2AddMatrix_NNNN builds one DMA packet in the system temp buffer (flPS2GetSystemTmpBuff): header (cnt, n, 0x13000000, 0x01000404, w7), ambient*AMB, optional
fade colour / fog (flFogEnd, 1/(end-start)), matrices (matMul2/matMul), light vectors, palette loops, closing tag `id | 0x15000000`. The packet pointer p is `u32 *`, packet
offsets are written `p + (o >> 2)` (macro PB; byte-offset arithmetic `(u8 *)p + o` changes arg-register order in 002E/002F/0032 etc).
The helpers (flPS2matMul .. PS2SHADER_FLMATRIX_COPY) are hand-written asm in the original, in the SAME file before their callers. Lessons:
- MWCC reads the register writes of a `static asm` function: callers keep values in caller-saved registers across the call (p stays in v0, the previous a1 argument is not
  reloaded: `matMul(PB(0x70), tmp, CLIPPROJ)` after `matMul2(..., tmp, ...)` emits no `addiu a1`) and, when the helper writes v0 or calls something (LIGHTVECP1 etc: jal flmatInvert),
  p is moved to an s-register. `.word` bodies hide this, so the helpers must be real mnemonics. tools/build.py now makes build/raw/NAME.inc as mnemonics at build time from disc/
  when a config/c_rawfuncs.txt line ends in `mn` (VU0 macro instructions stay `.word`, jal targets become symbol names). Never committed.
- tools/check.py now passes -Ibuild/raw (raw functions can be checked).
- A call result used as `p = helper(p + off, ...)` keeps p in v0; a call statement without assignment (return value unused) does not; functions that `return p` need p in s0.
- Local arrays: first-declared is at the HIGHER address (`f32 tmp[16]; f32 v[12];` gives v at sp+0x40, tmp at sp+0x70).
- 64-byte copies of flPS2VIEWPORT/flPS2VIEWPROJ/CLIPPROJ are struct assignments (`*(M64 *)PB(o) = flPS2VIEWPORT;`, M64 = struct of 16 words): MWCC emits the 8x lw/sw loop.
- Two-packet header (AddMatrix_0045): second header at +0xA20 is written interleaved in the m2c order (0xA20 before 0x10, 0xA24 before 0x14, ...).
- flmatMul (non-static, 0x172A30) is called by AddMatrix_0036 instead of the static asm matMul: then a1 IS reloaded for the 2nd call.
- Palette loop: `i = 0; mp = flMATRIX; sp2 = p; dp = p; for (; i < 32; i++) { if (m->flags & (1 << i)) p = matMulNormalize33(dp + A, sp2 + B, mp, M840); mp += 0x40; sp2 += 0x40; dp += 0x30; }`;
  the initialisation order of the induction pointers decides their registers.
- Generated from tools/draft.py (m2c) output with a scratch transpiler (not committed): calls and stores in m2c order, call arguments read from the asm (stale registers = same expression).
Held back as raw (config/c_rawfuncs.txt, not counted by progress.py): flPS2AddMatrix_0002 / _0003 (C in src/main/flps/fladdm_nm.c: 31 of ~190 instructions differ, only the
register numbering of the lights loop: orig counter s0 and pointers s4/s3/s2, mine s4/s3/s2/s1; tried decl order, init order, for/do, scoping) and _000D (toon shader, not written).
flPS2matMulNormalize33 is also referenced from a data table: config/main_aliases.txt keeps its symbol (the helper is static in C).
Small find: flPS2GetPaletteVramBlock (tex_nm.c) matches as `int r; if (h == 1) r = 2; else switch (...) {case 0: case 1: r = 4; break; case 2: r = 4;} return r;` (last case falls out, no jump).

### Network play sync (0x1B9F70-0x1BD660): src/main/net/netsyn01..09*.c, include/netsyn.h (new)
Capcom's online session sync: packets built in a local union of per-kind layouts (`cmd, len, 0, 0, then fields`), queued with AQ_data_put, applied by net_receive_*.
include/netsyn.h holds typed offset views (NPLV player work, NEMV enemy work, NGW game_w, NPSLOT/NEMACT pending slots) and the packet unions NPLPK / NPLRX; it is
GENERATED from an offset table (each view is a struct with u8 padding between the fields) so the types stay exactly what the matched loads show. It does not touch
pl.h/game.h/em.h (no shared header edits).
Linked (rebuild OK): net_send_pl (netsyn01), net_game_w_clear (04), net_plpos_set, net_receive_pl_pos_set, net_emact_set, net_receive_em_act (03), net_send_host (05),
net_send_chat (06), net_send_sys (08); jump tables 0x35EC10-0x35EC34 (send_pl) and 0x35EC70-0x35ECA4 (send_sys) registered.
Near-match, all complete, only register numbering differs (netsyn02_nm.c net_receive_pl 296/390: orig payload pointer in s0 and player in s1, mine the other way; netsyn05_nm.c
net_receive_host 76/108 same pattern; netsyn06_nm.c net_receive_chat 41/114: orig length a2 / text pointer a1 / counter a3; netsyn07_nm.c net_start_ck 244/309: the orig
loop test `if (pl_state[i] != 0xFF) goto next` compiles to `beq body; b next`, not reproduced). Not written: net_send_em (1824), net_receive_em (3672).
Lessons:
- `u8 kind` as an ANSI parameter plus `k = kind;` inside the master check reproduces `andi a0, s1, 0xFF` placed after the Pl_master_ck call (net_send_pl); a local
  `int k` hoisted before the call, or `(kind & 0xFF)`, moves it into an s-register or makes the `sb kind` reuse the masked copy.
- `for (i = 0; i < 2; i++) { if (pl->slot[i].timer == 0) {...} }` with `s8 i` and slots as a struct array in the work block gives the original
  pointer-for-the-test / index-for-the-store pair (net_plpos_set, net_emact_set, net_receive_em_act); do NOT write a separate pointer variable.
- `if (x == a) {..} ` with the same value compared twice: the second compare uses the loaded value, not the constant (net_start_ck `old != pl_state[i]`).
- A global struct read many times in one function is hoisted into an s-register only if you write `u8 *sw = (u8 *)&select_w;` and use `sw[off]` / `*(u16 *)(sw + off)`
  (net_send_sys); `select_w.field` typed access did not hoist.
- tools/declhill.py (declaration order hill-climb) found the register assignment of net_send_sys (pl/sw order); worth running on every near-match with many locals.
- Calls with stale argument registers: `Quest_error_set2()` has no arguments here (m2c invented four), `net_send_sys` takes two.
- Switch case order: ladder is the reverse of source order (`case 1: case 2:` for a ladder 2,1,0).

### Later in assignment 5 (after the net sync pass)
- net_send_em (netsyn10.c, 1824 B) matched on the first full attempt: m2c order + per-kind packet union (struct per kind, union padded to the frame size 0x40) + the flag byte as
  `u8 f; if (x & 4) f |= 1; ...`. Linked 0x1BA8E0-0x1BB000. Unions must be padded up to the original frame (net_send_em/net_send_host/net_send_sys all needed a `pad[]`).
- net_receive_em complete in netsyn11_nm.c (all four kinds; payload read through PU8/PS16/... byte-offset macros on a `u8 *p`): only the s0/s1 swap and load scheduling differ.
  All five net_receive_* functions (pl, host, chat, sys, em) share one symptom: the original puts the payload pointer p = buf + 4 in the LOWER s-register and the incoming buf
  in the higher one (net_receive_host: p=s0, buf=s1, then `s` reuses s1); this build gives buf the lower register. Declaring p last (net_receive_host 76 -> 71 diffs), p first,
  extra/unused parameters, in-place `buf += 4`, 3-parameter prototypes and tools/declhill.py (net_receive_sys, 118 diffs, no change) did not fix it. A future agent could try
  the permuter with a longer budget on net_receive_host (smallest, 432 B).
- fl clay: flPS2CreateClay (`if (shader != -1) {calls} else { return 0; } flClayNum++; return 1;` is the layout) and flReleaseClayHandle (`flPS2DmaTerminate(h)` takes the 1-based handle,
  not the index; `if (h > 0x180)` instead of `>= 0x181` keeps the compare in v0) linked: src/main/fl/clay02.c, clay03.c. Near-matches: flCreateClayHandle (clay01_nm.c, 4 instructions: the two
  independent argument loads of the second flMemcpy come in the other order), flPS2GetMLCLAY (clay02_nm.c, 20/28: s0/s1 roles).
- reward_mv (9 off): permuter 10 min, best score 195 -> 55, no zero; mutations tried by hand (`new_var = w->xB < 0` in the condition, dead `PitMenu.x12 = 0`) do not transfer.

## Assignment 6 (6 Oct): net sync receivers, fl hierarchy and pad layer (main 0x160000-0x1C0000)
Linked this pass (rebuild OK x5 each time, build_pc.sh builds): flpad01/02 (flpad_ram_clear, flPADInitialize/Destroy/WorkClear, padconf_setup_depth,
flupdate_pad_stick_dir/button_data/on_cnt, flPADFixedAnalogSelectSwitch), plpl01 (plplInit/Add/Next), pl_ps2io01/02 (ps2McModuleInit, flPS2PADModuleInit),
tarpad01 (tarPADDestroy, FixedAnalogSelectSwitch, flPADConfigSetACRtoXX, tarPADRead, ps2PADWorkClear), flnode01 (flCalcTrans, flSetSkinTrans,
flSetSkinTransMatrixList), flnode02 (flSetMotionExSub, flFindGroupRoot), flmotion01/02 (flGetMotionSetTime/LoopInfo, flGetMotionMatrix), flnode03/04
(flPlayMotionExSI, flCalcTransSI/Sub), flnode05a-d (hierarchy build: flGetHierarchySI, flGetHierarchy3_sub, flInitPostureHierarchySI/MAYA, flGetMatrixWithoutScale/SI/MAYA),
flps_misc01 (flPS2CheckGSClip), disp2_02 (Disp_button), net_receive_host (netsyn05.c now covers 0x1BCA20-0x1BCCF0) and net_receive_sys (netsyn09.c, 0x1BC200-0x1BC688 + jump table 0x35ECB0-0x35ECE4).
Near-matches left in this pass (all in *_nm.c): flPADConfigSet (10/32), flPADGetALL (flpad03_nm.c, 4/86), ps2McInit (2/34), tarPADInit (tarpad02_nm.c, 68/163),
flSetMatrixList (11/36), flPlayMotionExSISub (3/63), flnode05_nm.c: flGetHierarchy3 (11/74), flInitPostureHierarchySISub/MAYASub (2 each), flGetHierarchyData2 (2),
flGetFcurveValue (7), flPS2psAddQueue (5/43), flCreateClayHandle (4/101), flPS2GetMLCLAY, net_receive_chat (24/114: len/d/tmp registers), net_receive_pl, net_receive_em, net_start_ck, disp_load_msg (2/50), reward_mv (9/351).
Lessons:
- The node tree walk (child at +0xD0, sibling +0xCC, parent +0xC8) is `loop: work; if (n->child) {n = n->child; goto loop;} if (n->sib) {n = n->sib; goto loop;}
  while (n != top) { while (n->sib) {...} n = n->parent; }` written with goto; the walker's own params must be the loop variables (`void f(FLNODE *n, FLNODE *p) { FLNODE *top = n; ...`).
- Tail-recursive list code (plplNext) is a `for (;;)` loop in the original; in-place parameter modification (`dst += last;`) reproduces unfolded adds (flGetHierarchyData2: `p += 0x10; p += i << 6;`).
- struct assignment of two s16 fields copies as lh,lh,sh,sh (flPADConfigSet-style `*dst++ = *src++`), 128-bit lq/sq copies need `unsigned __int128` members.
- `a = p[0];` hoisted into its own declared local (u16) before the compare changes the scheduling of the loads (net_receive_host); the permuter then found
  `(unsigned long)(*(s32 *)(p + 4))` on the |= loads and an extra `int idx = s & 0xFF;` local. Order of statements and one temp each: use tools/perm.py -j1 for 10-15 minutes.
- Statement/declaration permutation scripts (kept out of the repo) beat hand tweaks for register swaps: permute the declaration lines (hill climb with pair swaps) or the
  store block (flps/disp: Disp_button matched when `q.col = -1` moved after the last field store). check.py's count for calls to functions in other modules always shows one
  diff per unresolved call (func_NNNNNN names): that is not a real difference.
- `if (...) return 0; return 1;` is not the same as `return !(...)` for float compares (flPS2CheckGSClip).

## Assignment 7 (6 Oct): single-player first, own ranges plus agent D's parked main ranges
Linked this pass (rebuild OK x5): sound driver host side src/main/sound/sdr02-sdr07, sdr09 (Sdr* queue writers, SIF RPC status calls, makebuff*,
SdrSendReq, sending_req 1036 B), flsnd04 (flSndRequest, flSndChange, flSndStatGet), flsnd05 (flSndModuleInit .. flSndPackLoadStatus), flsnd02 (flSndJointSet),
flsnd06; fl clay01 (flCreateClayHandle), flnode05e (flGetFcurveValue); flfnt07 (flfntSjis2Index); hk18 (hk_key_eisuu); ime runs imeaw..imebd (is_kuten,
alloc_record, set_wds, flush_head, newwdlen, get_entid_tab). Near-matches left: SdrSeReq/SdrSeChg (sdr01_nm.c), sdr_dmaadr_set/SdrDmaLoadReq (sdr08_nm.c),
staff_disp 21 off (down from 75), flGetHierarchy3 5 off, flPS2GetMLCLAY 12 off, reward_mv 9 off, reward_itembox, flGetHierarchyData2 2 off.
Not done: Sofdec/ADX/CRI (skipped as told), wait_alarm (asm, uses ei), net_receive_* (online, last).
Lessons (each shown by the named function):
- memcpy/flMemcpy size parameter must be unsigned: `void flMemcpy(void *, void *, u32)` changes the order in which the two argument loads of consecutive
  calls are scheduled (flCreateClayHandle, flSndJointSet).
- A function whose callee's result is returned in v0 must return it: `int f() { int r = g(); if (r > 0) return r; return h(); }` (flSndPackLoadStatus,
  SdrGetState, flSndOutputMode, flSndPackLoadBG2: a void version gives different branch layout).
- `int & 0xFFFFFF` compiles to dsll32 8 / dsrl32 8; the final command word `(x & 0xFFFFFF) | 0x4A000000` is plain int code (SdrPortStop). An unsigned long
  mask or shifts through `long` do not (they fold or add a sign extension before the sw).
- Globals of 8 bytes or less are gp-relative only when their size is known: `extern int sque_w_idx[];` (unknown size) gives lui/addiu; `volatile` there made
  the compiler re-read it after the byte stores like the original (SdrAllStop, SdrSeReq).
- Separate lui/addiu for every field of a queue entry (`sndque_tbl+4+off`) is `sndque_tbl[idx].field` through a global array of structs, not a pointer; a
  local pointer q gives `4(q)` offsets and is used where the original does (SdrSetRev, sdr_dmaadr_set).
- `*p++ = a; *p++ = b;` (pointer bumped by 2 after pairs of byte stores) vs `p[0]/p[1]` offsets (makebuff_tq, flush_head). Declaration order of 7 locals found with
  tools/declhill.py (makebuff_tq).
- A long nested if ladder with no jump table that tests `x & 0xF0` groups is ONE switch with the cases in reverse ladder order, `case 0x60: case 0x50:` for a
  ladder 0x50, 0x60 (sending_req); a `switch` whose failure paths jump straight to the function end has the shared tail code as a label INSIDE the switch,
  after `default: return;` (hk_key_eisuu).
- A switch with sparse cases compiled as compare ladder where `||` chains of != failed: write the switch (is_kuten).
- `x < K` of an unsigned subtraction: `(u32)(op - 0x39) <= 2` gives the `at` form. `n >= 5` vs `n > 4` the same (makebuff).
- A 64-bit parameter matters: `get_entid_tab(unsigned long id, ...)` (the callers pass s64 list entries) removed a sign extension and also improved dic_learn,
  dic_get1wd and dic_getallwd by 10 each. Return type int, not u16.
- A dead statement can fix register allocation: `if (c) {}` inside the switch of flGetFcurveValue (found by the permuter; output-0 had `if ((c && c) && c) {}`).
- In-place parameter updates (`depth |= (mode + 1) << 6;`, `size0 = (size0 + 15) & ~15;`) keep the original register (SdrSetRev).
- tools/rebuild.sh takes about 4-8 minutes now; run the permuter with PERM_ASM_DIR pointing at a snapshot of asm/ because the rebuild wipes it.
- The scratchpad directory is shared between agents: keep your own files in a subdirectory (mine: .../scratchpad/E).

### Assignment 7, second half (IME, chat UI, more lessons)
Linked in the ime runs (imeaw .. imebu): is_kuten, alloc_record, set_wds, flush_head, newwdlen, get_entid_tab, dic_open, iskanji, tmpoffset, api_funcent,
dic_tmptouroku, dic_newlearn, bs_ctd, main_getsyn, setu_match (jump table 0x36E090-0x36E0B4 is registered with it), syn_2to3, hchar_addchmem, prev_learn,
to_zenkaku, bytesin_kana_buf, count_byte_kana_buf (both were empty stubs), back_gun, muhenkan, set_record. Chat UI runs chat19-chat23: DispFrameListOptionArrowC,
sword_zokusei, Receive_mess_move, zen_kigou_suuji_chk, DispFrameListOptionArrow. New helper scripts are NOT in the repo (they lived in my scratch directory): a
function-local variant tester (replace text inside one function, run alignall, print the count), a "make a run file from an nm file" script (all declarations of
the nm file plus the chosen functions) and a greedy comparison flipper (tools/greedy_sub.py with `>= K` -> `> K-1` and `< K` -> `<= K-1`).
More lessons (each shown by the named function):
- K&R definition `u16 to_zenkaku(c) u16 c;` keeps the call sites with two arguments legal and gives the original's single widening at entry.
- `u16 t` instead of `s16 t` for a timer phase makes `(f32)t` the unsigned conversion with the bltz fix-up (DispFrameListOptionArrow, disp_cursorC 48 -> 4 off).
- A 64-bit parameter: `get_entid_tab(unsigned long id, ...)`; `char *name` plus `*name == 0` gives `lb`; `int page` instead of `s16 page` removes a sign extension when the
  callee already returns the value in a sign-extended register (main_getsyn); `(s16)klen == len` re-extends klen at the compare.
- `if (k < end) { p = ...; do { if (!test(*p)) break; k++; p++; } while (k < end); }` is the shape of `while (k < end && test(*p))` here (muhenkan).
- `if (cond1) { if (cond2) return 1; } if (cond3) {...}` is not a switch: the failed first test falls into the second test (zen_kigou_suuji_chk, ladder with
  fall-through into the next compare); a stack buffer can be bigger than the used length (`u8 buf[0x50]` in dic_tmptouroku / dic_newlearn: the frame size shows it).
- `if (kh == 0 || (pw = kh->pw) == 0) { else-branch values } else { ... }` puts the else-branch code first, as the original does (prev_learn).
- `disp_kouho()` with no argument where the original passes a stale a0 (back_gun); `rt = f(); rt++;` instead of `rt = f() + 1;` (dic_newlearn).
- Unprototyped callers that pass a second argument (`to_zenkaku(c | 0x100, c)`) force the callee to stay K&R in the whole-file C.
- flfntLocate(int, s16) is the prototype that makes an s16 argument pass without a re-extension.
- A greedy pass over `>=`/`<` rewrites on a 6000-line near-match file (ime_nm.c, 25 minutes) found improvements in josi_match (67 -> 30), setu_match, FAskRom_Seek and others.

## Assignment 8 (6 Oct, second round): near-matches first, then the biggest single-player areas
Linked (main START END): eft02_t joins eft/eft02 (0x27D6E0-0x27E940, rodata 0x384170-0x384220); Quest_start (quest/f_quest01), Quest_str_get
(f_quest02), mc_act_save (mc/mcact01), movie_draw (movie/movie01), cmd_henkan (sk/cmd07), sk_init_mode (sk/sk12 + jump table), disp_load_msg
(font/disp1_01), flGetHierarchy3 (fl/flhier01), flPS2SystemTmpBuffFlush (fl/flsys01), ins_bsmem+hchar_addbsmem (ime/imebw), Set_equip_data
(ud/udmisc03), flfntDrawTerm (flfnt/flfntx01), flfntFontPuts (flfnt/flfnty01), flPS2psAddQueue (fl/flpsm01, alias flPS2_Mem_move16_16A in
config/main_aliases.txt), staff_disp (staff/staff01). File-statics (lesson 1): GetAPXPixelMipmapAdrs + GetAPXPaletteAdrs are now C in
tex/apx01.c (two c_rawfuncs lines removed); ud/udgun01 (0x274660-0x274960, gun_check static; Gun_level_up 9 off and Gun_option_ck 3 off are
the two c_rawfuncs holdouts); fl/rs03 (GetFileHeadAAN static + GetModelHeadAAN, alias in main_aliases.txt for fl/rs02.c).
Near-matches left (off/instructions): flGetHierarchyData2 2/44, flInitPostureHierarchySISub 2/69, MAYASub 2/73, SdrSeReq 9/68, SdrSeChg 10/74,
sdr_dmaadr_set 22/73, SdrDmaLoadReq 10/87, reward_mv 9/351, flPS2GetMLCLAY 4/28, flfntSetPalData 28/108, flfntPrintf 45/103, flfntFontPutc
378/315, reward_itembox 35/310, enemy_trans 15/228, quest_em_init_sub2 3/61, stolen_item_stack 7/138, yn_mask_char_check 1/63,
sk_zen_han_check/sk_daisyo_check 3/22, mc_act_unformat (needs mc_unformat static in the same file), tmp_getsyn (OK only with ins_bsmem AND
exist_kouho static in one file). No include/ header edits.
Lessons:
1. FILE-STATICS: docs/survey/mh1_symbols.csv `bind` LOCAL = `static` originally. A static callee defined in the same file changes the caller's
   register use (a0 not saved): GetAPXPaletteAdrs 50->0, Gun_* 25->0, GetModelHeadAAN 36->0, tmp_getsyn 44->0. Link such functions in ONE
   file with their static callees; other files calling them need `name = addr;` in config/main_aliases.txt. tools/statictest.py FILE shows
   what `static` changes. It did not help reward_mv or flInitPostureHierarchy*Sub.
2. `x & 0xFFFFFFFULL` (u64) = dsll32 4 / dsrl32 4 (flfntDrawTerm, flPS2psAddQueue); `<<36>>36` folds to and.
3. `(u32)m->o[0] + (int)mission_area` (Quest_start, Quest_str_get) fixes load order; `((i) << 4) + (int)f` fixes addu operand order (mc_act_save).
4. Loop latch order: `y += ..; e++;` (staff_disp); `for (i = 0, t = tbl; ...)` (disp_load_msg); `i = 0; if (0 < cnt) {do{}while}` (flGetHierarchy3).
5. `c <<= 8; c |= c2;` (flfntFontPuts); `c = p[2];` before the `||` test (cmd_henkan); `if (f(img) <= mip) return 0;` gives slt at (GetAPXPixelMipmapAdrs).
6. 3-s16 struct copy loads all halves first (Set_equip_data); independent stores: brute-force the order (movie_draw).
7. switch cases written 9,18,23 give the chain 23,18,9 (enemy_trans).
8. decomp-permuter found nothing in 20 functions; tools/tweak.py (greedy rewrites on one function) and hand variants found the rest.
Largest unmatched single-player Capcom areas in my ranges (bytes; state): weapon/player draw 0x164410-0x168F00 (weapon_trans 5440, pl_item_trans 3840,
weapon_joint_calc 2376, player_trans 1548, Lb_player_trans 1524; near-match C 15-70% off); eft20 0x218670-0x21CC00 (eft20_t 5148, _m 4600, _i 4316,
pos_set 3564; 15-40% off); chat UI equip_exp_core 5080, DispFrameMessageA 3316; quest_condition_prog 3420 (123/855), remuneration_item_set 1840;
camera 0x21F470-0x225800 (cam_sub_std 2664 42/666, k_HitEmCamera 2152, cam_sub_stg 2096); hit_cap_cap2_m/cap3_m (raw in hit2all);
stage_spr_disp 1844; DispSoftkeyboard/hk_kbd_input 1.4-1.7K; fl library without any source: flPS2SetShaderParam 8264, flPS2ConvClayData 4536,
flSetRenderState 3572, flPS2InitRenderBuff 3056, flPS2LockTexture 2548; HdMerge 4280 (0x21E310). Network last: NetFileCreate 6780, disp_spr_sub 11912
(0x26D310), net_receive_em/pl, ms_network_*.

## Assignment 9 (6 Oct, long round): near-match sweep
Linked (main OK x5, build_pc.sh builds): flPS2GetMLCLAY (fl/clay02b), reward_mv (reward/f_reward4, was f_reward_nm.c), plmemPullHandle (plmem_03), GetPlayerShagamiData
(gmat02), Quest_retire_set (quest/f_quest03), Quest_pl_stage_init (f_quest04), flnecCheckFont (flfnt/flfntx02), soft keyboard helpers sk13-sk18, sk20 (yn_mask_char_check,
sk_zen_han_check, sk_daisyo_check, mh_char_make_check, Softkey_free_0/1, kbdExecServer, Reibun_print, SoftkeyAppInit/Load/TextureSet, SetBlendingMode; these are in
0x24A240-0x2814E0, now agent B's range, already committed), IME: is_jis (imebx), raw_kouho (imeby), chk_entry2+set_entry2 (imebz), bs_prefer+calc_point (imecb),
kh_merge_getone (imecc), reset_temp (imece), concat_bslen (imecf), getrda2 (into imeas), select_subtostr (into imec).
Left: flGetHierarchyData2/SISub/MAYASub (2 off each, scheduling), Gun_level_up 9 / Gun_option_ck 15 (raw holdouts, delay-slot fill differs), sk_get_key_code 3 off (sk19_nm.c),
make_bsmem 4, fl_check 10, inc_gun 12, kouho_makedisp 13, make_chmem 18, add_dummy_chmem 20 (original loads -1 via pcpyld, not reproduced), cam_sub_std 41, free_chmemlist (pcpyld).
Lessons (function that shows it):
1. switch with ascending case labels and no default compiles the reverse-order beq ladder AND removes the else store (yn_mask_char_check, mh_char_make_check).
2. `int t = tbl[a]; s8 u = t; ... (1 << u)` keeps t unclobbered (sk_daisyo_check).
3. K&R `u16 c` parameter gives the entry `andi v0,a0,0xFFFF` and in-place use (is_jis); `int` locals instead of s16 remove dsll32/dsra32 pairs (concat_bslen, calc_point).
4. A call with fewer/more args than the nm had: check the left-over a1/a3 registers (make_bsmem passes 4 args to pword_list, calc_point 2 to setu_point, raw_kouho a stale 5th to create_kouho: define the callee K&R).
5. `row = key[0]-0xA1;` as a local and `u8 *q = &tbl[...]; *q |= v;` (set_entry2/chk_entry2); `q = &p->f; ` pointer per loop iteration `p = temp_pages[i]; p[1]=0; p[0]=0;` (reset_temp).
6. `if (a == 0 || (x = a->f) == 0) {return -1;} else {...}` puts the else code first (bs_prefer); `p->x08 > best->x08` flips the load order (u16 field).
7. Order of 3 independent init statements decides the prologue schedule: try all permutations (concat_bslen: c=0; h=&hchar[pos]; n=0). Declaration-order hill climb: tools/declhill.py only works for brace-on-same-line; my scratch variant handled both.
8. `x >= 0x21` gives slti into v1, `x > 0x20` into at (is_jis). `loop: while (a && (len = f()) != 0)` (select_subtostr).
9. decomp-permuter solved GetPlayerShagamiData in 20 s (unused local `GKIND *k;` and direct `u8 idx = (&tbl[..])->f;`); 8 other runs of 9 minutes found nothing. tools/tweak.py --apply can corrupt a file (it replaced the wrong span in sk_nm.c): never use it without git diff.

## Assignment 10 (6 Oct, long round): single-player breadth, whole-TU linking
Ranges: main 0x160000-0x1C0000 (Capcom parts), 0x1C0000-0x24A240 (IME, skipping Sofdec/ADX 0x1C4000-0x216000) and 0x2814E0-0x293B68.
Linked (main OK x5, see git log): fl/amo_all (0x190600-0x192DC8: the whole AMO model file: plAMO* readers, mesh getters, clay converters; 7 raw holdouts incl.
ConvertModelMeshAMO_NormalModel/WeightModel), fl/res_all (0x18F0B0-0x1905F8: AAN/AHI readers and the whole motion-set creator: plCreateMotionSetFromAAN,
plCreateMotionFromAAN, the 7 Fcurve creators, motion start/end time; 2 raw), fl/pltim_all (0x1935C0-0x194890: TIM2 reader, pixel contexts, plDrawPixel, plGetColor,
plConvertContext, plReport, plMemset; 6 raw), fl/plbmp01, fl/flsys02 (flInitialize, system_work_init, flFlip, flPS2VramFullClear; system_hard_init and
flPS2VSyncCallback raw), fl/fldma01 (VIF1 DMA queue: InitControl, AddQueue, Wait, Terminate, IopModuleLoad/Start; AddQueue2, Interrupt, Send, the store-image handler raw),
fl/fltex01 (texture/palette handle creation: 0x187D10-0x1887F8; 3 raw), IME runs imerun01-07 (clear_allrtime, ask_strncmp, fl_check, make_chmem, make_bsmem,
kouho_makedisp, inc_gun: the "close ones", fixed in ime_nm.c), aq/aqrun01 (host_change), fl/plfcv01 (pl fcurve start/end time helpers; the Hermite
interpolation stays raw, 1 instruction off after a permuter run: `mul.s f4,f0,f5` operand order).
Main line: 36.763% at the start of this round (after merging main), 37.72% with my links alone, 38.39% after merging main again (other agents' work included).
Not mine: Gun_level_up / Gun_option_ck (0x274690/0x274760) sit in agent B's range now.
Near-matches left (off/instructions): GetTim2PictureHead 22/30, GetTim2PictureData 49/100, CheckTIM2FileHeader 17/52 (it needs the dead `if (CLT)` test the compiler removes),
GetTim2ClutData 2/35, plCalcAddress 2/46, plAMOGetModelMatrixlist 23/72, GetWeightAMOModelMesh 6/81, GetPrimVertexNum/CullType/VertexIndex 24-26/140, plGetInitMotionSetSizeFromAHI 3/23,
plCreateInitMotionSetFromAHI 5/138, plMemmove 96/128 (original copies through temp pointers), system_hard_init 4/108 (arg load order of the last flPS2IopModuleLoad),
flPS2DmaAddQueue2 19/112 (backward gotos become direct branches here, the original has `bnez; nop; b enq`), flPS2GetTextureInfoFromContext 2/135, flPS2GetVramTransAdrs 11/60,
flPS2GetPaletteInfoFromContext 18/94, flfntSetPalData 18, flfntPrintf 45, SdrSeReq 9, SdrSeChg 10, enemy_trans 15, flGetHierarchyData2 / SISub / MAYASub 2 each, page_gc 16, add_dummy_chmem 18.
Lessons (function that shows it):
1. THE BIG ONE: a file-static callee changes the register use of every caller defined AFTER it in the same file. LOCAL symbols (docs/survey/mh1_symbols.csv bind) in a run of
   functions mark one original source file: put the whole run in ONE translation unit in address order, statics defined before their users, and keep unfinished functions as
   raw `static asm` holdouts (config/c_rawfuncs.txt, tools/b_rawwrap.py style; /tmp-style helper rawwrap: wrap the C in #ifdef __MWERKS__ asm ... #else C #endif). amo_all went from
   many 40-70 off functions to 0 for 15 of them; res_all, pltim_all the same. The permuter (tools/perm.py) drops `static` from its copy, so it is useless for these.
2. `x = r = call(); if (r == -1)` (copy into the variable in the branch delay slot, test on v0): clear_allrtime, make_chmem, GetAllPrimitiveNumAMOModelMesh, plAMOCreateClayFromImage.
   `l = f(); if (l == -1) return; list = l;` (make_bsmem: the walker variable receives the call result).
3. `return n ? n : 1;` for `bnez; nop; li; move v0` (inc_gun); `} while (n-- != 0)` (make_chmem); `buf += n; n += g(...)` in place (kouho_makedisp).
4. `d += 0xC; return d + idx * 12;` gives addu v0,v1,v0 (GetVertexAMOModelMesh); `if (d == 0) return 0; ...` gives `bnez; nop; b END; daddu v0,0` (all AMO getters).
5. switch: ladder tests the cases in REVERSE source order (write ascending to get a descending ladder), the delay slots stay nops (so a `n == 0x20 || ...` chain becomes
   `switch (n) { case 0x400: ... case 0x20: break; default: log; return 0; }`: flPS2GetTextureInfoFromContext); a jump-table switch needs the dense case set (plAMO... AddQueue2).
6. int fields beat pointers: `c->base + c->stride * y + x` with `int base` (plCalcAddress) removes the addu operand swap; `u32 tag` vs `unsigned long tag`: `(unsigned long)tag & 0xFFFFFFFUL`
   is dsll32 4 / dsrl32 4 on the zero-extended register (flPS2DmaAddQueue2); `int v; (s16)v` instead of `long` kills dsll32 0/dsra32 0 (flPS2VramTrans).
7. Calls with fewer arguments than the callee takes: declare the callee K&R (`int flPS2GetTextureBuffWidth();`) and pass two; the third register stays stale (fltex01).
8. `0 < tries` vs `tries > 0` (slt at vs blez, flPS2IopModuleLoad); `!(a < b)` gives `sltu at` (flPS2DmaAddQueue); `(h & 0xFFFF0000) >> 16` explicit mask (flCreatePaletteHandle).
9. varargs: `int plReport(char *fmt, ...) { va_list ap; va_start(ap, fmt); vsprintf(plReportMessage, fmt, ap); return 1; }` with include/va.h.
10. A loop that polls a hardware word is `volatile int *p` (flPS2DmaWait); `for` loops over small constant counts are unrolled by the compiler itself when the body is simple
    (plCreateInitMotionSetFromAHI: three rows by index inside `for j<4`).
11. Statements that wrote 8 + stack args (flPS2VIF1MakeLoadImage has 11 arguments: 8 in registers, 3 `sd` on the stack, passed as `long`).
12. Helper scripts (kept in the scratchpad, not the repo): tv.py (try source variants of one snippet and keep the best only if it beats the baseline), declperm (all declaration
    orders of a function, found improvements in GetTim2PictureHead/PrimVertexNum/plGetColor 74 -> 0), dfn.py (one function's diff from tools/alignall.py -v), unm.py (unmatched list).

Fresh list of the largest unmatched single-player Capcom code in my ranges (bytes, address), 6 Oct after this round.
0x160000-0x195000 (116.9 KB unmatched): flPS2SetShaderParam 8264 0x179DD0 (GS/VU packets), weapon_trans 5440 0x166380 (nm 349 off), flPS2ConvClayData 4536 0x16B4D0, pl_item_trans 3840,
flSetRenderState 3572 0x177720, flPS2InitRenderBuff 3056 0x18C310 (GS packet stores, draft in m2c is fine but hundreds of stores), flPS2LockTexture 2548, weapon_joint_calc 2376,
flPS2SendRenderState_ALPHA 1872, stage_spr_disp 1844 (nm 132 off), flPS2SwapDBuff 1772, flPADACRConf 1716, flPS2SetMaterialData 1580 (statics flPS2RetouchMaterialTexData/_sub/_sub_mult follow it:
one TU 0x16C690-0x16D5BC, drafts of the small ones are easy, they fill GS register pairs), player_trans 1548, Lb_player_trans 1524, flPS2SetTextureRegister 1440, flPS2UnlockTexture 1360,
flPS2StoreImageB 1312, pl_item_trans_sub 1288, flps1600 1276, flPS2VIF1MakeLoadImage 1196, flPS2GetTextureVramBlock 1176 (division-by-zero checks everywhere), flPS2ConvertTextureFromContext 1060,
Ed_player_trans 1040, flCreateTextureFromApx_mem 1024, font_print_sp 972, enemy_trans 912 (nm 15 off). Many fl functions are VU0 macro assembler (flmat*, flvec*, PS2SHADER_*): raw only.
Working file src/main/fl/fltex02.c (not built): flPS2Conv4_8_32 and its statics Conv4to32/8to32 (0x18AC90-0x18B3A4); flPS2Conv4_8_32 matches, Conv4to32 is 19/84 (register names),
BlockConv8to32 23/65 (the original does not merge the `e++` increments: hand-unrolling x4 gets 61 -> 23).
0x216000-0x230000 (55.9 KB unmatched): eft20_t 5148, eft20_m 4600, eft20_i 4316, HdMerge 4280, eft20_pos_set 3564, quest_condition_prog 3420 (123 off), cam_sub_std 2664 (41 off), k_HitEmCamera 2152
(329 off), cam_sub_stg 2096, remuneration_item_set 1840 (49 off), GetOrthogonalPoint 1652 (camr6_nm.c, only 34 of 413 instructions off: register naming of out/mode and a non-rotated
`for (k < 5)` loop), flfntFontPutc 1260, Spline 1000 (147 off), Cardano 796 (72 off), DKA5 632, Item_regained 756 (54 off), Quest_next_em_set 576 (19 off).
0x1C0000-0x24A240: 438.8 KB of it is Sofdec/ADX/CRI middleware (skipped); IME (0x23ED80-0x24A240): henkan 1032, ch_check 1352, trans_roman 1152, to_roman 864, pword_list 776 ... all with C in
ime_nm.c (66-219 off), the near ones are page_gc 16, add_dummy_chmem 18, getallwd 19, unify_khmem 19.
0x2814E0-0x293B68 (23.6 KB, network last): NetFileCreate 6780, hit_cap_cap2_m 5012, hit_cap_cap3_m 3780, NetFileLoad 3124, reward_itembox 1240 (35 off), nb_flps0009, PatchExecCS, net_flps0008, net_flps0004.
Network functions with small gaps (nm files, off/instr): CpInetTcpOpen 3/13, CngSessionStart_online 3/55, CngNetMcsP2PPoll 3/102, InetIPAddrFromString 2/136, AQ_init 5/89, CngNetAQSessionWait 8/13.

## Assignment 11 (7 Oct, long round, single player first)
Main line 38.669% at the start (after merging main), 38.962% at the end (my links: fl/dmatag, fl/clay05, fl/fms, quest_em_init_sub2).
Linked (main OK x5, tools/build_pc.sh builds):
- fl/dmatag 0x16D9C0-0x16DC98: the whole DMA/VIF tag file as ONE TU (replaces dt01/dt02). The 3-5 off holdouts (Next/Ref/Refe/Call tag) are solved:
  parameter `u32 addr` and `int a = addr & 0x0FFFFFFF; int spr = 0; ... spr = (int)0x80000000; p[1] = a | spr;` (all int/u32, NOT unsigned long/long:
  long gave the extra dsll32/dsra32 sign extension before the sw). tools/vt.py sweep over (param type x temp types x mask form) found it in 4 minutes.
- fl/clay05 0x16C690-0x16D8F0: material DMA packets (HalfColorSub, MakeMaterialDmaData, SetMaterialData, RetouchMaterialTexData, _sub, _sub_mult),
  ClayMakeTextureList, ClayRetouchMaterialTag(+_sub); flExecuteClay stays raw (12 instructions off: the original moves the GetSystemTmpBuff result into
  its saved register in the delay slot of the NEXT call; every ordering I tried gave the move before the argument set-up). Rodata 0x35BD20-0x35BD58 (two jump tables).
- fl/fms 0x16ABB0-0x16ACC8 (frame memory stack, 4 functions as one file, replaces the fmsGetFrame stand-in): fmsInitialize / fmsAllocMemory raw (8 and 11 off, pure
  scheduling: the original starts with the `addiu v0,a3,-1` of the mask before the first store).
- quest_em_init_sub2 (added to f_questl.c): `if (quest_w.no == 0) { q = (s32 *)(int)q; } else {...}` instead of an empty then-branch gives `bne; nop; b` instead of `beq`.
Lessons:
1. A parameter that is only used after the dispatch of a switch: take it as `void *b0` and declare `u8 *buf = (u8 *)b0;` INSIDE each case block. The original copies the
   argument into its saved register per case (`daddu s0,a1` after the jump table); with a plain `u8 *buf` parameter MWCC copies it before the switch and takes a1 as a
   temporary (flPS2SetMaterialData: 393 off -> 0). Sweep param type (void*/u32/int) x local type with tools/vt.py.
2. A call with more arguments than the callee's definition: declare the callee K&R in the file (`void flPS2DmaAddCallTag();`) and pass all arguments the original passes
   (flPS2DmaAddCallTag(buf, qwc, addr, 0, 0): the fifth is `daddu t0,zero,zero` in the delay slot). A callee defined EARLIER in the same file with fewer parameters needs the
   extra (unused) parameter in its definition instead (flPS2ClayMakeTextureList(c, unused)).
3. Pointer-sized small globals used gp-relative: `extern int flTextureStage[2];` (size known and small) gives lw x(gp); `extern int x[];` gives lui/lw.
4. `n = (u8 *)(*(int *)(w + 4) - (int)old); n += (int)b;` fixes the operand order of an `addu` (a - b) + c on pointers (flPS2ClayRetouchMaterialTag_sub).
5. Statement order inside a function body can matter for loads (`nb = buf + c->cnt * *sz;` before `nprim = ...` fixed 21 diffs in flExecuteClay).
Tools added (tools/): declhill2.py (hill climb over local declaration order; works for K&R and brace-on-same-line headers and initialisers), nm_scan.py (every *_nm.c,
unmatched functions in my ranges sorted by difference count), unm_range.py (unlinked functions of an address range), reorder_tu.py (sort a near-match file's functions
into address order, types hoisted); vt.py now uses a per-process temp file so several sweeps can run at once.
Near-matches left (off/instructions): flExecuteClay 12/204 (raw), fmsInitialize 8/30, fmsAllocMemory 11/24, flPS2ClayMakeTextureList / MaterialDmaData done,
plmemDeleteBlockList 7/32 (the original reuses the register that held 0xFFFF for `next`; every form I tried keeps the constant live), flGetHierarchyData2 2 (sll v1,a2,6
before the addiu), flInitPostureHierarchySISub/MAYASub 2 each (mov.s f12 before daddu a1 in the sibling call; a permuter run of 13000 iterations found nothing),
ps2McInit 2 (daddu a1,zero after the first addiu), GetPlayerDiffuseData 19/100 (src/main/emw/gmat_nm.c: the original keeps a `b L; nop` jump-to-jump the optimiser removes here),
stolen_item_stack 6, Em_hagi_point_cnt_ck 20/50 (the original keeps `em` in a0 across the first call and loads n into a2), Quest_next_em_set 17, font_print_sp 69.
Quest file sweep: no literal-address accesses are left in src/main/quest/*.c (everything uses quest_w/game_w fields); the remaining quest near-matches are
register-allocation problems, not address problems.

## Assignment 12 (7 Oct, long round, single player first)
Ranges: main 0x160000-0x1C0000 (Capcom parts), 0x1C0000-0x24A240 (skip Sofdec/ADX 0x1C4000-0x216000) and 0x2814E0-0x293B68.
Main line: 38.962% at the start of the round (my branch), 39.037% after the first merge of main, 39.860% at the end (my runs add about 13.7 KB, roughly +0.8 points; the rest came with merges).
Largest unmatched single-player stretches left in my ranges (bytes, address; "no C" = nothing written yet), 7 Oct after this round:
- no C, GS packet / shader code: flPS2SetShaderParam 8264 0x179DD0, flSetRenderState 3572 0x177720, flPS2InitRenderBuff 3056 0x18C310, flPS2LockTexture 2548 0x188C90,
  flPS2SendRenderState_ALPHA 1872 0x178C90, flPS2SwapDBuff 1772 0x18CF00, flPS2SetTextureRegister 1440 0x1795E0, flPS2UnlockTexture 1360 0x189770, flPS2StoreImageB 1312
  0x16E1C0 (pcpyld + hardware registers), flps1600 1276, flPS2GetTextureVramBlock 1176 0x189FF0, flPS2ConvertTextureFromContext 1060 0x18A4F0 and flPS2ConvertContext
  872 0x18A920 (the pixel copy helpers are inlined, no calls), the flps00xx shader helpers (VU0 inline asm: raw only).
- no C, other: flPS2ConvClayData 4536 0x16B4D0 (the draft has ~40 spilled locals), HdMerge 4280 0x21E310 (sound), flPADACRConf 1716, PADReadSub 2488 / PADRead_for_PS2 1296
  (0x195000 pad layer: update_pad_stick_dir and the device open/close helpers are done now), eft20 (nm C, 15-40% off), cam_sub_std/cam_sub_stg/k_HitEmCamera, weapon/player draw
  0x164410-0x168F00 (nm C 270-1300 off), stage_spr_disp, quest_condition_prog, remuneration_item_set.
- libc / libm / SCE / Sofdec code (0x195xxx-0x1BFxxx: vfprintf, dtoa, strtod, malloc, __ieee754_*, sceCd*, sceDbc*, sceMc*, sceVu0*, Sfd/MPEG decoders) is not MWCC code: skipped.
- network last: NetFileCreate 6780, NetFileLoad 3124, net_receive_em 3672, prot_00/01 (0x2381F0), mcsls_recv 1876, InetDisconnectAll 1588.

Linked (main OK x5 after each batch of links, tools/build_pc.sh builds), address order:
- fl/flcreate01 0x16FA00-0x170198 flCreateTextureFromApx_mem + flCreateTextureFromTim2_mem (two 1 KB functions, matched at the first try from the asm: locals declared in the
  order i, w, ht; `0 <= mips` for the loop guard), fl/flcnv01 flPS2ConvertAlpha, fl/flview01 flmatrMakeViewport, fl/flproj01 0x1715E0-0x1717B8 (flmatrMakeProjection,
  flmatMakeProjection, flPS2MakeClipProjection), fl/flmat03-05,07 (flmatScaleFactor33 with `f32 t[16]`, AddTrans2, GetTrans, Copy33), fl/flvec01 flvecRotX/RotY,
  fl/flquat01 flQuatSetRot, fl/flfov01 flCheckMeshFOV, fl/flcolor01 flPS2ConvColor, fl/flpstex01 flPS2SendTextureRegister, fl/flsettex01 flPS2SetTextureRegister (1440 bytes of GS
  TEX0/TEX1/MIPTBP bit packing), fl/flrs01-05 the GS render state packets FOGCOL, TEX1, ZBUF, SCISSOR, TEST and ALPHA (1872 bytes, two jump tables: main:rodata 0x35BE70-0x35BEA8),
  fl/tarpad03 0x195510-0x19583C (update_pad_stick_dir with the soft-float calls, lever_analog_to_digital, PADDeviceInit, PADPortOpen, PADDeviceDestroy).
- fl/vr01 now the whole VRAM list file 0x18B3B0-0x18BAE0 (flPS2SearchVramSpace: `p = (aligned + len >= 0x4000) ? (VRC *)-1 : p;`), fl/tx02 + flReleasePaletteHandle_NOWAITDMA,
  fl/tx05b flPS2ReloadTexture, fl/fllog01 flLogOut (varargs, 0x800 byte buffer), fl/flmotion03 0x173A50-0x173E54 (motion set handles, flCalcTransVelocity, flBlendMotionEx + static
  flBlendMotionExSub), fl/flnode02 + flSetMotionEx, fl/flnode03b flPlayMotionExSISub, fl/flnode05f/g flInitPostureHierarchySISub/MAYASub (the two "2 off" functions of last round).
- Also moved in nm files: sel_sel_sub 0, Sel_back_disp 2 (omake_nm.c).
Lessons (function that shows it):
1. REGISTER ORDER FOLLOWS DECLARATION ORDER: of the locals that compete for registers the LAST declared gets the LOWEST register (saved registers s0.. and temporaries
   alike). Declare one variable per line, no initialisers, and try all orders (tools/declperm.py, or my scratch permdecl.py: all N! orders of the first N declaration lines).
   flPS2ReloadTexture (26 -> 0 by the order t,k,cnt,tex,pal,...), flPS2ConvertAlpha (x before y), flSetMotion/Create*Handle, flPS2SendRenderState_TEST (atst, aref, ztst).
2. THE PARAMETER ORDER OF THE CALLEE DECIDES THE ARGUMENT SET-UP ORDER OF A CALL: floats and ints are numbered separately (f12.. / a0..), so a prototype can be written with the
   float in any position without changing the registers. The original's "mov.s f12 before daddu a1" is a call to a function whose float parameter sits in the middle:
   flInitPostureHierarchySISub(n, sx, parent, sy, sz), flGetFcurveValue(mot, init, v, t, hint), flBlendMotionExSub(a, b, w0, id, w1), flGetMotionMatrix(mot, init, t, v, mat, hint)
   (fixed three "2-3 off" functions). Try every position of the float parameter.
3. A 64 bit packet built with a `u128 *` cursor: `p++` per quadword, `*(u64 *)p = ..; ((u64 *)p)[1] = ..;` and `top = p` saved before (typedef long u64, NOT unsigned long: the
   unsigned type adds dsra32/dsll32 pairs). With u64 * / u8 * cursors the compiler folds the advance into offsets and the stack/register shape never matches
   (FOGCOL, TEX1, ZBUF, SCISSOR, TEST, ALPHA). `(long)(u32)((K * 16) & 0xFFF) << 32` gives a bare dsll32; `(long)(int-expr) << 32` adds a sign extension pair.
4. `u8` constants load with daddiu (flPS2ConvertAlpha), `if (x > r) return 0;` for float tests gives the original delay slot filling (flCheckMeshFOV), a one-case switch
   `switch (rs & 0xF0) { case 0: v = 2; break; default: return 0; }` gives the ladder with the return block after the case (ALPHA), `a = ...; if (a == 0) a = 1` etc.
5. An address-taken local needs a struct, not an array: `FV3 t; t = *(FV3 *)v;` keeps the temporary in memory (flvecRotX), arrays of floats are kept in registers.
6. Calls that pass a leftover register: a callee that takes more arguments than the caller names: declare K&R and pass the real list (flPS2SendTextureRegister passes 8 arguments,
   m2c shows 3). flmatMakeProjection takes (m, far, near, fov, aspect) in f12-f15.
7. `i = 0; if (0 < n) { do {} while }` was not needed for a `for (i = 0; i < t->mips - 1; i++)` loop (flPS2SetTextureRegister): the for form gives `sltu at,zero,v0`.
Near-matches left (off/instructions): flPS2GetClaySize 19/212 (src/main/fl/clay06_nm.c: only the flags/vertex-size temp and the loop counter swap s5/t7), flCalcTransVelocity done,
flPS2VIF1MakeLoadImage 248/299 (flldimg01_nm.c: logic complete, spill slots), flPS2InitRenderState 60/108 (flrs06_nm.c), flmatMakeViewport 142/214 and flPS2MakeClipViewport 47/64
(flview01_nm.c), flQuatCnv 63/67 (flquat01_nm.c, register numbering), flmatCopy 6/9, flPS2DrawPreparation 87/105 (kept in my scratch only), fmsInitialize 8, fmsAllocMemory 10
(all 24 declaration orders x statement orders tried), flGetHierarchyData2 2, ps2McInit 2, plmemDeleteBlockList 7, Em_hagi_point_cnt_ck 20, stolen_item_stack 6, ZoomRateCalc 8,
flPADConfigSet 10, flSetMatrixList 11, flFCVGetValue2 13, em_status_ck 15.
Tool notes: tools/check.py reads the disc (not asm/), so it keeps working while tools/rebuild.sh regenerates asm/; align.py and draft.py need asm/.
Scratch helpers (not in the repo): fv.py (apply text variants to a whole file, print the check.py difference count), swaphill.py (greedy swaps of adjacent statements),
declmove.py, permdecl.py (all orders of the first N declarations).
