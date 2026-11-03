# MH Dos: data names, function pairs and the pointer call graph (DATA agent, 2 Nov 2026)

docs/mh2_port.md section 6, items 1 and 8. Community mh2-decomp not opened. MH2 has no symbols or relocations, so every name here is
scored evidence from MH1 (symbols, relocations) and G (our names, config/mhg). What was checked, and how, is below; the rest is a guess.

## Tools

    python3 tools/mh2_datamap.py            # config/mh2/data_map.txt: module name address confidence methods
    python3 tools/mh2_datamap.py --eval all|noref|single     # hold-out checks (below)
    python3 tools/mh2_pairmore.py [--eval]  # config/mh2/func_pairs.txt: more function pairs; build/mh2/order_suspects.txt
    python3 tools/mh2_funcfix.py            # config/mh2/func_fix.txt onto docs/survey/mh2_map.csv and config/mh2/symbols (mh2_map.py runs it last)
    python3 tools/mh2_callgraph.py          # stage reach table, docs/survey/mh2_reach.csv

Run order: mh2_map.py (it runs mh2_funcfix.py last), then datamap --fresh, pairmore, datamap --fresh, pairmore, datamap --fresh
(scratch iter.sh of the DATA agent; a fourth round only swaps 1-2 low/medium names back and forth). Both tools re-run themselves
with PYTHONHASHSEED=0, so the same inputs give the same files (checked by md5). datamap and funcfix read config/mh2/c_aliases.txt,
which mh2_carry.py register writes, so after a register run mh2_map.py again: on 2 Nov it gave the same files (a fixed point). The aligned-code votes are cached in build/mh2/datamap_ref.pickle;
--fresh rebuilds it (needed after mh2_map.csv or func_pairs.txt change).

- **mh2_datamap.py** now works like mhg_datamap.py: aligned code (`ref`, now also through `lui; addu; lw lo()`, the indexed table read),
  reference sequences in edited pairs (`refseq`), pointer words in and out, content, `chunk` (8-byte pieces of a table MH2 edited a little),
  layout. Confidence column: high / medium / low; mh2_pcmeta.py takes high and medium. A name can share an address with an alias in the
  reference, or with a table of identical bytes (MH2 merged equal tables).
  `c` (later on 2 Nov): a data name the byte-matched carried C uses (config/mh2/c_aliases.txt) is placed at the address its relocation
  lands on, before any vote; config/mh2/pc_names.txt still wins. Checked against the data names of the symbol files: 213 of 214 agree.
  The miss is MemcardWork (+0x28): an alias is the MH2 address minus G's offset, so a struct whose field moved gets a shifted base.
  323 names written high as `c:9.0`; 19 of them moved from the earlier map (CNFile 0x5D6AA0, flViewportDH/DW 0x20CBA0/0x20CBA8,
  set01_stack 0x6AE9E0 with set01_stack_timer/ptr/suu 0x6AE9C8/D0/D8, fly_adj*2_hosei_tbl, pl_motion_data 0x41ED50 ...).
  refseq votes now survive when every code vote for the name is misaligned (inside another known object): em_hate_add_tbl 0x8BC200
  (its words point at the em*_hate_add_tbl placed just before it), Silencer_Grow_Up_Tbl, eft10_all_time.
- **mh2_pairmore.py** finds function pairs mh2_map.py missed: `table` (a placed object holds the reference function where MH2 holds an
  unpaired one), `order` (between sure pairs, a file keeps its order: aligned by opcode ratio), `items` (same placed data and same paired callees,
  ratio >= 0.4). 1,868 pairs (items 829, order 727, table 312). mh2_datamap.py reads them as edited pairs.
