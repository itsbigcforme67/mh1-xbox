# Targeted decompilation: the game overlay and the village lobby (agent E, 8 Oct 2026)

Scope: `src/game` (game.bin) and the single-player lobby/village code (`src/lobby`, not plaza or online).
Agent B writes the `main` section of this file; this section is the part for the two overlays.

## How the list was made

* `tools/unmatched.py game|lobby` lists every function that is not inside a registered (byte-matched)
  range of `config/c_files.txt`. These are the only functions where the PC can run something that is
  not proven.  game: 40 functions, 48 KB (95.5 % of the overlay is matched).  lobby: 1740 functions,
  506 KB, but only 15 of them have any C at all (the lobby near-match files are mostly stale copies of
  functions that are matched in other files; the PC links the matched version, see
  `tools/pc_lobby_matched.txt`).
* Which of them really run: `COV=1 tools/build_pc.sh` builds with `-finstrument-functions`
  (`src/pc/rt/rt_cov.c`); with `RT_COV=file` every process appends the set of functions it entered.
  `tools/cov_report.py COVFILE BIN LIST...` maps that to names and marks the unmatched ones (RAN / ---).
  Not part of a normal build.
* Which of them are *wrong*: `tools/semdiff.py FILE.c [FUNC..]` compares the original and our compile
  of every near-match function as multisets of instructions with register names, relocation
  targets and branch targets erased. EQUIV = only register allocation / scheduling / block order
  differ. DIFFERS lists the instructions only one side has (a different constant, offset, compare,
  a missing or extra operation). It is a screen with false positives (jump-table bounds, signedness
  of promoted u16, inlining), not a proof; every DIFFERS was read against the asm.
* Host code that stands in for game/lobby functions: `nm` of `rt_em.o` against the other objects
  shows which of the weak `WSTUB`/`WEAK` definitions in `src/pc/rt/rt_em.c` are actually used. Only
  `src/pc/rt/rt_em.c` (`em_sleep_eff_set` ABI shim) and `rt_snd.c` (`ashi_sd_req`) define a
  game/lobby symbol strongly; everything else the host provides for these two overlays is a weak
  fall-back that a real definition overrides.

## What the numbers say

* game.bin: 2640 functions, 95.48 % of the bytes matched. The 40 unmatched functions are all
  near-match C (`*_nm.c`) the PC links; there is no game.bin function without C. 31 of them ran in the
  headless tests (quest loop, progression, urgent, all 38 offline quests, 34 village activities, frog).
* lobby.bin: 3272 functions, 38.1 % matched, but the unmatched rest is online code (cnLBS, browser,
  plaza, zlib, OpenSSL, 231 stand-ins in `build/pc/rt_gen.c`) and 15 near-match functions; only four of
  those run offline (`lb_disp_name`, `lb_pl_turn_sub`, `lb_insert_target_list`, `Lb_set_mini_data`).
  Most of what `src/lobby/**/*_nm.c` shows as "differing" are stale copies of functions that are
  matched in other files (the PC links the matched ones, `tools/pc_lobby_matched.txt`).
* Host (`src/pc`) definitions of game/lobby symbols: `em_sleep_eff_set` (ABI shim, fine),
  `ashi_sd_req`; weak fall-backs in `rt_em.c` that were still in use for game functions that have C:
  `em01_local_area_move_init` and `Em_Mode_Chg` (now linked, see below).

## Fixed: the PC ran something different from the original

Found by reading the asm against the near-match C after `semdiff.py` flagged it, or from the coverage run.

1. `em_cmd_ground_area_move` (monster command: walk to the next stage along an area route). The copy
   cast the wall test ray flat on the xz plane (`SetVector(v, x, z, 0)`) instead of from
   `(x, y + 10 * byte, z)` to the route point at the same height, and tested the wrong halfword of the
   route point (+0x12, the move index, instead of +0x10, "needs a wall test"). So a monster that had to
   cross a wall-blocked route tested a ray in the wrong plane and used the wrong flag. Now follows the asm
   (field `hit_ck` named in `include/em_cmd.h`); 152 of 253 instructions differ from the original only by
   saved-register allocation.
2. `em12_main` (the Em_Dmg_Sys result switch). The original's jump table has case 14 (the 12 reaction
   when the monster is not kind 0x19) and case 12 *without* the kind test; the copy applied the kind test
   to 12 and ignored 14. A 0x19 monster now reacts to result 12 and a result 14 is no longer dropped.
3. `em01_local_area_move_init` (kinds 1 and 11, 0x5665F0) was not linked: the PC ran the weak no-op of
   `rt_em.c`, so em01/em11 kept the stay and run-away timers of the previous area when moving to a new
   one. `src/game/em/em_modechg.c` (matched) is now picked in (`PICK_X` in `tools/build_pc.sh`), with
   `Em_Mode_Chg` (the host copy in `rt_em.c` was equivalent; the matched one wins now).
