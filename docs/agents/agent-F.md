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

Addendum (same session, later): more idioms verified by matching.
- Compound assignment matters: `bsw[0xE96C]++` / `x += 1` compiled with `lui at; addu at,base,at; lbu/sb disp(at)` per access like the original, while
  `x = x + 1` made MWCC CSE the address (`ori at; addu v1`); tagAct_604 matched only with `++`.
- Pass-through arguments: a call whose first arg register was never reloaded passes the caller's own argument (`tagprintf(a, &sp2C)`, `BsCloseCapDlg(1)` with
  the constant kept in a register, `To_BodyMain_RcvSrc()` takes none). When an instruction like `addiu a1,sp,0x2C` appears where ours has a0, a leading argument is missing.
- STATIC callees in the same TU: `bs_pul_wk`/`bs_psh_wk` (work pool) and `_inet_mem_get_free_cell_005E83C0` (cell allocator) are `static` in the original, which is
  why the callers keep temporaries in a0/a1/t0 across the call. They only match when the static helper and its callers are one contiguous registered run
  (lb_av01 0x5E9A20-0x5E9D44, lb_ao01 0x5E8330-0x5E8590); check.py then reports 1 insn off for the jal (static symbol) although the link is fine (FORCE_OK=name).
- Chains of equality tests on u8 fields that MWCC would merge into a range (case 0xB..0xF) are written as `switch` with explicit labels (To_ReqCancelWait).
- Loops with the test at the bottom after an entry jump (`b test`) come from `while (cond)`; `for(;;)` with break gives a top-tested loop (bs_url_end, not matched).
- Struct typedefs added to lobby_f.h for the browser: BSSYS (bsSys), BSWK (work object, 0x70 bytes), BSNODE (queue nodes), BSCELL1/2/4/8 index macros BSC1..BSC8.
Registered since the first note: lb_ao (cell allocator), lb_ap (BsUrlBaseClear, sjis2euc_sub), lb_aq (tagAct_604/145), lb_ar (tiny wrappers: BsParseInitialize,
inflateInit_, _png_malloc, font_data_clear, ItemboxWindow/Cursor, http_test_12), lb_as (mode dispatchers: BsPosterMode ... BsQuitMain), lb_at (BsBody07-15 wait-cancel
states), lb_au (BsQuit02_Push2, BsPoster00/06, BsPullPageWork/BsPushPageWork, BsCsMove06_CapWarn, To_ReqCancelWait, SetNextURL), lb_av (work pool, BsTextureFreeAll/Load),
lb_aw (tagoutprintf*, pos_cr), lb_ax (line buffers, tagprintf_cr), lb_ay (yes_no_select), lb_s15-26 (more tag handlers, parsetag_init, http_test_10 ...).


## Lobby session 4 (9 Oct 2026): near-match sweep, item box, plaza chat, eft25
Overlay 0x5C4E60-end. `tools/rebuild.sh` prints OK for all five modules after each batch. Everything below is in src/lobby/f/ and registered
as `lb_gNN` runs (lbf_runs.py with prefix lb_g<file>).
Third-party library code (not worth writing, skip): zlib + libpng glue 0x5E9ED0-0x5EE618 (inflate_blocks_reset .. _png_read_row; the Capcom
wrappers around it, plPNGSetContextFromImage and later, are game code). The crypto/SSL code is in agent B's range below 0x5C4E60.
New helper: tools/lbdbf.py FILE FUNC 'decl1|decl2|..' (tries every order of the given contiguous local
declaration lines, 3 compiles in parallel, keeps the best). It fixed s0/s1/s2 register swaps in bs_cache_queue_check, BsWorkInitAll, Plaza_log_id_chk,
plaza_chat_log_disp_line. More than 5 declarations is too slow (n!).
Idioms learned (all verified by matching):
- Leftover argument registers are real in K&R calls. When the original keeps a callee's argument registers untouched (`jal f; nop` with no `daddu a0,...`),
  write the call with fewer arguments (`Lb_move_common();`, `Lb_send_commer();`, `Fade_busy_ck();`, `BsRouteCurrent()`), and a parameter that is then no
  longer used after a call stops being saved in s1. Conversely `Lb_get_lb_rank(*(u8 *)0x3C733B)` and `font_print_uf(buf, 0xA, w, -0x7E)` pass real
  extra arguments (constants kept in a1/a3 by chance of the allocator).
- `if (a != 0) X else Y` with `beq/bne; nop; b` is a one-case `switch`: BsPoster05_RcvData (`switch (r[4]) { case 0: ...; break; default: x01 = 2; }`).
  m2c turns if-chains into switches and vice versa: Local_main is `if (x == 32) {...} if (x == 37) {...} return 0;` (code layout follows the if order).
- A shared final `return 1` is a label: `goto ret1;` from the middle (check_questLevelSelect); a case that ends `return 0;` and a trailing `return 0;` after the
  switch are two different layouts (guild_input_pass/message: `break` in the cases and one `return 0` after the switch, no `default:`).
- The last statement of the function decides whether an extra `b end; nop` is emitted: drop the final `return;`/`break;` of the last case
  (vs_square_exit, eft25_move), or add one (http_test_14: both blocks end with `return;`).
- Two-element byte copies (`r->x108 = a; r->x109 = b`) are a 2-byte struct copy `*(PAIR2 *)&r->x108 = *(PAIR2 *)&n->x108;` (loads both, then stores both).
- `*d++ = a; *d++ = b; *d = 0;` keeps `addiu d,2` before the last store, `d[1] = ..; d += 2;` gets merged (plaza_name_sprint, BsUrlEncode: three `*dst++ =`).
- u8 local `v` incremented in place: `u8 v = a1 & 0xF; v += 1; d[0x51] = (a1 & 0xF0) | v;` (tagAct_043/044). `u16 len` as a function parameter keeps the raw
  register and masks at the use site, with `daddiu` constants (stockTextField). K&R `long a` + `*(s8 *)&a` gives `sd a0,24(sp); lb a0,24(sp)`
  (CallBack_Result_SendChatMessageTU). `(u16)x & 0x40` (cast) is kept as andi+andi, `x & 0xFFFF & 0x40` is folded to one andi.
- `if (cond) return v; v = 3; return v;` (Lb_check_hotel) vs `if (a >= b) {} else v = 3`: the first gives the original `slt v1; bne`.
- Array-of-struct index (`cw[pl->id + 0x2BFE]`, `(s8)cw[id + 0x2BFE]`) fixed `addu v1,v0,a0` order in lb_check_mini_data / Lb_player_load. A cast pointer
  `(u8 *)(int)cw + (a & 0xFF) * 0x2FC` fixed the registers of Lb_room_member (only the addu order is left, see below).
- Counted loop `while (n-- != 0)` (BsWorkInitAll: `n = 0x200; if (n-- != 0) do { ... } while (n-- != 0);`).
- `(f32)` of a u16/u32 value produces the unsigned-int-to-float sequence, `(u32)(255.0f * a)` the float-to-unsigned sequence (eft25_t).
- Struct pointer args: `PLW *pl; (f32 *)(pl + 0xAC)` is pointer arithmetic by sizeof(PLW): cast to `(u8 *)` first (lb_check_target had this bug).
Header edit: include/lobby_f.h LBSYS.x78 is now u8 (lbu in vs_square_exit); nothing else read it as signed.
Written, not linked (compiles, believed equivalent; src/lobby/f/lb_ib.c, lb_pc.c, lb_e25.c are whole-file working copies):
- Item box 0x609770-0x60CE00 and the rest of the screen: Lb_ItemBox_open/mv, itembox_cursor_mv, itembox_stock/pickup/equipchange/sortup/sellout, ItemboxWindowX,
  ItemboxWindowCursorX, Disp_lb_item_box, kosuu/selling/yes_no/disp_cmd helpers. Matched exactly: ib_select_sub, disp_itembox_cmd, item_explanation,
  kosuu_disp_sub, selling_price_disp_sub, yes_no_disp_sub. The others differ by scheduling/register choices only (itembox_stock: the four `addu v0,s0,v0`
  vs `v0,v0,s0`; ItemboxWindowCursorX and ItemboxWindowX: float add order/registers). The original has `andi rX,zero,0xFFFF` constants
  (`ib[0xB] = (u16)0` in Lb_ItemBox_open, `(s16)(0 & 0xFF)` in Lb_ItemBox_mv): no source form found that stops MWCC folding them.
