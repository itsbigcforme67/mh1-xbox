# G on the PC: where MH1's logic still runs (GB6 triage, 4 Nov 2026)

RELEASE_G.md G6 / GB6 says 945 functions the G PC enters in ordinary play run MH1 C where G's code differs. This file
re-checks that list against today's code, ranks what is left by what a player would notice, and suggests an order of
work for the G agents. Research only: nothing was built or run (low-CPU rules); every finding is from reading G's asm
(tools/mhg_fdiff.py, tools/gdraft.py), the PC C and the build tree. Basis tags as in RELEASE_G.md: [asm] = read in G's
asm, [code] = read in the PC source, [inferred] = reasoned, not seen, [not checked].

## 1. Where the list comes from, and how it was re-checked

- **Source**: docs/survey/mhg_cov_g.txt (15 Oct 2026, agent A). A `COV=1 tools/build_pc_mhg.sh` build (gcc
  -finstrument-functions, src/pc/rt/rt_cov.c) ran the hidden runs listed in RELEASE_G.md section 3 (title, NEW GAME,
  creation, save, village intro, CONTINUE, tools/test_gvillage.py's 16 shop / forge / box cases, the elder's lists,
  15 quests clear + time-out, test_revival_g.sh). `tools/cov_report_g.py` maps each entered function to
  docs/survey/mhg_map.csv (G twin class: shape / edited / rewritten / missing) and keeps the ones whose PC body has no G
  marker (VERSION_MHG, rt_ver_mhg, RTVA, UDO, MHG, or sitting in the G branch of an `#if`).
