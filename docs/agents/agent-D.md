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