- Plaza chat (0x60D710-0x60E330): all 13 functions written. Matched: Plaza_chat_init, plaza_chat_log_disp_line, plaza_name_sprint, Plaza_log_id_chk,
  Plaza_ReibunEdit_i/mv. Near: Plaza_chat_move (2 insns swapped), Plaza_chatlog_mv (8), Plaza_disp_ReibunEdit (6), Plaza_disp_chatlog, plaza_disp_chat_log_sub.
- eft25 (0x60E330-0x610300): Eft25_set_pos, eft25_move/e matched; eft25_d, eft25_i, eft25_m, eft25_t written (struct E25/E25P in lb_e25.c,
  particle layout from the code: prim, idx, col[3], pos[3], alpha[2], ang, angspd, scale, prim ptr, time, rnd).
Near-matches left (real instruction differences, files in src/lobby/f): delay-slot class where the original fills/does not fill a branch slot with
the next compare constant (bs_url_slash, BsBody00_ReqSrc, Lb_get_pl_stat2, u_item_chk, lb_send_data, lb_check_chair: 1-2 insns); static-callee IPA
(BsRouteReload, BsRequestHtmlPost, Lb_act_set, pick_kosuu_sel_chk: callee must be a static earlier in the same registered run); tagAct_500..504
(register order of three temporaries, tried permuter 12000 iterations and 5 source forms); Lb_PlStatusSet (a3/a0 base order); vs_square_event (one branch
target), vs_square_init (a0 reuse), stockButtonImage (`bgtz; nop; nop; b end` layout); BsQuit00_Init (store order); loops of the form
`bne; nop; b exit; nop; nop; b top` (bs_route_queue_free_reverse, bs_url_end, bs_url_last_slash); rodata struct copy of 12 bytes (pl_sleeping, Lb_put_gold).
The big ones (Lb_guild, lb_basic_master, Lb_stage_load, lb_rule_seet_set, quest table functions) were not touched this session.

## Lobby session 5 (10 Oct 2026): translation-unit groups (static callees), raw functions, near-match fixes
Overlay 0x5C4E60-end. New tools: tools/lbtu.py (merge the registered runs of an address range plus the not-yet-C functions into ONE C file,
unmatched/unwritten functions become `asm` stubs fed by config/c_rawfuncs.txt; build.py gen_raw now supports lobby), tools/check.py has
-Ibuild/raw, tools/progress.py no longer counts raw functions as decompiled.
Why: MWCC only uses a callee's register usage ("IPA") when the callee is a `static` function defined EARLIER in the SAME file and every caller is
in that file. The original has such statics: a global loaded into a1 and still in a1 after the call (BsRouteForwardCheck), `r` kept in a0 across
`bs_route_current_page_status(r)` (BsRouteReload), a counter temp in v1 instead of v0 because `bs_page_status_flag_set` is static (BsRequestHtmlPost).
A static whose callers are partly outside the group cannot link (callers in raw functions are fine: they hold absolute jal targets).
Rules that held: scratch files must NOT live in src/ while tools/rebuild.sh runs (it compiles src/lobby/zz*.c); compile scratch copies in build/scr.
Source-form findings (verified by matching):
- `if (0 < n)` for a loop guard whose count is an s16 loaded value gives `slt at,zero,s0; beq` (original) where `n > 0` gives `blez` (eft25_i, eft25_d);
  `i++` on an s16 loop counter avoids the extra sign extension before the add that `i = (s16)(i + 1)` produces.
- switch label order: the compare ladder in the asm is the REVERSE of the order of the case labels in the source (eft25_i case 4: `case 0: case 1: ... default: case 2: case 3:`).
- `p->time = -i * 5 - 10;` (not `i * -5 - 10`) for the s16 particle index.
- A 20-byte float struct copy compiles to lwc1 x4 / swc1 x4 / lwc1 / swc1 only when the struct has f32 members: `struct F5 {f32 a,b,c,d,e;} t; t = *(struct F5 *)p;`
  and then access the s16 halves with casts `*(s16 *)&t.a`, `*(s16 *)&t.b` (Lb_put_2TF). An s16/u8 struct gives ldr/ldl copies instead.
- if/else-if chains (not switch) when the original compares the same u8 against 6, 7, 8 one after the other (eft25_m start).
- `(1 << *p) & mask` vs `mask & (1 << (*p & 0xFF))`: the second gave the original operand order of `and` (lb_check_chair).
- Local copies of the arguments (`int x = *(s16 *)A; int y = *(s16 *)B; buf[0] = 0; f(buf, x, y);`) move the store out of the delay slot (Plaza_chat_move).
- Using `lbCommer[id].name` again instead of the cached `name` pointer changed the s0/s1 order (lb_commer_message).
- `return x != 1 ? 1 : 0;` (not `return x != 1;`) gives the original branch-to-epilogue layout in small checkers (item_kosuu_sel_chk, u_item_chk); `if (id == 0) return 0; return X ? 1 : 0;`.
- A local `u8 *u = User_data;` declared BEFORE the int copy of the parameter (`u8 *u; int v; v = a & 0xFF; u = User_data;`) puts `u` into the freed a0 and `v` into v1 (u_equip_chk);
  `(u8 *)(i + (int)u)` / `(u8 *)(a0 + (int)u)` gives `addu idx,base` (the original operand order) where `u + i` gives `addu base,idx`.
- Callee register use matters even for register-allocated locals of the CALLER: pick_kosuu_sel_chk keeps `User_data` in a3 across the call only if item_kosuu_sel_chk
  is a static defined earlier in the same file AND itself matches (its a0-a2 usage is what makes a3 the first free register).
- The permuter works on a translation-unit function: build a file with the declarations plus that single function (strip `static`, asm stubs) and run
  `tools/perm.py lobby FUNC file -j1`; it needs asm/lobby/text to still contain the function, so run it BEFORE the function's range is registered.
- Large field offsets (> 0x7FFF, e.g. `bsw + 0xE96A`): the original forms the address in a register (`ori at,zero,0xE96A; addu v1,v0,at`) and uses it through a pointer variable:
  `u8 *p = bsw + 0xE96A; if (*p != 0) *p = *p - 1;` (pullTableImage, pushTableImage). Written as `bsw[0xE96A]` the compiler folds `lui at,1; addu; lbu -0x1696(at)` instead.
- `x = x * 10 + (c - 0x30)` as two statements (`x = x * 10; x = x + (c - 0x30);`) gives the original's early `addiu v1,a0,-48` (get_numeric_parameter2, found by the permuter).
- An invariant load that the original re-reads every loop iteration (`while (i < *(s32 *)(w + 4) - 1)`) needs `*(volatile s32 *)p` in the loop (tagoutprintf2).
- Gp-relative globals of 8 bytes or less must be declared with their real size (`extern char *BadHeaderList[2];`, `extern u8 Hn_Size[8];`), otherwise lui/addiu is emitted instead of gp addressing.
- m2c `if (v != 0) {} else v = s[x];` followed by `d[y] = v;` is `if (v != 0) d[y] = v; else d[y] = s[x];` (set_TH_TD_data_1st).
- Empty switch cases: the ladder in the asm is the reverse of the case labels; `case 0: break; case 1: {...} break; case 2: case 3: break;` gave the original (BsCheckLbsError).
- `a ? x : 0` with a compare of an unsigned byte against a constant: write `v[0x48] > 1 ? v : 0` (not `>= 2` / `< 2 ? 0 : v`) to get `slti at; movn` with the compare in `at` (check_upTD_rowspan2). A shared `return 0;` that the original reaches by jumping from several places is a `goto ret0;` (check_upTD_rowspan).
- Prototype args: floats in the PS2 ABI do not use up integer argument registers in MWCC: `drawString(int pal, int a1, int a2, f32 x, f32 y, int size, u8 *s)` needs two dummy ints so that size lands in a3 and the string in t0.
- Struct locals built for GS packets (BSQUAD/BSSPR/BSTRI): fill the fields in the order of the original stores (BsDrawSprite stores the colour first).

### Lobby session 5: what is linked and what is left
Linked this session (all `tools/rebuild.sh` OK): lobby 144.7 KB -> about 156 KB of matching C.
- Translation units with static helpers: src/lobby/f/lb_tu_browser.c (0x5E5F90-0x5E8330: queue/route/cache/request code, statics
  bs_route_queue_forward/back, bs_route_current_page_status, bs_page_status_flag_set; aliases for still-asm callers in config/lobby_aliases.txt; unwritten
  functions are `asm` stubs, e.g. BsCacheInitialize, BsRequestCheck), lb_tu_act.c (0x5CDF70-0x5CF100: static lb_action_timer_calc, Lb_act_set now matches),
  lb_tu_ib.c (item box 0x609750-0x60D6D8: u_item_chk, u_equip_chk, item_kosuu_sel_chk, pick_kosuu_sel_chk, equip_ok_chk matched; the other 15 functions of the
  screen are still `asm` stubs in config/c_rawfuncs.txt, their C is in lb_ib.c / lb_ay.c).