- **Re-check (today's G PC binary, build/mhg/pc/mhview_g of 3 Nov 23:01)**: the same marker test on the same 945 names,
  plus two things the old test missed:
  1. **Bug in cov_report_g.py**: it takes every `t` symbol from `nm -l` as a function start, including gcc's local
     labels (`.L1234`, one per switch case of a jump table). A function's body was cut at its first case label, so any
     G code further down was not seen (pl_attack's G Dual Swords cases, quest_condition_prog, em17_main ...). Fix: skip
     names starting with `.L` when building `starts` / `end_of`. That alone moves 17 functions to "G".
  2. **G hooks through macros**: many G ports switch behaviour through a macro defined in a `VERSION_MHG` block above
     the function (em_area_move_init's `G_AM(...)`, enemy_mv's `EMU_KIND_27`, Pl_damage_sub's `PB`, pl_mv071's
     `PL_MV071_*`). Counted as G when the body uses a macro defined under VERSION_MHG in its own file. Header macros
     that only carry G layouts (CW_OFS, PNET_OFS, LBSYS_OFS, IBO, ITEM_ROW ...) are counted separately as "layout only".
  3. Static functions gcc inlined (165 names absent from the binary): looked up in the files the binary is built from.
- **Result** (945 names):

| Now | Count | Meaning |
|---|---|---|
| G | 267 | the PC body is G's (ported since 15 Oct, or missed by the old test) |
| not linked | 4 | result_disp, movie_add_ck (G's own reward module f_reward_g.c replaced them), lm_mail_box_i, em_atk04_00581480 |
| MH1, online only | 74 | lobby client, plaza, chat, member lists, net patch: nothing to see offline |
| MH1, small | 375 | < 10 instructions differ once registers / immediates are masked. Almost all are layout (G's struct offsets, which the PC's G headers already carry) or texture-slot numbers the PC keeps as MH1's on purpose (agent C note: mem_tex slots) |
| MH1, real | 225 | 10 or more instructions differ: the triage below |

  The "small" group still holds a few real changes hidden in constants (skill ids, kind and stage lists); section 2's
  scans cover those. Caveat: the coverage itself is from 15 Oct; code paths changed since (new G ports) may enter or
  skip some functions now. Re-running coverage needs a COV build and the runs (not done: CPU rules).

## 2. Two systematic findings (scans over the whole list)

### 2a. G renumbered the skills; MH1 code still asks for MH1's numbers

G's Skill_set_PL (ported, pl53.c) gives the hunter **G's** skill ids. Every MH1 function that calls `Pl_Skill_ck(pl, N)`
with MH1's N then tests a different G skill. A scan of all 945 functions (the `li a1,N` next to each `jal Pl_Skill_ck`,
MH1 vs G) found these still on MH1 numbers [asm, all from the scan; skill names not looked up]:

| Function | MH1 asks | G asks | What it does / what the player gets on the PC |
|---|---|---|---|
| skill_hp_calc_00134FF0 (quest start max health) | 34 / 35 / 36 = +10 / +20 / +30 | 33 / 34 / 35 = +10 / +20 / +30, and 57 / 58 / 59 = -10 / -20 / -30 | a G "health +10" skill gives nothing, +20 gives +10, +30 gives +20, G's unrelated skill 36 gives +30; the minus-health skills never apply |
| Em_Kehai_Hate_Add, em_hate_suu_set (how much a monster's attention a hunter draws) | 39, 40 | 38, 39 | the two attention skills are read one id off: a G stealth / lure skill acts as the other one, or as an unrelated skill |
| item_theft_005EC560 (em20 family: a monster stealing an item) | 43 | 42 | the anti-theft skill is read one id off |
| pl_dm_value_sub (damage reaction) | 45 | 44, plus 156 / 157 / 154 set PLW+0x90C to 60 / 45 / 180 | the knock-back skill is read one id off; G's three new timer skills (what +0x90C times is a guess: a stun / faint length) do nothing |
| enemy_mark_chk, disp_whole_map (monster on the map) | 41, 46 (and item 142 in the pouch) | none: G tests a flag on the monster (EMW+0x56A) and a map setting (lpPit+0x96) | the map shows monsters by MH1's rule with G's unrelated skills 41 / 46 and G's item 142 |
| pl_guard_ck (can this hit be guarded) | none | 169, 79 | see the ranked list, #3 |
| Pl_poison_add | 14 / 9 / 19 | same + 55 / 56 | G's two poison skills (one triples the poison, the other scales it again) do nothing |
| pl_init_sub (sharpness gauge) | none | 0x41 / 0x42 (+50 / -50, 150..400) | see #2 |
| pl_mv093 | none | 170 | a hunter action G speeds up (x1.2 at PLW+0x1A0 / +0x1F0) with skill 170 runs at normal speed |
| Shell10_set | none | 83 | a shot's start height changes with skill 83 in G, not on the PC |
| Get_atk_value | none | 97 | already G through Get_equip_value (RELEASE_G section 15): nothing to do |

Already G (seen by the scan, ported): Pl_damage_sub, St_pick_ck2, timer_calc_sub_pl, pl_mv071. The id shift looks
regular (MH1 34-45 -> G 33-44, 50 / 51 -> 48 / 49) but G also added new ids, so each place needs G's number from G's
asm, not a formula.

### 2b. G's subspecies (kinds 35-50) and G stages in constant lists

A scan for the numbers 35-50 that G added to compare lists of the remaining MH1 functions found: em_material_sub (#1),
enemy_trans (kind 39 drawn like 20), mlCalcTransEM (38 / 45 / 46 and others), ef_move_sub_005F7800 (38 / 39),
em_die_rev_0058DD70 (stages 34-39, not kinds), QuestClearCameraRequest (monster kinds 21 / 36 / 46 / 50, not stages: docs/mhg_questend_trade.md), set04_trans / set10_m
(G stages 89-91 / kind 50). The other hits were online code or false hits (layout offsets).

## 3. Ranked: the top 60

Rank = how often a player meets it x how visible the error is. "Reached" names where a player meets it offline.

### Hunt: the hunter

| # | Function(s) | Reached | What G does differently | What the player notices on the PC | Basis |
|---|---|---|---|---|---|
| 1 | em_material_sub = host `em_mat` in src/pc/rt/rt_em.c (and enemy_trans's kind test) | every fight with a G subspecies (quests with boss kinds 37, 44, 49; others in event quests) | G's kind jump table (sub_main lit_1837, 51 entries) sends each subspecies to its original's case: 35->13, 37 / 42->1 (Rathian), 38->6, 41 / 49->11 (Rathalos), 43->14, 44->26, 45->15, 46->21, 47->17, 48->22, 50->7; 36 has its own case. enemy_trans sends kind 39 to the kind-20 path | the host's switch only knows kinds up to 34, so on a subspecies every material is shown: broken and whole parts drawn on top of each other, the tail's cut caps visible on an uncut tail, no blinking | [asm: G jump table read; host switch read]. Small fix: map G kinds to the base case before the switch |
| 2 | pl_init_sub (f_pl.c) | every quest start; every return to camp | (a) the sharpness gauge length (PLW+0x884) goes through G sub_main 0x4F1310: +50 with skill 0x41 (max 400), -50 with 0x42 (min 150); (b) the "back at camp" position (PLW+0x738 == 2) comes from a table of 11 camps indexed by game_w+0x2E (game 0x6CFF80 positions, 0x6D0010 angles); MH1 has seven stage numbers hard-coded (4, 10, 20, 21, 50, 61, 67); (c) the weapon-class switch has 7 classes | (a) a G Sharpness+1 / -1 skill does not change the gauge; (b) G camps that MH1's list lacks (table rows 4, 8, 9 / 10 hold positions none of MH1's seven cases has) put a fainted hunter back wherever MH1's code leaves him | [asm] (b): which maps those camps are is [not checked]; test_revival_g passed on the camps it tried |
| 3 | pl_guard_ck (pl_damage_nm.c) | every guarded hit (sword and shield, lance, great sword) | a hit whose attack data has bits 0xC0 at PLW+0x3EB can only be guarded with skill 169; a second test with skill 79 and PLW+0x940 | every attack can be guarded as in MH1, including G's unguardable ones | [asm]; which monster attacks carry the bits [not checked] |
| 4 | skill_hp_calc_00134FF0 | every quest start | 2a: G's ids and the minus-health skills | wrong maximum health with any health skill | [asm] |
| 5 | Em_Kehai_Hate_Add, em_hate_suu_set, item_theft_005EC560 | every hunt (monster attention), Gypceros / thieves | 2a: ids one lower | stealth / attention / anti-theft skills act as the wrong skill | [asm] |
| 6 | pl_dm_value_sub | every hit taken | 2a: id 45->44, new skills 156 / 157 / 154 setting a timer | knock-back skill off by one id; three G skills inert | [asm]; meaning of PLW+0x90C [guess] |
| 7 | Pl_poison_add | poison hits (Cephalos family, Gypceros, Iodrome ...) | skills 55 / 56 scale the poison | G's poison skills do nothing | [asm] |
| 8 | enemy_mark_chk, disp_whole_map | the pause map, every hunt | monster mark by a flag on the monster and the map setting, not by skill 41 / 46 or item 142 | monster icons shown or hidden by MH1's rule; may show with an unrelated skill or item | [asm] |
| 9 | pl_dm015 | a damage reaction (one of the knock-down acts) | G takes a second argument (cases 1 / 2 / 3), a new motion (pl_chr_set2 226, frame test) | the reaction plays MH1's single version | [asm, partly read] |
| 10 | pl_mv093, Shell10_set | the action of pl_mv093; gunners | skill 170 speed; skill 83 shot height | inert skills | [asm] |
| 11 | pl_chr_sub | hunter animation set-up | seven new motion ids 405-423 | some G motion choices fall back to MH1's | [asm, ids only]; which moves [not checked] |
| 12 | fish_com_ck | fishing | G drops MH1's act 79 branch and a pl_to_normal | fishing may take a step G no longer has | [asm] |
| 13 | cam_plEX_zoom | the first-person zoom / scope view | G lets the stick (Psw+8) move the view, uses the chat check and vector steps (+113 instructions) | the zoom view stays fixed as in MH1 | [asm]; which view this is (binocular look or bowgun scope) [inferred] |

### Hunt: monsters

| # | Function(s) | Reached | What G does differently | What the player notices | Basis |
|---|---|---|---|---|---|
| 14 | ef_move_sub_005F7800 (em20: Yian Kut-Ku 6, Gypceros 20, G's 38 / 39) | every Kut-Ku / Gypceros fight | 21 KB script; new branches for kinds 38 / 39 (Shell08 shots at set angles, shell 34 type 2), and new blocks of Eft20 bursts, sounds and quakes between frames 20 and 60 of at least one animation | 38 / 39 fire MH1's attacks; some G effects / sounds of the originals missing (which animation: [not checked]) | [asm]. 38 / 39 are no boss of an offline quest (RELEASE_G section 16) |
| 15 | ef_move_sub_005AB750 (em09: Velocidrome / raptor family) | raptor fights | animation script: tagAct_191 call, new motion ids 1070-1075 | some G effects / sounds missing | [asm, ids only] |
| 16 | mlCalcTransEM | every monster pose (the host poses, see note) | joint-override lists gain 37 / 38 / 41 / 42 / 45 / 46 / 49 / 50 | the host's rt_em_neck has 37 / 41 / 42 / 49 / 50 only: kinds 38, 45, 46 do not turn their heads like their originals | [asm + code] |
| 17 | em08 em_act10 / 11 / 13, em_move02 (Cephadrome) | Cephadrome fights | the map finds no G twin. Most likely a pairing failure (G's em08 dispatch must have modes 2 and those acts); needs G's dispatch read | possibly MH1's behaviour in those acts | [not checked] |
| 18 | em17 kyusyu_char_set2, em14 em_atk31 | Gravios / Diablos | same: "missing" in the map; read G's callers | as above | [not checked] |
| 19 | em27_init, em03_init, em04_init, em09_init, em12_init, em16_init2, em19_init, em29_init | every small monster and raptor spawn | G replaces em_char_set / em_hp_vital_set by game 0x549FE0 / 0x5499A0 (+0x54A1D0 / 0x54A230 in em27) | hit points already go through G's tables (RELEASE_G 15); what 0x549FE0 adds is [not read] | [asm] |
| 20 | em_die_rev_0058DD70 (em03 family revival) | small monster deaths | G adds stages 34-39 to its stage list | the revival rule of those stages is MH1's | [asm] |
| 21 | em_move06_0058A310, em_act14_005A8F90, em_move03_005AA670 | em03 / em09 story scenes | event flag 21 ends a scene (em03); revival set / escape (em09) | a story monster may stay in its scene or leave differently | [asm] |
| 22 | em10_msg_set | Felyne / Melynx talk | a talk table with messages 4 / 5 / 30 | the monster's speech bubble is MH1's | [asm] |
| 23 | Stage_Hate_Add | area entry | G removed it | its table is zero-filled on the PC, so it adds nothing: same as G | [inferred] |
| 24 | sound_call_* of em15 / em08, em_dmg08, em14_act_set, em01_to_normal | fights | sound ids 1 / 30 / 66 / 98, small kind lists | sounds; em01_to_normal's extra kinds are a dead branch (RELEASE_G 15) | [asm] |

### Quest flow, HUD and pause menu

| # | Function(s) | Reached | What G does differently | What the player notices | Basis |
|---|---|---|---|---|---|
| 25 | game3 (src/main/game/f_game.c) | every quest end (clear / fail message) | no font text: G spawns a set object (game 0x659560, its move 0x6595A0 with four states, ~1.7 KB of G-only code to 0x659C40) with game_w+0x3E | MH1's centred font line instead of G's own quest-end display | [asm]; what G draws: compare with a PS2 shot |
| 26 | QuestClearCameraRequest | every quest clear | own camera choices for monster kinds 21 / 36 / 46 / 50 (not stages; docs/mhg_questend_trade.md), quest_w fields, a call to sub_main 0x4C8910 | MH1's clear camera on those stages (RELEASE_G 6 lists it as not done) | [asm] |
| 27 | disp_timer | every quest HUD | MH1 picks the special clock by a list of quest numbers (101, 103-107, 205-207); G by a quest flag (quest_w+0x40) | G quests with that flag get the normal clock | [asm]; which G quests set it [not checked] |
| 28 | Menu_mix_mv (pause menu combine) | every combine | a failed combination gives item 0x7A (122); MH1 0x8F (143). Also new steps (sub_main 0x4CC100, mix_item_chk, lpPit+0x91 / 0x92) | a failed combine puts G's item 143 (some other item) in the pouch | [code: PC sets lpPit->x78 = 0x8F; asm: G 122]. The new steps [partly read] |
| 29 | trans_box (HUD item box) | every hunt | tests quest_w+0x165 and game_w+0x180 before drawing | an item G hides or greys in some quests is drawn | [asm, meaning not read] |
| 30 | Pit_mv, Pit_mv_lb (pause menu driver) | every pause | G runs item_stock_mv / menu_move under a new condition, a PitMenu+0x20 store | possibly a pause-menu path behaves as MH1 | [asm] |
| 31 | player_info_sub, disp_slash_level, disp_item, disp_item_list_present | HUD | texture slots 280-282 -> 319-321 (the PC keeps MH1 slots: fine); disp_slash_level reads its colours through a per-level table (needle_data+0x50) | sharpness bar colours may differ | [asm] |

### Items and save data

| # | Function(s) | Reached | What G does differently | What the player notices | Basis |
|---|---|---|---|---|---|
| 32 | Ud_item_stack, Pl_item_stack, Ud_u_item_stack, Ud_item_num_ck3 | every item gained | G sets an "item seen" bit (sub_main 0x4C8EF0: User_data+0xDE8 bit field) | nothing on the PC (nothing reads it), but a PC save lacks the bits: matters if a PC card is used on a PS2 G | [asm] |
| 33 | Pl_item_charge | quest start (supplies) | a call to func_0012D170 (online test) before the supply table | offline: none [inferred] | [asm] |
| 34 | init_item_work / move_item, Item_preparation_list_num | items on the ground; combine list count | G removed them / no twin | none found (RELEASE_G 15) | [docs] |
| 35 | Equip_ok_ck, Get_equip_bit, Warehouse_*, Set_equip_idx, Get_equip_* | box, shops | armour row 20 -> 32 bytes, box 100 / 200 / 300 | already G through the G structs and UD_WARE_N (RELEASE_G 15) | [docs + code] |
| 36 | Get_hunter_status, Get_hunter_rank | status, saves | G returns early when the rank byte is 0 | none for a normal save (RELEASE_G 6) | [asm] |
| 37 | mc_sel_ck, Load_userdata, wyvern_kill_cnt_up | card screens, online counts | printf wrapper for text; online counter (no G twin) | none offline [inferred] | [asm] |

### Village: shops, forge, NPCs

| # | Function(s) | Reached | What G does differently | What the player notices | Basis |
|---|---|---|---|---|---|
| 38 | lb_process_set_weaponList, lb_process_set_armorList (forge: "upgrade from the box") | forge, upgrade from the box | the box list runs over G's 100 / 200 / 300 entries (sub_main 0x4C9250) with a G "owned" mark (Lb_put_set01) | the PC loops 0x40 entries with 10 fixed pages: weapons / armour stored past box slot 64 cannot be upgraded | [code: `while (n < 0x40)`, `lbShop.x6D = 0xA`; asm] |
| 39 | lb_armor_decide, shop_armor2_question, Lb_armor_shop, Lb_process_shop, shop_process_after (G-macro), armor_set_myArmor | armour shop: try on, buy, "equip now", leave | after trying on / buying / leaving, G runs Set_equip_data and Skill_set_PL on the hunter (skills and defence recomputed); "equip now" goes through a new shop_armor2_stack and Warehouse_equip | skills / defence shown in the shop or after it may be the old ones until something else recomputes them | [asm]; the effect [inferred]: not compared (referee g_vil_* covered the item and weapon shops, not armour) |
| 40 | lb_armor_put_itemDetail, lb_process_drawHelp, lb_armor_listItem | armour shop detail window | values through lobby 0x54F360 (G's armour row lookup) and 0x5572D0; texture slot 280 -> 319 | detail numbers may be read from MH1-layout rows | [asm, partly read] |
| 41 | lb_process_kyoukaListProg, lb_process_make_kyoukaList | forge upgrade list | G drops MH1's kind-7 (bowgun) branch | bowgun rows of the upgrade list as MH1 | [asm] |
| 42 | shop_armor_question | armour shop question | "missing" in the map, but lb_armor_decide's G code points at a G function under the same name (lobby, 0x54F780 region): a pairing failure | possibly MH1's question flow | [asm]; pair it by hand |
| 43 | Lb_shop, Lb_shop_move, lb_process_select, Lb_shop_trans2 | every shop | lbShop fields moved (LBSHOP_OFS), new init steps (Lb_put_npc_default, lb_talk_init), Lb_shop_trans_sub hook | mostly layout; the NPC's default pose / talk start at shop entry is MH1's | [asm] |
| 44 | Lb_PlayerStatus | village status card | weapon-class name (menu_stat_job_str), hint_tbl lines, sprintf formats | the card shows MH1's lines | [asm] |
| 45 | Lb_check_receipt | taking a reward / delivery at the counter | puts the hunter back to normal (Lb_pl_to_normal, action 46) | the hunter may stay in the receiving pose | [asm] |
| 46 | lb_pl_mv076, lb_pl_mv094, lb_pl_normal, Lb_pl_move, lb_pl_move_sub | village hunter actions | mv076: action 93 and lb_sys+0x95 instead of MH1's 77 path (Lb_eat_to_bell); mv094 has no twin | one village action (eating / bell, guessed from the name) plays MH1's | [asm] |
| 47 | lb_npc_trans, npcMvWALK, Lb_npc_mv, set_event_npc | village NPCs | G material kinds, event-flag test, Online_ck | NPC drawing is the host's; left as is by agent D (RELEASE_G 7) | [docs] |
| 48 | Lb_make_quest_tbl | elder's / guild quest lists | rewritten list builder (online table) | agent C's lane; offline lists come from Lb_make_quest_tbl_local (G) | [docs] |
| 49 | Lb_stage_load, Lb_Pl_stg_ck, Lb_send_* | village area change | camera set-up, quest level; the rest online | small: the camera at a village area change [inferred] | [asm] |

### Title, character creation, text

| # | Function(s) | Reached | What G does differently | What the player notices | Basis |
|---|---|---|---|---|---|
| 50 | Init_task (select00.c) | power-on | no twin in the map (G rewrote the boot) | the PC's boot is the host's; G's power-on demo quest is a known stand-in (RELEASE_G 3) | [docs] |
| 51 | opening_demo, middle_logo, demo_task_sub, violence_logo | title loop | movie handling through G helpers 0x1E42C0.., setBGcolor | logo / demo timing as MH1 [inferred] | [asm] |
| 52 | cont_trans, roll_move (character creation) | NEW GAME | G's control screen draws through 0x12DD90 / select 0x544840 | creation's control help as MH1 | [asm] |
| 53 | kbd_disp_input, sk_init_mode, kbd_Disp_KouhoGun, key_mask_check, DispSoftkeyboard, hk_key_* | name entry (once per hunter) | G's soft keyboard: palettes, USB keyboard keys, other key masks | name entry works as MH1's keyboard | [asm] |
| 54 | font_print_sp, font_sp_ck, strlen_sp, flfntDraw* (host rt_font.c) | all text | G's text codes: palette changes, button icons (func_0012B8F0: done elsewhere, RELEASE_G 8), strncat | colour codes in G-only text may print as MH1 does | [asm]; host code, compare with docs/english_g.md |
| 55 | Disp_NPC_message, Pit_disp_receive_mes (host rt_2d.c), DispFrameMessageA (G) | NPC lines, "received" messages | G formats through its printf wrapper (0x12BDD0) and a new frame (0x221E00) | frame / layout of the message window | [asm] |
| 56 | Disp_NowLoading2 | loading screens | G's loading message helpers (0x12D400 / 0x12D450) | loading text layout | [asm] |

### Effects, stage sets, host models

| # | Function(s) | Reached | What G does differently | What the player notices | Basis |
|---|---|---|---|---|---|
| 57 | eft06_m / eft06_i / eft06_set_com | flashes (hits, items) | a new Eft06 spawn at a joint, other fade values | a G flash missing | [asm, partly read] |
| 58 | eft01_m / eft01_t, eft21_t, eft22_t, eft26_t | shadows and small effects | small changes (10-22 instructions) | minor [inferred] | [asm diff size only] |
| 59 | armor_create_model, Pl_model_id_set, player_trans (host rt_pl.c) | every hunter draw | G's own model-id rules (armor 131 vs 241 instructions, func_00114F80) | host code; G-new armour models should be checked like the weapon models were (RELEASE_G 9) | [asm diff size]; [not checked] |
| 60 | set04_trans, set10_m | G stages 89-91, kind 50 | turning set pieces on G's own stages | gallery / online stages only | [asm] |

## 4. The rest, grouped (225 "real" minus the above, the 375 small ones)

- **Online only (74 + most of lobby/f, lobby/b net code)**: lbc_*, plaza_*, chat, member lists, ms_net_patch_set,
  Lb_send_*, Lb_check_newCommer, CallBack_*: nothing offline; port with G10 (online) if ever.
- **Host replacements (28)**: Game_task, movie_*, flfnt*, load_bin*, round_init, all_reset, trans, mkTexture,
  snd_joint_load, Menu_snd_load: the PC host owns screen, loading, sound and movies; compare behaviour, not code.
- **Wall hits (GetWallHitBit2 / Em / Pl)**: same logic as MH1 (agent C, RELEASE_G 6).
- **Monster small functions (99 small, ~20 real)**: em_fly*/em_dmg*/em_die*/em_demo* "shape" twins, em_cmd_* checks,
  local_area_move_init (G removed the per-kind helpers; em_area_move_init is G), shell*_i / shell*_m (one instruction
  each: the 0xC40 EMW stride), target_kind_set: layout only.
- **Village small (80)**: field offsets of lbShop / lb_sys / cw (macros exist), icon / sprite positions.
- **Stage sets (34 small)**: set09 / set05 / set17 / set20 inits: layout (G's SETW).
- **Menus / HUD small (19)**: map_move, player_on_map, bar_disp, gage_disp, camp_disp_sub: GSLOT / GV layout macros.
- **Keyboard / title / text (~25)**: as #51-56.
- Full name list of the 225 by system, with the number of differing instructions: section 6.

## 5. Suggested order for the G agents

1. **Skill ids (2a), one sweep** (hunter lane): skill_hp_calc, pl_guard_ck, Pl_poison_add, pl_dm_value_sub,
   Em_Kehai_Hate_Add, em_hate_suu_set, item_theft, pl_mv093, Shell10_set, pl_init_sub's sharpness skills. Each is a few
   lines under VERSION_MHG; G's numbers are in the table above. Check: a hidden quest start with a poked skill list and
   PLW+0x792 / +0x884 printed; the referee (tools/referee.py) with a skilled hunter.
2. **em_mat subspecies mapping** (#1, host, rt_em.c): a kind remap before the switch, and kind 39 to em20_mat. Check: a
   hidden shot of quest 10038 / 10042 / 10033 (kinds 37 / 44 / 49) after a part break and a tail cut.
3. **pl_init_sub camp table** (#2b): import G's 11-row table (game 0x6CFF80 / 0x6D0010), index by game_w+0x2E; find
   which maps rows 4, 8, 9 / 10 are; tools/test_revival_g.sh on those maps.
4. **Forge box list size** (#38): UD_WARE_N in lb_process_set_weaponList / set_armorList and the page count. Check:
   tools/test_gvillage.py with a box of more than 64 weapons.
5. **Quest end**: game3's set object (#25), QuestClearCameraRequest (#26), disp_timer's quest flag (#27). Check against
   PS2 reference shots (tools/g_ref_shot.sh / the referee's clear scenarios).
6. **Combine failure item and G's combine steps** (#28), the map marks (#8), trans_box (#29).
7. **Armour shop skill refresh** (#39, #40, #42): add a referee village scenario for the armour shop (g_vil_armor) like
   g_vil_wshop, then port what differs.
8. **Monster scripts**: em20's script (#14) and em09's (#15) per animation (the call-list diff method of RELEASE_G 16),
   mlCalcTransEM's lists into rt_em_neck (#16); pair em08 / em14 / em17's "missing" ones by hand (#17, #18) and read
   0x549FE0 (#19).
9. The rest by lane as time allows; online and host groups last.
Also: fix tools/cov_report_g.py (skip `.L` labels; count VERSION_MHG macros used in the body) and re-run coverage once
the CPU allows, so the next count starts from today's code.

## 6. The 225 remaining names by system (instructions that differ, registers / immediates masked)

Grouping is by name / file and rough (the "monsters" row holds the item functions whose names contain `em_`).
- **camera**: cam_plEX_zoom 140
- **effects**: eft06_m 79, eft01_m 22, eft06_i 21, eft01_t 14, eft22_t 14, eft26_t 12, eft06_set_com 11, eft13_se_req 10, eft21_t 10
- **host**: Game_task 772, font_print_sp 155, armor_create_model 129, font_sp_ck 129, SoftKeyboard_move 112, Disp_NPC_message 108, Pl_model_id_set 82, flfntDrawAll 69, flps0008 66, player_trans 62, movie_request 59, SoftKeyboard_set 57, flfntCreate 56, all_reset 45, flfntDraw 41, trans 41, movie_server 40, flfntCacheFlush 39, strlen_sp 38, load_bin 32, round_init 32, mkTexture 29, load_bin_req 28, movie_exit 19, Pit_disp_receive_mes 18, snd_joint_load 18, make_mat_srt 12, Menu_snd_load 11
- **hud/text**: Disp_NowLoading2 141, kb_input_ck_enter 14, font_stack_reset 12
- **hunter**: pl_init_sub 154, pl_dm015 68, pl_chr_sub 30, pl_dm_value_sub 28, Get_equip_data_ptr 26, skill_hp_calc_00134FF0 24, pl01_effect_move 22, pl_guard_ck 22, pl_light_change 21, Pl_poison_add 20, fish_com_ck 18, pl_mv028 14, pl_at050 12, pl_mv093 11, Get_equip_kaitori 10, Get_equip_name 10, Get_equip_price 10, Get_equip_rare 10
- **keyboard / title**: kbd_disp_input 101, sk_init_mode 101, kbd_Disp_KouhoGun 48, key_mask_check 30, demo_task_sub 27, DispSoftkeyboard 19, hk_key_f1 17, hk_key_f2 17, hk_key_u_cursor 17, hk_key_backspace 14, sk_board_ptr_replace 14, cmd_kakutei_all 13, sk_palette_cursor_set 13, sk_get_key_code 12, SoftKeyboard_init 10; Init_task 119, opening_demo 70, roll_move 23, cont_trans 17, middle_logo 17
- **menus**: trans_box 128, Pit_mv 55, disp_timer 37, player_info_sub 32, disp_whole_map 31, enemy_mark_chk 31, Pit_mv_lb 27, player_name_print 27, disp_item 23, disp_slash_level 23
- **monsters / items**: em_material_sub 392, ef_move_sub_005F7800 200, Item_preparation_list_num 166, ef_move_sub_005AB750 96, em_act10_0059B300 76, Stage_Hate_Add 65, init_item_work 62, em_act11_0059B440 60, em27_init 52, em_act13_0059B640 52, disp_item_list_present 51, Ud_u_item_stack 48, Ud_item_stack 42, mlCalcTransEM 41, kyusyu_char_set2_005E64A0 38, Em_Master_Change 37 (online master change), em_atk31_005B97B0 31, Pl_item_stack 31, em03_init 30, em09_init 30, em19_init 29, em04_init 26, em10_msg_set 25, em_move06_0058A310 23, em16_init2 22, Pl_item_charge 22, move_item 20, Share_item_conv 19 (online), sound_call_sub_005CBA30 18, em_act14_005A8F90 17, em_dmg08_005C9630 16, em_move03_005AA670 16, em01 / em08 / em14 / em15 / em17 / em27_local_area_move_init 15 each, em01_to_normal 12, em14_act_set 12, em_die_rev_0058DD70 12, sound_call_005A3D10 12, sound_call_parts_005CBB00 12, Ud_item_num_ck3 10
- **monsters (shells)**: Shell10_set 26, pl_shell_sel 20
- **other**: GetWallHitBitPl 117, GetWallHitBit2 77, GetWallHitBitEm 76, game3 49, nwSetEff_FreeDialog 29, option_init 19, Fade_busy_ck 18, game2 15, Fade_task 13, GameTrans 13, TransReset 12, armor_model_free 10
- **quest**: QuestClearCameraRequest 30
- **save / user data**: Equip_ok_ck 50, Get_equip_bit 34, wyvern_kill_cnt_up 33, Get_atk_value 31, Warehouse_equip_stack 27, mc_sel_ck 17, Set_equip_idx 14, Warehouse_search_space 14, Get_hunter_status 12, Load_userdata 11
- **stage sets**: set04_trans 133, set10_m 15
- **village**: Lb_make_quest_tbl 307, lb_put_sprite 237, shop_armor_question 176, Lb_send_pl_status 119, lb_npc_trans 115, Lb_stage_load 109, Lb_PlayerStatus 99, Lb_shop 85, Draw_menu_square 83, ef_move_sub_0053D830 83, lb_armor_put_itemDetail 77, lb_process_tag_decide01 77, shop_armor2_question 77, lb_process_set_weaponList 75, lb_armor_decide 64, lb_process_drawHelp 64, Lb_armor_shop 61, lb_process_kyoukaListProg 57, Lb_check_newCommer 56, lb_pl_mv076 51, lb_pl_mv094 48, Lb_process_shop 46, Lb_send_pl_pos 44, Lb_pl_status_m 41, lb_process_select 41, Lb_shop_move 41, Lb_guild_check_requireF 37, Lb_equip_set 36, ef_move_sub_0053E360 34, lb_pl_move_sub 33, Lb_Pl_stg_ck 33, lb_process_set_armorList 33, Lb_put_itemIcon 30, Lb_shop_trans2 28, ck_pl_send 27, CheckItemPrice_005AFEE0 24, Lb_check_receipt 24, npcMvWALK 24, Lb_cockpit_move 21, Lb_pl_move 21, Lb_put_icon_free 20, armor_set_myArmor 17, lb_pl_normal 17, MoveAndTransSet 16, lb_armor_listItem 15, lb_pl_turn_sub 15, lb_process_use_item 15, Lb_put_status 15, Lb_send_chair_status 15, Lb_get_comment 14, Lb_put_job_limit 14, Lb_put_msg_type2 14, Lb_check_target 13, Lb_put_icon 13, Get_LobbyName 12, Lb_get_quest_str 12, set_event_npc 12, SetHelpLineMsg 12, disp_itembox_cmd 11, Lb_npc_mv 11, lb_process_make_kyoukaList 11, lb_check_target 10, Lb_npc_move_sub 10, lb_pl_mv000 10, Lb_send_pl_warp 10

What is guessed, in one place: the meaning of skill ids (only the numbers are read), what PLW+0x90C times, which maps
the extra camp rows are, what game3's G set object draws, which animations em20's new blocks belong to, and whether
the "missing" em08 / em14 / em17 / shop_armor_question entries are map pairing failures. Nothing here was run.
