# Agent D notes (main-module effect and set files)

Every "match" below was checked with tools/check.py (all functions OK) and
`tools/rebuild.sh main` printing "main OK" (byte-identical).

## eft26 (0x27CA50-0x27CF10) - 7/7 match
Marker model spinning and bobbing above the player PLW+0x3B0 points at,
tinted from col_tbl by the target's +0x8ED. What +0x3B0 points to is a guess.
- `if (x == 0) n = 0; else n = y;` gave the original branch layout; the
  ternary and the `!= 0` if/else did not (eft26_m).
- Reads a lobby.bin byte by absolute address (0x6EAED6, no relocation in the
  original), written as `*(s8 *)0x6EAED6`.

## eft01 (0x101E40-0x102BC8) - 9/9 match
Shadows: skinned shadow model (kinds 0/3, 3 snaps every bone to the ground
through SetSkinTransKKK), two foot blobs (1), five joint blobs (2, one prim
each) and the monster blob (4, enemy_shadow_size; the lobby copy when
game_w+0x1DC is set).
- References to overlay tables from main (enemy_shadow_size in game.bin,
  enemy_shadow_size_lb in lobby.bin) are written as externs D_63BC40 /
  D_610300 (names from config/main_undefined_syms_auto.txt); they link.
- SetSkinTransKKK: `g = GetGroundHit(v); y = g + (5 + 0.3*h)` with a local
  g; writing the call inside the expression swapped the add.s operands.
- eft01_m: a table indexed `type02_tbl[i]` in a loop, not a walking pointer
  (the pointer version increments in the wrong order).
- eft01_t: declbf found the saved-register order (mw, mats, chr, cl, tbl, i);
  stack matrices declared in reverse of their stack order.

## set21 (0x225FC0-0x2267EC) - 12/12 match (first try)
A model held between a monster's joints 6 and 9; thrown when animation 0x432
hits frame 48, flies 10 frames to a per-stage spot (stages 0x51-0x55), Eft13
puff, stays 300 frames. Small per-stage tables declared with their real
sizes so the s16 angle tables are gp-relative.

## eft02 (0x27D6E0-0x27EF58) - 13/14 match
Hit sparks and blood. eft02.c (move/i/m/d/e, 0x27D6E0-0x27DDB8, jump tables
0x384170-0x3841F0 incl. alignment pad) and eft02b.c (8 spawners,
0x27E940-0x27EF58, table 0x384220-0x384240) are built. eft02_t stays asm:
src/main/eft/eft02_nm.c (whole file) is 12 instructions off, all in case
9-11 (a1/a2 swap for the clay index temp and where `col = -1` is
scheduled); 20 minutes of permuter found nothing better.
- Float constants that are one ulp above the obvious literal come from
  folded expressions: 0x39D1B718 = `0.4f / 1000.0f` (0.0004f gives ...717),
  0x3C23D70B = `0.1f * 0.1f` (0.01f gives ...70A). Found by compiling the
  candidates with MWCC.
- `mw->clay + ew->timer / 2 + 97` (pointer + index, then constant), not
  `&mw->clay[97 + t/2]`, matches the add order.
- A u8 field read into a u32 local (`k = ew->arg` before a call) explains a
  value kept in a saved register and converted with the unsigned sequence.
- Eft_rendope_set takes a u16 (callers pass the u16 flags without andi).
Shared header: pl.h carves PLW+0x3EC (u16 x3EC) from _pad3D2.

## set13 (0x1569E0-0x158F18) - 6/10 match so far
Sun glare / lens flare (guess from sun_pos_tbl and camera maths).
set13.c (Set13_set/set2/move/i, 0x1569E0-0x157080) and set13b.c (d/e,
0x158130-0x158188) are built. set13_m (4.3 KB) and set13_trans (3.1 KB) not
attempted yet (fused mula.s/madd.s maths, an inlined angle helper).
src/main/set/set13c.c (not registered): set13_hit_calc matches,
set13_disp_pos_calc is 13 off (the original loads dir[1], dir[2] before the
first store; no natural source found yet).
- `PLW *pl = &player_work[game_w.master];` as an initialiser (not a later
  statement) gives the original's early address computation (set13_i).
