# Agent A notes (game overlay: shell08, set09)

## set09 (0x618CA0-0x61ED08) - whole file matches
src/game/set/set09.c, 52 functions, jump tables 0x689E00-0x689EC8.
Verified: tools/check.py all OK, `tools/rebuild.sh game` = game OK.
A stage effect manager: kind 0 is a controller whose work holds 32 child
pointers and per-kind counts; kinds 1-11 are small ambient objects (see the
file header; what each kind looks like in game is a guess). Types are
file-local (SET09W, SET09W1, SET09W5). Functions that are LOCAL in the
original are `static` here (checked: still matches).

## shell08 (0x62D4D0-0x6331C0) - split
Matching and built:
- shell08.c   0x62D4D0-0x62EA1C Shell08_set_ang .. shell08_i (+ rodata 0x68A0D0-0x68A120)
- shell08b.c  0x6307C0-0x63099C shell08_h .. shell08_trans_sub
- shell08c.c  0x632250-0x632AB8 shell08_impact_set .. shell08_z_adj (+ rodata 0x68A210-0x68A240)
- shell08d.c  0x632E70-0x6331C0 shell08_type6_init .. shell08_type8_pos_set
Types in include/shell08.h (new header, SH08W at sh->senko, SH08P particles).
Verified: check.py OK for all, rebuild game OK.
Still assembly:
- shell08_m (7584 bytes): near-match in shell08_nm.c, ~1065/1898 differ.
  Control flow / structure matches (sdiff shows only prologue and a few
  spots); the rest is register allocation: original keeps `flag` in fp,
  em spilled at sp+0xB0, w->p spilled at sp+0xC0, num s6, all s7, idx s0,
  step s5, t s1, y f20, d f21. Declaration-order searches did not find it.
- shell08_rgba (944 bytes): near-match in shell08_nm.c, 48/236 differ
  (order of the channel extraction; permuter found nothing).
- shell08_trans (6320 bytes): not written. It spills many byte locals to
  16-byte stack slots; expect a long job.

## Matching lessons (each confirmed by a match)
- `if (a == 0 || a == 5) return;` gives the "beqz -> stub `b end`;
  bne -> body" shape (set09_i); a switch or `&&` form does not.
- `case 8: return;` keeps its own `b end` stub; `case 8: break;` gets
  merged away (set09_i00).
- MWCC b52 folds `0.0f * x`; the original kept a multiply by a zero
  register. A local `f32 spread = 0.0f;` reproduces it (set09_09_pos_reset).
- u16 fields compared with negative constants (`rot > -0x1000`, `rot < 0`)
  explain odd `slti at, x, -4095` / `bgez` on an lhu (set09_m02); the
  emitted slti immediate is the sign-extended constant.
- A float `a > b` gives `c.le a,b; bc1t skip`, `a < b` gives `c.lt; bc1f`;
  `if (10000.0f > d)` was needed for `c.le 10000,d` (shell08_m).
- `x = (c) ? a : b` vs if/else: if/else with an assignment in each branch
  stops the next statement's load being hoisted into the delay slots
  (set09_i07).
- A field accessed both signed and unsigned: declare it u16 and cast
  `(s16)` at the signed uses (set09 kind 5 `bank`).
- `for` with explicit `p++; continue;` at every continue (not `i++, p++`
  in the header) matches shell08_m's duplicated pointer increments.
- Set09_set_ex's arg is s16 (no andi), passed straight on.
- set09_i00 clears cnt[0] then loops `cnt[i + 1] = 0` for 11 entries (the
  compiler unrolls it by 5 and peels the rest).

## tutorial / tuto (0x63ACA0-0x63BB50)
- src/game/tuto/tutorial.c 0x63ACA0-0x63B020: all 7 functions match
  (Tutorial_prog .. soncho_no_oshie).
- src/game/tuto/tuto.c 0x63B020-0x63BA78 (SonchoInit .. tuto_msg_flag_set)
  and tutob.c 0x63BB20-0x63BB50 (tuto_read_flag_set) match.
