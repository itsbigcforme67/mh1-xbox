# MH Dos: shell08 init, ground routes, and five EMW field fixes (design for the next build round)

Research only (4 Nov 2026). Written from our own split of the owner's disc (asm/mh2, asm/mhg, asm/game, build/mh2/*.elf and
build/mhg/*.elf read with a small stdlib script) and our own MH1/G C (names). Community mh2-decomp not opened. Nothing was built or
run. Names are ours (G/MH1 C, config/mh2/game_aliases.txt, func_pairs.txt); unnamed functions are sub_<addr> / game_func_<addr>.

Marks: **[asm]** read from the disassembly for this note (I'm confident of it); **[count]** an access-width count over every
game/gm_sub function (grep of `op $r, 0xNNN($r)`); **[guess]** not proved. Line numbers are those of the main checkout on 4 Nov.

---

## 1. shell08 (fireballs / beams): MH2's init and type tables

### 1.1 The functions (MH2 game addresses)

| MH2 | ours (G C) | notes |
|---|---|---|
| 0x6CADF0 | (none; G inlines it) | common setter: type 8, arg, x07, move = shell08_move, owner, x7A, em_no, **xD1 = owner byte 0x409**, xCC = owner ang[1] [asm] |
| 0x6CAE40 Shell08_set_ang, 0x6CAF20 Shell08_set_ang_time | same names | **first call Em_stg_ck(em); no shell when it returns 0**; then 0x6CADF0, joint position, char0, x7E = kind, ang; **x05 = 0** (G 1); x06 = 0 / time [asm] |
| 0x6CB010, 0x6CB0E0 | Shell08_set_shl (G has one) | two "start at this position" setters; **x05 = 1** (G 2). Which one is G's set_shl was not settled [asm, partly] |
| 0x6CB1D0 shell08_move | shell08_move | modes 0-9 (G 0-7): 0 init, 1 m, 2 d, 3 h, 4 d, 5 d, **6 0x6CEF70** (G's slot for shell08_e; guess it is MH2's e), 7 d, **8 nothing, 9 d** [asm, jump table 0x8DD4B0] |
| 0x6CB2A0 | (the pl_atck_data_set_shl line of shell08_i) | attack data: pl_atck_data_set_shl(sh, em, (arg is 12 or 24 ? 0 : x07) + atk_no[arg], shell08_tbl) [asm] |
| 0x6CB310 | shell08_i | the init, 0xCD4 bytes [asm, all of it] |
| 0x6CBFF0 m, 0x6CEE70 h, 0x6CEF00 d, 0x6CEF80 trans_sub, 0x6CF020 trans | same names | only their table reads were looked at |
| 0x6D27D0 | shell08_type8_init | MH2 calls it with two arguments (sh, w) after the type switch, **instead of** the attack data for type 8 [asm; 2 args = guess from the registers set] |
| main 0x194F40 | Em_se_req2 | same call shape as G's Em_se_req2(owner, se, 0, pos, 1, 0) in type 6 [asm] |

### 1.2 Types: 25 in MH2 (G 14, MH1 12)

The init's switch is `arg < 25` through a 25-entry jump table at 0x8DD4E0, and every per-type table below has 25 live entries then
zeros [asm]. MH2's types by what the init does (G's for 0-13 in brackets where it differs):

| arg | init in MH2 |
|---|---|
| 0, 11 | second prim; particle 0: x14 random; 1-2: x16/x18 from the two small s16 tables + jitter; 3+: x10 = 0xC000 (as G) |
| 12 | start offset from the type-12 table [x07], turned by ang[1], then as 0 (as G) |
| 1, **18** | aim: w->x01 = ang[0] >> 8, w->x37 = (ang[1] - owner ang[1]) >> 8, w->char0 = owner char0, shell08_type1_pos_set; **new: p[0].x1A = 0x71C** |
| 2, 3, 9, 13, **22, 23, 24** | rate x 2 when (9 and x07 >= 2) **or (23 and x07 > 0)**; second prim; particles no 0, x02 = -4i, x14 random, x1A = 0x199A (as G). **22** first plays se 0x1C; **24** first stores an f32 from a per-x07 table (0x89DE00) at **SHLW+0xC8** (the `_mh2padC8` in include/shell.h) and plays se 0x27 when x07 == 1, else 0x18 |
| 4, **21** | **the type-1 aim set-up as well** (x01, x37, char0, shell08_type1_pos_set), then w->x36 = sh->x06, **sh->x06 = 0**; particle with no 0: x1A = 0x1000 |
| 5, 7, **17** | nothing beyond the common part |
| 6 | pos2.y = owner x5AC; se 0x37; second prim; particles i > 0: (i == 5: no 4, x02 -5), x14 random. **p[0].x14 is not randomised** (G did all) |
| 8 | w->x00 = strike count [x07 % 6] + 1; w->base = owner pos; **p[0].x02 = -7** (G: random x06 bits, p[0] by type8_init, others -20); type8_init comes after the switch |
| 10 | pos2.y = GetGroundHit(pos2) (as G) |
| **14** | sets every particle's scale to 1.0 again (no change from the common part) |
| **15** | second prim; p[0] scale 3.0, x10/x12/x14 random; **w->x30 (MH2 +0x34) = GetGroundShellHit(pos2)** |
| **16, 19, 20** | p[0] scale 2.0 / 1.5 / 5.0; x10/x12/x14 random; x16/x18/x1A = (random & 0xFF) - 0x80 |

### 1.3 The init, step by step, against G's shell08_i [asm]

Common part (all types), in order:
1. mode = 1, be_flag = 1, trans = 0, shell_flag_set(sh, 0x200), xB4 = 0, stg (u16 at SHLW+0xCE) = em->stg.
2. w->x00..x03 = 0 (x02, x03 are MH2-only bytes), w->joint = sh->char0, w->x2E = 0, sh->char0 = 0.
3. **Rate**: rate = (0, 0, speed[arg]), then flvecRotX by ang[0], flvecRotY by ang[1] (angle units: 2 pi x a / 65536).
   **rate_g = (0, gravity[arg], 0)**. G had per-type vec3 rate and rate_g tables and a joint-matrix path when x05 == 0; MH2 has
   one f32 per type for each and always turns by the angles.
4. **Offset**: unless x05 == 1 (G: unless x05 == 2), ofs[arg] (vec3) through the joint's world matrix is added to pos2; pos0 =
   pos2. (MH2's setters use 0 = from a joint, 1 = from a position; G used 1 / 2.)
5. x05 = 0; main prim (trans = shell08_trans) or NULL; w->prim = 0.
6. **Particles, generic**: for i < num[arg]: no = i, x02 = 0, x10-x1A = 0, x1C-x1F = 0, **scale = 1.0** (G left scale alone and
   cleared per type).
7. The type switch (table above).
8. Type 8: shell08_type8_init(sh, w). Others: 0x6CB2A0 (attack data). **Then for all types** x88 = body_tbl[sh->body], x8C = 0
   (G skipped both for type 8).
9. shell08_m(sh).

### 1.4 MH2's work layout (SH08W, sh->senko): +2 / +4 against G

| field (G name, include/shell08.h) | G | MH2 |
|---|---|---|
| x00, x01 | 0x00, 0x01 | same |
| (new) x02, x03 | - | 0x02, 0x03 (cleared by the init, use unknown) |
| prim_no (s16) | 0x02 | **0x04** |
| prim | 0x04 | **0x08** |
| base (type 8) | 0x08 | **0x0C** |
| x14 | 0x14 | 0x18 [guess: +4 like base; not seen in the init] |
| joint, x2E, x30, char0, x36, x37 | 0x2C, 0x2E, 0x30, 0x34, 0x36, 0x37 | **0x30, 0x32, 0x34, 0x38, 0x3A, 0x3B** |
| p[] (SH08P, 0x20 each, layout unchanged) | 0x38 | **0x3C** |

Because shell08_m / h / d / trans read the same work by name, **the MH2 init and G's m/trans cannot be mixed unless SH08W itself
takes MH2's layout under VERSION_MH2** (the PC's G code is compiled from names, so a VERSION_MH2 SH08W fixes every reader at once).

### 1.5 The tables (MH2 game data; G addresses from config/mhg/pc_data.txt)

| MH2 | entry | count | what | G |
|---|---|---|---|---|
| 0x89CAE0 | s32 | 12 | body_tbl by sh->body (SHLW+0x6A) | shell08_body_tbl (MH1 import) |
| 0x89CB10 | attack rows | to 0x89CFC0 | pl_atck_data_set_shl rows | sub_main 0x2EEA20 area |
| 0x89CFC0 | {ptr, 0, 0, 0} | 1 | shell08_tbl, points at 0x89CB10 (already in config/mh2/pc_data.txt) | 0x2EEA20 |
| 0x89D0A8, 0x89D0B0 | s16 | 4 each (3 used) | the type 0/11 particle x16 / x18 bases (rotx_em01 / roty_em01) | MH1 imports |
| 0x89D760 | ptr | **18** + 2 zero words | shell08_data (key lists 0x89CFD0..0x89D75F) | 17 |
| 0x89D7B0 | s16 | 25 (row 0x40) | shell08_num | 0x6C5D00 |
| 0x89D7F0, 0x89D830, 0x89D870, 0x89D8B0 | s16 | 25 each | read at the top of shell08_m; **0x89D830 is also read by shell08_trans, as G's all_time is** | G: time_g 0x5D20, all_time 0x5D40, index 0x5D60, step_g 0x5D80 |
| 0x89D8F0 | s16 | 25 | **new**: read by shell08_trans only | - |
| 0x89D930 | s16 | 25 | shell08_atk_no (in pc_data.txt at size 0x1C: too short) | 0x6C5DA0 |
| 0x89D970 | f32 | 25 | **new form**: forward speed (rate z before turning) | rate_tbl vec3 0x6C5DC0 |
| 0x89D9E0 | f32 | 25 | **new form**: gravity (rate_g y) | rate_g_tbl vec3 0x6C5E70 |
| 0x89DA50 | f32[3] | 25 | shell08_ofs_tbl (also read by shell08_type1_pos_set, 0x6D1FBC) | 0x6C5F20 |
| 0x89DB80 | s16 | 6 | type-8 strike counts by x07 % 6 (shell08_type8_thunder_num) | MH1 import |
| 0x89DCE0 | f32[3] | 24 | type-12 start offsets by x07 (same shape as G) | shell08_type12_ofs_g 0x6C6130 |
| 0x89DE00 | f32 | 4 | **new**: type 24 value by x07, stored at SHLW+0xC8; also read at 0x6CFB04, 0x6D1D10 | - |

Which of 0x89D7F0 / 0x89D870 / 0x89D8B0 is time, index and step: **[guess] G's order** (time, all_time, index, step); only
all_time is pinned (both m and trans read it). Others in the block (0x89D5A0-0x89D740, 0x89DB90-0x89DCD8) are read by m, trans,
impact and type-8 code, not traced.

### 1.6 Plan for the build agent (shell08)

What the PC has today: G's shell08 code; under MH2 only shell08_tbl and shell08_atk_no (14 entries) sit at MH2's places, so every
other G table (num, rate, ofs, body, data...) is read at **G addresses in MH2's memory**, and args 14-24 run past them.

1. `include/shell08.h`: SH08W under `#ifdef VERSION_MH2` with the layout in 1.4 (x02, x03 new; pad to keep prim at 0x08, base at
   0x0C, joint at 0x30, p at 0x3C). `include/shell.h`: name the f32 at SHLW+0xC8 (MH2 block: `f32 xC8_f;` in place of
   `_mh2padC8[4]`, name free).
2. `config/mh2/pc_data.txt` (after the shell08 lines, replacing their "14 types" comment): `shell08_atk_no` size 0x32; add
   `shell08_num = 0x0089D7B0; // size:0x32 noptr`, `shell08_time_g = 0x0089D7F0`, `shell08_all_time = 0x0089D830`,
   `shell08_index = 0x0089D870`, `shell08_step_g = 0x0089D8B0` (size 0x32 noptr each; the three middle names by G's order,
   guessed), `shell08_ofs_tbl = 0x0089DA50; // size:0x12C noptr`, `shell08_body_tbl = 0x0089CAE0; // size:0x30 noptr`,
   `shell08_data = 0x0089D760; // size:0x48 allptr`, `shell08_type12_ofs_g = 0x0089DCE0; // size:0x120 noptr`,
   `shell08_type8_thunder_num = 0x0089DB80; // size:0xC noptr`, `rotx_em01 = 0x0089D0A8; // size:0x8 noptr`,
   `roty_em01 = 0x0089D0B0; // size:0x8 noptr`, and the new ones with descriptive names: `shell08_speed_mh2 = 0x0089D970;
   // size:0x64 noptr`, `shell08_grav_mh2 = 0x0089D9E0; // size:0x64 noptr`, `shell08_trans_mh2 = 0x0089D8F0; // size:0x32
   noptr`, `shell08_t24_mh2 = 0x0089DE00; // size:0x10 noptr`. (G's rate_tbl / rate_g_tbl get no MH2 entry: MH2 has none.)
   Check the shell08_data key lists are copied with it (allptr points into 0x89CFD0-0x89D75F).
3. `src/game/shell/shell08.c`: an MH2 shell08_i under `#ifdef VERSION_MH2` written from 1.3 (keep G's under `#else`), with
   0x6CB2A0 as a static helper (or inline). Setters: Em_stg_ck gate, x05 = 0 / 1, xD1 from owner+0x409 (MH2_SHELL_END1 already
   does the xD1 copy elsewhere). shell08_move: add `case 8: break; case 9: shell08_d(sh);` under VERSION_MH2 and send case 6 to
   shell08_e (guess that 0x6CEF70 is e). shell08_type8_init: MH2 calls it as (sh, w); keep G's body with p = w->p as a stand-in.
4. G's shell08_m / trans on MH2 types 14-24 have no cases: those shots will not move or draw. That is the next step after the
   init (MH2's m has jump tables 0x8DD550, 0x8DD5C0, 0x8DD630, 0x8DD6A0; trans 0x8DD780); not read here.
5. Test: quest 1014's Rathian fireball (the crash that the round-6 stand-in fixed) plus whatever arg MH2's em01 passes (trace
   Shell08_set_* arg on the PC first; config/mh2/pc_data.txt says arg 24 is used).

---

## 2. Ground routes: em_cmd 0x3F and 0xFF case 4 (and 5)

### 2.1 Where the data is

- `EMW.area` (MH2 EMW+0x894, MH1 0x940) = **em_pos_tbl[kind]** at **game 0x8BB280** (74 words), set by MH2's em init (gm_sub
  0x904770) [asm]. config/mh2/data_map.txt already names 0x8BB280 em_pos_tbl; rt_mh2_em.c:200 reads it ("guess" in its comment:
  it is right).
- `area->x18` (+0x18): the per-stage route lists, the same record shapes as G's (include/em_cmd.h EM_ROUTE / EM_ROUTE_PT,
  include/em.h EM_STG_POS) [asm: offsets used by both handlers]:
  - EM_STG_POS: 8 bytes {s16 stg, s16 num, EM_ROUTE *routes}; the list ends with stg -1.
  - EM_ROUTE: 0x18 bytes {u16 stg (stage it leads to, 0xFFFF = none), s16 num, EM_ROUTE_PT *pt, f32 pos[3] (where the monster
    appears in the new stage), u16 ang (+0x14)}.
  - EM_ROUTE_PT: 0x14 bytes {f32 pos[3], f32 radius, u16 hit_ck, u16 move (ground_area_move_ptr_set index)}.
  - Stage numbers in the records are raw; every reader passes them through sub_main 0x379B20 (the "current form" map,
    docs/mh2/em_cmd.md section 3) before comparing or storing.
- Example (MH2 data, kind 3): lists for 25 stages; stage 112 has two routes, to 111 (7 points) and 113 (3 points); most routes
  have stg 0xFFFF [read with the stdlib script].
- **Kinds whose area x18 is not a pointer**: for kinds 1, 2, 10, 11, 18, 19, 24, 36, 37, 41, 42, 49, 54, 56, 59, 60, 63, 64, 65,
  71 the word at +0x18 is 0x0708FFFF (G has NULL there for its flyers). Either those kinds never run 0x3F / 0xFF 4, or the PS2
  would fault. [guess: never run.] On the PC, treat an x18 outside the loaded images as NULL so a stray command cannot crash.
- `gp_ck` (game 0x707D70) and `gp_ptr_ck` (0x707E40), which pick the stage's entry, **changed in MH2** [asm]: an entry matches
  when stg -1 (the end, returned as is), or stg >= 10000 and stg - 10000 == the stage exactly, or 0x379AC0(entry) ==
  0x379AC0(stage) (same area, either form); at most 197 entries, then NULL. The PC's gp_ck / gp_ptr_ck
  (src/game/em/em_master_nm.c:346, :357) are G's: equality, 88 entries. With them a class-1 (other form) stage never finds its
  list and every route-based command falls back to "no route".

### 2.2 em_cmd 0x3F, em_cmd_ground_area_move (MH2 0x72C610, G 0x5898C0) [asm, whole function]

Operands: 2-byte big-endian target stage, 1 height byte (4 bytes with the opcode). Returns p + 3 on every path.
1. x8BE (0x812) = 0; height byte to **EMW+0x974** (see 3.6); h = 10 x byte.
2. target = 0x379B20(stage); if em->stg == target: return.
3. g = gp_ck(em, area->x18, em->stg); no routes: return. x73A = 0x379B20(stage) (u16 0x684).
4. x73A == 0xFFFF: the nearest route by its first point (XZ), as G; a route with no points sets x9DB (0x95F) = 7, j = 0.
   Else: the first route r with **0x379B20(r.stg) == x73A** (G compared r.stg directly); none: x9DB = 1, return path as G.
5. x86D (0x7C1) = the route. **Then two cases on the player's stage (u16 at 0x6ADCC4 = game_w+0x14):**
   - monster on another stage than the player: **the hit_ck point nearest the monster (XZ)** is taken, j = its index (G took the
     first hit_ck point and set j = num - 1);
   - same stage: as G, the first hit_ck point with a clear line (GetWallHitLine at height h, wall bits EMW+0x8B2).
   No point found: the route's last point, x9DB = 2, j = num - 1.
6. Tail as G: x9DB with x8C3 (0x817) resets to route 0 point 0; x86E (0x7C2) = j; x827 = 9, x828 / x829 (u16) = x86D / x86E,
   x881 / x882 / x883 copies (0x997, u16 0x998, u16 0x99A); cmd_p868 (0x7BC) = p; x86C (0x7C0) = pt->move; return
   ground_area_move_ptr_set(em, x86C) (MH2 0x733E10, carried unchanged from em_cmd_r25.c).

### 2.3 em_cmd 0xFF case 4, end of a ground route (MH2 0x7309F0, G 0x58D330) [asm, the case]

**MH2 sends case 5 to the same code**; the only difference is that the stage change at the end happens for 4 only [asm]. G has
no case 5 (the PC's switch then leaves r = p, so a 0xFF 05 in a script would run its 0x05 next: a bug on MH2 data).
1. g / routes as 0x3F; no routes: r = cmd_p868, cmd_p868 = NULL, x827 = x881 = 0 (as G).
2. Clamp x86D to g->num - 1 and x86E to the first route's num - 1 (as G: both games read routes[0].num); pt = routes[x86D].pt.
3. x8C3 set: x881 == 9 -> re-issue pt[x86E].move; else cmd_idx = 0, em_cmd_top (as G).
4. Distance to pt[x86E]: **kinds 0x3A and 0x48 use flvecCalcDistance (3D)**, the others CalcDistanceXZ.
5. Not within its radius: x8BE += 1; under 10: re-issue the move to pt[x86E], cmd_top = r (as G). **At 10: x86E = 0; if it was
   already 0 go to "arrived", else x8BE = 0 and re-issue the move to point 0** (G: x86E = num - 1 and return p).
6. Within the radius and x86E != 0: x8BE = 0, x86E -= 1, re-issue (as G).
7. Arrived (x86E == 0): **only for case 4**, when routes[x882].stg (as s16) != -1 and EMW byte 0x966 (x9E2) != 0:
   pos = routes[x86D].pos; **x5A0 (0x568) = pos (new)**; ang[1] = routes[x882].ang (**no + 0x4000**; G's asm has none either,
   MH1's has it); x92E (u16 0x882) = em->stg (MH2 loads only the low byte; stages are < 200, so no difference);
   em->stg = **0x379B20(routes[x882].stg)**; **game 0x705B80** (MH2's em_area_move_init: same per-kind dispatch shape as G's,
   with more kinds, func_pairs.txt order 0.66) ; em_g_init_flag_set(em, 1); **GetGroundHitStatusAreaEm(em, pos, EMW+0x660,
   &x5AC (0x574), &EMW+0x730)** (new; rt_mh2_em.c already calls it with these arguments).
   Then (both cases) r = cmd_p868, cmd_p868 = NULL, x827 = x881 = 0.

### 2.4 Plan for the build agent (routes), all in shared C

1. **gp_ck / gp_ptr_ck** (src/game/em/em_master_nm.c:346-365): add a `#ifdef VERSION_MH2` body: loop i < 197; return p when
   p->stg == -1; if (u16)p->stg >= 10000 compare p->stg - 10000 with stg exactly, else compare sub_main_func_00379AC0(p->stg) ==
   sub_main_func_00379AC0(stg) (declared in include/em_cmd.h; include it or declare locally). gp_ptr_ck the same with em->stg,
   returning p->pos. [asm]
2. **0x3F** (src/game/em/em_cmd_nm.c):
   - :2567 `if (wk->stg == stage)` -> under VERSION_MH2 `if (sub_main_func_00379B20((u16)wk->stg) == stage)`. [asm]
   - :2583-2600 (the point loop): under VERSION_MH2, test `*(stg_t *)MH2V(0x3F3404, 0x6ADCC4) != em->stg` once before the loop;
     if so take the hit_ck point with the smallest CalcDistanceXZ(em->pos, pt->pos) (start best at -100.0f: "best < 0 or d <
     best"), found = (index < num), j = index, pt = that point; else the G wall-test loop as it is. :2585's `*(u8 *)0x3F3404` is
     MH1's address even for G (G's asm reads 0x35ED34) and wrong for MH2: use the MH2V form there too. [asm]
3. **0xFF** (src/game/em/em_cmd_nm.c:4188): `case 0x4:` -> under VERSION_MH2 `case 0x4: case 0x5:`; in the body:
   - :4216 distance: `(em->kind == 0x3A || em->kind == 0x48) ? flvecCalcDistance(em->pos, pt[...].pos) : CalcDistanceXZ(...)`;
   - :4219-4220 the 10-try limit: MH2's form from 2.3 step 5;
   - :4235 add `*p == 4 &&` before the stage test (case 5 never moves);
   - :4236-4244 under VERSION_MH2: flvecCopy(em->x5A0, em->pos) after the position, ang without + 0x4000, em->stg =
     sub_main_func_00379B20((u16)rt[em->x882].stg), em_area_move_init(em) (the PC has G's; MH2's 0x705B80 has more kinds: fine
     as a stand-in), em_g_init_flag_set(em, 1), GetGroundHitStatusAreaEm(em, em->pos, (u8 *)em + 0x660, &em->x5AC, (u8 *)em +
     0x730) (prototype as in rt_mh2_em.c:287).
   - :4239 for G as well: G's asm stores the angle without + 0x4000 (only MH1 adds it), so the + 0x4000 belongs under
     `#ifndef VERSION_MHG`. This changes the PC's G behaviour: list it in docs/REVIEW.md. [asm, all three games]
4. Guard for the kinds with x18 = 0x0708FFFF (2.1): in both handlers, treat an area->x18 that rt_addr cannot map as NULL
   (PC only, comment it as a guard, not MH2 behaviour).
5. Test: a quest with a ground walker that changes stage (kind 3 / 4 / 12 on the jungle, stages 111-113; the 1014 jungle has
   Kelbi kind 3 on 112): RT_EMCMD_TRACE=1 shows 0x3F and 0xFF 04 / 05, and the monster should appear on the route's stage at
   the route's position.

---

## 3. EMW field fixes (exact edits)

The header is generated (include/mh2_emw.h, include/mh2_ofs.h by tools/mh2_structs.py gen from probe votes plus
config/mh2/struct_map.txt hand lines). As round 6 did, add the struct_map lines **and** hand-edit both headers the way gen would
write them (gen needs the probe output). Each fix needs both headers: mh2_emw.h for `em->name`, mh2_ofs.h for EM_FIELD / EMF /
MH2_OFS users.

### 3.1 home: 0x688, not 0x690 [asm + count, verified]

Evidence: f32 accesses at 0x688, 0x68C, 0x690 come in matching sets (3 lwc1 + 4 swc1 at each) [count]; MH2's kind-10 init (game
0x797F90) copies pos[0..2] to 0x688 / 0x68C / 0x690 where G's em10_init copies pos to home [asm]; 0x694 is an s16 (48 lh, 5 sh)
[count], so a vec3 at 0x690 is impossible.
- config/mh2/struct_map.txt (in the hand block): `EMW 73C 688 # home f32[3]: lwc1/swc1 sets at 0x688/0x68C/0x690, kind 10 init
  game 0x797F90; 0x694 is an s16`.
- include/mh2_emw.h:140-141 `u8 x734; ... u8 _pad689[0xF];` -> `f32 home[3]; /* 0x688, MH1 0x73C: hand */` and `u8
  _pad694[0x4];` (so x74C stays at 0x698); line 379 (the home overlay at 0x690) -> deleted.
- x734 then needs a place. No byte access was found at 0x67C-0x67F or 0x686-0x687 [count], and MH2's em01_init and kind-10
  init do not write it (G's write 3 and 0) [asm]: its MH2 place is unknown, maybe gone. Stand-in: put it in the unused pad
  0x686 (`EMW 734 686 # stand-in: no MH2 place found; kept off home`), header line 139 `u8 _pad686[0x2];` -> `u8 x734; /* 0x686,
  MH1 0x734: hand, stand-in */ u8 _pad687[0x1];`. [guess: place]
- include/mh2_ofs.h: before line 67 insert `    (o) < 0x734 ? (o) - 0xAC : \`, change line 67 to `    (o) < 0x736 ? (o) -
  0xAE : \` (0x734 / 0x735 -> 0x686 / 0x687); replace line 71 `    (o) < 0x74C ? (o) - 0xAC : \` by `    (o) < 0x748 ? (o) -
  0xB4 : \` and `    (o) < 0x74C ? (o) - 0xAC : \`.
- Then src/game/em/em10_nm.c:362-364 can say `em->home[i]` again (the comment at :349-350 goes).

### 3.2 x88B: 0x7DF, not 0x437 [asm + count, verified]

Evidence: MH2's kind-10 init stores x839 (0x78D) = 0 then 1 at **0x7DF**, in G's em10_init order (`x839 = 0; x88B = 1;`);
MH2's em01_init stores 0x78D, then 0x7DF, then x765 (0x6B1) as G's (`x839; x88B; x765`) [asm]. 0x7DF: 318 sb, 12 lbu; 0x437: 95
sb and **no loads** [count]. The neighbours fit the -0xAC run (x88A 0x7DE, x88C 0x7E0).
- config/mh2/struct_map.txt: `EMW 88B 7DF u8 # kind 10 init 0x797F90 and em01_init store it after x839 as G; 0x437 is never read`.
- include/mh2_emw.h:102-103 `u8 _pad41A[0x1D]; u8 x88B; /* 0x437 ... */` -> `u8 _pad41A[0x1E];`; line 267 `u8 _pad7DF[0x1];`
  -> `u8 x88B; /* 0x7DF, MH1 0x88B: hand */`.