- `if ((sw = pull_set_work(0)) != 0)` tests v0 before the copy to s0
  (Set13_set2, Eft02_set2), where `sw = ...; if (sw != 0)` tests s0.
- set13_hit_calc: `p = (f32 *)((u8 *)p + 8)` keeps two separate +8 steps
  that `p += 2` lets the compiler merge (found from a permuter hint).

## eft06 (0x102BD0-0x105B10) - 17/19 match
Hit sparks with ten types; pieces are 0x38 bytes (EFT06_PIECE).
eft06.c (move, i, init_subs, pw_die_ck, continue, type_ck; 0x102BD0-
0x103A38, tables 0x3579C0-0x357A08), eft06b.c (d/e) and eft06c.c (se_req,
set_com, Eft06_set/set2/set_hit) built. eft06_m (4.8 KB) and eft06_t
(2.2 KB) still asm, not attempted.
- eft06_i: the main loop must index `w[i]` (the compiler's own pointer
  induction then increments first); `p++` in the for header put the
  pointer increment last.
- Eft06_set takes the float scale as its LAST parameter: argument set-up
  order of the recursive call follows parameter order.
- eft06_continue's count parameter is s16 (int gave an extra sign-extend).

set13c.c (set13_hit_calc, 0x158E00-0x158F18) is now built too; the
near-match set13_disp_pos_calc moved to src/main/set/set13_nm.c.

## eft13 (0x105B10-0x109E28) - 13/19 match
Dust, splashes and debris (35 types). Built: eft13.c (move), eft13b.c
(d/e), eft13c.c (se_req, water_ck, set_sub), eft13d.c (eft13_set,
Eft13_set_scl), eft13e.c (Eft13_set_em/_em_scl/_pos/_pos2, water_set; jump
table 0x357D70-0x357D98). Not attempted (big): eft13_i, _m, _t, _set_pos,
_set_sub_em, _set_pos_em.
- Main calls game.bin's Eft08_set/Eft08_set2 by address with no symbol:
  call them as func_544C90 / func_544D20 (config/main_undefined_funcs_auto.txt).
  check.py shows these calls as differing ("original calls ?"); the build
  links them correctly.
- eft13_water_set: `sc = 2.7f * scale` into a new local gives the
  original's const*reg multiply order; reusing the parameter swaps it.
  Statement order inside cases matters (case 2 has `pos[1] += 5` before
  the scale, case 8 after).
- A parameter only passed on to other functions: declaring it s16 makes
  MWCC re-extend it at each call to a function defined in ANOTHER file,
  while a callee defined earlier in the same file is trusted. After the
  split, eft13_set/Eft13_set_scl need `int j` to keep the raw pass-through.
- game_w+0x1E is read as a u16 here (`*(u16 *)&game_w.x1E`); game.h names
  it as a u8 (eft12), left unchanged.

## eft20 (0x218590-0x21D464) - 9/13 match
Monster dust/debris, sibling of eft13. Built: eft20.c (move, se_req),
eft20b.c (d/e), eft20c.c (water_ck), eft20d.c (Eft20_set/_set2/_set_pl,
water_set). Not attempted (big): eft20_i (4.3 KB), _m (4.6 KB), _t (5.1 KB),
_pos_set (3.5 KB). Same lessons as eft13 (func_544C90 calls, `sc` local).

eft06 update: eft06_t matches too (18/19); it is merged with d/e and the
spawners into eft06b.c (0x104D30-0x105B10, table 0x357A70-0x357A98),
eft06c.c is gone. Lessons from eft06_t:
- A switch whose default only returns, with the original branching to the
  epilogue right after the compares: write `default: return;` FIRST.
- An address the original loads into a saved register before an unrelated
  call is a local pointer assigned there (`fa = fade_type7_61_4;`).
- `col &= 0xFFFFFF` compiles to dsll32/dsrl32 by 8 (u32 local in memory).
- eft_rgba_linear's time argument is an int here (callers pass lhu/lh as
  loaded); an s16 prototype turned the u16 load into lh.

