# MH Dos (MH2): the game camera and its CameraWork (research, 4 Nov 2026)

- **Scope:** MH2's (JP 1.04) game camera in sub_main 0x31A580-0x324080, its CameraWork (0x5AF420, 0x680 bytes), against our
  G camera C (src/main/cam/, include/cam.h) and src/pc/rt/rt_cam.c.
- **Source:** asm/mh2/sub_main (spimdisasm output), asm/mh2/sub_main/data (the slot tables), docs/survey/mh2_map.csv,
  config/mh2/func_pairs.txt, c_files.txt, c_aliases.txt, c_renames.txt. Read only, nothing built or run.
- **Proof:** offsets were read off the loads and stores of each function (base registers followed by hand, plus a register
  tracking script). "guess" marks meanings that were not checked. Nothing here is byte-checked yet.
- **Names:** from our G / MH1 C. sub_<addr> for MH2 functions without one. "slot N" is MH2's numbering (0-5).

## Short version

G's CameraWork is five 0x100-byte slots, each a 0x80 head plus 0x80 of per-mode work (the `d` union), then the shared tail
at 0x580. MH2 split that: **six 0x80-byte slot heads at 0x80..0x37F with no union**, and the per-mode work moved out of the
slots into **fixed blocks of the CameraWork** (std 0x3B0, the new slot 1 at 0x430, stage 0x470, cannon 0x4B0, player EX
0x520, demo 0x580), then three quakes, the rail position, and the old tail fields. The blend fields (eye_f, tar_f, roll_f,
fov_f, cnt, rate) went into the head, so every slot blends the same way through two new helpers.

MH2 also inserted a new **slot 1** between std and stage (a camera behind the hunter's shoulder, looking far ahead, with
its pitch from a byte of the hunter's work: guess, an aiming camera), dropped the camera-area grid (Get_cam_grid_XZ is gone;
cell 0 is always used), moved the zoom level into the std block, gave CameraMove a flags argument, and added a third quake.

## The slots and how one is chosen

Slot tables: init at 0x41CD90, move at 0x41CDB0 (6 words each). G's tables have 5.

| MH2 slot | head at | init | move | G slot (G name) |
|---|---|---|---|---|
| 0 std | 0x080 | cam_init_sub_std 0x31AA40 | cam_sub_std 0x31AC00 | 0 std |
| 1 new | 0x100 | sub_0x31C260 (empty) | sub_0x31C270 | none (new) |
| 2 stage | 0x180 | cam_init_sub_stg 0x31C7E0 | cam_sub_stg 0x31C800 | 1 stage |
| 3 cannon | 0x200 | cam_init_sub_pchngr 0x31CF40 | cam_sub_pchngr 0x31CF90 | 2 pachinger |
| 4 player EX | 0x280 | cam_init_sub_playerEX 0x31D6B0 | cam_sub_playerEX 0x31D6D0 | 3 player EX |
| 5 demo | 0x300 | cam_init_sub_demo 0x31DF70 | cam_sub_demo 0x31DFA0 | 4 demo |

config/mh2/func_pairs.txt pairs the table entries by index with MH1's five and is one off from slot 1 on: its
`0x31C260 cam_init_sub_stg`, `0x31C270 cam_sub_std`, `0x31C7E0 cam_init_sub_pchngr`, `0x31CF90 cam_sub_pchngr` (wrong
row), `0x31D6B0 cam_init_sub_demo` are wrong. The table above is right.

Choosing:
- **cam_no (0x65D)** picks the base view, as in G: area type 0 -> 0 (std); types 1, 2, 3 -> **2** (stage; G used 1). Slot 1
  sets cam_no = 1 itself while it runs. Stage also resets cam_no to 0 when the area turns out to be type 0.
- **std** runs when cam_no == 0. **slot 1** runs only while std is active (sl[0].act) and the std block's byte +0x78 is set;
  that byte is set by std when the hunter is in one of a set of weapon actions (sub_0x320DA0: player kind 1 or 5 with work flag
  0x36C & 8 and certain act_ck actions, or kind 10 with flag 0x80; meaning of the kinds not checked). While that state is on,
  manual_cam_chk refuses the pad turn.
- **cam2view (0x31F4E0)** copies slot cam_no into the CameraWork head, then a special slot overrides the view in this order:
  **5 demo, then 3 cannon, then 4 player EX** (G: demo 4, pachinger 2, EX 3). The winner is stored in a new byte **0x671**
  (-1 none). Quake: qk[0] for the normal view, **qk[1] with slot 3, qk[2] with slot 5** (G: qk[1] with the pachinger,
  nothing with the demo). set_to_std_cam gets the winner (5 is turned into -1) and stores it in sl[0].req.

## CameraWork (0x5AF420, 0x680 bytes; G 0x600)

CameraWorkInit (0x31A580) clears 0x680 bytes. "R/W" lists the functions seen reading / writing (camera code unless a module
is named). G offsets are G's include/cam.h.

### Head (0x00-0x7F)

| off | size | G field (G off) | R/W |
|---|---|---|---|
| 0x00 | 12 | eye (0x00) | cam2view W; std blend R |
| 0x0C | 12 | tar (0x0C) | as eye |
| 0x60 | 4 | roll (0x60) | cam2view W |
| 0x64 | 4 | **fov (0x68)** | cam2view W. MH2 packs roll, fov together |

