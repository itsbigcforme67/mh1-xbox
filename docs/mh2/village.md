# MH Dos (MH2): the village, from mode 6 to a quest (VILLAGE research, 4 Nov 2026)

- **Scope:** a map of MH2's offline village code and a plan to get it running on the PC. Nothing was built, run or changed; no C
  was written. Only this file was added.
- **Source:** our own disassembly of the owner's disc (asm/mh2/lobby, asm/mh2/sub_main, asm/mh2/main, asm/mh2/plsel), read by hand,
  with throwaway call-graph scripts in the scratchpad. Compared with G's asm (asm/mhg/lobby) and our G/MH1 C (src/lobby/f/lb_village_nm.c,
  src/lobby/f/lb_guild_g.c, src/pc/rt/rt_village.c, src/pc/viewer.c). The community mh2-decomp repo was not opened.
- **Proof level:** "read" = followed instruction by instruction; "skimmed" = only its calls, globals and constants were looked at;
  **guess** = a meaning given to something the code does not name. G counterparts are matched by call shape, callers and position
  in the same step table, not by bytes. Names in `code` without an address are ours (G/MH1 sources); `sub_X` / `lobby_X` is an
  unnamed MH2 function (the split calls them lobby_func_00X / sub_main_func_00X).
- All addresses are MH2 JP 1.04. "lobby" = lobby.bin at 0x6AF200 (text to 0x7DB880, data to 0x849F80, .bss to 0x8FAE00).

## 1. In plain words

After a quest, MH2 does not stay inside one "game task" the way G's Game_task does. Mode 6 ends the game task and asks the task
scheduler for the village; the scheduler loads lobby.bin and starts the **village task** (lobby 0x7C90C0). That task runs the
village's `Local_main` (lobby 0x7B97B0) every tick, and when a quest has been accepted and the hunter walks out, it starts the game
task again. So the PC's `village_step` needs an MH2 version of three layers: the village task, `Local_main`, and Local_main's step
functions. MH2 rebuilt the top two layers (5 steps where G has 7, the start-up merged into one big function, 80 talk modes where G
has 50, a world clock with seasons and day / night, 0x11C-byte member records). Many leaf functions (player walk, NPC movement,
mini data, set-prim, Pit menu) are the same shape as G's and keep G's names in our maps.

## 2. Entry chain

| # | where | what happens (read) | G |
|---|---|---|---|
| 1 | game task 0x3260B0, mode switch (jtbl 0x46CD70) | game_w.mode 6 calls 0x327770 | G: Game_task mode 6 calls Local_main itself |
| 2 | 0x327770 (mode 6) | system_w+0xE1 = 0; 0x36B600; stop streams (0x193CB0, 0x195230, 0x195B90); offline: quest_w+0x28 &= ~0x200000, all_reset, game_w.step = 1; then 0x327810 -> 0x33D6A0; returns 1 (the game task stops ticking) | new |
| 3 | 0x33D6A0 | system_w+0xBD (0x6AE04D, the "next task" code) = 1, Tsk_Exit(own task 0x5B3680) | new |
| 4 | scheduler 0x33D2C0 (every tick; 16 task slots of 0x20 at 0x5B3640) | code 1: Load_overlay(4) (main 0x18D070; overlay 4 = lobby.bin), Tsk_Execute(0x7C90C0, slot 5). Code 2: Load_overlay(4), Tsk_Execute(0x7CA500, 5). Code 3: Load_overlay(3) = game.bin, Tsk_Execute(game task 0x3260B0, 5). Code 4: Load_overlay(2) = select.bin, Tsk_Execute(0x6B5230, 3) | G has no such scheduler step |
| 5 | village task 0x7C90C0 | see section 3 | G Game_task offline steps (main 0x239490, 0x239F60) |
| 6 | Local_main 0x7B97B0 | see section 4 | Local_main 0x59E480 |

Who else asks for code 1 (the village): gm_sub 0x8FBD90 (the mode menu) and select 0x6B1EA0 (after the hunter is loaded / made;
skimmed). Code 2 comes only from gm_sub 0x8FBD90 and starts 0x7CA500, a two-step task (0x7CA550, 0x7CA700); **guess:** the network
entry or another menu item. Load_overlay's region table (main 0x1BB130) puts overlays 2, 3, 4 in one region (0x6AF200) and 5, 6 in
the next (0x8FAE00: gm_sub / lbguild / nethttp, docs/mh2/platform.md).

## 3. The village task (lobby 0x7C90C0, read)

Task work +8 = step, +9 = sub (lobby 0x7C8FB0 sets step and clears sub). First, every tick: 0x36CCE0.

| step | what it does |
|---|---|
| 0 | system_w+0xE5 = 1. Offline and Event_flag_ck(0) == 0 (never seen the opening): step 4. Else: **Clear_lobby_ram** (0x79EAA0), 0x36B600; if game_w.mode == 6 and game_w.step == 0 (online return) it waits for 0x7C9D80; then 0x194690, 0x7C9000 (= 0x36CCD0(10)), fade_set(2). Coming from a quest (mode 6): 0x36B130, game_init, game_w+0x41 = 1, step 2. Else (first entry): game_init, system_w+0x3F = 0, game_w+0x41 = 1, game_w+0x3D = 1, game_w+0x26E = 0, step 1 |
| 1 | online: 0x7CF5A0(0), all_reset. Offline: 0x7BC680 (network work reset, skimmed), 0x7CF5A0(0), all_reset. Step 2 |
| 2 | online: 0x7C9740. Offline: **0x7C9C50** (below). Result -2: 0x7C9530 (quit: all_reset, memset game_w 0x2BC and 0x5D46A0 0xE0, Tsk_Exit, back to the title through 0x33D760) |
| 3 | a quest was accepted: online 0x36CB00; system_w+0x35 = 0; offline game_w+0x3E (players) = 1 and game_w+0x224..0x227 = 1 / 0 per player; system_w+0x3F = 1; Tsk_Execute_mh2 (0x33D620) of the game task 0x3260B0 in slot 5. The game task then starts at mode 0 (game0 -> plsel task 0xD60280 -> game1 -> Quest_start 0x3A11A0) |
| 4 | first visit: fade_set(0), movie 0x3D4070(6) (**guess:** the opening movie), waits for it or START (Psw+4 & 0x8000), fade_set(1), stop; then step 0 path from Clear_lobby_ram on |

