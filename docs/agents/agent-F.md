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
built as such: every .c is compiled but only registered ranges are linked); `tools/lbruns.py FILE PREFIX "comment"` splits the fully matching,
not yet registered functions into contiguous runs PREFIXNN.c and appends the c_files lines (FORCE_OK=name for functions check.py cannot verify,
e.g. a callee whose symbol carries an address suffix). Other helpers: tools/lbasm.py (compact asm), tools/lbd.py / lbconv.py (m2c drafts;
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
