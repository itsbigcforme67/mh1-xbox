# MH Dos: the hunter walks in `--quest` (WALK agent, round 3, 3 Nov 2026)

Shared game C, struct views and config/mh2 names, for blockers 1, 2, 5 and 6 of docs/mh2/hunt.md. Every MH2 offset below was read
from MH2's own function (our disassembly of the owner's disc, asm/mh2); the function is named next to it. Community mh2-decomp not
opened. "Guess" marks what was not checked.

## What runs

    RT_STEP=1 build/mh2/pc/mhview_2 disc/mh2 --quest 1014 --input "idle*30,up*120" --time 5.5 --shot x.png

The hunter stands on the beach of stage 110 for 30 ticks, then walks forward (action 0/1, char0 3) into the sea, the game camera
behind him, the HUD drawn. player_work[0] read under gdb at each RT_SHOTS tick:

| tick | pos (x, y, z) | action |
|---|---|---|
| 1, 30 | 9500, -50, 11406 | 0/0 standing |
| 60 | 9663, -50, 11688 | 0/1 walking |
| 90 | 9812, -80, 12029 | 0/1 |
| 120 | 9768, -100, 12364 | 0/1 |
| end (165) | 10010, -90, 12850 | 0/44 (in the water) |

That run needs two host changes this agent may not commit (src/pc and config/mh2/pc_*.txt are QUEST's); see "Host needs".
Without --time the viewer shoots the first frame (frames = 1), which is why a bare `--shot` shows nothing moving.

## Struct views fixed (shared C, behind VERSION_MH2; MH1 and G unchanged)

| view | file | MH2 places | read from |
|---|---|---|---|
| FRW | include/frame.h | mt 0x154, chr 0x29C, layers 0x2C0, mdl_no 0x30F, mdl 0x4EC, sub_on 0x66C, sub_chr 0x670, sub_rate 0x678 | frame_init / frame_move (sub_main 0x2E5190, 0x2E5780). One function for PLW and EMW, so both agree (the EMW offset macro said 0x674 for sub_on: wrong, an interpolation) |
| HENT | src/main/hit/f_sphr.c | st 0x390, x4D4 0x437, x59C 0x564, old 0x568, x600 0x584, x604 0x588, gat 0x660, stg u16 0x680, hitdir 0x698, x7E0 0x72C, x7E8 0x734, gkind 0x735, x95D 0x8B1, wmask 0x8B2, size_pct 0x984 | GetGroundHitStatusAreaPl/Em, GetGroundHitArea(Upper), GetWallHitBitPl/Em, HitWallPlayer, GetEyeHitLine (sub_main 0x328CD0-0x330EB0) |
| GetEyeHitLine's raw reads | f_sphr.c | stage at 0x680 (lhu) | 0x32F220. `EF(ent, ...)` on a void* or HENT does not map (MH2_OFS only knows sizes 0x970 / 0x9D0) |
| PLLIGHT, PLLC | src/main/model/light_nm.c | col 0x540; near_em 0x597, tbl 0x664 | Pl_light_init 0x333850, Pl_light_set 0x333860; 0x333640 (pl_light_change's twin, which also changed its logic: the PC uses the host stand-in) |
| EFT01_CHR | src/main/eft/eft01.c | foot0 0xE4, foot1 0xF0, x138 0xF8, x5AC 0x574, stg 0x680, x763 0x6AF, alpha 0x6E4 (headers, not seen), x7D6 0x722 | eft01_m 0x3C7D20, eft01_t 0x3C8400 |
| EFT13_CHR | eft13*.c (6 copies) | char0 0x29C, x5AC 0x574, x909 0x861 | eft13_m, eft13_set_pos, eft13_set_sub |
| SetAreaData | src/main/cam/camarea05.c | the byte-matched MH2 form (CameraWork data 0x388 ...) now builds for MWCC only; the PC takes the CAMW form, which the rest of the PC camera C uses | |
| camera slot tables | src/main/cam/camm.c | PC only: G's five slot functions | MH2's CameraMove runs 6 slots from 0x41CD90 / 0x41CDB0 (G 5); importing those made a NULL call |

## PLW / EMW layout (blocker 6)

- **Part pointers: PLW 0xD0..0x14F** (MH1 0x110..0x18F). parts_init (sub_main 0x337D30) and pl_create_model (0x338190) store the
  parts_work blocks from PLW+0xD0; the flag bytes go to 0x4C8+i (MH1 0x4E6+i), x4E4 to 0x4C6. The named words inside that array moved
  with it (work124 0xE4, work130 0xF0, work160 0x120 already had votes), so mdl148 is 0x108.
- EMW has the same -0x40 there: eft01_t, one function for hunter and monster shadows, reads the foot part pointers at 0xE4 / 0xF0.
- config/mh2/struct_map.txt has these lines; include/mh2_plw.h and mh2_ofs.h were edited by hand to agree, because
  build/mh2/probe/votes.tsv (what `tools/mh2_structs.py gen` needs) is not on this machine. A regenerate should give the same.
- Not looked at: where EMW's part flags went, and the rest of EMW 0xC4..0x154.

## Names (blocker 5 and the data map)

- tools/mh2_pcmeta.py `fold_pairs`: in the PC's symbol view only (build/mh2/pc/symbols), a placeholder function (game_func_...) takes
  its config/mh2/func_pairs.txt name when the score is >= 0.9, no other such pair gives that name and no MH2 symbol has it: 866 names.
  Function pointers in MH2 data then import as the host function of that name. config/mh2/symbols is not changed (the split and the
  carry keep their names). Pairs below 0.9 are left out: how often they are wrong is not measured.
- config/mh2/data_fix.txt (new, applied by tools/mh2_funcfix.py into config/mh2/symbols; tools/mh2_map.py runs it last):
  - **pl_area_top and data_load_ptr were swapped.** MH2 keeps MH1's heap pointers in reverse order (stage_hit_area_w 0x6AE01C down to
    app_mem_top 0x6ADFE4); pl_area_top is 0x6AE004 (carried load_lf03 reads it there; c_symfix.txt had to rename it), data_load_ptr
    0x6ADFE8 (load_stage_hit loads the hit file there).
  - Em_Taisei_Set's tables by the field each fills (MH1's order): sleep2 0x8BDB50, poison 0x8BCCB0, mahi 0x8BD180, sleep 0x8BDA00.
    The fifth (0x8BD550, EMW 0x7D4) has no MH1 name. The rows that disagreed were taken out of config/mh2/data_map.txt.