4. Lobby effect 25 (`eft25_i`, `_m`, `_d`, `_e`, `_t`: spark/aura particles on a joint; the Felyne
   canteen / NPC emotes call `Eft25_set`) was missing: only `Eft25_set` and `eft25_move` were linked, so
   the effect started and did nothing. The matched `eft25_i/_d/_e/_t` files (`lb_ge2502-05.c`) are
   linked now and `eft25_m` (near-match in `lb_e25.c`, written from the asm) is picked in.
5. `lb_disp_name` (name above each avatar): the job icon x was computed from `(s32)` truncated
   `spF0[0]` before the 1.25 scale; the original scales the float first (one of the three uses
   truncates, two do not). Up to 1.25 pixels.

Checked against the asm and found equivalent (so the "differing" figure is only allocation/scheduling):
`em_char_set` (monster motion request), `em_cmd_escape_area_set`, `em_cmd_dansa_sel`,
`em_cmd_angle_ck` (its `!= -1U` test on a byte is dead in the original too), `lb_target_angle`,
`Em_Mode_Chg`; the neck code (em_neck_move_sub, neck_ang_set) was read for logic only, the original uses unsigned shifts and compares where the copy has u16 promoted to int (same values).

## Ranked list: unmatched game functions (what the PC runs from `*_nm.c`)

RAN = entered in the headless tests. EQ = `semdiff.py` says only register allocation / scheduling /
block order differ. Diff counts are `check.py` (inflated by shifts).

| # | function | bytes | ran | state |
|---|----------|-------|-----|-------|
| 1 | `em_neck_move_sub`, `neck_ang_set` (em_core_nm.c) | 1956, 1392 | yes | head turning every tick; unsigned/signed shift noise only, logic read OK; 35 / 73 real hunks |
| 2 | `em_char_set` | 936 | yes | motion request; read against asm, equivalent (stack spill vs fp) |
| 3 | `em_cmd_end_command`, `em_cmd_samestage_pl_target_sel`, `em_cmd_all_pl_target_sel` | 2276, 1052, 900 | yes | AI program interpreter / target pick; semdiff differences are frame size, u8 loop temps and a duplicated constant load; only the area-route case of end_command and the first half of samestage_pl_target_sel were read against the asm |
| 4 | `em_cmd_ground_area_move` | 1012 | yes | FIXED (real bug), allocation left |
| 5 | `em12_main` | 1660 | yes | FIXED (switch), jump table vs ladder left |
| 6 | `em_cmd_escape_area_set`, `NextStage_No_Set`, `NextStage_Dir_Set` (EQ, 3 instr off) | 812, 856, 540 | yes | area change; read OK |
| 7 | `em_cmd_flag_set/_clear/_ck`, `em_cmd_range_ck` (EQ), `em_cmd_angle_ck`, `em_cmd_horm_pos_ang_ck` | | yes | 5-7 instr off each, scheduling |
| 8 | `Em_Master_Change`, `Em_Taisei_Set` (EQ), `em_range_set` (EQ) | 1224, 108, 180 | yes | allocation only |
| 9 | `em09_act_set`, `em20_act_set` (1 instr: `daddiu`), `em09_material_sub` (agent D) | | yes | |
| 10 | `eft04_t` (6 instr), `eft05_t` (spline scheduling, flag is s16 in the original), `eft11_i`, `eft18_set_com` (EQ, one delay slot), `eft22_end_init` (EQ, 4 instr) | | yes | |
| 11 | `set17_trans` (EQ) | 872 | yes | |
| 12 | `shell08_m`, `shell08_trans` (written from the asm, 1544 differ, stack layout) | 7584, 6320 | yes | no semantic difference found, not re-read in full |
| 13 | `set05_m` (2 instr), `Set20_set` (EQ, block layout), `shell22_i`, `shell08_rgba` (EQ), `em_cmd_st25_pl_target_sel`, `em_cmd_pl_ang_sel`, `em09_effect_move`, `print_tuto_message` (EQ, register swap) | | no | |

Byte matches reached in this session: none. Every function above is 1 to 8 instructions off or a pure
allocation difference. What was tried (beyond the previous agents' work): `declbf.py` (NextStage_Dir_Set:
8 -> 3 instructions), 20 minute permuter runs on NextStage_Dir_Set, em20_act_set, set05_m,
eft22_end_init, print_tuto_message (no improvement), expression-order and type variants listed in
`docs/agents/agent-E.md`.

## Lobby (single-player village)

* Unmatched with C: `lb_disp_name` (FIXED, float scale), `lb_pl_turn_sub` (EQ), `lb_insert_target_list`
  (EQ, 8 instr: `c` lands in t2 instead of a2), `Lb_set_mini_data` (builds the 24 byte member record for the
  network; ran once, no offline effect).
* Matched in C but not linked until now: `eft25_*` (see above). The remaining lobby stand-ins that ran are
  all online (cnLBS_*, Bs*, sceHTTP*, plaza, lm_*, `lb_member_*Check`).
* Stand-ins in `rt_gen.c` that fire in offline play and belong to main are agent B's list.

## Tools added

* `tools/semdiff.py FILE.c [FUNC..]`: the equivalence screen described above.
* `COV=1 tools/build_pc.sh` + `RT_COV=file` + `tools/cov_report.py`: function coverage.