- lb_e25.c: eft25_i, eft25_d now match (with Eft25_set_pos, eft25_move, eft25_e only eft25_m and eft25_t are left: eft25_m is a 3.2 KB function whose
  prologue shows five table base registers and two spilled locals; eft25_t 2.5 KB).
- lb_pc.c: Plaza_chat_move, Plaza_disp_ReibunEdit match; left: Plaza_chatlog_mv (7 insns), Plaza_disp_chatlog (38), plaza_disp_chat_log_sub (64).
- Other source-form fixes: lb_commer_message, lb_check_chair, Lb_put_2TF, tagAct_050/052/053/310/318/320/339/341/342/349, BsBody05_ActDsp, BsTextureGet,
  RequestAllImages, get_numeric_parameter2 (permuter) ...
- New hand-written (unwritten before): src/lobby/f/lb_dr.c, lb_dr2.c, lb_dr3.c: fillRect, fillTrgl, drawHLine/VLine, drawOuterImage, drawString, BsDrawRectangle/
  Triangle/Sprite, http_test_proc/_01/_06/_18, HttpTaskInitialize/Pull, is_sjis, BtnScrollXY, DispFontSize, BsFixPalInit, BsUrlBadHeaderGet, BsCheckLbsError,
  PostLbsInfoGetOrGameEnd, BsRequestCancelAll/Html, BsStrtblGet, tagAct_035/042, tagoutprintf2, pull/pushTableImage, set_TH_TD_data_1st, check_upTD_rowspan(2) ...
Still near-matches (source in the working files / build/lbauto, not linked): drawRect (20 insns), Disp_TABLE_Line (2), tagAct_602 (7), font_data_off, set_align_data (15),
check_rowspan (41), check_rowspan2 (7), ResetFormParam (35), check_special_character (register order of 8 locals), BsParseCheck (register order),
cut_spacer_string(_t) (loop layout), BsTextureAdd (1), get_input_tag_sp_type (8: `daddiu` li in delay slots), setUpDnLtRtBlank (7: decl order), lb_insert_target_list,
BsCsMove07_NetError, Disp_Text, the ItemboxWindowX family and the itembox_* screen functions (structurally far: User_data base kept in a register).
The permuter (one function at a time, -j1, 5 minutes each) found get_numeric_parameter2 and equip_ok_chk; it is cheap to try on any function that is
under ~10 instructions off: `python3 tools/perm.py lobby FUNC file.c -j1 --stop-on-zero` (build/asmkeep keeps a copy of asm/lobby/text for functions that are
already registered).
Ideas not done: write http_test_00/04/05 (m2c switch output needs hand cleanup), table/layout code 0x5FD000-0x608D00 (about 60 functions), the drawing
functions 0x5DBA80-0x5E0F00 (drawInnerImg5/6, DrawPageObj, DrawPulldown, ...).

## Lobby session 6: chain assignment, mutation tools, near-matches
- `andi rX,zero,0xFFFF/0xFF` in the original is a CHAIN ASSIGNMENT through a narrower member: `F(s8, ib, 0xB) = F(u16, ib, 8) = 0;` (inner store sh/sb zero, outer store gets the unfolded masked
  zero). Fixed Lb_ItemBox_open (linked), Lb_stage_load (lb_pl_place inline: `pl->ang[1] = *(u16 *)&pl->ang_y = ang;`, linked; ang_y is s16 in PLW so cast), Lb_ItemBox_mv (chain u8->s16).
- Linked: sortup_idx_chk, kosuu_select, ItemboxWindowCursorX (statement order x,y,w,h found with a permutation script), Lb_ItemBox_open, Lb_stage_load, Lb_npc_mv, guild_trans_ot0, BsQuit00_Init, tagAct_602 (`v > 1` not `v >= 2`).
- `(int)row + idx * 24` (not idx first) fixed the St_unique_tbl row address order. A static inline helper is not copied into lbf_runs run files: add it by hand.
- Tools used (scratch, build/scr, not committed): hill-climb on safe source edits (>= / > swaps, ++ forms, adjacent assignment swaps) scored by the align metric took Lb_guild 39->10, lb_basic_master 21->12 differing insns.
- Still near-match: item box itembox_stock (4), cursor_mv, sortup, pickup, equipchange, sellout, ItemboxWindowX, Disp_lb_item_box, Lb_ItemBox_mv (6, scheduling only); eft25_m/t (frame differs); Plaza_chatlog_mv (7), Plaza_disp_chatlog, plaza_disp_chat_log_sub; Lb_guild (~10), lb_basic_master (~12), lb_rule_seet_set (large).

## Lobby session 7 (village first): item box, Lb_guild, Clear_lobby_ram
Linked (tools/rebuild.sh all five OK): Lb_ItemBox_mv, itembox_stock, itembox_equipchange (in src/lobby/f/lb_tu_ib.c, removed from c_rawfuncs; jump table
rodata 0x668440-0x668460 registered), Lb_guild (src/lobby/f/lb_u.c, run 0x5C5F30-0x5C6F34 + rodata), Clear_lobby_ram (new src/lobby/f/lb_n09.c).
Findings (verified by matching):
- A store through an absolute-address cast (`*(s16 *)0x39DAD2 = x`) makes MWCC assume it may alias any pointer, so it re-reads globals (ib, cw, mhRule index)
  after the store. Declaring the address as its own extern object (`extern s16 D_39DAD2[16];` plus `D_39DAD2 = 0x39DAD2;` in config/lobby_aliases.txt) removes the
  reload and matches the original (itembox_stock, Lb_ItemBox_mv, Lb_guild: D_3F360A/D_3F33DC, Clear_lobby_ram: D_3F3415). Objects larger than 8 bytes avoid gp addressing.
  Use the s16 form where the original stores with sh; casting to u8 gives sb.
- `F(s16, ib, 8) = F(u8, ib, 0xB) = 0;` chain assignment explains `andi t0,zero,0xFF` (Lb_ItemBox_mv).
- Address of `base + idx*4` in the original is `addu idx,base`: write `(F(u8, ib, 0xB) << 2) + (int)u` (itembox_stock); the `u = User_data` local at function
  top makes the original's s-register (pickup/sellout/equipchange keep it in s1/s2).
- `u8 *sp5 = w + 5; switch (*sp5)` and one later `*sp5 = x` gives the original's hoisted `addiu a2,a1,5`; `u8 *p = w + 0x1F;` assigned before the `if` gives the hoisted
  `addiu a0,v1,31` in the branch delay slot (equipchange). One shared `return pad;` after the switch (cases `break`) removes duplicated delay-slot moves.
- A byte compared in `st & (cond ? 1 : 2)` allocates differently when `st` is int instead of u8 (equipchange); inlining a temp mask removes an extra register.
- Statement order of three independent stores before a call matters (permute them), see itembox_equipchange `-1; 0xFF store; pad = 0`.
- Permuter on a raw-asm function: write the .inc words into a snapshot `.s` (glabel/endlabel) and use PERM_ASM_DIR (relocation penalties are constant).
Still near-match: itembox_cursor_mv (decimal part matches with int temporaries, `daddiu` slot fill and hex-part registers differ), itembox_sortup/pickup/sellout
(original keeps pad, User_data and a third value in s0-s2, frame 80; mine allocates two), ItemboxWindowX, Disp_lb_item_box, lb_rule_seet_set, lb_basic_master (delay-slot
fills), Lb_room_member (`addu` operand order, 1 insn), lb_trade_result (arg load order in the call delay slot).

## Lobby session 8 (village first, lobby tail 0x5C4E60-end): item box, lb_basic_master, lb_a
Linked (tools/rebuild.sh all five OK): itembox_sortup (lb_tu_ib.c), lb_basic_master (src/lobby/f/lb_r.c, rodata 0x664DA0-0x664E3C as ONE
`lobby:rodata` line: two lines for one object push the later rodata by 16 bytes), lb_trade_result / lb_set_pl_status / lb_set_pl_pos (lb_a04-06.c).
yes_no_select is now a K&R definition (`void yes_no_select(pad) u16 pad; {`, declared `void yes_no_select();`) so callers pass the raw register
(`daddu a0,s2,zero` instead of a re-masked copy); it still matches.
Lessons (each confirmed by a match unless marked):
- tools/align.py hides branch-target differences. lb_basic_master looked "4 off" there while four early exits jumped to the wrong block (the original
  returns straight to the epilogue, mine fell through to the tail code); only check.py (or rebuild) shows them. Judge by check.py, not align.py.
  A rebuild mismatch of +16 bytes whose first difference is a lui/addiu low half far later = a rodata slot problem, not a code problem.
