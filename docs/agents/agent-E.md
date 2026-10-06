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
