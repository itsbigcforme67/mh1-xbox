# MH Dos (MH2, JP 1.04) on PC, then Xbox: where it stands and the plan

- **Written:** 2 Nov 2026, first step. The owner's goal of that day: MH Dos gets PC and Xbox ports like MH1 and G
  (docs/DECISIONS.md, "MH Dos gets PC and Xbox ports").
- **Community repo:** mh2-decomp was not opened.
- **Numbers:** from the tools named. Scripts not named here were throwaway reading aids. Guesses are marked.
- **Newer notes:** sections 1-5 describe the state of 2 Nov. The four agents of 2 Nov and the CODE and HUNT agents of 3 Nov
  moved things on: docs/mh2/data.md, structs.md, quest.md, platform.md, code.md, hunt.md. Section 6 says what is done.
  Where a newer note disagrees with sections 1-5, the row is marked **Conflict** or **Later**.

## Where it stands on 3 Nov 2026

- `--quest 1014` now takes G's quest path on MH2's own Quest_start set-up: stage 110, the hunter in the game camera, HUD
  drawn. It stops at four blockers (frame.h offsets, ground test, quest flow, monster tables): docs/mh2/hunt.md.
- Monsters are off by default on MH2 (`RT_MH2_EM=1` turns them on; the run then stops in em_status_init).
- Still no game code past that runs. Xbox: nothing done.

## Plain summary (2 Nov 2026)

- **It reaches the real title screen.** `tools/build_pc_mh2.sh` builds a binary from the same game C as G. Run hidden, it opens
  DATA.BIN, loads MH2's own overlays and textures and draws the MH2 title art, PRESS START BUTTON and the copyright line. Seen
  in a hidden `--shot`, tick 150.
- **That is not the game running.** The title comes from about 150 lines written from MH2's code (src/pc/title_mh2.c). Sound,
  card, movie and the press of Start are left out, and pressing Start does nothing. None of the 3,486 linked C functions had
  run past fades, fonts and sprites on 2 Nov. (**Later:** see "Where it stands on 3 Nov 2026" above.)
- **The file formats are the good news.** Models, skeletons, motion and collision load with the readers we have, only textures needed
  a new reader, and the font file is identical to G's.
- **The code is the bad news.** MH2 rewrote the boot, the menus and the character screens, and our run-set is G's. See the table
  in "How big the gap is". A first town or a first quest is not one more step; it is the size of the G port or bigger.
- Xbox: nothing done. It should follow through the same platform layer, as it did for G, but no MH2 memory number exists yet.

## 1. What was built

    tools/build_pc_mh2.sh                  # build/mh2/pc/mhview_2, in its own tree build/mh2/pctree
    build/mh2/pc/mhview_2 disc/mh2 --boot --shot build/mh2/x/s.png --time 5     # always hidden, as G
    build/mh2/pc/mhview_2 disc/mh2 --quest 1014 --shot x.png    # drawing check: stage, monsters, hunter (docs/mh2/platform.md)

| piece | what | status |
|---|---|---|
| tools/build_pc.sh | `VERSION=mh2`: G's source lists (GFORM), plus -DVERSION_MH2. `PC_KEEP_GOING=1` lists all compile errors in one run | every file compiles and links |
| tools/mh2_pcmeta.py | symbols per address space, pointer words by scan, MH1 to MH2 address map, into build/mh2/pc | see "unknown" below |
| tools/mh2_datamap.py, config/mh2/data_map.txt | 1,499 more data names from aligned code in named function pairs | on the 641 names placed before: 640 agree, 1 differs. 930 of the 1,499 rest on one vote |
| tools/pc_abs.py, gen_rt_meta, gen_rt_tables, gen_rt_auto | mh2 branches. A name that is data in MH1 or G but unknown in MH2 becomes zeroed data, not a function stand-in (fade_w crashed that way) | MH1 and G branches unchanged |
| src/pc/fmt | `fmt_afs_open_sub`, `fmt_afs_find_mwo`, raw files, TIM2 in `fmt_apx_decode` | only used by MH2 |
| src/pc/title_mh2.c | MH2's boot as far as the title, from its asm, not byte-checked | stand-ins are logged at run time |
| src/pc/rt | `rt_ver_mh2()`; `rt_ver_mhg()` is true for MH2 too. rt_2d is 640 wide on MH2 | |

The viewer skips all its host scaffolding on MH2 (`goto mh2_boot`): that quest and stage set-up reads tables MH2 has no names for.

## 2. What carries over