- Early exits that the original jumps to the epilogue for: `if (!(cond)) goto done;` plus a final `done: return;` (a plain `return` gives an inverted
  branch around a `b`). A case ending `return` vs `break` decides whether its branch goes to the epilogue or to the shared tail; an empty-looking
  `default: return;` that the original does not have must be dropped (the default target is then the end of the switch).
  `if (a == 0 || a == 0x55) { body } return;` instead of `if (a != 0 && a != 0x55) return;` moves a branch target to the case's own return.
- A pointer local that the compiler would fold away (`p3 = w + 3`) but that is used by a call argument in a case (`ListSelect(p3, pad, 2)`) is hoisted by
  the scheduler into the delay slot of the first switch compare; that is how sortup gets `addiu a0,a1,3` there. Without it a0 stays the pad register
  and the constants/temps shift by one register.
- `first != F(u8, ib, 0xA)` (re-reading instead of using the local `second`) was found by the permuter and fixed the last diff of sortup.
- Slot index and idx*6: `((SW6 *)(u + 0x44))[idx]` gives `addu t0,idx6,s1` (index first) where `idx * 6 + (int)u` gave `addu s1,idx6`; `(u8 *)((int)(ib + 8) + col)`
  keeps `addiu v0,v1,8` as its own instruction.
- Absolute-address stores `*(s8 *)0x39DAD0 = 0` should use the extern objects D_39DAD0/D_39DAD2 everywhere in the item box functions (sellout 96 -> 60 differing
  lines, sortup the `lui at` hoist and nop slots); then the store sits in the delay slot of the `b` to the return block like the original.
- Prototype of a callee decides the order of argument loads around `jal`: `void Ud_item_stack(u16, int)` (lb_trade_result) gave `lhu a0` before the call and `lh a1`
  in the slot; with `(u16,u16)` the second load becomes lhu.
- Declaration order of two pointers (`PLW *pl; LBSTAT *st;` with separate assignments) swapped s0/s1 in lb_set_pl_status and lb_set_pl_pos; chain assignment
  `pl->ang[1] = *(u16 *)&pl->ang_y = p->ang;` removed a reload.
- `Disp_lb_item_box`: a temp for the flSin result computed before `w = ib` (permuter) removed 14 differing lines.
- A permuter run on a function that is only a raw `asm` stub in the TU: write the .inc words into a snapshot `.s` (glabel/endlabel), set PERM_ASM_DIR, give the
  permuter a file = lb_tu_ib.c header + the one function (static prototypes made extern). 2 iterations/s for 100-insn functions, 0.3/s for 500+. Scratch helpers
  (put.py, sc.sh, dperm.py, permprep.sh) lived in build/scr2 and are not committed.
Still near-matches: itembox_cursor_mv (cm6 variant: only `daddiu` vs `addiu` for the constants 9/0 in the decimal block; int lo/hi gives the right registers, u8 gives
daddiu but premasks), itembox_pickup (check.py 461/541 but align.py 256 lines: p5 = w + 5 lands in a1 where the original has t0, loop registers shifted by one),
itembox_sellout (check.py 34/412 differing insns, align.py 42 lines, nearly all one register shift: w in a1/const 1 in a0 where the original has a2/a1), Disp_lb_item_box (about 110: w in t0 not v1),
ItemboxWindowX (about 840), eft25_m/t (frame 336 vs 320, an f20 callee-saved float the original does not use), lb_rule_seet_set (500+), Lb_room_member (1: `addu v1,a0,v0` vs
`addu v1,v0,a0`), lb_set_pl_stage (47), Plaza_chatlog_mv (5: v0/v1 swap in the scroll-up block), Plaza_disp_chatlog, plaza_disp_chat_log_sub.

## Lobby session 9 (village first, lobby tail 0x5C4E60-end)
Linked (tools/rebuild.sh all five OK; village unless marked): Lb_PlStatusSet (permuter), Lb_get_pl_stat2, Lb_put_hint, Lb_put_job, lb_select_quest_level, Lb_load_player_all,
lb_set_key_quest_local, Lb_PlayerStatus, Lb_put_button, Lb_put_icon (+ its jump table as `lobby:rodata`), Lb_put_gold, pl_sleeping, Lb_talk_check_default, Lb_move_common,
vs_square_event, vs_square_init_init, vs_square_init, lb_member_outCheck/changeCheck/inCheck, lb_rule_seet_trans_ot (permuter). Nothing online-only was linked.
Finding: most `*_nm.c` / whole-file drafts (lb_af.c, lb_ab.c, lb_ag.c, lb_aa.c, lb_p.c ...) are one or two source-form fixes away; run `python3 tools/check.py FILE --module lobby -v`
(`--module lobby` matters: names such as Lb_put_icon and lb_put_room_member also exist in game) and read the `>>` lines.
Source-form lessons (each confirmed by a match):
- Local aggregate initializers are `.data` objects, not rodata literals: `f32 q[3] = {0.0f, 20.0f, 0.0f};` (pl_sleeping) and `IBICON t = {460, 28, 140, 22, 0x80FFFFFF, ...};` (Lb_put_gold)
  compile to the original `ld/lwc1` / `lq/lwc1` copy through pointer registers; register the object with a `lobby:data START END file` slot over the old `lit_NNN` address.
  A switch with a jump table needs its table as a `lobby:rodata` slot too (Lb_put_icon 0x664C50-0x664CB0).
- A 4-byte `typedef struct UV { s16 u, v; }` copied as a whole (`q.uv0 = b->a; q.uv1 = b->b;`) gives the lh/lh/sh/sh pairs through pointer registers (Lb_put_button/Lb_put_icon);
  member-wise s16 copies interleave the loads. Pointers to struct members (`s16 *py = &q.y`) are assigned where first used and declared in the order the registers go (t, ph, py, pw).
- Calls with fewer arguments than m2c shows: the extra register values are stale (Lb_put_hint `sprintf(buf, fmt)`; Disp_NowLoading(), fade_set(0xA), Lb_move_common(m)).
  Calls with MORE arguments than the draft: lb_check_mini_data(i, c, c + 0x1C), add_prim(..., 0).
- `return cond ? 1 : 0;` for a bool from a compare (Lb_get_pl_stat2).
- Return-block layout: an explicit `return 0;` that the original keeps as its own block (`b epilogue` with the value in the slot) must follow the whole if/else it belongs to,
  the last `return 0` is plain fall-through (Lb_talk_check_default). A `return;` after an inner switch becomes `break;` when the original jumps straight to the epilogue (vs_square_event);
  an extra trailing `return;` in the last case emits an extra `b` (vs_square_init_init).
- A function whose result nothing keeps is `void` (Lb_move_common: `int` + `r` cost a callee-saved register).
- `(u16)ran_suu(1) % n` instead of `(ran_suu(1) & 0xFFFF) % n` (ran_suu is s16 in the header). `0 > f()` gives `slt at,v0,zero` where `f() < 0` gives bgez.
  `0 < n` gives `slt at,zero,n; beq` where `n > 0` gives blez.
- `u8 *p3 = (k = cw) + 3;` used by the compare and the memcmp hoists the `addiu a0,v0,3` before the lb (Lb_move_common).
- s8 loop counters: `int i; ... i = (s8)(i + 1);` (no sign extension before the calls) and `(s8)i` only where the original extends it; `s8 i; i = i + 1` or `i++` add extensions.
- Declaration order decides which callee-saved register a local gets: the FIRST declared local gets the HIGHEST s-register (lb_member_outCheck: s5 result, i, c, off, pl, lp = s5..s0).
  `u8 *base;` declared first (it sits in a0 only) then i, c, off, c2, pl matched lb_member_inCheck. tools/declperm.py (or a script permuting the declaration lines) found changeCheck.