- include/mh2_ofs.h:86 `    (o) < 0x88C ? (o) - 0x454 : \` -> delete (0x88B then falls in the -0xAC line).
- src/game/em/em10_nm.c:378 `*((u8 *)em + 0x7DF) = 1;` -> `em->x88B = 1;`.
- Leave src/pc/rt/rt_mh2_em.c:222 `B8(0x437) = 1;`: MH2's em init really stores 1 at 0x437 (gm_sub 0x90482C) [asm]; what that
  byte is stays open.
- Follow-up seen on the way [asm]: **MH2's em01_init stores 0 in x88B** (G's C, em01_ai_nm.c:309, stores 1). Once x88B lands
  at 0x7DF the PC's Rathian starts with G's 1; an MH2 branch there (`em->x88B = MH2V(1, 0)`) is right by the asm, its effect in
  play is not known.

### 3.3 0xFF written into the u16 stage fields x92F / x92E [asm, verified]

Evidence: MH2's em_fly09 copies (game 0x7EB4B0 em01, 0x7B1290 em15, and 0x747CD0, 0x77BEC0, 0x830E60, 0x84E080) all store 0xFFFF
at 0x986 (x92F), and em01's two readers (0x7EB3DC, 0x7EBEAC) compare x92F with 0xFFFF [asm]. On the PC a stored 0x00FF reaches
MH2-faithful code as "stage 255" (NextStage_No_Set, the stage maps' 200-entry tables).
- First move `#define EM_STG_NONE MH2V(0xFF, 0xFFFF)` from include/em_cmd.h:51 to include/em.h (em_cmd.h includes em.h, so its
  17 users keep it). MH1/G bytes do not change (0xFF there).
