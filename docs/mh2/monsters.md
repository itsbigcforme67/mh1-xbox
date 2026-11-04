# MH Dos: the 74 monster kinds (research, 4 Nov 2026)

Research only: nothing built, run or committed. Written from our own reading of the owner's disc (SLPM_662.80 and the overlays
carved from DATA.BIN, read with small scratch scripts and rabbitizer) and from our G/MH1 C and docs. Community mh2-decomp not
opened. Function names are ours (G/MH1 names through docs/survey/mh2_map.csv); unnamed ones are sub_<addr>. How each claim is
backed, so the reader knows what to trust:

- **[table]** read straight from a per-kind table in the executable (addresses below). Solid.
- **[text]** the species is named in the quest text of the mission files whose large-monster list holds that kind (all 329
  quests, all six .mib variants each, scanned). Solid for the kinds that are a quest's only large monster.
- **[G]** the same number means this species in G (docs/pc.md, docs/RELEASE_G.md, docs/FINDS.md) and MH2's tables agree (same
  model / motion files, same code family). Good evidence, not proof.
- **[guess]** a reasoned guess from the data (stages it lives on, hit points, files shared, code family). Marked every time.

## Per-kind tables used

| table | address | what |
|---|---|---|
| kind main (em_move dispatch) | gm_sub 0x904AB0, jump table 0x932FC0 | one `jal` per kind; kinds share a case when they share code |
| kind init (em_init_sub) | gm_sub 0x903EE0, jump table 0x932E90 | same grouping as the main; first it stores em_prog_tbl[kind] in EMW+0x3D0 and calls slot 0 |
| em_prog_tbl | gm_sub 0x90DE10 | per kind a table of 7 words: [0] [1] local init (equal everywhere), [2] another per-kind hook (an empty function for some), [3] effect move, then **3 slots MH2 added**: [4] a per-kind value (default gm_sub 0x90A9C0 returns 0; em_cmd opcode 0x79 reads it), [5] for kind 1 a tail call to sub_7E7290 (the map calls it em01_init; meaning of the slot not traced), [6] only on some big kinds (kind 1's tests mode/act; guess: "busy" state test) |
| model / motion / effect model | sub_main 0x4216F0 / 0x421820 / 0x421950 | AFS indexes in DATA.BIN entry 0, by kind (load_enemy_model 0x334D30, load_em_motion 0x334A50, load_enemy_sub_model 0x334D90, all called from em_create_model 0x33C690). -1 = no effect model. There is no separate texture table: the em\*_tex.bin files sit next to the models |
| model part rows | sub_main 0x42E690 / 0x42E7C0 | per-kind pointers em_create_model passes to em_model_work_set / model_work_set (main / effect model). Not decoded |
| em_cmd scripts | gm_sub 0x932C70 | 26 pointers (em_cmd level, game_w+0x30), each to 74 words: an em_cmd.afs entry (DATA.BIN entry 6587, 53 files) per kind; loaded by gm_sub 0x90AB00 (rt_mh2.c already does this) |
| em_hit_push_tbl | game 0x8B9FC0 | wall spheres by kind (pointer listed) |
| hunt_group_tbl | sub_main 0x44C800 | per kind a 0xFF-ended list: the kinds that count as the same species (subspecies) |
| em_base_size | sub_main 0x44C930 | f32 per kind (0 for small monsters) |
| em_vital_tbl | gm_sub 0x90DB30 | per kind, s16 hit points per variant; the HP column is variant 0, before the level rate (inits and quest entries can override: kind 10's init writes 200) |

All monster mains but one call em_cmd_ck through a small MH2 wrapper (game sub_709A90, 30 callers, one per code family): em10
(kind 10) is the only family that never runs a command script, so its em_cmd column means nothing.

## Table 1: code

"G match" is the main's class in docs/survey/mh2_map.csv: *changed* = the same function with edits (ngram / exact pairing to the
G or MH1 address given), *same shape* = same instructions with other offsets, **new** = MH2-only code. Kinds with the same main
share all their code (the init column shows the same address too). Init and program-slot functions are almost all unnamed
(mh2-only or ambiguous in the map); exceptions: em07_init, em18_init, em29_init, em33_init, em33_local_init.

| kind | species | main | G match | init | em_prog_tbl entry: slots [0]..[6] |
|---|---|---|---|---|---|
| 0 | none (dummy row) | 0x7F5360 | em01_main, changed (MH1 0x574080) | - | 0x90DF90: 90A9A0 90A9A0 90A9B0 90A9B0 90A9C0 0 0 |
| 1 | Rathian [text] | 0x7F5360 | em01_main, changed (MH1 0x574080) | 0x7E7810 | 0x8C9210: 7E7250 7E7250 7F8240 7F8020 90A9C0 7E7950 7F8250 |
| 2 | Fatalis [text] | 0x7FC2A0 | em02_main, changed (G 0x5AA390) | 0x7FBAE0 | 0x8C9C40: 8044E0 8044E0 8044F0 804490 90A9C0 8044F0 0 |
| 3 | Kelbi [G] | 0x7C2070 | em03_main, changed (MH1 0x587780) | 0x7C2020 | 0x8C8B10: 7C4C00 7C4C00 7C4C10 7C5300 90A9C0 7C2060 0 |
| 4 | Mosswine [G] | 0x7C7B50 | em04_main, changed (MH1 0x58E070) | 0x7C5560 | 0x8C8BB0: 7C8010 7C8010 7C8020 90A900 7C8030 7C55A0 0 |
| 5 | Bullfango [G] | 0x7C7B50 | em04_main, changed (MH1 0x58E070) | 0x7C5560 | 0x8C8BB0: 7C8010 7C8010 7C8020 90A900 7C8030 7C55A0 0 |
| 6 | Yian Kut-Ku [text] | 0x7836A0 | em20_main, changed (G 0x623A80) | 0x778230 | 0x8C58D0: 777C40 777C40 786EE0 786AB0 90A9C0 778270 0 |
| 7 | Lao-Shan Lung [text] | 0x7CEB10 | em07_main, changed (G 0x5BEB80) | 0x7CB880 | 0x8C8BF0: 7CB330 7CB330 7D2560 7D2260 90A9C0 7D2560 0 |
| 8 | Cephadrome [text] | 0x7DB7D0 | em08_main, changed (G 0x5CDFD0) | 0x7D4620 | 0x8C8DE0: 7D4450 7D4450 7DE230 7DDE80 90A9C0 7D4700 0 |
| 9 | Felyne [G] | 0x7E3AF0 | em09_main, changed (MH1 0x5AB1F0) | 0x7E05D0 | 0x8C90E0: 7E4650 7E4650 7E4660 7E4360 90A9C0 7E0690 0 |
| 10 | camp NPC (G's trader) [G, data] | 0x7991D0 | em10_main, same shape (MH1 0x5AEAB0) | 0x797F90 | 0x8C7290: 799350 799350 799360 799300 90A9C0 799360 0 |
| 11 | Rathalos [text] | 0x7F5360 | em01_main, changed (MH1 0x574080) | 0x7E7810 | 0x8C9210: 7E7250 7E7250 7F8240 7F8020 90A9C0 7E7950 7F8250 |
| 12 | Aptonoth [G] | 0x79E320 | em12_main, changed (G 0x5DF350) | 0x79A350 | 0x8C7320: 79EFC0 79EFC0 79EFD0 79EF70 79EFE0 79A390 0 |
| 13 | Genprey [G] | 0x7C0140 | em16_main, changed (G 0x602D00) | 0x7BA830 | 0x8C8920: 7BA470 7BA470 7C0B50 7C0AA0 7C0AF0 7BA8A0 0 |
| 14 | Diablos [text] | 0x7A8320 | em14_main, changed (MH1 0x5BBC70) | 0x7A1850 | 0x8C73B0: 7A1660 7A1660 7AB280 7AB220 90A9C0 7A1890 7AB290 |
| 15 | Khezu [text] | 0x7B6160 | em15_main, changed (G 0x5F6BF0) | 0x7AE5B0 | 0x8C7C40: 7AE410 7AE410 7B8700 7B8690 7B8710 7AE6A0 0 |
| 16 | Velociprey [G] | 0x7C0140 | em16_main, changed (G 0x602D00) | 0x7BA830 | 0x8C8920: 7BA470 7BA470 7C0B50 7C0AA0 7C0AF0 7BA8A0 0 |
| 17 | Gravios [G, guess] | 0x76C910 | em17_main, changed (MH1 0x5E1530) | 0x765030 | 0x8C4B70: 764E60 764E60 76F1A0 76F150 90A9C0 7650E0 76F1B0 |
| 18 | cart Felyne [G] | 0x770C70 | em18_main, changed (MH1 0x5E74E0) | 0x7704D0 | 0x8C5350: 770FA0 770FA0 770FB0 770F50 90A9C0 770FB0 0 |
| 19 | Vespoid [G] | 0x7744D0 | em19_main, changed (MH1 0x5EAA40) | 0x7715C0 | 0x8C5380: 7748F0 7748F0 774900 7748A0 90A9C0 771600 0 |
| 20 | Gypceros [text] | 0x7836A0 | em20_main, changed (G 0x623A80) | 0x778230 | 0x8C58D0: 777C40 777C40 786EE0 786AB0 90A9C0 778270 0 |
| 21 | Plesioth [text] | 0x792460 | em21_main, changed (G 0x635F90) | 0x78A190 | 0x8C6240: 789FF0 789FF0 795210 794EC0 90A9C0 78A270 0 |
| 22 | Basarios [text] | 0x76C910 | em17_main, changed (MH1 0x5E1530) | 0x765030 | 0x8C4B70: 764E60 764E60 76F1A0 76F150 90A9C0 7650E0 76F1B0 |
| 23 | Melynx [G] | 0x7E3AF0 | em09_main, changed (MH1 0x5AB1F0) | 0x7E05D0 | 0x8C90E0: 7E4650 7E4650 7E4660 7E4360 90A9C0 7E0690 0 |
| 24 | Hornetaur [G] | 0x7744D0 | em19_main, changed (MH1 0x5EAA40) | 0x7715C0 | 0x8C5380: 7748F0 7748F0 774900 7748A0 90A9C0 771600 0 |
| 25 | Apceros [G, guess] | 0x79E320 | em12_main, changed (G 0x5DF350) | 0x79A350 | 0x8C7320: 79EFC0 79EFC0 79EFD0 79EF70 79EFE0 79A390 0 |
| 26 | Monoblos [text] | 0x7A8320 | em14_main, changed (MH1 0x5BBC70) | 0x7A1850 | 0x8C73B0: 7A1660 7A1660 7AB280 7AB220 90A9C0 7A1890 7AB290 |
| 27 | Velocidrome [text] | 0x73B930 | em27_main, changed (G 0x6407A0) | 0x736930 | 0x8C3F50: 7366C0 7366C0 73C6C0 73C670 90A9C0 736970 0 |
| 28 | Gendrome [text] | 0x73B930 | em27_main, changed (G 0x6407A0) | 0x736930 | 0x8C3F50: 7366C0 7366C0 73C6C0 73C670 90A9C0 736970 0 |
| 29 | breakable target [G] | 0x73D580 | em29_main, changed (MH1 0x6145D0) | 0x73D060 | 0x8C41A0: 73D770 73D770 73D770 73D730 90A9C0 73D770 0 |
| 30 | Ioprey [G] | 0x7C0140 | em16_main, changed (G 0x602D00) | 0x7BA830 | 0x8C8920: 7BA470 7BA470 7C0B50 7C0AA0 7C0AF0 7BA8A0 0 |
| 31 | Iodrome [text] | 0x73B930 | em27_main, changed (G 0x6407A0) | 0x736930 | 0x8C3F50: 7366C0 7366C0 73C6C0 73C670 90A9C0 736970 0 |
| 32 | em04-family small kind [G, unknown] | 0x7C7B50 | em04_main, changed (MH1 0x58E070) | 0x7C5560 | 0x8C8BB0: 7C8010 7C8010 7C8020 90A900 7C8030 7C55A0 0 |
| 33 | Kirin [text] | 0x73E480 | em33_main, changed (G 0x642FB0) | 0x73E160 | 0x8C41C0: 741510 741510 741550 741B30 90A9C0 741550 0 |
| 34 | Cephalos [G] | 0x7DB7D0 | em08_main, changed (G 0x5CDFD0) | 0x7D4620 | 0x8C8DE0: 7D4450 7D4450 7DE230 7DDE80 90A9C0 7D4700 0 |
| 35 | Giaprey [guess] | 0x7C0140 | em16_main, changed (G 0x602D00) | 0x7BA830 | 0x8C8920: 7BA470 7BA470 7C0B50 7C0AA0 7C0AF0 7BA8A0 0 |
| 36 | Fatalis variant [text: volcano] | 0x7FC2A0 | em02_main, changed (G 0x5AA390) | 0x7FBAE0 | 0x8C9C40: 8044E0 8044E0 8044F0 804490 90A9C0 8044F0 0 |
| 37 | Pink Rathian [text] | 0x7F5360 | em01_main, changed (MH1 0x574080) | 0x7E7810 | 0x8C9210: 7E7250 7E7250 7F8240 7F8020 90A9C0 7E7950 7F8250 |
| 38 | Blue Kut-Ku [text] | 0x7836A0 | em20_main, changed (G 0x623A80) | 0x778230 | 0x8C58D0: 777C40 777C40 786EE0 786AB0 90A9C0 778270 0 |
| 39 | Purple Gypceros [text] | 0x7836A0 | em20_main, changed (G 0x623A80) | 0x778230 | 0x8C58D0: 777C40 777C40 786EE0 786AB0 90A9C0 778270 0 |
| 40 | Yian Garuga [text] (G: Higeruu) | 0x74DFC0 | **new** | 0x744A00 | 0x8C42F0: 744440 744440 7509F0 750970 90A9C0 744AA0 750A00 |
| 41 | Rathalos variant, no quest [G] | 0x7F5360 | em01_main, changed (MH1 0x574080) | 0x7E7810 | 0x8C9210: 7E7250 7E7250 7F8240 7F8020 90A9C0 7E7950 7F8250 |
| 42 | Rathian variant, no quest [G] | 0x7F5360 | em01_main, changed (MH1 0x574080) | 0x7E7810 | 0x8C9210: 7E7250 7E7250 7F8240 7F8020 90A9C0 7E7950 7F8250 |
| 43 | Black Diablos [text] | 0x7A8320 | em14_main, changed (MH1 0x5BBC70) | 0x7A1850 | 0x8C73B0: 7A1660 7A1660 7AB280 7AB220 90A9C0 7A1890 7AB290 |
| 44 | White Monoblos [text] | 0x7A8320 | em14_main, changed (MH1 0x5BBC70) | 0x7A1850 | 0x8C73B0: 7A1660 7A1660 7AB280 7AB220 90A9C0 7A1890 7AB290 |
| 45 | Red Khezu [text] | 0x7B6160 | em15_main, changed (G 0x5F6BF0) | 0x7AE5B0 | 0x8C7C40: 7AE410 7AE410 7B8700 7B8690 7B8710 7AE6A0 0 |
| 46 | Green Plesioth [text] | 0x792460 | em21_main, changed (G 0x635F90) | 0x78A190 | 0x8C6240: 789FF0 789FF0 795210 794EC0 90A9C0 78A270 0 |
| 47 | Black Gravios [text] | 0x76C910 | em17_main, changed (MH1 0x5E1530) | 0x765030 | 0x8C4B70: 764E60 764E60 76F1A0 76F150 90A9C0 7650E0 76F1B0 |
| 48 | Daimyo Hermitaur [text] (G: Basarios variant) | 0x7588C0 | **new** | 0x758850 | 0x8C4B10: 760A60 760A60 760AB0 761B20 761B70 758840 761CF0 |
| 49 | Azure Rathalos [text] | 0x7F5360 | em01_main, changed (MH1 0x574080) | 0x7E7810 | 0x8C9210: 7E7250 7E7250 7F8240 7F8020 90A9C0 7E7950 7F8250 |
| 50 | Ashen Lao-Shan [text] | 0x7CEB10 | em07_main, changed (G 0x5BEB80) | 0x7CB880 | 0x8C8BF0: 7CB330 7CB330 7D2560 7D2260 90A9C0 7D2560 0 |
| 51 | Blangonga [text] (G: Someruu) | 0x817CD0 | **new** | 0x80D150 | 0x8C9E40: 80CE00 80CE00 81AA80 81AA30 81AA90 80D260 81AD80 |
| 52 | Congalala [text] | 0x817CD0 | **new** | 0x80D150 | 0x8C9E40: 80CE00 80CE00 81AA80 81AA30 81AA90 80D260 81AD80 |
| 53 | Rajang [text, guess] | 0x817CD0 | **new** | 0x80D150 | 0x8C9E40: 80CE00 80CE00 81AA80 81AA30 81AA90 80D260 81AD80 |
| 54 | Kushala Daora [text] | 0x836470 | **new** | 0x82D510 | 0x8CA240: 82CEF0 82CEF0 838F90 838E10 90A9C0 82D690 838FA0 |
| 55 | fortress dragon [text, unnamed] | 0x825B80 | **new** | 0x823160 | 0x8CA0E0: 822F60 822F60 8281E0 827FD0 8281F0 8281E0 0 |
| 56 | unknown small, new | 0x83AE60 | **new** | 0x839350 | 0x8CA2D0: 83B210 83B210 83B250 83B1C0 90A9C0 8393E0 0 |
| 57 | unknown small, new | 0x83F680 | **new** | 0x83CC20 | 0x8CA350: 8402D0 8402D0 8402E0 840220 840270 83CCF0 0 |
| 58 | Yama Tsukami [text] | 0x8489D0 | **new** | 0x844A30 | 0x8CAAE0: 844770 844770 849C50 849800 849C60 849C50 0 |
| 59 | Chameleos [text] | 0x851C90 | **new** | 0x84BF70 | 0x8CABE0: 84BD70 84BD70 853600 8535B0 853610 84BFB0 8536A0 |
| 60 | Rusted Kushala [text, guess] | 0x836470 | **new** | 0x82D510 | 0x8CA240: 82CEF0 82CEF0 838F90 838E10 90A9C0 82D690 838FA0 |
| 61 | Blango [guess] | 0x817CD0 | **new** | 0x80D150 | 0x8C9E40: 80CE00 80CE00 81AA80 81AA30 81AA90 80D260 81AD80 |
| 62 | Conga [guess] | 0x817CD0 | **new** | 0x80D150 | 0x8C9E40: 80CE00 80CE00 81AA80 81AA30 81AA90 80D260 81AD80 |
| 63 | Gabras / Remobra [guess] | 0x856DF0 | **new** | 0x854590 | 0x8CAE50: 857810 857810 857820 857440 90A9C0 8545D0 0 |
| 64 | Lunastra [text] | 0x836470 | **new** | 0x82D510 | 0x8CA240: 82CEF0 82CEF0 838F90 838E10 90A9C0 82D690 838FA0 |
| 65 | Teostra [text] | 0x836470 | **new** | 0x82D510 | 0x8CA240: 82CEF0 82CEF0 838F90 838E10 90A9C0 82D690 838FA0 |
| 66 | Hermitaur [guess] | 0x7588C0 | **new** | 0x758850 | 0x8C4B10: 760A60 760A60 760AB0 761B20 761B70 758840 761CF0 |
| 67 | Shogun Ceanataur [text] | 0x7588C0 | **new** | 0x758850 | 0x8C4B10: 760A60 760A60 760AB0 761B20 761B70 758840 761CF0 |
| 68 | Bulldrome [text] | 0x85BA20 | **new** | 0x858B90 | 0x8CB4C0: 85BF10 85BF10 85BF20 90A900 90A9C0 858BD0 0 |
| 69 | Anteka [guess] | 0x7C2070 | em03_main, changed (MH1 0x587780) | 0x7C2020 | 0x8C8B10: 7C4C00 7C4C00 7C4C10 7C5300 90A9C0 7C2060 0 |
| 70 | Popo [guess] | 0x79E320 | em12_main, changed (G 0x5DF350) | 0x79A350 | 0x8C7320: 79EFC0 79EFC0 79EFD0 79EF70 79EFE0 79A390 0 |
| 71 | Fatalis variant, no quest [data] | 0x7FC2A0 | em02_main, changed (G 0x5AA390) | 0x7FBAE0 | 0x8C9C40: 8044E0 8044E0 8044F0 804490 90A9C0 8044F0 0 |
| 72 | Yama Tsukami variant, no quest [data] | 0x8489D0 | **new** | 0x844A30 | 0x8CAAE0: 844770 844770 849C50 849800 849C60 849C50 0 |
| 73 | Ceanataur [guess] | 0x7588C0 | **new** | 0x758850 | 0x8C4B10: 760A60 760A60 760AB0 761B20 761B70 758840 761CF0 |

## Table 2: data

Files as `AFS index name` (DATA.BIN entry 0). em_cmd: the em_cmd.afs entry at level 6, the level all of quests 1001-1016 use,
then the set of entries over all 26 levels when they differ. Entry 0 (em01_cmd1) also fills the rows of kinds that have no
script of their own (0, 10): it looks like a zero default, not a real choice (guess).

| kind | model | motion | effect model | em_cmd (level 6) | hit push | hunt group | base size | HP (vital 0) |
|---|---|---|---|---|---|---|---|---|
| 0 | 4765 em01_amh | 4899 em01_tbl | - | 0 em01_cmd1 (levels use 0, 4, 5) | 0x8B9D70 | 0 | 0 | 5000 |
| 1 | 4765 em01_amh | 4899 em01_tbl | 4721 em01_eft_amh | 3 em01_cmd6 (levels use 0, 1, 2, 3, 4, 5, 6) | 0x8B9D70 | 1 42 37 | 1645.6 | 5000 |
| 2 | 4767 em02_amh | 4900 em02_tbl | 4721 em01_eft_amh | 7 em02_cmd | 0x8B9D90 | 2 36 71 | 0 | 26666 |
| 3 | 4769 em03_amh | 4901 em03_tbl | - | 8 em03_cmd | 0x8B9DB0 | 3 | 0 | 60 |
| 4 | 4771 em04_amh | 4902 em04_tbl | - | 9 em04_cmd | 0x8B9DD0 | 4 | 0 | 50 |
| 5 | 4773 em05_amh | 4903 em05_tbl | - | 9 em04_cmd | 0x8B9DD0 | 5 | 0 | 200 |
| 6 | 4775 em06_amh | 4904 em06_tbl | 4723 em06_eft_amh | 22 em20_cmd6 (levels use 0, 20, 21, 22, 23) | 0x8B9D70 | 6 38 | 919.8 | 2000 |
| 7 | 4777 em07_amh | 4905 em07_tbl | - | 10 em07_cmd | 0x8B9DF0 | 7 50 | 0 | 26666 |
| 8 | 4779 em08_amh | 4906 em08_tbl | 4725 em08_eft_amh | 11 em08_cmd | 0x8B9D70 | 8 | 1538.3 | 2000 |
| 9 | 4781 em09_amh | 4907 em09_tbl | - | 12 em09_cmd | 0x8B9DD0 | 9 | 0 | 40 |
| 10 | 4783 em10_amh | 4908 em10_tbl | - | 0 em01_cmd1 | 0x8B9DD0 | 10 | 0 | 10000 |
| 11 | 4785 em11_amh | 4909 em11_tbl | 4721 em01_eft_amh | 3 em01_cmd6 (levels use 0, 1, 2, 3, 4, 5, 6) | 0x8B9D70 | 11 41 49 | 1629.4 | 4000 |
| 12 | 4787 em12_amh | 4910 em12_tbl | - | 13 em12_cmd | 0x8B9E10 | 12 | 0 | 80 |
| 13 | 4789 em13_amh | 4913 em16_tbl | - | 16 em16_cmd | 0x8B9E70 | 13 | 0 | 100 |
| 14 | 4791 em14_amh | 4911 em14_tbl | 4721 em01_eft_amh | 14 em14_cmd | 0x8B9E30 | 14 43 | 1993.4 | 7000 |
| 15 | 4793 em15_amh | 4912 em15_tbl | 4727 em15_eft_amh | 15 em15_cmd | 0x8B9E50 | 15 45 | 873.2 | 4000 |
| 16 | 4795 em16_amh | 4913 em16_tbl | - | 16 em16_cmd | 0x8B9E70 | 16 35 | 0 | 70 |
| 17 | 4797 em17_amh | 4914 em17_tbl | 4729 em17_eft_amh | 17 em17_cmd1 (levels use 0, 17) | 0x8B9E90 | 17 47 | 2099.9 | 5000 |
| 18 | 4799 em18_amh | 4915 em18_tbl | - | 18 em18_cmd | 0x8B9EB0 | 18 | 0 | 10 |
| 19 | 4801 em19_amh | 4916 em19_tbl | - | 19 em19_cmd | 0x8B9EC0 | 19 | 0 | 40 |
| 20 | 4803 em20_amh | 4917 em20_tbl | 4731 em20_eft_amh | 22 em20_cmd6 (levels use 0, 20, 21, 22, 23) | 0x8B9EE0 | 20 39 | 1013.7 | 3000 |
| 21 | 4805 em21_amh | 4918 em21_tbl | 4733 em21_eft_amh | 24 em21_cmd | 0x8B9F00 | 21 46 | 2315.2 | 5000 |
| 22 | 4807 em22_amh | 4919 em22_tbl | 4723 em06_eft_amh | 17 em17_cmd1 (levels use 0, 17) | 0x8B9E90 | 22 | 1297.6 | 3000 |
| 23 | 4809 em23_amh | 4907 em09_tbl | - | 12 em09_cmd | 0x8B9DD0 | 23 | 0 | 50 |
| 24 | 4811 em24_amh | 4916 em19_tbl | - | 19 em19_cmd | 0x8B9EC0 | 24 | 0 | 50 |
| 25 | 4813 em25_amh | 4910 em12_tbl | - | 13 em12_cmd | 0x8B9E10 | 25 | 0 | 200 |
| 26 | 4815 em26_amh | 4911 em14_tbl | 4721 em01_eft_amh | 14 em14_cmd | 0x8B9E30 | 26 44 | 2004.2 | 6000 |
| 27 | 4795 em16_amh | 4913 em16_tbl | - | 29 em27_cmd6 (levels use 25, 26, 27, 28, 29, 30) | 0x8B9E70 | 27 | 738.4 | 1000 |
| 28 | 4789 em13_amh | 4913 em16_tbl | - | 29 em27_cmd6 (levels use 25, 26, 27, 28, 29, 30) | 0x8B9E70 | 28 | 732.1 | 1500 |
| 29 | 4817 em29_amh | 4920 em29_tbl | - | 31 em29_cmd | 0x8B9F20 | 29 | 0 | None |
| 30 | 4819 em30_amh | 4913 em16_tbl | - | 16 em16_cmd | 0x8B9E70 | 30 | 0 | 150 |
| 31 | 4819 em30_amh | 4913 em16_tbl | - | 29 em27_cmd6 (levels use 25, 26, 27, 28, 29, 30) | 0x8B9E70 | 31 | 774.6 | 2000 |
| 32 | 4821 em32_amh | 4902 em04_tbl | - | 9 em04_cmd | 0x8B9DD0 | 32 | 0 | 1 |
| 33 | 4823 em33_amh | 4921 em33_tbl | 4735 em33_eft_amh | 32 em33_cmd (levels use 8, 32, 33) | 0x8B9DB0 | 33 | 464.3 | 4000 |
| 34 | 4779 em08_amh | 4906 em08_tbl | 4725 em08_eft_amh | 11 em08_cmd | 0x8B9D70 | 34 | 0 | 300 |
| 35 | 4825 em35_amh | 4913 em16_tbl | - | 16 em16_cmd | 0x8B9E70 | 16 35 | 0 | 100 |
| 36 | 4827 em36_amh | 4922 em36_tbl | 4721 em01_eft_amh | 34 em36_cmd8 | 0x8B9D90 | 2 36 71 | 0 | 20000 |
| 37 | 4829 em37_amh | 4899 em01_tbl | 4721 em01_eft_amh | 3 em01_cmd6 (levels use 0, 1, 2, 3, 4, 5, 6) | 0x8B9D70 | 1 42 37 | 1645.6 | 5500 |
| 38 | 4831 em38_amh | 4904 em06_tbl | 4723 em06_eft_amh | 22 em20_cmd6 (levels use 0, 20, 21, 22, 23) | 0x8B9D70 | 6 38 | 919.8 | 3500 |
| 39 | 4833 em39_amh | 4917 em20_tbl | 4731 em20_eft_amh | 22 em20_cmd6 (levels use 0, 20, 21, 22, 23) | 0x8B9EE0 | 20 39 | 1013.7 | 3500 |
| 40 | 4835 em40_amh | 4923 em40_tbl | 4721 em01_eft_amh | 36 em40_cmd6 (levels use 35, 36) | 0x8B9D70 | 40 | 1290.0 | 6000 |
| 41 | 4837 em41_amh | 4909 em11_tbl | 4721 em01_eft_amh | 3 em01_cmd6 (levels use 0, 1, 2, 3, 4, 5, 6) | 0x8B9D70 | 11 41 49 | 1629.4 | 5000 |
| 42 | 4839 em42_amh | 4899 em01_tbl | 4721 em01_eft_amh | 3 em01_cmd6 (levels use 0, 1, 2, 3, 4, 5, 6) | 0x8B9D70 | 1 42 37 | 1645.6 | 7000 |
| 43 | 4841 em43_amh | 4911 em14_tbl | 4721 em01_eft_amh | 14 em14_cmd | 0x8B9E30 | 14 43 | 1993.4 | 7500 |
| 44 | 4843 em44_amh | 4911 em14_tbl | 4721 em01_eft_amh | 14 em14_cmd | 0x8B9E30 | 26 44 | 2004.2 | 7500 |
| 45 | 4845 em45_amh | 4924 em45_tbl | 4727 em15_eft_amh | 15 em15_cmd | 0x8B9D70 | 15 45 | 873.2 | 4500 |
| 46 | 4847 em46_amh | 4918 em21_tbl | 4733 em21_eft_amh | 24 em21_cmd | 0x8B9F00 | 21 46 | 2315.2 | 5500 |
| 47 | 4849 em47_amh | 4914 em17_tbl | 4729 em17_eft_amh | 17 em17_cmd1 (levels use 0, 17) | 0x8B9E90 | 17 47 | 2099.9 | 5500 |
| 48 | 4851 em48_amh | 4925 em48_tbl | 4737 em48_eft_amh | 37 em48_cmd | 0x8B9D70 | 48 | 1044.0 | 3000 |
| 49 | 4853 em49_amh | 4909 em11_tbl | 4721 em01_eft_amh | 3 em01_cmd6 (levels use 0, 1, 2, 3, 4, 5, 6) | 0x8B9D70 | 11 41 49 | 1629.4 | 4500 |
| 50 | 4855 em50_amh | 4905 em07_tbl | - | 10 em07_cmd | 0x8B9DF0 | 7 50 | 0 | 26666 |
| 51 | 4857 em51_amh | 4926 em51_tbl | 4739 em51_eft_amh | 38 em51_cmd | 0x8B9F40 | 51 | 860.0 | 3500 |
| 52 | 4859 em52_amh | 4927 em52_tbl | 4741 em52_eft_amh | 39 em52_cmd | 0x8B9F40 | 52 | 984.0 | 3500 |
| 53 | 4861 em53_amh | 4928 em53_tbl | 4743 em53_eft_amh | 38 em51_cmd | 0x8B9F40 | 53 | 960.0 | 4500 |
| 54 | 4863 em54_amh | 4929 em54_tbl | 4745 em54_eft_amh | 40 em54_cmd | 0x8B9D70 | 54 60 | 1577.0 | 6000 |
| 55 | 4865 em55_amh | 4930 em55_tbl | 4747 em55_eft_amh | 41 em55_cmd | 0x8B9D70 | 55 | 1000.0 | 20000 |
| 56 | 4867 em56_amh | 4931 em56_tbl | - | 42 em56_cmd | 0x8B9F60 | 56 | 0 | 40 |
| 57 | 4869 em57_amh | 4932 em57_tbl | 4749 em57_eft_amh | 43 em57_cmd | 0x8B9DD0 | 57 | 0 | 300 |
| 58 | 4871 em58_amh | 4933 em58_tbl | 4751 em58_eft_amh | 44 em58_cmd | 0x8B9F80 | 58 72 | 0 | 15000 |
| 59 | 4873 em59_amh | 4934 em59_tbl | 4753 em59_eft_amh | 45 em59_cmd1 | 0x8B9D70 | 59 | 1744.0 | 7000 |
| 60 | 4875 em60_amh | 4929 em54_tbl | 4745 em54_eft_amh | 40 em54_cmd | 0x8B9D70 | 54 60 | 1744.0 | 6000 |
| 61 | 4877 em61_amh | 4935 em61_tbl | 4739 em51_eft_amh | 38 em51_cmd | 0x8B9F40 | 61 | 0 | 150 |
| 62 | 4859 em52_amh | 4936 em62_tbl | - | 39 em52_cmd | 0x8B9F40 | 62 | 0 | 200 |
| 63 | 4879 em63_amh | 4937 em63_tbl | - | 46 em63_cmd | 0x8B9FA0 | 63 | 0 | 200 |
| 64 | 4881 em64_amh | 4938 em65_tbl | 4755 em64_eft_amh | 47 em64_cmd | 0x8B9D70 | 64 | 1740.0 | 6000 |
| 65 | 4883 em65_amh | 4938 em65_tbl | 4755 em64_eft_amh | 47 em64_cmd | 0x8B9D70 | 65 | 1740.0 | 6000 |
| 66 | 4885 em66_amh | 4939 em66_tbl | - | 48 em66_cmd | 0x8B9D70 | 66 | 0 | 150 |
| 67 | 4887 em67_amh | 4940 em67_tbl | 4757 em67_eft_amh | 49 em67_cmd | 0x8B9D70 | 67 | 863.0 | 4000 |
| 68 | 4889 em68_amh | 4941 em68_tbl | - | 50 em68_cmd | 0x8B9DD0 | 68 | 566.0 | 1500 |
| 69 | 4891 em69_amh | 4942 em69_tbl | - | 8 em03_cmd | 0x8B9DB0 | 69 | 0 | 120 |
| 70 | 4893 em70_amh | 4943 em70_tbl | - | 13 em12_cmd | 0x8B9E10 | 70 | 0 | 160 |
| 71 | 4895 em71_amh | 4944 em71_tbl | 4759 em71_eft_amh | 51 em71_cmd15 | 0x8B9D90 | 2 36 71 | 0 | 20000 |
| 72 | 4871 em58_amh | 4945 em72_tbl | 4761 em72_eft_amh | 52 em72_cmd | 0x8B9D70 | 58 72 | 0 | 15000 |
| 73 | 4897 em73_amh | 4946 em73_tbl | - | 48 em66_cmd | 0x8B9D70 | 73 | 0 | 150 |

## Notes by family (why each species is named as it is)

MH2 keeps G's numbering for kinds 0-50 except **40 and 48**, and adds 51-73. Families below are kinds that share one main.

- **em01 family (1, 11, 37, 41, 42, 49): Rathian and Rathalos.** One main (em01_main, changed from MH1's), one init 0x7E7810.
  Hunt groups {1, 42, 37} and {11, 41, 49}. Quest text names Rathian for 1 and 37 (37's quests say 桜 = Pink Rathian), Rathalos
  for 11, and 49's quests say 蒼 (Azure Rathalos) [text]. 41 and 42 are in no quest on the disc (G's other two variants) [G].
  Each has its own model (em37/41/42/49_amh) on the em01 or em11 motion file. 7 em_cmd scripts by level (em01_cmd1..13).
- **em02 (2, 36, 71): Fatalis.** 2's quest is at Schrade castle, 36's has the black dragon flee to the volcano [text]; 71 is in
  no quest (a third Fatalis model, em71, with its own script em71_cmd15) [table; which variant is a guess].
- **em03 (3, 69): Kelbi and a new snow herbivore.** 69 uses em03's code on its own model and lives only on stages 92-109 (the
  snow stages: Blangonga's big stages are 94-106) with 120 HP: **Anteka** [guess].
- **em04 (4, 5, 32): Mosswine, Bullfango** [G]; 32 has 1 HP, its own model and no quest [G: one of G's em04 kinds].
- **em20 (6, 20, 38, 39): Yian Kut-Ku, Gypceros and their variants** (Blue Kut-Ku 38, Purple Gypceros 39) [text]. 38 uses 6's
  motions, 39 uses 20's.
- **em07 (7, 50): Lao-Shan Lung and Ashen Lao-Shan** (50's quest: a grey rock-like dragon at the fort) [text].
- **em08 (8, 34): Cephadrome, Cephalos** [text, G].
- **em09 (9, 23): Felyne, Melynx** [G: 40 and 50 HP as in G].
- **em10 (10): the camp NPC, G's trader.** Same-shape main as MH1's em10_main. In 237 quests, two entries on the base-camp
  stage of each map (110 in the jungle), one of which is kept by a random quest byte (Quest_next_em_set's kind-10 rule). No
  em_cmd. See "Kind 10 in act 0" below.
- **em12 (12, 25, 70): Aptonoth, Apceros [G] and a new herbivore.** 70 lives only on the snow stages 94-108, 160 HP: **Popo**
  [guess; quests that mention Popo hold 70].
- **em16 (13, 16, 30, 35): Genprey, Velociprey, Ioprey [G], and 35.** 35 shares 16's hunt group and motions, lives on the snow
  stages and a few others: **Giaprey** [guess; G's 35 joined the Velociprey pack too].
- **em27 (27, 28, 31): Velocidrome, Gendrome, Iodrome** [text]; their models are the em16/em13/em30 ones, as in G.
- **em14 (14, 26, 43, 44): Diablos, Monoblos, Black Diablos, White Monoblos** [text].
- **em15 (15, 45): Khezu, Red Khezu** [text].
- **em17 (17, 22, 47): Gravios, Basarios, Black Gravios** [text for 22 and 47; 17 by G and its group {17, 47}]. In G 48 was a
  Basarios variant; MH2 gave 48 to a new monster and 22's group is {22} alone.
- **em18 (18): the cart Felynes** [G]. No quest entry (the cart is spawned by the game itself).
- **em19 (19, 24): Vespoid, Hornetaur** [G]. 19 is in 295 quests, the most of any kind.
- **em21 (21, 46): Plesioth, Green Plesioth** [text].
- **em29 (29): the breakable target** [G: em29.c]; no vital row (its init takes hit points from its own table, as in G).
- **em33 (33): Kirin** [text]. Two scripts (em33_cmd, em33_cmd8).
- **40: Yian Garuga, new code** [text: quest 6001, 黒狼鳥]. G's kind 40 was the unused cut-down Felyne "Higeruu" (docs/FINDS.md);
  MH2 reuses the number for Garuga (as MH Portable did). Main 0x74DFC0 (2204 bytes), own model em40_amh, Rathian effect model,
  6000 HP, scripts em40_cmd / em40_cmd6. **The PC's stand-in is wrong for it**: rt_mh2.c em_family[40] = 40 runs G's Felyne code.
- **48 / 66 / 67 / 73 family (main 0x7588C0): the crabs.** 48 Daimyo Hermitaur and 67 Shogun Ceanataur [text]; 66 and 73 are the
  small ones (150 HP each, no effect model, own models): **66 Hermitaur** (jungle and desert beaches; quests that name Hermitaur
  hold 66) and **73 Ceanataur** (quests that name Ceanataur hold 73) [guess].
- **51 / 52 / 53 / 61 / 62 family (main 0x817CD0): Blangonga, Congalala [text], Rajang [guess: its quests describe a golden
  monster as strong as an elder dragon], Blango and Conga [guess].** 61 (150 HP) lives only on the snow stages with Blangonga's
  effect model; 62 (200 HP) uses Congalala's model on its own motion file. G's 51 was "Someruu" (name only, no code).
- **54 / 60 / 64 / 65 family (main 0x836470): Kushala Daora (54), Lunastra (64), Teostra (65) [text], 60 [guess: Rusted
  Kushala]**: 60 has its own model on Kushala's motions and effects, is in Kushala's hunt group {54, 60}, and its two quests name
  Kushala Daora. 64 / 65 share a motion file (em65_tbl) and an effect model (em64_eft), as a pair of the same species would.
- **55 (main 0x825B80): a fortress dragon** [text: its three quests send it against the fort as a "mysterious giant dragon",
  unnamed]; 20000 HP, own model and effects, big stages 14 / 79 (Lao-Shan's) and 176 / 180. Species not settled.
- **56 (main 0x83AE60) and 57 (main 0x83F680): two new small monsters, not identified.** 56: 40 HP, no effect model, 77 quests
  on many maps. 57: 300 HP, own effect model, 83 quests. Neither is named in any quest text the scan matched.
- **58 / 72 (main 0x8489D0): Yama Tsukami** [text] (15000 HP). 72 uses 58's model with its own motions, effects and script and
  is in no quest [table].
- **59 (main 0x851C90): Chameleos** [text].
- **63 (main 0x856DF0): Gabras (English: Remobra)** [guess: the one quest whose text names it, 1070, has 63 as its only new small
  kind besides 73 (Ceanataur, which that text names too); 200 HP; its main is closest in shape to em19_main, a flier].
- **68 (main 0x85BA20): Bulldrome** [text], built on Bullfango: its init (0x858B90) is instruction for instruction the em04
  init (0x7C5560), and program slot [3] is em04's effect move (gm_sub 0x90A900); the main is new.
- **0: a dummy row** (em01's case and files, no init, slots that only call eft01_set). Never made.

Kinds in no quest on the disc (all six .mib variants of all 329 quests): 0, 18, 32, 41, 42, 71, 72.

## Quest 1014 and the first village quests

Quest numbers 1001-1087 are taken as the village set (guess from their elder texts; 1014 is the Rathian quest the PC tests); 1001-1016 below were taken in number order
(the in-game order of the elder's list was not read). All 16 are on the jungle (stages 110-120, start 110) and use em_cmd level 6.
Large monsters (kind, stage) from the mission's +0x18 block; small monsters from the +0x14 per-stage lists (identical in all
three choices for these quests).

| quest | large monster | its em_cmd entry |
|---|---|---|
| 1001 | none (deliver jungle mushrooms) | - |
| 1002, 1003 | 27 Velocidrome (stage 117) | 29 em27_cmd6 |
| 1004, 1005 | 68 Bulldrome (115) | 50 em68_cmd |
| 1006, 1008 | 6 Yian Kut-Ku (119) | 22 em20_cmd6 |
| 1007 | 38 Blue Kut-Ku (119) | 22 em20_cmd6 |
| 1009 | 48 Daimyo Hermitaur (120) | 37 em48_cmd |
| 1010 | 39 Purple Gypceros (119) | 22 em20_cmd6 |
| 1011, 1012 | 52 Congalala (120) | 39 em52_cmd |
| 1013 | 46 Green Plesioth (113) | 24 em21_cmd |
| **1014** | **1 Rathian (120)** | **3 em01_cmd6** |
| 1015 | 37 Pink Rathian (120) | 3 em01_cmd6 |
| 1016 | 1 Rathian (120), with an egg delivery | 3 em01_cmd6 |

Small monsters, the same in every one of them, by stage: 110 kind 10 (two entries, one kept); 111 Aptonoth 12 x5, Vespoid 19;
112 Kelbi 3 x7; 113 Vespoid 19, Hermitaur 66 x2; 114 66 x2, Mosswine 4 x2; 115 4 x3; 116 Bullfango 5, 4 x3, 19; 117 19 x2,
Velociprey 16 x3; 118 Hornetaur 24, 19 x6; 119 16 x5; 120 66 x4. So **every village quest needs 3, 4, 5, 10, 12, 16, 19, 24 and
66**; all but 66 are G families.

## Kind 10 in act 0

host.md and shared5.md saw the kind-10 monster animate (motion 1001) but never leave act 0, with its pose field x2D4 at 0. Cause
(read from the code, not run): **the PC runs G's em10_init, not MH2's.** The host's em_init_sub is G's (src/main/em/f_em_nm.c
case 10), and G's em10_init (src/game/em/em10_nm.c) has start points only for G's village stages 5, 16 and 41; on any other stage
(110 here) it takes the default branch: it puts the NPC at (3600 - 80 x slot, 0, 3200 - 80 x slot), over the position the quest
entry gave it, and leaves the pose at 0. In em10's act 0 a pose of 0 means "stand until a hunter comes close and talks", so it
stands for good. MH2's own init for kind 10 (game 0x797F90, 384 bytes) has no stage switch at all: it keeps the entry's position
and angle as the home point, **sets the pose (MH2 EMW+0x294, G's x2D4) to 1**, sets 200 hit points, and picks the motion by the
pose; with pose 1 act 0's timer hands over to the wander acts (G's 0xA / 0xB), as G's pose 1 does. So the stillness is a stand-in
artefact, not MH2 behaviour, and the NPC is also standing in the wrong place. em10's main does not run em_cmd, so the em_cmd.afs
entry 0 it gets is unused. Fix: MH2's 0x797F90 as C (it is short) or, as a stand-in, an MH2 branch in em10_init that keeps the
entry position and sets pose 1. Its talk / trade tables on MH2 were not read.

## Port plan, by what the player sees first

1. **Kind 10's init** (above): small, and the NPC is at the camp of 237 quests.
2. **The Rathian (1; 37 and 42 come free)**: em_cmd for MH2 (docs/mh2/em_cmd.md section 4, under way), then its MH2 init 0x7E7810
   and program slots [5] / [6]. Target of 1014, 1015, 1016.
3. **The jungle's small monsters** (3, 4, 5, 12, 16, 19, 24): G families with "changed" mains; the work is checking each against
   MH2's EMW offsets and its edits, then the MH2 inits (most are 56-byte stubs that set up and call the shared code: cheap).
4. **The crab family, main 0x7588C0** (66 Hermitaur in every village quest and 152 in all; 48 Daimyo Hermitaur, 1009; 67 Shogun
   Ceanataur, 15 quests; 73 Ceanataur): the most-used new code. Main 3828 bytes plus its acts.
5. **The next village targets in order**: 27 Velocidrome (G em27; 1002/1003), 68 Bulldrome (new main 0x85BA20 on em04's init and
   effect code; 1004/1005, 22 quests), 6 / 38 / 39 (G em20; 1006-1010), 52 Congalala (new family 0x817CD0, which also brings
   Blangonga 51, Rajang 53, Blango 61, Conga 62 for the snow maps; 1011/1012), 46 (G em21; 1013).
6. **Other new small monsters on many maps**: 63 Gabras (108 quests), 57 (83), 56 (77), 35 Giaprey / 69 / 70 (G families, snow
   maps): short mains (480-688 bytes).
7. **The other G big monsters** in MH2's quests (8, 11, 14, 15, 17, 20, 21, 22, 26, 28, 31, 33, 43-47, 49, 2, 7, 36, 50): G families
   already on the PC, each needing the MH2-offset check of step 3.
8. **Elder dragons**: family 0x836470 (54 Kushala, 60, 64 Lunastra, 65 Teostra; together about 54 quests), 59 Chameleos (17),
   then 40 Yian Garuga (one urgent quest, 6001; first stop rt_mh2.c's wrong em40 stand-in), 58 Yama Tsukami (4), 55 the fortress
   dragon (3), 53 Rajang (2).
9. Nothing to do for 0, 18 (spawned by the cart code, check with the cart), 32, 41, 42, 71, 72.

Until each family is ported, rt_mh2.c's em_family stand-ins stay as they are except kind 40 (G's 40 is a Felyne; a stand-in of 0,
or G's em20 as the nearest shape, guess, is better than the cat).

## What is guessed or open

- Species marked [guess] above (17 by G only; 35, 53, 60, 61, 62, 63, 66, 69, 70, 73; which Fatalis 36 and 71 are); 55, 56 and 57
  are not identified. The scan matched ≪name≫ and plain katakana names in quest text only; the hunting-log name table (G's
  sub_main 0x527A40) was not found in MH2's executable (the names are not plain Shift-JIS strings there), so it was not used.
- Program slots [4] [5] [6]: what reads [5] and [6] was not traced.
- The model part tables 0x42E690 / 0x42E7C0 and em_hit_push_tbl's contents were not decoded.
- That 1001-1087 are the elder's village quests is assumed from their texts and the PC's use of 1014; the list order was not read.
- Kind 10: the fix is read from the code; no run confirmed it.