- An absolute-address byte array of its own (`extern u8 D_6EABD9[16]; D_6EABD9 = 0x6EABD9` in config/lobby_aliases.txt) gives `lui; addiu; addu idx` instead of a folded offset (lb_set_key_quest_local).
- `my_user_id` is declared 16 bytes in lb_gaf06.c so that it is not gp-addressed (the real object is 8 bytes).
- Header: LBSYS `_pad70` renamed `x70` (lb_gq01.c and lb_q.c updated; proven by Lb_load_player_all/Lb_stage_load matching).
- Permuter noise that matches: lb_rule_seet_trans_ot needs an empty `if (mhRule.x4F && mhRule.x4F) {}` (scheduling of the lui).
Still near-matches (counts are check.py differing instructions): Lb_room_member 1 (addu operand order), lb_send_data 3 (delay-slot fill of the second switch compare), lb_insert_target_list 8 (head in a2: decl order did not
help), Lb_put_room_message 11 (y/len registers), lb_put_room_member_005CB220 9 (y/i registers), Lb_draw_square ~37 (pointer registers), Lb_send_chat_plus ~23 (params get the lowest s-registers in the
original, the highest in mine), lb_pl_turn_sub ~54 (temp registers), Lb_pl_to_chair ~80 (registers only, loop shape matches), get_flag_quest/get_new_quest ~25, lb_select_quest ~100, itembox_cursor_mv 2 (daddiu for the
constants 9/0: no integer type tried gives it), itembox_sellout 34 (w in a1 instead of a2; declaration order irrelevant), itembox_pickup, ItemboxWindowX, Disp_lb_item_box, eft25_m/t (frame 336 vs 320).
Not tried: lb_rule_seet_set, lb_guild_make_room, lb_questpage_trans, lb_set_questpage_info, Lb_make_quest_tbl(_local), lb_disp_name, Lb_put_help, lb_put_sprite, Lb_check_target, lb_target_angle (drafts are far).
Also linked late in the session: lb_questpage_trans (`if (Lbs_InRoomCheck() == 0 || i != 3) Put_msg(p)` shares one call; s3,s2,s1,s0 declared first, second loop with its own p2/j declared after i/p).
Near-match worth finishing: lb_guild_make_room (15 differing insns: only the mhRule.x5C bit-field update order; cases must `break` to one shared `return 2`, inner switch case labels in source order 0,1).

## Lobby session 10 (range 0x5C4E60-0x5EE618)
Linked (rebuild OK x5): Lb_pl_to_chair, Lb_check_receipt, lb_pl_mv088, Lb_check_target, lb_target_angle (src/lobby/f/lb_fz01-05.c), Lb_put_unique_act_hint (now C inside lb_tu_act.c, raw entry removed, jump table rodata 0x664CE0-0x664D50).
Lessons: m2c-style drafts hide the real structure; re-derive from the asm. `int t = p[0]` (not u8) lets a call arg be raw `daddu a2,s0` while the switch scrutinee is masked (Lb_check_receipt);
`switch` ladders (not `||` chains) when the asm has separate beq compares (merged into range tests otherwise); a float param placed LAST in the prototype puts `mov.s f12` in the call delay slot (lb_check_target);
u32 args converted with `(f32)` give the bltz/srl unsigned convert, int locals for `0xFFFF - d` (lb_target_angle); `e += n;` before the `if (i >= max || e == 0 ...) return;` puts it in the delay slot (Lb_pl_to_chair);
loop-carried counters kept in a u16 var that is summed but never used survive (cnt in Lb_check_target); u16 local used in `pad & 0x20` gives the double andi.
Near-matches: lb_guild_make_room 15 (x5C update order), Lb_put_room_message 2 (delay slot of beq on x load), Lb_room_member 1 (addu order), lb_send_data 3 (if-false branch lands on a `b end` block), lb_insert_target_list 8, lb_pl_turn_sub ~43, get_flag_quest 28, lb_set_pl_stage 47. Item box / eft25 / browser moved to agent B.

## Lobby session 11 (range 0x5C4E60-0x5EE618, village first)
Linked (rebuild OK x5, all village): Lb_put_room_message (lb_v05.c), lb_put_room_member_005CB220 (lb_e10.c), lb_select_quest (lb_x02.c), lb_set_questpage_info (lb_v06.c),
lb_select_quest_level_trans (lb_v07.c), lb_put_sprite (lb_ag03.c). Lobby 30.20% -> 30.80%. Nothing online-only linked.
Method that worked: read the ORIGINAL asm (asm/lobby/text/NAME.s, it has symbol names and %hi/%lo), write the C from it, and judge with `tools/align.py FILE FUNC`
(real differences only); check.py counts are inflated by shifted branch targets. m2c drafts were far off only in a few systematic ways:
- Lobby data the original reaches with `lui at; lb -N(at)` (lb_sys fields, mhRule) is fine as a cast absolute address (`*(s8 *)0x6EAE78`); but an address the original forms with
  `lui; addiu` as an argument needs its own alias object (`extern char D_6EAE5A[]` + config/lobby_aliases.txt `D_6EAE5A = 0x006EAE5A;`, same trick as D_6EABD9).
- Two copies of a 4-byte struct (UV) whose source is `tbl + off` and `tbl + 4 + off`: the second source is written `(u8 *)(tbl + 4) + off` (offset kept on the symbol), the
  destinations are two separate pointer locals; one pointer with +0/+4 merges them. Same for `(char **)(lb_quest_all + 199)[k]` instead of `(u8 *)lb_quest_all + 0x31C + k * 4`.
- A loop-invariant address (`(char *)exp + 0x3EC` inside a strcat loop) is hoisted into an s-register unless written `(char *)(exp + 0x3EC)`.
- `s0 += 2; ... s0[0]` folds the add into offsets; two statements `s0++; s0++;` keep the real `addiu s0,s0,8`.
- 16-bit offsets: an s16 local gives plain `addu` in `sp.x += xo` and the if/else with the then-constant in the branch delay slot; an int local gives movz or extra dsll32/dsra32.
  `sext = (s16)xo` into an int local gives the extension that is then reused (lb_select_quest_level_trans). Passing an s16 variable to a K&R function re-extends it: give the
  callee an s16 prototype (Sel_csr_disp, font_print_double, Lb_put_icon in the files that use them) and the extra dsll32/dsra32 disappear (also removes the explicit (s16) casts).
- `s16 a3 = 30; if (c) a3 += 30;` is NOT constant-folded (an `int` with `a3 = (s16)(a3 + 30)` is); with the s16 prototype the call needs no extra extension (lb_put_sprite).
- Remaining part-2 locals of a long function: random permutation of declaration order (script of 30 lines, scored by check.py) found the register map in a minute (lb_select_quest_level_trans).
- A `switch (x) { case 0: case 0xF: case 8: break; default: return; }` is how the original gets three `beq` + `b end` for an early exit (lb_disp_name top).
Near-matches left: Lb_room_member 1 (the cast form `(u8 *)(int)cw + idx*0x2FC` gives addu operand order wrong, `idx * 0x2FC + cw` is 8 off), lb_send_data 3 (the second-switch
default block must be reached from the flag test, goto/label variants get threaded away), lb_insert_target_list 8 (head must be in a2, K&R decl blocks declperm), lb_guild_make_room 15
(x5C update: bit-field and temp variants all worse), get_flag_quest 25 (tbl gets the param register instead of its own s4), Lb_send_chat_plus 23 (params must take s0-s2), lb_disp_name
(draft has a wrong 1.25f*w locate argument: original uses (f32)(int)scr[0]; half = len / 2; rewrite from the asm, scr[1]/scr[2] must NOT be hoisted), Lb_put_help (not started).
Never touched: lb_rule_seet_set, Lb_make_quest_tbl(_local), lb_set_pl_stage, lb_pl_turn_sub, Lb_draw_square, get_new_quest and everything from BsParseCheck/http_test on (browser).

## Lobby session 12 (range 0x5C4E60-0x5EE618, village first, then room/member code)
Linked (rebuild OK x5): Lb_draw_square (lb_v09.c), lb_set_pl_stage (lb_v10.c), get_new_quest (lb_v12.c), get_flag_quest (lb_v13.c). The last two use `#include "types.h"` plus a local
`int ran_suu();` because lobby_f.h declares ran_suu as s16, which adds a dsll32/dsra32 pair that the original does not have.
Lessons:
- Lb_draw_square: the original keeps &q[1], &q[2], &q[3] in registers; writing `s16 *p1 = &q[1]` etc. (p3 assigned just before its first use) reproduces it; y1 is an `int`, the
  third line group must be `*p1 = *p3 = y; q[0] = sx;` (the q[0] store ends in the jal delay slot).
