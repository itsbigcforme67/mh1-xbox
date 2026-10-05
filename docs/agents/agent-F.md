# Agent F notes: player code (asm/main/text/f_pl.s, 0x134950-0x14D1C8, 224 functions)

Status (6 Oct 2026, late night): ~260 functions byte-matching and registered (src/main/pl/pl01..pl48.c,
`tools/rebuild.sh main` = main OK). Everything else written so far is in src/main/pl/pl_nm.c (compiles, not built). Registered since the
last summary: pl09 .. pl48 = 0x1371B0 .. 0x14C4F4 (item/attack/damage/death/demo/egg/chat handlers and the dispatchers pl_normal, pl_attack,
pl_damage, pl_die, pl_demo, pl_egg, pl_chat, pl_move, pl_move_sub_sub with their jump tables). What is left in f_pl.s as asm (all in
pl_nm.c as near-matches, or not yet written): pl_move_sub (0x14C500, 2144 bytes, drafted but not written), pl_turn_sub, pl_horm_sub,
basic_com_ck, gun_adj_sub, sougun_adj_sub, wall_act_ck, wall_vec_set, stick_pow_get, em_ninshiki_ck, pl_work_clr, player_init0,
timer_calc_sub_pl, pl_dm_value_sub, pl_mv021, pl_mv060, pl_at008/009/012, pl_dm003/008, pl_demo000, pl_egg03/05, egg_com_ck.
Typical causes of the near-matches (see Lessons): IPA with static callees, delay-slot scheduling after calls, register naming of locals.
Workflow (about 2 minutes per small function): `tools/plnext.sh F1 F2 ..` drafts into pl_wip.c, `python3 tools/pl_asm.py F`
shows the asm without the noise, write the C by hand (the m2c output is only a guide: arg counts, switch order, locals),
`python3 tools/check.py src/main/pl/pl_wip.c -v | grep '>>'`, then `python3 tools/plreg.py plNN "descr" F1 F2 ... [RODATA=a-b]`
moves them into src/main/pl/plNN.c and registers the range; `tools/rebuild.sh main` must print OK.

## Shell work pool (7 Oct 2026)
src/main/pl/shell_work.c (0x158F20-0x159378: init/clr/pull/push/move/trans_shell) and shell_work2.c (Ana_ok_ck, softdip stubs) match;
Taru_ok_ck is a near-match in shell_work_nm.c (8/14 insns, original lays the `return 0` arm inline before the else load).
SHLW is 0xD4 bytes (shell_work = 64 entries x 0xD4): include/shell.h was padded to that size and got xC4, prev/next/heap_pos/heap_n/x7B.
Not done: sound glue 0x159410-0x15A520 (Snd_init, se_req*, Pl/Em/Npc_se_req*, snd_joint_load*; PC port replaces sound), online select
screens 0x14E0C0-0x14E8E0, set13_m/set13_trans (see set13_nm.c), SpritePut/CalcPoint/trans_sprite.

## Near-matches in pl_nm.c
- pl_move_sub 469/536 insns differ, only delay-slot hoisting of `move a0,s0` (see pl_nm.c); all else matches.
- Pl_item_charge OK in nm too (registered pl04). player_init0 8 off (register choice for work616 load), pl_work_clr 15 off
  (u8 arg `no`: original keeps raw a1 in s1 and re-masks), timer_calc_sub_pl 367/425 (original keeps `move a0,s0`
  in the jal delay slot, we hoist it into the bne slot; structure otherwise equal), pl_dm_value_sub 23 off
  (switch on Stage_env_ck result; original compares 2 before 1 and has no extra b-stubs), stick_pow_get 3 off (u8 r returned),
  em_ninshiki_ck 63 off (EMW pointer in s1 vs s0, bit mask register).

## Tools (all new, in tools/)
- pl_draft.py FUNC --add: m2c draft (with jump tables from the main image) appended to src/main/pl/pl_wip.c, raw PS16(pl,0x..) accessors.
- plconv.py FILE: turns raw accessors into PLW fields (adds `workXXX` fields to include/pl.h via plx.py), retypes auto fields on signedness.
- plx.py add/set/rm/list/at: edit PLW padding in include/pl.h. plmove.py: wip -> pl_nm.c.
- plsplit.py [forced names]: moves OK functions into new plNN.c + c_files.txt (needs /tmp/claude-1000/pl_funcs_F.txt, regenerate from config/symbols/main.txt range; jump tables go in tools/pl_rodata.py).
- vt.py (try function variants), hexf.py (hex float literals), pl_asm.py (compact asm listing).

