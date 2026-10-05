# Agent F notes: player code (asm/main/text/f_pl.s, 0x134950-0x14D1C8, 224 functions)

Status (paused by owner): ~28 functions byte-matching and registered (src/main/pl/pl01..pl08.c,
`tools/rebuild.sh main` = main OK). Everything else written so far is in src/main/pl/pl_nm.c
(compiles, not built). Next function in address order: after stick_pow_get/em_ninshiki_ck (0x136D40/0x136E70,
both in pl_nm.c, near-match: stick_pow_get 3 off, em_ninshiki_ck 63 off) comes unique_act_set (0x1371B0).

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
- Raw pointer-cast accessors (`*(s8*)((u8*)p+off)`) change codegen (CSE of the address); use real struct fields.
- `x > 100` gives `slti at` (value kept); `x >= 101` puts it in v1. `(u32)(a-3) > 1` gives `sltiu at`.
- Declaration order of s16 locals fixes t0/t1 swaps (item_sel_sub).
- `daddiu` constant loads come from u8/u16 locals (stick_pow_get, pl_init_sub case 2).
- A struct array of PL_ITEM {u16 id; s16 num;} item[20] at 0x828 explains the `i += 5` unrolled clear (Pl_item_charge).
- Float stores like `sw 0x3F800000` are f32 fields assigned float literals (see hexf.py).

## Shared header edits
include/pl.h (PLPROG, PL_ITEM, many work fields, flag14/15 u8, work72C u16, x73A merged), include/game.h (pl_state, x213-x216),
include/em.h (x7EE/x88F/x9EC comments). Struct merge done with tools/merge_struct.py.
