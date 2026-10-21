# The lockstep referee (tools/referee.py)

The real game (the owner's disc in PCSX2) and the port play the same quest with the same pad, and the game state
is compared tick by tick. A difference points at game code the port still runs from MH1 (G), at the port's glue
(host stand-ins, imported tables, start-up order), or at emulator / platform noise. Decision: docs/DECISIONS.md
"Lockstep referee" (17 Oct 2026). Built by agent D, 17-18 Oct 2026.

## Use

    tools/referee.py list                     the scenarios
    tools/referee.py run SCENARIO...          PS2 run, then the PC run, the comparison and the pictures (~1 min each)
    tools/referee.py pc SCENARIO [--free-rng] the PC side again against the last PS2 trace: after a PC fix, no PCSX2
    tools/referee.py compare DIR              the report again from the traces in DIR
    tools/referee.py state [--game mhg|mh1]   make the PS2 "quest taken, at the gate" save state again

Run it uncapped (PCSX2 does not start under `ulimit -v`); it takes the emulator lock (xemu.lock next to the main
checkout) for the PS2 part only and caps the PC runs at 1.5 GB itself. Everything it writes is in
build/referee/<scenario>/ and is never committed (the traces hold the save and the work areas: Capcom data):

| file | what |
|---|---|
| report.txt | the first play tick that differs, every differing field with both values, the hunter's action handler on both sides, per-field counts, the random-call comparison |
| pair_N.png, pair_N_first.png | PS2 (left, from the run's video) and PC (right) at play tick N (the first difference and ticks 100 / 300 / 600) |
| ps2.ref, pc.ref | the traces (format below) |
| ps2_rng.txt, pc_rng.txt | every ran_suu call: play tick, channel, caller |
| ps2.mp4, ticks.txt | the PS2 screen during the lockstep run and the video time of each play tick |
| user_data.bin, run.txt, pad.txt, pokes.txt | what the PC run is given |

## How it works

**Same start.** A PCSX2 save state (slot 5 in build/referee/pcsx2home) stands at the village gate with a quest
taken (G: NEW GAME, the Village Elder's first quest, made by the same scripted menus as tools/g_ref_shot.sh). Each
run loads it, pokes the scenario's weapon into the equipped item-box entry and the quest number into select_w
(G +0xD0), and departs. At the first play tick the referee keeps the PS2's User_data (the hunter's save:
equipment, pouch, rank) and Rnd_w (the game's random state, ran_suu's two Lehmer channels). The PC run starts the
same quest with `--quest N` and gets both (RT_REF_UD, RT_REF_RND). By default the PC also gets the PS2's Rnd_w at
the start of every play tick (RT_REF_RNDS, set after the PC's record of that tick), so a difference in random calls
shows in the tick where it happens instead of changing everything random afterwards; `--free-rng` turns that off.

**Same input, tick by tick.** The PS2 runs a small hook the referee writes into free EE RAM (0xFF000; checked to
be zero first): the call of flPADGetALL in ACRMain (the main loop: pad, then the task scheduler) is pointed at it.
The hook calls flPADGetALL and, once the quest runs (game_w mode 2), stops the game at the start of every frame
until the referee says go. Meanwhile the referee reads the state in one batched PINE message (tools/pcsx2/pine.py
read_many) and writes this tick's pad into the fl library's data for port 0 (flpad_adr[0]: buttons, stick
directions, stick angle and power, state words), exactly as src/pc/rt/rt_pad.c fills it on the PC from the same
pad state. The game then turns it into Psw / PLW.sw itself on both sides; Psw is compared to prove the input
matched. About 30 ticks per second.

**Play ticks.** Area loads take one tick on the PC and dozens on the PS2, so the two runs are lined up by play
ticks: ticks that start in game_w mode 2 with step 0 or 1 (game2 runs game_core). The pad script is indexed by
play tick; loading ticks get an idle pad.

**Scenario aids.** `pokes`: writes at a play tick on both sides (the PC at the same point, RT_REF_POKE). Used for an
area change the way an area exit does it (PLW+0x73A stage, +0x73C position, +0x570 facing, +0x738 = 1; game2 then
loads the area): weapons cannot be drawn in the base camp.

**Random calls.** A second hook replaces ran_suu's first two instructions with a jump to a logger (RING at
0xFD000: return address and channel), then runs them and jumps back. The PC logs the same (RT_REF_RNG, the caller's
offset from ran_suu, named from the binary's symbols). The report names the first play ticks whose calls differ and
the callers whose call counts differ over the run (callers are grouped by file, eftNN / emNN / sound, because the PC
inlines statics and uses its own sound layer).

**Compared each play tick** (state at the start of the tick): Psw (pad), Rnd_w, the hunter (stage, action group
flag14 / action flag15 / step, position (1 unit), facing, HP, stamina, motion, hit flag, area change, pouch), every
monster slot in use on either side (alive, kind, stage, mode / sub-mode / step, position (2 units), facing, HP,
motion, the nine part durabilities), game_w mode / step / stage, quest_w state bytes and kill counts. The quest timer
is only reported at the end (the PS2 clock also runs while an area loads).

**Trace format** (both sides): a header line `REF <game> plw=0xA00 em=20:<stride> gw=<size> qw=<size> rnd=4
psw=0x24`, then per tick `REF1`, play tick (-1 while loading), raw tick, PLW[0], em_work[0..19] (PS2 G stride
0xA00, PC 0xA10: G on the PC keeps MH1's EMW layout, the compared offsets are the same), game_w, quest_w, Rnd_w,
Psw[0].

## PC switches (src/pc/rt/rt_ref.c, --quest runs)

RT_REF_OUT (trace), RT_REF_PAD (pad per play tick: `bits lx ly rx ry` lines), RT_REF_UD (User_data file), RT_REF_RND
(`a,b`), RT_REF_RNDS (`ptick a b` lines), RT_REF_POKE (`ptick region offset type value`, regions plw / emN / gw / qw /
ud), RT_REF_RNG (random-call log).

## Limits

- The PS2 hook changes the running game's code in RAM (one jal, ran_suu's first two words); the save state on disk
  is untouched and every run starts from it.
- PS2 floats round toward zero and have no denormals; the port does the same by default since 20 Oct 2026 (RT_FPU_PS2,
  src/pc/rt/rt_ref.c), the host's own x87 code (libm) does not. Positions are compared with a tolerance; slow drift
  over hundreds of ticks is expected noise.
- MH1's gate state is made from the PC port's own save (tools/test_quest_loop.sh, build/show/loop/card, put on the
  PCSX2 card by run_pcsx2.sh --card-from): CONTINUE, then the Elder's quest card. G's from NEW GAME.
- The quest is chosen by poking select_w after a quest has been taken at the Elder (the departure needs a taken quest).
- `--quest` runs on the PC show a default hunter (armour, no HUD name); the comparison is not affected.
- The PS2 quest clock also runs while an area loads; it is reported, not compared.
- The pictures: the PS2 frame comes from the run's video at the tick's time (about one tick off); the PC picture
  from a second PC run with RT_SHOTS at the PC's raw tick.

## Results

18 Oct 2026, after the fixes (docs/RELEASE_G.md section 11, docs/RELEASE.md section 6, docs/REVIEW.md):

| scenario | game | play ticks | first gameplay difference |
|---|---|---|---|
| walk, sns, gs, hammer, lance, bowgun | G | 400-600 | none (sound-layer random draws only) |
| ds | G | 600 | 254: walking after a demon-mode attack, 1.3 units |
| rathian (quest 10) | G | 1100 | 203: root motion of a new motion a tick late on the PC |
| kutku (quest 2) | G | 1100 | 369: hunter and a small monster (kind 23) pushed apart a tick apart (likely the same lag, not traced) |
| mh1_walk, mh1_sns, mh1_gs | MH1 | 400-600 | none |
| mh1_rathian (quest 10) | MH1 | 1100 | 241: the same root-motion lag |
| mh1_kutku (quest 4) | MH1 | 1100 | 318: the same |

Before the fixes every scenario differed from play tick 0 (start-up order) and the fights from the first monster
contact. The referee runs with RT_FPU_PS2=1 (REF_FPU=0 turns it off): without it the Rathian fight differs at 186.
Per-scenario time: ~30-60 s for the PS2 part (lockstep, ~30 play ticks a second), ~30 s for the PC part.

20 Oct 2026 (agent D), after the second round of fixes (PS2 rounding the default, joints built in move(), the Elder's
tutorial, G's Dual Swords walk-start blend; RELEASE_G section 11 R-F8..R-F10), PC side re-run against the same PS2
traces:

| scenario | game | play ticks | first gameplay difference |
|---|---|---|---|
| walk, sns, gs, ds, hammer, lance, bowgun | G | 400-600 | none |
| rathian (quest 10) | G | 1100 | 245: a small monster (kind 16) 2 units off after a slow drift (was 203) |
| kutku (quest 2) | G | 1100 | 369: the push between the hunter and a small monster (kind 23) 2.5 units off (unchanged, cause not found) |
| mh1_walk, mh1_sns, mh1_gs | MH1 | 400-600 | none |
| mh1_rathian (quest 10) | MH1 | 1100 | 291 (was 241) |
| mh1_kutku (quest 4) | MH1 | 1100 | 319 (was 318): a body push, as kutku |


20 Oct 2026, third round (agent D; RELEASE_G section 11 R-F13..R-F17: the wall pass after body_hit, CameraMove's place,
the PS2's VU0 sine / cosine / curve formulas and rounding, the C library's atan2f, the hunter's joints without the
drawing offset):

| scenario | game | play ticks | first gameplay difference |
|---|---|---|---|
| walk, sns, gs, ds, hammer, lance, bowgun | G | 400-600 | none |
| rathian (quest 10) | G | 1100 | 286: a Velociprey 2 units off after slow drift (was 245) |
| kutku (quest 2) | G | 1100 | 804: the hunter 1 unit off after a push (the PS2 run of 20 Oct; was 622 on that run) |
| mh1_walk, mh1_sns, mh1_gs | MH1 | 400-600 | none |
| mh1_rathian (quest 10) | MH1 | 1100 | 365: red-health recovery a few ticks apart (N-R9, load-time frame counter; was 291) |
| mh1_kutku (quest 4) | MH1 | 1100 | 364: the same (was 319) |

The PS2's own runs are not always identical: a new `run` of the same scenario can start with another random state and
play differently (kutku: first difference 478 in one run, 622 in the next); compare PC changes with `referee.py pc`
against one PS2 trace. Debug aid: `REF_NODES=SLOT:COUNT:FIRST-LAST tools/referee.py run S` also records the joint
matrices of monster SLOT (-1: the hunter) on both sides; `tools/referee.py nodes DIR` lists per tick how far apart
they are (the hunter's 0.3-unit offset of R-F17 was found this way).