- print_tuto_message (0x63BA80) is assembly; near-match in tuto_nm.c,
  8/40 off (the line counter and y position swap s1/s2).
- Shared header edit: include/game.h gets `u16 quest` at 0x2C (carved
  from padding). quest_w (0x3C7440) is a file-local type in tutorial.c.
- The main data object `soncho_no_oshie` (0x389C54) has the same name as the
  tutorial function, so the split calls it soncho_no_oshie_00389C54;
  tuto.c references it by that name.

## pl damage (0x639EB0-0x63AC9C)
- src/game/pl/pl_damage.c 0x639EB0-0x63A008 (mahi_dm_ck .. Guard_dir_ck)
  and pl_damageb.c 0x63A0F0-0x63AC9C (pl_guard_set, Pl_damage_sub, jump
  table 0x68A4F0-0x68A52C) match.
- pl_guard_ck (0x63A010) is assembly; near-match in pl_damage_nm.c, 1 off.
  It returns a u8 and Pl_damage_sub masks the result, which our compiler
  only does when the caller sees an int return. In C the
  function would have to be u8 and int at once, so it stays split out.
- Shared header edit: include/pl.h, fields carved from padding: kind 0x002,
  ang_y 0x00E, vital 0x302, dm_flag 0x38D, dm_ang/dm_pow/dm_type 0x3EC-0x3F0,
  x43E, x738, stamina 0x748 (guess), dm_vital 0x766, vital_red 0x790,
  x7AA..x7C6 ailment gauges (names are guesses). sizeof(PLW) still 0xA00.

## More matching lessons
- `x <= 0xE39` gives `slti at, x, 0xE3A`; `x < 0xE3A` puts the result in
  v0/v1 (Guard_dir_ck). `(u32)(f - 4) > 1` gives the `sltiu at` range test.
- A `return` in the middle of a switch or a nested block makes the compiler
  emit a `b epilogue` stub. The original jumps straight to the epilogue.
  To get that, Pl_damage_sub needed `break`/`goto vib`/`goto end`
  (shared tail labels) instead of returns.
- `a != X && a != Y ...` chains stay separate beq tests; `a == X || a == Y`
  chains with consecutive constants get folded into range checks.
- Fixed-size sprite structs: one field written with `sw` in one place and
  as halves with `sh` in another needs `*(s16 *)&spr.uv0` (DispTutorial).
- Statement order of plain stores matters even with no dependencies
  (DispTutorial's second sprite: w, y, h, col, x).

## Assignment 3: fl graphics library (5 Oct 2026)
- docs/formats/graphics.md: Meltw, link files, AMO chunk tree, MLCLAY,
  clay handles, shader params, VU1 programs, flSetRenderState map.
- tools/clay_dump.py (AFS -> .obj, em01 renders as Rathalos) and
  tools/vu_dis.py (VU1 microcode disassembler, output only to build/vu1).

## Assignment 4: textures, skeletons, motions, Wii MHG (5 Oct 2026)
- APX texture format + material->texture chain in docs/formats/graphics.md
  section 7; Wii MHG fpk notes in 7b.
- docs/formats/motion.md: AHI bones, skinning, *_tbl.bin banks, AAN curves.
- tools/clay_dump.py: PNG/.mtl/UV export, --motion/--frame posing.

## Assignment 5: hunters, stages, Xbox HUD art (5 Oct 2026)
- docs/formats/player.md + tools/pl_dump.py: six armour parts bound to the
  legs skeleton through ptmat_tbl (read from main.bin at run time); motion
  ids (bank = id%1000/100, slot = id%100; >=1000 own table).
- docs/formats/stage.md + tools/stage_dump.py: stage tables, attribute
  chunk -> render states, HITS collision. st04 = base camp (render).
- graphics.md 7a: cpit1xb/cpit2xb decoded (cpit2xb = real Xbox buttons).
- Wii FPK LZ: still not cracked (two short attempts).
