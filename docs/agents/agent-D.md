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

## f_weapon (0x163AB0-0x1678xx, display "trans" code) - started
Built and byte-matching (main OK): src/main/weapon/trans.c (TransReset,
TransSet, GameTrans, trans; 0x163AB0-0x163D20), weapon.c (SetPartsTrans,
SetPartsTrans2, weapon_dat_make/2/3; 0x163E40-0x16440C), weapon2.c
(sight_disp2, sight_disp_ballista; 0x164D60-0x164F68). Parked as near-match
in src/main/weapon/weapon_nm.c: trans_pl_sub/Lb_trans_pl/Ed_trans_pl (10/23:
the original keeps an empty then-block, call placed after `b end`),
weapon_joint_calc (jump table ok, ~440/600 differ in layout; written as C).
Not started: pl_item_trans_sub, pl_item_trans (3.8 KB), weapon_trans (5.4 KB),
player_trans, lb_pl_item_trans, Lb_player_trans, Ed_player_trans,
enemy_trans, player_mat_calc, player_modify, player_mk, get_tex_num,
Material_set_sub, plplAdd2. These are display transforms (skeleton/weapon
model draw for the PS2 renderer): low value for the Xbox port, which will
redraw them on its own renderer.
- `for (i = 0; i < 0x40; i++) trans_func[i] = 0;` compiles to the original's
  8x unrolled loop (TransReset); do not hand-unroll.
- ot4..ot8 are 4-byte objects in .sdata (declare `extern u8 ot4[4]`).
- `if (a == 0) {} else {r = v}` kept as an empty then-block by the original
  is reproduced by `switch (a) { case 0: r = v; break; default: break; }`
  (weapon_joint_calc) but not for trans_pl_sub.
- The jump table for a switch on a 0..5 value needs an explicit `case 0:`
  before `default:` when the original table sends 0 to default.
- Locals `f32 *vy = &v[1], *vz = &v[2];` reproduce the original's hoisted
  element pointers (sight_disp2/ballista).

## f_cam_223B50 (rail camera, spline, wall hit camera) - started
Linked: camr1.c (vInnerProductXZ, vInnerProduct), camr3.c (dCnvComplex,
dSubComplex, dMulComplex). Near-match in camr_nm.c: ZoomRateCalc (8/34),
ZoomBaseAngleRail (1/10), RollAngleRail (11/28), dDivComplex (13/34),
QuestClearCameraRequest (33/65; C complete). Not started: cam_rail_move_sub,
cam_rail_move, cam_rail_move_0, CamRailMove, CamRailPoint, GetOrthogonalPoint
(finds the t where the camera rail cubic is nearest to a point: builds the
degree-5 polynomial of (P(t)-Q).P'(t), solves it with DKA5 (Durand-Kerner,
complex roots, uses the d*Complex helpers) or Cardano/linear when the
leading terms are below 1e-10; keeps roots with |im| < 0.001), tri_diag and
Spline (natural cubic spline via tridiagonal solves, 0x30 bytes of
coefficients per segment), DKA5, Cardano, k_HitWallCamera, k_HitEmCamera.
m2c cannot read mula.s/madd.s: read the asm.
- Float-last prototypes again: ScaleVector(f32 *out, f32 *in, f32 t).

# Third assignment: rest of f_weapon, f_cam_223B50, stage hit (f_sphr)

Policy: breadth first; every function has C now except pl_item_trans (below).
"Not built" = lives in an `X_nm.c` file (compiles, logic believed equivalent,
byte-different). Counts are "instructions differing / total" from check.py.

## f_weapon part 4 (0x164F70-0x1692C0), include/trans_pl.h
New header trans_pl.h: PLX is an overlay of PLW with the display fields
(armor model pointers at +0x534, model at +0x50C, weapon model +0x514, ...),
PLMDL (clay list +0x30, skin +0x24, materials +0x10), WNODE (skeleton node,
0x190 bytes, matrix at +0x40). It does not touch pl.h.
- Built (main OK): weapon3.c: player_mat_calc, player_modify, player_mk,
  get_tex_num, Material_set_sub (0x168EA0-0x1691B4).
