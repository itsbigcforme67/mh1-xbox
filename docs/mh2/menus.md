# MH Dos (MH2): title -> game menu -> new game / continue -> character creation -> village (MENUS research, 4 Nov 2026)

- **Scope:** a map of the boot path from the title screen to the first village tick, and a plan for `--boot` to walk it on the PC.
  Nothing was built, run or changed; no C was written. Only this file was added.
- **Source:** our own disassembly of the owner's disc (asm/mh2/select, gm_sub, sub_main, main), read by hand with two throwaway
  scripts in the scratchpad (print a function by its symbol bounds; list its calls). Compared with our G/MH1 C: src/select/edit_nm.c
  (G's Edit_task), src/main/omake/omake.c (G's Select_task family), src/select/demo.c, src/pc/rt/rt_boot.c, src/pc/title_mh2.c,
  docs/mhg.md, docs/RELEASE_G.md (G2). The community mh2-decomp repo was not opened.
- **Proof level:** "read" = followed instruction by instruction; "skimmed" = only its calls, constants and globals looked at;
  **guess** = a meaning the code does not name. G counterparts come from config/mh2/func_pairs.txt / c_aliases.txt where they say so,
  otherwise from call shape and position (marked).
- **Not here:** the village task, its first-visit movie and Local_main are in docs/mh2/village.md; the card, MemcardWork and the
  User_data layout of a save are for docs/mh2/save.md (being written by another agent). This file stops where village.md starts.
- All addresses are MH2 JP 1.04. select.bin is at 0x6AF200 (overlay slot A), gm_sub.bin at 0x8FAE00 (slot B).

## 1. In plain words

The PS2 path is the same idea as G's (title, game menu, NEW GAME -> character creation -> card save -> village; CONTINUE -> card load
-> village), but MH2 moved and rewrote nearly every piece:

- The **game menu task (Select_task) moved out of the main executable into gm_sub.bin** (0x8FAFD0). Its helpers and its drawing stay in
  select.bin, so the menu needs **both overlays at once**. MH2's Load_overlay brings gm_sub in on its own whenever it loads select or
  game (section 7).