eft13 update: eft13_t matches (14/19); merged with d/e/se_req/water_ck/
set_sub into eft13b.c (0x106DB0-0x107C50, table 0x357C50-0x357CDC);
eft13c.c is gone. eft13_t repeats one identical case body for several case
groups (0, the big group, 5/10): MWCC does not merge identical blocks, so
they are written out separately. Declaration order found with a greedy
move search (/tmp/claude-1000/agentD/permsub.py).

## Policy change (coordinator): cover whole files, park near-matches
eft20_nm.c (not built) now holds C for every eft20 function still in asm:
eft20_i (1032/1080 instructions differ), eft20_m (885/1150), eft20_t
(1209/1287), eft20_pos_set (842/891). These counts are mostly register
allocation and block order; the logic was written from m2c drafts (with
jump tables, /tmp/claude-1000/agentD/jdraft_main.py) checked against the
asm by hand, and compiles. Not verified at runtime. Calls into game.bin
by address: func_628690 (shell01_set2), func_628750 (shell01_set3),
func_629C20 (shell04_set2).
eft13_nm.c (not built) holds C for the five eft13 functions still in asm:
eft13_i (468/483 differ), eft13_m (608/672), eft13_set_pos (598/648),
eft13_set_sub_em (269/349), eft13_set_pos_em (628/695). Notable: in
eft13_m a piece marked 0xFF ends the whole update (`return`, not
`continue`), and in set_sub_em / set_pos_em case 19 falls through into
case 20 (both checked in the asm). game.bin calls by address:
func_53FDF0 (Eft17_set_ex), func_628690, func_62A2C0 (shell05_set3).
eft06_nm.c (not built) holds C for eft06_m (1097/1212 differ); with it
every eft06 function now has C.
set13_nm.c (not built) now also holds set13_m (853/1074 differ) and
set13_trans (761/775). set13_trans uses a helper set13_roll() that the
original inlines twice (flare roll from sun/camera on XZ, fused
mula/madd maths). Several values are left uninitialised exactly as in
the original (set13_m case 1 on stages other than 0x18/0x22, case 7 box
limits on stages other than 0x2D/0x38). The uv offsets in set13_trans
cases 3 and 7 go to x (checked: f13 = 0).

## Coverage summary (agent D)
Every function of the eight assigned files now has C. Built and
byte-matching: eft26 7/7, eft01 9/9, set21 12/12, eft02 13/14, eft06 18/19,
eft13 14/19, eft20 9/13, set13 7/10 (89 of 103). The other 14 are in
*_nm.c files (not built): eft02_t (12 off), eft06_m, eft13_i/_m/_set_pos/
_set_sub_em/_set_pos_em, eft20_i/_m/_t/_pos_set, set13_m/_trans/
_disp_pos_calc (13 off).

# Second assignment: hit, cam, weapon (main)

## hit (0x111B20-0x114A88) - 17/20 match
Shell hit detection: every live shell against monsters, players and other
shells; damage per body part, sharpness (s_gauge_tbl) and meat values
(em_meat_tbl), ailment build-up, hit sounds and hit-mark effects. New
header include/hit.h (HCHR = header shared by PLW/EMW, HSHL = hit side of
SHLW, HBODY = 0x28-byte hit volume), fields mostly named by offset.
Built: hit.c (0x111B20-0x1131C0, jump table 0x358230-0x358250), hitb.c
(dm_vec_calc), hitc.c (0x114880-0x114A88). hit_nm.c holds the whole file;
not matching: hit_hit_sub_em (112 off), hit_hit_sub_pl (2: add.s operand
order of `def + 80.0f`), hit_calc_shl (2: one nop placed differently).
Shared header: game.h carves game_w+0xD3 (pl_num) from _pad0D2.
- `if (x == 0xFF) own = 0; else if (...) own = A; else own = B;` gives the
  original's "bne to compute; delay own=0; b end" (shell_hit_ck); the
  pre-initialised `own = 0; if (x != 0xFF) ...` does not.
- An early "return if any of these values" test was a switch with the
  cases then `return;` (hit_shl_shl_ck); an if-chain gets range-merged.