### Slot head (6 x 0x80 at 0x80; G's CAMS without the 0x80 union)

| off | size | G field (G off) | notes |
|---|---|---|---|
| 0x00 | 12 | eye | |
| 0x0C | 12 | tar | |
| 0x24 | 12 | vec (0x24) | std's last helper (sub_0x31FBA0) writes tar - eye; cam_sub_stg reads it |
| 0x30 | 12 | eye_f (0x30) | blend-from, now used by std, slot 1, stage (G: std head, stage kept its own copy in d) |
| 0x3C | 12 | tar_f (0x3C) | |
| 0x60 | 4 | roll (0x60) | |
| 0x64 | 4 | **fov (0x68)** | |
| 0x68 | 4 | **roll_f (0x64)** | |
| 0x6C | 4 | fov_f (0x6C) | |
| 0x70 | 1 | no | CameraMove init loop |
| 0x71 | 1 | act | |
| 0x72 | 1 | req | slot to take the yaw from (set_to_std_cam on sl[0]) |
| 0x73 | 1 | new (G: demo x65 / x66, demo.stop) | demo flags: bit 0 = the followed monster is gone (get_em_local, G's d->stop), bit 1 = skippable with pad 0x4000 (G's x65). DemoCameraRequest clears it, sub_0x31E0A0 sets 2, sub_0x31E170 tests bit 1 |
| 0x74 | 2 | cnt (0x74) | blend frames left (G: d->cnt for stage, cs->cnt for std/demo) |
| 0x76 | 2 | cnt_max (0x76) | demo get_angle |
| 0x78 | 1 | mode (0x78, a word in G) | CameraMove clears 0x78..0x7B with one word store; some slots clear 0x78..0x79 as a halfword |
| 0x7A | 1 | **step (0x7C)** | blend / script step 0, 1, 2 |
| 0x7C | 4 | **rate** (G: d->std.rate 0x64, d->stg.rate) | 1 / frames, set by the blend start helper |

Unseen: 0x18-0x23, 0x48-0x5F, 0x79, 0x7B (guess: padding, kept from MH1's head).

Blend helpers (new as functions; G does this inline in each slot):
- sub_0x320B70(cs, frames): step = 1, cnt = frames - 1, rate = 1 / frames.
- sub_0x320AB0(cs): the blend weight for this frame, an ease in / ease out curve of cnt x rate (two halves with flPow, power 2);
  counts cnt down and clears step at 0.
- sub_0x320A10(cw, cs, area): on an area change (0x3AF), looks the old area up in the new area's blend list (16 pairs, as G)
  and starts the blend, or clears step for 0 / 1 frames. G's inline code in cam_sub_std.

### Shared tail before the blocks (0x380-0x3AF)

| off | size | G field (G off) | R/W |
|---|---|---|---|
| 0x380 | 4 | init (0x580) | CameraInit W 0, CameraMove R/W |
| 0x384 | 4 | pl (0x584) | CameraInit W (&player_work[game_w.master], stride 0x970); most slots R |
| 0x388 | 4 | data (0x588) | SetCameraData, default_area_data W; StageCamInit, SetAreaData R |
| 0x38C | 4 | area (0x58C) | SetAreaData W; CameraMove, std, stage R |
| 0x390 | 0x18 | x590 (0x590) | StageCamInit copies the camera data's 0x04..0x1B here (w, h, cell sizes, origin); SetAreaData reads 0x390 (w) |
| 0x3A8 | 2 | gx (0x5A8) | CameraMove writes 0 every frame |
| 0x3AA | 2 | gz (0x5AA) | CameraMove writes 0 every frame |
| 0x3AC | 1 | grid (0x5DC) | CameraMove writes 0 every frame; SetAreaData R |
| 0x3AD | 1 | area_no (0x5DD) | CameraMove |
| 0x3AE | 1 | area_old (0x5DE) | CameraMove W; sub_0x320A10 R |
| 0x3AF | 1 | area_chg (0x5DF) | CameraMove W; sub_0x320A10, cam_sub_stg R |

No Get_cam_grid_XZ on MH2 (mh2_map.csv has none; CameraMove does not call one): SetAreaData always looks in cell 0. That fits
the host's old divide by zero on stage 110 (config/mh2/pc_data.txt: MH2's camera files have no usable grid; guess that the
grid sizes are 0 in all of them).

### std block (0x3B0, 0x80; G: sl[0].d.std, CAMD_STD)