- Pressing START on the title (G's csub00, Select_task step 0) is now inside the title task itself.
- **Character creation (Edit_task, select 0x6B1EA0) has 14 steps where G has 12.** It starts with a male / female choice and shows one
  preview hunter instead of G's two side by side; the edit menu has 7 rows with new sub-screens; there is a random-hunter button.
- **The hand-over to the village is a request, not a task start.** Where G's Edit_task calls Tsk_Execute(Game_task), MH2's ends the
  menu task and sets a "next task" code that the scheduler turns into Load_overlay(lobby) + the village task (village.md section 2).
- The opening movie is not in character creation any more: the village task plays it on the first visit (village.md section 3, step 4).
  Edit_task still carries a movie branch, but nothing in Edit_task reaches it (section 5, step 10).

So, as with the quest and the village, G's C for these tasks (omake.c Select_task, edit_nm.c Edit_task / Cont_task, demo.c) does not fit
MH2. The plan (section 10) is host C written from MH2's asm for the task layer, reusing G-form leaf C where MH2's function has the same
shape.

## 2. The chain, step by step

| # | MH2 (read unless marked) | G counterpart | what happens |
|---|---|---|---|
| 1 | Init_task select 0x6B5CF0 (9 steps, jtbl 0x6CDF20) | Init_task (select) | textures, sound packs, card check, then Demo_task. On the PC: src/pc/title_mh2.c `mh2_Init_task` (stand-ins listed there) |
| 2 | Demo_task select 0x6B5230, slot 3 | Demo_task (src/select/demo.c) | step 0: all_reset, Quest_init, 0x36B130, 0x6B51B0, fade_reset, texture list 0 (sub_main 0x334260), 0x3D54E0. If system_w+0x44 == 1: Select_task in slot 2 at once. Else by **system_w+0xBC** (the "how to enter the title" code): 0 -> the title / attract, 1 or 2 -> Select_task at once (code back to 0). Step 1: 0x6B5400 |
| 3 | 0x6B5400 (title state on the demo work, pointer D_006D2180) | Demo_task modes | state 0: fade in; state 1: by demo+0x38 (0x6B51E0 sets it from system_w+0x24: 0 / 1 / other -> 0 / 1 / 2): 0 -> 0x6B58D0 (logos and opening movie, **guess** from title_mh2.c), 1 / 2 -> 0x6B5A90; state 2 -> Tsk_Execute(Select_task, slot 2), Tsk_Exit |
| 4 | 0x6B5A90 (title screen) | G csub00 (Select_task step 0) + Demo_task title mode | draws the title (0x6B5660) or, in mode 2, a loading message (disp_load_msg). **START (0x8000) or circle (0x20):** decide sound 0x36B9C0 and fade_set(5) with a 60-tick wait (mode 1) or fade_set(0xB) and 10 (mode 0); demo+0x34 = 1. When the wait runs out with demo+0x34 set: demo state 2 (row 3). Without a press the timer (demo+0x20) runs out and the title goes back to the attract loop |
| 5 | **Select_task gm_sub 0x8FAFD0**, slot 2 (c_aliases.txt: `sub_main Select_task 0x008FAFD0`) | Select_task (main, omake.c) | the game menu, section 4 |
| 6a | NEW GAME: Edit_task select 0x6B1EA0, slot 4 (func_pairs: G 0x546E80) | Edit_task (edit_nm.c, G branch) | Select_task starts it, then Tsk_Sleep(2) (sub_main 0x33D5F0, matched C). Section 5 |
| 6b | CONTINUE: Cont_task select 0x6B3B50, slot 4 (func_pairs: G 0x548AC0) | Cont_task | card load, section 6 (skimmed) |
| 7 | Edit_task step 10 / mode_sel_end 0x8FBD90 | G Edit_task step 9 / mode_sel_end | system_w bytes (section 8), then **sub_main 0x33D6A0**: Tsk_Exit(slot 2, the sleeping Select_task) and system_w+0xBD = 1 |
| 8 | scheduler sub_main 0x33D2C0, next-task code 1 | (G: Tsk_Execute(Game_task, 5)) | Load_overlay(4) = lobby.bin, Tsk_Execute(village task lobby 0x7C90C0, slot 5). Code 2 = the network town task 0x7CA500. From here: village.md |
| 9 | village task step 0 | G Game_task offline | Event_flag 0 never set on a new hunter (user_data_copy clears User_data) -> village task step 4, the first-visit movie, then the village set-up (village.md sections 3-4) |

Task slots (tcb_w 0x5B3640, 16 x 0x20; sub_main 0x33D680(n) returns slot n's TCB): 1 = Select_Tsk_Execute's slot (MH1's, see section 4),
2 = Select_task, 3 = Demo_task, 4 = Edit_task / Cont_task / the menu's other tasks, 5 = the village task or the game task, 9 = Fade_task.

## 3. The title's ways in and out (read)

Small sub_main functions that every "back to the title" path uses; they all start Demo_task in slot 3 and set system_w+4 = 2:

| function | system_w+0xBC | used by |
|---|---|---|
| 0x33D6E0 | 1 (straight to the menu) | Cont_task failing after a row-5 start; sub_main 0x36B640 (a full reset that also reloads select) |
| 0x33D720 | 2 (straight to the menu) | Cont_task's end with system_w+0x48 set (section 6) |
| 0x33D760 | unchanged | village quit (village.md 0x7C9530) |
| 0x33D790 | 0 (title / attract) | the game menu's cancel (state 0x14) |

And the two "next task" requests: 0x33D6A0 (code 1, the village) and 0x33D6C0 (code 2, the network town). Both end slot 2's task.

## 4. The game menu: Select_task (gm_sub 0x8FAFD0, read)

Its work is not in the TCB: step 0 takes the pointer stored at **system_w+0x8C (0x6AE01C)** into the gm_sub word D_00933380 and clears
0xB0 bytes there. docs/mh2/walk.md names 0x6AE01C `stage_hit_area_w` (a heap pointer), so the menu borrows the stage-hit heap for its work
(**guess** that nothing else uses that heap while the menu runs). Pad: held 0x6AE0B0, pressed 0x6AE0B4 (Psw, PSWV(0) / PSWV(2)).

Task step (TCB+8):

| step | what (read) | G |
|---|---|---|
| 0 | work set-up as above; system_w+0x44 == 1 -> 0x8FAE80 (reset of the work's fields, then all_reset, texture list, 0x6BCD30, Tsk_Execute(0x6BF5A0, 4), Tsk_Sleep; **guess:** the return from the gallery-like row) and return; else step 1 | csub00 / init_mode_sel |
| 1 | switch on **work+0** (four phases): 0 -> 0x8FB3B0 (setBGcolor, Init_rev_set, all_reset) then phase 1; 1 -> **the menu 0x8FB700**; 2 -> start the chosen task (below); 3 -> start one of the other rows' tasks (below). Then, if the draw callback work+0x38 is set and is not 0x6BA810, TransSet(work+0x34); if work+0x38 is set, 0x36D120(work+0x38, 1) | init_mode_sel + mode_sel |
| 3 | **mode_sel_end 0x8FBD90** (func_pairs, G main 0x1EF060, score 0.55) | mode_sel_end |
| 4 | **mode_sel_exit 0x8FBED0** (edited, 0.92 to G's): fade, all_reset, Tsk_Exit, Demo_task in slot 3 | mode_sel_exit |
| any but 0 | 0x377800 (**guess:** View_move), TransSet(trans) | View_move, trans |

Phase 2 (the row chosen, after the fade): row 0 -> system_w+0xC5 = 0, **Tsk_Execute(Edit_task, 4)**, Tsk_Sleep(2), phase 1 with menu state
0xB (the menu re-opens there when Select_task is woken). Row 1 -> if work+0x30 >= 0 (a sub-choice was made, see state 5) step 3, else
**Tsk_Execute(Cont_task, 4)**, Tsk_Sleep(2), phase 1 / state 0xB. Phase 3: row 2 -> select 0x6BF5A0 (slot 4), row 3 -> 0x6B6840, row 4 ->
0x6B8C50, row 5 -> 0x6C2E60 (row 2 and 4 then menu state 0xB, rows 3 and 5 state 0xA).

### The menu 0x8FB700 (read; states in work+4)

| state | what |
|---|---|
| 0 | 0x8FB3E0 (all_reset, texture list 0, 0x6BCD30(0), **Menu_snd_load**, setBGcolor; it reads system_w+0x44 / 0xC4 and system_w+0xBC, then fills the work: draw 0x6BA0C0 ...) and fade_set(0xC) |
| 1 | after the fade: state 2. If the menu was shown before (work+0x20): straight to state 5 (work+0x2C == 1, the CONTINUE sub-choice) or state 3; else 0x6B9B70 (sets six random values, **guess:** the background) and a 4-tick count |
| 2 | the count; then state 3 (draw callback 0x6B9CF0, 0x6B9B10(-1)) or, with work+0x2C == 1, 0x6BB230(0) and state 5 (draw 0x6BA810). work+0x20 = 1 |
| 3 | **the rows**: up / down through sel_sel_sub (gm_sub 0x8FB570, identical to MH1's) on work+0x24, 0x6B9B10 / 0x6B9B30 on a change. Circle or START: decide sound, work+0x2C = row, state 4, fade_set(0xB) for rows 0-4. Cross (0x40): cancel sound, state 0x14, fade_set(0xB) |
| 4 | after the fade: str_stop(0); rows 0 / 1 -> phase 2 (and the slot byte 0x6ADC90 = 0 while no sub-choice was made); others -> phase 3 |
| 5 | the CONTINUE sub-choice 0x8FBFA0 (calls select 0x6BB430 / 0x6BB3F0 and gm_sub 0x8FB640): -1 -> cancel sound, state 6; 1 -> work+0x30 = work+0x28, byte 0x5D6B39 = it, byte 0x5D4780 = 0, decide sound, state 4, fade_set(5) |
| 6 | after the fade: state 0xB-like reopen (draw 0x6B9CF0), system_w+0xBC = 1, title stream str_play(0, 0x2F) |
| 0xA, 0xB, 0xC | reopen after a row's task: all_reset, texture list 0, fade_set(0xC), state 1 with work+0x20 = 1 and the count at 4; 0xB also clears system_w+0xC5, work+0x2C, work+0x30, runs 0x6B9B70(1), plays stream 0x2F and sets draw 0x6BA0C0; 0xC runs 0x8FAF40 first |
| 0x14 | after the cancel fade: Tsk_Exit(slot 2), back to the title (0x33D790) |

**Rows.** sel_sel_sub's limit looks like 6 (the register is set to 6 earlier in the function; not traced through the delay slots: **guess**),
so six rows. The row art is sprites (0x6B9D20 draws strips from the table at select 0x6CB110 with flps0008), not font text, so the labels
were not read. By what each row starts:

| row | starts | reading |
|---|---|---|
| 0 | Edit_task 0x6B1EA0 | NEW GAME (read: it is the task func_pairs names Edit_task) |
| 1 | Cont_task 0x6B3B50, then the sub-choice and mode_sel_end | CONTINUE (read) |
| 2 | select 0x6BF5A0 (calls 0x6BF730, 0x6BFC50, 0x6C0AF0; near Quest_clear_bit_set) | **guess:** GALLERY / extras ("おまけ" strings sit in select's data at 0x6D01B4, 0x6D06A0) |
| 3 | select 0x6B6840 (0x6B87A0(0), 0x6B84C0, 0x6B89E0; option_sub_menu is 0x6B7B20) | **guess:** OPTIONS. The map's name em19_next_act_set there is wrong |
| 4 | select 0x6B8C50 (the same helpers, mode 1) | **guess:** NETWORK OPTIONS |
| 5 | select 0x6C2E60 (0xEFC bytes: sceDevctl, tags written into a work, then Tsk_Execute(Cont_task) with system_w+0x48 set) | unknown; a load from another device or a data import (**guess**) |

G's menu has five rows (NEW GAME / CONTINUE / GALLERY / OPTIONS / NETWORK OPTIONS, RELEASE_G.md GB2); MH2's sixth is the unknown row 5.

### mode_sel_end 0x8FBD90 (read)

For rows 0 / 1 after the fade: system_w+0x44 = 0, +0x45 = 0; by the sub-choice work+0x28: 0 -> system_w+0x10 = 1, +0x43 = 0;
1 -> +0x10 = 0, +0x43 = 0; 2 -> +0x43 = 1, +0x10 = 0. Then +0x02 = 0, +0x03 = 1, +0x05 = 0, Tsk_Exit(self); system_w+0x10 == 0 ->
**0x33D6A0 (village)**, else 0x33D6C0 (network town). G's system_w+0x10 is "go to town (network mode)" (rt_boot.c Game_task), so the
reading is: sub-choice 0 = online, 1 = offline village, 2 = a third mode with system_w+0x43 (**guess** for 0 and 2).

### A loose end: Select_Tsk_Execute

Edit_task's "quit" (step 12) and Cont_task's failure paths call Select_Tsk_Execute (sub_main 0x33D590, matched C: MH1's, **slot 1**, step 1),
while the menu runs in **slot 2** (Demo_task starts it there, 0x33D6A0 ends slot 2). Read as written, quitting character creation starts a
second Select_task in slot 1 at step 1 (phase from the work, so the menu re-opens) and leaves the sleeping one in slot 2. Whether that is
harmless (the scheduler skips sleeping tasks) or something else wakes slot 2 was not checked. Keep the PS2's order on the PC and check in a run.

## 5. Character creation: Edit_task (select 0x6B1EA0, read)

Every tick first: edit_w+0x3D and +0x43 count down; while +0x43 (an input lock) is non-zero the four pad copies at edit_w+0x48..+0x4E
are zero, else they take Psw pressed / old / held and PSWV(12) (0x6AE0B4 / B2 / B0 / C8; new in MH2: G reads Psw directly). The key repeat
(+0x3E / +0x40) is G's. If edit_w+0x5F is set, circle or cross clears it with a sound and blanks the pads for the tick. SetTrnslMode(4, 5),
font_set_stack_no(0). Steps (TCB+8, jtbl 0x6CDE30); G's step numbers from edit_nm.c's G branch:

| step | MH2 | G |
|---|---|---|
| 0 | all_reset, texture list 2 (0x334260), 0x6BCD30(2), 0x334680(0xC, 0x16B, 0) (**guess:** one more texture file into slot 0xC), fade_reset, **char_make_init(0)** (0x6AF280), 0x377770 (**guess** View_init), CameraInit, 0x31A580 (clears 0x680 bytes at 0x5AF420: **guess** CameraWorkInit). demo_prim (0x6A39D0) +0x18 = task, +0x14 = edit_trans; a **second prim** 0x6A39F0 +0x14 = 0x6B1E00, +0x1C = task (new) | 0, substep 0 |
| 1 | edit_se_load(t) (main 0x195970, edited 0.93) until done; fade_set(2) | 0, substep 1 |
| 2 | after the fade: edit_w+4 = 0, +6 = 0, 0x6BCE20, input lock 30 ticks | 0, substeps 2-3 (G's card check for an MH1 hunter to carry over: **not in MH2**) |
| 3 | **male / female**: left / right (0x800 / 0x400) move edit_w+4 with a cursor sound panned to that side. Circle: step 4; if the sex changed since the last build (edit_w+0x5C): char_make_init(1), 0x195A10(sex) (**guess:** the voice set), game_w+0x14 (stage) = 0, game_w+0x2E = 0, Quest_init, light_init(0), 0x3786A0(0) (**guess** stage_w_init), system_w+0x35 = 1, a sound, 0x36BE30 (memory mark); first time (+0x5C == 0xFF): load_pit, com_motion_load(4), 0x33D150(sex), **edit_pl_init_new(player_work, 0, 0)**; later: 0x33D1F0, flCompact, 0x33D150(sex). Then 0x36BE50, system_w+0x35 = 0, +0x5C = sex, player_work[0] +1 = 1 and +0x11 = sex, **pl_chr_set2(pl, sex ? 0x32B : 1, 0, 0)**, ed_view_set(pl, 0, 0), lpView+0x38 = 0x3F5F66F3, lpView+0x40 = 0, two sound volumes (main 0x194910(6, 0/1, 0)), 0x6BD410(0xFF). Cross: step 0xB (quit?) | 1 (preview: both hunters at once, stage 0x11) |
| 4 | **the edit menu**, rows edit_w+2 = 0..6 (up / down wrap, cursor sound se_req(1, 0xC)). Circle on row 6: step 6 (confirm), cursor on "yes" (+3 = 1). Other rows: step 5, camera ed_view_set(pl, view table 0x6C7D80[row], 1), 0x6BD4B0(row), lock 20, 0x6BD560, 0x6BD600(0); row 0 -> **SoftKeyboard_set** (sub_main 0x359D30 with 3, 0xF, 8, name: G's arguments), rows 2 / 3 / 5 -> 0x6B0B60(edit_w, 0 / 1 / 2) (a list set-up). Cross: step 0xD. **Random hunter** (0x4000 held + 0x8000 pressed, or the reverse; **guess:** L1 + R1 or similar): +0x47 = rnd & 3, +5 = rnd % 27, +8 = colour table main 0x1BB140[rnd & 15], +7 = rnd % 17, +6 = rnd % 15, 0x195A10(sex, +6), then the eight colour bytes +0x50..+0x57 from per-sex tables (male 0x6C7E50 / 0x6C7DD0 / 0x6C7F10, female 0x6C7EB0 / 0x6C7E10 / 0x6C7F30; indexed by +5 x 3, +7 x 3, +6 x 2), 0x63 read as 0 | 3 (G: 7 rows too, but other ones: name, colour, voice test, done) |
| 5 | the row's editor (jtbl 0x6CDE10), each returning to step 4 through 0x6B1E40 (step 4, 0x6BD410, camera back, lock 20, 0x6BD600(1)): row 0 SoftKeyboard_move on the name, then 0x319520 (**guess** SoftKeyboard_exit); row 1 param_change_sub on +0x47 (4 values; circle leaves); row 2 0x6B0F30; row 3 0x6B1270; row 4 **ed_color_sel** (edited, 0.86 to G's); row 5 0x6B15B0, and 0x100 plays voice +0x3C (12 voices, se_req(2, n)) | 4 |
| 6 | confirm: name_str_check (identical to MH1's) and 0x6B4B20 (a jump to cmn_mongon_check_sub, the bad-word check: G's NG_name_chk) must pass, else circle / cross go back to step 4. Yes: a sound, +1 = 0, step 7, **0x3D79C0(3)** (**guess:** McOperationSet(3), the save: G calls McOperationSet(3) at the same place). No / cross: step 4. Left / right move the yes / no cursor (+3) | 5 |
| 7 | **save**: McCardOperation until non-zero; result 2 -> +0x3B = 0 and a sound, else +0x3B = 1; **user_data_copy(edit_w, 0xFF)** (0x6AF3D0, the new hunter into User_data, below); step 8; slot byte **0x6ADC90** = +1; ed_view_set(pl, 2, 1); for both player_work entries: +0xA4 = 0 and 0x6AF910(pl, sex, +6) (**guess:** decide_chr_set); two volumes | 6 |
| 8 | 60 ticks, then fade_set(5) | 7 |
| 9 | after the fade: substep (TCB+9) = 0, count 10 | 8 |
| 10 | substep 0: count down, then all_reset, fade_set(2), and the exit below. Substeps 1 / 2 are a movie (0x3D40E0 start, 0x3D40D0 status 2 playing / 3 ended, START skips, 0x3D40B0 movie_stop, 0x3D40C0 movie_exit) that **no code in Edit_task reaches** (nothing sets TCB+9 to 1). **Exit:** Tsk_Exit; system_w+0x10 = 0, +0x03 = 1, +0x02 = 0, +0x05 = 0, +0x43 = 0, +0x44 = 0, +0x45 = 0; **0x33D6A0** (village request); fade_set(2) | 9 (G: Tsk_Execute(Game_task, 5); the same seven system_w bytes) |
| 11 | "quit?" (from step 3's cross): yes -> step 12 and fade_set(1); no / cross -> step 3 | 0xA |
| 12 | after the fade: Tsk_Exit, Select_Tsk_Execute, fade_set(2) (back to the game menu; section 4's loose end) | 0xB |
| 13 | "back?" (from the menu's cross): yes -> step 3 (sex choice) and 0x195230; no / cross -> step 4 | new |

After the switch, every tick (read): for player_work[0..1] in use, pl_timer_calc, 0x39C090 (**guess** hit_stop_calc), pl_chr_sub; player_mk;
light_move; the camera eases towards edit_w's eye / at (lpView+0..0x14, as G's); 0x377800; the hunter's shadow prim (add_prim on ot1);
0x6BD680; add_prim2 of demo_prim on ot0 and of the second prim on D_6A9AF8; 0x6AFBE0; 0x6BDA00; trans.

### What the new hunter gets: user_data_copy (select 0x6AF3D0, read)

Slot 0xFF -> User_data 0x5D4A60, otherwise the buffer 0x6AA2B4. Cleared first: **0x37F8 bytes** (User_data's size, as config/mh2/pc_names.txt).
Then +0 = 1 (in use), +1 = sex, +2 = edit_w+5, +3 = edit_w+7 (0x11 stored as 0x13), +4 = the colour word, +8 = the name (0x12 bytes),
+0x20D8 = edit_w+6, +0x2994 = edit_w+0x47, +0x60 (money) = 0, +0x5C = 0, five u16 0xFFFF at +0x2188, +0x20D9 = 1;
Warehouse_equip_stack(ud, 6, 0x9C, 0) and Warehouse_equip (starting weapon 0x9C, as MH1 / G); 0x3BA940(ud); bytes 0..5 at +0x209D +12n.
**If system_w+0x46 is set:** seven Ud_u_item_stack gifts (items 7, 0xAC, 0x30, 0x47, 0x8E, 0x97, 0x9A), money + 3000, Event_flag_set(0x77)
(**guess:** a bonus for having an earlier save; what sets +0x46 was not found). Then the hunter rank into +0x2726, 32 random u16 at +0x2192,
0x18DAE0(ud), ten 0x20-byte records at +0x25E4 (six 0xFFFF and a numbered default name from the format at 0x4475C0; **guess:** the
comrade / friend list), 0x3BD510(ud, 0..3). Field meanings past the name belong to docs/mh2/save.md.

Edit_w's fields as Edit_task and char_make_init use them (**guess** for the meanings past sex and name): +1 save slot, +2 menu row,
+3 yes / no cursor, +4 sex, +5 face (27), +6 a 15-value field passed to 0x195A10 (voice, **guess**), +7 hair (17), +8 colour word, +0x24 name,
+0x38 timer, +0x3A colour index, +0x3B save result, +0x3C voice test, +0x3D / +0x43 waits, +0x3E / +0x40 key repeat, +0x47 a 4-value field,
+0x48..+0x4E pad copies, +0x50..+0x57 colour bytes, +0x5C sex of the built preview (0xFF = none), +0x5D, +0x5F.

## 6. CONTINUE: Cont_task (select 0x6B3B50, skimmed)

Nine steps (jtbl 0x6CDE70). Step 0 sets up like Edit_task (texture list, 0x8FC080(0), 0x6BCD30, a texture from PIT_TEX+4, char_make_init(2),
cont_trans as the draw, stage 0, Quest_init, light) and asks for the card load (0x3D79C0(4)); step 1 runs McCardOperation; then the slot
list (user_load, edit_pl_init(pl, 1, n) per slot, the slot byte 0x6ADC90, 0x3B5F50), confirm, fade. On a failure or at the end it either
returns to the menu through Select_Tsk_Execute, or (system_w+0x48 set: started from row 5) ends slot 5's task and goes back through the
title (0x33D6E0 / 0x33D720). How Select_task then gets to state 5 (the sub-choice) with the loaded hunter was not settled: system_w+0xBC = 2
makes Demo_task restart Select_task, whose step 0 clears its work, so something else must carry "CONTINUE done" (**open**; system_w+0xC4 /
+0xC5, read by 0x8FB3E0, are the candidates). Then mode_sel_end -> 0x33D6A0 -> village.

## 7. Overlays on this path (read)

Load_overlay (main 0x18D070): for an overlay of region 2 other than lobby (that is select and game), it first calls **0x18CFE0**, which loads
overlay **5 (gm_sub.bin)** into region B unless system_w+0x53 says it is already there, and sets that byte. Loading an overlay of region 3 or 5
clears system_w+0x53 (it overwrites gm_sub). Power-on loads select through sub_main 0x2E3C2C (Load_overlay(2)), so gm_sub comes with it. While
the menu and Edit_task run, select (0x6AF200) and gm_sub (0x8FAE00) are both resident; the village request then loads lobby over select, and
gm_sub stays until lbguild or nethttp replace it.

## 8. Work areas

| what | address | size / fields |
|---|---|---|
| system_w | 0x6ADF90 | +0x02, +0x03 (= 1 on the way to the village), +0x04 (= 2 by the title starters), +0x05, +0x10 (network town), +0x24 (title mode, 0x6B51E0), +0x35 (load mode for the preview), +0x43, +0x44 (also: "go to the menu at once" at Demo_task step 0), +0x45, +0x46 (new-hunter gift, user_data_copy), +0x48 (CONTINUE started from row 5), +0x53 (gm_sub resident), **+0x8C = stage_hit_area_w (0x6AE01C), the menu's work pointer**, **+0xBC title entry code**, **+0xBD next-task code**, +0xC4, +0xC5, +0xE5 (cleared at Demo_task step 0, set by the village) |
| menu work | *(0x6AE01C), copy of the pointer in gm_sub D_00933380 | 0xB0: +0 phase, +4 menu state, +0x20 shown before, +0x24 row, +0x28 sub-choice, +0x2C chosen row, +0x30 sub-choice taken (-1 none), +0x34 / +0x38 draw callbacks, +0x50 count |
| demo work | pointer D_006D2180 (select data) | +0 state, +1 sub, +8, +0x20 timer, +0x34 START pressed, +0x38 mode |
| edit_w | 0x6ADB50 | **at least 0x60 bytes** (build/mh2/pc/symbols says 0x4C, MH1's size: too small for MH2). Fields: section 5 |
| select_w | 0x6ADBC0 (size 0xC0 in the PC symbols) | not touched by Edit_task. The slot byte **0x6ADC90** lies at select_w+0xD0, past that size: whether it is a select_w field or a variable of its own is **unknown** (G's Edit_task writes the slot to select_w+0xB6; G's select_w+0xD0 is the accepted quest) |
| Psw | 0x6AE0B0 | held +0, old +2, pressed +4, PSWV(12) +0x18 (same indices as G) |
| lpView | pointer at 0x5BAC70 | Edit_task writes +0x38 = 0x3F5F66F3 and +0x40 = 0 where G writes +0x2C = 0x3F5F66F4 and +0x34 = 0: **12 bytes later** (MH2 re-laid the view; ties in with docs/mh2/camera.md) |
| demo_prim | 0x6A39D0, second prim 0x6A39F0 | +0x14 draw, +0x18 / +0x1C task |
| tcb_w | 0x5B3640 | 16 x 0x20, slots in section 2 |
| game_w | 0x6ADCB0 | Edit / Cont set +0x14 (stage) = 0 and +0x2E (quest number) = 0 |
| User_data | 0x5D4A60 | 0x37F8; the new hunter's fields in section 5 |
| other bytes | 0x5D6B39 (the CONTINUE sub-choice copy), 0x5D4780 (cleared with it; also cleared at power-on, sub_main 0x2E3C20), 0x6AA2AC (Cont_task's slot), 0x6ADAB0 (a pointer Cont_task reads per slot) | not followed |

## 9. What G's C on this path assumes, and what MH2 needs

Unlike the village (village.md section 9), the G-form C here has almost no raw literal addresses: edit_nm.c, omake.c and demo.c use names
(system_w, game_w, select_w, edit_w, Psw, lpView, demo_prim, player_work, PIT_TEX, mem_tex), and build/mh2/pc/symbols places those at MH2's
addresses. The problem is layout and flow, not addresses:

| G-form C | assumption | MH2 |
|---|---|---|
| omake.c Select_task, csub00, init_mode_sel, mode_sel, mode_sel_end, mode_sel_exit, disp_mode_menu | Select_task is main code with its work in the TCB, 5 rows, G's row tasks (Option_task, Omake_task, gallery) | gm_sub 0x8FAFD0 with its work at *(0x6AE01C), 6 rows, select.bin's draws and tasks. **The PC must not run G's Select_task on MH2**: today Select_Tsk_Execute (e.g. rt_boot_to_select) would start it |
| omake.c `D_0035F013..15`, `D_528538`, `D_528550`, `D_002EC668`, `D_533BE0`, `D_5367F0` / `D_5375F0` | MH1 / G data names in G's menu | not needed once the MH2 menu is host C; leave unmapped |
| edit_nm.c Edit_task (G) | 12 steps, two preview hunters on stage 0x11, card op 0xB check, Tsk_Execute(Game_task) | 14 steps, one hunter, stage 0, no carry-over check, 0x33D6A0. Needs MH2's own task |
| edit_nm.c `select_w.xB6 = slot` | G's slot byte | 0x6ADC90 (see section 8) |
| edit_nm.c `B32(lpView, 0x2C)`, `B32(lpView, 0x34)` | G's view layout | +0x38, +0x40 |
| edit_nm.c `sw[0x35]`, `sw[0x10]`, `sw[0x02..0x05]`, `sw[0x43..0x45]` | G's system_w offsets | **the same offsets** in MH2 (read from Edit_task step 10 and mode_sel_end) |
| edit_nm.c user_data_copy (G) | G's User_data (0xE00 a slot) | MH2's 0x37F8 layout, section 5; UDMO (include/mh2_layouts.h) covers part of it; check against save.md |
| edit_nm.c `PSWV(n)` | Psw indices | the same; MH2 copies them into edit_w+0x48.. first |
| EDIT_W (include/select.h, 0x4C) | MH1 / G size | at least 0x60 in MH2; the PC's edit_w symbol size should grow before any C writes +0x48..+0x5F |
| demo.c Demo_task / Init_task | MH1 / G boot | title_mh2.c already replaces them on MH2 |
| rt_boot.c `Game_task` hook (boot ends when Game_task starts) | G's hand-over | MH2 never calls Tsk_Execute(Game_task) here: the hook must be the next-task code (system_w+0xBD) |

Leaf functions the PC already links in G form and MH2 has in the same or a close shape (mh2_map.csv): name_str_check (identical), sel_sel_sub
(identical), roll_move (shape 1.00), ed_view_set (0.95), edit_se_load (0.93), mode_sel_exit (0.92), ed_color_sel (0.86), edit_pl_init_new (0.83),
param_change_sub (0.83), edit_pl_init (0.82), user_load (0.82), cont_trans (0.79), edit_trans (0.75). Each needs a look at MH2's asm before use
(the edited ones read edit_w and the view at MH2's offsets). New in MH2 with no counterpart: everything else in sections 4 and 5 (the
sub-editors 0x6B0B60 / 0x6B0F30 / 0x6B1270 / 0x6B15B0, the menu draws 0x6B9B10..0x6BA810, 0x6BD410..0x6BDA00, 0x195A10, 0x194910).

## 10. Plan: `--boot` to new game -> character creation -> village

Same pattern as the quest and the village: MH2's own task layer as host C written from its asm (next to src/pc/title_mh2.c, e.g.
src/pc/rt/rt_mh2_menu.c), shared G-form leaf C where the shape matches, stand-ins logged with rt_log_standin, hidden runs only.

1. **The next-task hook.** Host versions of sub_main 0x33D6A0 / 0x33D6C0 / 0x33D6E0 / 0x33D720 / 0x33D760 / 0x33D790 (each a few lines,
   section 3) and, after each Scheduler tick on MH2, the scheduler's tail 0x33D2C0: code 1 -> end the boot (rt_boot.c's boot_done, as G's
   Game_task hook does) and enter the village (village.md plan step 1; until it exists, log and stay dark as village_step does today);
   code 2 -> log "network town not on the PC" and treat as 1; code 3 -> the game task (the quest path, later); code 4 -> rt_import_select and
   Demo_task in slot 3. Check: a test aid (say RT_MH2_NEXT=1) that sets code 1 on the title and the log shows the request taken.
2. **START on the title.** In title_mh2.c, MH2's title states 0x6B5400 / 0x6B5A90 instead of the fixed title: START or circle, the fade, then
   Tsk_Execute(Select_task, slot 2). Keep the movie stand-in (0x6B58D0) and take mode 1 (title only). Check: `--input "idle*150,start*2"`,
   log shows Select_task started, shot.
3. **The game menu.** Select_task 0x8FAFD0, the menu 0x8FB700 with 0x8FB3E0, mode_sel_end 0x8FBD90, mode_sel_exit as host C. Work: a host
   buffer of 0xB0 standing for *(0x6AE01C) (log it; on the PS2 it is the stage-hit heap). Draws first as font rows with a stand-in log, then
   MH2's own (0x6B9CF0 / 0x6B9D20 row strips from 0x6CB110, 0x6BA810, 0x6BA0C0). Rows 2-5: stand-ins that log and return to state 0xB.
   Menu_snd_load and the title stream through the existing MH2 sound path (docs/mh2/platform.md) or silent. Check: shots of the menu with
   the cursor on rows 0 and 1; cross goes back to the title.
4. **Edit_task 0x6B1EA0** as host C with MH2's 14 steps, in this order: steps 0-3 (set-up, sound load, sex choice with the preview hunter),
   then 4 and 5 for row 0 (name through the existing SoftKeyboard C, checked against MH2's 0x359D30 / 0x319520 first) and row 6, then 6
   (confirm). Leaves: char_make_init 0x6AF280 (new, 0x134 bytes: write it), edit_pl_init_new / ed_view_set / pl_chr_set2 / roll_move from the
   G-form C after a check against MH2's asm (MH2 builds one hunter: player_work[0], ids 1 / 0x32B as G), view writes at lpView+0x38 / +0x40,
   rt_hunter_draw_hook = ed_hunter_hook as G's boot does. The sub-editors (rows 1-5) and the random button after the name works.
   Check: hidden run through sex choice, name "TEST", confirm; shots per step; the log shows the hunter built once per sex change.
5. **The save (steps 7-9).** user_data_copy 0x6AF3D0 as host C writing MH2's fields (section 5) into User_data; the card op 3 through
   docs/mh2/save.md's path. Until that lands: a stand-in McCardOperation that answers "saved" (1) and logs, so the chain runs on. The slot byte
   0x6ADC90. Check: dump of User_data's first 0x40 bytes after the save (in use, sex, name, colour) and the money 0.
6. **To the village.** Step 10's exit: the seven system_w bytes, then the host 0x33D6A0 -> step 1's hook -> village.md Part 1 (its step 1
   says how the village task's step 4, the first-visit movie, is skipped at first). Check: the log line "boot: new hunter saved, village
   requested" and village.md's checks from there.
7. **CONTINUE.** Cont_task 0x6B3B50 (9 steps) with the card load from save.md, the menu's state 5 (0x8FBFA0) and mode_sel_end. Settle first how
   the menu learns the load is done (section 6). Check: new game -> save -> quit to the title (debug menu) -> CONTINUE -> village with the
   same hunter.
8. **Test script.** tools/pc_scripts/newgame_mh2.txt in the style of newgame.txt (title START, row 0, circle on "male", name, row 6, yes), so
   the whole path can be rerun hidden; and the "quit?" / "back?" exits (steps 11-13) once each.

Later: rows 2-5 (gallery, options, network options, row 5), the opening movie of 0x6B58D0 and the village's first-visit movie (AFS02, not read
yet), the card check of Init_task, sound packs and the title stream, the random hunter, the system_w+0x46 gift.

## 11. Open questions

- Who sets system_w+0x24 (title mode 0 / 1 / 2), system_w+0x46 (the new-hunter gift) and system_w+0x48 / row 5's meaning.
- The labels of rows 2-5 (sprites from 0x6CB110 against the title strips) and what row 5 (0x6C2E60, sceDevctl) does.
- How CONTINUE's result reaches Select_task after the restart through Demo_task (section 6), and whether the slot-1 Select_Tsk_Execute
  leftover (section 4) matters.
- The meanings of edit_w +5, +6, +7, +0x47 and the colour tables 0x6C7DD0..0x6C7F31; the 15 / 17 / 27 counts are read, the names are not.
- Whether 0x6ADC90 is a select_w field; why the PC symbol view sizes select_w 0xC0 and edit_w 0x4C.
- sel_sel_sub's row limit (6 is a reading of a register set before the call, not followed through every branch).
- 0x334680(0xC, 0x16B, 0) in Edit_task step 0 and 0x6BCD30's job (both look like texture loads: **guess**).