- **mh2_funcfix.py / config/mh2/func_fix.txt**: name corrections that outrank mh2_map.py (78 lines: 34 em_cmd handlers, 43 by file order, gp_ptr_ck).
  A line is left out when the byte-matched C (c_aliases.txt) holds that name at another address or another name at that address
  (NAME_mh2_ADDR counts as NAME; func_ADDR placeholders do not count; names compared across modules, because resident sub_main names
  reach an overlay's link). 4 left out: ADXM_Lock (C: 0x161EC0), Menu_chatlog_mv at sub_main 0x35ABD0 (lobby C calls lobby 0x7B91F0 by
  that name; naming sub_main's broke lobby, tried), Stage_unique_data_get (C: 0x3A0E20), em01_act_set (C: em20_act_set at 0x7516D0).
  A fixed name leaves other addresses of its own module only.

## Numbers

| | before | now |
|---|---|---|
| data names placed (config/mh2/data_map.txt, high+medium) | 1,499 (one vote rule) | 7,084 (high 4,014, medium 3,070; low 1,811 written, not used) |
| PC tables with no MH2 address (gen_rt_tables, mhg_tables_missing.txt) | 933 of 1,340 | 584 of 1,340 |
| other data names zeroed (mhg_auto_missing.txt) | 462 | 249 |
| MH1 data addresses mapped for pc_abs (addr.txt) | | 7,308 |

## Verification sample

Truth: the 669 data names tools/mh2_map.py (twins, all votes agreeing) and config/mh2/pc_names.txt place. Each mode hides part of them
from the tool, maps them like unplaced names and compares.

| mode | held out | high right | medium right | low right | not placed |
|---|---|---|---|---|---|
| (numbers below: before the c names; after them, same sample: noref high 79 of 79, medium 67 of 67, low 5 of 7, 15 not placed; single high 58 of 58, medium 6 of 7, low 0 of 3) | | | | | |
| all (every 8th, keeps its code votes: the easy case) | 84 | 72 of 72 | 12 of 12 | - | 0 |
| noref (every 4th, aligned-code votes removed: tests refseq, pointers, content, layout) | 168 | 80 of 80 | 65 of 65 | 6 of 8 | 15 |
| single (every 4th that has an edited-pair vote; it keeps only that one vote) | 68 | 57 of 57 | 6 of 7 | 0 of 3 | 1 |

Earlier versions of the rules got 2 of ~140 medium wrong in noref, all in the CRI library (adx*, sj*, skg*). The held-out names are
twins' operands, the easiest kind, so real precision on names that only edited code reaches is lower than these tables; how much lower
is not known. Each method on its own, top candidate against the 669: content 134/136, layout 242/246, ptr-in 2/2, ptr-out 8/8, ref 496/499.

**The 1,499 names of the old map** (930 of them single-vote): now 1,320 at the same address (848 of the single-vote ones), 80 at another
address, 69 not placed. Of the single-vote ones kept at the same address, 307 have a second independent method (layout, content or
pointers) and 113 more have bytes that agree (>= 60%); 391 are work areas with no bytes and only the code vote, so nothing can confirm them
short of running; 45 have bytes that differ (MH2 changed the table, or the name is wrong).

**Function pairs** (--eval: every 8th edited pair with sim >= 0.75 hidden): found again 203 of 274, wrong 12, not found 59. By method:
items 151 right / 6 wrong, order 37 / 3, table 15 / 3.

**Pointer scan** (tools/mh2_pcmeta.py's rule, which has no MH2 check): run on MH1, which has relocations. 22,870 of the 23,470 words it
calls pointers are relocated (97.4%); it finds 69% of the 32,981 relocated words. Words that are exactly a function start: 1,510 of 1,546
relocated (97.7%).

## em_cmd handlers (QUEST's 47 rows)

Read from QUEST's em_cmd_ck.c (MH2) and src/game/em/em_cmd_r01.c (MH1), handler by handler. **No opcode moved.** All 109 opcodes MH1 has
sit in MH2 in the same address order as in MH1 (the file kept its order: a moved opcode would break it, none does), and every same-opcode
pair is either the same code or a plausible edit of it. So all were map errors. config/mh2/func_fix.txt names them by opcode; the
4 addresses that held a wrong old name and are new opcodes get placeholders em_cmd_op1F, op37, op73, op76 (real names unknown). The ones
that only looked alike: 0x0E stage_no_ck (map: emtype_ck), 0x23 emtype_ck, 0x15 stage_no_sel, 0x63 male_ck (map: stage_no_ck),
0x08 stand_ck at 0x7279D0 (0x727AE0, op 0x1F, is a byte-shape copy of it). 0x1F, 0x36, 0x37, 0x73, 0x76 are copies of old handlers that
read other fields (0x37 reads quest_w + 0x28; 0x73 reads EMW + 0x89D).

Changed, for QUEST and STRUCTS: 0x1C mind_no_ck reads EMW 0x2C2 and 0x6DE where MH1 read 0x88A (rewritten); 0x63 male_ck reads EMW + 0x1B
where G read + 0x11; 0x0E/0x15 read the stage through 0x379B20 and + 0x680 (MH1 + 0x736); 0x3D/0x3E + 0x882 (MH1 + 0x92E).

**QUEST must rename in src/mh2/game/em_cmd_ck.c on merge** (the map's old names there now point elsewhere): each game_func_ADDR of the
list in config/mh2/func_fix.txt becomes that line's name (all em_cmd lines apply now, including stand_ck 0x7279D0, mind_ck 0x7292F0,
type_sel 0x72A190, which the first funcfix check had left out); em_cmd_stand_ck (case 0x1F) -> em_cmd_op1F, em_cmd_emtype_ck (0x0E) ->
em_cmd_stage_no_ck, em_cmd_stage_no_ck (0x63) -> em_cmd_male_ck, em_cmd_male_ck (0x37) -> em_cmd_op37, em_cmd_stage_no_sel (0x73) -> em_cmd_op73,
em_cmd_type_sel (0x76) -> em_cmd_op76; em_cmd_near_pos_ck_mh2_00729CE0 stays. tools/mh2_check.py will show any left over.

## What each stage reaches (item 8)

tools/mh2_callgraph.py: jal and j, function addresses taken in code, function pointers in data (exact function starts only), data
reached through data. Overlays loaded per stage are an assumption (column 2); the cross-references below support game+gm_sub together.

| from | loaded besides main+sub_main | jal only: functions, bytes | with pointers: functions | bytes | C | identical/absolute/shape | edited | no counterpart |
|---|---|---|---|---|---|---|---|---|
| title | select | 1,018, 208,192 | 2,160 | 490,124 | 34,464 | 145,652 | 101,804 | 242,668 |
| mode menu | gm_sub, game | 850, 191,988 | 1,128 | 286,948 | 38,308 | 121,632 | 83,660 | 81,656 |
| char select | plsel, gm_sub, game | 1,460, 380,116 | 9,434 | 3,341,920 | 224,160 | 480,760 | 821,636 | 2,039,524 |
| game task | game, gm_sub, plsel | 6,197, 1,868,232 | 9,434 | 3,341,920 | 224,160 | 480,760 | 821,636 | 2,039,524 |
| quest | game, gm_sub, plsel | 6,204, 1,870,756 | 9,441 | 3,344,444 | 224,160 | 480,760 | 822,976 | 2,040,708 |
| quest, other monsters' named code cut | game, gm_sub, plsel | 4,625, 1,401,808 | 7,831 | 2,839,800 | 169,756 | 373,824 | 740,560 | 1,725,416 |
| town | lobby, lbguild, nethttp, plsel, gm_sub | 2,120, 503,764 | 9,444 | 2,804,336 | 185,992 | 474,560 | 678,724 | 1,651,052 |

How to read it:
- docs/mh2_port.md section 4 counted the game task at 1,446 functions with jal only. Here it is 6,197 with jal only, because sub_main calls
  straight into game and gm_sub (enemy_mv lives in gm_sub) once those are loaded: **the game task and the quest are one graph**.
- With pointers, everything from char select on reaches about 9,400 of 17,034 functions: task and monster tables tie the whole game
  together. That is an upper bound for what one hunt runs, not a count of it; one monster cuts it by about 1,600 functions. Most of the
  2.0 MB "no counterpart" is in main and sub_main, where the Sony and CRI libraries are mixed in with no marker.
- The jal-only column is the lower bound. The real need of one hunt lies between the two and can only be measured by running.

Exact function starts of one overlay held by another (code or data), the hint of which load together: gm_sub -> game 397, game -> gm_sub 80,
yn -> lobby 56, gm_sub -> select 37, nethttp -> lobby 27, lobby -> nethttp 20, gm_sub -> lobby 18, select -> gm_sub 15, plsel -> gm_sub 12,
game -> nethttp 11, plsel -> game 7, plsel -> lobby 7, lobby -> gm_sub 6, lobby -> yn 6, select -> plsel 4. Overlays share addresses, so a
count can be a collision; game and gm_sub look like one load, yn with lobby.

## Tables still missing (584), ranked for the quest

docs/survey/mh2_reach.csv and the reference functions that read each missing table (scratch script, numbers only):
- 114 read by a function on the one-monster quest path that has a pair (was 120: set01_stack_timer/ptr/suu, quest_price, eft10_num by
  the c names, eft10_time_tbl, shell08_rate_g_tbl): 80 have a low-confidence candidate (contested or bytes differ), 7 a medium one in
  plsel, 1 high in select (default_reibun 0x6CAB60, by the C; gen_rt_tables reads only the main and game views), 26 none. Examples: em_hit_push_tbl (twin reads two tables: MH2 split it),
  em01_stay/runaway_timer_tbl, option_w, Snd_bgm_tbl, st*_id_tbl, set19_s*_pos_tbl. These need a look by hand, one by one; most have one reader and a refseq vote of 0.3-1.3, too weak to
  place without it. The fastest proof for any of them is QUEST's byte-matched C: once a reader matches, its c_aliases line places the table.
- 376 whose readers have no pair yet (136 of them literals, lit_*), 51 read elsewhere, 43 with no reader in the reference code (G-only
  tables found by hand, gal_*, *_contents_tbl).

## Notes for the other areas

- PLATFORM: the runtime log of a hidden `--boot` run lists 33 tables at gm_sub addresses (0x90xxxx-0x93xxxx: em_prog_tbl, em_meat_tbl,
  em_dur_tbl, set*_pos_tbl) as "not found": MH2 moved monster data into gm_sub, which the runtime does not load yet. 7 quest tables are in
  plsel (0xD78xxx), outside mh2_pcmeta.py's LOAD groups. 15 more are looked up at MH1 addresses (set00_st04_pos_tbl 0x678160 ...): names my
  map does not place, which gen_rt_auto leaves at their MH1 address.
- STRUCTS: config/mh2/pc_names.txt now pins your bases and sizes (quest_w, User_data, net_common_w, CameraWork, lb_sys, MemcardWork,
  game_w, system_w, shell_work). The data map had net_common_w at 0x5D4690, a field; the others agreed. Tables placed high whose bytes
  differ from G's (MH2 changed the format): Armor_Head/Body/Arm/Waist_Data, g_sub_51B9B0.
- QUEST: the rename list above.