## Lessons
- MWCC IPA again: a `static` callee defined earlier in the same file lets the compiler keep a0 (and other temps) alive across the call.
  piyo_reset (leaf, 20 bytes) made pl_dm019/pl_dm021/pl_dm022 match only when it is `static` in the same plNN.c. A static function
  cannot be referenced from other files, so the file must contain all of its callers (here pl_dm022 was the last asm user: the link
  failed with `undefined reference to piyo_reset` until pl_dm022 was moved into the file; then it linked).
- Small constant tables: `extern s8 piyo_ret_tbl[6];` (size known) gives gp-relative `addiu v0,gp,off` like the original; an
  unsized extern gives lui/addiu. Always declare data with the size from config/symbols (size:0x6 -> [6]).
- check.py does not compare jump-table DATA: only the full `tools/rebuild.sh main` does. A jump table needs its own `main:rodata`
  range in c_files.txt; the first mismatch offset 0x25xxxx (= address 0x35xxxx) points at it; a mismatch in the table contents
  means the case bodies are laid out differently (e.g. `case 0:` merged into case 1/2 instead of pointing at the default).
- Dispatchers (pl_normal/pl_attack/pl_damage): `switch (flag15) { case N: fn(pl, mode); break; ... default: pl_to_normal(...); }`
  with K&R declarations (`void pl_mv000();`) because the handlers are called with and without a mode argument; the LAST case body
  must not end in `break;`/`return;` (an extra `b end` appears).
- `p == 1 || p == 2 || p == 3` is merged into a range test by MWCC, `!= && != &&` chains too; the original used `switch` (compare
  chain with separate beq, last one `b end`) in kabe_com_ck, pick_set_sub (q), pl_mv052 (h) and others: if the asm shows
  `beq a,1 / beq a,2 / beqz a / b end` write a switch with those labels (case order = reverse of the chain).
- `ran_suu(1) & 0xFFFF & 3` in m2c = `u16 r; ... ((r = ran_suu(1)) & 3)` (two andi); a plain `(u16)` cast or one expression merges them.
- `daddiu rX,zero,N` for a local means a u16 (not s32/s8) local: `u16 n = 1` in pl_mv095. A local that is both set to constants
  in a switch and passed on unextended is int; an s16-cast chain (`n = (s16)(t + t / 4)`, t = (s16)n) is m2c's `<< 0x30 >> 0x30`.
- Stack frame order: locals declared LATER get LOWER stack addresses (pl_mv030, pl_mv068): declare the one that sits at the
  highest sp offset first.
- Calls like ItemPickingDeclaration(pl, &pl->x8E6): the second arg is the address of a PLW field; the 0x8D4 name is 0x12 bytes.
- frame_check(f32, pl, 0) / frame_check2 / frame_check3(f32, f32, pl, 0) / front_land_ck2(f32, f32, pl, 0): the extra float args are
  real (f13 loaded); leftover a1/a2/a3 values in m2c are NOT arguments unless the callee reads them.
- IPA register preservation: gun_adj_sub keeps a value in a3/t0 ACROSS calls to blend_set/blend_calc/scope_add. MWCC only does
  that when the callee is a `static` function defined EARLIER IN THE SAME TU (it then knows the callee's clobber set; a global
  callee, or one in another .c, gives normal saved-register code). The original f_pl.c is one TU, so such functions cannot match
  from separate plNN.c files unless the callee is copied in as static (which adds code). Parked as near-match.
- check.py -v prints `>>` for branch-target differences that tools/align.py ignores: always look at both (a wrong `return` vs
  `break` shows up only as a different branch target; unique_act_set default arm).
- Switch case-label source order is the REVERSE of the compare chain MWCC emits (item_action_set: copy the chain from the asm,
  reverse it; labels of one body are also reversed). A dense jump table (basic_atack_ck, mv001 kind switch, ex_atk_ck) needs
  explicit empty/duplicate labels so that the range 0..N is covered (`case 0: default:`, `case 1: case 5: break;`).