- The map's camera names look shifted by one (guess from address order and sizes): by address MH2 has init_std 0x31AA40,
  sub_std 0x31AC00, init_stg 0x31C260 (0x10 bytes, G 0x14), sub_stg 0x31C270, then pairs at 0x31C7E0/0x31C800, 0x31CF40/0x31CF90,
  0x31D6B0/0x31D6D0 and a new sixth pair 0x31DF70/0x31DFA0; the map calls 0x31C270 cam_sub_std. Not corrected.

## Host needs (src/pc, config/mh2/pc_*.txt: QUEST's)

1. config/mh2/pc_data.txt: `camera_data_tbl = 0x004224E0; // size:0x31C noptr` (199 stages; the host array is MH1's 100, so stage
   110 read a wrong file number and the camera divided by zero in Get_cam_grid_XZ).
2. src/pc/rt/rt_pl.c parts_init: on MH2 also store the part pointers into the PLW (`PU32(pl, 0x110 + i * 4)`, which RT_PLO now maps
   to 0xD0). eft01_m reads part[10] (PLW 0xF8) for the hunter's shadow: NULL crash at tick 3 without it.
3. src/pc/rt/rt_pl.c `player_work_ptr` steps 0xA00 per player; MH2's PLW is 0x970 (only player 0 is right).
4. Blocker 7's per-stage tables at MH1/G sizes are the same kind of bug as 1.

**Without 1 and 2 this branch alone crashes `--quest 1014 --time ...` at tick 3** (SIGFPE in Get_cam_grid_XZ, then a NULL part
pointer in eft01_m). Before it, the hunter never got through frame_init, so G's CameraMove returned at `pl->be_flag == 0` and the camera
code never ran (that is why main's run went on to tick 158: a guess from the code, main was not rebuilt to check). Merge with both.
The exact test patch: /tmp/claude-1000/-home-james-claude-projects/780e2c66-a32c-48a8-b1d5-3f56feafbde2/scratchpad/walk/host_test.patch
(rt_pl.c keeps its mh2_parts copy as well, for rt_player_parts).
