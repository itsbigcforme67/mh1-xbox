# MH Dos: the hunter's weapon classes (research, 4 Nov 2026)

What MH2 (JP 1.04) did to G's per-weapon player code, and how to get the hunter attacking in `--quest 1014`.
Read-only research: nothing compiled or run. **Source:** our disassembly (asm/mh2/game, asm/mh2/sub_main), their jump
tables (asm/mh2/*/data), config/mh2/symbols, config/mh2/game.yaml (which segments carry from G byte-identical),
config/mh2/func_pairs.txt, the owner's DATA.BIN name directory. Community mh2-decomp not opened.
Scripts used (stdlib, read-only): scratchpad/mh2pl/{disp,kindsw,kindcmp,fields,callsby,xref,table,mot}.py.

**Marks:** *read* = seen in the code or data as stated. *carried* = the MH2 bytes are built from our G C (game.yaml
`c` segment), so the G counterpart is certain. *guess* = an inference, with the reason given. Names `pl_atNNN` /
`pl_mvNNN` in config/mh2/symbols/game.txt that are not carried come from the name map's votes and can be wrong
(e.g. 0x6B6330 is called `pl_chat05` but is an SnS attack); this file says when a name is only a vote.

## 1. Weapon kinds (PLW+0x2, `pl->kind`, G's "job")

MH2 has 11 kinds, 0-10 (*read*: every per-kind jump table in the player code has 11 entries, `sltiu kind, 0xB`).
DATA.BIN entry 0 has one motion file per kind, w00_tbl.bin .. w10_tbl.bin (entries 4953-4963, *read*).

| kind | G | name | why |
|---|---|---|---|
| 0 | 0 | Great Sword | G's numbering kept: same G functions carried in its block (pl_at014), G's guard / roll paths |
| 1 | 1 | bowgun (light or heavy as in G) | shares one attack dispatcher and one attack-start helper with kind 5 (*read*), as in G (`kind == 1 \|\| kind == 5`) |
| 2 | 2 | Hammer | carried pl_at024/026/028/030/031/078, tame_pow_ck / tame_release (charge) |
| 3 | 3 | Lance | carried pl_at035/037/040/074, lance_kan_ck, guard attack in guard_atk_ck |
| 4 | 4 | Sword and Shield | carried pl_at051/052, we04_hit_sub. **The hunter of our `--quest 1014` runs** (logs: "weapon motions w04_tbl created") |
| 5 | 5 | the other bowgun | see kind 1 |
| 6 | 6 | Dual Swords (G's) | carried G pl_at091 (game 0x6B6E30 = G 0x55CC90), G pl_at083/085/089 at pair score 0.78-1.00 |
| 7 | new | **Long Sword** (*guess*, strong) | the 0-100 gauge at PLW 0x776 that hits fill (sub_main 0x3B5A90, only for kind 7), attacks gated on it (0x3B5B60 "gauge >= n", called 8 times from the kind-7 block); guard_atk_ck treats kind 7 like the Great Sword (roll only, no guard attack): MH1/G's long swords were Great Swords |
| 8 | new | **Hunting Horn** (*guess*, strong) | its attacks call Pl_vital_calc / Pl_max_vital_calc (heal / max-health songs) and Eft06_set; a 12-variant action (0x6BC480, args 0-11: songs?); ~40 new PLW bytes/shorts at 0x8BD-0x8F6 (note buffers and 11 s16 timers: song effects?); per-frame hooks from pl_move_sub (0x6BE9C0, 0x6BEAE0); no guard |
| 9 | new | **Gunlance** (*guess*, strong) | its own copy of lance_kan_ck, uses G's lance pl_at042 / pl_at065, guards (guard_atk_ck kind 9, the hit code's can-guard 0/3/4/9 in docs/mh2/hits.md), calls Pl_slash_calc from attacks (shelling costs sharpness) and tame_cnt_up (a charge: Wyvern's Fire?) |
| 10 | new | **Bow** (*guess*, strong) | Pl_shell_set / Shell_type_set (arrows are shells), pl_shell_sel has a kind-10 case (coatings), Pl_stamina_calc (charging drains stamina), Pl_view_reset (aim view); hits.md: kind 10 never loses sharpness and skips slash_adj_tbl |

The four new kinds match MH Dos's four new classes; which kind is which is a guess from behaviour, not from a name table.

## 2. Action dispatch: what MH2 changed

G (src/main/pl/f_pl.c) and MH2 keep the same outer layer: `pl_move` (game 0x722600) and `pl_move_sub_sub`
(0x722660) are carried from G, flag14 selects normal / attack / damage / die / demo / egg / chat, flag15 the action.

What changed is **flag14 = 1 (attack)**:

- **G:** one flat attack id space shared by all weapons: `pl_attack` is a switch over flag15 0x00-0x67 (MH1's ids,
  G's Dual Swords at 0x53-0x67). Every per-weapon choice elsewhere is a `switch (pl->kind)` inside a shared function
  (basic_atack_ck, ex_atk_ck, guard_atk_ck, job_special_com_ck, basic_com_ck, ...).
- **MH2:** `pl_attack` (game 0x71B830, 0x104 bytes; the map calls it game_func_0071B830) clears PLW 0x310, runs
  `pl_at020` if flag15 == 0x63 (a common attack for every weapon: G's id 0x14, started by `sw.trg & 1`), else jumps
  through jtbl 0x8ECB60 by kind to a **per-weapon attack dispatcher**. Each dispatcher switches flag15 in **its
  own id space** starting at 0. So attack ids are local to the weapon: SnS id 2 and Hammer id 2 are different moves.
- The per-kind switches in shared functions became **calls into per-weapon functions** that live in that weapon's
  block. MH2's game overlay starts with ten weapon blocks in kind order (0, 1/5, 2, 3, 4, 6, 7, 8, 9, 10; 0x6AF280-
  0x6C2880), each laid out as: attack actions, the attack dispatcher, the weapon's part of basic_atack_ck, its part of
  ex_atk_ck, then small "start attack N" stubs (0x10 bytes: Pl_act_set(pl, 1, N, 4) as a tail call).

pl_normal (flag14 0, game 0x71AF70) keeps G's 0x00-0x72 slots (same functions at the same ids, *read* against G's
switch) and adds 0x73-0x7D, mostly variants of existing moves with new arguments (pl_mv001(9), pl_mv071(6/7),
pl_mv093(2/3), pl_mv040/041/054/087(1), 0x7195B0(1)); 0x73 is new (0x71AD10, has a kind-8 test). Only 0x74 is started
from a weapon block (the Bow's 0x6C2600); the others come from shared search/unique/item code.

### Shared functions that dispatch by kind (MH2 game addresses, *read*)

| shared function (G name) | MH2 | per-kind targets (kind: function) | G form |
|---|---|---|---|
| pl_attack | 0x71B830, jtbl 0x8ECB60 | 0:6B1290 1:6B2390 2:6B3ED0 3:6B5340 4:6B6460 5:6B2390 6:6B7F20 7:6BA6B0 8:6BCAF0 9:6C0190 10:6C22B0 | flat switch 0x00-0x67 |
| basic_atack_ck (attack from the drawn stance: `sw.an_trg & 0x3C`, roll on `trg & 0x40` with stamina >= 75) | 0x70F3D0, jtbl 0x8EC780 | 0:6B1520 1:6B2480 2:6B4050 3:6B5490 4:6B6570 5:6B2480 6:6B81F0 7:6BA920 8:6BCDF0 9:6C0390 10:6C2440 | G 0x54CCE0, inline switch |
| ex_atk_ck (combo follow-ups, side steps 0x31/0x32, roll 0x1C, called from the attacks) | per weapon, no shared function | 0:6B1630 2:6B4100 3:6B5560 (lance hops 0x6B-0x6E) 4:6B6690 6:6B82E0 8:6BCEF0 9:6C04D0 10:6C2600; 7: 6BABB0 (*guess*, called from every kind-7 attack) | G 0x54C520, one function with a kind switch |
| basic_com_ck's draw attack (sheathed, attack button) | 0x711030 at 0x711988, jtbl 0x8EC7B0 | 0:6B1A20 1/5:6B2680(pl,1) 2:6B4590 3:6B5700 4:6B67B0 6:6B8470 7:6BB180 8:6BD510 9:6C0910 10:6C2810 | G inline: GS 3, lance 0x2E, hammer 0x20, SnS 0x3E |
| job_special_com_ck (R1) | 0x70E950, jtbl 0x8EC7?? (two tables, 11 and 10 entries) | 2:6B45B0 6:6B8490 7:6BB190 8:6BD520 10:6C2870; 0/3/4/9: Pl_act_set2(pl, 2, 3, 0) as G | G 0x54BC30 |
| guard_atk_ck | 0x70F510, compare chain | 0 and 7: roll only; 3: stick hop 7, `trg & 1` / attack -> 6B56B0(pl,0/1); 4: roll, `an_trg & 0x20` -> 6B67C0; 9: 6C0840 then hop 7 | G 0x54D2F0 (cases 0/3/4) |
| pl_mv028, pl_mv049 (moves that can turn into attacks) | 0x714800, 0x7160B0 | 2:6B45A0 4:6B67D0 6:6B8480 | G inline |
| pl_mv001 (walk start blend) | 0x7126A0, jtbl | data only: blend 4/6/8 by kind (0,3,7,9 -> 4; 2,8 -> 8; others 6) | G 7 cases |
| stick_pow_get | 0x70E0B0, jtbl 11 | data only | G 7 cases |
| pl_move_sub | 0x7227F0 | 8: 6BE9C0, 6BEAE0 each frame (*read* callers) | - |
| pl_dm001 / pl_dm003 / pl_dm008 | 0x71BA20 / 0x71C260 / 0x71CD50 | kind compares 1, 3, 4, 7, 9 (guard / guard-break reactions) | G 1, 3, 4 |

From other code: game 0x891F00 (a large effect-side function, *guess*) calls 6B0670 (GS), 6BD570 (Horn), 6C0920
(Gunlance), 6C0B00 (Bow); game 0x890C70 calls 6BEBD0 (Horn); game 0x894B80 calls 6C0920.

## 3. Per-weapon attack tables

One line per action function. "ids" are the MH2 flag15 values of that weapon (hex), with the argument in brackets
when not 0. "G ids" are G's flat ids for the same function (from G's pl_attack). CARRIED = MH2 bytes built from G C.
Function names other than CARRIED ones are votes or placeholders.

### kind 0 Great Sword: dispatcher 0x6B1290 (0x00-0x1C)

| MH2 | size | ids | G counterpart |
|---|---|---|---|
| 0x6AF280 pl_at000 | 0x464 | 0, 5-8 (1-4) | G pl_at000 (G ids 0, 0x12, 0x13, 0x40, 0x45) |
| 0x6AF6F0 pl_at001 | 0x4F0 | 1, 9-C (1-4) | G pl_at001 (1, 0xD, 0xF, 0x3F, 0x43) |
| 0x6AFBE0 | 0x8C0 | 2, D-12 (1-6) | G pl_at004 (*guess*: pair 0.61, same 7 variants) |
| 0x6B04A0 pl_at006 | 0x128 | 3 | G pl_at006 (6) |
| 0x6B05D0 pl_at014 | 0x98 | 4 | G pl_at014 (0xE), CARRIED |
| 0x6B06E0 | 0x448 | 13-19 (0-6) | new (no G pair); calls 0x6B0670 |
| 0x6B0BD0 | 0x6B8 | 1A-1C (0-2) | new (no G pair); calls 0x6B0B30 |
| hooks | | | basic_atack 0x6B1520, ex_atk 0x6B1630, draw attack 0x6B1A20 (id 0x14), 0x6B14A0, 0x6B1A70 / 0x6B1AD0 (a per-kind table D_45EE20, used by the gun's 0x6B2480) |

### kind 1 / 5 bowguns: dispatcher 0x6B2390 (0x00-0x08)

| MH2 | size | ids | G counterpart |
|---|---|---|---|
| 0x6B1C20 | 0x358 | 0 (1), 4, 5 (2), 8 (1) | G pl_at008 (G ids 2 (1), 8, 0xA (2); pair 0.67) |
| 0x6B1F80 pl_at009 | 0x130 | 1 | G pl_at009 (9) |
| 0x6B2250 pl_at012 | 0x98 | 2 | G pl_at012 (0xC) |
| 0x6B22F0 pl_at061 | 0x98 | 3 | G pl_at061 (0x3D), CARRIED |
| 0x6B20B0 | 0x1A0 | 6, 7 (1) | G pl_at009-like (pair 0.73), *guess* a new variant |
| hooks | | | blend_set 0x6B1B20 (shared by the gun attacks), basic_atack 0x6B2480, draw attack 0x6B2680 |

### kind 2 Hammer: dispatcher 0x6B3ED0 (0x00-0x13)

| MH2 | size | ids | G counterpart |
|---|---|---|---|
| 0x6B28D0 pl_at022 | 0x140 | 0 | G pl_at022 (0x16) |
| 0x6B2A10 pl_at023 | 0x2DC | 1, B (1) | G pl_at023 (0x17, 0x4B) |
| 0x6B2CF0 pl_at078 | 0x20C | 2 | G pl_at078 (0x4E), CARRIED (MH2V next id 19) |
| 0x6B2F00 pl_at024 | 0x234 | 3, C-E (1-3) | G pl_at024 (0x18, 0x1B, 0x4C, 0x67), CARRIED |
| 0x6B3140 pl_at026 | 0xC8 | 4 | G pl_at026 (0x1A), CARRIED |
| 0x6B3210 pl_at028 | 0x370 | 5 | G pl_at028 (0x1C), CARRIED (MH2V ids 17, 9) |
| 0x6B3580 pl_at029 | 0x384 | 6, F-12 (1-4) | G pl_at029 (0x1D, 0x21, 0x39, 0x3A, 0x51) |
| 0x6B3910 pl_at030 | 0x130 | 7 | G pl_at030 (0x1E), CARRIED |
| 0x6B3A40 pl_at032 | 0x1C8 | 8 | G pl_at032 (0x20) |
| 0x6B3C10 pl_at077 | 0x194 | 9, 13 (1) | G pl_at077 (0x4D) plus a new variant |
| 0x6B3DB0 pl_at031 | 0x120 | A | G pl_at031 (0x1F), CARRIED (MH2V id 18) |
| hooks | | | we02_hit_sub 0x6B2790 CARRIED (MH2V id 10), basic_atack 0x6B4050, ex_atk_ck 0x6B4100, tame_pow_ck / tame_release CARRIED, 0x6B43F0 (G tame_com_ck, pair 0.65), starters 0x6B4590 (id 8), 0x6B45A0 / 0x6B45B0 (id 3) |

### kind 3 Lance: dispatcher 0x6B5340 (0x00-0x10)

| MH2 | size | ids | G counterpart |
|---|---|---|---|
| 0x6B4630 pl_at036 | 0x2DC | 0, 8-B (1-4) | G pl_at036 (0x24, 0x2E, 0x35, 0x36, 0x52) |
| 0x6B4910 pl_at045 | 0x22C | 1, C, D (1, 2) | G pl_at045 (0x2D, 0x3B, 0x3C) |
| 0x6B4B40 pl_at040 | 0xF8 | 2 | G pl_at040 (0x28), CARRIED |
| 0x6B4C40 pl_at034 | 0x2F8 | 3, E (1) | G pl_at034 (0x22, 0x26) |
| 0x6B4F40 pl_at035 | 0xCC | 4, F (1) | G pl_at035 (0x23, 0x2F), CARRIED |
| 0x6B5010 pl_at037 | 0x114 | 5, 10 (1) | G pl_at037 (0x25, 0x27), CARRIED (MH2V id 14) |
| 0x6B5130 pl_at074 | 0xD8 | 6 | G pl_at074 (0x4A), CARRIED |
| 0x6B5210 | 0x128 | 7 | G pl_at065 (0x41; pair 0.99: the lance's own renumbered copy) |
| hooks | | | lance_kan_ck 0x6B45C0 CARRIED (MH2V id 7), basic_atack 0x6B5490, lance ex_atk 0x6B5560 (hops 0x6B-0x6E), guard attack 0x6B56B0, draw attack 0x6B5700 (id 8). G's pl_at041 / pl_at042 / pl_at043 are not in the lance's table (pl_at042 went to the Gunlance) |

### kind 4 Sword and Shield: dispatcher 0x6B6460 (0x00-0x0C)

| MH2 | size | ids | G counterpart |
|---|---|---|---|
| 0x6B57E0 pl_at048 | 0x440 | 0, 6-8 (1-3) | G pl_at048 (0x30, 0x37, 0x38, 0x47; G's arg 4 / id 0x48 is gone). Same motions 0x579/0x57A/0x57B, same frames 24/20/64/50; next ids 0x37 -> 6, 0x38 -> 7, 0x33 -> 2; ex_atk_ck -> 0x6B6690 (*read*) |
| 0x6B5C20 pl_at049 | 0x274 | 1, 9, A (1, 2) | G pl_at049 (0x31, 0x3E, 0x50) |
| 0x6B5EA0 pl_at051 | 0x100 | 2 | G pl_at051 (0x33), CARRIED |
| 0x6B5FA0 pl_at052 | 0xB0 | 3 | G pl_at052 (0x34), CARRIED |
| 0x6B6050 | 0x2DC | 4, B (1), C (1) | *guess* new (no pair; twice G pl_at050's size) |
| 0x6B6330 ("pl_chat05", vote) | 0x128 | 5 | G pl_at050 (0x32) (*guess*: the carried we04_hit_sub goes to G 0x32 / MH2 5) |
| hooks | | | we04_hit_sub 0x6B5710 CARRIED (MH2V(0x32, 5)), basic_atack 0x6B6570 (ids 0xC, 2, 0, 1(?), roll 0x1C), ex_atk 0x6B6690 (side steps 0x31/0x32, roll), draw attack 0x6B67B0 (id 9 = G 0x3E), shield attack 0x6B67C0 (id 3 = G 0x34, from guard_atk_ck), 0x6B67D0 (id 0xB, from pl_mv028/049) |

### kind 6 Dual Swords: dispatcher 0x6B7F20 (0x00-0x14)

| MH2 | size | ids | G counterpart |
|---|---|---|---|
| 0x6B68B0 | 0x578 | 0, 9-B (1-3) | G pl_at085 (G 0x55C4C0, pair 1.00; G ids 0x55-0x58) |
| 0x6B6E30 | 0x15C | 1, C-E (1-3) | G pl_at091 (0x55CC90), CARRIED |
| 0x6B6F90 | 0x2AC | 2, F, 10 (1, 2) | *guess* G pl_at093 (order; no pair) |
| 0x6B7240 ("pl_at070", vote) | 0x22C | 3, 11 (1) | *guess* G pl_at094 (order) |
| 0x6B7470 | 0x174 | 4, 12, 13 (1, 2) | G pl_at083 (0x55C160, pair 0.78) |
| 0x6B75F0 | 0x4DC | 5, 14 (1) | new? (no pair) |
| 0x6B7AD0 | 0x288 | 6 | G pl_at089 (0x55CA40, pair 0.95) |
| 0x6B7D60 ("pl_mv037") | 0x90 | 7 | bytes equal to G's pl_mv037 with MH2V (motion 1412, then normal 4): *guess* MH2's form of G pl_at102 |
| 0x6B7DF0 ("pl_at050", vote) | 0x128 | 8 | *guess* G pl_at084 |
| hooks | | | we04_hit_sub copy 0x6B67E0 (DS numbering), ds_atk_set-like 0x6B80B0, basic_atack 0x6B81F0, ex_atk 0x6B82E0, starters 0x6B8470 (id 0xF), 0x6B8480 (id 0x11), R1 0x6B8490 |

### kind 7 Long Sword (new): dispatcher 0x6BA6B0 (0x00-0x1F, 32 ids)

| MH2 | size | ids |
|---|---|---|
| 0x6B85B0 | 0x4BC | 0, C (0), B, D (1), 11 (2), 13 (3) |
| 0x6B8A70 | 0x298 | 1, 18 (1) |
| 0x6B8D10 | 0x3A8 | 2, 5, 6, 15, 16, 19, 1A (0-6) |
| 0x6B90C0 | 0x128 | 3 (pair G pl_at031 0.97) |
| 0x6B91F0 | 0x1B8 | 4 (pair G pl_at077 0.67) |
| 0x6B93B0 | 0x4A4 | 7, 12, 1C-1F (0-5) |
| 0x6B9860 | 0x208 | 8 (pair G pl_at051 0.60) |
| 0x6B9A70 | 0x2B8 | 9, 1B (1) |
| 0x6B9D30 | 0x1C0 | A |
| 0x6B9EF0 | 0x278 | E, 14 (1) |
| 0x6BA170 | 0x398 | F, 17 (1) |
| 0x6BA510 | 0x198 | 10 |
| hooks | | 0x6B84E0, 0x6BAA80, 0x6BAAE0, 0x6BABB0 (called from nearly every kind-7 attack: hit sub / gauge / ex_atk, *guess*), basic_atack 0x6BA920, draw attack 0x6BB180 (id 0x13), R1 0x6BB190 (asks the gauge, 0x3B5B60(pl, 15)) |

### kind 8 Hunting Horn (new): dispatcher 0x6BCAF0 (0x00-0x27, 40 ids; id 8 does nothing)

| MH2 | size | ids |
|---|---|---|
| 0x6BB4B0 | 0x51C | 0-3 (0-3), 4-7 (0-3), 16, 24, 27 (3) |
| 0x6BB9D0 | 0x2FC | 9, D, 15, 19, 25 (0-4) |
| 0x6BBCD0 | 0x198 | A |
| 0x6BBE70 | 0x180 | B |
| 0x6BBFF0 | 0x204 | C, 23 (1) |
| 0x6BC350 | 0x12C | E, 26 (1) (pair G pl_at042 0.60) |
| 0x6BC480 | 0x364 | F-12, 17, 1C-22 (args 0-11: *guess* the songs) |
| 0x6BC7F0 | 0x1BC | 13, 1A, 1B (0-2) |
| 0x6BC9B0 | 0x140 | 14 |
| 0x6BC200 | 0x148 | 18 |
| hooks | | 0x6BB240, 0x6BB390, 0x6BD1A0, 0x6BCEF0 (ex_atk), basic_atack 0x6BCDF0, draw attack 0x6BD510 (id 0x19, Pl_act_set2), R1 0x6BD520, 0x6BD570 / 0x6BD6C0 (called 55 times; named CngNetAQInit by a wrong vote) / 0x6BD720 (0xEE8, the biggest) / 0x6BE610-0x6BEBD0 (song state, per-frame from pl_move_sub) |

### kind 9 Gunlance (new): dispatcher 0x6C0190 (0x00-0x1B)

| MH2 | size | ids |
|---|---|---|
| 0x6BECF0 | 0x294 | 0, 8-B (1-4) (lance pl_at036's id pattern) |
| 0x6BF090 | 0x490 | 1, C, D, 17-1A (1-6) |
| 0x6BF520 | 0x1EC | 2, F, 10, 15, 16 (1-4) |
| 0x6BF800 | 0x2E0 | 3 |
| 0x6BFAE0 | 0x168 | 4, 11, 12 (1, 2) |
| 0x6BFC50 | 0xEC | 5, 1B (1) |
| 0x6BFD40 | 0x26C | 6, 14 (1) |
| 0x6BFFB0 pl_at065 | 0x128 | 7, CARRIED from G pl_at065 (MH2V next id 11) |
| 0x6C00E0 pl_at042 | 0xB0 | E, 13 (1) (G pl_at042, 0x2A; vote name) |
| hooks | | 0x6BEC30, lance_kan_ck copy 0x6BEC60, starters 0x6BECD0 / 0x6BECE0 (Pl_act_set2 flag14 2), basic_atack 0x6C0390, ex_atk 0x6C04D0 (lance hops 0x6B-0x6E), guard 0x6C0840, draw attack 0x6C0910 (id 0x14), 0x6C0920 (from effect code) |

### kind 10 Bow (new): dispatcher 0x6C22B0 (0x00-0x14)

| MH2 | size | ids |
|---|---|---|
| 0x6C0F90 | 0x2D4 | 0, E (1) |
| 0x6C1270 | 0x3B4 | 1, 9, B, F, 13, 14 (1-5) |
| 0x6C1630 | 0x90 | 2 |
| 0x6C16C0 | 0x2D4 | 3, D (1) |
| 0x6C19A0 | 0xB8 | 4 |
| 0x6C1A60 | 0x19C | 5, 7 (1) |
| 0x6C1C00 | 0x160 | 6 |
| 0x6C1D60 | 0x22C | 8, A, 12 (1, 2) |
| 0x6C1F90 | 0xA0 | C |
| 0x6C2030 pl_at025 | 0x90 | 10, CARRIED (G pl_at025 with MH2V: motion 1411, then normal 4) |
| 0x6C20C0 | 0x1F0 | 11 |
| hooks | | 0x6C0AB0, 0x6C0B00, 0x6C0C20, 0x6C0C50, 0x6C0E70, lance_kan_ck copy 0x6C0F20, basic_atack 0x6C2440, ex_atk 0x6C2600 (goes to normal 0x74), draw attack 0x6C2810, R1 0x6C2870 |

## 4. New PLW fields (MH2 offsets; PLW is 0x970)

From the field accesses in each weapon block (any base register except sp: a few may belong to other structs),
plus hits.md. "only k" = not touched by the G-derived blocks 0-6.

| offset | used by | what (guess unless *read*) |
|---|---|---|
| 0x776 s16 | 7 (sub_main 0x3B5A90 / 0x3B5B60) | gauge 0-100 (*read*: clamped, filled from hits, tested by attacks); Long Sword spirit gauge |
| 0x598 s16 | 7 | set to 900 when the gauge fills (*read*), with Eft06_set once: glow timer |
| 0x629 s8 | 7 | counter, capped at 25 when the gauge fills (*read*) |
| 0x416 u8 | only 7 | state byte of the kind-7 block |
| 0x8A2 / 0x8A3 s8 | all | cut % / blunt % from a 2-byte-per-kind table D_45EE00 (sub_main 0x3B5BA0; hits.md) |
| 0x8BD-0x8C8 bytes | only 8 | note / song buffer (8 bytes written in a row) |
| 0x8D6-0x8E0 bytes, 0x8E2-0x8F6 s16 (11 shorts) | only 8 | per-song state and timers (11 = the number of songs? *guess*) |
| 0x224 u8, 0x409 u8, 0x682 u8, 0x706 s16 | only 8 | not identified |
| 0x775 u8 | only 9 and 10 | not identified |
| 0x8C7 u8 | 8, 9 | not identified |
| 0x38C u16 | only 9 | PLSW+0x20 = sw.pow[0] (stick power): a read of the pad block, not a new field |
| 0x77E s16 | only 10 | inside `item[]` as include/mh2_plw.h has it (0x77C): either item[0].num or that view is off; check |
| 0x8B9 u8 | only 10 | not identified |
| 0x184 s32 | 8, 9, 10 | a word inside the part area (0xD0 + ...): not identified |
| 0x310 u8 | pl_attack | cleared on every attack frame (G's work350) |

include/mh2_plw.h stops at 0x8A2; 0x8A2-0x96F is MH2's new tail and holds the Horn's state. G's Dual Swords demon flag
(G work93D) is not placed for MH2 yet.

## 5. Motion sets

- One file per kind: wNN_tbl.bin, NN = kind, 00-10 (*read*: DATA.BIN entry 0 names 4953-4963; sizes 143 KB (w04) to
  229 KB (w08)). plcom_tbl.bin (entry 4947) holds the common sets.
- create_pl_motion (sub_main 0x2E4B60) is our f_frame.c, carried: 6 banks of 100, player n at handle 710 + 300n.
  Weapon motion ids are 1000 + bank*100 + i, the attacks mostly 1401-1431 (bank 4), draw / walk 1006-1019; plcom ids
  stay below 1000 (*read*: pl_chr_set2 constants in each block). So a new weapon needs only its wNN file through the
  same loader: the host's weapon_motions (src/pc/viewer.c) already builds "w%02d_tbl.bin" from the kind.
- SnS attacks use the same motion ids in MH2 as in G (pl_at048: 0x579/0x57A/0x57B, *read*).

## 6. G code MH2 kept, changed, dropped

- **Kept byte-identical (CARRIED):** pl_move, pl_move_sub_sub, pl_turn_sub / pl_horm_sub, pl_at014 / 020 / 024 / 026
  / 028 / 030 / 031 / 035 / 037 / 040 / 051 / 052 / 061 / 065 / 074 / 078 / 091, we02_hit_sub, we04_hit_sub,
  tame_pow_ck / tame_release, lance_kan_ck, ~35 pl_mv / pl_dm / pl_demo / pl_egg / pl_chat functions (list in
  config/mh2/game.yaml `mhg_main_pl_f_pl_*` and mhg/main/pl/f_pl_p*). Several of them carry only with MH2V(G, MH2)
  constants in src/main/pl/f_pl.c: those constants are **MH2's local attack ids** (e.g. we04_hit_sub `MH2V(0x32, 5)`,
  pl_at065 `MH2V(0x52, 11)`, pl_at028 `MH2V(0x3A, 17)`), and two (pl_at025, pl_mv037) carry onto a different MH2
  function (the Bow's id 0x10, the DS's id 7) with that function's motion (1411 / 1412).
- **Changed:** pl_attack (per-kind dispatch), basic_atack_ck / ex_atk_ck / guard_atk_ck / job_special_com_ck /
  basic_com_ck (per-kind calls), the G attacks that set a next attack id (renumbered; most also gained or lost
  variants, e.g. pl_at048 lost arg 4), pl_normal (+11 ids), pl_dm001/003/008 (kinds 7 and 9).
- **Dropped:** G's flat attack ids (0x14 moved to 0x63; the rest are per weapon). G pl_at041 / pl_at043 have no slot
  in the lance table (not looked for elsewhere).

## 7. Port plan: the hunter attacks in quest 1014 first

Our `--quest 1014` hunter is kind 4 (Sword and Shield; rt_player.c's default weapon 156 read through MH2's
Ken_data / Battle_type gives kind 4, and the logs load w04_tbl). The PC build compiles G's f_pl.c with
-DVERSION_MH2, so today it runs **G's** attack numbering while the MH2V constants in carried functions already use
**MH2's**: e.g. after a shield hit, we04_hit_sub sets attack 5, which G's pl_attack has no case for (to normal).
Order:

1. **Look first (hidden run, low CPU):** `--quest 1014 --input "idle*30,triangle*1,idle*60"` (button token names:
   src/pc/pad/pad_sdl.c) with RT_QUEST_TRACE, and print flag14 / flag15 / char0 per tick. Expect G's path: G
   basic_atack_ck case 4 -> G ids 0x30-0x33 -> G pl_at048 with MH2's w04 motions. If the swing plays, step 2 is
   about correctness, not about getting a first swing.
2. **pl_attack in MH2's form under VERSION_MH2** (f_pl.c): flag15 0x63 -> pl_at020, else a switch on kind calling
   one dispatcher per kind; kinds without a port yet fall back to pl_to_normal. Write only kind 4's dispatcher
   (0x6B6460, the 13 ids in section 3) at first.
3. **basic_atack_ck, ex_atk_ck, basic_com_ck's draw attack, guard_atk_ck, pl_mv028/049's starters for kind 4** in
   MH2's form: 0x6B6570, 0x6B6690, 0x6B67B0, 0x6B67C0, 0x6B67D0 (small; ids from section 3). The trg&1 attack
   becomes 0x63. The other kinds keep G's code until ported.
4. **Kind 4's attacks with MH2 ids:** pl_at048 and pl_at049 under VERSION_MH2 (next ids 0x37 -> 6, 0x38 -> 7,
   0x33 -> 2, 0x3E -> 9, 0x50 -> A; ex_atk_ck -> the step 3 function); 0x6B6330 (G pl_at050's slot, id 5) and the new
   0x6B6050 (ids 4, B, C) written from the asm. pl_at051 / pl_at052 / we04_hit_sub are already MH2's. Check each with
   tools/mh2_own.py byte match where it compiles, or as near-matches (*_nm.c) otherwise.
5. **Hit the monster:** the shells (shell00_set per swing) and the hit path of docs/mh2/hits.md: sharpness at 0x7DF,
   cut/blunt at 0x8A2/0x8A3 (sub_main 0x3B5BA0), per-kind sound and meat tables. Test on stage 120 with
   RT_MH2_EM_HERE=1 once the Rathian is reachable.
6. **Kinds 0-3, 5, 6** the same way (dispatcher + hooks + renumbered G attacks; their CARRIED functions are done).
   Hammer and Lance have the most CARRIED functions; Dual Swords need work93D placed.
7. **New kinds 7-10** from the asm, one at a time (Long Sword first: smallest new state, 0x776 / 0x598 / 0x629, and the
   two sub_main gauge helpers; then Gunlance, which reuses lance code; Bow needs shells and coatings; Horn last: the
   most state and songs). Each needs its wNN motions only through the existing loader.
8. Before any of this lands, keep MH1 and G byte-identical (all MH2 changes behind VERSION_MH2) and rerun mh2_check.

## 8. Not checked / open

- Which bowgun is kind 1 and which kind 5 (same as G; not looked at here).
- The names of kinds 7-10 (behaviour-based guesses, section 1); the meaning of the Horn's fields and of 0x775, 0x8B9,
  0x77E, 0x184.
- Most non-carried G counterparts in kinds 0, 1 and 6 are guesses from size, argument count and file order.
- job_special_com_ck's two jump table addresses were not written down (in 0x70E950, bounds 11 and 10).
- Kind 7's ex_atk part (0x6BABB0 is the candidate) and whether its Pl_act_set ids are reached through a register.
- The guard rules for kind 7 in the damage code (pl_dm* compare 7 with 3 and 9, while hits.md's can-guard test leaves
  7 out): not followed through.