**Unchanged and working on MH2 now:** build pipeline, GL backend, 2D draw and record/replay, font renderer, Meltw, link files, task
scheduler, fade, hidden run and crash log, the `--shot` path. **Should carry, not run yet:** audio mixer, ADX, VAG, MOMO sound packs
(version differs, below), SFD movie player, pad, saves, debug menu, profiler, lockstep referee, Xbox platform layer (src/pc/xbox).

**Does not carry:** everything that assumes G's addresses or file numbers. Sites seen: hard G file numbers (font had 0x9AE, MH2 5906),
`rt_addr(0x25F180)` in rt_font.c, 699 literal addresses in game C, of which 35 go through `MH2V`. tools/pc_abs.py moves the rest by name, and
where MH2 has no name the address stays and crashes when used. There are 371 C files with VERSION_MHG blocks; literals inside them
are G addresses that pc_abs reads as MH1 ones. How many of those are wrong is unknown.

`rt_ver_mhg()` true on MH2 is a stand-in decision: it takes G's field offsets in rt_player, rt_village and others (User_data 0xC1C, 10 players,
225 weapon models), which MH2 probably changed. Every use needs a look before town. User_data: done on 3 Nov for rt_dbg, rt_player
and rt_flow (UDMO maps G's offsets to MH2's: worn equipment 0x2090, pouch 0x2040, gold 0x2984; armour one byte per 12); not run in a game.

## 3. Data formats against G

Checked by running the PC readers on MH2 files (a throwaway test against src/pc/fmt), by file headers, and by comparing with disc/mhg. DATA.BIN and SLPM_662.80 in disc/mh2 are
byte-identical to those inside the ISO named English v1.03 in that folder, so the ISO is what holds the streams below.

| thing | G | MH2 | evidence, status |
|---|---|---|---|
| Archive | AFS_DATA.AFS, 3,347 files | DATA.BIN = AFS of 2 AFS: entry 0 6,650 data files with names, entry 1 12 overlays with no names | works. File numbers in code are entry 0 indexes (0x42F1A0 table: 6636 = demo_tex.bin, 6638 = init_tex.bin) |
| Streams | AFS00, AFS01 | AFS00 309 ADX, AFS01 414 sound packs, AFS02 13 SFD movies | only in the ISO; not read by the port yet. Install must carve them. **Later:** install, ADX and MOMO v3 decode done (docs/mh2/platform.md); AFS02 movies not read |
| Overlays | packed | stored raw, 12, by MWo3 name | works. gm_sub, plsel, lbguild are new modules with no loader in the runtime yet |
| Compression | Meltw | Meltw; some files stored raw | works. Which files are raw is **guessed**: `fmt_afs_load` returns raw when a file does not parse as Meltw. softkey1.tm2 starts like Meltw and fails: not looked into |
| Textures | APX | TIM2 version 4, one picture, one mip, 4/8/32-bit | new reader; 1,695 sampled pictures fit these shapes. Title, sprites and font-sized art seen. Alpha doubled (GS 0x80 = opaque). The (C) sprite still shows a white box, so one case is wrong. **Conflict:** docs/mh2/platform.md says alpha is 0-255 in the files, not doubled; commit c2671f0 ("TIM2 alpha, (C) box fixed") is later than this table |
| Other textures | | MOMO `.tex`, nested AFS of TIM2 (endroll, gallery) | not read. MOMO texture layout unknown |
| Models (AMO) | yes | same container. Many files carry header word 0x040C1B00 where G has 0x020B1C00 | reader returns OK on 6 samples, part counts sensible. **Not drawn yet**, so vertex layout is unproven; that word changing suggests a difference. **Later:** docs/mh2/platform.md: the 0x040C1B00 models draw with the existing reader (the word does not change the layout) |
| Skeleton (AHI) | yes | same | reader OK on 6 samples, bone counts sensible |
| Motion (tbl) | yes | same table shape; plcom_tbl.bin stored raw | fmt_aan_load: em01 226 ok, w00 72, plcom 240, 0 bad. Not played. MH2 splits the village and town motions into lbcom_off / on / coco |
| Collision | lwgNNN.bin | same, 121 files | ground lookups hit on lwg004 |
| Stages | 2-entry link | st*_amh.bin mostly 3 entries; the third entry's form varies | not read. Day/night variants st131_a, _b, 1a/1b/1c |
| Hunter parts | apx | .tm2, new head and haircut sets | viewer builds `.apx` names: needs changing |
| Quests | mNNNNN.mib, header 0x48, Meltw, 245 files | mib.afs: nested, 1,974 files, m00001d0..n2 (day/night x 3), not compressed, first word 0x74 | **new header, not parsed.** First entry 9,328 bytes. What d/n and 0-2 mean is a guess from the names. **Later:** header partly read (docs/mh2/platform.md, docs/mh2/hunt.md Quest_start) |
| Monster programs | | em_cmd.afs, 53 `.hig` files; st*cmd.bin | not parsed |
| Text | Shift-JIS in overlays and .mib | strings seen in select.bin's data the same way; quest text in .mib not looked at | font s20rm04.bin is byte-identical to G's (780,800 bytes), so G's font work carries. English: nothing done; the ISO's patch belongs to its translators |
| Sound packs | MOMO v4 | MOMO v3 | fmt_snd_open checks only the magic; not tried on MH2. **Later:** v3 read and one note decoded (docs/mh2/platform.md) |
| 2D coordinates | 512 wide | flps0008 scales x by frame width / 640, so 640 wide | read from the code and confirmed by the picture. Only flps0008 was read; rt_2d now treats all prims as 640, a **guess** for the rest. G-form screens that draw at 512 will come out narrow |