- weapon3_nm.c (not built, all compile at the original size): plplAdd2 (22/26:
  the original does not hoist the load of the key's +4), player_trans (272/391),
  Lb_player_trans (275/387), lb_pl_item_trans (129/137), Ed_player_trans
  (257/260), enemy_trans (165/228), pl_item_trans_sub (300/322), weapon_trans
  (1287/1360; the 0x10-0x13 byte constants of the weapon placement tables
  are real, the decompiler dropped middle float arguments, read from the asm).
- pl_item_trans (0x165480, 3.8 KB, 887/960): written too, in weapon3_nm.c: a
  hand-placement table written out as code (clay offset, joint 0xE/0x12,
  offset vector, rotation, scale for ~20 action ids with frame_check2
  windows, plus the item-in-use cases of x56B). Float constants are the exact
  values from the asm; the meaning of each action id is a guess.
- Lesson: a small helper that returns a float and is defined earlier in the
  same file (vInnerProduct) keeps float temporaries in caller-saved registers
  only if it is `static` (GetOrthogonalPoint, camr6_nm.c).

## f_cam_223B50 (rail camera), 0x223B50-0x225200
Built: camr1.c, camr2.c (CamRailMove, CamRailPoint 0x223E90-0x223F8C), camr3.c,
camr4.c (tri_diag). camr2_nm.c: cam_rail_move_sub (2/80), cam_rail_move (83/84:
the original keeps the section byte in a stack slot), cam_rail_move_0 (1/43).
camr4_nm.c: Spline (259/262). camr5_nm.c: DKA5 (153/158), Cardano (99/203),
k_HitWallCamera (46/104), k_HitEmCamera (446/538). camr6_nm.c:
GetOrthogonalPoint (234/413, saved register choice). camr_nm.c (older):
ZoomRateCalc, ZoomBaseAngleRail, RollAngleRail, dDivComplex,
QuestClearCameraRequest. Every function of the file now has C.
Notes: the double constant 1e-10 / 1e-6 compares are `(double)x < 1.0e-6`
(soft-float _dpflt/_dpfgt); tri_diag(x, a, b, c, d, n) (Thomas algorithm, 64
unknowns, scratch g[] on the stack) matched first try; DKA5 relies on the
d*Complex helpers being defined before it in the same file (a2 survives).
A scratch tool that expands the PS2 FPU mula/madd/msub/adda ops for m2c:
/tmp/claude-1000/agentD/draft2.py (copy it into tools/ if wanted).

## Stage hit (f_sphr, 0x114AE0-0x11CA74, 33 functions; hit/shit*.c)
Previously no C. include/hit3.h: DIORAMA (diorama_w: wall grid at +8..+0x1C,
ground grid at +0x20..+0x34: cell size x/z, cell counts, cell table, polygon
area), HPOLY (56-byte polygon: kind, flags, 3 vertices, normal, plane d),
HKIND (per-kind flags: lava +0xB, water +0xC, +0xE special), HSWEEP (swept
sphere), and the result arrays hit_decision/hit_near_point/hit_hosei_base/
hit_kouten/hit_side/hit_area_out/hit_poly_num.
Built (main OK): shit1.c load_stage_hit, shit15.c GetWallTblAdrs, shit5.c
NormalClipFace, shit6.c add_vec_sub2, shit16.c GetGroundTblAdrs, shit7.c
check_angle; hit3.c hit_point_cyl (0x290560).
Also in hit/: hit3_nm.c hit_point_cbd (5/113), tri_nm.c tri_in_check (1/143),
VectorHitCheck (54/182), old_pos_save (41/52), hitw_nm.c HitWallPlayer
(104/217).
Not built (all compile at the original instruction counts):
- shit1_nm.c WallHitInit / GroundHitInit (6/70 each; a register swap of the
  -1 constant and the cell pointer, 400 s permuter found nothing).
- shit2.c (registered nowhere, near): BlockPlaceCgeck (74/80), Ground/Wall
  FieldInCheck (51/72), AreaFieldInCheck (13/40), GetWallTblAdrs (1/46, mult
  operand order).
- shit3_nm.c ground heights: GetGroundHit (186/204), GetGroundShellHit
  (200/218), GetWaterHit, GetTenjoHit, GetYouganHit (GetGroundTblAdrs matches, shit16.c).
- shit4_nm.c FaceLinePos (25/140), check_slide (6/52) + the three matching
  helpers; shit8_nm.c GetGroundHitArea/Upper/StatusAreaPl/Em; shit9_nm.c
  GetFloorSlide; shit10_nm.c sphr_face_o3/o4 + GetWallHitBit2; shit11_nm.c
  GetWallHitBitPl/Em; shit12_nm.c GetWallHitLine/GetEyeHitLine; shit13_nm.c
  hosei_sub; shit14_nm.c PushAdjust3.
Semantics worth knowing: ground queries collect up to 5 polygon heights under
the point and take the highest not above y+50 (the lowest if none); the wall
tests sweep a sphere (HSWEEP) over a 2x2 or n x n block of cells, test every
polygon with a face test, then edge/corner tests, and PushAdjust3 combines the
pushes. A few places where the decompiler lost data are commented in the C
(the "seen" loops of GetWallHitLine/GetEyeHitLine, the A[k]/cov[k] pairing in
PushAdjust3, the y override of face contacts in sphr_face_o4).
Lessons: struct/array offsets from the asm are exact but m2c drops the middle
float argument of calls (flmatMakeScale, SetVector, flmatSetTrans: read the
asm); `if (0 < n) for (i = 0; ...)` becomes `for (i = 0; i < n; i++)` with
the original's sltu form (WallHitInit).

## Stage collision API (for the PC runtime, agent A)
Status 5 Oct 2026 (second pass): everything below has C in src/main/hit/
(shit*.c built or *_nm.c near-match; none is exact except the helpers listed
at "Built" above). The logic of GetWallHitLine, GetEyeHitLine, PushAdjust3,
GetWallHitBit2 and sphr_face_o4's y override was re-read against the asm in
this pass. Fixes found: the "seen" list of GetWallHitLine / GetEyeHitLine only
compares the polygon with the FIRST remembered entry (a quirk of the original,
kept), and PushAdjust3's third pass re-reads its loop bound because nd grows
inside it. The remaining diffs are register allocation and frame size.

Data. load_stage_hit(stg) loads lwNNN.bin (wall) and lgNNN.bin (ground) and
WallHitInit / GroundHitInit turn file offsets into ABSOLUTE 32-bit POINTERS
inside the file image (cell lists are -1 terminated arrays of s32 pointers to
56-byte HPOLY). On a 64-bit PC build those casts (`(HPOLY *)*cell`) are wrong:
either load the HITS images into the low 4 GB (mmap MAP_32BIT) or change the
cell list type to a 32-bit offset from the image base. The grid lives in
diorama_w (include/hit3.h); ground_tbl_add[stage][kind] (0x10-byte HKIND)
gives per-kind flags (water at +0xC, lava +0xB, special +0xE); game_w.stage
selects the row. Format: docs/formats/stage.md.
NOTE: GetWallHitLine / GetEyeHitLine take their cell size from the GROUND
grid (gcsx/gcsz) even for the wall cells; the stages use equal sizes.

Ground questions (pos = f32[3] {x, y, z}; all return the height of the floor):
- GetGroundHit(pos) -> f32 y. Polygons whose triangle (xz) contains the
  point and whose normal.y > 0, up to 5; takes the highest not above
  pos.y + 50, else the lowest; pos.y if none. GetGroundShellHit: same for
  shells (+100, skips water/lava kinds).
- GetWaterHit(pos, &y) -> 1 and the water surface height when a polygon of a
  water kind is under pos (the last such polygon).
- GetTenjoHit(pos, &y, HPOLY *attr) -> 1 and the ceiling height (normal.y < 0)
  with its kind/b1/h2 copied to attr.
- GetYouganHit(pos) -> 1 when a lava polygon is under pos.
- GetGroundHitArea / GetGroundHitAreaUpper(ent, pos, out): same with the
  stage area's floor as fallback (ent+0x736 = stage); returns 1 on the
  ground file, 0 outside it (floor height used), -1 entity not on the stage.
  GetGroundHitStatusAreaPl / ...Em(ent, pos, GATTR *at, out, flag): as above
  plus polygon attribute word and water / special surface height (players /
  monsters, monsters use +100 on special kinds).
- GetFloorSlide(ent, out, flag): ent pos at +0xAC; out = slide push vector on
  slopes of at least 0x1500 (check_angle), flag != 0 also applies it. Returns
  0 sliding, 1 stands, -1 off the ground file.
Wall questions:
- HitWallPlayer(ent, keep) (0x11CA80): the per-frame entry for players AND
  monsters (ent+0x10 != 0 is a monster). Builds the segment old pos (ent+0x5A0)
  -> new pos (ent+0xAC) for every collision sphere of the entity and calls
  GetWallHitBitPl / GetWallHitBitEm, which push ent+0xAC out of the walls and
  record what was touched (players: pl_wall_mat[id][] with angle / kind /
  normal; monsters: bit mask ent+0x74C and special-wall flag ent+0x95D).
  keep != 0 keeps the previous mask.
- GetWallHitBit2(r, a, b, pos, mask) (0x115EB0): generic sphere sweep (camera
  and effects use it): sphere radius r from a to b; pushes `pos` out of the
  walls; mask = polygon h2 bits to ignore (0x8001 / 0xC001 for the camera).
  Returns hit_poly_num (number of contacts) or -1 when b is outside the wall
  grid. Contacts remain in hit_decision / hit_near_point / hit_side arrays.
- GetWallHitLine(a, b, out, mask) -> 1 and the crossing point (wall polygons
  only; a point outside the grid counts as the hit), 0 and b when free.
  GetEyeHitLine(ent, a, b, out, mask): same against ground polygons (not
  water / special) and walls: line of sight / camera collision.
Building blocks: sphr_face_o3 / o4 (sphere vs polygon for players / monsters:
face test, then edges / corners), hosei_sub (contact -> push vector),
PushAdjust3 (combine contacts, move pos, limit push to 1.8 * sweep length),
FaceLinePos (edge vs plane), NormalClipFace (point inside triangle),
tri_in_check / VectorHitCheck (angle-sum triangle test, segment vs triangle),
GroundFieldInCheck / WallFieldInCheck / AreaFieldInCheck (inside the loaded
grid, 8 unit margin), BlockPlaceCgeck (quadrant of a cell), GetGroundTblAdrs /
GetWallTblAdrs (cell list for a position).
Sphere/capsule tests between entities (hit2*.c, hit_*_m) are separate: they
take plain vectors and need no stage data.

# Fourth assignment (drawing code: player_trans, enemy_trans, shell08_trans)

Every function of 0x163D20-0x169228 already had C (weapon_nm.c, weapon3_nm.c,
see above). This round:
- plplAdd2 (0x1691C0) now MATCHES and is linked in weapon3.c (range extended to
  0x169228, main OK). Lesson: write the list node as a struct (`PLN {u32 next;
  f32 key;}`) and the two "insert" tails twice (`if (v == 0 || !(v & 1)) {ins;
  return;}` then `if (w->key <= n->key) {ins; return;}`); with plain u32/f32
  casts the compiler hoists n+4 out of the loop.
- enemy_trans (weapon3_nm.c): declaration-order move search (greedy, script
  in /tmp, not kept) took it from 165 to 129 of 228 differing; `int n` (not s16)
  removes the extra sign extension. Rest is register allocation; parked.
- trans_pl_sub/Lb_trans_pl/Ed_trans_pl: five more source shapes tried (empty
  then, else, &&, goto, trailing return): all still 10/23. Parked.
- shell08_trans (0x6309A0, game.bin, 6320 bytes): C written for the whole
  function at the end of src/game/shell/shell08_nm.c (not built). Same size as
  the original, 1544/1580 instructions differ (registers, block order). Read
  from the asm because m2c needs the jump table (switch on sh->arg, 12 cases)
  and drops float arguments. Meaning per sh->arg: 0/11 ring + glow (3 + 1
  particles), 1 flash cone, 2/3/9 puffs, 4 smoke (3 particles, mode sh->x05),
  5 billboard, 6 shards, 7 streaks (p is not advanced for p->no == 1), 8
  thunder (its "loop" runs once). Guesses: the names. Clay offsets in the
  original are byte offsets, here clay indices (offset / 0x8C).
  Uninitialised-register reads in the original (clay index of kinds 6/7/8 in
  rare modes) are given defaults (5, 1).

# Fifth assignment: non-monster game.bin overlay leftovers

Remaining unmatched (non-em) functions of game.bin, with sizes, at the start of
this pass: set17_trans 872 B, shell00_i 912, shell22_i 1188, shell22_h 256,
shell08_rgba 944, eft05_t 1160, eft11_i 672, eft22_end_init 564, fish_type_set
340, Set20_set 232, print_tuto_message 160, pl_guard_ck 212, Fish_set 132,
set05_m 1800, shell06_move_sub 3132, set14_trans 3148, eft04_t 5064, eft16_m
7032, shell08_m 7584, shell08_trans 6320. All have C except Fish_set (new).
Now linked (game OK): shell00 (7/7; shell00_i needed `u8 atk` and `if (atk)`:
the original's constant 1 loads with daddiu; jump table 0x68A3B0-0x68A3D0),
Fish_set (eft23b.c, 0x5589F0; the stage's fish spawn list, entries of 0x18
bytes: x,y,z, range, kind (<0 ends), count; `for(;;){if(kind<0)break;...}`
gives the original's loop layout), pl_guard_ck (pl_guard.c; return type u8
and `u8 ret` fixed the last instruction), set17 without set17_trans.
Near-matches improved: shell22_h 52 -> 3 off (switch with `case 1: case 0:
default:`; original compares 1 where we compare 0), shell08_rgba 48 -> 43,
set17_trans 88 -> 67 (declaration hill-climb; tool /tmp/dperm4.py moves one
declaration at a time and keeps improvements).
Tried and left: eft22_end_init (4 off, constant 35/27 goes to v1 not v0),
fish_type_set (6 off, sum/r swap a1/a2 whatever the declaration order),
eft11_i (the original loads the address of eft11_t0 just before the copy
loop; we load it first), Set20_set (only branch delay slots), print_tuto_message.

# Sixth assignment: monster AI files em14, em15, em17, em20, em21 and f_em_55B060

## f_em_55B060 (0x55B060-0x56653C): the monster command interpreter, 137 functions
All 137 functions have C in src/game/em/em_cmd_nm.c (include/em_cmd.h holds the shared
declarations). 99 match and are linked as em_cmd_r01..r26 (config/c_files.txt; jump tables
0x685A40-60, 0x685AE0-0C, 0x685B10-30). The 38 near-matches stay asm: flag_set/flag_clear/flag_ck,
the *_sel family (24/117 off: one shared shape, CMD_SEL_FUNC macro), end_command (514/571), the
pl_target_sel functions, angle_ck/range_ck/rnd32 and others (see em_cmd_nm.c, check.py).
Lesson: top-level macro invocations (CMD_SEL_FUNC) are copied into every run file by mkrun2.py:
delete them from runs that do not contain the function.

## Pipeline used for the big monster files (em21 first; same for 14/15/17/20)
Each file is ~110-150 functions of the same family (act/mv/fly/atk/dmg/die/demo/move dispatchers,
main, uvmove, effect_move, ef_move_sub = a per-animation sound/effect script of 3-11 KB).
1. m2c draft with a context file (all EMW/PLW/GAME_W structs and the prototypes of the other em files
   merged into one header, unparsable lines dropped): `DRAFT_CTX=ctx.c python3 tools/draft.py game --file F`.
   m2c knows the float arguments in $f12 only for the first parameter, so floats and ints after a float
   are wrong in its output; a small resolver rewrote those calls from the asm (lui+mtc1 constants).
   Functions with jump tables need the asm of their lit_NNN tables (tools/draft.py now accepts plain
   `lit_NNN` names, not only lit_NNN_ADDR).
2. clean-ups that were the same in every function (each confirmed by matches): m2c `return;` at the
   end of a case -> `break;` (the original has no extra `b`); `var = 4; if (c) {} else var = 6;` ->
   `if (c) x = 4; else x = 6;` with the store in both branches; three stores to consecutive stack
   words + `&sp` -> `f32 v[3]`; named EMW fields instead of M2C_FIELD(em, T, off); `EM_LYR(em, i)`
   = ((EML *)&em->x194)[i].v (stride 0x50) gives `sll,addu` in the original order; a leftover `0x41200000`
   literal stored in a float field must be written as a float (670.0f).
3. every function whose name carries an address suffix is `static` in the original and the
   statics matter for codegen: a caller keeps temporaries in caller-saved registers only when the static
   callee is defined EARLIER in the same file (hire_move needs static hire_move_sub1/2 in its file).
   So runs must contain a function together with its earlier static callees.
4. tools: `tools/alignall.py FILE [-v]` = align.py for every function with one check run (real
   differences only); `tools/mkruns3.py` splits the matching address-contiguous functions into runs
   (verifies each run on its own, finds jump-table ranges from the original binary, one merged rodata
   range per run including the padding between tables, drops `static` where the symbol is used by
   asm or another run). check.py shows "calls X, original calls Y" for address-suffixed names: ignore it,
   rebuild.sh is the judge.
5. NM files must compile (the build compiles every src/**/*.c); WIP files live in wip/ until they do.

## Status at the end of this assignment (all in src/game/em/, game OK, rebuild.sh byte-identical)
| file | functions | match (alignall = 0 diff) | linked runs |
|---|---|---|---|
| em_cmd_nm.c (f_em_55B060) | 137 | 99 | em_cmd_r01-r26 |
| em21_nm.c (f_em_5FFFD0) | 114 | 94 | em21_r01-r14 |
| em15_nm.c (f_em_5C2A80) | 120 | 100 | em15_r01-r14 |
| em14_nm.c (f_em_5B5290) | 115 | ~92 | em14_r01-r14 |
| em17_nm.c (f_em_5D9EE0) | 118 | ~93 | em17_r01-r19 |
| em20_ai_nm.c (f_em_5EBA10; em20_nm.c is the older setter file) | 148 | ~113 | em20_r01-r25 |
Every function of the five monster files and of the command interpreter has C; the near-matches stay asm.
New headers: include/em14.h em15.h em17.h em20.h em21.h (work areas, EMW+0x444), include/em_cmd.h.
Shared header edits: none beyond new files (em_cmd.h macro CMD_SEL_FUNC uses q instead of r in the
else loop, equivalent).

## Matching lessons from these files (each confirmed by a match)
- A function that passes its second parameter through unchanged (`em14_atk_end_sel(em, w)`) keeps a1 live:
  the compiler then puts the temporaries in a2/a3. Define the callee with the second parameter too.
  A callee that reads a1 without being given it (em14_to_fly(em, flag), flag = stale a1 of the caller)
  is declared with its parameter and called with `(em, 0)` here; the original passes nothing.
- `if (x != 0) return; call();` at the end of a move dispatcher is a single-case `switch (x) { case 0: ... }`
  (em_move06, act_dist_select, em14_to_fly: beq/b layout).
- Ladders of `||` compares over the same variable keep `sltiu at`; when the original shows `at` for the first
  compare and the && / || form does not reproduce it (em_mv03/05, em21 mv02/fly03/fly09) leave it.
- A float local assigned first (`f32 k = 0.4f;` then `x * k`) loads the constant before the cvt (em20_to_normal).
- `case 0x3E9: break;` as the FIRST case of a big switch is real when the original's range check starts at
  0x3E9 (em20/em14 ef_move_sub); and two neighbouring case labels in the opposite source order change the
  compare ladder (em15 ef_move_sub: 0x42E before 0x433).
- Local vector arrays: the order of the declarations decides the stack slots (later declarations lower);
  reading the sp offsets of the original and sorting the arrays by them fixed em14 ef_move_sub.
- m2c drops the store in the delay slot of a tail call (`j f; sb ...`): em14_to_swim, em15_to_tenjo/fly,
  em21_to_swim. tools/alignall.py shows it as 2-4 differing instructions at the end.
- Tables of 2 u16 per entry indexed `p = tbl + i * 2` (em_act_search2): the stride is bytes/2.
- `(f32)(u32)u8` gives the bltz fix-up of the original for `1500.0f * x13` in every init.

## Not done / next
- hire_move_sub1/sub2 (em21; my C is not the original's), em14_main (112 off), em17_main (21), em20_main (61),
  em21_main (27), em15_main (23): written as C, logic believed right (the conditions of the damage switch were
  rebuilt from the compare ladders), but the exact branch layout is not matched.
- em_cmd: *_sel family (7 functions, 18/117 off each, one shared macro), end_command, the pl_target_sel group.
- The m2c-based pipeline scripts lived in /tmp and are not committed; the steps are listed above.

# Seventh assignment: game overlay near-matches (agent D, 5 Oct 2026)
Goal: push the game overlay from 82% toward 100%. Workflow tools added:
- `tools/new_game_runs.py NM.c STEM "comment" [--skip a,b] [--dry]`: finds functions of a game near-match file that now
  match but are not in any linked run, emits only those into new run files (STEM<next>.c, via mkruns3 --only --verify)
  and appends the config/c_files.txt lines. Existing runs stay untouched. Skip functions that fail "inside their run".
  em_cmd runs must not contain the top-level CMD_SEL_FUNC lines (delete them from the new run by hand).
- `tools/greedy_sub.py FILE REGEX REPL`: applies a substitution to each match one at a time and keeps it when no function
  gets worse and the total drops (`py:` prefix = Python lambda on the match).
- `tools/regen_game_runs.py`: whole-file regeneration (NOT used: it turns statics global and breaks ef_move_sub).
- Statics that must stay `static` for codegen but are called from asm or other runs: keep them `static` in the run and
  add `name = 0xADDR;` to config/game_aliases.txt (em15 ef_move_sub_005CBC40 + its 5 helpers: sound_call*, quake_call,
  move_default). With the helpers global the 13 KB ef_move_sub is 333 instructions off; static it links byte-identical.

## Matching lessons (each confirmed by a match)
- `if (u8_returning_call() != 0)` gives an extra `andi 0xFF`; `if (u8_returning_call())` does not (em21 fly06).
- `x >= 2` -> `x > 1`, `x < K` -> `x <= K-1` (and `slti at` / `sltiu at` forms): em17_soukou_dm_sel_set, em21 fly03/05/09,
  em_mv03/05 `d <= 0xE38` in all five monster files.
- A trailing `else { return; } break;` in a switch case: delete the else (the compiler then falls into the epilogue
  instead of emitting extra `b` pairs). Fixed em_mv02/03/05, em_fly03 in em14/15/17/20/21.
- `u8 kind = em->kind; switch (kind) ... eft09_set(em, kind)`: declare the local `u32` (not u8) to avoid an `andi` on the
  argument (em15_init, em17_init). A callee whose extra arguments are stale registers in the original is declared with
  fewer parameters (em08_init: `void eft09_set(EMW *)`; em21_init: two args; em_mode_timer_sub: unprototyped
  `void Em_Mode_Chg();` called with 3 arguments although other callers pass 4).
- `x = a - b` where the original loads b first: `t = b; x = a - t;` (em21 fly05, `temp_f1 = em->adj_z; temp_f1 = w->dist - temp_f1`).
- `pos[1] = pos[1] + 20.0f` compiles as `20 + pos` (add.s operands swapped); `pos[1] += 20.0f; t = pos[1];` gives the original
  `pos + 20` order (em15 fly12).
- `f & 0xFF & 0x40` on a u8 compiles without the extra `andi 0xFF` of the original; `(u8)(f & 0xFF) & 0x40` has it
  (em_cmd cancel_prog_ck).
- tools/declbf.py found the declaration order for em_mv07 (all four monsters).
- tools/alignall.py ignores branch-address shifts, so a "2 off" function can still have a different tail layout
  (Em_Taisei_Ck, shell06_move_sub): look at check.py -v before trusting a small count.

## Unmatched game-overlay functions after this pass (139; size in instructions / real instruction distance, smallest distance first)
Distance = tools/alignall.py differing instructions (ignores branch-address shifts; rebuild.sh is the judge). Files are src/game/em/<file>.c etc.

em_atk04_005DE3F0 79/1 (em17_nm); em20_act_set 92/1 (em20_nm); em_cmd_sub_contents 22/2 (em_cmd_nm); eft18_set_com 26/2 (eft18_nm); takeoff_eff_set_005FC910 26/2 (em20_ai_nm); Em_Taisei_Ck 414/2 (em_taisei_nm); set05_m 450/2 (set05_nm); shell06_move_sub 783/2 (shell06_nm); em01_frame_reset 39/4 (em01_ai_nm); shell22_h 64/4 (shell22_nm); em01_reset_char_set 76/4 (em01_ai_nm); eft22_end_init 141/4 (eft22_nm); else_ck 43/5 (em_cmd_nm); em_fly18_005F1530 117/5 (em20_ai_nm); em04_act_set 158/5 (em04_nm); eft04_t 1266/5 (eft04_nm); em01_effect_move 24/6 (em01_ai_nm); em02_effect_move 24/6 (em02_ai_nm); em07_effect_move 24/6 (em07_ai_nm); em14_effect_move 24/6 (em14_nm); em15_effect_move 24/6 (em15_nm); em16_effect_move 24/6 (em16_nm); em17_effect_move 24/6 (em17_nm); em20_effect_move 24/6 (em20_ai_nm); em27_effect_move 24/6 (em27_nm); em_cmd_position_set 25/6 (em_cmd_nm); em08_effect_move 31/6 (em08_ai_nm); em21_effect_move 31/6 (em21_nm); fish_type_set 85/6 (eft23_nm); em_fly22_005F1B80 87/6 (em20_ai_nm); em_fly22_006041A0 101/7 (em21_nm); em_dmg15_00605DC0 201/7 (em21_nm); em_atk11_0056E160 356/7 (em01_ai_nm); print_tuto_message 40/8 (tuto_nm); Set20_set 58/8 (set20_nm); em_cmd_ninshiki_timer_sub 32/10 (em_cmd_nm); em10_turn_sub 42/10 (em10_nm); em_act_search 51/10 (em_core_nm); em_atk30_005F4090 80/10 (em20_ai_nm); em_cmd_area_move_ck 93/10 (em_cmd_nm); em_eye_search_set 345/10 (em_core_nm); eft16_m 1758/10 (eft16_nm); em_fly15_005F1120 49/11 (em20_ai_nm); em_cmd_pl_ride_ck 88/11 (em_cmd_nm); shell22_i 297/11 (shell22_nm); Em_Dmg_Sys 489/11 (em_taisei_nm); em_atk26_005F3CE0 103/12 (em20_ai_nm); item_theft_005EC560 73/14 (em20_ai_nm); em_mov01_005B06E0 254/14 (em12_nm); em09_effect_move_005AC940 78/15 (em09_nm); set14_trans 787/15 (set14_nm); em20_material_sub 126/17 (em20_ai_nm); em20_init 403/17 (em20_ai_nm); em_demo00_005A0FF0 449/17 (em08_ai_nm); em_fly10_005C6430 125/18 (em15_nm); hire_req_set_0060C0C0 48/19 (em21_nm); em_atk11_005F3170 74/19 (em20_ai_nm); em_cmd_before_stage_ck 80/19 (em_cmd_nm); em_hagitori_lv_up 116/19 (em_master_nm); em_fly10_005F0730 129/19 (em20_ai_nm); em_dmg07_005B9ED0 155/19 (em14_nm); em14_init 223/19 (em14_nm); em07_main 317/20 (em07_ai_nm); em21_target_ang_calc 43/21 (em08_ai_nm); em_cmd_range_ck 112/21 (em_cmd_nm); em17_main 461/21 (em17_nm); em_range_set 45/23 (em_core_nm); em15_main 543/23 (em15_nm); em_fly14_006030F0 169/24 (em21_nm); em_fly16_00603460 173/24 (em21_nm); em_atk06_00604AE0 183/24 (em21_nm); Em_Taisei_Set 27/26 (em_master_nm); em_cmd_boss_atk_ck 84/26 (em_cmd_nm); em_cmd_flag_clear 68/27 (em_cmd_nm); em_cmd_boss_same_stage_ck 80/27 (em_cmd_nm); em_fly03_005B7EE0 105/27 (em14_nm); em21_main 834/27 (em21_nm); em_cmd_rnd32 101/28 (em_cmd_nm); em_fly29 109/28 (em15_nm); em_fly30 109/28 (em15_nm); senko_ck 100/29 (em_core_nm); em_cdm_act_flag_ck 54/30 (em_cmd_nm); em_cmd_near_pos_ck 93/30 (em_cmd_nm); NextStage_Dir_Set 135/30 (em_cmd_nm); area_route_rnd32 32/31 (em_cmd_nm); em_fly13_005C6970 193/31 (em15_nm); smell_ck 161/35 (em_core_nm); em_fly27 163/38 (em15_nm); em_fly33 183/38 (em15_nm); em_fly34 183/38 (em15_nm); em_fly13_005F0CF0 216/38 (em20_ai_nm); em09_material_sub 120/40 (em09_nm); em_fly08_005F02E0 165/41 (em20_ai_nm); em_fly31 179/41 (em15_nm); em_fly10_00602A70 144/42 (em21_nm); em_fly11_00602CC0 144/42 (em21_nm); shell08_rgba 236/43 (shell08_nm); em09_act_set 102/45 (em09_nm); em_demo00_005DFDF0 573/46 (em17_nm); em_cmd_pl_ang_sel 130/49 (em_cmd_nm); em_cmd_flag_set 72/50 (em_cmd_nm); em_hate_suu_set 88/52 (em_core_nm); em_mv00_005DC550 84/54 (em17_nm); em_atk21_005F3440 534/59 (em20_ai_nm); em_atk08_005F2680 584/61 (em20_ai_nm); em20_main 621/61 (em20_ai_nm); eft05_t 290/65 (eft05_nm); set17_trans 218/73 (set17_nm); em_fly04_00602030 146/76 (em21_nm); ef_move_sub_0058E500 950/76 (em04_nm); em_cmd_angle_ck 175/80 (em_cmd_nm); em_char_set 234/85 (em_core_nm); em14_uvmove 125/88 (em14_nm); em15_uvmove 125/88 (em15_nm); em17_uvmove 125/88 (em17_nm); em20_uvmove 125/88 (em20_ai_nm); em21_uvmove 125/88 (em21_nm); em_cmd_horm_pos_ang_ck 146/94 (em_cmd_nm); em_cmd_flag_ck 152/101 (em_cmd_nm); hire_move_sub1_0060BCA0 142/103 (em21_nm); em14_main 747/112 (em14_nm); NextStage_No_Set 214/114 (em_cmd_nm); em_cmd_escape_area_set 203/115 (em_cmd_nm); em_cmd_dansa_sel 130/118 (em_cmd_nm); hire_move_sub2_0060BAD0 112/128 (em21_nm); hire_move_sub2_005A69A0 110/129 (em08_ai_nm); Em_Master_Change 306/158 (em_master_nm); eft11_i 168/165 (eft11_nm); em_cmd_ground_area_move 253/171 (em_cmd_nm); em12_main 415/183 (em12_nm); hire_move_sub1_005A6B70 204/184 (em08_ai_nm); em_cmd_all_pl_target_sel 225/213 (em_cmd_nm); shell08_m 1896/240 (shell08_nm); em_cmd_samestage_pl_target_sel 263/250 (em_cmd_nm); neck_ang_set 348/251 (em_core_nm); em_cmd_st25_pl_target_sel 283/276 (em_cmd_nm); em_cmd_end_command 569/289 (em_cmd_nm); shell08_trans 1580/561 (shell08_nm); em_neck_move_sub 489/642 (em_core_nm)

## More lessons from the second half of this pass (each confirmed by a match)
- Stale-argument calls: m2c writes `fn(temp_a1)` / `fn(1, temp_a2)` / `em20_horm_main(em, 1, temp_a2)` where the original passes
  only `em` (or `em, 1`): em14/17/20 horm_main, tossin_move, fly_adjy, fly_adjy2, senkai_target. The switch variable then no
  longer lives in a0 and the whole function's register allocation falls into place (about 25 functions).
- CMD_SEL_FUNC (include/em_cmd.h, 7 functions + body_status_sel): use the parameter `p` directly instead of a copy `q = p`
  (the original keeps the script pointer in the parameter register). tools/q2p.py does it per function. `*p++ != x` compiled
  differently from `vv = *p; p += 1; x != vv`; the latter matched with the operands as `(u8)em->x762 != vv`.
- `dd = CalcDistanceXZ(...); em->work08 -= 1; if (dd <= 500.0f || em->work08 < 0)` (em15/17/20 fly09): a float temp before
  the decrement.
- Case order matters twice: the compare ladder is emitted in the REVERSE of the source order of the case labels, and a case
  that only breaks (`case 0x406: break;`) must be written AFTER the body of its neighbour 0x405 (em17 ef_move_sub: three branch
  targets differed although alignall said 0; only rebuild.sh caught it). A `goto block_N` pair from m2c is usually
  `if (a == b || b == 0xFF) { equal block } else { other }` (em15 fly08).
- `if (x == 1 || x == 2 || x == 3)` instead of `(u32)(x - 1) < 2 || x == 3` (em14 act01) and removing a trailing
  `else { return; }` before `break;` (act01).
- A static callee must stay static for codegen: a whole group (sound_call*, quake_call, move_default, ef_move_sub) goes into ONE
  run as static functions with aliases (tools/static_group_run.py, em15 and em17 done). It only works when every function in the
  group matches by itself (em17 sound_call needed `em->ex[0x1B]` and `joint == 0x14 || joint == 0x1A`).
- Tried without success (left as is): the *_effect_move family (12 functions, 6 off each: the original keeps the eff byte in
  a2 and the constant 1 in v1, mine uses v1/v0; 10 source forms, declbf and a permuter run found nothing; the permuter scores
  a version 0 that differs only in branch targets, do not trust it for branch shape), em20_act_set (addiu vs daddiu on
  `kind = 3`), Set20_set (delay-slot nops), eft04_t (colour packing order), em_cmd_sub_contents, print_tuto_message
  (s1/s2 swap of loop variables), fish_type_set, em_fly22 (float register numbers), em15 fly10 (copy of w to s0 first).

# Eighth assignment: main module 0x1C0000-0x230000 and 0x24A240-0x2814E0 (agent D, 5 Oct 2026)
Scope: Capcom game code only (CRI Sofdec/ADX 0x1C1958-0x217xxx and Sony/MWCC runtime skipped). Almost every
Capcom function in these ranges already had C in an *_nm.c file, so the work was turning near-matches into
byte matches and linking them. Tools:
- `tools/new_game_runs.py` and `tools/mkruns3.py` now work for main (module taken from the path src/main/...).
  mkruns3 also finds main jump tables (rodata 0x340000-0x3C0000) and accepts calls whose original target has no
  symbol ("?"); it no longer runs off the end of the image when a table scan misfires.
- `tools/permapply.py FILE FUNC`: copies the zero-score function from `tools/perm.py main FUNC FILE -j1
  --stop-on-zero` (build/perm/FUNC/output-0-*) back into the near-match file. A queue of small near-matches
  (1-25 instructions off) run one after the other found about a third of them within 4 minutes each. The result
  often contains junk like `if ((m && m) && m) {}` or a `new_var` temporary: it only reproduces a branch layout.
  (Do not run the queue while tools/rebuild.sh runs: rebuild wipes asm/.)
- `tools/lbdraft_jt.py` takes MOD=main (m2c draft of main functions that use jump tables).
- config/main_aliases.txt (new; the build already adds config/<module>_aliases.txt): `frame_check_001263F0` is
  frame_check under a second name so one file can call it with its real float-LAST prototype
  `(PLW *, int, f32)` next to plf.h's float-first guess (the float variable's mov.s is then scheduled last).

## Linked this pass (main OK, all five OK)
hk10-hk15 (10 more f_hk functions), sk10-11, ud09-12, netbgm03, ms03, ms04 (ms_net_patch_set, 6920 B), and
pl_snd01 = 0x24A240-0x2542E0: pl_local_init, pl01_effect_move, sound_call*, wall_sd_req, ashi_*, yoroi_sd_req,
move_default and ef_move_sub (39 760 bytes, the per-motion sound/effect script of player kind 1).

## ef_move_sub (pl_snd_nm.c) - what turned 8620 differing instructions into 0
- `GW8(0xD3)` -> the named field `game_w.pl_num`; `for (i = 0; i < n; i++) { p = &player_work[i]; ...}` (the
  compiler's own pointer induction gives the original's `lui s2` AFTER the loop guard; `p = player_work` in the
  for header put it before); `s16 i`; `p++, i++` order in the header was irrelevant once p became inductive.
- The ladder of compare constants keeps registers a0-a3 alive when a case body later uses the same constant: the
  original's "stale argument" calls are real constants. `Code_Make(STALE, 2, STALE, 2)` was `Code_Make(35, 2, 36, 2)`
  (a1 = 35 and a2 = 36 left over from the ladder), `sound_call2(pl, STALE, STALE)` was `(pl, 42, 41)`, and
  `sound_call(pl, 4, STALE)` was `(pl, 4, 64)`. Replacing the placeholders made the whole ladder match.
  Lesson: when the original passes an argument without a load, look at the compare ladder above for the constant.
- A call that passes one extra stale argument (`move_default(pl, w)`: a1 = w) needs the extra parameter in the C
  signature, not just a call with one argument.
- Eft20_set_pl is (f32 scale, PLW *, s16, s16) and was called with the wrong prototype in the nm file.
- Float loop variable: `f32 f = 26.0f; for (...) { if (frame_check(f, ...)) ...; f += 4.0f; }` (not constants).
- Two consecutive calls with the same float variable: `pl = (q = pl);` between them and the second call using `q`
  reorders the second mov.s (found with the permuter on a 20-line harness, tools/perm.py-style dir with a
  hand-written target.s; scratch harness compiled with the same flags).
- The switch on `pl->kind` (jump table 0x36E0F0) has kind 0 as `case 0: default:` placed LAST, kinds 1, 2, 5, 3, 4
  before it: the table content is only checked by rebuild.sh, not by check.py/alignall (they ignore the table).

## Other lessons confirmed by matches this pass
- A call with arguments the original does not set (`hk_key_space` -> `sk_henkan_sub()`, `cmd_henkan()`,
  `Net_fade_check()`, `net_swdata()`, `net_shot_ng_ck()`, `Ncm_mmbb_spr_load()`) is declared unprototyped `int f();`
  and called without arguments; a pointer that is then no longer passed also frees its register (hk_key_f7/f6).
- `if (hk_kanainp_ck() != 0)` on a u8-returning function adds an andi; `if (hk_kanainp_ck())` does not.
- `u16 field += n` adds an `andi` of n; `field = field + n` does not (cmd_kakutei_all, sk_reibun_input).
- `switch (x) { case 0xB: case 3: ...}` for an `x == 3 || x == 11` with beq/beq/b layout (sk_set_yn_kigou_f).
- Small extern objects (<= 8 bytes) are gp-relative; an unsized `extern T x[]` gives lui/addiu (dakuten_1257 needs
  `[2]`, NET_CON_TEX and D_6E9700 need `[]`).
- Absolute reads of an overlay address are best written as an extern unsized array (`D_6E9700[0]`): the compiler
  then sees no aliasing with net_common_w and hoists the load above the stores like the original does.
- Statement order for `field = const; x89++`: write the increment first (the original's li/lbu register choice).
- A jump-table switch whose cases are written in the original's BLOCK order (Equip_ok_ck/Get_equip_bit: kinds 2,3,5,4,0)
  is needed for the table; alignall reports 0 for the wrong order because it ignores table addends.
- `x >= y` on two s16 fields compiled as `slt at` with the loads in the original order when written `y <= x`.
- `0 < r` instead of `r > 0` gives `slt at, zero, v0` + beq (instead of blez) for an int result.
- Shared header edit: include/netcw.h: x7E/x7F/x84(s32)/x8B named from padding, x89/x8A are u8 (lbu in
  ms_net_patch_set); ms_nm.c/ms01-03 users still match.

## Eighth assignment, later additions
Linked by the permuter queue and by hand (all main OK): bgm (lobby_bgm_set, stage_bgm_set), cmd (cmd_prev_bun, cmd_prev_kouho,
Set_KouhoTable), cngmsg (Write, WriteFloat32/ReadFloat32, CnInetNetworkInitialize_online, swapb), aqcmd (AQQuickSortSub),
netwk (return_to_net_top_menu, Net_kb_input_init2, net_swdata3), camr (ZoomBaseAngleRail, RollAngleRail, dDivComplex),
camarea03-04 (default_area_data + StageCamInit + CamAreaAttribChk), ud (Copy_user_id, Gun/Equip helpers), hk (key_delete,
l_cursor and others), qstb04 (Modori_dama_ck). Main had duplicates of some of these from another agent (ud11/12, cmd05,
aqcmd04, sndb01/02): the merge keeps main's runs and pl_snd01 (a superset of sndb01/02).
More lessons (each confirmed by a match):
- A callee defined EARLIER in the same file and `static` keeps the caller's argument registers alive: StageCamInit does
  not save cw across `default_area_data(cw)` only when default_area_data is static in the same translation unit; the
  callee therefore has to match too (it was linked together with StageCamInit in camarea03).
- `(u32)float_value` written directly produces the original's inline c.le.s / sub.s / or sequence; a helper function
  (even `static inline`) does not (Get_cam_grid_XZ).
- `u8 field` read where the header says s8: cast at the use `(u8)PitMenu.x0F` (lbu) rather than changing the header.
- Tables of at most 8 bytes are gp-relative: `extern s16 receive_mark_pos[2][2];` (Put_receive_mark), unsized `[]` is not.
- Integer + pointer operand order: `v + (s32)mission_area` gives the original `addu v0,v0,v1`, `mission_area + v` the
  reverse (Em_data_com_adrs_get, Em_data_st_adrs_get, Start_item_data_adrs_get); `p += idx; *p` where the original
  advances the pointer register.
- `which != 0 ? a : b` vs `which == 0 ? b : a` swaps the branch sense and which load comes first.
- `c = x14 != 0 || x15 < 0x27 || x15 > 0x2B` (CamAreaAttribChk): `> 0x2B` (not `>= 0x2C`) keeps the compare result in `at`.
- A switch's case labels are tested in the REVERSE of their source order, and that holds for groups: hk_key_eisuu needed
  `case 2: case 7:` then `case 10: case 15:` then `case 0, 1, 6, 8, 9, 14` (descending compare chain in the asm).
- Not solved (left near-match): hk_key_eisuu (2 off: one `b`+nop pair), Seisan_ok_ck (register naming of locals, 8 locals),
  Get_cam_grid_XZ (10), Em_direct_set (register naming, K&R parameter), str_gattai (needs MWCC's own va_start; the nm
  file's `va_start` is an implicit call), quest_condition_prog (124 off), the *_effect_move family.

# Fourth assignment (main module 0x1C0000-0x230000, 0x24A240-0x2814E0)

Linked, all five modules OK (rebuild.sh):
- ud12.c Seisan_ok_ck (0x2743E0-0x274654). Fix: the final test is `cnt == 0 || any != 0`
  (the older C had it inverted), and `for (j = 0, e = ent; ...)` puts j=0 first.
- boot01.c/boot02.c: main (0x22FDD0), SlashToBackslash, InetDbgPrint (varargs stub with four
  named params), InetConnectInitialize, LobbyToMcsInitSocket, LobbyToMcsInit. cnNet_ModuleLoad
  (0x22FE00, 12 bytes) sits between main and SlashToBackslash and stays asm: always check
  tools symbol list for hidden functions between two linked ones.
  `switch (*CurDevice) { case 2: case 3: ...; break; default: break; }` gives the original
  "beq, beq, b" with the empty fall-through (the if/else forms do not).
- camq1.c Fish Wyvern / Legend Sword camera requests (0x225F50).
- ncm01-03.c network connection message drawing (ncm_char_length, ncm_get_char_size_num,
  ncm_center_x, Ncm_mssage_disp, Ncm_err_mssage_disp). ncm_char_length must return u8 with a
  u8 result variable (daddiu constants). Spilled s16 parameters of the callee are `int` params.
  A `do {} while` guarded by `if (*lines != 0)` and a copy `x0 = x` inside the guard
  matches the original compare of the first x only.
- flfnt01-05.c: the Capcom "fl" bitmap font library (0x216490-0x217E48, 27 functions, no C
  before). Linked: Create, GetSystemMemorySize, StackReset, Init, CacheFlush, SetSize, Locate,
  SetZ, SetPalette (`(s16)pal % 32`), SetHalftype, Draw, DrawAll, MakeHandle, PaletteTrans,
  DrawStart, flnecAscii2Sjis. np (gp pointer) is the font work FNP (struct in flfnt_nm.c):
  five request lists of 0x80 FREQ (0x10 bytes), string buffer, glyph cache, texture and
  palette handles, DMA packet cursor. Whole library with near-matches in flfnt_nm.c.
  Near-matches (compile, logic believed right): flfntSetPalData (99/108, the original steps
  the destination by 2 twice, no way found to stop MWCC folding it), flfntPrintf (73/103,
  hand-made va_list: &fmt + 0x48 - 56; original re-reads np for every field), flfntFontPuts
  (18/108, char register a0 instead of a1), flfntDrawTerm (18/54, the 36-bit mask is
  dsll32/dsrl32 in the original, MWCC folds it to and), flfntSjis2Index (6/21),
  flfntSjis2Jis (51), flnecCheckFont (12), flnecCheckString (32), flnecExpandFont (88; the
  original steps the output pointer after every pixel pair), flnecReloadTexture (7/189).
  Not written: flfntFontPutc (GS packet builder, 1260 bytes).
- ncm_nm.c also holds Ncm_mssage_disp_option (37/86), Ncm_br_mc_mssage_disp (95/200),
  Ncm_menu_disp (139/162; the switch needs explicit case 0, 2..11 for the jump table at
  0x373460, not yet registered) and ncm_str_disp_sub (34/156, saved-register order of s/x/size;
  declbf over six locals found nothing better).
Near-matches left: Quest_start (10 off: `mission_area` loaded before `m->o[0]`, the original
loads o[0] first; tried operand order, temp variable), Get_cam_grid_XZ (the original loads the
cell origin after the float conversion, u8 result), hk_key_eisuu (original returns straight to
the epilogue from the failed tests; every if/else, break and return form compiled to a stub).
Not Capcom, skipped: Sofdec/ADX/CRI middleware (0x1C4000-0x216000), PS2 kernel stubs (0x254300+),
libcdvd-style RTC helpers (0x27C698).

# Fifth assignment (main module 0x1C0000-0x230000, 0x24A240-0x2814E0)

Linked, all five modules OK (tools/rebuild.sh, 5 Oct 2026):
- cam/camq0.c (0x225D80-0x225F4C): QuestClearCameraRequest, RedDragonEscapeCamera, F_DragonEscapeCamera,
  PlayerDieCameraRequest, PlComebackCameraRequest, PilebunkerCameraRequest. All matched first or second try.
  QuestClearCameraRequest = one `switch (e[2])` with cases 7, 2, default that only *prepare* the camera number
  (`if (x34 == 0) cam = N; else return;`, not `goto`), then ONE DemoCameraRequest call after the switch.
  A tail call with fewer arguments than the callee has (RedDragon: `a1 = a0; a0 = 0x1C; j`) needs the
  unprototyped declaration `void DemoCameraRequest();` (the 3-argument prototype adds `a2 = 0`).
- cam/camr2s01.c GetNearSection (0x223760): declbf found the declaration order.
  `f32 dist[16]` is the stack array (frame 0xC0).
- sound/flmw01.c flmwVSyncCallback (`flAdxControll(0)`, the 0 is in the tail-call delay slot) and flmwFlip: the
  hardware read `*(u64 *)0x12001000` (GS CSR, bit 13 = field) must come first into a local, then the two stores.
  `a >= b` was written `b <= a` (loads in the original order).
- tex/apx01.c (0x217E50-0x218588, new include/apx.h): the Capcom APX texture helpers plAPXGetMipmapTextureNum,
  plAPXGetPaletteNum, plAPXSetContextFromImage, plAPXSetPaletteContextFromImage, plAPXGet*AddressFromImage,
  GetAPXFileHeader. The callees return `int` (a `u16` return type adds an andi). The colour-channel fields
  are written in the original's store order (c1: shift before bits for the "all zero" cases; the 0x1C/0x34 pair is
  written twice on purpose). Two holdouts stay original bytes in the file via config/c_rawfuncs.txt:
  GetAPXPixelMipmapAdrs (12 off) and GetAPXPaletteAdrs (50 off); the C is in apx_nm.c. GetAPXPaletteAdrs calls
  GetAPXFileHeader(img) and GetAPXPixelMipmapAdrs(img, 0) without saving `img` across the first call (a0 is never
  copied to a saved register); no C form that does this was found.
- flfnt/flfnt06.c flnecReloadTexture (permuter output; `j = 0; arr[j++] = ...` for the first three texture
  handles, `flReloadTexture(j, arr)` passes the count 0x23 as first argument). Clean source is in flfnt_nm.c.
- net/cng00.c CnInetNetworkAveTcpPoll (0x22E090, jump table 0x36CCB0-0x36CCC8): explicit `case 4: case 5:`
  makes the 6-entry table.
Written but not matching (nm files): flfntFontPutc (flfnt_nm.c, 1260 B: GS TEX0/TEX1 packet when the texture page,
palette or the "size != 22x22" flag changes, then a 0x50-byte sprite packet; logic follows the asm, compile is
completely different: 315/315), flSndPackLoadSub2 (sound/flsnd_nm.c, 118/122: the original keeps two copies of
`pack` in saved registers), flSndPackLoadBG2/flSndOutputMode (sound/flsnd00.c, not built: the original has
the "else" part first, 7 and 8 instructions off), flSndJointSet (sound/flsnd02.c, 1 off: the first memcpy length
is loaded through a0 in the original, s0 in mine).
Near-matches left in the font library (flfnt_nm.c): SetPalData 99/108, Printf 73/103, FontPuts 18/108,
DrawTerm 18/54, Sjis2Index 6/21 (original order: `sra v1,v0,8; addiu a0,v1,-33; andi v0,v0,0xFF`, the
compiler always starts with the andi), CheckFont 12/26 (original stores the unmasked idx, MWCC reuses the masked
register), CheckString 32/86, ExpandFont 88/106. ncm_nm.c: ncm_str_disp_sub 34/156, Ncm_mssage_disp_option
37/86 (register numbering of y/step/lines), Ncm_br_mc_mssage_disp 95/200, Ncm_menu_disp 138/162 (the original
walks one induction variable `s1 = i*4` for both the string table and `s1<<1` for the s16 position table; the
C compiler makes two). Quest_start stays 10 off: the original keeps `m = mission_area` in s0 and
reads `m->o[0]` BEFORE reloading the global for `quest_w.x94 = mission_area + m->o[0]`, and it stores x38/x36 after
the loads of o[4]/x94; every operand order / pointer cast tried (u8 *, int, s32, either side) compiles identically.
Permuter (-j1, 200-420 s each) was run on 38 functions of this assignment; only flnecReloadTexture reached
a real match (and linked). flSndJointSet "reached score 0" but the applied source still showed the one diff in
check.py: always re-run check.py after tools/permapply.py. Hit rate for 1-15 instruction diffs: about 1 in 20.
Also linked: boot/boot03.c cnNet_ModuleLoad (`CngNetPS2ModuleBootInitialize(1, 1)` as a tail call), sound/flsnd00.c
(flSndAllStop, flSndPortStop(int), flSndSetRev(int, int, int, int, int): a wrapper whose callee prototype has
s16/u8 parameters gets the dsll32/andi conversions in the tail-call delay slots) and sound/flsnd02.c
(flSndJointInit).
Lessons: `0x1C0000..` ranges: a function that "matches in check.py" inside an nm file is NOT linked until its
own run is registered (all five such cases here were already handled). Shell: `pkill -f NAME` kills the shell
that runs it when NAME appears in the command line; use PIDs.

## Matching lessons from the second half of this pass (all confirmed by a match)
- `p = table; p += n; return *p` (pointer advanced in its own register, then dereferenced) gave the original
  `addu v1,v1,v0` with the base pointer as destination (Stage_mv/item/unique_data_get, qstb05/06). Plain `table[n]`
  and `*(T *)((int)table + n * 4)` give the same instructions with the index register as destination.
  Quest_str_get (5 off, only register numbers) uses the same idea.
- A K&R/ANSI `s8` parameter that the loop then increments gets a sign extension per iteration; the original
  widens once: `void f(int arg) { int n = (s8)arg; ... n++ ... }` (ext_pick_point_tbl_clr). The two pointers of a
  `do { ... } while` loop took their registers from the declaration order (`s8 *t;` before `int last;`).
- `if (0 < n) { do { ... i++; t++; } while (i < n); }` (ext_pick_point_fifo_ck): `0 < n` gives
  `slt at,zero,n; beq at,zero` where `n > 0` gives `blez`; and `n = x3B; if (n >= 20) { n--; ...` loads the byte
  once and compares before subtracting (the other order loads `quest_w.x3B` into a different register).
- `x ? 0 : 1`, `!x`, `x == 0 ? 1 : 0` on an s8 all compile to xor/sltiu here, but the original of
  CngSessionStart_online / CngNetMcsP2PPoll (p->host = ...) uses `addiu v0,zero,1; movn v0,zero,v1`; no source
  form found (3 instructions off in each, nothing else).
- check.py counts a call to an overlay address (jal into game/lobby code, e.g. func_5C5E20) as one differing
  instruction ("original calls ?") although rebuild.sh links it fine: Quest_str_get's real diff is 4, not 5.
- A tail call that passes fewer arguments than the callee's prototype wants needs an unprototyped
  declaration (camq0.c, `void DemoCameraRequest();`).

## Unmatched Capcom functions left in my ranges (5 Oct 2026, end of the fifth assignment)
78 functions, 69 640 bytes (was 106 functions, 73 448 bytes at the start of this pass), sorted by size. `N off of M` = instructions that
differ from the original in the near-match C (file in parentheses; *_nm.c is not built). Sofdec/ADX/CRI
(0x1C4000-0x216000) and PS2 kernel stubs (0x254300+) are not counted.

- 1C1958  9976 cftraw_CnvMbRAW8toPlaneARGB: hand-written MMI asm (CRI middleware): no C possible
- 21A9F0  5148 eft20_t: 1209 off of 1287 (eft/eft20_nm.c)
- 219750  4600 eft20_m: 885 off of 1150 (eft/eft20_nm.c)
- 218670  4316 eft20_i: 1032 off of 1080 (eft/eft20_nm.c)
- 21E310  4280 HdMerge: no C yet; Sony .HD bank merger (4 chunk types, padding loops), next candidate for a long session
- 21BF50  3564 eft20_pos_set: 842 off of 891 (eft/eft20_nm.c)
- 22A410  3420 quest_condition_prog: 532 off of 855 (quest/f_quest_nm.c)
- 21F9B0  2664 cam_sub_std: 63 off of 668 (cam/cam_nm.c)
- 225510  2152 k_HitEmCamera: 446 off of 538 (cam/camr5_nm.c)
- 2206B0  2096 cam_sub_stg: 410 off of 524 (cam/cam_nm.c)
- 22B170  1840 remuneration_item_set: 51 off of 460 (quest/f_quest_nm.c)
- 223FE0  1652 GetOrthogonalPoint: 234 off of 413 (cam/camr6_nm.c)
- 216F90  1260 flfntFontPutc: 315 off of 315 (flfnt/flfnt_nm.c)
- 226C30  1136 Quest_start: 10 off of 284 (quest/f_quest_nm.c)
- 2248C0  1000 Spline: 259 off of 262 (cam/camr4_nm.c)
- 222410   900 point_cam_sub: 28 off of 225 (cam/cam_nm.c)
- 22C050   812 Quest_net_sub: 179 off of 205 (quest/f_quest_nm.c)
- 225050   796 Cardano: 99 off of 203 (cam/camr5_nm.c)
- 2286D0   756 Item_regained: 158 off of 189 (quest/f_quest_nm.c)
- 22EAA0   656 CngNetAQPacketMake: 147 off of 164 (net/cng_nm.c)
- 224DD0   632 DKA5: 153 off of 158 (cam/camr5_nm.c)
- 228130   576 Quest_next_em_set: 17 off of 144 (quest/f_quest_nm.c)
- 228440   552 stolen_item_stack: 7 off of 138 (quest/f_quest_nm.c)
- 2289D0   508 Share_item_stack: 86 off of 127 (quest/f_quest_nm.c)
- 22D2F0   504 get_AQdata: 16 off of 126 (aq/aq_nm.c)
- 2160D0   488 flSndPackLoadSub2: 118 off of 122 (sound/flsnd_nm.c)
- 228BD0   468 Net_Share_item_stack: 71 off of 117 (quest/f_quest_nm.c)
- 223980   460 GetNearPoint: 122 off of 124 (cam/camarea_nm.c)
- 22BA40   456 quest_item_ck2: 109 off of 114 (quest/f_quest_nm.c)
- 22E690   440 CngRecvMsg: 87 off of 110 (net/cng_nm.c)
- 216DE0   432 flfntFontPuts: 18 off of 108 (flfnt/flfnt_nm.c)
- 2166E0   432 flfntSetPalData: 99 off of 108 (flfnt/flfnt_nm.c)
- 217A60   424 flnecExpandFont: 88 off of 106 (flfnt/flfnt_nm.c)
- 22E900   416 CngNetAQDataPut: 98 off of 104 (net/cng_nm.c)
- 225370   416 k_HitWallCamera: 46 off of 104 (cam/camr5_nm.c)
- 216920   412 flfntPrintf: 73 off of 103 (flfnt/flfnt_nm.c)
- 22E450   408 CngNetMcsP2PPoll: 3 off of 102 (net/cng_nm.c)
- 22F2A0   396 CngNetAQdataToObj: 90 off of 99 (net/aqcmd_nm.c)
- 22FC50   384 movie_draw: 10 off of 96 (movie/movie_nm.c)
- 22A220   368 em_work_serch2: 43 off of 93 (quest/f_quest_nm.c)
- 21D470   368 em_status_ck: 15 off of 92 (sound/bgm_nm.c)
- 22CBF0   356 AQ_init: 45 off of 89 (aq/aq_nm.c)
- 228DB0   344 Share_item_num_ck: 61 off of 86 (quest/f_quest_nm.c)
- 217C10   344 flnecCheckString: 32 off of 86 (flfnt/flfnt_nm.c)
- 223C90   336 cam_rail_move: 83 off of 84 (cam/camr2_nm.c)
- 227CF0   320 Em_direct_set: 53 off of 80 (quest/f_quest_nm.c)
- 218360   308 GetAPXPixelMipmapAdrs: original bytes linked via c_rawfuncs; C in tex/apx_nm.c (12 off)
- 22D9C0   300 AQ_data_put: 31 off of 75 (aq/aq_nm.c)
- 21F470   288 SetCameraData: 65 off of 72 (cam/cam_nm.c)
- 223870   264 get_near_point_sub: 18 off of 66 (cam/camarea_nm.c)
- 223190   264 Get_cam_grid_XZ: 20 off of 66 (cam/camarea_nm.c)
- 22DAF0   248 pl_data_put: 38 off of 62 (aq/aq_nm.c)
- 229E60   244 quest_em_init_sub2: 49 off of 61 (quest/f_quest_nm.c)
- 223410   232 Area_XZ_Check: 57 off of 58 (cam/camarea_nm.c)
- 2184A0   232 GetAPXPaletteAdrs: original bytes linked via c_rawfuncs; C in tex/apx_nm.c (50 off)
- 22E350   220 CngSessionStart_online: 3 off of 55 (net/cng_nm.c)
- 217680   220 flfntSjis2Jis: C in flfnt_nm.c (45/55 off: the original compares 64-bit zero-extended byte copies with slti)
- 227160   216 Quest_pl_stage_init: 11 off of 54 (quest/f_quest_nm.c)
- 2162F0   212 flSndJointSet: 1 off of 53 (sound/flsnd02_nm.c)
- 229980   200 Em_hagi_point_cnt_ck: 20 off of 50 (quest/f_quest_nm.c)
- 217550   200 flfntDrawTerm: 18 off of 54 (flfnt/flfnt_nm.c)
- 22DBF0   196 pl_AQ_put: 14 off of 49 (aq/aq_nm.c)
- 22D690   164 self_data_ctrl: 2 off of 41 (aq/aq_nm.c)
- 22DEA0   136 host_change: 9 off of 34 (aq/aq_nm.c)
- 224660   136 ZoomRateCalc: 8 off of 34 (cam/camrz01.c)
- 22D740   120 set_other_data: 18 off of 32 (aq/aq_nm.c)
- 2290B0   116 Ext_pick_point_init: 45 off of 49 (quest/f_quest_nm.c)
- 217D70   104 flnecCheckFont: 12 off of 26 (flfnt/flfnt_nm.c)
- 229AE0    96 str_gattai: 25 off of 25 (quest/f_quest_nm.c)
- 2270A0    92 Quest_retire_set: 15 off of 23 (quest/f_quest_nm.c)
- 22EDE0    84 CngNetAQPoll: 10 off of 21 (net/cng_nm.c)
- 228370    84 Quest_str_get: 5 off of 21 (quest/f_quest_nm.c)
- 217620    84 flfntSjis2Index: 6 off of 21 (flfnt/flfnt_nm.c)
- 22DE00    76 item_ans_send: 7 off of 19 (aq/aq_nm.c)
- 216080    76 flSndPackLoadBG2: 7 off of 19 (sound/flsnd00_nm.c)
- 22F200    64 CngNetAQBuffEmptyCheck: 14 off of 16 (net/cng_nm.c)
- 22ED90    52 CngNetAQSessionWait: 9 off of 13 (net/cng_nm.c)
- 216050    48 flSndOutputMode: 8 off of 13 (sound/flsnd00_nm.c)

## Lobby overlay tail, 0x5EE618 - 0x610288 (agent D, 10 Oct 2026)
Map (V = village/offline path, O = online-only; from names and callers, not traced at runtime):
- 5EE618-5EFFE0 PNG/BMP texture glue (plPNGSetContextFromImage, BsCreateTexturePixelFromPNG/BMP, flCreate*From*_mem_err): O (browser images).
- 5EFFE0-5F1DB0 lobby info CSV, game style, HTML tag type parsing (parsetag, special_tag_check): O.
- 5F21D8-5F6F50 browser state bodies (MainBsInitialize, BsBody01-06, AppendWork 9.8 KB, CheckHTMLSource): O.
- 5F7430-5FD6xx browser cursor, scrolling, forms (moveCursor, dragScroll, eachObjAction, FormHandler, linkPage): O.
- 5FE800-602430 tagAct_NNN handlers and tag parameter parsers: O.
- 602430-609400 table / text layout (Disp_Text, tagprintf, set_TABLE_*, chack_TableTagClose*, the *_t twins): O.
- 609700-60D6E0 ITEM BOX (Lb_ItemBox_*, itembox_*, kosuu_select, ItemboxWindow*, Disp_lb_item_box): V (also used online).
- 60D710-60E330 plaza chat log: O.
- 60E330-610288 eft25 (effect spawned by the town NPC scripts lbnpc/lbem04/09/10, PC runtime stubs func_60E2B0 = Eft25_set): V (guess from callers).
Start state: 102 functions (80 KB) had no C at all, all browser; m2c + tools/lbauto.py matched none byte-identical.
Linked this session (rebuild OK, all five modules):
- V: item box Lb_ItemBox_open, kosuu_select, sortup_idx_chk, ItemboxWindowCursorX (matched here, but main got the same four from another agent at the same time:
  the merge took main's lb_tu_ib.c; tagAct_602 is lb_gdr2x01 on main, my lb_dd14 was dropped).
- O: BsInit01_LoadWait, BsCountdownTimer, BsBody01_RcvSrc, BsBody03_PrsSrc, BsBody06_WaitCancel1, BsCheckInetProblem, BsInitAllObj, BsCsMove05_CapRegist,
  BsCsMove07_NetError, get_input_tag_sp_type, Disp_TABLE_Line, check_rowspan, check_rowspan2, set_align_data, (src/lobby/f/lb_dd01-13,15.c,
  one registered range each; lb_d01-03 are agent F's files, do not reuse those names).
Near-matches left, V (item box, working copies in src/lobby/f/lb_ib.c / lb_ay.c, not linked; counts are differing lines of tools/align.py):
- Lb_ItemBox_mv 6 (the `lw v1,ib` before the 0x39DAD0 store at the case-1 label, and the lui/sb order in the cancel tail),
  itembox_stock 2 (sh store scheduled after the constant loads in case 1), itembox_equipchange about 26 (only a0/v1 temp names, mask var in v0, store order at +0xABF8),
  itembox_pickup 262, itembox_sortup about 110, itembox_sellout 255, itembox_cursor_mv 40, Disp_lb_item_box 124 (ib pointer in t0 instead of v1), ItemboxWindowX 840.
Near-matches left, O: font_data_off 2 (nop placement), tagAct_600 (the second `bsw[bsw[0xE96C]+0xE96C]` read gets CSE'd into an ori/addu), BsPalCheck, BsDlgMvCsr, CheckAllImages (about 95).
Lessons (each confirmed by a match):
- Chained assignment: `a = b = 0` with b a different width makes the compiler keep the converted zero in a temp (`andi t0,zero,0xFFFF` + `sb t0`):
  `F(s8, ib, 0xB) = F(u16, ib, 8) = 0;` (Lb_ItemBox_open); `F(s16, ib, 8) = F(u8, ib, 0xB) = 0;` (Lb_ItemBox_mv).
- A K&R call `f();` leaves a0.. as they were: m2c's extra arguments (`LoadInnerImage(4, ..)`, `BsSetRenderState(.., bsSys)`, `RetryShadowPost(a, b, c)`) are stale
  registers, drop them (BsInit01_LoadWait, BsBody01_RcvSrc). Same for `se_req(7, 0x2C, 0, tmp)` in the item box.
- `u = User_data` kept in a saved register while the slot index is `lbu; sll 2; addu v0,v0,s0` (index first): write `k = F(u8, ib, 0xB) * 4;` as its OWN statement and
  `*(u16 *)(k + (int)u + 0x37C)`; any single-expression form (`ib[11] * 4 + (int)u`, struct array, comma operator) gives `addu v0,s0,v0` (itembox_stock, kosuu_select).
- `while (a < N) { if (match) break; a += 1; }` gives the rotated loop with the first test duplicated at the bottom; `while (a < N && !match)` does not (itembox_pickup shape).
- Orig lays an inline `return 0` at two places: write both (`if (v == 0) return 0; do {..} while (v != 0); return 0;`), not a shared label (check_rowspan).
- `switch (x) { case 6: case 7: break; case 1: f(); return; }` followed by common code after the switch gives ladder (1,7,6) with body of 1 first (BsBody01_RcvSrc).
  An empty last case that the original ends with `b end` needs `case 2: return;` (not break).
- `daddiu` constant loads: u8 return type and u8 locals (get_input_tag_sp_type); `s1++` on a u16 loop counter avoids the pre-mask that `s1 = s1 + 1` adds (BsInitAllObj).
- `x > 1` instead of `x >= 2` moves the slti result to `at` (tagAct_602); `& 0xFFFFFFFF` on one call argument changed the load order (Disp_TABLE_Line, found by the permuter).
- Declaration order: the cell pointer declared before the s32 it is read from and assigned first (check_rowspan2).
- A static-sized local that the original allocates but the compiler would drop: `SW4 tmp4` / `SW6 tmp6` in the item box keep the 80-byte frame.
- Permuter on raw/registered functions: the asm is not in asm/lobby/text; make a copy with `config/lobby.yaml` where the registered range is turned into an `asm` subsegment
  (see build/lobby_ib.yaml idea: replace `[0x0D5DD0, c, f/lb_tu_ib]` by `[0x0D5DD0, asm, text/ibtu]`, `asm_path: build/asmib/lobby`, run splat) and use PERM_ASM_DIR=build/asmib.

## Game overlay leftovers round (agent D, 11 Oct 2026)
Linked (rebuild OK, all five modules): em21_main, em20_main, em_atk11_0056E160, the hire group of em21 and em08
(hire_move_sub1/sub2/hire_move, 6 functions), em21_target_ang_calc, em01_frame_reset, em01_reset_char_set,
em_dm03_0058D4E0, em_fly03_005B7EE0, em_dmg07_005B9ED0, em_cmd_pl_ride_ck, area_route_rnd32.
Method that paid off: small throwaway scripts that try ONE source mutation at a time and keep it when the real
difference count (tools/alignall.py, not check.py's count which includes shifted code) drops:
- operand-order flips of every comparison (`a >= b` -> `b <= a`): found em_atk11 (`t >= --em->work08`) and em20_main
  (`temp_a2 <= temp_t0 * 0x1E / 100`); hire_move_sub2 needed `0 >= x`-style flips too.
- declaration-order permutations (tools/declbf.py; for 5-6 locals run all 120-720 orders in the background).
Lessons (each confirmed by a match):
- `for (...) { if (match) break; } if (i >= n) { ... }` where the original jumps straight past the if-body on a match:
  write `if (match) goto skip;` with the label at the end of the enclosing block (em21_main). A local that is compared
  against a loaded byte may need `u8` (not int) to get the original register (em21_main `u8 n`).
- `w->x--; if (w->x <= 0)` on an s8/s16 field: the original has the decrement INSIDE the condition: `if (--w->x <= 0)`
  (dsll32/dsra32 on the compared value shows it). hire_move_sub2.
- A two-value `switch (mode) { case 1: ...; case 2: ...; }` that the original compiles as two `bne`: write
  `if (mode == 2) {...} else if (mode == 1) {...}` (order = the original's first test). hire_move_sub1/2.
- Two switch cases `case 2:` / `case 3:` that differ only in a table (original has the body twice, case 2 ends with
  `b end`): write both bodies out instead of sharing (hire_move_sub1). `u16 cnt = s16 field` gives lhu: use
  `s16 cnt0 = field; ... u16 cnt = cnt0;` to get lh + later andi. `if (k != 0 && p->t == 0)` on a u8 k: the original
  materialises `!(k != 0)` (sltu/xori): write `if (k && !p->t)`.
- A function that walks its pointer parameter (`a += 3`, `return a`) must use the parameter itself, not a copy `q`
  (area_route_rnd32); `int cum; cum = (cum + w) & 0xFF;` and `int w; (u8)w == 0xFF` give the original's unmasked adds;
  declaration order of cum/i then fixed the temp registers.
- `if (A) { x } else { y }` with an empty first body is kept by MWCC only when written with an explicit (empty) else:
  `if (v == 2) {} else if (v == 1) {} else {}` (em_cmd_area_move_ck, partly).
- A calling function that passes a u16/s16 parameter: callee prototype `s16` instead of `u16` removes the andi at the
  call (edit_pl_init_new; select Edit_task/Cont_task are already linked in edit04.c).
- check.py/mkruns3 --verify cannot verify a function whose plain name also exists in another overlay (em10 em_act03:
  the compare picks the em01 one); give such statics their address suffix or test with rebuild.sh.
More linked later in this round: em_cmd_boss_atk_ck, em_cmd_before_stage_ck, em_cmd_boss_same_stage_ck, em_cmd_rnd32,
em_cmd_area_move_ck, smell_ck, shell06_move_sub, shell22_h, em_fly08_005F02E0, em_hagitori_lv_up (about 28 functions in all).
More lessons (each confirmed by a match):
- A switch ladder keeps a compare only for case labels that are not adjacent to `default:`. When the original has a
  compare whose target is the next instruction (`beq a0,v0,L; nop; nop; L:`) or a compare the compiler drops, add or
  reorder empty case labels: shell06_move_sub `default: case 0: case 1:`, shell22_h `case 0: case 1: default:`
  (try both orders). An empty `if (v == 2) {} else if (v == 1) {}` is dropped; write the switch instead
  (em_cmd_area_move_ck: `switch (v) { case 1: break; case 2: break; case 0: clr: ... }` with `goto clr` from the
  other tests).
- CMD_SKIP loops that keep their condition in a variable: write the skip loop out as `while (flag) { ... }` and set
  the flag in BOTH arms: `flag = 1; if (b == NULL) { flag = 1; } else { if (cond) break; flag = 1; }` (the first assignment
  is the `andi s0,v1,0xFF` in the branch delay slot, the second a `daddiu`). em_cmd_boss_atk_ck, before_stage_ck,
  boss_same_stage_ck.
- Two table pointers `tbl[em->kind]` loaded before a switch and used in different cases must be written as two locals
  assigned before the switch (em_fly08: `hu = em_hungry_tbl[kind]; th = em_thirst_tbl[kind];`), and the if-arm that the
  original lays out first is the one that is `==` (`if (a == b || b == 0xFF) {A} else {B}` instead of
  `if (a != b) { if (b == 0xFF) goto A; ...} else { A }`).
- Saved-register numbers follow declaration order: a pointer that the original keeps in the LOWEST saved register is
  the one declared LAST (em_hagitori_lv_up: `h` after the arrays, and `flmatCopy(&mat, get_joint_wmat_em(em, tbl[j].joint))`
  reading the joint straight from the table). Permute all declaration orders in a scratch copy (tools/declbf.py or
  a loop over itertools.permutations): em20_material_sub 14 -> 1, smell_ck 21 -> 0.
- `u8 r; r = GetWallHitLine(...); if (!(u8)r)`: a u8 local keeps the second andi (smell_ck); `int` drops it.
- One experiment loop per function works best on a scratch copy of the *_nm.c (src/game/em/zz_x.c, never commit) so
  that a background sweep on the real file cannot overwrite your edit (it did, twice).
Left as near-matches (alignall real differences): em20_act_set 1 (addiu vs daddiu on `kind = 3`; u16 K&R param gives
the daddiu but loses the register), the 12 *_effect_move (4: the original uses v1 for the constant and for a2+1, mine v0;
unchanged by 20 source forms and two permuter runs; with no call after the switch the compiler picks v1, so something
in the original makes it behave as if there were none), em_cmd_range_ck 6, em10_turn_sub 10, em_act_search 9,
em_cdm_act_flag_ck 10, em_eye_search_set 10, em_cmd_rnd32 6, em_cmd_flag_clear 19, cmn_mongon_check_sub (select) about
120: structure now matches the original (index `flt[pos]`, `base = check_mongon` hoisted, pointer p/q loop), the rest
is the inner-loop register choice; disp_color (select) not started (m2c draft, 231 of 293).

## Game overlay leftovers round 2 (agent D, 12 Oct 2026)
Linked (rebuild OK, all five modules): the 12 *_effect_move (em01, 02, 07, 08, 14, 15, 16, 17, 20, 21, 27; em04/09/10/12 were not touched), em20_material_sub.
Cause of the "v1 vs v0" 4-6 off: effect_move was compiled in a file of its own. Compiled in ONE translation unit with the file that holds
its neighbours (em_uvmove as `static` placed before it, the big ef_move_sub before it), the plain 2-arg form
`switch (w->eff) { case 0: w->eff++; break; case 1: ef_move_sub(em, w); break; } emNN_uvmove(em);` matches at once.
em08/em21: the hire_move group also joins the unit, and `*(u8 *)w = e + 1` (em08/21) instead of `w->eff++`/`em->ex[0]`.
So the old em*_uv.c / em*_rNN.c files are gone; each monster's run now spans uvmove .. effect_move (c_files.txt ranges widened).
- em20_material_sub: `p = (s32 *)((u8 *)(type * 0x8C) + (int)tbl)` (operand order of the final addu).
Still near-matches: em20_act_set 1 (daddiu on `kind = 3`; u16 K&R param gives it but loses the register; permuter 7 min no gain),
em_act_search 9 (n/x/r registers in the second loop; every local type and declaration order tried), em_eye_search_set 10, em_cdm_act_flag_ck 10,
em10_turn_sub 10, em09_material_sub 13, em09_effect_move 4, em_cmd_flag_clear 19, em_fly10 (em15/em20) 18. Select overlay not attempted.
The *_effect_move copies left in the *_nm.c files are stale (the linked versions are in the run files).

## Game overlay leftovers round 3 (agent D, 13 Oct 2026)
Linked (rebuild OK, all five modules): em10 as ONE translation unit (em_act03 is the new match; em10_turn_sub too), em08 em_demo00, em17 em_demo00,
em_taisei whole file (Em_Dmg_Sys matched, three runs merged into one em/em_taisei).
Findings (each confirmed by a match):
- em04, em10, em12 effect_move were already linked from the earlier one-TU fix; only em09 is left of that family (see below).
- A static helper that the original inlines into its callers (em10_msg_set into em_act03) must be in the same TU, but a static copy of
  it in a second TU is emitted as an extra function and breaks the layout. So the whole file has to be one TU; a holdout function that
  does not match yet can stay original bytes in the middle of it: config/c_rawfuncs.txt now accepts `game` (base 0x533980), see
  em10_turn_sub in the history of em10.c (it matched later and the raw stub is gone).
- Block-scoped temporaries per switch case change register choice: em08 em_demo00 case 4 matches with `{ s32 tt; u32 sp; u16 aa; u32 dd; ... }`
  declared in the case block (order tt, sp, aa, dd; found by trying all 24 orders).
- em17 em_demo00: when the original frame is 80 bytes bigger and only stack offsets differ, the two uses of one local array/matrix are
  separate locals in the original (ang/a120/out110/m and ang2/vF0/outE0/m2): use two sets, declared in the order of the offsets.
  `if (v >= 0x801 && v < 0xF800) {} else if (X)` is `if (v <= 0x800 || v >= 0xF800) { if (X) ... }`.
- em10_turn_sub: `d = (tgt - (ang & 0xFFFF)) & 0xFFFF; if ((u32)((d + spd) & 0xFFFF) < (u32)(spd * 2))` with declaration order spd, tgt, d, ang
  (the same shape as in em08 em_demo00, tried all 24 orders).
- Em_Dmg_Sys: `r = 0xD; if ((u8)x762 != 3) { body }; goto fin;` is really `if (x762 == 3) { r = 0xD; goto fin; } body...` (the original lays
  the `b fin` before the body); and `HAGI_CNT(i)++` must be `c = HAGI_CNT(i); m = 1 << i; HAGI_CNT(i) = c + 1;` with `hit = 1` first.
- `*(u8 *)0x3F34C3` etc are game_w.pl_num / master / stage; replacing them (em_cmd_nm) did not change any result here.
- The build compiles every src/**/*.c: keep scratch .c files OUT of src (it compiled a zz_ file and failed).
Stale effect_move copies: the copies in the *_nm.c files are the same text as the linked ones for em01/02/04/07/10/12/16/27; the other six (em08, 15, 17, 20, 21)
were synced with the linked text. They cannot be deleted: the PC build (tools/build_pc.sh) links the *_nm.c versions of those functions; check.py shows
them as 6 off only because the nm TU is not the original TU. em14_nm keeps its old 3-argument ef_move_sub call.
Still near-matches (instructions off): em09_effect_move 4 (switch `case 5: case 4:` gives the right size and 4 off, the original has an unfilled
delay slot plus a nop), em09_material_sub 13 (switch x4A in v0 vs a2; declaration orders done), em09_act_set 90, em_act_search 5 (block-scoped second
loop gets n/y/r in the wrong registers), em_eye_search_set 9, em_cdm_act_flag_ck 9, em_cmd_range_ck 6, em_cmd_flag_clear 24, em_fly10 (em15/em20) 16
(copy of w to s0 is scheduled after the load of em->x05), em20_act_set 1, em_range_set 13 (post-increment form), em_mv00_005DC550 50, eft22_end_init 4,
eft18_set_com 7, set05_m 2, em12_main (register allocation shifted by one saved register).

## Game overlay: remaining unmatched functions (agent D, round 4 start, game 93.24%)

Source file: the survey CSV has no source name for these (bind column is only LOCAL/GLOBAL), so the file is the near-match TU that holds the C.
Off = real differing instructions of the C in that TU (tools/alignall.py), blank = no C yet.

| address | bytes | bind | function | off | near-match file |
|---|---|---|---|---|---|
| 0x0062EA20 | 7584 | LOCAL | shell08_m | 240 | shell/shell08_nm.c |
| 0x0054DC30 | 7032 | LOCAL | eft16_m | 10 | eft/eft16_nm.c |
| 0x006309A0 | 6320 | LOCAL | shell08_trans | 561 | shell/shell08_nm.c |
| 0x005412B0 | 5064 | LOCAL | eft04_t | 5 | eft/eft04_nm.c |
| 0x0058E500 | 3808 | LOCAL | ef_move_sub | 76 | em/em04_nm.c |
| 0x00623400 | 3148 | LOCAL | set14_trans | 15 | set/set14_nm.c |
| 0x005F2680 | 2392 | LOCAL | em_atk08 | 39 | em/em20_ai_nm.c |
| 0x005638B0 | 2276 | LOCAL | em_cmd_end_command | 289 | em/em_cmd_nm.c |
| 0x005F3440 | 2200 | LOCAL | em_atk21 | 34 | em/em20_ai_nm.c |
| 0x00534730 | 1956 | GLOBAL | em_neck_move_sub | 642 | em/em_core_nm.c |
| 0x0061FB50 | 1800 | LOCAL | set05_m | 2 | set/set05_nm.c |
| 0x005B3A50 | 1660 | GLOBAL | em12_main | 183 | em/em12_nm.c |
| 0x00534EE0 | 1392 | LOCAL | neck_ang_set | 251 | em/em_core_nm.c |
| 0x00533A00 | 1380 | GLOBAL | em_eye_search_set | 10 | em/em_core_nm.c |
| 0x005395F0 | 1224 | GLOBAL | Em_Master_Change | 158 | em/em_master_nm.c |
| 0x00638500 | 1188 | LOCAL | shell22_i | 11 | shell/shell22_nm.c |
| 0x00543200 | 1160 | LOCAL | eft05_t | 65 | eft/eft05_nm.c |
| 0x00562640 | 1132 | LOCAL | em_cmd_st25_pl_target_sel | 273 | em/em_cmd_nm.c |
| 0x005615C0 | 1052 | LOCAL | em_cmd_samestage_pl_target_sel | 249 | em/em_cmd_nm.c |
| 0x005600C0 | 1012 | LOCAL | em_cmd_ground_area_move | 171 | em/em_cmd_nm.c |
| 0x00632AC0 | 944 | LOCAL | shell08_rgba | 43 | shell/shell08_nm.c |
| 0x00534200 | 936 | GLOBAL | em_char_set | 85 | em/em_core_nm.c |
| 0x00561230 | 900 | LOCAL | em_cmd_all_pl_target_sel | 213 | em/em_cmd_nm.c |
| 0x00625840 | 872 | LOCAL | set17_trans | 73 | set/set17_nm.c |
| 0x00565840 | 856 | GLOBAL | NextStage_No_Set | 114 | em/em_cmd_nm.c |
| 0x0055D7F0 | 812 | LOCAL | em_cmd_escape_area_set | 115 | em/em_cmd_nm.c |
| 0x0055D140 | 700 | LOCAL | em_cmd_angle_ck | 76 | em/em_cmd_nm.c |
| 0x00545F20 | 672 | LOCAL | eft11_i | 165 | eft/eft11_nm.c |
| 0x0055ECE0 | 608 | LOCAL | em_cmd_flag_ck | 101 | em/em_cmd_nm.c |
| 0x005605D0 | 584 | LOCAL | em_cmd_horm_pos_ang_ck | 94 | em/em_cmd_nm.c |
| 0x00556DA0 | 564 | LOCAL | eft22_end_init | 4 | eft/eft22_nm.c |
| 0x00565BA0 | 540 | GLOBAL | NextStage_Dir_Set | 30 | em/em_cmd_nm.c |
| 0x00562BF0 | 520 | LOCAL | em_cmd_dansa_sel | 118 | em/em_cmd_nm.c |
| 0x0055DF60 | 520 | LOCAL | em_cmd_pl_ang_sel | 48 | em/em_cmd_nm.c |
| 0x005F0730 | 516 | LOCAL | em_fly10 | 18 | em/em20_ai_nm.c |
| 0x005C6430 | 500 | LOCAL | em_fly10 | 18 | em/em15_nm.c |
| 0x005ACA60 | 480 | GLOBAL | em09_material_sub | 13 | em/em09_nm.c |
| 0x005636F0 | 448 | LOCAL | em_cmd_range_ck | 6 | em/em_cmd_nm.c |
| 0x005A8210 | 408 | GLOBAL | em09_act_set | 45 | em/em09_nm.c |
| 0x00533F70 | 400 | LOCAL | senko_ck | 29 | em/em_core_nm.c |
| 0x0055E2B0 | 372 | LOCAL | em_cmd_near_pos_ck | 30 | em/em_cmd_nm.c |
| 0x005FD8A0 | 368 | GLOBAL | em20_act_set | 1 | em/em20_nm.c |
| 0x00539490 | 352 | GLOBAL | em_hate_suu_set | 52 | em/em_core_nm.c |
| 0x005DC550 | 348 | LOCAL | em_mv00 | 54 | em/em17_nm.c |
| 0x00558800 | 340 | LOCAL | fish_type_set | 6 | eft/eft23_nm.c |
| 0x005AC940 | 288 | LOCAL | em09_effect_move | 4 | em/em09_nm.c |
| 0x0055C920 | 288 | LOCAL | em_cmd_flag_set | 50 | em/em_cmd_nm.c |
| 0x0055CA40 | 272 | LOCAL | em_cmd_flag_clear | 19 | em/em_cmd_nm.c |
| 0x00566500 | 240 | GLOBAL | Em_Mode_Chg |  |  |
| 0x00626E70 | 232 | GLOBAL | Set20_set | 8 | set/set20_nm.c |
| 0x00565DC0 | 216 | GLOBAL | em_cdm_act_flag_ck | 10 | em/em_cmd_nm.c |
| 0x00536040 | 204 | GLOBAL | em_act_search | 10 | em/em_core_nm.c |
| 0x00639DF0 | 180 | GLOBAL | Pl_poison_add |  |  |
| 0x00536BC0 | 180 | GLOBAL | em_range_set | 23 | em/em_core_nm.c |
| 0x0063BA80 | 160 | LOCAL | print_tuto_message | 8 | tuto/tuto_nm.c |
| 0x00562220 | 128 | LOCAL | em_cmd_ninshiki_timer_sub | 10 | em/em_cmd_nm.c |
| 0x00539C90 | 108 | GLOBAL | Em_Taisei_Set | 26 | em/em_master_nm.c |
| 0x005546E0 | 104 | LOCAL | eft18_set_com | 2 | eft/eft18_nm.c |
| 0x005665F0 | 60 | GLOBAL | em01_local_area_move_init |  |  |
| 0x00639DD0 | 20 | GLOBAL | Pl_piyo_ck |  |  |

## Game overlay round 4 (agent D, 14 Oct 2026)
Game overlay 93.24% -> 95.3% (functions linked, every module rebuild OK; see the commit log). All linked with rebuild.sh printing OK for all five modules.
New C (these had no source at all): Pl_piyo_ck (u8 return, `x7AA >= 0x32`), Pl_poison_add, Em_Mode_Chg + em01_local_area_move_init (em_modechg.c; the loop walks
`player_work[i]` directly, `em->x7EE & (1 << i)` in that order, `s16 mode2` parameter). The PC build's hand-written stand-ins for Pl_piyo_ck / Pl_poison_add were removed.
Linked after real work: eft16 (whole file as ONE TU, eft16_m included), set14 (whole file, set14_trans included), ef_move_sub_0058E500 (em04d.c), em_atk08, em_atk21,
em_eye_search_set + senko_ck (em_core_eye.c), em_hate_suu_set (em_core_hate.c), em_cmd_near_pos_ck (em_cmd_r97.c), em_mv00_005DC550 (em17_mv00.c).
Lessons (each confirmed by a match):
- Spill slots: MWCC gives spilled values stack slots in REVERSE order of creation (declared first = highest address; compiler induction variables are created last, so they
  sit lowest). eft16_m's `i * 5` induction variable sits at the HIGHEST slot in the original, so it is a user variable declared first: `int x5` (spilled) and `int y15`
  (the `i * 5 + 15` one, in a register), both stepped in the loop condition: `i = 0; if (i < n) { y15 = 15; x5 = 0; do { ... } while (x5 += 5, y15 += 5, i++, i < n); }`
  (the order of the two initialisations matters; `continue` still reaches the increments). A for-loop with the init in the header hoists the store above the guard.
- Call operand order: `Em_Calc_angY(...) + e->fov - e->ang` loads e->fov BEFORE the call and keeps it in a saved register; assign the call result to a local first
  (`t = Em_Calc_angY(...); f = e->fov; ... t + f - e->ang`) to load after the call (senko_ck, em_mv00 `temp_v0 = Em_Calc_angY(..) & 0xFFFF; temp_v1 = em->ang[1];`).
- A float temp that the original keeps in $f20 across a call is a variable assigned from the byte directly: `lim = 100.0f * *q;` (em_cmd_near_pos_ck); via a u32 local
  `v = *q; ... 100.0f * v` the multiply is sunk below the call.
- u8 parameter + compound assignment: `u8 type; type += 8;` gives `addiu v0,s3,8; andi s3,v0,0xFF` (raw add, mask after) with `daddiu` constants, while `type = type + 8`
  masks first (em_hate_suu_set). That one also needs `int n` for the player number although the callers' prototype says u8, so it lives in its own TU (em_core_hate.c).
- `u8 ok; if (x == val) ok = 0; else ok = 1;` (not `ok = x != val`) gives the original's branches; `ok` is uninitialised on the kind-5/6/7 paths (em_cmd_flag_ck).
  A u8 counter that the original steps without a mask: put it in the for header `for (i = 0; i < n; i++, kind++)` (em_cmd_flag_set/clear), `kind++` in the body masks each time.
- An EMPTY non-static function defined earlier in the TU is inlined away; the original keeps the call. In em04 the default case of ef_move_sub calls the empty
  move_default_0058E4F0 (0x58E4F0, 8 bytes): declared `static` and defined right before ef_move_sub in one TU (em04d.c, range 0x58E4F0-0x58F3E0) the call is kept
  and the whole function matches (76 off -> 0; the missing call made the compiler place `daddu s0,a0` in the first delay slot). Check this whenever a function's
  PROLOGUE differs and a `default:` / tail calls something trivial.
- m2c drafts: `x > 100.0f` -> `100.0f < x` (c.lt vs c.le), `(s32)x < 0x801` -> `x <= 0x800` (compare result in `at`), `0 >= x` -> `x <= 0` (bgtz),
  `temp_a1 = em->x05; switch (temp_a1) ... em->x05 = temp_a1 + 1` -> `switch (em->x05)`, `em->x05 += 1` (the early load pins a saved register),
  `player_work[i].stg` -> `pl = &player_work[i]; pl->stg` (otherwise the 0x736 offset folds into the address), and m2c dropped a second argument:
  `kyusyu_senkai_ret_005FCBA0(em, w)` (the callee ignores w but the original loads it). em_atk08 39 -> 0, em_atk21 34 -> 0 with these.
- tools: tools/alignall.py takes `CHECK_MODULE=game` so scratch copies can live OUTSIDE src/ (the build compiles every src/**/*.c; a scratch file that disappears mid-build
  breaks it). Hill-climbs used (scripts were in the scratch directory, not committed): declaration order (alignall score), order of top-level statements, ADJACENT STATEMENT
  SWAPS anywhere in the function (set14_trans 15 -> 0: the u/v assignment order in two uv-scroll cases), types of scalar locals, comparison forms against literals.
- Shared header edit: include/em_cmd.h `Stage_data_get(u8)` -> `Stage_data_get(int)` (NextStage_Dir_Set passes a u16 and the original loads it with lhu; every em_cmd file that
  uses it still matches). set14.c/eft16.c replace their *_nm.c; tools/build_pc.sh lists them.
Near-matches (off = instructions) and what is known: see the table at the end of this section. Highlights: em20_act_set 1 and em09_effect_move 4 / eft18_set_com 2 (original leaves
a branch delay slot EMPTY where mine fills it; set05_m 2 and shell22_i have the opposite), em_cmd_flag_set/clear 2 (the original computes `ex = em->ex` after the loop guard),
eft04_t 5 (hoisting of `srl v0,a1,16` into the arms of the u32 conversion diamond; a toy compile with 1000 expression shapes never reproduced it), eft22_end_init 4 (second
35.0f constant in v1, not v0), em_hate_suu_set matches only in its own TU.

### Unmatched game-overlay functions at the end of round 4 (46 functions, 50,088 bytes)

Off = real differing instructions of the C in the whole-file near-match TU (tools/alignall.py); blank = none measured.

| address | bytes | function | off | near-match file |
|---|---|---|---|---|
| 0x0062EA20 | 7584 | shell08_m | 240 | shell/shell08_nm.c |
| 0x006309A0 | 6320 | shell08_trans | 561 | shell/shell08_nm.c |
| 0x005412B0 | 5064 | eft04_t | 5 | eft/eft04_nm.c |
| 0x005638B0 | 2276 | em_cmd_end_command | 289 | em/em_cmd_nm.c |
| 0x00534730 | 1956 | em_neck_move_sub | 642 | em/em_core_nm.c |
| 0x0061FB50 | 1800 | set05_m | 2 | set/set05_nm.c |
| 0x005B3A50 | 1660 | em12_main | 183 | em/em12_nm.c |
| 0x00534EE0 | 1392 | neck_ang_set | 251 | em/em_core_nm.c |
| 0x005395F0 | 1224 | Em_Master_Change | 158 | em/em_master_nm.c |
| 0x00638500 | 1188 | shell22_i | 11 | shell/shell22_nm.c |
| 0x00543200 | 1160 | eft05_t | 65 | eft/eft05_nm.c |
| 0x00562640 | 1132 | em_cmd_st25_pl_target_sel | 273 | em/em_cmd_nm.c |
| 0x005615C0 | 1052 | em_cmd_samestage_pl_target_sel | 249 | em/em_cmd_nm.c |
| 0x005600C0 | 1012 | em_cmd_ground_area_move | 171 | em/em_cmd_nm.c |
| 0x00632AC0 | 944 | shell08_rgba | 43 | shell/shell08_nm.c |
| 0x00534200 | 936 | em_char_set | 85 | em/em_core_nm.c |
| 0x00561230 | 900 | em_cmd_all_pl_target_sel | 213 | em/em_cmd_nm.c |
| 0x00625840 | 872 | set17_trans | 73 | set/set17_nm.c |
| 0x00565840 | 856 | NextStage_No_Set | 113 | em/em_cmd_nm.c |
| 0x0055D7F0 | 812 | em_cmd_escape_area_set | 109 | em/em_cmd_nm.c |
| 0x0055D140 | 700 | em_cmd_angle_ck | 76 | em/em_cmd_nm.c |
| 0x00545F20 | 672 | eft11_i | 165 | eft/eft11_nm.c |
| 0x0055ECE0 | 608 | em_cmd_flag_ck | 6 | em/em_cmd_nm.c |
| 0x005605D0 | 584 | em_cmd_horm_pos_ang_ck | 94 | em/em_cmd_nm.c |
| 0x00556DA0 | 564 | eft22_end_init | 4 | eft/eft22_nm.c |
| 0x00565BA0 | 540 | NextStage_Dir_Set | 30 | em/em_cmd_nm.c |
| 0x00562BF0 | 520 | em_cmd_dansa_sel | 118 | em/em_cmd_nm.c |
| 0x0055DF60 | 520 | em_cmd_pl_ang_sel | 48 | em/em_cmd_nm.c |
| 0x005F0730 | 516 | em_fly10 | 18 | em/em20_ai_nm.c |
| 0x005C6430 | 500 | em_fly10 | 18 | em/em15_nm.c |
| 0x005ACA60 | 480 | em09_material_sub | 13 | em/em09_nm.c |
| 0x005636F0 | 448 | em_cmd_range_ck | 6 | em/em_cmd_nm.c |
| 0x005A8210 | 408 | em09_act_set | 45 | em/em09_nm.c |
| 0x005FD8A0 | 368 | em20_act_set | 1 | em/em20_nm.c |
| 0x00558800 | 340 | fish_type_set | 6 | eft/eft23_nm.c |
| 0x005AC940 | 288 | em09_effect_move | 4 | em/em09_nm.c |
| 0x0055C920 | 288 | em_cmd_flag_set | 2 | em/em_cmd_nm.c |
| 0x0055CA40 | 272 | em_cmd_flag_clear | 2 | em/em_cmd_nm.c |
| 0x00626E70 | 232 | Set20_set | 8 | set/set20_nm.c |
| 0x00565DC0 | 216 | em_cdm_act_flag_ck | 10 | em/em_cmd_nm.c |
| 0x00536040 | 204 | em_act_search | 10 | em/em_core_nm.c |
| 0x00536BC0 | 180 | em_range_set | 13 | em/em_core_nm.c |
| 0x0063BA80 | 160 | print_tuto_message | 8 | tuto/tuto_nm.c |
| 0x00562220 | 128 | em_cmd_ninshiki_timer_sub | 10 | em/em_cmd_nm.c |
| 0x00539C90 | 108 | Em_Taisei_Set | 26 | em/em_master_nm.c |
| 0x005546E0 | 104 | eft18_set_com | 2 | eft/eft18_nm.c |

## Game overlay round 5 (agent D, 6-7 Oct 2026)
Linked: eft23 (ONE TU eft23.c = old eft23.c + eft23b.c + fish_type_set + Fish_set, range 0x557480-0x558A74; remember to carry EVERY function of the old runs into the merged file,
a missing function shows up as a link-size mismatch far away, e.g. "first difference at 0x860" = an address in the data region moved by the missing bytes),
em_fly10 x2 (em20_fly10.c, em15_fly10.c, new run files made with a small cut-one-function-out-of-the-nm-file script), em_cdm_act_flag_ck (em_cmd_r98.c).
Tricks (each confirmed by a match):
- tools/perm.py WORKS and found two of these in 3 minutes each (`python3 tools/perm.py game FUNC src/x_nm.c -j5 --stop-on-zero`, wrapped in `timeout 170`; run several
  functions one after another from a script; do not run tools/rebuild.sh meanwhile, it wipes asm/). The permuter source is noisy (reformatted) and it sometimes "fixes" a
  function by changing semantics (a dead `new_var = 2; new_var = x;` assignment and a changed argument), so read its diff for the IDEA and re-apply by hand.
- fish_type_set / em_act_search: a second u16 accumulator for the pick loop (`sum = 0; ... sum += w; if (r < sum)`) is NOT a new variable: reuse the first accumulator
  (`total = 0;` again). Two accumulators gave swapped argument registers.
- m2c invents arguments: `em20_senkai_target(em, 2)` in em_fly10 was really `em20_senkai_target(em)` (the callee ignores a1; the original passes whatever a1 held). Both
  em_fly10 copies matched at once after the extra argument went. When a function differs only by which register holds a small constant, check for invented arguments.
- em_cmd_range_ck: a float loop limit that the original keeps in $f2 across the loop (`lwc1 $f2` before the loop, loop value in $f1) is a local declared FIRST:
  `f32 lim; ... lim = *(f32 *)&em->x3AC;` before the loop and `if (lim <= v)`.
- em_cdm_act_flag_ck: reuse the first loop's counter `i` for the second search loop (no extra `j`), and end case 1 with `break` instead of `return` (an explicit
  `return;` makes an extra branch to the epilogue).
- A loop test `while (*p != 0 && str[j] != 0)` that the original compiles to `lb; sltu v,zero,c; xori v,1; bnez v,exit; ...; lb d; bnez d,body` is written
  `while (!(!*p || !str[j]))` (cmn_mongon_check_sub 60 -> 36 off; `!(!a || !b)` is the only form I found that makes MWCC materialize `!(c != 0)` with sltu/xori).
- Delay slots: `beq x,zero,L; nop` where mine fills the slot (or the reverse) is NOT fixed by source tricks I tried (pragmas peephole/scheduling, early return, ternary);
  em20_act_set, eft18_set_com, em09_effect_move, set05_m, Set20_set, shell22_i, print_tuto_message stay near-matches.
- Scratch scripts used (in the session scratchpad, not committed): typeall (exhaustive product of scalar local types, scored by alignall), declsub (every order of chosen
  declaration lines), typehill, mkrun_nm (cut one function plus the file preamble out of a *_nm.c into its own run file).

## Game overlay round 6 (agent D, 7 Oct 2026)
Linked: em_cmd_ninshiki_timer_sub (em_cmd_r99.c), em_act_search (em_core_act.c). Game 95.447% -> see commit log; select unchanged (cmn_mongon_check_sub 36 off, disp_color 67 off).
Run-file ranges: END = start + function size (em_cmd_ninshiki_timer_sub 0x562220 + 128 = 0x5622A0); a range 4 bytes short shifts everything after it (game MISMATCH +16).
Tricks (each confirmed by a match):
- em_cmd_ninshiki_timer_sub: `for (i = 0; i < game_w.pl_num; i++) { switch (v) { case 0: if (!(em->x88C & (1 << i))) em->x890[i] = 0; break; } }`.
  The m2c form (a walking `s16 *w` pointer + EM_FIELD(w, s16 *, 0x890), literal 0x3F34C3) was 13 off; indexing the real array field with the loop counter
  gave 3, and keeping the one-case `switch` (not `if (v == 0)`) gave 0. When a draft walks a pointer in step with the counter, try `field[i]`.
- em_act_search: the second pick loop must read the table twice (`while (tbl->rate != 0xFFFF) { sum += tbl->rate; ...`), not through a temp `x = tbl->rate`
  (the temp took the wrong argument register). The first loop (sum of weights) keeps its temp.
- Compare ladders that leave every branch delay slot as a `nop` in the original (Set20_set): putting `default: return;` FIRST in the switch took 8 off to 6
  (the unfilled slots appear); still not 0, `#pragma scheduling off` / `peephole off` / `optimization_level` change nothing.
- `u16 kind` instead of `int kind` turns `kind = 3` into daddiu (what em20_act_set's original has) but then moves the switch temp; no form found for both.
- em_cmd_flag_set/clear: where `ex = em->ex` is written decides where its addiu is scheduled (it follows the source position); the original has it AFTER the jump-table
  `lui` (= compiler-hoisted). Writing `em->ex[0x44]` in the cases removes the register altogether (16 off). Moving the assignment before `kind = *p++` gives 4/3 off.
- em_cmd_flag_ck: original shape of the skip loop is "outer test -> jump into the loop's bottom test, which is also the inner `if (ok)` test"; our CMD_SKIPF and a
  `while (ok)` rewrite both emit a duplicate entry test (6 off). Not solved.
- print_tuto_message: s1/s2 swapped for y and n; no declaration order, loop form or type combination (47 tried) changes it.
- Em_Taisei_Set: all 24 x 24 orders of (pointer loads, stores) with locals tried; never better than the pointer-to-pointer form (26 off).
- tools/perm.py takes `select` as module too (permuter score is noisy: score 1270 for cmn_mongon_check_sub from a base of 1565 gave no usable idea).
- tools/declhill2.py works on edit_nm.c (select) but needs ~2 s per try; run it under timeout in the background.
- alignall.py on a *_nm.c file shows false diffs for functions that are already linked from another file (e.g. emNN_effect_move, Edit_task/Cont_task): the nm TU
  sees the callee as a same-TU static. tools/unmatched.py is the real list.
- cmn_mongon_check_sub 36 -> 27 off by declhill2 (declaration order: tbl, q, j, len, pos, found, n, p, r ...); the rest is the loop-exit layout (`found` tests) and which of p/q/n gets t4-t7.
- Round 7 (coordinator's "bigger functions" pass): em_cmd_nm.c NextStage_Dir_Set 30 -> 11 off (`Stage_data_get((u16)em->x73A)` gives the lhu, and computing dx BEFORE dz).
  game_w.pl_num instead of literal 0x3F34C3 helped only em_cmd_st25_pl_target_sel (273 -> 161) and em_cmd_samestage_pl_target_sel (249 -> 241); tools/greedy_sub.py
  (CHECK_MODULE=game) applies such a substitution one occurrence at a time. declhill2 and a 128-combination type search on em_cmd_all_pl_target_sel (31 off) found nothing;
  the 1000-instruction em_cmd_* selectors are register-allocation bound, not structure bound, so no whole-TU rewrite helped.
- 8 Oct 2026, raptor crests (PC): Velociprey drew the Velocidrome's crest. em16/em13/em30_amh hold both crests as
  materials 4/5; em_material_sub (0x10CEA0) hides one per kind by material alpha 0. Ported the raptor case to
  rt_em_material_hide + GFX_RS_BATCH_HIDE; details in docs/pc.md "Per-kind materials". The other kinds' cases of
  em_material_sub are still not ported (part-break materials).
- 8 Oct 2026, all monster materials (PC): the rest of em_material_sub, em09/em20_material_sub, the per-clay
  EMW+0x4E6 flags and the 0x798 fade, through rt_em_materials + GFX_RS_BATCH_HIDE / BATCH_TEX / FADE_COLOR; test
  em_materials; docs/pc.md "Per-kind materials". Note: clay 1 of the big monsters is the eft09 tail.
- 8 Oct 2026, tail cutting (PC): the cut logic (Em_Dmg_Sys -> em_tail_off_sub -> eft09 tail_off, carving point) was
  already game C; added the drawing: tail tree on nodes 43/43/44 before the cut, the cut tail drawn apart afterwards
  (rt_em_cut_tail, fl_skel_cut_tail, draw_cut_tail). EMW+0x948 bit 0 is "tail cut", not "asleep" (corrected).
  Test tail_cut. em_alpha_clay left alone (alpha-reference scale unchecked).
- 8 Oct 2026, tail cut follow-up: cuttable = part 8 in em_dur_tbl (1, 11, 14, 17, 22, 26); 6/8/15/21 never cut.
  Basarios needs to be awake; real attacks cut it on the PC. Cut tail now gets its materials (bug fixed).
- 9 Oct 2026, owner F8 report "keeps rewind moving and walking into walls" (Aptonoth, quest 131 stage 39): exact replay with
  the report's build a963743f (git archive into a scratch folder, built there) and main reach the same state. Cause: the host
  posed every non-Rathian monster model (em_mdl) without root_lock, so walk loops drew their root travel on top of the game's
  root-motion movement (body up to 390 ahead, ~295 snap back per loop). Fix: root_lock on em_mdl (viewer.c). Aids:
  RT_EM_DRAW_TRACE, RT_EM_ROOT_FREE; RT_EM_TRACE gained `wall 0x74C/0x7EA`. Test: test_activities.py herbivore_rewind.
  Details and the wall / body_hit notes in docs/pc.md ("F8 report ... rewind moving").

## G round 2 (agent D, Sonnet, 13 Oct 2026): game.bin + lobby, then the village
Registered/verified: 45 more shared C files (game+lobby), set08/15-18/22, em_master_b (sleep/paralysis: G's map names Em_Sleep_End/Em_Sleep2_End sit
at swapped call targets, use EM_SLEEP_END/EM_SLEEP2_END from version.h; stage tables 92 not 88; SETW layout swapped pos/move, flag3E at 0x1F).
Raw stand-ins for G: `python3 tools/mhg_rawwrap.py FILE FUNC...` wraps a function that does not match G as original bytes
(config/mhg/c_rawfuncs.txt, build/mhg/raw/*.inc) so the rest of the file can be registered. A bulk run over all game files failed to link in
one go (rodata slot/ordering problems not found yet): the bulk registration list is NOT in the tree; re-add by small batches
with a real `VERSION=mhg tools/rebuild.sh MODULE` per batch (tools/mhg_rawwrap.py + mhg_addc.py).
Lobby files with lb_sys/lbShop addends fail G (lbShop field layout moved, lb_by43/45/48/50/53/61/77/90/91/92, lb_j07/j09/j12/n01/c02/z52, lbshpb).
New tools: mhg_addrmap.py (MH1 data address -> G address votes from twin functions; `--ptr pNet|cw` for pointer structs; `--emit` writes
config/mhg/pc_addr.txt used by tools/pc_abs.py for literal MH1 addresses in G PC builds), mhg_fdiff.py (side by side of an MH1 function and its G twin with
symbolised data, `-g` prints G only with branch targets).
Village (G): lb_sys has G's layout (include/lbsys_g.h: LBSYS_G struct at 0x6548F0; LBSYS_OFS/PNET_OFS/CW_OFS for raw offsets; the G rebuild stays
byte-identical, which checks the map). G copies in lb_village_nm.c: Local_main (exit codes 0x22/0x27), vs_square + init_pre/init_init/init_se/init/exit/event,
Clear_lobby_ram (lb_n.c), Lbc_set_prim / Lbc_init_network_work (lb_cli.c, five prims, pNet 0x38). tools/pc_lobby_g_exclude.txt keeps the MH1 lb_gaf* copies out
of the G PC link. G stand-ins (rt_log_standin): Lb_make_quest_tbl_local (G rebuilt the quest tables, 397 insns), Lb_ItemBox_init/Disp_lb_item_box
(item_box has no G address), pending-gold lobby_func_0055A140, func_0012D6F0. rt_data.c maps unnamed `jr ra; nop` G functions in pointer tables to a host no-op.
Result: hidden new-game run reaches the hunter's house with the hunter drawn (RT_NO_NPC=1 skips the NPCs). Open: village NPCs (em_cmd tables of NPC kinds are
unrelocated: cmd_tbl garbage in em_cmd_init), x68 talk codes are +2 in G from some value on (0x20->0x22, 0x21->0x23: other talk states still MH1's),
Lb_move_common / lb_basic_master / Lb_stage_load(st, 1) G copies not diffed yet, cw per-member records are 0x5C in G (MH1 0x2FC).

## G round 3 (agent D, Sonnet, 13-14 Oct 2026): the village walks on the PC
Hidden G run (`@village1` waits for the village, then pad `idle*1500,down*100,square*3` ...; `RT_NO_NPC=1`): intro event, the hunter gets up, leaves the
house (square at the door), walks all of Kokoto, re-enters the house, opens the item box chest (square next to it: menu draws, tabs work).
Position traces are bit-identical to the MH1 build for the same input (RT_VILLAGE_TRACE=1 RT_QUEST_TRACE=1).
- Talk-mode numbering (lb_sys x68): G has two new modes (3 and 18): G = MH1 for 0..2, +1 for 3..16, +2 from 17 (0x20->0x22 ...). The PC keeps MH1's
  numbers everywhere (all C is MH1's); G copies written from G asm use the MH1 equivalent (Local_main exits 0x20/0x25, event 0x21). Using G numbers
  made lb_check_status run the quest-counter states during the intro (never finished).
- G copies / edits (all under VERSION_MHG): Lb_stage_load(st, arg) (CameraInit at the end, Lb_pl_init(arg), random arrival slot), Lb_pl_init (10 hunters, prim
  only when arg), Lb_move_common (10 hunters, cw+0xB5A countdowns, pl_num count), Lb_put_unique_act_hint (action list from lobby_func_00595530),
  Lb_put_help ("%s%s" with net_etc_mes_tbl[6] as separator, room price via strcat, blink counter lb_sys+0x91/0x92; the rule-sheet frame
  lobby_func_005926C0 not ported), lbc_text_lobby_trans (pNet is a pointer, prims at +0..+0xC; a 5th prim on a new ot not drawn), Lb_ItemBox_init/open/mv.
- main -> lobby pointer words (hint_tbl, ...): G's ELF has no relocations, so config/mhg/pc_main_lobby.txt (tools/mhg_main_lobby.py, from MH1's list by
  symbol name; only 22 of 73 words found: the others have no G name) -> rt_meta_main_lobby -> rt_mem.c. hint_tbl was pointing into game.bin (crash).
- Item box: ib = item_box 0x653330 (G Lb_ItemBox_init = lobby 0x552470, called from main's Pit_init; the old "no init" stand-in is gone). IBO(mh1 offset) in
  lbsys_g.h maps the struct (cursors 8/9/0xA became u16 at 0x10/0x12/0x14). G's User_data is re-laid out: pouch/box 0x1C4 -> +0x588, 0x37C -> +0x8A8, the six
  equipped-slot bytes 0x456.. -> u16 at 0xCFE.. (UDO/UDEQ in lbsys_g.h). NOT ported: G's tidy-up prompt (ib+0x2F, func_0021DFA0 compacts the pouch),
  3-digit quantity editor (itembox_cursor_mv uses User_data+0xC97 bits), skills; the header shows "(null)" (category_name_str has no G address), item tables
  (D_3396D3 stride) and main's Ud_* functions still use MH1's User_data layout, so item counts are wrong until they are ported.
- Still open: Lb_make_quest_tbl_local (G: u16 quest ids, lb_quest_all 16 bytes per level, guildStr; the quest counter UI is rewritten with it, needs the NPCs),
  lb_basic_master (G adds NPC talk branches: get_questLevelNum(46/50/60/80) story checks, new talk modes, NPC_Message, trade; MH1's version drives walking
  fine), Lb_make... CameraWork: G's CAMS has its union at +0x80 (MH1 +0x90; ax/ay moved into the demo struct): the PC runs MH1's camera C on it, which is
  self-consistent; G's NPCZoomInCameraRequest(0) (zoom on the hunter) has no MH1 equivalent.
- Bulk registration retry: 141 files passed check.py, 127 added, bisecting with a game-only rebuild (15 s each) left 107 registered; 34 parked in config/mhg/c_skip.txt.
  G matched 9.3% -> 14.6% (tools/progress.py with VERSION=mhg). Candidate scan: for f in files of config/c_files.txt not in config/mhg/c_files.txt run
  `VERSION=mhg tools/check.py src/MODULE/PATH.c` (all OK) then tools/mhg_addc.py.

## Clean-build fix (agent D, 14 Oct 2026)
Registered G files stub unmatched functions with `#include "NAME.inc"`; the G build looks in build/mhg/raw (config/mhg/c_rawfuncs.txt, G bytes) then
build/raw (MH1 bytes). Commit 8ea67095 had dropped 160 of those entries (the worktree only built because build/mhg/raw kept stale .inc files), and 4 more
(flPS2matMul, flPS2matMul2, PS2SHADER_ADD_LIGHTVECD3, plMemmove: pulled in via src/mhg/*_pN.c wrappers that include ../src/*.c) were never listed.
Restored by finding each stub's bytes inside the file's registered G range. c_rawfuncs now 174 lines. Clean proof: rm -rf build/mhg/raw build/raw, MH1 5 OK,
VERSION=mhg 7 OK (each alone, with `ulimit -v 3000000`; peak child RSS 420 MB, splat 300-420 MB per module).
The 5.6 GB python was not reproduced by rebuild.sh, build.py (also with a missing .inc), splat or the PC builds (all < 450 MB); a bad merge of main.yaml/sub_main.yaml
(registered set21/setwork2 whole AND main's _pN split) makes splat stop with "segments out of order", not loop. Keep the ulimit cap on every build.

## G village status (end of this run)
- Works (hidden runs, build/D/g/): village entry, bed save, intro camera, item shop buy, quest counter (accept -> start hand-off to A's path).
- Weapon/armour/process shops: tags now use G's shop_default_tag_00610A68+1 (GTAG in lb_arm/lb_sh1a/lb_sh1c/lb_by47/49/54/129); crash gone, buy/sell menu shows. Buy list and sell list NOT verified (my pad scripts did not get past the menu; the tools/gvil.sh pad names are ddown/dup, not down).
- Item box "no item" text: `item_str` added to config/mhg/pc_names.txt (0x501AD0), untested (the build was killed by the OOM before I could re-run). Main now has item_list_title for the "(null)" header.
- Open: forge (slot 10), item box tidy/3-digit editor, G lb_basic_master/lb_check_status (elder first meeting needs RT_EVENT_SET=1), cat kitchen, farm, sell, quantity picker, final verification set (clean G rebuild 7 OK, MH1 rebuild 5 OK, test_quest_loop.sh).

## G village round 2 (agent D, Sonnet, 14 Oct 2026): shops, box, forge, elder on the PC
Everything below runs hidden on build/mhg/pc/mhview_g and is checked by `tools/test_gvillage.py` (15 cases, ~10 min: weapon / armour / item shop buy, sell,
quantity picker; forge weapon / armour / upgrade; item box store / take / equip change / tidy / sell; the elder's first talk). Pictures in build/D/g/.
Tools: `tools/gvil_ev.py TAG END_TICK "tick:pad,..." [W=x,z] [ENV=v]` (the timing of tools/test_activities.py works: circle*2 at 45/75/105/135.., a G run needs
`W=` for the warp, RT_MONEY / RT_POUCH / RT_BOX_ITEMS / RT_WARE for the state; GDB=1 for a backtrace under gdb with ASLR on if you add `set disable-randomization off`),
`tools/gpeek.py MODULE ADDR [N] [-b|-h]` (G data words), tools/gvil.sh now ends the log with `EXIT n`.
- Shops: lb_armor_tag_decide01/00 (G's table pick by story level and job, 100/200/300 sell list), tab-name tables (see REVIEW), `shop_select_items` G copy
  (lb_shop_nm.c: 10 x 10 grid pages, keys 0x80 / 0x8 turn pages), armour icons +1 (lb_by125), the item shop's detail panel (lbshop2_nm.c), G ItemboxWindowX
  (lb_ib.c; also draws the sell grid), pouch list (chat_nm.c ItemListWindow: village pouch, sorted preview), Set_userdata / ItemCopy on G offsets (udmisc_nm.c).
- Box: 16-bit cursors (IBCT / IBCURW in lb_ib.c), itembox_cursor_mv with pages, the tidy prompt (key 0x4000 = Select: pouch via main 0x21DFA0, box via lobby
  0x5583B0 = ib_tidy_apply), the box and pouch offsets through UDO everywhere, yes/no / quantity helpers (lb_ay.c) on G's ib layout. "No item" text works
  (item_str). Literal MH1 User_data addresses are mapped by config/mhg/pc_addr_hand.txt (one entry per 0x10 bytes; **delete build/mhg/pctree/build/pc/abs/* and the
  matching .o after changing a pc_addr file: the build does not track it**).
- Forge: G's recipe tables imported at G size (pc_data.txt), the rest was MH1's code.
- Elder: lb_talk_init G copy (rows by story / event flags), ck_talkEvent from G, lb_basic_master G kind dispatch (lb_village_nm.c), story events (mode 47) from G's
  lobby 0x544760, pointer tables for the counter's story tests (hunt_group_tbl, equip_set_tbl); the counter talk reads the NPC kind at +0xE (this port's NPC layout).
- Not done: see docs/REVIEW.md "Not ported" (equipment description windows, skill pages, banner animation). The farm and the cat kitchen are not in G's village.

## G for players + village triage (agent D, 15 Oct 2026)
GB3: `./mh1 linux|windows|xbox DISC` accepts the G disc (tools/disc_files.py game: boot file SLPM_658.69 plus size+CRC32 of the four
files). Linux: tools/build_pc_mhg.sh -> dist/linux/MHG/{mhview_g,play.sh,data/}. Windows: VERSION=mhg tools/build_win.sh (tree
build/winmhg/tree, runs tools/mhg_pcmeta.py first) -> VERSION=mhg tools/package_win.sh (mhgpc-win32-*.zip; play_g.bat/bug_report_g.bat
renamed to play.bat/bug_report.bat; G README). The G program is hidden unless MH1_SHOW=1 (viewer.c); the player scripts export it.
src/pc/install.c is per game now (data folder mhgpc/data, G sizes).
GB6 triage (village/lobby; docs/survey/mhg_cov_g.txt is over-approximate: its marker test misses UDO/IBO/LBSHOP_OFS/UD_WARE_N macros):
- Ported: Lb_put_shopYesNo (G prints message table D_2EC5D8[answer state] with func_0012BDD0; MH1 had two fixed strings).
- Looked at, nothing a player sees: Lb_get_armor_num (G only bounds the loop by the box count, sub_main 0x4C9250), Warehouse_search_space /
  Warehouse_equip_stack (already G-sized through UD_WARE_N), Lb_put_msg_type2 (online chat only).
- Still open, probably noticeable: Lb_equip_set (G rewrote it; used by the box's equip-change), lb_armor_put_itemDetail / equip description
  windows (GN7), Lb_make_quest_tbl (MH1 copy; G's Lb_make_quest_tbl_local is the one called), lb_npc_trans 0.78, DispDialogData 0.70, Disp_lb_item_box 0.71.
  The rest of the lobby list is plaza/online code (lbc_*, Lbs_*, CallBack_*) that offline G does not reach in play.
Windows G (verified from a clean clone, `./mh1 windows disc/mhg`, then `wine cmd /c play.bat data` with MH_ARGS="--shot ..." gave the same
title picture as Linux): two host fixes were needed that the MH1 Windows build also needed (rt_hud_gun.c's RECT clashed with windows.h,
viewer.c lacked a rt_font_set_draw declaration). A hidden G run needs `--play --boot` (plain `--shot` without it crashes in
Em_data_com_adrs_get: no quest loaded; same on Linux). With `--shot` the exe does not look for the data folder: pass it (`data`).

## G village, second pass (agent D, 15 Oct 2026)
Agent C ported the equipment detail windows (src/main/menu/equip_g.c); I checked them in the weapon shop (square key = detail, hidden run). My own work:
Lb_put_shopYesNo (the table was read through a D_ name: use real symbol names, the PC treats D_<hex> as an MH1 address), the elder's fee number (Lb_num_to_str),
the box's equip-change flow (no Lb_equip_set in G), the banner wait. Details and the items left as they are (lb_npc_trans, DispDialogData, Disp_lb_item_box,
with the reasons) are in docs/RELEASE_G.md section 7. Pad names for hidden runs: circle / cross / square (detail) / triangle / ddown / r1; the shop's list is at
about tick 165 of tools/gvil_ev.py with circles every 30 from 45.

## One network loader per game (16 Oct 2026, COORDINATOR.md queue item 0)

Owner's request: starting Monster Hunter G updates G from the laptop and starts it, Monster Hunter the same for MH1;
without the laptop each starts its copy on the disk. Done; details and test results in docs/xbox.md "Network loader" and
"Two network loaders: one per game", decision in docs/DECISIONS.md, dropped behaviour in docs/REVIEW.md.
- Loader (src/pc/xbox/loader): finds its folder from XeImageFileName (new shared helper src/pc/xbox/xbox_path.h, also
  used by xbox_log.c), the game from loader.txt "game=" or the folder name; all paths and requests per game
  (`?game=mh1|mhg`, /mh1.xbe or /mhg.xbe). install_data(dir, game) writes into the loader's own folder, refuses a list
  for the other game and any default.xbe. No network right after a game exit: dashboard, not the game again.
- Game (xbox_log.c): leaving returns to `<folder>\default.xbe` with launch data "MH1X" unless the running XBE is
  default.xbe itself. rt_log.c's Xbox session-log folder is XBOX_GAME_DIR\logs (G wrote into MH1's folder).
- tools/xbox_serve.py: both games at once (GAMES dict), --xbe / --xbe-g (defaults dist/xbox/MH1|MHG/default.xbe),
  --data repeatable, --test-game, logs <time>-<game>.txt; plain /version, /manifest, /data, /test, /log = MH1.
- tools/xbox_upload.sh --loader: builds the loader twice (cxbe title only differs) and installs both folders with
  loader.txt "<ip>:8765" + "game=..". --settings goes to the MH1 folder (XBOX_DIR for another).
- Test: tools/xemu_loader_test.sh (new; scratch disk, test DVD src/pc/xbox/testlaunch), 23/23 checks pass. Lesson:
  in xemu a cold boot from the DVD already reports launch data type 2, so a test launcher can't use XGetLaunchInfo to
  tell "first boot" from "came back"; it deletes its E:\launch.txt instead. "Back to the dashboard" on a disk with no
  dashboard ends at the kernel's error 21 screen (expected).
- The coordinator's running server (old xbox_serve.py) must be restarted after the merge: new loaders get 404 from it
  and start the copy on the disk.
- full_check.sh (16 Oct): all ok except test_ps2_online (agent C's PCSX2 test, merged from agent-C). First it failed on
  `ulimit: error setting limit` (it raises its own cap to 3 GB inside full_check's 1.5 GB cap): test_ps2_online now gets
  the 3 GB cap in full_check like test_ps2_patch. Run alone after that it gets further and stops at "no terms notice"
  (02_net.png black) in PCSX2: not looked into (no PS2 code changed here); for agent C.

## Lockstep referee (agent D, 17-18 Oct 2026)
Task: the automatic referee against the real game (DECISIONS "Lockstep referee"). Tool tools/referee.py, PC side
src/pc/rt/rt_ref.c, write-up docs/referee.md, results docs/RELEASE_G.md section 11 / docs/RELEASE.md section 6,
dropped-behaviour list docs/REVIEW.md "Lockstep referee".
- PS2 control without changing the disc: a 40-instruction hook written into free EE RAM (0xFF000, checked zero) and the
  jal flPADGetALL in ACRMain pointed at it; it blocks each frame (once game_w mode 2 is seen) until the host's "go"
  word, and copies the host's pad words into flpad_adr[0]. A second hook replaces ran_suu's first two instructions
  with a jump to a logger (ring at 0xFD000: return address, channel). PCSX2's recompiler picked both changes up
  without a state reload. PINE batches (several ops in one message; pine.py read_many / write_bytes) read 30 KB of
  state per tick; ~30 play ticks a second.
- Line-up: "play ticks" (game_w mode 2, step <= 1), because area loads take dozens of PS2 ticks and one PC tick.
  Same start: the PS2's User_data and Rnd_w at play tick 0 go to the PC (RT_REF_UD / RT_REF_RND); the PS2's Rnd_w
  at every play tick too (RT_REF_RNDS), so a random-call difference stays in its tick.
- Lessons: (1) MH1/G attack with the right stick (cam_u etc.), circle/triangle in the field do other things, and no
  weapon is drawn in the base camp: weapon scenarios change area first (pokes like an area exit, g_ref_shot.sh's way).
  (2) G quest 10001's camp has the Elder's tutorial on triangle ("△ PUSH"): the PS2 masks all buttons while it is
  open (Cockpit_menu_chk in sw_set_sub), the PC has no tutorial window. (3) gdb hardware watchpoints on em_work /
  player_work fields in the PC run, with the referee's env (pad, pokes, RNG), find the code that moves a value in one
  run (build/referee/<s>/gdb*.cmd). (4) `nm` symbols of the PIE PC binary name ran_suu callers from the offset to
  ran_suu itself. (5) The MH1 save of test_quest_loop.sh stands at the village gate; MH1 menus differ from G's.
- Fixed: R-F1..R-F7 in RELEASE_G section 11 (ran_suu(1) per frame, stick rounding, monster order, game_core order,
  game13 tick, em_dur rows noptr (G), G's monster body table). Not fixed, ranked there: root motion a tick late on the
  PC (first difference in every fight, both games), PS2 float rounding (RT_FPU_PS2, owner's call), the Elder's
  tutorial window, a Dual Swords walk-start difference, the sound layer's random draws.

## Referee round 2 (agent D, 18-20 Oct 2026)
Owner decisions of 18 Oct. Details and numbers: docs/RELEASE_G.md section 11 R-F8..R-F12, docs/referee.md (results of
20 Oct), docs/REVIEW.md "Referee, second round".
- PS2 float rounding on by default (rt_ref.c fpu_ps2; RT_FPU_PS2=0 off). DAZ only where MXCSR_MASK has it (Pentium
  III: none, ldmxcsr would fault). Host x87 code unaffected. Checked: A/B pictures and sound dumps, G reference shots,
  full_check (35 ok), Windows exe under wine, Xbox MH1 + G in xemu (mxcsr FFE0 at boot, kept at the quest start;
  FFF1 there is only sticky exception flags, which the check ignores).
- Lesson: "root motion a tick late" was the joints. move() builds a monster's joints right after it moves
  (enemy_mk) and the hunters' after body_hit (player_mk); find such things with a gdb watchpoint on the moved field
  in the referee's PC run (env as tools/referee.py run_pc builds it).
- Elder's tutorial ported (tuto.c + tutob.c + tuto_nm.c into the PC build, tables.txt, G texts in pc_data.txt; G's
  texture slots 0x141/0x142 are the PC's 0x11A/0x11B because the PC keeps MH1's slot numbers). Works on PC and Xbox
  (xemu shots of the lesson list in G 10001 and MH1 131).
- G Dual Swords walk-start blend (f_pl.c). pc_noptr.txt sweep (94 words; G forge cases of tools/test_gvillage.py OK).
- --help / hidden-run install dialog fix (viewer.c; test_log.sh checks it).
- Debug aid: REF_NODES=SLOT:COUNT:FIRST-LAST tools/referee.py run S writes the PS2's (ps2_nodes.txt) and the PC's
  (pc_nodes.txt, RT_REF_NODES) joint matrices of one monster; `tools/referee.py nodes DIR` compares them.
- Note: rt_log's "quest N START" prints the quest as a byte: G's 10001 shows as 17.
Next: N-R7 (Kut-Ku body push) with REF_NODES, then N-R8 (Velociprey drift) and the MH1 fights.

## Referee round 3 (agent D, 20 Oct 2026)
R-F13..R-F17 in docs/RELEASE_G.md section 11 (wall pass, CameraMove order, VU0 sin/cos/curves + rt_vu.h nearest
rounding, newlib atan2f, hunter joints without hunter_yoff). Lessons: the PS2 side is PCSX2, where VU0 results round to
nearest and FPU results toward zero (the sideways drift of a hunter walking at facing 1 needs cos(~0) = 1 + 1 ulp, i.e.
nearest); PS2 `run`s of a scenario are not always identical (another random start), so compare PC changes with
`referee.py pc` against one trace; `exact.py`-style per-field checks with a tiny tolerance find the first float-step
divergence long before the report's tolerance does. Next: N-R7 (first float steps at root-motion starts, e.g. bowgun
em0 tick 77: z one step apart after a blend start).

## Referee round 4: G monsters, items, gathering (agent D, 20-21 Oct 2026)
RELEASE_G section 11 R-F18..R-F22 and the scenario table there. Lessons: (1) byte-diff a monster's EMW between the two
traces (offsets < 0x9F4; skip pointer words) to find the first field that differs: it found the hunger countdown, the
size scale and act speed. (2) A garbage number (HP 32648, item 0xF645) in a G table read through a pointer list = a
pc_ptrs.txt false pointer: look for the row's words in pc_ptrs and add them to config/mhg/pc_noptr.txt. (3) Joint
differences that start on a blend tick = the blend method (REF_NODES). (4) The first actions of monsters are set
during the area load, before the PC has the PS2's random state: the referee now copies EMW+0x39A at play tick 0.
Open: N-R10 Gravios turn end, N-R11 kind 13 first act (G em16 act set), N-R12 village day not comparable (hook only in
quests), g_plesioth / g_kind37 PS2 runs failed to start / stopped.

## Referee round 5: story scenes, village day, carving (agent D, 21-22 Oct 2026)
RELEASE_G R-F23..R-F25 (fifth round block). Lessons: (1) a `_nm.c` file may be a weak copy: check which file the
binary really uses (`gdb -batch -ex "info line FUNC" build/mhg/pc/mhview_g`) before editing; em04_main lives in
em04b.c. (2) When a monster is put into a G scene (act 6/N), its act 6 code must be G's as well: MH1's waits for MH1's
demo flags and keeps the monster in the scene for the whole quest (quest 10046's Khezu; test_all_quests_g found it).
(3) G's EMW keeps MH1's offsets up to about 0x934; MH1 0x9E1 is G 0x9D9 (the PC keeps MH1's layout, so G code that
writes 0x9D9 writes x9E1 on the PC). (4) Village lockstep: scenario `village=True` (referee.md).
Open: N-R10 Gravios, N-R13 g_plesioth start, N-R14 kind 19 fly height, N-R12 reward screen; g_scene_rath's Velociprey
turns the other way at tick 1151 (the hunter sits almost behind it: a 1-ulp position step decides the side, N-R7 class).

### Round 5, second half (22 Oct 2026)
RELEASE_G R-F26 / R-F27. Merged main (agent A's em01 / em16 G ports replace my scene parts there). Next round: emulator
work when memory allows (g_vil_sell, mh1_vil_* scenarios, item box, combine, reward screen), N-R10 Gravios.

## GB6, eighth pass (agent D, Opus, 23 Oct 2026)
Assignment: functions the G PC enters in play that still run MH1 C where G differs (docs/survey/mhg_cov_g.txt). Details, evidence tags and the
list of what was looked at: docs/RELEASE_G.md section 15.
- Method that worked: `python3 tools/gdraft.py -m MODULE FUNC` (G's m2c; MODULE game / sub_main / main / lobby; names as in config/mhg/symbols, or
  `game_func_00XXXXXX`) for the G side; `python3 tools/draft.py main|game FUNC` for MH1 only where the MH1 asm still exists (a function matched in
  MH1 has no asm/ dump: read the PC C instead); normalise both (temps, D_ names, addresses) and `diff`; `tools/mhg_fdiff.py MODULE FUNC [-g]` for
  the asm when m2c drops call arguments (it dropped the last argument of Eft17_type3_set / Em_se_req2: read a4 / t0 from the asm). A G data table the
  code reads: `gread.py`-style read of build/mhg/split/MODULE.bin at the G address (vram from config/mhg/MODULE.yaml), then a pc_data.txt line.
- The cov list is over-approximate (see section 15). A better first filter than the marker test: look for G-only data a function reads (a table
  address that pc_data.txt does not have, a G kind number above 34) and for kind lists in the MH1 C (G's subspecies are kinds 35-50: a
  `case 1: case 11: ...` list in MH1 code means nothing is done for them).
- Pitfall: a state-in-place rewrite. G's shell08_m keeps the Gravios beam's state in the work block (w[2]); the PC keeps it in sh->x05 (SH08W's layout is
  shared with the PS2 builds, so no field can be added at 0x02). Same behaviour, different place; the type 4 `all` is x06 + 31 and w->x36 holds x06 (G w[0x3A]).
- Pitfall: a temporary spawn hack found a crash that was only the test's (type 13 with x07 = 3: attack row 37 does not exist). Force with the arguments
  G's callers really pass (x07 0).
- Tools left: build/D/e13/run.sh TIME OUT.png (hidden Dual Swords run, shot at a game time; BIN=other binary for before / after).
- Not done / next: eft04 (G has a type 9 and 60-byte pieces; one G caller, em15 uvmove), eft02_t, eft15_m, eft16_m (G drops the neck types, adds 15),
  set14_m / set14_trans, em02 Fatalis, em14_main, the combine list count (Item_preparation_list_num).
- Results (23 Oct 2026, final tree): tools/rebuild.sh 5 OK; VERSION=mhg tools/rebuild.sh 7 OK; tools/test_all_quests_g.sh 98 quests + carts + Dual Swords all PASS;
  tools/full_check.sh EXIT 0 (every test ok incl. both Xbox builds, 4 emulator tests skipped by design).

## GB6, ninth pass (agent D, Opus, 23 Oct 2026)
Assignment: the eighth pass's "not looked at" list (em02, em14_main, eft04_i / _m, eft15_m type 9, eft16_m / type 15, eft02_t, set14_m /
set14_trans). Write-up with evidence tags: docs/RELEASE_G.md section 16.
- Method additions:
  - A per-case call-list diff for the animation scripts (ef_move_sub): extract `fn(args)` per `case N:` from the PC's C and from G's m2c
    (hex float arguments converted to floats, the argument ORDER differs: m2c prints the PS2's f12 first, so compare as sets or ignore
    em_frame_check / Eft13_set_em_scl / Eft15_set3 argument order) and diff the lists; it found em15's 0x459 second script and the
    deleted 0x417 shot fan in one run (the throwaway script is in the scratchpad of the session; it is 40 lines, rewrite it).
  - A whole-module objdump of G (`objdump -D -b binary -m mips:5900 --adjust-vma=VRAM build/mhg/split/MODULE.bin`) to grep callers of a
    G function with the immediate argument (`jal 0xADDR` then the `li a1,N` before it): found who passes eft04 type 9 / eft16 types.
  - Forced spawn without touching the game's flow: a TEMPORARY env hook (RT_TESTEFT=SYS:TYPE) at the top of em15's ef_move_sub that
    calls Eft04_set / Eft15_set2 / Eft16_set_ex3 every 90 ticks, run through quest 41 (kind 15 on stage 18, RT_QUEST_STAGE=1,
    RT_PL_TARGET=k15, RT_PL_WARP_EM=100-90000, shots at 4.55-4.7 s so the hunter stands at the monster). Removed again (git checkout).
    Shots: build/D/te/keep/ (eft15 type 9 = blue lightning, eft16 type 15 = white puff, baseline).
- Pitfalls:
  - fdiff aligns by mnemonic: a block that moved shows as deleted + inserted, so never read a `!` run as "G added this" without
    checking the other side (em15's 0x41E x14 test, the Eft04_set_time divide were both "moves").
  - The m2c of an unknown-signature call (func_00101FE0 / func_00102020, G's two stubs in main that scale a vector by 1.0) looks like
    new logic; read the callee first (here a 1.0 constant: no-op, left out).
  - PS2 `Eft04_set_time(em, arg, time, scale)`: G's callers pass the RAW time and the function divides by 2 x act speed; the PC's callers
    (em03.c, em33.c, em15_nm.c) still divide. The G branch only applies the clamp (>200 or act speed < 0.001: 100), which is equivalent.
  - **game_w.area_mdlw is never filled on the PC** (all NULL): eft04, eft01 shadow arg 4, eft20 / shell08 clay pieces draw nothing in
    both games. Found while trying to take a shot of eft04 type 9; not fixed (platform task, see RELEASE_G section 16 "Found").
- Done: eft04 (type 9, 60-byte pieces, size / scale rules, time clamp), em15 ef_move_sub (0x417, 0x451, 0x459 subspecies roar), eft15
  type 9 + sounds + 0x6C, eft16 type 15 + dark blood + 0x6C, eft02 dark blood, set14 stage 90, em12 neck spray gone. em14_main was already
  complete.
- Not done: **em02** (kind 2 / 36): seven new G functions, acts 11-18, ~14 edited functions; no offline quest spawns it, nothing to verify;
  list of the pieces in RELEASE_G section 16. Next candidates from section 15's list: em03_init / main, em16_init2.
- Results (23 Oct 2026, partial: coordinator asked to stop): tools/test_all_quests_g.sh 98 quests + carts + Dual Swords all PASS; tools/rebuild.sh 5 OK;
  VERSION=mhg tools/rebuild.sh 7 OK; tools/full_check.sh stopped early on request: 25 steps ok (rebuild, builds, kr_returns, Xbox online, PC tests up to
  test_online), no FAIL, the rest not run (coordinator runs it at merge). Status: PARTIAL (em02 not started, by evidence; see RELEASE_G section 16).

## area_mdlw platform fix (agent D, Opus, 24 Oct 2026)
Assignment: game_w.area_mdlw[] never filled on the PC. Write-up: docs/RELEASE_G.md section 16, "Fixed (24 Oct 2026)".
- Where the PS2 fills it: em_create_model (crmdl05.c, matched): GWM.sub_ptr[slot] at game_w+0xA8 (G 0xB0) = the kind's sub model
  (em_sub_model_data / ENEMY_SUB_TEX). The name area_mdlw came from the readers (Em_area_ck = model slot of a kind). Lesson: when a
  game_w pointer is never set on the PC, grep the matched C for a second struct view of game_w (crmdl05's GWM) before the asm.
- PC: viewer.c em_sub_model + model_bind (shared with load_eft_models), rt_eft.c rt_bind_area_model, rt_game.c rt_rebind_clay.
- Test aids used: a temporary `RT_E04TR` print in Eft04_set* (removed) gave the ticks (quest 41: kind 15's eft04 type 0 at tick
  144, types 1 / 2 at 874-938); build/D/am/run.sh BIN DISC QUEST KIND TIME OUT.png for hidden before / after shots;
  RT_QEM_KIND=30:33 puts a Kirin into MH1 quest 41.

## Red line at the top-left (agent D, Opus, 24 Oct 2026)
Hidden shots of MH1 quest 41 with a Kirin (RT_QEM_KIND=30:33) showed a red horizontal line across the top-left, through the life gauge.
- Cause (RT_2D_TRACE showed one box `-26291,31-80,35`; gdb watch on PLW+0x790): a host test aid, not the game and not the renderer. The
  god mode (RT_PL_GOD / debug menu "can't faint", rt_player.c) sets the hunter's vital to 30000 before pl_move; a hit makes Pl_damage_sub
  set vital_red (PLW+0x790, the red part of the life bar) to about 15000, and bar_disp's s16 width (0.9 * 288 * red / 100) wraps negative:
  a 4 px bar from x=80 running 26000 px left. Only the red bar; a no-god run never has it. Without the Kirin the hunter takes no hit in
  the shot, so the line was absent (not Kirin specific: any hit under god mode did it, MH1 and G share rt_player.c).
- Fix: rt_player.c's after-pl_move god block now also sets vital_red to the maximum, with the vital. Checked: before / after hidden shots
  (the line is gone, rest identical), PC builds of MH1 and G compile. The real game's HUD code is untouched. Test aid / debug menu only,
  nothing player-visible in normal play, so no RELEASE.md entry.

## Xbox fog vs PS2 (agent D, Opus, 25 Oct 2026)
Owner: the Xbox fog is too thick in Forest and Hills (the first quest, deliver 2 Raw Meat: G 10001, MH1 131; camp = stage 21,
area 1 = stage 39). Write-up: docs/RELEASE_G.md section 17; pictures in build/D/fog/ (gitignored).
- Cause 1 (the owner's bug, both games, Xbox and the PC in a window): **a stage's fog never reached the renderer after an area
  change.** game_flip (viewer.c) records the draws of a load tick from its first flip (gfx_rec.c) and flip_tick_end drops that list
  unshown; stage_fog_set runs inside the load, after a flip, so its colour / start / end (fl states 0x0F-0x11) were only recorded and
  thrown away. The backend kept the camp's fog (4F4F49, 0-9000) in area 1 (506060, 0-90000): the far cliffs a flat 4F4F49 grey, the
  grass 10-30% fogged. Headless --shot runs return early in game_flip (no recording), so every PC comparison picture was right.
  Fix: gfx_rec_state records the three fog states AND returns 0 (the backend applies them now). Shown: RT_FLIP_SHOT makes a headless PC
  run record like a window: before / after build/D/fog/q/pcwin_before_after_39.png; the Xbox log's new "nv2a fog:" line showed only
  the camp's values before the fix.
- Cause 2 (latent, Xbox only): COLOR1.a carries the fog factor (vs.vs.cg / skin.vs.cg), and the NV2A forces the specular alpha to 1
  unless SET_LIGHT_CONTROL has ALPHA_FROM_MATERIAL_SPECULAR (xemu since commit 7a34eedd6f, from hardware tests; CSV0_C bit 17).
  Nothing set it: the result depended on the register's power-on value. With RT_NV2A_LC=0 xemu draws the whole screen in the fog colour,
  HUD included (build/D/fog/cmp4_s21b.png, bottom left). Now set explicitly (ALPHA_FROM_MATERIAL_SPECULAR | SEPARATE_SPECULAR). Not the
  owner's bug (his HUD was not grey), but a real-hardware risk removed. [xemu-vs-hardware doubt: the power-on value on a real NV2A is
  unknown; with the explicit value it no longer matters.]
- The fog maths itself (factor = saturate((w - start) / (end - start)), w = clip w = eye depth, lerp in the pixel shader) gives the same
  picture as GL's linear eye fog: xemu --view shots of stage 21 match the PC within ~5 levels everywhere (near ground too, so not fog).
- Test aids added: RT_FIXED_CAM=1 (with --view / --cam: keep that camera; the Xbox always plays, so its host camera followed the hunter),
  RT_NV2A_LC=hex (the LIGHT_CONTROL value), RT_FOG_TRACE now also prints what each backend got ("gl fog: ..." / "nv2a fog: ...") and
  the rt_fl lines reach the Xbox debug log (rt_fog_trace; stderr on the Xbox is the HDD log file only). Note rt_fl.c had no rt_plat.h:
  `#ifdef XBOX` is only defined for the SDL-side files; use MH1_XBOX from rt_plat.h in rt/.
- How the shots were taken: the Xbox with RT_ARGS="--stage N --view ..." + RT_HOST_CAM=1 RT_FIXED_CAM=1, or RT_ARGS="--quest 10001
  --input idle*2000" + RT_AREA_GO=100,N RT_NOMOVIE=1 (the same as g_ref_shot.sh's PCQUEST runs on the PC); xemu wrapped in
  flock xemu.lock + unshare (empty /dev/input). Script: a copy of the wrapper is not kept (scratch); the env files are in build/D/fog/.
- Open: tools/g_ref_shot.sh's stage-39 PS2 camera (13190,-606,9735) does not give the same view on the PC (PS2 sees the river, PC the
  cliffs): the lpView read may be mid-move after the area change; the 21 views match. The PS2 area-1 picture is still useful for the
  look (cliffs clear, light haze).

## Renderer score (agent D, Opus, 25-26 Oct 2026)
Owner (for fun, but solid and rerunnable): how close do our renderers draw the PS2's picture? `tools/render_score.sh`
(+ render_score.py, stdlib + ffmpeg), scenes in `config/render_scenes.json`, scoreboard `docs/render_score.md` (numbers
only), pictures in build/D/score/<date>/ (sbs_<id>.png = PS2 | PC | Xbox, heat_<id>.png = |PC-PS2| | |Xbox-PS2|, never
committed). Not in full_check (emulators).
- **Determinism = the referee's lockstep**, not frozen RNG / forced cameras: the PS2 (PCSX2 software renderer, hidden)
  is stopped at the start of each tick by referee.py's hook; the PC and the Xbox play the same play ticks with the PS2's
  User_data, Rnd_w per tick, pad and pokes (area changes as exits do), so hunter, monsters, camera and random numbers
  are the PS2's own. The picture: PS2 grabbed while held at play tick N; PC RT_SHOTS at the matching game tick; Xbox
  RT_HOLD at that tick + an x11grab of xemu's display (the Xbox's referee inputs go on the data ISO: run_xemu.sh
  --file, paths D:/data/...). All scaled to 640x448 (ffmpeg area), PC --no-ini (no widescreen / MSAA / options).
  Every platform plays every scene twice; self-difference now >= 99.4 SSIM everywhere.
- Metrics: SSIM (Wang 2004, luma, 8x8 windows every 4 px, windows touching the mask dropped), mean |diff| per pixel
  (0-255), % pixels with a channel off by > 16. Overall = mean SSIM. Neighbour ticks (N-1 / N+1) are scored too: no
  scene is better a tick off, so the PS2/port tick mapping is right.
- Fixed on the way (all verified by the self-difference / the pictures):
  - MH1's held PS2 frame depended on the interlace field: ticks released at random points gave self-difference SSIM
    83-93 in 4 scenes (fine differences everywhere, HUD too, no shift; same game state and camera, lpView compared).
    Now each tick is released right after an even vertical blank (adxm_vsync_cnt, MH1 0x312F70, G 0x26E9B0): 100.
    (PCSX2's deinterlace mode alone did not help; run_pcsx2.sh PCSX2_DEINTERLACE is kept, render_score uses blend.)
  - referee.py's lock_path pointed at <main checkout>/xemu.lock, not the shared "MH XBOX/xemu.lock" that
    g_ref_shot.sh, the PS2 tests and `flock` users take: the referee could run next to another emulator. Fixed.
  - rt_ref.c read RT_REF_POKE with fscanf("%lf"): pdclib's fscanf converts no floats, so on the Xbox no poke ever
    loaded (no area change). Now fgets + sscanf (the Xbox's own float sscanf).
  - N-R6 (referee path): with RT_REF_UD the PC/Xbox hunter now gets Set_userdata + armor_create_model (the save's name
    and look) instead of the default armour.
  - PCSX2's start-up toast ("Compatibility Patches are not enabled") was in a picture: 15 s wait after each start;
    "start" positions are read after the save state is loaded (a read on a fresh PCSX2 gave 0,0).
  - The emulator lock is taken once per platform phase (flock is not fair: other agents' tests took it between scenes).
- First scoreboard (current main + these fixes): G PC 55.9 %, Xbox (xemu) 59.2 %; MH1 PC 46.6 %, Xbox 50.1 %.
  Worst scenes, causes (guesses marked):
  - g_desert / m_desert (30-33): the far sand: the ports draw it sharp and noisy, the PS2 blurred (mip level / LOD
    choice: GS K/L LOD vs GL's derivative LOD) [guess from a crop, not traced].
  - g_village (36-39): the village camera is ~5-6 px off (shift search) and village NPCs stand elsewhere (the lockstep
    does not compare the village camera or NPCs) [measured shift; cause not traced].
  - m_forest1 (38-39): an Aptonoth's body fills the view; it is darker on the PS2 (actor lighting) [guess].
  - g_camp / m_camp / menus (45-56): dense foliage at the PS2's sharper, higher-contrast texture look; the hunter's skin
    lighter on the ports (the open "hunter brightness" item, RELEASE_G section 10) [seen in the pictures].
  - The Xbox scores ~3 points above the PC everywhere [guess: xemu draws 1280x960, scaled down = supersampled, softer
    like the PS2; the PC draws 640x480 directly].
- SSIM punishes texture detail hard: scenes that look the same to the eye score 45-75. Use it to compare runs (before /
  after a renderer change), not as an absolute "percent the same".
- Hook for a real Xbox: put capture-card frames as DIR/<game>/<id>/hw_r1.png (hw_r2.png) and run `render_score.py score
  DIR`: scored as a fourth platform. A pad-script to drive the console to the same tick is not there yet.
- Cost: one full run ~1.5 h of emulator time (PS2 ~10 s / scene, Xbox ~60 s / scene / run), more while other agents
  hold the lock.

## Renderer score: fixes (agent D, Opus, 27 Oct 2026)
Assignment (COORDINATOR.md): raise the score where it lost most (desert, hunter skin), faithful to the PS2.
- **No mip-maps on the PS2** [checked, both ways]: flPS2SetTextureRegister (src/main/fl/flsettex01.c) only writes a mip
  TEX1 (MXL, L, K, MMIN 2-5) when flSystemRenderOperation & 0xC00000 (fl state 0x6B) is set and the texture has more than
  one level. Nothing in main / game / G sets 0x69 / 0x6A / 0x6B (grep of every `addiu $4, $0, 0x6x` before a
  flSetRenderState call); PCSX2 at the g_camp / g_desert shots: RenderOperation 0x0120B032 (& 0xC00000 = 0), K = L = 0,
  every live flTexture has 1 level; the stage 53 APX files have 1 level (RT_APX_TRACE=1). So the GS samples level 0 with
  bilinear, exactly GL_LINEAR / the nv2a's linear filter: no LOD change to make.
- **What made the PS2 soft is its output, and the first scoreboard's capture of it** [checked]:
  - flWidth = 512, flHeight = 448 (both games, every scene; render_score writes ps2_r1_video.txt): the 3D and the 2D are
    drawn into a 512-wide frame buffer, shown 4:3 on the TV (DISPLAY MAGH 4).
  - flPS2SwapDBuff video mode 1, merge on (flPs2State+4 = 1, +8 = 0): PMODE 0x7F27, circuit 1 one line below circuit 2,
    50 %: each TV line = mean of its frame line and the one above (PCSX2 screenshot with anti-blur off: edge rows 210 /
    132 / 54).
  - The old capture added PCSX2's own softening on top: blend deinterlacing and its 640x480 4:3 window (512x448 scaled to
    640x480 bilinear, then back to 640x448 by the scorer). The old PS2 picture against the exact one: SSIM 89.8 (g_desert).
- Fixes: (1) render_score's PS2 side is the exact output now (PCSX2 screenshot at the internal resolution, not aspect
  corrected, F8 hotkey via xkey; deinterlace off, anti-blur off; PCSX2_GS_EXTRA / PCSX2_INI_EXTRA in run_pcsx2.sh).
  (2) `--ps2-output` / `ps2_output = 1` in both ports (docs/pc.md "PS2 output"): 512x448 frame, the merge, bilinear to
  4:3. GL: the back buffer's corner + glCopyTexSubImage2D; nv2a: pbkit's extra buffer 0 (only allocated with the option)
  sampled as a linear X8R8G8B8 texture. Player default off; render_score runs the ports with it (`--port-args ''` for
  the full-resolution default).
- First check, g_desert against the exact PS2 picture: PC full-res 27.8 -> PS2 output 32.7; Xbox full-res 28.9 -> 32.4
  (xemu, the new build) [render_score, build/D/score/t1, t2]. The Xbox matches the PC within 0.3 with the option
  (without it the Xbox was ~3 points above the PC: xemu draws 1280x960, a supersampled picture).
  What is left in the desert is the sand's fine noise: same texture and camera (low-pass alignment best at 0,0), but
  an 8x8 SSIM window of uncorrelated noise scores near 0 whatever the renderer does.
- Seen on the way: the current Xbox build does not draw G's "村長の教えが追加されました" banner in g_desert (also without
  --ps2-output; the 25 Oct Xbox capture had it, the PC has it) [not looked into: Xbox only, appeared between 25 and 27 Oct].
- **Hunter skin (open, findings)**:
  - The PS2 actors use Vu1Code_0016_0004 (skinned) / 0016_0002 (rigid) (live flPS2Clay param tables in PCSX2, m_camp /
    m_forest1). Its lighting: colour = min(128, sum_k max(0, n . Lk) * min(128, 128 * a_k * B) + (mem15 + mem0) * A),
    GS modulate /128; the vertex colours are not read (they are 255 in the hunter anyway); fog in the same program.
  - The actor lights in PCSX2 (light_work set 1, flLIGHT, the hunter's eased colours at PLW+0x578) equal what the PC's
    RT_LIGHT_GAME path computes (m_camp: L0 1.0 from +x+y+z, L1 0.3 opposite, L2 0.3 = the camera light from light_move;
    no ambient rows; flPS2Ambient = 0; m_desert: ambient rows 0.35 / 0.25 / 0.20).
  - Mean colour of the hunter's pixels (mask = pixels that change when RT_LIGHT_MUL=0,0 blacks the actors; PS2 shifted
    by the best match): g_camp PS2 82,67,56 / PC fixed light 105,81,67 / game lights 57,48,42 / game lights reversed
    101,76,59; g_desert PS2 111,79,53 (shift less sure) / fixed 88,65,50 / game 157,111,78. Visually the game lights
    match the desert hunter well and are too dark and too directional at the camp: the PS2 hunter there is flatter.
  - So the default fixed light is too bright at the camp and too dark in the desert (a stage-independent light cannot
    match both), and the game lights are right in kind but something in the VU path flattens them. Lead: the program's
    PROG_LOOP turns the light matrix (mem 9-11, PS2SHADER_ADD_LIGHTVECD3, raw asm) into each bone's space and normalises
    its ROWS; if mem 9-11 hold one light per column (agent F's reading), that renormalisation mixes the lights. Needs
    PS2SHADER_ADD_LIGHTVECD3 decoded and the per-bone result checked against the camp picture. Not changed (no guess
    shipped). The MH1 forest Aptonoth (darker on the PS2) is the same actor path.
- Test aids: RT_APX_TRACE=1 (APX headers), RT_LIGHT_TRACE=4 now prints the vertex colour and normal length ranges,
  RT_LIGHT_MUL=AMB,COL (scales the actors' ambient and light colours), RT_LIGHT_FLIP=1 (light directions reversed).
