# G PC build: the zero-filled tables (RELEASE_G G6), table by table

Research only (4 Nov 2026, low-CPU agent): no build, no game run, no config edited. The config lines a later agent
should add are in section 4, as text. Tags: [verified: G asm] = read off G's own code at the address given;
[inferred] = follows from the code but not seen in a run; [guess] = plausible, not checked.

## 1. Where the "37" came from, and what it is today

- `tools/gen_rt_tables.py` writes `build/mhg/pctree/build/pc/mhg_tables_missing.txt`: every name in
  `src/pc/rt/tables.txt` that G's symbol view (`build/mhg/pc/symbols`, made by `tools/mhg_pcmeta.py` from
  config/mhg/symbols + data_map.txt + pc_names.txt + pc_data.txt) has no address for. Each becomes a zeroed host array.
- `tools/cov_report_g.py` (COV=1 build + RT_COV runs) printed which *entered* functions refer to them:
  `docs/survey/mhg_cov_g.txt` (15 Oct), section "ZERO-FILLED G tables (74)". Its RAN lines are the 37 of G6.
- Since then (list of 3 Nov, 75 names) the picture changed a lot. I re-did the "who refers to it" half without a
  build: `nm -u` / `objdump -dr` over the 796 objects in build/mhg/pctree/build/pc, then checked that the referring
  copy is the one linked into `build/mhg/pc/mhview_g` (same size in the binary; weak copies dropped), then that the
  linked binary calls it (x86 `objdump -d`, direct calls). No new coverage run (rules: no game runs).

What happened to the 37 RAN tables of 15 Oct:

| Fate | Tables | How |
|---|---|---|
| Fixed by address (7) | lit_2331, lit_3181_00383570, lit_3440, lit_3441, st49/51/53_pos_tbl_* | pc_names.txt / pc_data.txt (GN4, GM4) |
| Fixed by G code that no longer reads them (26) | set19_s67_pos_tbl, set19_s73_s75_pos_tbl (set19.c G branch, set19g_*), lit_4156..lit_4173 (16), lit_4221, lit_4222, lv123str (equip_g.c / status_g.c replace equip_exp_core, sword_zokusei), sort_no, spr_sort_flag, spr_sort_num (trans2.c: G has no depth sort), em14_act_add, em17_act_add (em14_nm.c / em17_nm.c: G dropped the search), movie_add_tbl (f_reward_g.c reads movie_add_tbl_g 0x525FC0; f_reward.o's movie_add_ck is not linked) | VERSION_MHG code |
| Still read (4) | eft16_neck_tbl, stage_hate_add_tbl, lit_3659, (movie_add_tbl: see above, not any more) | rows below |

So of the 37 only **lit_3659** still matters, plus eft16_neck_tbl and stage_hate_add_tbl, which are harmless.
But 19 *other* names on today's list are read by linked code that the 15 Oct runs simply did not enter (the combine
screen, the trader, the fortress quest page...). Those are in the table too. Today's real count: **22 zero tables
read by linked code**, of which 11 give G's behaviour already (G has no such table), 11 are visible (sections 2-3).
One more find: a table that is *mapped to the wrong G address* (lit_2540), and three that are mapped one map off
(map2_trade_*), both next to the zero tables they belong with.

## 2. The table, by player impact

Addresses are G's (main 0x100000.., sub_main 0x4A0C80.., game overlay 0x542600..). "Cell" = a word in G's data
holding a pointer to the string; G reads most menu strings that way where MH1 used the string itself.