| off | cw off | size | G field (G d off) | notes |
|---|---|---|---|---|
| 0x00 | 0x3B0 | 12 | eye (0x00) | |
| 0x0C | 0x3BC | 12 | tar (0x0C) | |
| 0x18 | 0x3C8 | 12 | old (0x18) | |
| 0x24 | 0x3D4 | 12 | **eye_d (0x30)** | |
| 0x30 | 0x3E0 | 12 | **tar_d (0x3C)** | |
| 0x3C | 0x3EC | 4 | cnf (0x58) | cam_cnf_chs (0x41C830) unless area type 0 (area + 0x20) |
| 0x40 | 0x3F0 | 4 | cnfe (0x5C) | e[zoom], e[4] (state 1 at zoom 2), 0x41C930 (bit +0x74 set), 0x41C970 (water) |
| 0x44 | 0x3F4 | 4 | fov (0x48) | eases 1/20 per frame toward fov_d (G 1/2) |
| 0x48 | 0x3F8 | 4 | fov_d (0x4C) | cnf->fov, or 70 degrees with +0x74 |
| 0x4C | 0x3FC | 4 | roll (0x50) | |
| 0x50 | 0x400 | 4 | gnd (0x24) | |
| 0x54 | 0x404 | 1 | gflag (0x28) | |
| 0x55 | 0x405 | 1 | x29 (G only) | water state (sub_0x31BF80, G's func_001DACA0) |
| 0x56 | 0x406 | 2 | x2A (G only) | water timer |
| 0x58 | 0x408 | 4 | x2C? | zeroed by init only (guess: G's ridge lift x2C, unused on MH2) |
| 0x5C | 0x40C | 4 | hit_h (0x60) | written by MH2's k_HitWallCamera (sub_0x323040) |
| 0x60 | 0x410 | 4 | x68? | zeroed by init only |
| 0x68 | 0x418 | 2 | ang (0x70) | wanted yaw; BBQcamera_set W |
| 0x6A | 0x41A | 2 | ang2 (0x72) | camera yaw (eases 1/4) |
| 0x6C | 0x41C | 2 | on (0x74) | std_cam_sw_set_sub W |
| 0x6E | 0x41E | 2 | trg (0x76) | |
| 0x70 | 0x420 | 1 | **CameraWork.zoom (0x5F6)** | zoom level moved into the block; Q_camera_init W 2, BBQcamera_set W 2 |
| 0x72 | 0x422 | 1 | wall (0x79) | |
| 0x73 | 0x423 | 1 | new | zeroed by init only |
| 0x74 | 0x424 | 1 | new | lock timer: sub_0x31BE10 sets 15 and turns toward the point at hunter work +0x568 (guess: the target monster) when bit +0x7C is on; zoom buttons cancel it |
| 0x75 | 0x425 | 1 | new | eye pulled in: set when the line from the target to the wanted eye hits a wall (sub_0x330570, radius 30, mask 0x8009) |
| 0x76 | 0x426 | 1 | new | hold timer: 8 after a pad turn or zoom change, 20 after a reset; blocks the auto turn |
| 0x77 | 0x427 | 1 | new | reset behind in progress; cleared within 0x1000 of the goal; sub_0x31BBC0 (game 0x70E060) R |
| 0x78 | 0x428 | 1 | new | slot 1 request (set while +0x7A >= 0 and the auto turn ran); slot 1 R |
| 0x79 | 0x429 | 1 | new (role of G's x6E, guess) | eye lifted over ground; next frame's ground limit 256 instead of 100 |
| 0x7A | 0x42A | 1 (s8) | new | weapon action state from sub_0x320DA0 (-1 none, 0 / 1); sub_0x31C240 R (manual_cam_chk) |
| 0x7B | 0x42B | 1 | new | hold count for +0x7A (init 3) |
| 0x7C | 0x42C | 1 | new | lock allowed: game_w+0x41 == 0 and func_734640(pl) == 1; sub_0x31C250 R |

G's CameraWork.x5F7 (and its stage_camera_data_ex states) has no MH2 field; +0x74 with the entry at 0x41C930 takes its place
(guess). The three 0x20-byte zoom entries after cam_cnf_chs (0x41C930, 0x41C950, 0x41C970) are MH2's stage_camera_data_ex
(height 280 / 280 / 250, distance 500 / 500 / 100, target height 170 / 50 / 0, ground 80); std uses the first and third.

### slot 1 block (0x430, 0x40; new)

| off | cw off | size | what |
|---|---|---|---|
| 0x00 | 0x430 | 12 | eye |
| 0x0C | 0x43C | 12 | tar (hunter position + height) |
| 0x18 | 0x448 | 12 | std's eye when slot 1 started |
| 0x24 | 0x454 | 2 | pitch: (hunter work byte +0x821 + 100) x 34 - 0xB00, eased 1/4 |
| 0x26 | 0x456 | 2 | yaw offset from std's yaw, swings toward +-0x800 |
| 0x28 | 0x458 | 2 | yaw offset goal (+-0x800) |

It blends over 9 frames from std's offsets to its own: eye 320 behind (0x41CDD0), target 1000 ahead (0x41CDE0), height 170,
then pulls the eye in on walls and lifts it over the ground; roll and fov are std's. Guess: an aiming camera for a ranged
weapon (the +0x821 byte looks like an aim pitch); not checked against the player code.

### stage block (0x470, 0x40; G: sl[1].d.stg)

| off | cw off | size | G field (G d off) |
|---|---|---|---|
| 0x00 | 0x470 | 12 | eye (0x00) |
| 0x0C | 0x47C | 12 | tar (0x0C) |
| 0x18 | 0x488 | 4 | **roll (0x34)** (init 0) |
| 0x1C | 0x48C | 4 | **fov (0x30)** (init 0.96, ZoomRateCalc result) |
| 0x20 | 0x490 | 2 | vx (0x40) |
| 0x22 | 0x492 | 2 | vy (0x42) |
| 0x28 | 0x498 | 2 | ax (0x48) |
| 0x2A | 0x49A | 2 | ay (0x4A) |

eye_f / tar_f / fov_f / roll_f, rate and cnt are in the slot head on MH2. 0x24..0x27 and 0x2C.. unseen.

### cannon block (0x4B0, 0x70; G: sl[2].d.pch, CAMD_PCH, same layout)

m[4][4] 0x4B0, pos 0x4F0, min 0x4FC, max 0x500, range 0x504, fov0 0x508, fov1 0x50C, x60 0x510, ang 0x512, type 0x514,
x63 0x515, zoom 0x516: G's CAMD_PCH field for field. Readers: cam_init_sub_pchngr, cam_sub_pchngr, GetPachingerInfo
(sub_main 0x350B70 calls it), sub_0x31D660 (lobby 0x70D8B0: sets pos, x60, ang). MH2's cam_sub_pchngr picks its mode from
sub_0x31D550 (3 when game_w+0x41, 1 for acts 0x65 / 0x66, 2 when PachiTypeCheck, 0 for player kinds 1 / 5, else -1), so this
slot covers more than the cannon (guess).

### player EX block (0x520, 0x60; G: sl[3].d.ex)

| off | size | G field |
|---|---|---|
| 0x520 | 0x10 | fish (CAMFISH: step 0, on 4, fish 8) |
| 0x530 | 12 | zoom.pos |
| 0x53C | 12 | zoom.tar |
| 0x548 | 1 | zoom.step |
| 0x54C | 4 | zoom.npc |
| 0x550 | 4 | zoom.x24 (manual height, 120) |
| 0x554 | 4 | zoom.x28 (manual distance, 300) |
| 0x558 | 4 | zoom.x2C (fov: 20 degrees for an NPC, 30 manual; G 30 both) |
| 0x55C | 2 | zoom.cnt |
| 0x55E | 1 | zoom.req |
| 0x55F | 1 | zoom.x21 (0 NPC, 1 manual) |
| 0x560 | 2 | new |
| 0x562 | 1 | new: sub_0x31D9A0 sets it (about 30 lobby callers) |

MH2's CAMZOOM is reordered (pos and tar first, step at +0x18).

### demo block (0x580, 0x90; G: sl[4].d.demo less its float roll / fov)

| off | cw off | G field (G d off) |
|---|---|---|
| 0x00 | 0x580 | eye (0x00) |
| 0x0C | 0x58C | eye_o (0x0C) |
| 0x18 | 0x598 | tar (0x18) |
| 0x24 | 0x5A4 | tar_o (0x24) |
| 0x30 | 0x5B0 | cmd (0x40) |
| 0x38..0x3D | 0x5B8 | move, pos_mode, tar_mode, pos_part, tar_part, x4D (0x48..0x4D) |
| 0x3E | 0x5BE | no (0x4E) |
| 0x3F | 0x5BF | state (0x4F); DemoCameraCheck (sub_0x31E160) R |
| 0x40 | 0x5C0 | arg (0x50; the monster for get_em_local) |
| 0x44, 0x48 | 0x5C4 | x54, x58 |
| 0x4C..0x52 | 0x5CC | ax, ay, ax0, ay0 (G 0x5C..0x62) |
| 0x54..0x7B | 0x5D4 | new: roll and fov as s16 angles (x 10430.38) with their from / to pairs |
| 0x7C, 0x84, 0x88 | 0x5FC | new, cleared at script start |
| 0x80 | 0x600 | copy of cmd (G's loop 0x44, guess) |

G's float roll / roll_o / fov / fov_o (0x30..0x3F) are gone. The script table is Demo_cam_tbl 0x41C5E0; entries below 0x80
are indexes that func_00194630 turns into a pointer (new; guess: cameras loaded with a file).

### quakes, rail, the rest (0x610-0x67F)

| off | size | G field (G off) | R/W |
|---|---|---|---|
| 0x610 | 0x14 | qk[0] (0x5BC) | set_quake_sub, set_quake_sub2, Pl_set_quake_sub, Em_set_quake_sub, sub_0x320020 |
| 0x624 | 0x14 | qk[1] (0x5CC) | sub_0x320150 (player: G's Pachinger_set_quake_sub) |
| 0x638 | 0x14 | new qk[2] | 0x3200C0 (mh2_map's "Pachinger_set_quake_sub" by shape; a monster quake for the demo view) |
| 0x64C | 0x10 | rail_t (0x5AC), rail_u (0x5B0), rail_no (0x5B4) | CamRailMove passes cw + 0x64C as the RAILPOS |
| 0x65C | 1 | cam_old (0x5E0) | |
| 0x65D | 1 | cam_no (0x5E1) | also sub_0x31F6A0 R |
| 0x65E | 2 | sw_on (0x5E2) | cam_sw_set_sub W (Psw[0]) |
| 0x660 | 2 | sw_trg (0x5E4) | Psw[2] |
| 0x662 | 2 | sw_x5E6 (0x5E6) | Psw[4] |
| 0x664 | 2 | an_ang (0x5E8) | Psw[9] |
| 0x666 | 2 | an_pow (0x5EA) | Psw[11] |
| 0x668 | 4 | reset (0x5EC) | WyvernFindPlayer W; CameraMove W 0; CameraInit W 0 |
| 0x66C | 4 | demo_arg (0x5F0) | DemoCameraRequest, Q_camera_init, CameraInit |
| 0x670 | 1 | demo_req (0x5F4) | |
| 0x671 | 1 (s8) | new: the special slot cam2view chose | sub_0x31BBC0 R |
| 0x672 | 1 | x5FC (0x5FC) | std W 1; CameraMove clears it first and runs sub_0x31F750 when set |
| 0x673 | 1 | x5FD (0x5FD) | sub_0x31F750 W (1 = camera close to the hunter) |
| 0x674 | 4 | x5F8 (0x5F8) | std W 1.0, sub_0x31F750 W 0.3; sub_0x31F710 R (game pl_move_sub, player_trans: the hunter fade, guess) |
| 0x678 | 4 | new: CameraMove's argument | sub_0x31AA30 R |
| 0x67C | 4 | unseen | |

MH2's CAMQUAKE (0x14): pos 0x00, on 0x0C, type 0x0D, time 0x0E, new byte 0x10 = range index into 0x41C820 (2000, 3000, 4000,
5000). quake_sub drops the shake to 0 past that range; inside it the fade is still 1 - distance / 2000 (as read; for ranges
over 2000 that goes below 0, so the sign flips: odd, not checked in play). sub_0x320020 is Em_set_quake_sub with a third
argument (the range index); the other setters write 0 there.

## CameraMove's argument

CameraMove now takes one word (stored at 0x678). Bit 0 skips k_HitEmCamera, bit 1 skips k_HitWallCamera (both test it through
sub_0x31AA30). Callers: game13, game2 (after pl_init) and plsel pass 1; move (sub_main 0x33D7D0) and the lobby per-frame
paths pass 0. So the frame after a stage start does not push the camera out of monsters (guess for why).

Other CameraMove changes against G: the init path reads area->no and area->type without the NULL / negative checks (MH2
assumes an area), the per-frame grid call is gone, and after View_move it runs sub_0x31F750 when x5FC is set (G: an empty
func_001DD5E0 on the PC).

## StageCamInit, default_area_data, cam_cnf_chs

- **StageCamInit (0x320580)**: as G: if CameraWork.data is NULL, build the default; then copy the data's 0x04..0x1B to
  CameraWork 0x390..0x3A7 (G 0x590..0x5A7). Only the CameraWork offsets moved (-0x200).
- **default_area_data (sub_0x3204B0)**: builds one follow-the-hunter area in cam_data_area (pointer at 0x6AE028) with the same
  header and area words as G (0x102, 1 x 1 grid of 20000, near 400, far 2400, 1.0, 0.75), then calls sub_0x320470, which copies
  the 0x100 bytes of cam_cnf_chs into the area's config (area + 0x20, data + 0x50), and stores the pointer in CameraWork 0x388.
  No stage_camera_data_tbl rows. Already in src/main/cam/camarea03.c under VERSION_MH2 (copy helper matched; mh2_map.csv calls
  0x320470 "yn_proxy_wk_load", a wrong name).
- **cam_cnf_chs (0x41C830, CAMCNF)**: fov 50 degrees, roll 0, e[0] = height 300, distance 160, target height 184, ground 80
  (G's default_area_data values). Used for std whenever the area is not type 0 and as the default area's config.
- SetAreaData (0x320600) is matched (camarea05.c, MH2's own form under `VERSION_MH2 && __MWERKS__`). CameraAreaCheck is carried.
  MH2's CamAreaAttribChk is guess sub_0x3208D0 (area flag 0x80 counts when the master hunter is in state 0 acts 0x27-0x2B as in
  G, or the new acts 0x77-0x78; helper sub_0x320840 is also called by GetWallHitBitPl). mh2_map.csv's CamAreaAttribChk at game
  0x88A650 is a weak match (0.76) and probably wrong.

## Functions, MH2 against G

same = same logic, only CameraWork offsets moved; changed = logic changed too; new = no G twin. Addresses are sub_main
(MH2) and main (G).

| MH2 | name | G | kind | notes |
|---|---|---|---|---|
| 0x31A580 | CameraWorkInit | 0x1D9700 | same | size 0x680 |
| 0x31A5A0 | Q_camera_init | 0x1D9720 | same | zoom is std block 0x420 |
| 0x31A5C0 | CameraInit | 0x1D9740 | changed | clears 3 quakes, demo_arg, demo_req too |
| 0x31A640 | SetCameraData | 0x1D97B0 | same | |
| 0x31A760 | CameraMove(flags) | 0x1D98D0 | changed | 6 slots, cam_no 2, no grid, argument |
| 0x31A9C0 | sub (demo alone) | 0x1D9B50 | same | slot 5; plsel 0xD74450 calls it; rt_cam_demo_tick's twin |
| 0x31AA30 | sub (CameraMove flags) | - | new | |
| 0x31AA40 | cam_init_sub_std | 0x1D9BC0 | changed | block layout; zoom from the block |
| 0x31AC00 | cam_sub_std | 0x1D9D60 | changed | 4024 bytes (G 3080): auto turn, lock, slot 1 hand-over, wall pull-in |
| 0x31BBC0 | sub (reset behind?) | - | new | game 0x70E060 |
| 0x31BC10 | WyvernFindPlayer | 0x1DA970 | same | |
| 0x31BC40 | BBQcamera_set | 0x1DA9A0 | same | |
| 0x31BC80 | std_cam_sw_set_sub | 0x1DAAC0 | changed | also off while slot 5 acts; player kind 10 keeps 0x88 |
| 0x31BDD0 | set_to_std_cam | 0x1DABC0 | changed | 5 -> -1 (G: 4) |
| 0x31BE10 | sub (lock turn) | - | new | |
| 0x31BF80 | sub (water) | func_001DACA0 | same idea | |
| 0x31C0E0 | sub (ground under, retry 150 up) | - | new | |
| 0x31C160 | sub (weapon state) | - | new | |
| 0x31C240, 0x31C250 | subs (read std +0x7A / +0x7C) | - | new | |
| 0x31C260, 0x31C270 | slot 1 init / move | - | new | |
| 0x31C7E0 | cam_init_sub_stg | 0x1DAF40 | same | |
| 0x31C800 | cam_sub_stg | 0x1DAF60 | changed | block layout, shared blend helpers |
| 0x31CF40 | cam_init_sub_pchngr | 0x1DB790 | same | |
| 0x31CF90 | cam_sub_pchngr | 0x1DB7E0 | changed | mode from sub_0x31D550 |
| 0x31D440, 0x31D4C0, 0x31D550 | subs of cam_sub_pchngr | - | new | motion list at hunter +0x29C; matrix; mode |
| 0x31D610 | GetPachingerInfo | 0x1DBDE0 | same | |
| 0x31D660 | sub (set cannon pos / angles) | ? | unknown | lobby 0x70D8B0 |
| 0x31D6B0 | cam_init_sub_playerEX | 0x1DBE30 | same | |
| 0x31D6D0 | cam_sub_playerEX | 0x1DBE50 | same | |
| 0x31D730 | cam_plEX_fishing | 0x1DBEB0 | same | |
| 0x31D7E0 | fish_cam_sub | 0x1DBFB0 | same? | |
| 0x31D900 | NPCZoomInCameraRequest | func_001DC0D0 | changed | fov 20 degrees for an NPC |
| 0x31D980 | NPCZoomInCameraCancel | 0x1DC140 | same | carried (mhg camd_p1) |
| 0x31D990 | NPCZoomInCameraCheck | 0x1DC150 | same | carried |
| 0x31D9A0 | sub (zoom +0x32) | - | new | |
| 0x31D9B0 | sub (zoom done check) | func_001DC1F0 | changed | 20 frames (G 15) |
| 0x31DA10 | sub (zoom ease) | ? | | |
| 0x31DAA0 | cam_plEX_zoom | 0x1DC250 | changed | |
| 0x31DF70 | cam_init_sub_demo | 0x1DC640 | same | |
| 0x31DFA0 | cam_sub_demo | 0x1DC670 | same | |
| 0x31E080 | DemoCameraRequest | 0x1DC740 | changed | clears sl[5].x73 |
| 0x31E0A0 | sub (request, skippable) | - | new | sets x73 = 2, then sub_0x33E490 |
| 0x31E0C0 | sub (restart if another runs) | - | new | |
| 0x31E130 | DemoCameraCancel | 0x1DC780 | same | |
| 0x31E160 | DemoCameraCheck | 0x1DC7B0 | same | |
| 0x31E170 | sub (x73 bit 1) | - | new | |
| 0x31E190 | point_camera | 0x1DC7D0 | changed | angles as s16, table indexes |
| 0x31E340 | get_em_local | 0x1DC900 | same | stop flag in the head |
| 0x31E380, 0x31E4B0 | cmd_set_pos, cmd_set_tar | 0x1DC940, 0x1DCA70 | same | |
| 0x31E5D0 | cmd_copy (guess) | 0x1DCBA0 | changed | copies the s16 pairs |
| 0x31E6C0 | get_angle | 0x1DCC30 | same | |
| 0x31E770 | cmd_cam_move | 0x1DCCD0 | changed | 2136 bytes (G 880) |
| 0x31EFD0 | point_cam_sub | 0x1DD040 | changed | |
| 0x31F4E0 | cam2view | 0x1DD3C0 | changed | 3 quakes, 6 slots, 0x671 |
| 0x31F6A0 | sub (cam_no == 2) | - | new | |
| 0x31F6C0 | PachingerCamChk | 0x1DD550 | same | |
| 0x31F710 | sub (x5FC / x5F8 reader) | ? | | game pl_move_sub, player_trans |
| 0x31F750 (+0x31F7F0, 0x31F880, 0x31F9F0) | camera close to the hunter | func_001DD5E0 | new on the PC | |
| 0x31FBA0, 0x31FD70 | sub (keep the hunter's head in view, guess) | gcam_lift? | new | joints 20 and 2 |
| 0x31FE70 | set_quake_sub | 0x1DDA10 | same | +0x10 = 0 |
| 0x31FEC0 | set_quake_sub2 | 0x1DDA60 | same | carried (camg) |
| 0x31FF00 | Pl_set_quake_sub | 0x1DDAA0 | same | |
| 0x31FF90 | Em_set_quake_sub | 0x1DDB30 | same | |
| 0x320020 | sub (Em quake with range) | - | new | |
| 0x3200C0 | sub (quake 2) | - | new | not G's Pachinger_set_quake_sub |
| 0x320150 | Pachinger_set_quake_sub | 0x1DDBC0 | same | |
| 0x3201E0 | quake_sub | 0x1DDC50 | changed | range table |
| 0x320370 | sub (pad allowed) | - | new | game_w+0x41, Game_clear_ck, hunter +0x81E, Cockpit_menu_chk |
| 0x3203E0 | cam_sw_set_sub | 0x1DDE10 | changed | uses 0x320370 |
| 0x320470 | default_area_data's copy | - | new | matched |
| 0x3204B0 | default_area_data | 0x1DDEB0 | changed | |
| 0x320580 | StageCamInit | 0x1DE090 | same | |
| 0x320600 | SetAreaData | 0x1DE100 | same | matched |
| 0x320720 | CameraAreaCheck | 0x1DE330 | same | carried |
| 0x320840, 0x3208B0, 0x3208D0 | CamAreaAttribChk and helpers (guess) | 0x1DE450 | changed | |
| 0x320920 | Area_XZ_Check | 0x1DE4A0 | same | |
| 0x320A10, 0x320AB0, 0x320B70 | blend helpers | inline | new | |
| 0x320BB0 | sub (yaw from two points) | inline | new | |
| 0x320C10 | sub (auto turn amount) | - | new | |
| 0x320DA0, 0x3210C0 | subs (weapon actions, reset request) | - | new | |
| 0x320D80 | fishing_cam_chk | 0x1DBF90 | same | carried |
| 0x321190 | manual_cam_chk | 0x1DA9E0 | changed | refuses while std +0x7A >= 0 |
| 0x321270 | sub (hunter +0x680 not 0xB7 / 0xB3) | - | new | game callers |
| 0x3212D0..0x322D60 | GetPanTarget .. Cardano (rail, spline math) | 0x1DE5xx.. | same | most carried; CamRailMove passes cw + 0x64C |
| 0x323040 | k_HitWallCamera | 0x1E0400 | changed | GetWallHitBit2, offsets at 0x41CE40 |
| 0x323160 | k_HitEmCamera | 0x1E05A0 | changed | flag bit 0 |
| 0x3239B0 (+0x323C90, 0x323CA0) | QuestClearCameraRequest | 0x1E0DB0 | changed | per-kind table 0x41CE60 |
| 0x323CB0..0x324070 | demo request wrappers | camq0 / camq1 family | changed | renumbered demo cameras; not mapped one by one |
| (gone) | Get_cam_grid_XZ | 0x1DE220 | - | |
| 0x3ACE10 | PachiTypeCheck | 0x1DBD10 | changed | moved out of the camera |
| 0x335140 | LoadCameraData | 0x11F1E0-ish | | camera_data_tbl 0x4224E0 (199) |
| 0x377800, 0x377B20, 0x377B30 | View_move, RollView, SetAngleOfView (by their callers) | | | |

## What already sits on MH2 addresses with a shifted CameraWork

The carried camera functions were matched by aliasing CameraWork so that G's offsets land on MH2's:
config/mh2/c_aliases.txt `sub_main CameraWork 0x005AF54E` (= 0x5AF420 + 0x12E: G's zoom.req 0x430 lands on MH2's 0x55E),
config/mh2/c_renames.txt `CameraWork_mh2_005AF474` (set_quake_sub2: G qk[0].on 0x5C8 -> MH2 0x61C) and
`CameraWork_mh2_005D7FE4` (camd_151). With a real MH2 CAMW these go back to CameraWork = 0x5AF420. pc_names.txt already has
0x5AF420.

## Port plan

Goal: shared C on MH2's layout under VERSION_MH2 (the PC's MH2 build is VERSION_MHG + VERSION_MH2, so MH2 blocks must be tested
first), byte-checkable with tools/mh2_diff.py, without breaking G (VERSION=mhg must stay byte-identical) or MH1.

1. **Header (include/cam.h).** Under VERSION_MH2 a CAMS of 0x80 (head as above) and a CAMW of 0x680 with the blocks as named
   members (std, aim, stg, pch, ex, demo, qk[3], rail, tail). MH2 forms of CAMD_STD (0x80), CAMD_STG (0x40), CAMZOOM, CAMD_DEMO
   (0x90), CAMQUAKE (0x14); CAMD_PCH and CAMFISH as G. Offset checks with a negative-array-size typedef per field the tables
   above give. Accessor macros so one source serves all three: `CW_STD(cw)` (MH2 &cw->std, G &cw->sl[0].d.std), `CW_STG`,
   `CW_PCH`, `CW_EX`, `CW_DEMO`, `CW_ZOOM_LV(cw)` (MH2 cw->std.zoom, else cw->zoom), slot numbers `CS_STD 0`,
   `CS_STG MH2V(1, 2)`, `CS_PCH MH2V(2, 3)`, `CS_EX MH2V(3, 4)`, `CS_DEMO MH2V(4, 5)`, `CAM_SLOTS MH2V(5, 6)`, and the
   head fields that moved (step, rate, roll_f, fov) as names that exist in both. G and MH1 headers unchanged.
2. **Move the G camera C onto the macros** (no logic change; G and MH1 rebuilds must stay identical: full check). Files:
   camm.c, cam.c / cam_nm.c, camb.c, camc.c, camd.c, camg.c, camp.c, camarea_nm.c, camq0.c, camq1.c, src/mhg/main/cam/camd_g.c,
   plus rt_cam.c and rt_village.c (they hardcode G slot 4 offsets: 0x480 + 0x66, 0x5F4, 0x90 + 0x4E). After this the PC's MH2
   build runs **G's logic on MH2's layout** with an empty slot 1 (init / move = act 0) and cam_no 2 for stage areas. Run the
   hidden walk and stage 120 tests; nothing should look different.
3. **Byte-check the small MH2 functions** under VERSION_MH2 (straight from the tables, each a few lines): CameraWorkInit,
   Q_camera_init, CameraInit, StageCamInit, default_area_data (camarea03.c's MH2 form then stores to the real 0x388),
   set_to_std_cam, WyvernFindPlayer, BBQcamera_set, cam_init_sub_* (6), cam_sub_playerEX, GetPachingerInfo, PachingerCamChk,
   DemoCameraRequest / Cancel / Check, sub_0x31E0A0 / 0x31E0C0 / 0x31E170, NPCZoomInCameraRequest, sub_0x31D9A0 / 0x31D9B0,
   the quake setters (5) and quake_sub, cam_sw_set_sub + sub_0x320370, CamRailMove (cw + 0x64C), the blend helpers 0x320A10 /
   0x320AB0 / 0x320B70, 0x320BB0, sub_0x31AA30, sub_0x31BBC0, sub_0x31F6A0, sub_0x31F710. Then re-carry: CameraWork alias back to
   0x5AF420, drop CameraWork_mh2_005AF474 / 005D7FE4 and the 0x5AF54E alias (tools/mh2_carry.py; mh2_rebuild + mh2_check).
4. **CameraMove(flags) and cam2view** in MH2 form (6 slots, cam_no 2, no grid, 3 quakes, 0x671, sub_0x31F750 hook). Host: the
   PC's callers of CameraMove (game2 / game13 / move / lobby C, and rt_cam_tick) need the argument; on MH1 / G the C keeps
   `void CameraMove(void)` (a macro or a VERSION_MH2 prototype).
5. **std camera**: cam_init_sub_std, std_cam_sw_set_sub, manual_cam_chk, then cam_sub_std with its helpers (0x31BE10, 0x31BF80,
   0x31C0E0, 0x31C160, 0x320C10, 0x320DA0, 0x3210C0), k_HitWallCamera 0x323040 and k_HitEmCamera's flag. Big: a near-match
   (cam_std_mh2_nm.c) is fine. Needs sub_0x330570 (the wall line test, not looked at), func_734640, and hunter work offsets
   +0x390 (state), +0x3AC, +0x568, +0x588, +0x81E, +0x821, +0x36C, which come from MH2's PLW view (include/mh2_plw.h).
6. **slot 1** (0x31C270) as new C, then cam_sub_stg, cam_sub_pchngr (+0x31D440 / 0x31D4C0 / 0x31D550), cam_plEX_zoom,
   point_camera / point_cam_sub / cmd_cam_move / cmd_copy (s16 roll and fov; Demo_cam_tbl 0x41C5E0 and func_00194630),
   QuestClearCameraRequest and the request wrappers, sub_0x31F750 family, sub_0x31FBA0 / 0x31FD70.
7. **rt_cam.c** (alongside steps 2 and 4): slot loops to CAM_SLOTS; the trace prints the std block and slots 2..5 (MH2);
   rt_cam_demo_tick uses CS_DEMO (MH2's own twin is sub_0x31A9C0); rt_cam_tick calls CameraMove(0), and CameraMove(1) on the
   first tick after rt_cam_init (as game2 / game13 do); LoadCameraData stays (camera_data_tbl 199 rows already in pc_data.txt);
   Get_cam_grid_XZ is no longer called on MH2, so the divide-by-zero guard for stage 110 can go. View_move / RollView /
   SetAngleOfView stay host (MH2 0x377800 / 0x377B20 / 0x377B30).

Steps 1 to 3 are mechanical and checkable; 4 onwards changes what the player sees (auto turn, lock, slot 1), so each should be
tried in hidden runs with RT_CAM_TRACE before the next.

## Open / guessed

- What slot 1 is for (aim camera guess), the player kinds 1 / 5 / 10 in sub_0x320DA0, and what +0x7C / func_734640 means
  (target lock guess).
- sub_0x330570 (wall line test with radius), sub_0x33E490 (called by the skippable demo request), func_00194630.
- 0x18-0x23 / 0x48-0x5F of the slot head, 0x58 / 0x60 / 0x73 of the std block, 0x24-0x27 of the stage block, 0x7C-0x88 of the
  demo block: no reader seen.
- The quake fade beyond 2000 (sign flip) as read.
- Whether MH2's camera files really have no grid (one read of a camera file would settle it).
