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