- `if (a != 1) {X} else {Y}` and `switch (a) { default: X; break; case 1: Y; break; }` differ in delay-slot filling; the switch
  form matched pl_mv006 (the if form hoisted `move a0,s1` into the branch slot, the original left a nop).
- Loop `for (i = 0; i < n; i++, p++)` with an `if` on the first test gives `slt at,zero,n` like the original; a guarding
  `if (n > 0)` gives blez (trade_get_ck). A shared tail `x = 0x5A; return 0;` needs the goto/label split seen in trade_get_ck.
- Args that look wrong in m2c (stick_pow_get(pl,1,7), Pl_master_ck(), basic_atack_ck()) are usually registers left over in
  a0/a1/a2: check the asm for the real arity (Pl_master_ck(pl), basic_atack_ck(pl), Pl_view_reset(pl) is 1-arg in the PLW code).
- front_land_ck(f32,f32,f32,PLW*,int*), SetVector(f32,f32,f32,f32*), calc_vec_ang(f32,f32,f32,f32) take float args in f12-f15.
- PLW 0x004..0x007 is now a union: s32 work04 or x04/x05(step)/x06/x07; plx.py keeps the union block. 0x3B4/0x3C0 are f32 vel[3]/acc[3].
- Statics: a `static` scope_add in the same file made sougun_adj_sub's registers match, confirming the IPA lesson.
- tools/plclean.py rewrites PLSW raw accessors and drops the m2c switch comments; tools/pl_draft.py now finds jump tables whose
  symbols have no address in the name (lit_NNNN) via config/symbols/main.txt. After each draft run, delete the junk prototypes
  pl_draft appends to include/plf.h (M2C_UNK/s64/duplicates) before compiling: they silently break already-registered files.
- Raw pointer-cast accessors (`*(s8*)((u8*)p+off)`) change codegen (CSE of the address); use real struct fields.
- `x > 100` gives `slti at` (value kept); `x >= 101` puts it in v1. `(u32)(a-3) > 1` gives `sltiu at`.
- Declaration order of s16 locals fixes t0/t1 swaps (item_sel_sub).
- `daddiu` constant loads come from u8/u16 locals (stick_pow_get, pl_init_sub case 2).
- A struct array of PL_ITEM {u16 id; s16 num;} item[20] at 0x828 explains the `i += 5` unrolled clear (Pl_item_charge).
- Float stores like `sw 0x3F800000` are f32 fields assigned float literals (see hexf.py).

## Shared header edits
include/pl.h (PLPROG, PL_ITEM, many work fields, flag14/15 u8, work72C u16, x73A merged), include/game.h (pl_state, x213-x216),
include/em.h (x7EE/x88F/x9EC comments). Struct merge done with tools/merge_struct.py.