- lb_set_pl_stage: `me = &player_work[game_w.master]` must come BEFORE `pl->stg = *stg` (it fixes the load order); K&R `int id` and `Lb_clearChatMember((s8)id)`.
- get_new_quest / get_flag_quest: declarations in the order k, i, n (the compiler assigns s-registers in reverse), `v = (ran_suu(1) & 0xFFFF) % n; i = v & 0xFF;` (a temp, then the mask),
  `if (0 < n)` gives `slt at` + beqz (n > 0 gives blez), `if (*e != 0x90 || Quest_clear_bit_ck(0x94) == 1) return *e;` shares one return. In get_flag_quest the 0x67..0x6A test needs a `u8 w = i;`
  temp (`if (w < 0x67 || w > 0x6A)`) to get `andi` + `slti at`.
Near-matches left (not built): lb_disp_name (lb_v08_nm.c, ~140 of 364 words differ: register map is off - s-register for lb_player pointer/len/px/py, the original keeps spA0 on the stack),
lb_pl_turn_sub (lb_v11_nm.c, 45: the original keeps a1 as a u16 local with daddiu constants and no mask on use; mine masks), lb_insert_target_list (lb_v14_nm.c, 8: head must be a2, compare in `at`),
Lb_room_member 1 (addu operand order; tried 14 spellings), lb_send_data 3 (Lbs flag branch must jump to the default block, not past it), Clear_lobby_ram is already linked (lb_n09).
Not started: Lb_put_help (m2c goto draft), lb_rule_seet_set, Lb_make_quest_tbl(_local), lb_guild_make_room.

## Lobby session 13 (range 0x5C4E60-0x5EE618, village first, then online)
Linked (rebuild OK x5): Lb_put_help (lb_v15.c), Lb_make_quest_tbl_local (lb_v16.c), Lb_make_quest_tbl (lb_v17.c, jump table 0x664AC0-0x664AD8 as `lobby:rodata`). Lobby 31.54% -> 32.06%. All three are village code
(guild quest table / hotel help line); no online-only function was linked.
Lessons (each confirmed by a match):
- `extern u8 lb_quest_clear[8];` (explicit size <= 8) is gp-addressed like the original (`addiu a1,gp,..`); `u8 x[]` is not. Same idea as the my_user_id 16-byte trick, the other way round.
- A 5/6-round counted loop is only left un-unrolled when written as `do { ... } while (m < 6)` with explicit pointer locals (`tp += 4; cp++; p += 5`); a `for (i = 0; i < 5; i++)` gets unrolled.
- Declaration order of those pointer locals decides the temp registers of the first loop (m, t, tp, cp, p = highest to lowest); the loop counter of a long second loop is a different variable (`i`).
  Spilled locals (stack) get slot addresses in declaration order, later declared = lower address (lv, rnd, flag, sel, cnt ... = 0x100, 0xF0, 0xE0, 0xD0, 0xC0).