Family counts against G (same name, same stored bytes): 994 of 1,942 shared names. Monster models 36 of 45, weapons 186 of 225, stages 1 of 83.

## 4. How big the gap is

**The linked binary** (from build/mh2/pctree/build/pc, 2 Nov):

| | count |
|---|---|
| C functions linked from game objects | 3,486 |
| function stand-ins generated | 344 |
| data tables imported by name | 1,340, of which 933 have no MH2 address (zeros). **Later:** 584 after the DATA agent (docs/mh2/data.md) |
| other data names zeroed | 458 |
| pointer words found by scan | 39,809 |

**Those 3,486 functions by what MH2 did to them.** Size is MH2's where a twin exists, gcc's otherwise.

| group | no twin named | edited, score < 0.9 | edited >= 0.9 | shape | identical/absolute | rewritten |
|---|---|---|---|---|---|---|
| main + game | 813 (505,565) | 465 (281,532) | 442 (214,240) | 383 (79,132) | 291 (57,232) | 21 (3,604) |
| lobby | 596 (158,414) | 176 (65,044) | 138 (27,956) | 72 (14,020) | 40 (4,332) | 3 (1,724) |
| select | 25 (17,802) | 7 (3,064) | 7 (2,496) | 1 (100) | 6 (1,716) | 0 |

So about 10% of the linked functions are the same code as MH2 and 13% more have the same shape. "No twin named" mixes near-match helpers and host stand-ins with real gaps: an upper bound. Edited functions may be fine or not; `edited` below 0.9 is about one name in six wrong (docs/mh2.md).

**What each stage reaches in MH2's own code** (`jal` call graph from each start; bytes, each stage includes what it shares with earlier ones). Pointer calls and jump tables are not
followed, so these are lower bounds: Local_main alone reaches 22 functions because the town runs on function tables. "C" = matched C exists.
**Later:** docs/mh2/data.md redoes this with game and gm_sub loaded together and with pointers followed: the game task reaches
6,197 functions by jal alone, not 1,446, because sub_main calls straight into game and gm_sub.

| from | functions | bytes | C | identical/absolute/shape | edited | no counterpart |
|---|---|---|---|---|---|---|
| title tasks (select 0x6B5CF0, 0x6B5230) | 638 | 109,860 | 9,248 | 29,324 | 27,204 | 42,108 |
| mode menu (gm_sub 0x8FAFD0) | 364 | 74,004 | 10,048 | 26,544 | 22,032 | 13,916 |
| character select (plsel 0xD60AB0) | 874 | 226,676 | 26,748 | 37,256 | 66,684 | 92,832 |
| game task (sub_main 0x3260B0) | 1,446 | 343,760 | 41,096 | 52,952 | 110,596 | 132,592 |
| quest (enemy_mv, Quest_start, hit_check, CameraMove) | 3,298 | 985,208 | 72,776 | 113,464 | 209,824 | 539,016 |

The title tasks' large "no counterpart" share looks like Sony and CRI library code from its largest entries, which the PC replaces with host code; the libraries have no marker yet, so this is not counted. The quest row is **not** library: it is game.bin. It has 1,681 functions with no counterpart. How much of that is really new and how much a function the map failed to pair is unknown.

## 5. What blocks each stage

