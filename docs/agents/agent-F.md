# Agent F notes: player code (asm/main/text/f_pl.s, 0x134950-0x14D1C8, 224 functions)

Status (6 Oct 2026): ~50 functions byte-matching and registered (src/main/pl/pl01..pl13.c,
`tools/rebuild.sh main` = main OK). Everything else written so far is in src/main/pl/pl_nm.c
(compiles, not built). Done in this round: pl09 (unique_act_set .. guard_atk_ck, 0x1371B0-0x138900),
pl10 (basic_kabe_ck, item_action_set, trade_get_ck, 0x138BE0-0x1398F0), pl11 scope_add, pl12
(pl_mv000/001/004/006), pl13 (pl_mv008, pl_mv013). Next in address order: pl_mv014 (0x13BF20), pl_mv017 ...
(pl_mv* are the PLPROG state handlers; draft with tools/pl_draft.py FUNC --add, then plconv.py + plclean.py).
Near-matches added to pl_nm.c: basic_com_ck (many branch delay slots filled differently), gun_adj_sub and
sougun_adj_sub (see IPA lesson), wall_act_ck/wall_vec_set (one commutated addu), pl_mv021 (prologue register shift).

## Near-matches in pl_nm.c
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