## Session 7 Oct 2026 (second player pass): 0x14D1D0-0x154F14 region
Registered new: pl49..pl78 (pl_chr_sub, Oki/Taru/Ana_item_set, act_set family + pl_flag_set/clr, status/atk/def adjust + skills
+ resistances (pl53), chr_set family (pl54), rate helpers + action timers (pl56), hit_data_expand/front_land_ck (pl57),
World_calc, St_unique_ck/Item_get_ck/item pouch search/erase/supply (pl62-66), Pl_adj_calc/pos_adj, vital/stamina/chat act (pl67-70),
equipment accessors Get_equip_* (pl71, jump tables 0x35B210-0x35B2B0), Shell_type_set + Pl_item_get_se (pl73/74, plitem.h), misc).
New headers: include/plst.h (stage item/unique records), include/plequip.h (equipment row structs; pl71 only, must not be
included with plf.h), include/plitem.h (Item_data as struct array; pl73/pl74 only). Reason: the original folds 16-bit member
offsets into the symbol (`lhu` from `Item_data+0xA`), which only a struct-array declaration produces; casts on the u8[][16] array
declared in plf.h never fold. Such functions go into a file that includes pl.h + the struct header instead of plf.h.
Near-matches added to pl_nm.c (logic believed equal): St_pick_ck2 (15 off, tail of the if chain), Pl_item_stack (241 off: logic
complete, uses an extra saved register; read the asm before trusting it; returns an uninitialised ret if no slot is found like
the original), Pl_item_num_ck2/3 (reloc only + one delay slot), Get_Use_itemnum (2), Pl_vital_calc_item (16), Pl_horm_adj (float temp
order: the original keeps 0.3*angle in f20 across the get_joint_mat call, we call first), Pl_slash_lv_ck/Pl_slash_calc,
Pl_shell_set (36, chain/regs), Pl_basic_flagset, pl_flag_ck, to_normal, Pl_scope/silencer/barrel_ck (nop only), Pl_hold_item_ck,
Sansai_talk_ck (register swap), pl_atck_data_set_shl2 / atck_data_set_shl2 (original copies the 0x18-byte record as three
word pairs with pointer increments; we emit a struct copy), rate_g_calc, pad_timer_calc_sub (gp-relative table).
Still not written: Pl_box_select/box_get (item box UI), body_hit family, pl_body_make, pl_light_ck, pl_voice_req, Basic_item_set,
Fue_item_set, Plsel_task..em_select (online player/monster select screens), Pit_disp_pit_effect, pef_get_alpha, the
set/shell work (0x155xxx-0x15A) and sound (se_req..) functions further down (not player code).
Lessons: (1) always `tools/check.py src/main/pl/plNN.c` after tools/plreg.py: a function that matches inside pl_wip.c can
differ once split (implicit prototypes of callees that used to be defined in the same file: add `s16 Pl_item_num_ck(PLW *, int);`).
(2) plreg only copies preamble lines before the first function: extern/typedef lines further down must be added by hand.
(3) A callee defined earlier in the same file as `static` (pl_chr_set_com) gives the IPA register behaviour; params passed as `int`
and masked at the use site (`(u16)slot`, `(s16)num`) reproduce the raw-register + repeated dsll32/dsra32 pattern.
(4) switch case order: when the chain compares 3,2,1,0 the source usually lists cases in ascending order (Get_weapon_job2 used if/else).

Addendum (same day): pl79/80 Basic_item_set + Fue_item_set (matched), pl81 pl_voice_req, pl82 pl_body_make, pl83 body_hit_sub_pl.
Near-match only (in pl_nm.c, logic from asm): body_hit, body_hit_sub_new, body_hit_sub_em, box_get, Pl_box_select, pl_light_ck.
body_hit_sub_em keeps a quirk of the original: the target sphere-list pointer is not rewound for later spheres of the first
monster. Fue_item_set calls Pl_master_ck(pl) (not the master slot). Still asm: Plsel_task..sel_default_set (online select screens),
Pit_disp_pit_effect, pef_get_alpha. NOTE: tools/build.py compiles every src/**/*.c including pl_wip.c/pl_nm.c, so pl_wip.c must
always compile (git checkout of an old wip with junk drafts broke a rebuild once).