**Title, complete (start works, opening movie, sound).** Small. Init_task's 9 steps, sound packs (select 0x6B6200, packs from AFS01), movies from AFS02, slot 0x155's mark. Tamper check: Init_task step 1 jumps to code inside sub_main's data at 0x4901B0, hand-written asm that hashes (multiply by 0x21) and compares with a constant; two logo strings at 0x490100 and 0x490130 read as zeros on disc, so something fills them at run time (**guess**: the check). The PC should skip it, but what it writes must be understood, because the rating screen strings come from there.

**Mode menu and hunter (gm_sub, plsel).** MH2 has new tasks where G has Select_task and Edit_task; 98 of gm_sub's 128 functions and 165 of plsel's 179 have no name. The runtime needs gm_sub and plsel loaded at 0x8FAE00 and 0xD60200: today there are only game, lobby, select. **The overlay table is the first runtime design change** (rt_mem.c `rt_addr`, rt_set_*, LOAD groups in tools/mh2_pcmeta.py; which load together is still an assumption from addresses).

**Save data.** User_data is laid out (3 Nov, docs/mh2/structs.md); MemcardWork is not (base 0x6AF100, 0xF4 bytes). A hunter cannot exist without both.

**Town.** Local_main 0x7B97B0 (edited, 0.82). lb_sys, CW and Clear_lobby_ram were the G blockers and will be again. Stage files with a third entry; the village is split in cocot/town models with lbcom_* motions.

**Quest.** quest_w is laid out (3 Nov); the .mib 0x74 header is partly read (docs/mh2/platform.md); em_cmd.afs, then the monster and player tables. PLW and EMW exist (generated, with guesses listed in docs/mh2.md). `enemy_mv` and `em_move` moved to gm_sub, so monster code now spans two overlays. The host's hunter and monster models are G's; MH2 armor adds new parts.

## 6. Ranked work for the decompile agents (for the port, not the percentage)

**Done by the four agents of 2 Nov 2026, merged 3 Nov** (notes: docs/mh2/data.md, structs.md, quest.md, platform.md):
- Item 1, data names: tools/mh2_datamap.py has the pointer, content, chunk, layout and `c` methods with a confidence column; 7,084 names
  placed high or medium. Tables the PC build cannot place: 584 of 1,340 (was 933). Item 8, the pointer call graph: tools/mh2_callgraph.py,
  docs/survey/mh2_reach.csv. The game task and the quest are one graph once game and gm_sub are loaded.
- Item 2, layouts: quest_w, User_data, net_common_w, lb_sys and PitMenu laid out (config/mh2/layouts.txt, include/mh2_layouts.h), with
  QWO, UDMO, NCO, LBM, PMO for raw offsets (about 1,000 uses). Bases and sizes proven by memsets in config/mh2/pc_names.txt (also game_w,
  system_w, shell_work, MemcardWork, CameraWork). Still open: CameraWork (MH2 restructured it), MemcardWork, system_w, ConnWork, CW.
- Item 9, quest core: next_cmd_search, cmd_target_kind_set, target_kind_set and em_cmd_ck byte-matched; Em_Dmg_Sys a near-match the PC
  links. em_cmd's handlers are named by opcode (config/mh2/func_fix.txt): no opcode moved. Item 7: 0x3A2E30 (the quest's state machine)
  and 0x3A9230 (a quest event script, new in MH2) are near-matches, not linked.
- Item 10, platform: gm_sub and plsel load as extra images; `--model` and `--quest` draw MH2 models, stages, the quest's monsters from the
  .mib and a hunter; random-start stages; install from the disc image with the AFS00-02 streams; ADX and MOMO v3 sound packs decode.
  None of this runs game code: they are drawing and format checks. The host's User_data reads go through UDMO.

What is left of the list, in order for the `--quest` hunt: the .mib header beyond what docs/mh2/platform.md has, the 114 missing tables
on the one-monster quest path (docs/mh2/data.md), CameraWork, em01_senkai_sub and the rest of item 9, then a game task that runs.