- Stack aggregates: declared first = highest address; matching the
  original's slots fixed whole functions (hit_shl_shl_ck, hit_calc_shl).
- `if (a && (b = p->x) != 0)` puts the store in the delay slot; the
  original tested `p->x != 0` and assigned in the body.
- Float registers: declaration order again (hit_hit_sub_em: sharp, rate,
  then the four ailment values).
- 2-D tables indexed `tbl[k * 4 + v]` as flat arrays match where `[k][v]`
  computes the address differently (s_gauge_tbl, hit_se_tbl).

## f_hit_28CE00 (0x28CE00-0x290560) - 11/14 match, 9 built
Geometry tests: point/sphere, sphere/sphere (bool, contact point, push
vector), capsule/capsule and capsule/sphere (contact point or push-out),
sphere/plane, line/sphere. HPK (include/hit.h) is the capsule packed by
hit_cap_pk: p0, p1, r, dir = p1 - p0, centre, bounding radius. HLINE is
the line form used by hit_line_sphr2. Shared prototypes: include/hit2.h.
Built: hit2.c, hit2b.c, hit2c.c, hit2d.c, hit2e.c. hit2_nm.c holds the
whole file with the four helpers static as in the original.
Not matching: hit_sphr_sphr2 (18/64, scheduling), hit_cap_cap2_m (41/1253)
and hit_cap_cap3_m (90/945): only t2/h float registers and the i/j int
registers swap; the logic is complete. hit_cap_sphr2_m and hit_line_sphr2
match in hit2_nm.c but not when split off, because the call to the static
hit_point_sphr then costs the full clobber set.
- A C range that ends at a function followed by alignment zeros must end
  at the function's last byte, not at the next function: the object has
  no trailing padding and SUBALIGN is off, so everything after shifts.
- hit_sphr_cap_m keeps an original bug: on the second try for a
  perpendicular it bumps p[1], not the copy it then uses.
- `rr = r + k->r` into a new local (not reusing the parameter) and
  computing all of px/py/pz before the v[] subtraction fixed the
  cap/sphere functions; the result pass `len = rr - d; out = len * m`.
- permsub (greedy declaration moves) halved hit_cap_cap2_m: t first.

## f_cam (0x21F3D0-0x222E20) - IN PROGRESS, paused by the owner
Where I stopped: src/main/cam/cam_t.c (not registered, not built) holds 42
of the 51 functions; 38 of them match per check.py, near: SetCameraData
(66/72, register numbering in the block loop), cam_init_sub_pchngr (3,
range reloads max), pch_lock_chk (6, tail branch layout), fish_cam_sub (2,
mov.s in delay slot). New header include/cam.h: CAMW (CameraWork, 0x5F8),
CAMS (5 slots of 0x100 at 0x80: std, stage, pachinger, player EX, demo),
per-mode work unions at slot+0x90, CAMQUAKE, CAMAREA, CAMCNF.
Shared header: include/pl.h PLW carved part[2] 0x110, mdl148, x3A8, x56E,
x714, x763, pch_on 0x764, fish878, x8C6, x8C8, x8EE (camera users).
Next: write CameraMove, cam_init_sub_std, cam_sub_std, cam_sub_stg,
cam_sub_pchngr, cmd_set_pos, cmd_set_tar, cmd_cam_move, point_cam_sub (the
last three use jump tables lit_1012/1110/1179 at 0x36B0D0-0x36B178, need
main:rodata lines), then split cam_t.c into matching runs, register, park
the rest in cam_nm.c. Then f_cam_223B50 (22) and f_weapon (29).
Lessons so far: the original takes `CAMW *cw = &CameraWork` into a local
(WyvernFindPlayer, BBQcamera_set, PachingerCamChk); manual_cam_chk and
GetPachingerInfo take an unused first argument; 0x3F75BE0B is 55 degrees
in radians (0.9599311f); `if (a == 1 || b) {zero} else {copy}` order
(cam_sw_set_sub); statement order pl/npc/src in cam_plEX_zoom was found by
permuting (scratch tools: /tmp/claude-1000/agentD/tryv.py, rep.py, carve.py).