0x7C9C50 (offline step 2, read): every tick it adds one to game_w+0x20 and, every 30 ticks, one to User_data+0x203C (play time,
capped 35,999,999; that is lobby 0x7C9010, called from Local_main). If game_w+0x25 >= 2 and != 4 (**guess:** a menu screen is
open), it only counts lb_sys x8E down, runs Lb_shop_sw(0) into cw+0x3932, Pit_mv_lb and 0x36D3D0, and returns 0. Else it runs
Local_main: 1 -> task step 3, byte 0x59D662 = 0x14, system_w+0x3F = 1, system_w+0x40 (s16) = 0; -1 -> returns -2 (quit); 0 -> `trans`
(0x7C8FA0 jumps to it).

## 4. Local_main (lobby 0x7B97B0, read) and its five steps

Local_main: play-time tick (0x7C9010), world-clock tick (0x7BCB60 -> 0x7D8010, section 6), pNet+0x39 = 0, pNet+0x3D = 0,
font_set_stack_no(**6**) (G 0), lb_sys x8E counts down, cw+0x3932 = Lb_shop_sw(0), then the step function
`D_00819610[lb_sys x03]` (lb_sys+0xF29); if pNet+0x3D == 0, lbc_text_lobby_trans (0x7B2FE0). lb_sys x68 (lb_sys+0) == 0x21: game_w+0x41 = 0,
Pit_reset (sub_main 0x33DEA0), Lbc_set_prim(0,0,0,0), return 1. x68 == 0x25: Pit_reset, Lbc_set_prim, return -1. G uses 0x22 / 0x27
and MH1 0x20 / 0x25 for the same two exits.

Step table 0x819610 (the same five words also at 0x8195F0 and 0x819630):

| step | MH2 | G (vs_square_func_227 order: init_pre, init_init, init_se, init, move, exit, event) | what it does |
|---|---|---|---|
| 0 | 0x7BB880 | vs_square_init_pre + vs_square_init_init (+ init_se, init) | offline: fade out (sub 0-2), then sub 3 runs the **offline set-up 0x7BA260(0)** until it returns 0 (-> step 2, x8E = 6) or 3 (-> step 4, x68 = 0x22, lb_sys+8 = 1). Online: step 1 |
| 1 | 0x7BBA30 | vs_square_init | the area load after an exit or online: 0x36BE30, Lb_stage_load 0x7A67C0(PLW+0x684, 1), then Lb_npc_set 0x7A6C30 or 0x7A6BF0, Lb_check_newCommer, fade_set(10), 0x36BE50; stage 0x57 skips the sound step; init_se 0x7BB2C0; BGM 0x367C10; a countdown (lb_sys x05); then step 2, fade_set(15), cw+0x395E = 1, CameraMove(0), event check 0x7BAB20 (1 -> warp to the event position and step 4) |
| 2 | Lb_move_common 0x7A4810 | Lb_move_common 0x59B350 | the walking loop (below) |
| 3 | vs_square_exit 0x7BBD50 | vs_square_exit 0x59F140 | leave an area: fade out, Lb_Pl_basic_flagset, Lb_act_set, stop streams / SE, area clean-up 0x7A6DE0 (G Lb_reset), then **step 1** |
| 4 | 0x7BBF70 | vs_square_event 0x59E280 | an event: Lb_move_common plus a demo (0x367510, 0x31E130/0x31E160), fades, NPC event 0x77FF90 / 0x77F7B0 |

### The offline set-up 0x7BA260(mode) (read)

A staged function on cw+0x3934 (s16): returns 2 while busy, 0 when the village is ready, 3 when an event must play.

- **sub 0:** fade_set(10). Mode 0 / 1: 0x7B9B10 (all_reset, Q_camera_init, smell_init, Set_userdata, Lb_set_mini_data 0x775C00,
  world-clock start 0x7D82E0, skimmed), then for the master: Lb_set_mini_data_to_pl 0x775F30(master, lbCommer[m]+0x1C),
  Lb_set_player(master, lbCommer[m], lbCommer[m]+8), copy lbCommer[m] (0x11C bytes) to cw+0xA16+0x11C*m; game_w+0x3E = 1 (mode 0).
  Modes 5 / 6: the same for all ten member records.
- **sub 1:** 0x7B9F10 (load_pit, 0x327DA0, eft01_set, Lbc_connect, 0x794330; skimmed).
- **sub 2:** 0x7BA000 (Lb_Pl_basic_flagset, Lb_act_set, Lbc_set_prim, Quest_init, 0x33DA10). Then the area to load is written to
  **PLW+0x684** (u16) by mode (jtbl 0x846C00): mode 0 -> 0x57 if byte 0x5D8148 (User_data+0x36E8) is set, else **0x89**; modes 1 / 2 -> 0x83;
  mode 3 -> 0x83 with the hunter placed at (5762, -30, 10434) and lb_sys+0xF3C = 1; mode 4 -> 0x84; mode 5 -> 0x84 or the u16 at
  0x5CA910; mode 6 -> straight to sub 3. **Guess** for mode 0: 0x89 is the village after a quest; what User_data+0x36E8 means is not
  known. Then (jtbl 0x846BE0) 0x7BA170(mode) = Lb_stage_load 0x7A67C0 + Lb_npc_set 0x7A6C30, or 0x7A60E0 + CameraInit (mode 2).
- **sub 3:** init_se 0x7BB2C0 (sound pack loads, McCardOperation, streams) until it returns 0; CameraMove(0); BGM start 0x7A6590;
  fade_set(15); Lbc_set_prim(0); offline and system_w+0x44 != 1: event check 0x7BAB20 -> 1 means "warp to 0x8195A0[event] and return 3".