1. [Done 2-3 Nov, DATA] **Data names first.** 933 of 1,340 tables are zeros (584 after DATA), and it is cheap. Extend tools/mh2_datamap.py the way tools/mhg_datamap.py goes (pointer words in and out, content, layout), add a confidence column, and check its 930 single-vote names. Unlocks every stage.
2. [Partly done, STRUCTS: quest_w, User_data, PitMenu, lb_sys] **Struct layouts the runtime touches:** User_data, quest_w, MemcardWork, system_w, CameraWork, PitMenu, lb_sys and CW. docs/mh2.md section 6 item 1 lists most. Each one fixes C already linked.
3. [No progress recorded] **Name the task chain:** the tasks Tsk_Execute starts (select 0x6B5230 and 0x6B5CF0, gm_sub 0x8FAFD0, plsel 0xD60AB0, lobby 0x7C90C0 and 0x7CA500, sub_main 0x3260B0). Real names, then C, matching where cheap. This is the skeleton of the game flow.
4. [No progress recorded] **The fl layer's edited primitives:** flSetRenderState, flPS2SendRenderState_ALPHA, flps0004, flps0005, flps000C, flExecuteClay, font_print_sp, flfntPrintf. Only to learn what changed from G: the host stands in for them, and 640-wide coordinates may not be the only change.
5. [No progress recorded] **gm_sub's menu** (0x8FB700, 0x8FB3E0, 0x8FBD90, mode_sel_exit) and sub_main 0x377800.
6. [No progress recorded] **Character screens:** sub_main 0x37EBB0, 0x37B570, 0x3753B0, 0x37CB10, 0x37DC60, 0x356380, 0x318190 (all no counterpart), Pl_reg_calc, Get_equip_value, yure_move_hair.
7. [Partly done: 0x378A30 matched 3 Nov, 0x3A9230 and 0x3A2E30 near-matches] **Game task:** 0x3A9230, 0x378A30, 0x3A2E30, 0x33E730, then edited timer_calc_sub_pl, SpritePut, GetWallHitBitPl/Em, PushAdjust3, GetGroundHitStatusAreaEm, disp_whole_map, item_stock_mv.
8. [Done 2 Nov, DATA: tools/mh2_callgraph.py] **The call graph with pointers.** Follow function tables in data (the pointer words exist now) so the town and monster numbers in section 4 become real. Do this before planning town work.
9. [Partly done: next_cmd_search, cmd_target_kind_set, target_kind_set, em_cmd_ck, em01_senkai_sub, game 0x80D270, 0x75FBF0 matched] **Quest core by callers:** next_cmd_search (49 callers in the quest set), em01_senkai_sub (39), cmd_target_kind_set (33), game 0x80D270 (30), Em_Dmg_Sys (29), target_kind_set (27), game 0x75FBF0 (21), then em_cmd_ck, shell08_m, Pl_damage_sub, hit_hit_sub_em.
10. [Mostly done 2 Nov, PLATFORM; AFS02 movies and the SE table open] **Platform side, no decompiling:** overlay table, install and disc check for DATA.BIN plus the streams, sound packs v3, MOMO textures, AFS02 movie table, stage third entry, AMO draw test. One model on screen settles the 0x040C1B00 question cheaply; do it early (a `--model` mode or a stage in the viewer).

Order for a first town: 10 (overlays), 1, 2, 3, 5, 6, 7, then town. Order for a first quest skipping town, as G's `--quest` did: 10, 1, 2, then the .mib header and items 7 and 9. **A `--quest` shortcut got G to a hunt before the village worked.** It may be the faster way to learn what is wrong here too, and it does not need gm_sub's menu or plsel's screens. That is a suggestion for the owner, not tested.

## 7. Size and how much is unknown

Only numbers with evidence: section 4. No time estimate, because the base rate does not transfer. G's plan (docs/mhg.md section 4) counted about half of its run-set as identical or shape; for MH2 the same count is about 23% (section 4).

**Unknown, plainly:**
- Whether any G-form C past fade, font and sprite does the right thing on MH2. Nothing else has run.
- Whether the pointer scan is good. There is no MH2 relocation list to check against; a wrong word corrupts a table silently. It also treats any word-aligned value into data as a pointer, which is looser than mhg_pcmeta's rule.
- No main-to-lobby pointer list exists (G has a list of about 67 words); those words point into game.bin instead.
- Which overlays load together, and whether game C crosses them in new ways.
- How much of game.bin's 539,016 bytes "no counterpart" is new code.
- MH2's memory use on Xbox. G already needed work to fit in 64 MB; MH2's data is 2.7x G's archive. An Xbox build was not tried.
- Whether sprites other than flps0008 scale the same way.
- How much of offline play depends on code the port leaves out on purpose (network, DNAS).

## 8. Rules that came out of this

- Never put a `.c` under src/select, src/main, src/game or src/lobby for the PC only without a guard: tools/build.py compiles them with MWCC for MH1. `src/pc/` is skipped. This broke the MH1 build once on the way, and the rebuild check caught it.
- `-DVERSION_MH2 -DVERSION_MHG` compile together for everything the PC links.
- Check MH1 and G bytes after touching shared C: a file edit there is not MH2-only. f_reward_g.c and lb_village_nm.c were touched; both are PC-only in effect.