### f_cam update
cam_t.c is split and linked (main OK): cam.c (0x21F3D0-0x21F464), camb.c
(0x220420-0x2206A4), camc.c (0x221460-0x221700), camd.c (0x221820-
0x221D28), came.c (0x221F90-0x2220C0), camf.c (point_cam_hit), camg.c
(0x2227A0-0x222E20): 38 functions. cam_nm.c (not built) holds the whole
file incl. near-matches SetCameraData, cam_init_sub_pchngr, pch_lock_chk,
fish_cam_sub. Still no C for: CameraMove, cam_init_sub_std, cam_sub_std,
cam_sub_stg, cam_sub_pchngr, cmd_set_pos, cmd_set_tar, cmd_cam_move,
point_cam_sub (jump tables 0x36B0D0-0x36B178 still need main:rodata lines).

f_cam update 2 (final state of this pass): built and byte-matching (main OK):
cam.c, camm.c (CameraMove, cam_init_sub_std 0x21F590-0x21F9A8), camb.c,
camp.c (cam_init_sub_pchngr, cam_sub_pchngr, pch_lock_chk 0x220EE0-0x221460),
camc.c (PachiTypeCheck .. fish_cam_sub 0x221460-0x221814), camd.c
(0x221820-0x222408: NPC zoom, demo camera, static get_em_local, cmd_set_pos,
cmd_set_tar, cmd_copy, get_angle, cmd_cam_move, point_cam_hit; jump tables
0x36B0D0-0x36B108), camg.c (cam2view .. cam_sw_set_sub). 46 of 51 functions.
Still asm: SetCameraData (66/72; C in cam_nm.c, 65 diffs whatever the
declaration order: the original loop shape differs), cam_sub_std (65/668:
angle smoothing registers ca/da, two stray nops after the k switch and the
blend-rate if), cam_sub_stg (written, 409/524: register assignment of
cw/cs/area/d/spl and the smoothing blocks, not worked through), point_cam_sub
(28/225: command pointer a2 vs a3). All in src/main/cam/cam_nm.c.
Lessons:
- get_em_local must be `static` and defined BEFORE its callers in the same
  file: MWCC then knows its clobber set and keeps `out` in a temp register
  across the call (cmd_set_pos/tar). Otherwise a saved register is used.
- Float-last prototypes: cpInterVector(f32 *out, f32 *a, f32 *b, f32 t) and
  flvecRotY(f32 *v, f32 a) (flvecRotX likewise); with the float first the
  `mov.s $f12` is scheduled too early. act_ck returns int here (an s16
  prototype adds a sign-extend; PachiTypeCheck casts, cam_sub_std does not).
- A 6-entry switch with an empty `case 5:` gets a jump table (sltiu 6);
  without it, an if-chain. Source case order = body order; the compare chain
  of a small switch comes out reversed from the source order (pch_lock_chk:
  write the cases in reverse of the original's compare order).
- `if (a <= 0 || b >= 0) {loop} else {finish}` gave the original layout where
  `if (a > 0 && b < 0) {finish} else {loop}` did not (point_cam_sub case 21).
- `if (f != 1) { if (f != 0) {A} else {B} } else {B}` (B duplicated) gives
  the original's code for `f != 1 && f != 0`; `&&` gave a different layout.
- `switch (x) { default: k = 950; break; case 2: k = -950; break; }` gave the
  original's unfilled-delay-slot layout for a two-way constant choice.
- `a > 0x60` (u16 field) compiled with `slti at`; `a >= 0x61` did not.
- A global pointer hoisted into a local (`spl = SplineRvalue`) is how the
  original gets a saved register for it (cam_sub_stg).
- cmd_cam_move constant 0x38C90FDB = 0.000095873799f (2*pi/65536).
- Frame/ordering tool: /tmp/claude-1000/.../scratchpad/dperm2.py permutes the
  first N declaration lines and keeps the best (same idea as tools/declbf.py
  but with a count limit; declbf over 7 lines is too slow).
Shared header: include/cam.h area_chg is u8 (lbu in cam_sub_std).