## Lobby overlay, 0x5C4E60 - end (online town), work log
Setup: lobby C lives in src/lobby/, registered as `lobby START END lb_xNN` in config/c_files.txt; `tools/rebuild.sh lobby` must stay OK
(all five OK). Shared lobby structs/prototypes in include/lobby.h (client work `cw` is `u8 *` = D_6DD7E0, accessed with CW8()/CWPLAYER(),
lb_sys/lbCommer/lb_player/lastSend structs, many K&R prototypes). Working files `lb_a.c .. lb_k.c` hold the whole C of a region (not
built as such: every .c is compiled but only registered ranges are linked); `tools/lbf_runs.py FILE PREFIX "comment"` splits the fully matching,
not yet registered functions into contiguous runs PREFIXNN.c and appends the c_files lines (FORCE_OK=name for functions check.py cannot verify,
e.g. a callee whose symbol carries an address suffix). Other helpers: tools/lbasm.py (compact asm), tools/lbd.py / lbf_conv.py (m2c drafts;
drafts are made with draft.py into a scratch dir, see LBDRAFTS), tools/lbset.py (replace a function in a file from stdin).
Reuse from main: only 11 lobby functions are byte-identical to main code (Lb_act_ck, Lb_stick_dir_set, Lb_Pl_adj_calc, Lb_Pl_pos_adj,
lb_pl_flag_clr/set, Lb_hit_stop_calc, Lb_World_calc, lb_pl_chr_set_com, lb_pl_to_normal_clr2): src/lobby/lb_pl01-09.c. Many more are
close copies of main player code (sw_set_sub, pl_timer_calc, to_normal_clr, action_timer_calc): copy the main C and edit.
Map of the range (0x5C4E60-0x610300): 5C4E60-5C5E80 receive handlers (trade, status, chair, commer); 5C5F30 Lb_guild (4100 bytes, quest
guild UI) .. 5CB0E0 guild/quest select screens; 5CB100-5CD0F0 room members, drawing helpers (Lb_put_*), 5CD0F0-5CDB00 member in/out checks and
player load; 5CDBD0-5D3640 player code (sw_set_sub, act_set, to_normal, move dispatch lb_pl_mv000-099, Pl_to_chair, lb_pl_normal,
lb_basic_master 4840 bytes, lb_pl_chat00-16); 5D58C0-5D6420 Lb_send_* network senders; 5D6420-5D7790 lb_check_status/Lb_move_common/stage load;
5D7790-5D8460 misc lobby UI; 5D8470-5D93A0 vs_square; 5D93A0-5DB9C0 Bs*/HttpTask/http_test_NN; 5DBA80-5E2A90 browser drawing (draw*, stock*
page objects); 5E2A90-5ED940 browser (Bs*, cache, URL, zlib/png glue); 5F2xxx-601xxx tagAct_NNN HTML tag handlers, 602xxx-605xxx table/text layout;
609750-610300 item box, plaza chat, eft25. About 40 percent is GCC/library (crypto/SSL is in the lobby text before 0x5C4E60, B's range).
Idioms learned here (all verified by matching):
- `switch (x) { case 0: case 0xF: ... }` is how the original writes `x == 0 || x == 0xF` (compare chain beq/beq/b); a two-case switch
  compare chain is in REVERSE source order; a single-case switch gives `beq; b else`. The LAST case must not end in `break;`/`return;`
  (MWCC emits an infinite `b .` loop), but a middle case that ends `if (c) { ...; }` needs `break;` rather than `return;` twice.
- A static leaf callee defined earlier in the same file (check_sender0/1 return 0) is IPA'd: its arguments are dropped and the caller
  keeps temporaries in a3/t0/t1: write `static s8 check_sender1() { return 0; }` K&R, call it with ONE arg (Lb_send_pl_pos/Pl_status).
- `F(T, p, off)` style raw field macros make MWCC hoist `p+off` into a register when the field is used twice (extra addiu); use the
  PLW field names (pl->work81D ...) when they exist, `u8 *` locals, or a temp variable.
- 5th+ args go in t0..t3 ($8..$11): `void Lb_put_status(int a0,int a1,int a2,int a3,int no)`; a K&R call `f();` leaves a0.. untouched.
- A function ending a u8 local with `u8` return type returns without the andi: `u8 Lb_stick_pow_get(PLW *)` (this is also the 3-off cause of
  main's stick_pow_get).
- Struct copy of a local `u128` pair (lq/sq) needs `unsigned __int128`; copying 12-byte f32 triples as `*(LBV3 *)a = *(LBV3 *)b` gives the
  load-3-then-store-3 pattern.
- `Lb_Pl_act_set`, `Lb_act_set` take u8 params: define them K&R (`f(pl, a, b) PLW *pl; u8 a; u8 b; {`) because lobby.h declares them `()`.
Near-matches (kept in the working files): lb_commer_message (s0/s1 swapped), lb_set_pl_status/pos/stage, lb_check_mini_data, lb_trade_result
(2 insns), Lb_room_member, Lb_PlStatusSet, Lb_put_gold (struct copy of rodata), lb_pl_horm_sub (original re-reads the field), Lb_act_set
(original calls Lb_act_ck without setting a0), Lb_check_chair (1 insn), Lb_player_load (2 insns).

### Automatic pipeline for lobby functions (tools/lb*.py)
1. Drafts: `python3 tools/draft.py lobby FUNC...` (40 per call) into a scratch dir `drafts/dNNN.c`; functions with jump tables need
   `tools/lbdraft_jt.py OUT FUNC...` (reads the table words from disc/mh1/split/lobby.bin) and go into `drafts/djNNN.c`. LBDRAFTS names the dir.
2. `tools/lbauto.py [-j3] [--out J] --all | NAME...`: converts a draft (F() fields, gp-relative globals via the main symbol table,
   K&R externs, `int argN` params filled in), compiles it ALONE with `tools/check.py --module lobby`, adds `int f();`/`extern char x[];`
   for undefined identifiers, and records OK / diff (d of n) / error / unsupported (M2C_ERROR: float mula/madd, unset registers) / nodraft.
   About 12 percent of the lobby functions came out byte-identical with no hand work; many more are 1-3 instructions off.
3. `tools/lbfix.py NAME...` repairs near misses: m2c drops pass-through arguments (a0 untouched), so it tries inserting `arg0,`/`arg1,`
   as extra leading args at each call site.
4. `tools/lbf_merge.py PREFIX "comment" NAME...` writes contiguous runs of the OK ones (sources in build/lbauto/NAME.c) to src/lobby/PREFIXNN.c,
   verifies them and appends the c_files.txt lines. Always finish with `tools/rebuild.sh` (do NOT run it while lbauto/lbfix are running:
   they create src/lobby/zz_*.c temp files that rebuild.sh would compile).
Names whose lobby symbol carries an address suffix (trade_get_ck_005D0750 ...) are not found by check.py (it falls back to the game
module at the same address): check them through a renamed copy (scratch lbchk.sh idea: sed the name to the csv name, `--module lobby`).

### Lobby status at the end of this session (6-7 Oct 2026)
335 of the 928 functions in 0x5C4E60-0x610300 are C (34.8 KB of 302 KB); `tools/rebuild.sh` prints OK for all five modules.
All my lobby C lives in src/lobby/f/ (names f/lb_xNN in config/c_files.txt) and uses include/lobby_f.h (agent B has its own include/lobby.h
and src/lobby/cnet etc.; the helper scripts are lbf_runs.py / lbf_merge.py / lbf_conv.py because main already had B's lbruns/lbmerge/lbconv).
Per file: lb_a (receivers), lb_b (quest money), lb_c (small helpers), lb_d (senders), lb_e (members/icons/cockpit), lb_f (gold/player init),
lb_g (player basics), lb_h (flags/stick/adjust), lb_i (move dispatch), lb_j (lb_pl_mvNNN), lb_k (chat handlers), lb_l (trade/sleep/guest room),
lb_m (Bs*Trans helpers), lb_n (generic senders), lb_o (lb_pl_normal), lb_p (lb_move_common, near-match), lb_pl* (copies of main pl code),
lb_zNNN (auto-drafted: m2c + tools/lbauto.py, see the pipeline section). The working files lb_a.c .. lb_p.c keep the near-matches.
Jump tables: a function whose switch compiled to a jump table in the original needs its table data registered as
`lobby:rodata START END f/NAME` (tools/lbf_jt.py prints the range from the asm BEFORE the function is registered; lbf_runs/lbf_merge do it
automatically). tools/check.py cannot see this; the failure shows up only as `undefined reference to .Lxxxxxxxx` at the lobby link.
(Superseded by 'Lobby session 3' below.) Left (about 590 functions): browser (Bs*/draw*/stock*/tagAct_NNN and layout), guild UI (Lb_guild 4100, lb_rule_seet_set, lb_select_quest_level_trans,
lb_questpage_trans), lb_basic_master (4840), lb_check_status (2316), Lb_stage_load (1852), Lb_put_help, lb_disp_name, vs_square*, http_test_*.
Known stubborn classes: (1) `addu rd, idx*N, base` vs `addu rd, base, idx*N` (operand order of an indexed address: ~12 tagAct_/stock functions
are exactly 1 instruction off for this reason; no source form found that flips it); (2) s0/s1 register order of two long-lived locals
(lb_commer_message, lb_set_pl_status/pos/stage, Lb_move_common); (3) rodata struct copies (Lb_put_gold, pl_sleeping); (4) functions whose
m2c draft needs hand work for stack arguments beyond 8 (AppendWork stock* wrappers, 13 args: reg args a0-a3,t0-t3 then 5 stack dwords).


## Lobby session 3 (8 Oct 2026): town logic, guild UI, first browser objects
Registered (exact): lb_check_status (lb_q01 + jump table), Lb_guild helpers (lb_v01-03: lb_get_quest_level, lb_rule_seet_trans, lb_guild_talk),
lb_guild_check_keyQuest-style small ones (lb_t01-05), the browser tag handlers tagAct_* (lb_s01-14), stock* recorders + UpdateEndpoint (lb_ak01-09),
browser screen objects (CharSet/Task of BgImg, PageObj, H/V scroll bar, title bar, tool menu, soft keyboard, cursor, dialog: lb_al01-09),
queue helpers (lb_am01-05, lb_an01). Hand-written near-matches (compile, believed equivalent, NOT registered; the working files keep them):
lb_q.c Lb_stage_load (46 of 463 insns off: two `andi v0,zero,0xFFFF` constants and the St_unique_tbl index scheduling), lb_r.c lb_basic_master (structure
equal; only s0/s1 register naming and a few delay slots differ), lb_u.c Lb_guild, lb_v.c quest-level/startMsg/make_room/input/rule sheet,
lb_w.c lb_rule_seet_set, lb_x.c quest board draw + select_quest, lb_y.c quest table generation, lb_aa..lb_ai (player status window, member checks,
target selection, chat, chair, plaza phases vs_square_*, name tags, help bar, mv052/076/088, Lb_check_receipt), lb_am/lb_an (route/request queues).
Idioms learned (all verified by matching):
- `addu idx*N, base` order (the old "stubborn class 1"): index an ARRAY of structs of size N: `((CELL *)base)[idx].p[off]` (macros BSC/BSC2/BSC4/BSC8 in
  lobby_f.h for sizes 0x5C/2/4/8). Taking the address (`&arr[i]`) or `(int)base + i*N` gives base-first. A global pointer needs a separate `(i*N)+(int)p`.
- A base declared `int` makes MWCC split a large constant offset (addiu 32767 + addiu x); declare the base `u8 *`/pointer to get `ori at,zero,K; addu`.
- `x > N-1` gives `slti at`, `x >= N` gives `slti v1` (also for unsigned `sltiu at`); try the other form when only the register of a compare differs.
- `if (a == 1) {...}` for a single value compiled as `beq; b end` is a one-case `switch (a) { case 1: ... }` (all the Bs*CharSet functions).
- Switch compare chain = reverse of source order, and labels that jump to the end (`case 8: case 32: break;`) or to default must be listed
  explicitly; they change the jump table (check_status: table entries 8 and 32 pointed at the end). A dense table is only emitted when the
  explicit labels fill the range (basic_master fish switch: add `case 0: case 2: ... default: return;`). The table needs its own `lobby:rodata`.
- `default:` placed FIRST in a switch means the default body is the first code (lb_check_status). Cases that ended in `goto block` in m2c are often
  plain `break`/`return` plus code after the switch (Lb_guild: `lb_guild_talk()` after the switch; lb_basic_master: block_233 after the switch).
- Functions with more than 8 args: declare the callee with int params (AppendWork has 13); the 5th..8th go in t0-t3 and the rest as `sd` dwords.
  The stock* wrappers then match directly. Leftover argument registers are not arguments: `Lb_pl_to_chair();`, `Online_ck();` take none.
- A function returning float args needs an ANSI prototype (`int f(f32 r, PLW *pl, ...)`); K&R promotes float to double and shifts a0..
- Stage/state globals as struct fields (lb_sys.x04/x07/x08, MHRULE x00/x4F/x58, BSSYS, BSWK, BSNODE in lobby_f.h) avoid the CSE of `&field` addresses
  that raw `*(s8 *)((u8 *)&sym + off)` accessors cause (lb_rule_seet_set saved a register that way).
- IPA again: BsRouteForwardCheck/BackCheck use a1 across the call to bs_route_queue_forward, so they only match in one TU with the callee defined
  before them (needs the whole range bs_route_queue_forward..BsRouteBackCheck in one file; not done).
New tools: tools/lbexp.py FILE (compile + disassemble a scratch file), lbdecl.py (permute local declarations), lbbsc.py / lbptr.py / lbv.py (mechanical rewrites
of the auto drafts), lbfix2.py (second repair pass for .err.c), lbsweep.py (check all auto sources), lbleft.py [MIN [MAX]] (functions not yet written),
lbshow.sh / lbsrc.sh (auto source + align diff), align.py with RN=1 (register-renaming insensitive diff).
Left: see `python3 tools/lbleft.py` (about 400 functions, mostly browser: Bs* request/cache/memory/URL/work, zlib/png glue, parsetag, layout/table,
tagAct_*, DispFontSize, item box 0x609770-0x60E330, plaza chat, eft25, http_test_*). The m2c drafts for all of them are in the scratch drafts dir (LBDRAFTS).