- `k = 5; if (t[5] != 0) { do {...} while (t[k] != 0); }` puts `k = 5` in the delay slot of the test; `if (i != 0 && i != 1 && (k = 5, t[5] != 0))` does the same behind other tests.
- `u32 r = ran_suu(1) & 0xFFFF; (r >> (k % 16)) & 1` gives srlv with the `bgez/andi/addiu` mod sequence; `(u16)ran_suu(1) % (u32)(5 - flag)` gives divu; `n = flag + (u16)ran_suu(1) % (u32)(...)` in one expression gives the original addu operand order.
- A local u8 `sel`/`cnt` that the original keeps in a stack slot is just a normal local that ran out of registers; no volatile needed. Uninitialised `rnd` is read once before it is set (stack garbage in the original, kept).
- `if (key_quest != 0x67) {else-part} else {0x67 part}` gives the original block order (beq to the 0x67 block).
- Lb_put_help: `switch (lb_sys.x68) { case 0: case 0x27: case 7: case 8: break; default: return; }` for the early exit (labels in reverse ladder order); the nested hotel-price switches list 0, 1, 3, 2 with 1/3 falling through into 2 (the real order is the reverse of the compare ladder);
  `*(u8 **)((int)pl + 0x878)` for the second read (a `F(u8 *, pl, 0x878)` twice gets the address CSE'd into a spare s-register); lb_sys+0xA / +0x28 as the alias objects D_6EAE5A / D_6EAE78 (config/lobby_aliases.txt) so every use is `lui; addiu` again; `PLW *pl = &player_work[*(u8 *)0x3F34C1]`.
- lb_guild_make_room: all cases `break` to one `return 2` (explicit `return 0` / `return 1` only where the asm jumps to the epilogue); inner switch labels in the order 0, 1 (reverse of the compare ladder); `int s1` counter with `if (0 < RoomRule[0x97])`.
  Down from 136 to 16 differing insns (src/lobby/f/lb_v.c). The rest is the order of the two mhRule.x5C updates.
Online (non-browser) functions left in the range, largest first (all are near-matches, browser excluded):
  1. lb_rule_seet_set 2356 bytes (lb_w.c, rule sheet editor; first block order and register use far off, not started in earnest)
  2. lb_guild_make_room 772 bytes (16 insns, lb_v.c)
  3. Lb_send_chat_plus 288 bytes (about 23: params must take the lowest s-registers)
  4. lb_send_data 216 bytes (3 insns, lb_n.c: the Lbs flag test must branch to the `b end` block after the switch ladder; the original also has a nop before it)
  5. lb_insert_target_list 192 bytes (8: head must be a2, the loaded half-word in v1; a `u16 w` temp, declaration orders tried)
  6. Lb_room_member 88 bytes (1: addu operand order)
Village near-matches: lb_disp_name 1456 bytes (lb_v08_nm.c, now 160 insns: stack layout fixed by declaring sp100, spF0, spB0 in that order; the (s16)(1.25f * spF0[0]) of the second locate has no (s32) cast; s-register map still off: original keeps
(cw + off + 0x1346) in s23 via `c = cw + off; q = c + 0x1346; t = c[0x1347]`, x in s21, y in s17, i in s20, row in s22, off in s30), lb_pl_turn_sub 520 bytes (lb_v11_nm.c, 31: a1 is a u16 with daddiu constants, loaded half-word goes to a0 and the +0x300 sum to v1 in the original, swapped in mine;
final clamp is `if (a0 >= 0x8000) 0xF600 else 0xA00`).

## Lobby session 14 (range 0x5C4E60-0x5EE618, long round)
Linked (rebuild OK x5): lb_rule_seet_set (village, lb_w.c, text 0x5C78F0-0x5C8224 + `lobby:rodata 0x664A70-0x664A90`; 589 insns, from 505 differing to 0) and
Lb_send_chat_plus (online chat target list, lb_aa02.c). Lobby 32.06% -> 32.41%.
How lb_rule_seet_set went (all confirmed by the match):
- Read the asm and note where each path ends: `b epilogue` with `daddu v0,zero,zero` in the slot is an explicit `return 0`; a plain jump to the shared `daddu v0` block is `break` (the function ends in `return 0`). Outer `switch (x08)` case 0 / case 1, inner `switch (x4F)` has no default.
- `u16 pad = Get_sw2(0);` gives the original `andi s0,v0,0xFFFF` plus one `andi v1,s0,0xFFFF` per case that is then reused for every `pad & 0x...` test.
- A branch whose original asm has the `x4F = 7` byte store FIRST and the lui/ori constant after is written with the constant stores first in the source (`RDT(8) = K; RDT(0x1C) = K; mhRule.x4F = 7;`): the scheduler hoists the sb above them. Same for the init block's `mhRule.x00 = 3;`, which must sit after RDT(0x94) and before RDT(8) in the source.
- The first element of a pointer array passed to sprintf is `lb_rule_msg_etc[0]` (a `lw`), not the array name.
Lb_send_chat_plus (found with a script, not by hand): the original calls Lb_send_chat with THREE args (a, b, c); the s-register order then needed the statement order `bit = 1; cw[0x32BE] = 1; i = 0; p = ..; id = ..; bit = 1;`
(a repeated `bit = 1` is permuter noise that matches, like the empty `if` in lb_rule_seet_trans_ot) and `bit += bit`. Method: tools/perm.py found that a duplicated initialiser changes the s-register ranking; then a script tried every statement order of the 5 initialisers
(plus one duplicate) x declaration orders. Scratch scripts for that were not committed (loop over permutations, call check.py on a temp copy under src/lobby/f/zz_*.c, delete it).
Do NOT trust a permuter output blindly: several "improved" outputs change meaning (they assign to a parameter); read diff.txt first.
Near-matches left (check.py differing insns): lb_guild_make_room 16 (the two mhRule.x5C updates: original loads quest, then x5C, does the `and`, then the two sh stores, then or/sw; every statement order, temp variant, bit-field and chained-store variant tried: 15-17),
lb_send_data 3 (original fills the first switch-ladder `beq` delay slot with the next compare's `addiu`, and the Lbs flag branch lands on the `b end` block; permuter 7000 iterations found nothing), Lb_room_member 1 (`addu v1,v0,a0` operand order; ~400 spellings),
lb_insert_target_list 8 (head in a2: making `c` and `ang` extra K&R params gives a2/a3 but then prev/cur/loaded half-word registers move; 9), lb_pl_turn_sub 34 (lb_v11_nm.c: a1 is `u16` for the daddiu constants, work750 stores duplicated per branch, the original has no mask on a1 and the loaded half-word in a0, sum in v1),
lb_disp_name 138 (lb_v08_nm.c, stack layout now right with sp100, spF0, spB0 declared first; s-register map still off, original keeps `-1` in a register across the job and status calls, `q = cw + off + 0x1346` in s23).

## Lobby session 15 (plaza, 0x594260-0x59DB3C, plus village near-matches)
Linked (rebuild OK): plaza_moveMain, plaza_enterLobby, plaza_movePlaza, getFriendNow, plaza_searchAll, plaza_searchMember, plaza_setMyComment,
plaza_setChatMode, lb_put_comment, Lbs_plaza_trans, put_member_info, Lb_put_new_mail (all online/plaza, runs f/lb_pz01..11, TU f/lb_plz).
Lobby 33.356% -> 35.2%. Header edits: include/lbui.h plaza_moveMain int->void, LB_NETW.x0E carved from _pad0E. Aliases (config/lobby_aliases.txt):
D_3A2940, D_3A1622 (absolute reads of PlazaInfo/LobbyInfo entries), get_page_num (static in lb_plz.c).
Lessons (each confirmed by a match):
- `u16 sw = Get_sw2(0);` then `LB_NETW *a = pNet;` AFTER the call keeps `a` in a temp register (a before the call goes to an s register).
- One-case `switch (f()) { case 3: ... break; }` gives `beq v0,v1; nop; b end` (plaza_moveMain); `if (r == 3)` does not.
- Every path ending in `return 2` is `break` out of the switch plus one final `return 2` (shared exit); explicit `return 0/1/3` stay.
- `if (x > 1)` instead of `x >= 2` flips `slti at` vs `slti v0` (put_mail_input_square, plaza_setChatMode).
- K&R int params with explicit `(s16)` casts at calls: a variable that is only ever used narrowed gets narrowed once at definition; use int + cast per call (put_member_info, lb_put_comment).
- A call whose result is kept in an argument register after the call (`a2` read after get_page_num) needs the callee as a static function defined EARLIER in the same TU:
  tools/lbtu2.py (copy of lbtu.py that accepts runs in lb/ and b/ too) merges registered runs + asm stubs into one TU. lb_plz.c = 0x595F70-0x598DB0 does that.
  A TU holding several jump tables must cover its whole rodata range contiguously (objects have one .rodata): that blocked merging 0x594260 and up while plaza_checkFriend is not C.
- Statement-order search (24 permutations of 4 stores) fixed plaza_searchMember case 3; declaration order alone often changes nothing for temporaries.
Near-matches left: plaza_checkFriend 199/937 (src/lobby/f/lb_pz13_nm.c), Plaza_add_friend 15 (lb_pz12_nm.c: register numbers of sw/a/st), put_mail_input_square 17,
plaza_disp_mail ~73 (needs int ty narrowed per use), Lb_addChatMember 10 / Lb_clearChatMember 20 (lbui_nm.c), draw_dialog_square 8 (20.0f/tw float register order),
ConditionSearchUser 3, lb_mix_put_itemDetail 2 / lb_mix_decide 4 / shop_select_items / kyoukaListProg / lb_npc_old_guild 2 (village: whole-file TU of lb_mix_nm.c gave the same diffs).
Warning: tools/build.py compiles EVERY src/**/*.c, so never leave scratch files under src/ while a rebuild runs.

## Lobby session 16 (plaza, village near-matches)
Linked (rebuild OK x5): plaza_checkFriend (f/lb_pz13, standalone run; online/plaza), plaza_movePlazaTrans (f/lb_pz15, online), put_mail_input_square (f/lb_pz16, online),
lb_mix_put_itemDetail (lb/lbmixe, village; replaces the raw asm run b/lb_by178 and its c_rawfuncs line). Lobby 35.53% -> 36.25%. No shared header edits.
Lessons (each confirmed by a match):
- plaza_checkFriend went from 199 to 0 differing insns with: `u8 *q = &a->x12; if (*q == 0) ... *q = *q - 1;` (pointer computed before the test: the original has `addiu v1,a0,18` in the bne delay slot);
  `switch (SaveNetFile_ForLobby()) { case 1: case -1: ...; return 3; } break;` (a two-case switch gives `beq; addiu(next compare); beq; nop; b end`, labels in REVERSE order of the compare ladder);
  `switch (getFriendNow(...)) { case 0: ...; break; }` for a lone `== 0` that ends the function; the `0x3F36AB` byte is `system_w.softkey` (include/sysw.h): use the symbol, the scheduler then hoists the lw like the original;
  the memcpy shift loop needs `int kk` (not u8) with `memset(tl + (u8)kk * 0x2FC ...)` and a separate pointer `u8 *pp` declared after it.
- MWCC narrows an `int` that is only ever used as `(s16)v` ONCE AT THE DEFINITION. The original instead narrows per use (dsll32/dsra32 before every call) because the callee has a PROTOTYPE with s16 parameters. The headers
  declare these functions unprototyped (`int font_print_double();`), so a redeclaration is an error. Fix in the .c file (no header edit): `#define font_print_double font_print_double_hdr` before the include, `#undef` after, then
  `int font_print_double(s16, s16, int, int, char *);`. Same for flfntLocate(s16, s16) (not in lbui headers), put_titles(s16, s16, char *) and Put_page_num(s16, s16, int, int, int). Then pass `x2 = sx + 0xFC` un-cast, and keep
  `s16 y; ... y += 0x16;` for values the original passes raw (see lb_pz15.c). Do NOT put the prototype in a shared header: it inserts extra dsll32/dsra32 in every other caller (tried: lobby grew by 32 bytes).
- put_mail_input_square: `s16 q[6]` plus pointer locals p1 = &q[1], p2 = &q[2], p3 = &q[3] and `u32 *pc = (u32 *)&q[4]` assigned INSIDE both arms of the first colour if/else (the original has `addiu s1,sp,152` in each arm);
  declaration order (f, p2, p3, pc, sx, sy, p1) found with a script that tries every single-move reordering of the declaration block (check.py on a copy outside src/); a hill climb over declaration moves is cheap and worked.
- lb_mix_put_itemDetail: `*(int *)0x351E84` is `pit_help_str_tbl[1]`; `pit_help_str_tbl[1][id + 0x18]` gives the original operand order (idx first).
- `lb_by178.c` was a raw-asm run of the same address: when adding a C run, search c_files.txt AND c_rawfuncs.txt for the address first, otherwise the build is bigger (here +32 bytes, first difference far from the new code).
Near-matches (not linked, check.py differing insns): plaza_mailBox 4 (src/lobby/f/lb_pz14_nm.c: after the sw tests the original loads x26 into a1 with pNet in v0; mine v0/v1; every declaration order tried),
Plaza_add_friend 12 (lb_pz12_nm.c: sw/a/st are a1/a2/a3 in mine, a3/a1/a2 in the original), plaza_disp_mail ~110 (params are (a0 unused, x, z): x and z are passed raw to flfntLocate and t = (s16)x + 0x3C is narrowed per use;
ANSI `s16 x, s16 z` params plus a flfntLocate(s16, s16) prototype got closest, the register map and z update still differ), plaza_setMyCommentTrans ~175, plaza_enterLobbyTrans ~270 (draft with prototypes in this session was not kept),
draw_dialog_square 8 (float register order of 20.0f * tw, tried 3 spellings), lb_mix_decide 4 (m = mixData + cur: original puts the sll between lui and addiu), lb_npc_old_guild 2 (mv recomputed vs kept in a2).
Tools (scratch, not committed): adiff.py (aligns check.py -v output with difflib and prints only real differences, ignoring relocation noise), hill/rand declaration-order searchers.

## Lobby session 17/18 (whole-file TUs, village)
- Lb_put_materialItem matched (lb_by177.c, replaces the raw asm run): reuse `id` as the stock variable, cast `(s16)` per use, `if ((s16)need <= (s16)num + (s16)id)`.
- Plaza one-unit TU 0x594260-0x598DB0 (wip/lb_plz2_tu.c, NOT registered; agent C owns it now): `python3 tools/lbtu2.py lb_plz2 0x594260 0x598DB0` works. Needed by hand: rename clashing header declarations
  (`#define plaza_movePlaza plaza_movePlaza_hdr` ... around the include, then forward declarations), `X0A(p)` -> `X0Ap(p)` (function-like vs object-like macro clash) with `#undef/#define X0A` before plaza_movePlaza
  (a-based) and again before plaza_backToServer (pNet-based), `getUserInfo()` stub unprototyped. Result: all 23 functions OK on PS2, rebuild OK x5 (rodata slots handled by split_rodata_objects), BUT build_pc failed:
  `static get_page_num` clashes with the header's extern and the `asm` stubs include `.inc` files that gcc cannot read: wrap the asm stubs in `#ifdef __MWERKS__` and make get_page_num non-static for gcc.
  Putting the C versions of plaza_mailBox / Plaza_add_friend into the TU gave the same 4 / 12 differences as standalone.
- Village TU attempt 0x59DB40-0x5A2A1C (lbnpc, incl. lb_npc_old_guild): lbtu3 output compiles except for the two header families (lobby_b.h vs lobby.h: Item_data, lbShop, lb_pit, lb_sys, em_work, Lb_act_set ...); the
  `#define name name_hb` around the first include fixes all but lb_sys (a header-defined `lb_sys` macro). Not finished.
- set_dialog_square (lbui_nm.c, 36/43): the original keeps `t3 += 40` as real adds between record groups; every pointer/struct spelling tried gets folded into constant offsets by MWCC.

## Lobby session 19 (village whole-file TUs)
- The village runs (lbnpc*, lbmix*, lbui*) include lobby.h-family headers, NOT lobby_b.h: the header-family clash of session 18 disappears if the TU does not include lobby_b.h at all.
  tools/lbtu3.py now has `LBTU_NOB=1` (do not seed/emit lobby_b.h) and `LBTU_HDR=lbui_proto.h` (header every run includes first: seeded and emitted at the top). Then no lb_sys macro clash.
- Registered: f/lb_npc.c (0x59DB40-0x5A2A1C, 38 fns OK + lb_npc_old_guild as asm stub; C version still 2 off: the original loads 0x69 into a2, mine into v1; mv scope/recompute variants only made it worse),
  f/lb_mix.c (0x535240-0x536724; lb_mix_decide stays asm, 4 off: sll placement of mixData + cur and of player_work[...]), f/lb_ui.c (0x590D40-0x5931B0). All five modules OK.
- draw_dialog_square MATCHES in the TU: the near-match C passed a spurious third argument `Put_sprite_rotate(&sp, 2, tw)`; the original calls it with two (the K&R callee hid that).
- Lessons for TU building by hand: (1) a function K&R-defined in one run but ANSI-prototyped in another (CheckItemPrice, put_button_help, font_print_double, Draw_square) must keep its ANSI proto for those callers:
  lbtu3 now keeps the ANSI proto next to the K&R definition; for incompatible pairs use a `_a`/`_u` alias name with the ANSI proto in the callers + a line in config/lobby_aliases.txt. (2) Functions whose symbol has an
  address suffix (CheckItemPrice_005366D0) are looked up by suffix. (3) `_k` rename only if the function is really called (not just used as a pointer). (4) `#ifdef __MWERKS__ asm ... #else C #endif` b/ files
  (lb_by158/164/157) confuse lbtu3's chunking: delete the #else C by hand. (5) `Draw_menu_square` raw asm needs `asm int` (the header declares int).
- PC build: tools/pc_lobby_matched.txt and build_pc.sh still compile the old run files (src/lobby/lb/lbmix*.c, lbui*.c, b/lb_by89.c, b/lb_by180.c), so those were restored unregistered next to the new TUs. Do not delete them without editing the lists.
- lb_process_use_item (0x53A560): the b/ region 0x539220-0x53A9A4 forms a TU with lbtu3 (LBTU_NOB=1 LBTU_HDR=lobby_s.h, hand-delete the #else C of by164/157/158): everything OK except use_item, 14 off: the original forms
  `lui 0x67; addiu -9274` (shopList+0x26 as an address constant) after `i = 0` and adds n*40 to it, mine keeps the symbol base and folds +0x26 into the lhu displacement. Not registered (no gain yet).
- lb_process_use_item retry: local pointer `q = (u16 *)((u8 *)shopList + 0x26) + n * 20` before/inside the cases, `&((SI26 *)shopList)[n].x26` with a local struct typedef, and `&arr[n*20]` all stay 14 off (or get worse, 63-85 off when hoisted above the switch). The 0x539220-0x53A9A4 TU was therefore not registered.

## Lobby session 20 (village TUs, near-match retries)
- Merged: lb_uif TU (0x5931B0-0x594260) replaces by41/by42/by181/lbuif/lbuig. After `git merge main` the c_files.txt conflicts are resolved by taking our side and dropping lines whose src file no longer exists or that overlap C's lb_plz2/lb_plz3 TUs.
- Matched: event_eat_trans_ot0 (in lb_ui.c): `int x` -> `s16 x`. A local that is passed to a callee whose ANSI proto takes s16 gets a dsll32/dsra32 re-extension when it is int; the original had no extension, so the original local was s16. Check this first whenever align.py shows dsll32/dsra32 pairs before a call.
- Near-match retries that did NOT converge (all kept as asm stubs): value_result (9 insns; K&R `u16 v, u16 op` + u16 return fixes return and the 1..6 cases, the case 0 compare still lands in `at`), shop_select_items (443/485, structure differs), lb_process_drawHelp (m2c draft; its `value_result(.., 7, temp_a3)` call had spurious extra args, 295/423 off),
  lb_process_kyoukaListProg (9/376: with `F(s32,&shop_process2_help,N*4)` instead of `shop_process2_help[N]` (char[] in the TU gives lb instead of lw) only the last x6E+1 clamp block is off: original puts lim in v1, mine coalesces it with the ternary reg; permuter 800 iterations found nothing),
  itembox_cursor_mv (2: daddiu vs addiu for `lo = 9` in a branch delay slot; many variants tried), Lb_room_member (1: addu operand order), lb_guild_make_room (14), lb_eat_set (original keeps a dead `k` counter alive + frame 144), set_dialog_square, Draw_menu_square, lb_process_select (192/235).
- Tools used: scratch scripts swap/put/try2 (swap an asm stub for its nm C, try variants and count check.py differences). permuter on a TU function: perm.py's base.c needs the `asm ` prefixes removed (sed) before running tools/permuter/permuter.py directly.

## Clean-up round (10 Oct 2026)

How to find dead PC code without guessing (used for the 10 Oct clean-up, see docs/pc.md "History"):
- Which object a linked global comes from: the strong definition, else the first weak one in link order
  (build/pc/objs.txt; the front-end sources come first). Check it on the binary with
  `nm build/pc/mhview` + `addr2line -e build/pc/mhview 0xADDR` (the source file of the winning copy).
- A weak definition in src/pc/rt whose symbol resolves elsewhere is dead. An object none of whose globals
  wins is dead in the PC build. Do not judge by "nothing calls it": rt_data.c resolves pointers in the
  PS2 data tables to host symbols by name at run time (em_prog_tbl entries are only reached that way).
- After removing anything, build/pc/undefined.txt must not gain names (else gen_rt_auto.py silently makes
  a no-op stand-in for them).
- Host versions that still win over matched C (12 Oct): Material_set_sub (rt_eft.c): the game's loop hands
  flSetRenderState(0x3A + i) a pointer into the model's material table (MDLW +0x10), which the port's model works
  do not have (NULL), so it needs a real material table per model first; get_mdlw_ptr (rt_motion.c): the PC keeps
  its own model works, the game's light03.c indexes the PS2 model heap. get_joint_* are the game's since 12 Oct
  (rt_actor_nodes_fill keeps every live actor's node array filled).
- Still host no-ops: init_eft_work / init_shell_work (all_reset in the character editor): making them real moves
  the random stream of the whole boot (the scripted tests would need new seeds). Each em_work clear (village entry,
  clr_em_work) drops the slots' model works and new ones are allocated (a leak of ~50 KB per slot per clear).