### Lb_move_common 0x7A4810 (skimmed: call order)

0x7A47A0, sub_main 0x367150 (new), the pad read 0x7A44D0 (G Get_sw2), 0x7BCB50, Lb_check_newCommer, 0x77F800, member-data compare
(memset / memcmp / memcpy), ItemCopy_Ud2Pl, Info_control, Lb_pl_move 0x797040, old_pos_save, 0x905930, enemy_mk, em_ride_sub,
Lb_npc_mv 0x77E020, Lb_npc_mk 0x77E1F0, player_mk, yure_move, the talk-target check 0x796880 (G Lb_check_target), lb_check_status
0x7A4FD0, CameraMove, light_move, move_eft, move_item, move_stage, Pit_mv_lb, 0x78EFF0 (G Lb_cockpit_move), 0x793C40
(G Lb_check_pl_load), add_prim(ot1), 0x7D8C30 (new). Same skeleton as G's; G's Lb_shop_sw moved up into Local_main.

### Areas (stages) and NPCs (read)

The lobby's area list (lobby 0x80ACD0, u16, 0 ends) and the NPC slot count per area (0x7FE270, same index; 0x79EB50 turns a stage
into the index):

| stage | 0x83 | 0x84 | 0x85 | 0x86 | 0x87 | 0x88 | 0x89 | 0xAA | 0xAB | 0xAC | 0x57 | 0xAD | 0xAE | 0xAF | 0x56 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| NPCs | 16 | 11 | 8 | 9 | 11 | 8 | 25 | 1 | 1 | 1 | 19 | 6 | 8 | 14 | 0 |