| # | Table (MH1 name) | G address, size | What it holds | Read by (PC, linked) / G reader | What the player sees while zero | Sure? |
|---|---|---|---|---|---|---|
| 1 | frame_mixed_item | main 0x25E680, 0x10 (one frame of 16 bytes; same layout as MH1, a few size bytes differ) | the box of the field combine screen | Pit_disp_item_mix (menu21.c) / G Pit_disp_item_mix main 0x11EF00 passes 0x25E680 as DispFrameMessage's frame, 4 times | combining items from the pause menu: the result box has no frame (all-zero frame) | address [verified: G asm + layout matches MH1's bytes]; effect [inferred] |
| 1b | lit_2540 (mapped WRONG, not zero) | sub_main 0x538980, 0x28 with its end byte (MH1 0x1B) | the box's label lines (item after combining / success rate) | same function / G reads it through the cell 0x527BD8 | the G view gives lit_2540 the address 0x25E680, which is the frame above: the label is printed from frame bytes (garbage or nothing) | [verified: G asm; the cell's target string matches MH1's lines] |
| 1c | lit_2542 | sub_main 0x5389B0, 5 | the success-rate format (number + percent sign) | same / cell 0x527BE0 | success rate blank | [verified: G asm] |
| 1d | lit_2546 | sub_main 0x538A00, 0x22 (MH1 0xB) | "combine failed" lines (G adds result / count labels) | same / cell 0x527BF0 | after a failed combine the message is blank (success shows: lit_2545 is mapped) | [verified: G asm] |
| 2 | map4_trade_sp, map4_trade_nm (and map2_*, map6_* mis-sized / one off) | sp: main 0x2ED348 / 0x2ED350 / 0x2ED358, 8 bytes each (3 item ids + 0xFFFF); nm: game 0x69CB70 (0x10A), 0x69CC80 (0xEC), 0x69CD70 (0xF8) for maps 2 / 4 / 6 | the quest-map trader's (monster kind 10, em10) trade lists: items it takes and what it gives | talk_move (em10_nm.c, MH1 code, 9 calls) / G talk_move game 0x5D8690 | map 4 (Swamp [guess: map numbers as map_name]): the trader reads zeros and then walks past the 10-byte array looking for an 0xFFFF end, so it may offer a random trade or none. Maps 2 and 6 are broken too: the view puts map2_trade_* on G's map-4 lists, and G's rows are 6 bytes (take, give A, give B) where the PC reads 4 | addresses + row layout [verified: G asm, 0xFFFF ends counted]; effect [inferred] |
| 3 | lit_3659 | sub_main 0x537B40, 6 (already named lit_1001_00386A58 there: G merged the two) | format "item name + infinity sign" | disp_shell_name (menu30.c, 13 calls) and trans_box (menu_disp_nm.c) / G disp_shell_name 0x122420 via cell 0x2EC5FC, G trans_box 0x124ED0 via cell 0x527C0C, both pointing at 0x537B40 | items or ammo with unlimited count (item data byte 3 = 0xFF, e.g. a bowgun's unlimited normal ammo [guess on which ones]) are printed as an empty name in the ammo list and the item box line | [verified: G asm] |
| 4 | lit_2128 | sub_main 0x53FCF8, 5 (first entry of G's table D_00528A80) | MH1: the word "none" on the quest page; G: one of 25 "who may join" texts (none / HR 5 or more / ...) | Pit_disp_quest (menu_ftime.c) / G Pit_disp_quest main 0x11D980 prints D_00528A80[quest row +0x1C] at that place, for every quest | the pause menu's quest page shows no join-requirement line. Mapping the name gives "none" where MH1 printed it; G's real logic needs a code change (section 3) | address [verified: G asm]; G logic [verified: G asm] |
| 5 | lit_2242 | sub_main 0x538948, 5 | format of the fortress dragon's damage percent | font_print_Bdragon (menu20.c) / G 0x11DEB0 via cell 0x527BB0 | on the fortress (big dragon) quest the pause page's percent is blank. The C already says `GSTR(lit_2242, gptr_00527BB0)`, but include/g_alias.h picks the cell only for the PS2 (MWCC) G build; the PC G build uses the MH1 name | [verified: G asm + g_alias.h] |
| 6 | lit_4948 | sub_main 0x538B58, 0xA (MH1 6) | the combination list's page number (G's has a colour code; rt_font.c handles `~Cnn`) | disp_mix_list (menu35.c) / G disp_mix_list 0x125AF0 via cell 0x527C24; G adds a second form via 0x527C2C for alchemy recipes (recipe byte 7 bit 0) | the combination list shows no "No. n" page header | [verified: G asm] |
| 7 | lit_826_003581F8 | main 0x2B14E0, 3 | the "%s" format of game mode 5's line | game4 (f_gameb.c, mode 5: wait for every player, then a one-second "quest has ended" line) / G game4 0x1035C0 (a0 = 0x2B14E0, a1 from the cell 0x528618) | the "quest has ended" line is missing for that second. Mode 5 waits for other players: an online path [inferred], G online does not exist yet (G10) | [verified: G asm] |
| 8 | eft16_neck_tbl | none in G | per-kind neck spray points (Cephalos) | eft16_move (linked) via Eft16_set_ex (not called) / G eft16_m 0x577210 has no type 3 / 4 path; G has no Eft16_set_ex | nothing: em12_nm.c already fences both calls for G, and a zero entry ends the effect anyway | [verified: G asm] |
| 9 | stage_hate_add_tbl | none in G | per-stage "hate near this point" (MH1 fills one stage) | Stage_Hate_Add, called from em_move (f_em_nm.c) / G em_move sub_main 0x4E9890 goes from kehai_set straight to kehai_ck: no call | nothing (zero = no extra hate = G). Cleaner: fence the call in f_em_nm.c for G | [verified: G asm] |
| 10 | eft12_bone_col, eft12_meat_col | none in G | MH1 colours of the stage-21 spit's meat | eft12_t (case 2) / G eft12_t02 game 0x570DA0 has one path, sp colours (0x67B250 / 0x67B260, mapped as eft12_sp_*) by byte 6 | nothing: arg 2 (the stage-21 spit) is gone in G and eft12.c already ends it | [verified: G asm + eft12.c] |
| 11 | nfcol_tbl, nfrvcol_tbl | none of that form in G (G's palettes: 17 colours at main 0x25F180) | MH1 font palettes 1-15 | font_set2, only from gfs_nm's weak all_reset, which the host's all_reset overrides (dead code) / G all_reset 0x12CA40 calls G font_set 0x12B3E0 | nothing today. If anything ever calls font_set2 on G, palettes 1-15 turn black (rt_font.c's G palette takes the 5th argument as the colour) | [verified: G asm, linked binary] |
| 12 | Skill_name | sub_main 0x528B70 (= gskill_name, pc_data.txt; pc_data's entry at the same address drops the MH1 name) | skill names | Lb_PlayerStatus (lb_aa.c), called only from the online lobby / G 0x59DE40 prints skills through func_00223610 | offline: never reached. The online build has its own gn_Lb_PlayerStatus.c | [verified: G asm] |
| 13 | str_3166 | none in G | the chat window's "to whom" labels | rt_real_Pit_disp_chat (chat_nm.c) through the rt_2d.c wrapper, in the lobby's chat layer / G Pit_disp_chat 0x221DA0 (rewritten) prints no such label; the strings are not in G's files | offline: not reached [inferred]; if reached it passes a NULL string to font_print_uf | [verified: G asm + byte search] |
| 14 | lb_sendpl_list, lit_136_00358B70, pf_lb_chcnfg_sendpl, q_sendpl_list | none (G has no lb_disp_chat_cnfg_sendpl) | chat config "send to" list | lb_disp_chat_cnfg_sendpl: linked, but called only from the MH1 branch of Pit_disp_chat_cnfg; no call in the binary | nothing | [verified: binary] |
| 15 | monster_data, pf_chat_cnfg, pfl_menu_data, lit_676_003869E0 | not looked up | - | only from object copies that are not linked (menu32 disp_monster_list, menu37 Pit_disp_chat_cnfg, menu33 Pit_disp_data, f_reward3 add_disp) | nothing | [verified: sizes in the binary differ] |

The other 49 names on today's list are not referred to by any object (G code replaced their readers, or the reader
is gone): set19_s11/s12_mask_tbl, set19_s67/s73_s75_pos_tbl, atck_data_no, eft12_st21_set_pos, lit_425/638,
stage_hit_data_f, em_hp_vital_tbl, Equip_Bonus, questName, frame_status_main/sub_*, lit_3514, lit_3588/3591/3594,
lit_4156..4173, lit_4221/4222, lv123str, sort_no / spr_sort_*, status_sub_str_00387C60, weapon_exp_str_common,
movie_add_tbl, em14/em17_act_add, moji_tbl_*, lit_446_0068F0C8. They stay zero harmlessly; they could leave the G
import (tables.txt has no per-version switch today, so I would leave them).

## 3. What needs code, not just a config line

- **Trader (row 2)**: G rewrote the trade step of talk_move (game 0x5D8690). Before the lists it checks item 448
  (99 or more) and room for item 119 (outcome 3, its own message); the "sp" list gives item 139 (MH1 77); the "nm" rows are
  6 bytes and give A or B (`ran_suu % 100 < 20` picks B). A G branch of that case in em10_nm.c is needed; adding the
  config lines without it makes maps 2 / 6 no better (rows read at the wrong stride). Port and config together.
- **Quest page (row 4)**: G prints `D_00528A80[row[0x1C]]` (sub_main 0x528A80 already in pc_names.txt) at
  (0x167, 0x12E) for every quest; MH1 prints "none" only when row byte 3 is 0. A VERSION_MHG line in Pit_disp_quest
  is the right fix; the lit_2128 line is a stop-gap.
- **Combine screen (rows 1-1d)**: the config lines give the box back. G's version also prints the hunter's count of
  the result item after success / failure (Pl_item_num_ck; the extra labels are in G's lit_2545 / lit_2546), so
  with MH1's code those labels show without their numbers. A port of G's Pit_disp_item_mix (main 0x11EF00) would
  finish it.
- **lit_2242 / GSTR in general (row 5)**: every `GSTR(mh1, gptr_X)` in the C reads the MH1 name on the PC G build.
  Each such MH1 name needs a G address (pc_names) or GSTR could use the cell on the PC too (it would need the cell
  words imported). Worth a sweep of include/g_alias.h's gptr_ list.
- Optional tidy: fence Stage_Hate_Add's call in f_em_nm.c for G (row 9).

## 4. Config lines for a later agent (not applied)

Check first that `build/mhg/pc/symbols` still has the names noted. Order matters: in tools/mhg_pcmeta.py pc_names.txt
is applied before pc_data.txt, and a pc_data line drops every other name at its address in that module.

config/mhg/pc_names.txt:

```
main frame_mixed_item 0x25E680   # G Pit_disp_item_mix (main 0x11EF00) passes it to DispFrameMessage 4 times (layout = MH1's 0x2F0A20); was named lit_2540 in the G view
sub_main lit_2542 0x5389B0       # G Pit_disp_item_mix: success-rate format through the cell 0x527BE0 (hunter_appellation+0xD0)
sub_main lit_3659 0x537B40 +     # G disp_shell_name (0x122420, cell 0x2EC5FC) and trans_box (0x124ED0, cell 0x527C0C): the unlimited-count format; same string as lit_1001_00386A58
sub_main lit_2242 0x538948       # G font_print_Bdragon (0x11DEB0) through the cell 0x527BB0 (the PC build does not take GSTR's cell)
sub_main lit_2128 0x53FCF8       # stop-gap: entry 0 of G's join-requirement texts D_00528A80; G Pit_disp_quest (0x11D980) prints D_00528A80[row+0x1C]
main lit_826_003581F8 0x2B14E0   # G game4 (main 0x1035C0): its "%s" format (a0 = 0x2B14E0, the text from the cell 0x528618)
```

config/mhg/pc_data.txt (G strings longer than MH1's, so with G sizes):

```
[sub_main]
lit_2540 = 0x00538980; // size:0x28 noptr     # combine box labels: G Pit_disp_item_mix through the cell 0x527BD8 (+0xC8); MH1 0x1B. Needs frame_mixed_item's pc_names line (the old lit_2540 slot)
lit_2546 = 0x00538A00; // size:0x22 noptr     # combine failed: cell 0x527BF0 (+0xE0); MH1 0xB
lit_4948 = 0x00538B58; // size:0xA noptr      # combination list page number: G disp_mix_list (0x125AF0) cell 0x527C24; MH1 6
```

Only together with the em10 trade port (section 3); replace the two existing `[game]` lines 706-707:

```
[main]
map2_trade_sp = 0x002ED348; // size:0x8 noptr   # G talk_move (game 0x5D8690): map 2 (default) list; 3 ids + 0xFFFF (the old data_map entry 0x2ED350 is map 4's)
map4_trade_sp = 0x002ED350; // size:0x8 noptr
map6_trade_sp = 0x002ED358; // size:0x8 noptr
[game]
map2_trade_nm = 0x0069CB70; // size:0x10A noptr  # 44 rows of 6 bytes {take, give A, give B} + 0xFFFF (was 0x69CC80 = map 4's)
map4_trade_nm = 0x0069CC80; // size:0xEC noptr   # 39 rows + end
map6_trade_nm = 0x0069CD70; // size:0xF8 noptr   # 41 rows + end (the old line said 0x100)
```

The host arrays then take G's sizes (gen_rt_tables uses pc_data's size). em10_nm.c's EM10_TRADE needs a third u16 under VERSION_MHG.

## 5. How it was done, what is left

- Method: as docs/mh2/host.md "Round 5": for each table, the MH1 instruction pair that forms its address (my full
  disassembly of disc/mh1/split and build/mhg/split with lui/lo and gp resolved), the same place in the G twin
  (docs/survey/mhg_map.csv; tools/mhg_fdiff.py side by side), the address G forms there, then the size from the
  next label or the 0xFFFF / NUL end, and for strings the cell's target. Helper scripts are in the agent's scratch
  folder, not in the repo.
- Not checked in a run: every "what the player sees" (rules: no game runs). Worth a hidden PC G run after the config
  lines: combine two items in the field (box, label, rate, fail text), open a bowgun's ammo list, the fortress quest
  pause page.
- Not done: the GSTR sweep (section 3), the em10 trade port, the Pit_disp_quest line, and which quests have the trader
  on map 4.