- Writes, `em->x92F = 0xFF;` -> `em->x92F = EM_STG_NONE;` at: src/game/em/em01_ai_nm.c:2099, em01_aid.c:767, em15_nm.c:1900,
  em15_r17.c:370, em17_nm.c:2168, em17_r24.c:358, em20_ai_nm.c:2552, em20_r28.c:450. (The _rNN / _aid files are the MH1/G match
  parts of the same functions; change them too so the copies stay alike.)
- Reads: the locals that take x92F are u8, so `== 0xFF` works on MH2 only by truncation and `(u16)em->x73A == local` compares 8
  bits. Change the local's type `u8` -> `stg_t` and `== 0xFF` -> `== EM_STG_NONE` at: em01_ai_nm.c:2053-2054 (local `ar`, declared
  in the function from :2010) and :2334-2335 (from :2273), em01_aid.c:726-727 (from :683), em15_nm.c:1852-1853 (`temp_v1_3`, from
  :1797) and :2152-2153 (`temp_v1`, from :2095), em20_ai_nm.c:2504-2505 (from :2445) and :2821-2822 (from :2756); the same
  pattern in em15_r17.c / em17_r24.c / em20_r28.c if their copies have the read (grep `= em->x92F;`). stg_t is u8 for MH1/G.
- Also em_cmd_r01.c:196-197, em_cmd_r04.c:175, em_cmd_r21.c:171/185, em_cmd_r26.c:153-154 write 0xFF; they are MH1/G match parts
  not built for MH2 (only em_cmd_r04's 0x728C70-0x728D18 part is carried, and it is not that function): EM_STG_NONE there too,
  for consistency.

### 3.4 em_species_tbl for MH2 [asm, verified]

Evidence: MH2's species helper sub_main 0x3BC090 returns byte 0 of hunt_group_tbl[kind] (D_0044C800 + kind x 4) [asm], exactly
what G's code reads as `*em_species_tbl[kind]`; in G both names are one table (config/mhg/pc_data.txt: em_species_tbl and
hunt_group_tbl both 0x4FEE90). Today the MH2 PC takes em_species_tbl from G's pc_data (G's address, junk in MH2's memory): it is
read by em_cmd_nm.c:2024, em_master_nm.c:1095, f_quest.c:749, camq0.c:64, evdemo.c:382.
- config/mh2/pc_data.txt, after line 32 (hunt_group_tbl): `em_species_tbl = 0x0044C800; // size:0x128 allptr # = hunt_group_tbl
  (sub_main 0x3BC090 reads byte 0 of the kind's list): G's two names for one table`.