Which area is the village square, the house, the farm or the guild is **not known** (0x89 with 25 NPCs is the obvious guess for the
square). Lb_npc_set 0x7A6C30 places each slot with 0x77C490(i, mode) deciding, pull_enemy_work, EMW+0x1E = 1, +0x30F = 0, +0xC = i,
+0x680 = game_w+0x14, then 0x77DA10 (the NPC's data from 0x7FE230). Offline, on 0xAD / 0xAE / 0xAF / 0x56 (0x7A8430), a second list
from 0x7FE230[index] (via 0x6D4E90) is added. 0x7A6760 loads the area's model file from 0x7FE280[index] (load_file_mdl).

## 5. Talking, the quest givers and leaving for a quest

### Talk modes (lb_sys x68)

lb_check_status 0x7A4FD0 switches on x68 < 0x50 (jtbl 0x8466D0); G's (0x59B6F0) on x68 < 0x32 (jtbl lit_561_00665C70). By the
handler each case calls (read from both tables):

| MH2 x68 | handler | G x68 | G handler |
|---|---|---|---|
| 0 (also 0xF, 0x15, 0x22, 0x25, 0x26, 0x37, 0x4D) | idle: soft keyboard, START -> menu 0x7709A0, SELECT online chat toggle | 0 (also 0x14-0x16, 0x23, 0x27, 0x28, 0x2E) | idle |
| 1 | 0x76DD30 (online rooms, Lbc_ReadRoomInfo) | 1 | Lb_join |
| 2 | 0x7A1120 (**guess:** plain NPC conversation; most talk ids open it, see below) | 6 / 7 | Lb_talk |
| 3 | 0x775590 + Lbc_set_prim | 5 | 0x58DE70 + Lbc_set_prim |
| 4, 5 | 0x7A8ED0 (talk text, then the dispatcher 0x7A8700) | 6, 7 | Lb_talk |
| 6, 0x1B, 0x42 | Lbs_MatchEntry | 8, 0x1C | Lbs_MatchEntry |
| 7, 0x21 | soft keyboard exit, Lb_menu_exit | 9, 0x22 | the same |
| 0xB | Lb_menu_move, Lbc_init_network_work | 0x10 | the same |
| 0xD | Lb_select | 0x11 | Lb_select |
| 0x17 | Lbs_RoomExit | 0x18 | Lbs_RoomExit |
| 0x18 | NPC zoom-in cancel (0x31D980 / 0x31E130), sound | 0x19 | NPCZoomInCameraCancel |
| 0x1D | Lb_ItemBox_mv (0x762500) | 0x1E | Lb_ItemBox_mv |
| 0x1F -> 0x20 -> **0x21** | str_fadeout; then fade wait, Lbc_set_prim, Pit_reset; Local_main returns 1 (to the quest) | 0x20 -> 0x21 -> 0x22 | the same |
| 0x23 -> 0x24 -> **0x25** | the same, Local_main returns -1 (quit) | 0x25 -> 0x26 -> 0x27 | the same |
| 8, 9, 0xA, 0xE, 0x10-0x14, 0x2B-0x36, 0x38-0x41, 0x43-0x4C, 0x4E, 0x4F | 37 service handlers in lobby 0x6AF280-0x76DD30, none named yet | 4, 0xA-0xF, 0x13, 0x29, 0x2D, 0x31 | Lb_shop, Lb_mix, Lb_armor_shop, Lb_process_shop, Lb_eat, Lb_gh_board, Lb_event_GH, ... |

So G's x68 numbers do not carry over: G-form C that writes x68 (lb_village_nm.c uses MH1 numbers, the G copies their MH1 equivalents)
needs MH2's numbers. None of G's shop handlers is called by name from MH2's switch: MH2's shops, forge, kitchen and so on are among
the 37 unnamed handlers.

### How a talk starts (read)

The talk-target check (0x796880, G Lb_check_target) picks an NPC; lb_basic_master (0x79C540) calls the talk start **0x7AE940**, which
stores the NPC record's first byte as the **talk id** (u16 at 0x8BDBF0), a sub id (u16 0x8BDBF2, set to 0x63 at night by 0x7BCBC0)
and two words (0x8BDBD0, 0x8BDBEC), and sets x68 to 4 / 5. After the text, 0x7A8ED0 calls the dispatcher **0x7A8700**, a compare chain
on the talk id that calls each service's "open" function; each open function sets x68 to its handler's mode (and lb_sys+8):

| talk id | open | x68 | handler | reading |
|---|---|---|---|---|
| 2, 0x38, 0x4F, 0x6C | 0x7196F0 | 0x35 | 0x719AF0 | **a quest giver** (calls the quest accept 0x71A240) |
| 3, 0x37 | 0x6CB490 | 0x3F | 0x6CB690 | **a quest giver** (via 0x6CBA50 / 0x6CBEF0 -> 0x6CCC10 -> 0x71A240) |
| 0x41 | 0x71F710 | 0x34 | 0x71F8A0 | **a quest giver** (via 0x71FD50 -> 0x71A240) |
| 0x4E | 0x6C91C0 | 0x45 | 0x6C9640 | **a quest giver** (calls 0x71A240; also PitWork_init, ran_suu) |
| (none in the chain) | 0x6C1FC0 | 0x43 | 0x6C20B0 | **a quest giver** (via 0x6C2330 -> 0x71A240); opened from 0x6B5568, 0x7581A8, 0x7A1280, 0x6C4F08 |
| 7, 0xA, 0xB, 0xC, 0xD, 0x28, 0x2D, 0x2E, 0x39, 0x42, 0x50, 0x5D, 0x6A | 0x7A0FB0 | 2 | 0x7A1120 | plain talk (**guess**) |
| 8, 0x2C | 0x776580 | 0xA | 0x7766D0 | unknown service |
| 0xE, 0x56, 0x5E | 0x757F90 | 8 | 0x7580B0 | unknown service |
| 0xF, 0x55, 0x58, 0x68 | 0x6C4CB0 | 9 | 0x6C4E40 | unknown service |
| 0x1C | 0x6D5260 | 0x4B | 0x6D5330 | unknown |
| 0x1E | 0x6FE680 | 0x44 | 0x6FE810 | unknown |
| 0x49 | 0x6E4E20 | 0x38 | 0x6E4FA0 | unknown |
| 0x6F | 0x6F2630 | 0x3C | 0x6F2890 | unknown |
| 0x71 | 0x6C8260 | 0x47 | 0x6C8350 | unknown |
| 5 / 0x13 / 0x10 / 0x12 / 0x40 | 0x6B5340+0x6B53A0 / 0x6B5860 / event flags / Lb_put_set01 | | | small cases |

Which NPC is the village chief, the guild clerk or a shop is **not known yet**: the talk ids come from the NPC records (0x7FE230
lists); a hidden run that prints EMW slot, position and talk id per NPC (rt_village.c's RT_VILLAGE_TRACE does this for G) settles it.

### The quest accept 0x71A240(quest, fee, flag) (read)

The MH2 form of MH1's lb_u.c offline accept (rt_village.c `village_accept` mirrors MH1's):
quest record = 0x75F3B0(quest) (below); 0x18EB10 (clears byte 0x5D83B5); **quest_price** (0x5BC4A0) = fee; online: 0x5CA910 = stage;
0x8BEB6F = 0; 0x8B2BE2 = quest; cw+0x396A = 1 ("a quest is held", tested by lb_check_status); Lb_menu_quest_info(record);
**main 0x18DED0(quest): u16 0x6ADB48 = quest** (the number Quest_start reads); game_w+0x2E = quest; sound 10; the quest flag over the
hunter: PLW+0x86E = Lb_get_quest_type(record) (0x75FDF0) | 0x40 (| 0x80 and quest_price += record+8, cw+0x3948 = 1, game_w+0x270 = 1
when `flag`); Gold_add(-quest_price) (0x3B6500; money is User_data+0x60); Lb_set_mini_data on cw+0xA32+0x11C*master and
lbCommer[master]+0x1C; 0x6ADAD0 = record word 0; u16 0x6ADB4A = record+0x20. Offline: 0x5BFB70 = 0, and the season, day / night
and day-in-season are copied to 0x5BFB88, 0x5BFB80, 0x5BFB78 (0x7D8460, 0x7BCBC0, 0x7D8470), so **the quest remembers the season
and time it was taken in** (reading of the copies; their readers were not followed).

Quest records: 0x75F3B0 -> numbers below 50000: 0x75F300 finds the group in the u16 list main 0x1BB3D0, takes the group's table from
the pointer list **0x924240** and entry (quest % 100), relocated by sub_main 0x36B110. 0x924240 lies inside **lbguild.bin** (slot B,
0x8FAE00-0x924280): lbguild is the quest table overlay (**guess** from the address; its loader was not found). Numbers 50000 and up:
0x75F3F0 scans three 0x300-byte slots at 0x5BFBA0 (number at +0x2A of the record each slot points to; **guess:** downloaded / event
quests). G's counter (src/lobby/f/lb_guild_g.c: lb_quest_all, 8 quests per star level, Lb_make_quest_tbl_local) has no MH2 twin found.

### Leaving (read, except where marked)

lb_basic_master (0x79C540, G 0x597630) sets x68 to 0x1F at the village exit when a quest is held (it is the only writer of 0x1F; the
gate test itself was skimmed). 0x1F (stream fade) -> 0x20 (fade wait, Lbc_set_prim, Pit_reset) -> 0x21 -> Local_main returns 1 ->
0x7C9C50 moves the task to step 3 -> the game task. lb_basic_master also writes x68 = 3, 4, 0xC, 0x19, 0x1B, 0x1C, 0x1E, 0x26, 0x27,
0x29, 0x2A, 0x2D, 0x36, 0x39 (area exits, beds, chairs ... **not sorted out**), and it memsets the 0x18-byte quest order block 0x6ADB30
(main 0x18DEB0 copies one in; Quest_start copies it to quest_w+0x1F4, the result screen's three bonus entries) when 0x5BFB70 is set.

## 6. Seasons and day / night (read; meanings are **guesses** where marked)

A world clock at lobby 0x8FAC00 (at least 0x78 bytes), ticked from Local_main (0x7BCB60 -> 0x7D8010) by the difference of a counter at
0x6AE398 (**guess:** a system tick) and set up by 0x7D82E0 / 0x7D7990 at village entry:

| field | meaning (read) |
|---|---|
| 0x8BF0C8 | day length in seconds, 86,400 by default (0x7D7990 sets it when 0) |
| clock+0x0C, +0x10, +0x14 | day length as float; x another float (0x6AE050); x 30.0 |
| clock+0x18 | second of the day (start value mod 86,400); clock+0x54 = that / 86,400 (the day fraction) |
| clock+0x5C | days in the current season; when it reaches the table 0x8FAC1C[season] it wraps and the season moves on |
| clock+0x60 | **season**, 0..2 (wraps at 3); offline saved to User_data+0x2728 (0x5D7188), day-in-season to +0x2729, +0x40 to +0x2730 |
| clock+0x64, +0x48 | counters raised at each season wrap (**guess:** years) |
| 0x8BF0D8 | time of day in seconds; 0x7BCBA0 / 0x7BCBC0 = before / after 43,200 (noon): **day / night** |
| 0x8BF0A0..0x8BF0B8 | the online copies of the same values |

0x7D7CB0 is the "next day" step (season wrap and save); 0x7D9EA0 (called after each clock step) and 0x6F6240 (on a season change) are
**guesses** for the lighting / NPC refresh. Three seasons is what the code counts; that they are MH2's warm / cold / breeding seasons is
the obvious reading, not checked. Day / night changes the talk sub id (0x63 at night) and is stored with an accepted quest.

## 7. Main functions

| MH2 | G counterpart | what it does |
|---|---|---|
| sub_main 0x327770 | (G Game_task mode 6) | mode 6: stop sounds, all_reset, ask for the village task |
| sub_main 0x33D6A0 / 0x33D6C0 | new | task exit with next-task code 1 / 2 |
| sub_main 0x33D2C0 | new | task scheduler; next-task codes 1-4 (section 2) |
| main 0x18D070 | Load_overlay | MH2 form (region table 0x1BB130, address table 0x1C8580, PatchExecCS) |
| lobby 0x7C90C0 | G Game_task offline steps | the village task (section 3) |
| lobby 0x7C9C50 | G main 0x239F60 | offline step 2: menus or Local_main |
| lobby 0x7C9530 | G Game_task case -1 | quit to the title |
| lobby 0x7C9010 | (G Lb_move_common part) | play time, game_w+0x20 |
| lobby 0x79EAA0 | Clear_lobby_ram / G 0x59A270 | memset lb_sys 0xF68 and 0x8BDC08 (6), cw = 0x8EA910, pNet = 0x8BFAC0, Lbc_init_network_work, Lb_set_mini_data(0x5CA920) |
| lobby 0x7B97B0 Local_main | Local_main 0x59E480 | one village tick (section 4) |
| lobby 0x7BB880 | vs_square_init_pre | step 0 |
| lobby 0x7BA260 | vs_square_init_init (+ init_se, init) | the offline set-up, staged on cw+0x3934 |
| lobby 0x7B9B10 | part of init_pre / init_init | all_reset, camera, smell, Set_userdata, clock start, mini data |
| lobby 0x7B9F10 / 0x7BA000 / 0x7BA170 | parts of vs_square_init_init | loads (pit, effects) / Quest_init, act set / stage + NPCs |
| lobby 0x7BBA30 | vs_square_init | step 1, the area load |
| lobby 0x7BB2C0 | vs_square_init_se | sound packs, streams, card |
| lobby 0x7A4810 Lb_move_common | Lb_move_common | step 2 |
| lobby 0x7BBD50 vs_square_exit | vs_square_exit | step 3 |
| lobby 0x7BBF70 | vs_square_event | step 4 |
| lobby 0x7A67C0 | Lb_stage_load (0x42C vs G 0x7FC) | area load: model file, motions (com_motion_load), stage, fog, set objects, light, Lb_pl_init, camera, player_mk |
| lobby 0x7A6C30 / 0x7A6BF0 | Lb_npc_set | NPC placement |
| lobby 0x7A6DE0 | Lb_reset | area clean-up |
| lobby 0x7A6590, sub_main 0x367C10 | lobby_bgm_set | area BGM (str_play_f_vol) |
| lobby 0x7BAB20 | (inside G vs_square_init / event) | story event check (Event_flag_ck / set, 0x3BD970) |
| lobby 0x79C540 | lb_basic_master (0x1178 vs 0x179C) | the village hunter; exits, talk start |
| lobby 0x797040 Lb_pl_move | Lb_pl_move | hunter movement |
| lobby 0x77E1F0 / 0x77E020 | Lb_npc_mk / Lb_npc_mv | NPCs (0x314 vs 0x22C: edited) |
| lobby 0x796880 | Lb_check_target (0x518 vs 0x368) | talk target |
| lobby 0x7A4FD0 | lb_check_status (0xF60 vs 0xAF4) | talk-mode switch, 80 modes |
| lobby 0x7AE940 | (G lb_set_talk part, **guess**) | talk start, talk id |
| lobby 0x7A8ED0 / 0x7A8700 | Lb_talk / (new) | talk text / service dispatcher |
| lobby 0x7196F0 + 0x719AF0, 0x6CB490 + 0x6CB690, 0x71F710 + 0x71F8A0, 0x6C91C0 + 0x6C9640, 0x6C1FC0 + 0x6C20B0 | Lb_guild (**guess**: five quest givers) | quest menus |
| lobby 0x71A240 | lb_u.c offline accept | accept a quest |
| lobby 0x75F3B0 / 0x75F300 / 0x75F3F0 | (G 0x559B40 quest row) | quest record by number |
| lobby 0x75FDF0 | Lb_get_quest_type | quest type bits -> flag |
| lobby 0x792C40 | Lb_menu_quest_info | quest info for the menus |
| main 0x18DED0 / 0x18DEA0 / 0x18DEB0 | new | set quest number 0x6ADB48 / byte 0x6ADB4C / the 0x18-byte block 0x6ADB30 |
| lobby 0x775C00 / 0x775F30 | Lb_set_mini_data / _to_pl (0x328 vs 0x104) | member mini data (0x100 bytes in MH2) |
| lobby 0x794480 Lb_set_player | Lb_set_player | |
| lobby 0x794170 | Lb_check_newCommer | |
| lobby 0x7BCA00 / 0x7B2FE0 | Lbc_set_prim / lbc_text_lobby_trans | |
| sub_main 0x33DEA0 / 0x33EED0 | Pit_reset / Pit_mv_lb | |
| lobby 0x7A4410 | Lb_shop_sw | |
| lobby 0x7D8010 / 0x7D82E0 / 0x7D7CB0 / 0x7D7990 | new | world clock tick / start / next day / set day |
| lobby 0x7BCBA0 / 0x7BCBC0, 0x7D8460 / 0x7D8470 | new | day / night tests; season / day-in-season getters |

## 8. Work areas

| what | address | size | fields (MH2 offsets) |
|---|---|---|---|
| lb_sys | 0x8BDC10 | 0xF68 (Clear_lobby_ram) | +0 x68 talk mode; +4 event number; +8; +0xC; +0xF29 x03 step; +0xF2A x04 sub; +0xF2B x05 wait; +0xF4E x8E countdown; +0xF3C flag (mode 3); +0xF5F / +0xF61 / +0xF71 bytes used by the talk code. LBM() in include/mh2_lay_ofs.h already maps MH1 offsets |
| talk block (**guess** name) | 0x8BDBD0..0x8BDC0F | | 0x8BDBF0 u16 talk id, 0x8BDBF2 u16 sub id, 0x8BDBD0 / 0x8BDBEC words. build/mh2/pc/addr.txt puts lb_pit at 0x8BDBE8: these u16 at its +8 / +0xA do not fit MH1's lb_pit bytes, so lb_pit's MH2 place or layout is unchecked |
| cw (client work) | pointer 0x8EA908 -> 0x8EA910 | at most 0x102F0 (the clock follows) | +0 byte (mode 5 test); +0x3932 Lb_shop_sw; +0x3934 set-up sub; +0x3948 flagged quest; +0x395E; +0x396A quest held; +0x396F; +0xA16 + 0x11C n member records (mini data at +0x1C = +0xA32). G: records at +0x3A8, 0x5C each; MH1 +0x1346, 0x2FC |
| pNet | pointer 0x8BFAB8 -> network_work 0x8BFAC0 | | +0x39 (MH1 +0xC, G +0x2D), +0x3D (MH1 +0x11, G +0x31) |
| lbCommer | 0x8C0140 | 10 x 0x11C | +0 in use, +8 name / id, +0x1C mini data (0x100). addr.txt also maps MH1's lb_player to 0x8C0140: one of the two is wrong |
| world clock | 0x8FAC00 | >= 0x78 | section 6 |
| quest order | 0x6ADB30 | 0x18 + 6 | 0x6ADB48 u16 quest number, 0x6ADB4A u16, 0x6ADB4C s8 |
| quest_price | 0x5BC4A0 | 4 | the fee |
| game_w | 0x6ADCB0 | 0x2BC | +0 mode, +1 step, +0xE, +0x14 stage, +0x20 tick count, +0x25 (menu state, **guess**), +0x2E quest number, +0x3C master, +0x3D, +0x3E players, +0x41 "in the lobby" (GWX(0x1DC)), +0x224..+0x227 per player, +0x26E, +0x270 |
| system_w | 0x6ADF90 | | +0x1B, +0x35, +0x3F (leaving for a quest), +0x40 s16, +0x44, +0xBC, +0xBD next-task code, +0xE1, +0xE5 |
| player_work | 0x5F89D0, 0x970 each | | +0xAC position, +0x3B4 talk target, +0x684 u16 area to load, +0x754 (copied with the position on a warp), +0x844, +0x86E quest flag (MH1 +0x916) |
| em_work (NPCs) | EMW 0x9D0 each | | +0xC index, +0x1E, +0x30F, +0x680 stage |
| User_data | 0x5D4A60 | | +0x60 money, +0x203C play time, +0x2728 season, +0x2729 day in season, +0x2730, +0x36E8 (picks 0x57 / 0x89) |
| village tables | lobby 0x80ACD0, 0x7FE270, 0x7FE230, 0x7FE280, 0x8195A0, 0x819610 | | areas, NPC counts, NPC lists, model files, event positions (12 bytes each), step table |
| quest tables | main 0x1BB3D0, lbguild 0x924240, 0x5BFBA0 | | quest groups, per-group tables, quests >= 50000 |

## 9. Absolute addresses in the G-form lobby C (what needs MH2V)

tools/pc_abs.py (VERSION=mh2) moves a literal `(T *)0x...` through build/mh2/pc/addr.txt: an exact entry, or the nearest earlier entry
if it is at most 0x40 away; otherwise the literal stays a raw PS2 address and crashes on the PC (0x3F34C1 in Lb_set_mini_data). The
nearest-entry rule moves by the MH1 offset, which is wrong for every struct MH2 re-laid out (game_w past 0x16, User_data, PitMenu,
quest_w). A scan of the 221 lobby C files the PC village build compiles (tools/build_pc.sh LOBBY, LOBBY2, LOBBY3; the online-only files
left out): **261 sites, 56 addresses, none in MH2V() yet.** 25 have no mapping (crash), 27 get a nearest-entry mapping, 4 are exact.

| MH1 address (field) | sites | files | pc_abs today | MH2 |
|---|---|---|---|---|
| 0x3F34C1 (game_w+0xD1 master) | 56 | lb_village_nm, lb_talk, lbshop2_nm, lb_mix_nm, lbarm01 | none | 0x6ADCEC (+0x3C; MH2V already used in em14_r06.c) |
| 0x3F34C3 (+0xD3 players) | 2 | lb_village_nm | none | 0x6ADCEE |
| 0x3F35CC (+0x1DC in lobby) | 2 | lb_village_nm | none | 0x6ADCF1 (+0x41; MH2's Local_main clears it) |
| 0x3F3603..0x3F3606 (+0x213..+0x216) | 12 | lb_village_nm, lb_bz135, lbui_nm | none | unknown |
| 0x3F360A (+0x21A) | 1 | lb_village_nm | none | **guess** 0x6ADF20 (+0x270) |
| 0x3F33DC (select_w+0xAC, accepted quest) | 3 | lb_b, lb_village_nm | none | 0x6ADB48 (u16) |
| 0x3F3404 (+0x14 stage) | 37 | lb_village_nm, lbshop2_nm | near 0x6ADCC4 | right |
| 0x3F33FE (+0xE) | 6 | lb_village_nm | near 0x6ADCBE | right |
| 0x3F340E (+0x1E) | 1 | lb_l | near 0x6ADCCE | wrong: +0x20 (0x6ADCD0) |
| 0x3F341C (+0x2C quest number) | 4 | lb_village_nm | near 0x6ADCDC | wrong: 0x6ADCDE |
| 0x3F3415 (+0x25) | 1 | lb_n | near 0x6ADCD5 | unchecked (MH2's own +0x25 is a menu state) |
| 0x3F36AB / BF / C5 / CF (system_w+0x1B / 0x2F / 0x35 / 0x3F) | 27 | lb_village_nm, lb_j, lb_l | near, same offsets | +0x1B, +0x35, +0x3F likely right (MH2's village writes them); +0x2F unchecked |
| 0x3F3710 / 0x3F3714 (Psw, Psw+4) | 9 | lbui_nm, lb_village_nm | exact / near | right |
| 0x39DAD0..0x39DAE1 (PitMenu+0x10..+0x21) | 46 | lb_ib, lb_tu_ib, lb_village_nm, lb_z101/102, lb_aa, lb_by86/123 | near, same offsets | wrong: use PMO() (e.g. +0x12 -> +0x2E) |
| 0x3C6FC1 / C3 (User_data+1 / +3) | 3 | lb_ay, lb_tu_ib, lb_village_nm | near | right |
| 0x3C6FE0 (User_data+0x20 money) | 9 | lb_ad, lb_b, lb_f, lb_mix_nm, lbshop2_nm | near +0x20 | wrong: money is +0x60 (Gold_add 0x3B6500). **Note:** UDMO(0x20) gives 0x2984, not 0x60: check UDMO before using it here |
| 0x3C733B, 0x3C738D, 0x3C7392..97, 0x3C73A8, 0x3C741C/D (User_data+0x37B..+0x45D) | 19 | lb_ad, lb_n, lb_village_nm, lb_z04, lb_by59/78/80/84, lbshop2_nm | none | UDMO() |
| 0x3C7450 / 54 / 0x3C74D4 (quest_w+0x10 / 0x14 / 0x94) | 3 | lb_c | near / none | QWO() |
| 0x3E5506 (player_work+0x916 quest flag) | 1 | lb_village_nm | none | PLW+0x86E (read in 0x71A240) |
| 0x3D8231 (stage_work+1) | 1 | lb_village_nm | near | right |
| 0x5BC505 / 0x5BC506 | 3 | lb_z102 | none | unknown |
| tables: Snd_steft_tbl, Snd_stcom_tbl, item_str (exact), pit_help_str_tbl+4, NET_CON_TEX (near), Skill_name (none) | 9 | | | check each by its readers |

The full list with counts was made by a scratch script (stdlib, reads tools/build_pc.sh's lists and build/mh2/pc/addr.txt).
Other G-only assumptions in the same C, which MH2V alone does not fix:
- `vs_square_func_213` / `_227`: G's 7-step table; MH2 has 5 different steps (section 4).
- lb_sys x68 numbers (section 5) and Local_main's exit codes (MH2 0x21 / 0x25).
- `CW_OFS` (include/lbsys_g.h) is G's client-work map; MH2's cw layout is different (section 8) and has no map yet.
- `PNET_OFS` is the identity outside MHG_ONLINE; MH2 needs +0xC -> +0x39 and +0x11 -> +0x3D.
- src/lobby/f/lb_guild_g.c reaches G's lobby memory with `GL(addr)` = lb_sys + (addr - 0x6548F0) (92 uses, 59 G addresses): on MH2
  that lands in unrelated lobby .bss. It is G's counter and has no place in MH2's village.
- rt_village.c: `Clear_lobby_ram` (MH1's), select_w+0xD0 for the quest (G; MH2 0x6ADB48), User_data+0x20 money, UDO() pouch offsets,
  lb_quest_info / key_quest (no MH2 address).

## 10. Plan: MH2's village on the PC

The pattern that worked for MH2's quest (docs/mh2/quest_flow.md, host.md): MH2's own top-level functions as host C written from its
asm (src/pc/rt/rt_mh2_*.c), shared leaf C where MH2's function has the same shape as G's, MH2V / LBM / PMO / UDMO for raw offsets.
Keep G's village path (rt_village.c with the G-form Local_main) untouched for G; add an MH2 path beside it.

### Part 1: the village on screen, walking

1. **Entry.** In viewer.c `village_step`, VERSION_MH2: replace the dark stand-in with `rt_mh2_village_enter()` / `rt_mh2_village_tick()`
   (new file, e.g. src/pc/rt/rt_mh2_village.c). Enter = what 0x327770 and the village task's step 0 / 2 do: rt_world_reset (as G),
   rt_lb_reload, MH2's Clear_lobby_ram 0x79EAA0 (lb_sys 0xF68 cleared; cw / pNet pointers to 0x8EA910 / 0x8BFAC0 in the host's lobby
   image), game_w+0x41 = 1, lb_sys x03 = x04 = 0. Skip task step 4 (the opening movie): treat Event_flag 0 as set, log a stand-in.
2. **Local_main 0x7B97B0** as host C with MH2's own 5-entry step table. Play time (0x7C9010) and Lb_shop_sw can be in from the start.
   The world clock: a stand-in that sets 0x8BF0D8 to a fixed daytime value and clock+0x60 to the saved season (User_data+0x2728),
   logged; the real tick (0x7D8010, its time base 0x6AE398) later.
3. **Step 0 offline: 0x7BB880 + 0x7BA260(mode 0)** as host C. Sub-steps 0-3 in order; for the first light: 0x7B9B10's Set_userdata /
   mini data (0x775C00 is 0x328 bytes: new C or a stand-in that only copies the name), Lb_set_player, the stage choice into PLW+0x684,
   then Lb_stage_load 0x7A67C0 and Lb_npc_set 0x7A6C30. The host already draws a stage from game_w+0x14 (viewer load_stage_models), so
   the first useful check is "stage 0x89 (or 0x57) drawn with the hunter on it". init_se 0x7BB2C0 (sound packs, card) and the BGM
   0x7A6590 can be stand-ins that return "done" at first; the event check 0x7BAB20 returns 0 (no event) at first.
   - Lb_stage_load 0x7A67C0 is MH2's own (0x42C bytes vs G 0x7FC; it calls com_motion_load, stage_fog_set and stage_set_set, which
     the MH2 PC already has from the quest path, and Lb_pl_init). Write it from MH2's asm rather than adapt G's.
   - Its stage numbers (0x83-0x89, 0xAA-0xAF, 0x56, 0x57) must be inside the host's 199-stage tables (they are below 199).
4. **Step 2 Lb_move_common 0x7A4810**, cut down: pad read, Lb_pl_move 0x797040, old_pos_save, Lb_npc_mv / Lb_npc_mk, player_mk,
   CameraMove, move_eft / move_stage, Pit_mv_lb, add_prim. Use the G-form leaf C where the MH2 function has the same shape (Lb_pl_move
   0x13C vs 0x108, Lb_npc_mv 0x118 vs 0x128, Lb_check_newCommer), each checked against MH2's asm first; fix their raw addresses with
   section 9's table (MH2V for game_w / select_w, PMO, UDMO, QWO, PLW 0x970 / EMW 0x9D0 strides). Leave lb_check_status at mode 0
   (idle) and the talk-target check out: no talking yet.
5. **Hidden check** (no window, empty /dev/input): quest 1014 to its end, then mode 6; trace lb_sys x03 / x04, cw+0x3934, PLW+0x684,
   game_w+0x14 and the hunter's position every 30 ticks; `--input "idle*30,up*120"` must move the hunter on the village stage. Shot.
6. **Area changes:** lb_basic_master 0x79C540's exits -> step 3 vs_square_exit 0x7BBD50 -> step 1 0x7BBA30 (Lb_stage_load of the new
   PLW+0x684). lb_basic_master is 0x1178 bytes and new in many places: host C from MH2's asm, exits first, talk start later.
7. Then the NPCs' talk ids: an RT_VILLAGE_TRACE-style print of each NPC's slot, position and talk id, to name the quest giver(s).

### Part 2: a quest started from the village

8. **lbguild.bin** into the host's lobby image at 0x8FAE00 (rt_lb_mem covers it: LB_SPAN 0x2C0000 on MH2). The PC currently loads
   gm_sub.bin there as an extra image; while the village runs, rt_addr for 0x8FAE00-0x924280 must give lbguild, not gm_sub.
9. **Quest records** 0x75F3B0 / 0x75F300 (groups main 0x1BB3D0, tables lbguild 0x924240) as host C. Check by printing quest 1014's
   record and its fee.
10. **The accept** 0x71A240 as host C, with a test aid in the style of RT_VILLAGE_ACCEPT (RT_MH2_VILLAGE_ACCEPT=q: call it once the
    village is ready). Writes: quest_price, cw+0x396A, 0x6ADB48 (main 0x18DED0), game_w+0x2E, PLW+0x86E, Gold_add(-fee) on
    User_data+0x60, the season / time copies.
11. **Leaving:** lb_basic_master's gate test -> x68 0x1F; lb_check_status cases 0x1F / 0x20 (stream fade, fade wait, Pit_reset
    0x33DEA0) -> 0x21; Local_main returns 1; rt_mh2_village_tick returns *(u16 *)0x6ADB48 and viewer.c's existing
    `quest_from_village()` starts the hunt (the host's rt_quest_load stands for game0 / plsel 0xD60280 / game1). Before that, a test
    aid that sets x68 = 0x1F after the accept proves the path without walking to the gate.
12. **A quest giver's menu** (the real counter): the talk path 0x796880 -> 0x7AE940 -> mode 4/5 0x7A8ED0 -> dispatcher 0x7A8700 ->
    open 0x7196F0 -> mode 0x35 -> 0x719AF0 (0x744 bytes, with its quest list 0x71A590 / 0x71A5F0 / 0x71B480 ...). This is the biggest
    piece; the four other quest givers (0x6CB690, 0x71F8A0, 0x6C9640, 0x6C20B0) follow the same pattern. Which one the village chief
    is comes from step 7.

Later: the event step 0x7BBF70 and 0x7BAB20's story events, the opening movie (task step 4), init_se's sound packs, the BGM, the real
world clock and its lighting (0x7D9EA0), the 37 service handlers (shops, forge, kitchen, farm ...), the item box (0x762500), the
bed / save, and the quit path (x68 0x23 -> 0x25 -> 0x7C9530).

## 11. Open questions

- Which area is which (0x83-0x89, 0xAA-0xAF, 0x56, 0x57), and what User_data+0x36E8 (0x57 vs 0x89 at entry) means.
- Which talk id is which NPC; which of the five quest givers is the village chief and which the guild.
- Who loads lbguild.bin (overlay number) and when; whether gm_sub must be unloaded for it.
- The meaning of game_w+0x25 (0x7CA110) and of the 0x8BDBD0..0x8BDC0F block (lb_pit's MH2 place).
- UDMO(0x20) = 0x2984 vs money at User_data+0x60.
- The time base 0x6AE398 and the float 0x6AE050 in the clock set-up.
- Task 0x7CA500 (next-task code 2) and 0x7C9740 (online step 2): online, not needed for the offline village.
