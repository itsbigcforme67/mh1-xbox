# Monster Hunter G (PS2, SLPM_658.69) in this repo

Owner's decision (13 Oct 2026, docs/DECISIONS.md): PS2 Monster Hunter G is the main release,
built from the same tree as MH1 JP ("One codebase, several games"; layout: "How G lives in the
repo: a VERSION switch"). This file says what carries over, how to build G, and the plan
for getting G running on the PC/Xbox platform layer. Written by agent B, 13 Oct 2026.
Numbers are measured with the tools named; estimates are marked as such.

## 1. What carries over (measured)

Details and method: docs/survey/mhg_vs_mh1.md. Per-function map: docs/survey/mhg_map.csv.

| | share of MH1 code bytes |
|---|---|
| identical in G (only call targets / global addresses differ) | 56.2% |
| same shape, some constants or struct offsets differ | 5.7% |
| edited (a counterpart >= 50% similar) | 32.5% (62% of it >= 90% similar) |
| rewritten (counterpart at a twin's call site, < 50% similar) | 0.7% |
| no counterpart found | 5.0% |

- By module, identical+shape: main 63%, game 60%, lobby 62%, yn 63%, select 32%.
  Some counterpart: 95% overall (game 99%).
- Code the PC port runs as game C (2589 MH1 functions): identical 1638 (0.44 MB), shape 213
  (0.08 MB), edited 652 (0.52 MB: 379 functions >= 90% similar, 219 at 70-90%, 54 below),
  rewritten 26, missing 60 (0.06 MB). Roughly half of the port's game C runs in G as is.
- Matched C (7374 MH1 functions): 4768 are clean identical twins; 858 of 1719 C files are
  made only of clean twins with the same layout in G. 805 of them now build byte-identical
  inside G (section 3).
- G-only code: about 0.36 MB of G's 3.9 MB of text: main 80 KB, sub_main 102 KB (27% of
  it), game 39 KB, lobby 128 KB (the plaza/gathering-hall code: plaza_*, lb_npc_pig_*,
  quest levels), select 10 KB. Most of it is only called from other G-only code, i.e. a few
  new subsystems rather than scattered edits. Not identified yet.
- Moves: G put a resident overlay sub_main.bin (0x4A0C80, right after main's .bss) holding
  577 former main functions (effects eft01/02/06/13/20, set13, stage drawing trans_*, hit
  tests, quest/reward/result, net sync); 218 player functions (pl_mv*, pl_at*, pl_dm*) moved
  from main into game.bin; lobby's OpenSSL/HTTP (1425 functions) became nethttp.bin
  (0xA06200, loaded with lobby). yn.bin now loads at 0xA06200 too.
- Data: every G file loads with our readers (tools/mhg_data_check.py: 0 rejects in 3676
  files). 1828 of 2002 shared AFS_DATA files are byte-identical. Format changes: collision
  lg/lw -> lwgNNN.bin (a link file of wall + ground HITS), quest files m001..m178 ->
  mNNNNN.mib with a 0x48-byte header (was 0x40). G's new monsters load and pose with
  their own motions (em36: tools/clay_dump.py on disc/mhg, rendered check
  build/mhg/show/em36.png, not committed).
- Struct layouts changed in places: e.g. GAME_W's `master` byte is at +0x3A in G (+0xD1 in
  MH1), seen in pl_master_ck. Work areas moved (MH1 0x3Fxxxx -> G 0x36xxxx), so C that
  still uses literal MH1 addresses (`*(u8 *)0x3F34C1`, m2c drafts) does not transfer.

## 2. Building G (Phase 1: done, byte-identical)

    tools/mwo_unpack.py is used in-process; nothing to extract by hand.
    VERSION=mhg tools/rebuild.sh          # split + build all seven modules
    VERSION=mhg python3 tools/build.py    # rebuild after editing C
    VERSION=mhg python3 tools/progress.py # G matched %
    tools/rebuild.sh                      # MH1, unchanged (all five OK)

- Modules: main (SLPM_658.69 0x100000-0x2EF200; text to 0x23A280, VU1 microcode, data from
  0x253D20, .sdata from _gp-0x7FF0 = 0x2EAB80, _gp 0x2F2B70 from .reginfo), sub_main, select,
  game, yn, lobby, nethttp (overlays from AFS_DATA.AFS, MWo3, unpacked in setup_split.py).
  dnas_net/dnas_ins are not split (replaced, as for MH1).
- Names: config/mhg/symbols/*.txt from tools/mhg_map.py (MH1 names at G addresses: 10428
  function twins + edited counterparts, 13.9k data names from twin operands, content matches
  and layout between mapped neighbours). config/mhg/*_aliases.txt PROVIDE every name (so a
  C file's statics still resolve for the asm around it) and the MH1 `D_XXXXXX` names the C
  uses, moved to G addresses.
- The text is cut into files at the G twins of MH1's file starts, so asm/mhg file names
  follow MH1's (f_em, f_eft17, ...).
- First result, 13 Oct 2026: all seven modules OK with no C (asm only).

## 3. Shared matched C in G

tools/mhg_cfiles.py writes config/mhg/c_files.txt: every MH1 C file whose functions are all
clean identical twins laid out the same way in G, with its jump-table/data slots moved
through the symbol at the slot start. config/mhg/c_skip.txt lists files that were tried and
differ from G after linking (tools/mhg_cdiff.py finds them): they read a struct field G moved
(game_w.master), use literal MH1 addresses, or call a function with no G twin (powf).
Those need G copies (src/mhg/...) or `#ifdef VERSION_MHG`.

Result (13 Oct 2026): **793 MH1 C files build G's bytes unchanged**; all seven G modules OK
with them. `VERSION=mhg tools/progress.py`: 7.5% of G's function bytes matched (main 8.8%,
game 8.8%, lobby 8.2%, yn 20%, sub_main 2.8%; nethttp 0.4%). 56 files are in c_skip.txt.

Why not more: most MH1 C files contain at least one function G edited. Of the 1719 matched
MH1 files, 870 are all clean twins (0.41 MB); 675 have one or two functions that are not
(0.62 MB, 0.44 MB of it in those functions: mostly one- or two-function files); 174 files
have three or more (1.24 MB, 0.83 MB not clean). The clean twins inside mixed files add up
to about 0.59 MB: a G copy of such a file with its few edited functions redone would bring
them in. Next step for G matching: per-file G copies (src/mhg/...) for files with one or two
edited functions, smallest diff first; and the struct-offset tool (section 4) for the
shape twins, which mostly differ only in field offsets.

## 4. Plan: G on the PC with our platform layer (fastest route)

How the MH1 PC port is built (docs/pc.md "Port runtime"): src/pc (gfx, audio, pad, fmt,
movie, net, menu, install: version-independent) + src/pc/rt (runtime glue: boot, flow,
data import from the user's ELF/overlays by address, overlay call routing, native stand-ins
for undecompiled main functions) + about 800 game C files (tools/build_pc.sh lists).

### What in src/pc/rt is game-specific

1. **Addresses and file names.** SLPM_654.95 / AFS sizes in install.c and viewer.c, overlay
   load address 0x533980 (rt_mem.c OVL_GAME_VRAM, rt_data.c LB_VRAM, rt_online.c LBA),
   ptmat_tbl 0x3018F0, rt_overlay.c's `func_6229B0`-style address names. About 280 distinct
   hex constants in src/pc; most are comments, a few dozen are code.
   -> one per-version table (rt_version.c: ELF name, sizes, overlay addresses, sub_main) and
   names instead of addresses (generated from the version's symbol files). *1-2 days.*
2. **Data import** (rt_data.c, tables.txt, gen_rt_tables.py, gen_rt_auto.py, pc_abs.py): by
   name through config/symbols -> config/mhg/symbols. 756 of the 1054 tables already have a
   G name; the rest need more data mapping (mostly tables only edited code touches).
   **G has no relocation sections**, and rt_mem.c turns pointer words into host pointers
   using MH1's R_MIPS_32 list. For G: carry MH1's pointer positions over through the
   content-matched data objects (7914 matched) and mark G-only tables by hand or by a
   "looks like an address in a module" scan; write the list into config/mhg (positions
   only). *3-4 days.*
3. **Overlays.** Natively there are no overlays; the port links game C from all modules into
   one binary and keeps lobby/select as host copies of the overlay image. G adds sub_main
   (always resident: easy, link it like main) and nethttp (replace like DNAS: the PC never
   speaks SSL to Capcom servers). The rt_overlay.c routing table becomes generated per
   version. *1-2 days.*
4. **File formats.** HITS reader: read lwgNNN.bin (entry 0 wall, entry 1 ground) instead of
   lg/lw. Quest .mib: the G header (0x48). Both small. *1 day.*
5. **Text/English.** G's English patch instead of MH1's (docs/english.md; translators'
   terms first). The text renderer and font code are shared. *depends on the patch.*

### Game C for G

- **Identical/shape functions (about half of what the port runs):** the MH1 C is reused.
  Shape twins need the struct/constant difference: write a tool that, from the 726 shape
  twins, lists for each base symbol (game_w, player_work, quest_w, ...) which MH1 offset
  became which G offset, and gives G headers per version (`#ifdef VERSION_MHG` field
  blocks, or G copies of the structs). *Tool 1-2 days, headers 3-5 days.*
- **Edited functions the port runs:** 652 functions, 0.52 MB; 379 are >= 90% similar. Each
  gets a G copy (src/mhg/...) made from the MH1 C plus the asm diff, checked with the same
  matching setup (G matching works like MH1's: same compiler). Agent rate in MH1 for
  "known logic, small change" work was tens of functions a day. *Estimate: 3-5 agent-weeks;
  the >= 90% group first, by what the first quest needs.*
- **Missing/rewritten in the run-set:** 86 functions; decompile from G's asm. *1 agent-week.*
- **G-only systems:** identify first (call graph from known code: who calls the clusters;
  strings; then name). The plaza (lobby) and the sub_main clusters are the big ones; new
  monsters em35-em50 seem to reuse the monster engine with new tables plus edited AI code
  (game.bin only has 39 KB of G-only code). *Identification 2-3 days; porting unknown,
  likely 2-4 agent-weeks for what offline play needs.*
- **Stop-gap for the rest:** like MH1, native stand-ins (rt_*.c) and PS2Recomp were the way
  to run code not decompiled yet; G has no symbols, but our symbol map gives names to 75% of
  its functions, so the same approach works.

### Fastest route (order)

1. Version table + generated names in src/pc/rt; build the PC binary with VERSION=mhg
   linking only identical/shape game C and stand-ins: show G's village stage and a G-only
   monster in the viewer from G's own disc (*about 1 week*).
2. Data import with transferred pointers, lwg collision, quest .mib: a G quest loads (*1
   week*).
3. G struct headers + G copies of the edited functions on the quest path (player, monster
   core, quest flow, HUD): first G hunt playable (*3-5 agent-weeks*, parallel over agents
   by module).
4. G-only systems, then English text, then online (*afterwards*).

With four agents working in parallel, a first playable G hunt on the PC is an estimate of
3-5 weeks; the village/menus of G-only features come after. These are guesses based on
MH1's pace, not measurements.

## 5. G on the PC: runtime (agent B; shared list of what blocks G at run time)

Build: `tools/build_pc_mhg.sh` -> build/mhg/pc/mhview_g (own tree build/mhg/pctree, so build/pc stays MH1's; `ONLINE=1` builds in build/mhg/pctree_online, its objects are compiled with -DMHG_ONLINE).
Run (always hidden: a G build opens no window and no real controller unless MH1_SHOW=1 is set by hand, and
refuses to start without `--shot`):
`MH1_SAVE_DIR=$PWD/build/mhg/x/card RT_NOMOVIE=1 RT_NAME=TEST build/mhg/pc/mhview_g disc/mhg --boot --input "$(cat tools/pc_scripts/newgame.txt),idle*1500" --shot build/mhg/x/s.png --time 110`
(RT_SHOTS=t1,t2,... for more pictures; logs of hidden runs go to build/test_logs; without MH1_SAVE_DIR saves
go to ~/.local/share/mhgpc).
- tools/mhg_map.py: +974 data names from edited functions (aligned lui/lo and gp references); G rebuild 7 OK.
- tools/mhg_pcmeta.py: config/mhg/pc_ptrs.txt (G pointer words, addresses only: MH1 relocations carried
  through same-name objects + a scan; scan checked on MH1, precision 96.6-99.6%, misses are jump tables)
  and build/mhg/pc/symbols (sized view; 38 unnamed G functions named from pointer tables, e.g.
  pl01_effect_move = 0x4E2EB0, which mhg_map had paired with 0x4A9150 at 0.56: feed back into mhg_map).
  config/mhg/pc_names.txt: G addresses of MH1 data names found by hand (`module name 0xADDR # how`).
- tools/gen_rt_meta.py -> rt_meta.c (G symbols, pointers, MH1->G address map); src/pc/rt/rt_ver.c
  (rt_ver_mhg, RTVA/RTVA_LB/RTVA_SEL); rt_mem.c reads overlay vram/bss from MWo3 headers, sub_main image,
  G symbols/pointers from meta; fmt_mwo_unpack (melt.c) unpacks G's overlays; gen_rt_tables.py /
  gen_rt_auto.py have a VERSION=mhg mode (150 tables + 16 D_ names not in G's map: zeros, listed in
  build/pc/mhg_*missing.txt).
- f_sphr.c: G load_stage_hit (lwgNNN.bin link: wall entry 0, ground entry 1) under VERSION_MHG.

### How far (13 Oct 2026, evening)
Power-on -> Sofdec/ADX logos -> title (right size now) -> GAME MENU (NEW GAME / CONTINUE / GALLERY /
OPTIONS) -> NEW GAME -> character creation (name, sex, face, hair, colour, voice; hunter shown) -> save to
the card (Japanese card dialogs) -> Game_task -> **stops at the village** (Local_main). CONTINUE crashes in
the game menu (below). Shots: build/mhg/ng4
(s_1000 creation, s_1200 save question, s_1400 saved), build/mhg/menu.png, build/mhg/title.png.
Fixed for G on the way (all under VERSION_MHG / rt_ver_mhg(), MH1 unchanged):
- title_disp: G's title art is one 512x512 page (MH1: 256x256): G's UVs and menu line (demo.c).
- font: G's glyph file is AFS_DATA 0x9AE and G's palettes are one colour over four CLUTs (rt_font.c).
- init_move_work: G has no item work (pl78.c); village motion/NPC tables through RTVA (rt_village.c).
- Edit_task / edit_trans: G copies written from G's code (edit_nm.c; G added a step 0, MH1's steps +1).
- yn_tbl, mc_sel_tbl, client_work, filedef_sys (button/frame textures): hand-found G addresses
  (config/mhg/pc_names.txt).
- the mc_mes_disp crash: card_data_init is edited in G (memset 0x130D0, icon 0x9B0 at +0x20000;
  mccomb.c / mccomb_nm.c).

### Stand-ins (G only, logged with rt_log_standin)
- G boot: set-up quest 10 not loaded, no monster spawned at power-on (viewer.c).
- G Edit_task: the MH1-hunter carry-over (card ops 0xB-0xD, step 0 substeps 4-14, select 0x546A40 screen)
  is skipped: NEW GAME always makes a new hunter.
- G Edit_task: func_0012D6F0 (sets the gp word 0x2EF26C to -1) and func_0021E370 (after SoftKeyboard_set)
  are not called.
- rt_village_enter: MH1's Clear_lobby_ram is not run for G (see the lobby list below).

### Blocking list at run time (who; in path order)
Village (agent D, lobby): the village is blocked here. G's lobby village code is edited almost
everywhere and MH1's C on G's memory writes MH1 offsets: G's lb_sys is laid out differently (G clears
0x98 bytes at 0x6548F0, Local_main reads lb_sys+3 at 0x654909 and the exit code at 0x6548F0, ends on
0x22 / 0x27 where MH1 has 0x20 / 0x25), pNet fields moved (+0x2D / +0x31, MH1 +0xC / +0x11), lb_player
has 10 entries of 0x38 bytes. Needed as G copies, in the order the village runs them:
lobby_func_0059A270 (G's Clear_lobby_ram: lb_sys, lb_player, cw = 0x6DEC10, pNet = network_work,
Lbc_init_network_work, Lb_set_mini_data), Local_main 0x59E480, vs_square_init_pre / init_init /
init_se / init / event / exit, Lb_stage_load, Lb_npc_set, Lb_set_player, Lb_set_mini_data(_to_pl),
Lb_make_quest_tbl_local, Lb_check_newCommer, lb_basic_master, Lb_npc_mv, lbc_text_lobby_trans,
Lbc_init_network_work, Lbc_set_prim; main: Pit_reset (agent C). lb_set_key_quest_local is missing in G.
G's Game_task (main 0x239490) is rewritten (offline step 2 = 0x239F60: Local_main every tick, 1 ->
quest, -1 -> back to the title); the PC's rt_flow/rt_village stand for it.
Quest path (agent A, 14 Oct 2026): `--quest N` (G numbers: 1 = m00001.mib, 10001 = training ...) loads
G's mission file and starts the hunt: build/A/g/q1.png (quest 1, forest base camp, hunter, HUD). Done, all
under VERSION_MHG, written from G's asm for the PC, not byte-checked (they need G's GAME_W layout):
Quest_start, Quest_pl_stage_init (random start areas), quest_em_init_sub/_sub2 (+ G's quest random
generator 0x4B52E0/0x4B5480, random monster sizes 0x4B7810/0x4B7920), f_quest.c; Stage_data_get /
Stage_mv_data_get per map (agent C's hunks + 0x4A1970, qstb02/05.c); Tutorial_quest_ck; em_cmd_init
(11 levels, 52 kinds); em command interpreter: G's new commands 0x2C (species select), 0x69, 0x70 (the
old 0x2C), 0x71, 0x99 and their lengths (em_cmd_nm.c); fish_type_set (eft23.c). include/quest.h: G's
QUEST_W tail (0x290) and QEM+0x30; include/flow.h: select_w quest number at +0xD0.
G tables for the PC: config/mhg/pc_data.txt (names, G addresses and sizes; G-only tables under descriptive names, never D_ with a G address; kind tables
grown from 35 to 52 kinds; sun_pos_tbl = 0x5243E0, G's stage_set_set copies it into stage_work+0xC8).
tools/mhg_pcmeta.py also takes pointer words from the G split (`.word SYMBOL`) and its labels as targets;
a word pointing into another module must hit a label start (em20's byte code had been rewritten).
Full hunt checked (scripted, hidden): G quest 10 (Rathian, stage 40) with RT_QUEST_STAGE=1 RT_EM_HP=30
RT_DMG_MUL=40: kill at tick 154 -> clear (x06 4) -> carve wait -> game3 -> game5 -> reward list (204, 205,
278 x2) -> reward screen (build/A/g/q10_end*.png). Also on this path: em_cmd 0x56/0x57 take u16 quest
numbers in G; hit zones per monster state (em_meat_tbl, G 0x509410); carve tables per quest rank
(em_hagi_type_tbl, G 0x6D5D50); G's move() has no item_check/move_item (no item work in G);
player_work has 10 entries in G (rt_game.c); EMW keeps MH1's 0xA10 stride on the PC (em.h, emw01/02.c,
version.h EMW_STRIDE: G's 0xA00 only for MWCC; the PC's local EMX/EWK copies are 0xA10). Hidden G new game
without RT_NO_NPC reaches the hunter's house and runs 110 s (build/A/g/v/s.png); NPC em_cmd tables are
converted now (pointer words from the split). G next_cmd_search checked opcode by opcode against an emulation of G's asm.
Still open: EMW bytes G 0x9FB/0x9FC (monster state for hit zones, em command 0x71) have no writer on the
PC; em_cmd_flag_set/clear/ck: G adds kinds 0x32 and 0x2D (subspecies) to the kind 7/0xF cases (not
done); the reward screen draws its frame but no item list/text; armour tables not imported.
Main (agent C), blocking CONTINUE: Select_task's menu family is edited in G (csub00 0x1EE8B0, init_mode_sel
0x1EEAD0, mode_sel 0x1EEC50, mode_sel_end 0x1EF060, mode_sel_exit 0x1EF270, disp_mode_menu 0x1EF420; G's menu
is NEW GAME / CONTINUE / GALLERY / OPTIONS). NEW GAME happens to work with MH1's C; CONTINUE (with a save on
the card) takes MH1's sub-menu path and crashes on game_menu_msg / game_menu_msg2 (no G address;
build/mhg/ct1). Cont_task (select 0x548AC0) is edited too (2784 bytes vs 2616): after that.
Main (agent C), seen on the way, not blocking:
- McCardOperation (main 0x231940, edited): G's ops 0xB-0xD (MH1 hunter carry-over).
- DispSoftkeyboard (0x20B020, edited): the key grid is drawn empty (RT_NAME types the name).
- Note for G copies that draw: G renumbered mem_tex (all_reset loads filedef_sys 4/5/6 into
  0x143/0x180/0x17F, MH1 5/6 into 0x157/0x156; the HUD list starts at 0x13F, MH1 0x118). The PC keeps
  MH1's numbers everywhere (its own loads and MH1's C agree), so a G copy must use MH1's slot numbers,
  or all loads must move together. (The white squares where buttons belong were filedef_sys, which had
  no G address: fixed, config/mhg/pc_names.txt.)
- mc_sel_ck (0x22BD40): G reads its table through the message-table list at 0x2B0B40 (the PC uses
  mc_sel_tbl = 0x2B0780 = entry 7).
Data names (13 Oct 2026, agent B: tools/mhg_datamap.py; text tables agent E, the rest C/D): the G PC build now
takes G addresses of MH1 data names from config/mhg/data_map.txt (generated, high/medium only) before the hand
list config/mhg/pc_names.txt (which still wins). Result on the same new-game run: data stand-ins (rt_gen.c weak
functions that are MH1 OBJECTs) 73 -> 50, zero-filled tables (mhg_tables_missing.txt) 149 -> 111, D_/lit_
names (mhg_auto_missing.txt) 16 -> 13; same pictures and warnings up to the save, same stop in Local_main.
Left as stand-ins: Friend_data SearchCondition SearchResult category_name_str chatHandleList chatListFlag
check_bit_no dod_new_reguration_agree_type edit_msg em17_rail_add evdemo_03..24 event_demo ex_equip_tbl
fight_bgm_tbl item_box item_ctr item_sp item_w_top item_work lb_guild_str lb_quest_clear lb_quest_info
lb_quest_message lb_set01_msg mix_manual_tbl movie_no netDialogMessage omake_str pSceneSubTitle pSceneTitle
rtcDate shop_armor00_tag shop_armor01_tag shop_process00_tag st64_dir st_sun_tb_tbl staff_tbl text_lobby_msg
tl_etc. Left zero-filled (111): build/mhg/pctree/build/pc/mhg_tables_missing.txt after a G build (monster_data,
enemy_name, St_data, Shell_data, Armor_*_Data, em*_timer_tbl, many lit_ strings of f_chat/menu, ...).
For most of the rest the tool HAS a candidate but only at low confidence (`low` lines in data_map.txt), and
often because G changed the table, not because the address is unknown: em01_stay_timer_tbl's twin in G
em01_init passes a main-data table of a different layout (0x2ECD1C), monster_data's G candidate 0x5278D0 shares
7% of the bytes. Those need G copies of the code (A: em, C: menus, D: lobby), not a name. The functions that
read them are often `missing` in mhg_map (staff_disp, em_hp_vital_set2, init_item_work: item_work is gone in G).
mhg_datamap.py (all standard library, about 30 s):
- per MH1 data object without a G name, evidence from: `ref` (same instruction in an identical/shape twin,
  gp and lui/lo on either side, G moved small objects out of .sdata), `refseq` (edited/rewritten twins: the
  two functions' sequences of data references aligned; references to names G has must land on the same G
  object, the unplaced ones pair up in order between them), `ptr-in` (MH1 pointer word into the object
  inside a G-named container), `ptr-out` (the object's own pointer words point at G-named objects/functions:
  find those G words at the same offsets), `content` (bytes with pointer words masked; strings with their
  NUL, inside the window the neighbours give), `layout` (same distance between two named neighbours, or one
  neighbour plus agreeing bytes); `bytes=` on each line is how much of MH1's content G has at the address
  (< 0.25 makes it low: G changed the table, the MH1 C would misread it; < 0.5 is worth a look).
- four rounds (what one places anchors the next); one name per G address; confidence high/medium/low.
- `--eval`: holds out every 8th name mhg_map placed and maps it again: high 99.8% right (1582), medium
  97.5% (118), low 84% (49). Held-out names are easier than the real leftovers: treat medium as likely, not
  proven. `-v NAME` prints a name's candidates; `--pc` lists the PC names still missing with their best guess.
- mhg_pcmeta.py: data_map names are added beside an existing G name at the same address (never rename it);
  scan pointer words inside a data_map object are kept even where MH1's layout has none (G may have changed it);
  pointer words 33802 -> 35466. Hand-list entries the tool confirms are dropped (yn_tbl; docs/REVIEW.md).
- Not fed into config/mhg/symbols (the PS2 G rebuild), only into the PC symbol view.
After merging main (13 Oct 2026, evening) the G new game had regressed on the PC; fixed on the PC side only
(`#ifndef __MWERKS__`, the PS2 G build is unchanged, 7 OK):
- omake.c G mode_sel: `Tsk_Execute(D_546E80)` crashed at tick 708: gen_rt_auto.py reads a D_ name's hex part
  as an MH1 address. On the PC the G block uses MH1's D_5367F0 / D_5375F0 (mapped to G's Edit_task /
  Cont_task). Rule for G C that the PC links: never a G address in a D_ name; use the symbol name or MH1's D_.
- mccomb.c G McCardOperation op 0xB: func_002306C0 (carry-over check) is a PC stand-in that never finishes, so
  Edit_task waited forever at step 0; the PC answers "no MH1 hunter" (2) and logs a stand-in.
With both: title -> NEW GAME -> character creation -> save -> Game_task -> stop in Local_main (village), as before.
G tables for fair hunts (14 Oct 2026, agent A; docs/agents/agent-A.md "G round 4"): monster hp from G's
em_vital_tbl x em_lv_rate_tbl (em_hp_vital_tbl is MH1-only), attack/defence rates per level, G ammo (Shell_data
2-byte rows, Shell_max_tbl), skills (Skill_data, armor_kind_tbl), Get_equip_value(pl, kind) on G's User_data
(0xE00 bytes), G blade-hit sharpness, G reward list. tools/pc_g_exclude.txt keeps matched MH1 files whose functions
G edited (emu01/emu02/ems01, f_ud) out of the G PC build. pc_data `list:N`: a G {stage, value} list's own length.
Quest 10 fails by time-out (quest-failed screen).
G offline quests (14 Oct 2026, agent A; docs/agents/agent-A.md "G round 5"): tools/test_all_quests_g.sh, village
1-50 and training 10001-10048 (`--quest 89` is not a village quest: group 0 has 56 list entries, 89 reads group 1):
96/98 load, spawn, clear with the aids, fail on time-out; carts (3 faints) fail quest 9. Found for other lanes:
- player (pl): G's item_action_set (game 0x54D8C0) uses G item ids; the PC runs MH1's (only the tool ids are
  G now, f_pl.c). Using potions, whetstones etc. on G is wrong until it is ported.
- hit: quests 25 / 41 / 10046 crash now and then (about 1 run in 4) in hit_calc_shl -> hit_data_expand ->
  flmatGetTrans (a shell's owner joint, fault in host memory; about 1 run in 4).
- stage (B/D): the per-stage tables G grew to 92 (stage_fog_tbl, ground_tbl_add, st_sun_rgba_tbl,
  pl_light_tbl2) stay at MH1 size: at G size the village crashed in GetGroundHitStatusAreaPl.
- G jungle stage 74 has exits out but none in: real data, a start-only area (G's random quest start, 3%; docs/FINDS.md).
- monster (A): em01/em15/em17 G revival tested with RT_QEM_LIVES (tools/test_revival_g.sh); 10048 passes (G event demos,
  em07 route); G hunting log and its records ported (round 7); kind 40 (unused, a cut-down Felyne) ported for the PC
  (em40_g.c, docs/FINDS.md); G em_type_act_set kinds 35-50 and em_parts_num at 52 kinds (15 Oct).

Quest 10 fails by time-out (quest-failed screen); quests on map 6 (89, 129) hang in an em program (open).
Loading screens and transitions (14 Oct 2026, agent F; docs/agents/agent-F.md "G: loading screens"): G's loading screen
is a request drawn on each vsync of a load (main 0x12DA20 from load_bin's wait); the PC draws it after disc reads
(disp1_nm.c VERSION_MHG, viewer.c load_wait). Area-change map drawn white as G does; the quest start shows black until
the fade-in (MH1 too). rt_village.c reads the accepted quest at select_w+0xD0 in G.

G on the Xbox (14 Oct 2026, agent B): `python3 tools/build_xbox.py --version mhg --gfx nv2a` after
tools/build_pc_mhg.sh; own title id / save / folder (E:\Games\MHG); in xemu new game -> save -> village and
`RT_ARGS=--quest 10` -> reward -> village. Memory and what to test on hardware: docs/xbox.md "Monster Hunter G
on the Xbox".

Release checklist for G (15 Oct 2026, agent A): docs/RELEASE_G.md (criteria a player notices, every known gap by impact
with lane and evidence, a coverage run of the whole offline game); the reward screen's item line, CONTINUE, the elder's
quest lists and the em12 revival points were fixed with it.