- Same problem, not checked here: f_quest.c:749 also reads em_size_max_tbl (G's place) by species.

### 3.5 sub_main 0x379AC0: it returns u16 [asm + byte match, verified]

The function returns 0xFFFF for 0xFFFF, else 0x379A70's u16 result, else its argument; src/mh2/sub_main/stage_map.c:43 declares it
`u16 sub_main_func_00379AC0(u16 s)` and byte-matches (mh2_check). include/em_cmd.h:55 says the same.
- src/mh2/game/em_dmg_sys_nm.c:22 `int sub_main_func_00379AC0(), sub_main_func_003799A0();` -> `u16 sub_main_func_00379AC0(u16);`
  and `int sub_main_func_003799A0();` (its `& 0xFFFF` masks at :153-161 stay, harmless). This file would not compile next to
  em_cmd.h's declaration (today em_taisei.c includes it without em_cmd.h).
- src/mh2/game/p0080D270.c:34 `s16 sub_main_func_00379AC0();` -> `u16 sub_main_func_00379AC0(u16);`. This file is byte-matched
  (config/mh2/c_files.txt:1298): run mh2_rebuild / mh2_check after the change. The results go straight into u16 fields with sh,
  so I expect no byte change [guess].

### 3.6 Found on the way: the 0x3F height byte (MH1 0x9F0) is at 0x974, not 0x96C [asm, verified]

em_cmd_ground_area_move stores and reloads its height byte at EMW+0x974 [asm]; it is the only access there (1 sb, 1 lbu [count]).
include/mh2_ofs.h:101 `(o) < 0x9F1 ? (o) - 0x84` maps MH1 0x9F0 to 0x96C, which is x9E8 (votes 5; 8 sb, 1 lbu there), so the
PC's EM_FIELD(em, u8 *, 0x9F0) in em_cmd_nm.c:2531-2532 overwrites x9E8. x9F1 at 0x975 already sits in the -0x7C run.
- config/mh2/struct_map.txt: `EMW 9F0 974 u8 # em_cmd_ground_area_move (game 0x72C610) sb/lbu, its only use`.
- include/mh2_ofs.h:101 -> delete the line (0x9F0 then falls in `(o) < 0xA02 ? (o) - 0x7C`).

---

## 4. What is verified and what is guessed

Verified from the asm: the 25 shell08 types and the init's steps (1.2-1.3); the SH08W offsets the init touches; the shell08 table
addresses, entry sizes and readers; MH2's setters' x05 values and Em_stg_ck gate; the route records' layout and both handlers'
logic (2.2, 2.3) including case 5, the 3D-distance kinds, x5A0, GetGroundHitStatusAreaEm, and that G's asm has no + 0x4000;
em_pos_tbl feeding EMW+0x894; MH2's gp_ck / gp_ptr_ck matching rule; home at 0x688; x88B at 0x7DF; MH2 writing and comparing
0xFFFF in x92F; em_species_tbl == hunt_group_tbl via 0x3BC090; 0x379AC0's u16 result; the 0x9F0 -> 0x974 place.

Guessed: which of 0x89D7F0 / 0x89D870 / 0x89D8B0 is G's time / index / step; 0x6CEF70 = shell08_e; type8_init taking (sh, w);
SH08W.x14 at 0x18; 0x705B80 = em_area_move_init (shape only); the flyers' 0x0708FFFF x18 never being used; x734's MH2 place (none
found; 0x686 is a stand-in); the effect of MH2's x88B = 0 in em01_init; that p0080D270 still matches with the u16 declaration.

Not read: MH2's shell08_m / h / d / trans bodies (only their table reads), the shell08_data key lists, the meaning of SH08W
x02 / x03, the SHLW+0xC8 f32's use, EMW+0x437, MH2's em_area_move_init kinds beyond G's.
