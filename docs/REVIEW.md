# Changes for the owner to review

The owner asked (11 Oct 2026) that every change which gives up old code for a
new approach is listed here, even when we go ahead with it, so they can look
at it later. Each entry: what was dropped, what replaced it, why, and the
commit to revert. Newest first. Mark an entry "reviewed" once the owner has
looked at it; do not delete entries.

Agents and the coordinator: add an entry in the same change whenever you
remove or replace a working (or supposedly working) path, not only for
obvious workarounds.

## Open

### G gallery event scenes and credits clips; G's monster event demos (21 Oct 2026, agent E)
- **Dropped:** the GALLERY's "closed entry" answer for its 15 event scenes (omake_g.c) and the black credits; **now**
  the host plays the scenes (viewer.c gal_scene_*: the quest's stage and monsters, the event-demo camera, no hunter) and
  the credits start G's ROLL01..06 clips (rt_movie_clip). **Why:** GB2's open part.
- **Changed (G):** the monsters' event-demo checks in em04b, em12_nm, em19b, em20_ai_nm compare G's quest numbers
  (include/em_demo_g.h QN(mh1, g); MH1 unchanged; em01/em16/em27 had the same fix on main from the G quest lane, kept);
  G-only demo acts added for em02 (kinds != 2, stage 0x58), em04, em09, em15, em19, em20 from G's asm; em02's G branch keys on the kind, not on MH1's quest
  list. em20_ai_nm read MH1's info_stop address (0x3F360F) in both games: now game_w.info_stop. **Why:** G's in-quest
  first-sight demos had the camera but the monster did not act (also in quests, not only the gallery).
- **Changed (G):** G's per-stage tables (92 entries), stage_start_pos and Demo_cam_tbl (38) imported at G's sizes, the
  unnamed demo camera lists 5-9 / 35-37 imported (config/mhg/pc_data.txt); viewer.c's stage limit 88 -> NSTAGE (92 in
  G); the credits' page 4/5 first lines kept as numbers (pc_noptr.txt). The staff roll's logo marked its page even off
  screen (omake_g.c gal_staff_logo, against G's 0x4E8AE0): fixed.
- **To revert:** the commit "G gallery: event scenes, credits clips ..." on agent-E.

### Xbox log written through to the disk (21 Oct 2026, agent A)
- **Changed (MH1 and G, Xbox):** the log files are committed to the disk (fflush + NtFlushBuffersFile) at most every
  0.5 s and on every warning / error, and stderr once a second (was: fflush only, every 3 s, which a crash never wrote
  out). **Why:** the owner's crash log stopped long before the crash. **Cost:** a disk write per half second while
  logging. **To revert:** the commit "Xbox G: the Elder's quest list crash ...".

### G quest lane, seventh pass (20 Oct 2026, agent A; RELEASE_G section 13)
- **Changed (MH1 and G):** viewer.c em_slot_sync no longer draws a monster whose x01 (em+1) is 0 (was: drawn whenever
  it was alive on the stage). **Why:** the game's em_move only queues the monster's prim when x01 != 0; G's quest
  10035 hides two Velociprey that way during the opening scene and the PC showed them.
  **To revert:** the commit "G quest lane: em16 (Velociprey family) as G; hidden monsters are not drawn (both games)".
- **Dropped (G PC only):** MH1's em27 die04 (revival + start point inside die04) for G; **now** G's flow: em27_act_set
  (5, 4) does the revival and the start point (em27_revive_init), die04 only picks the act. **Why:** porting die04
  alone (16 Oct) made the Velocidrome family come back forever; this is how G does it.
- **Dropped (G PC only):** MH1's em17 attack dispatch (em_move03) and the 10043 demo (em_demo00) for G; **now** G's
  12-entry table (MH1's atk07 is not reached in G) and G's demo. MH1's em17 atk04 / atk05 / atk10 / atk17 / atk22
  bodies replaced by G's for G. em01's atk09 / 10 / 17 / 22 likewise. MH1 builds unchanged.
- **Dropped (G PC only):** set04_i / _m / _trans MH1 bodies for G (G's from the asm).
- **To revert:** the commits "G quest lane: ..." on agent-A (20 Oct 2026).

### G player packages show their window by themselves (20 Oct 2026, agent E)
- **Dropped:** `MH1_SHOW=1` in the G play scripts (dist/linux/MHG/play.sh written by `./mh1`, tools/win/play_g.bat).
  **Now:** a file PLAYER.txt next to the program (written by `./mh1 linux|windows` for both games and by
  tools/package_win.sh) lets the G build open its window unless MH1_HIDDEN is set (viewer.c, install_player_package
  in install.c). Build-folder runs stay hidden as before; MH1_SHOW=1 still forces a window (tools/play_g.sh).
  **Why:** the README says to drag the .iso onto mhview_g.exe, which bypassed play.bat and so refused to start; and a
  play script that set MH1_SHOW opened a window even for an agent with MH1_HIDDEN=1.
- **Changed (G PC):** the settings file is G's own `mhgpc.ini` (was `mh1pc.ini`, already in G's folder); an old file
  is renamed once at start (gfx_opts.c). Window title "Monster Hunter G" / "Monster Hunter" (was "MH1 PC viewer").
- **To revert:** the commit "G player packages: PLAYER.txt ..." on agent-E.

### G release audit fixes (15 Oct 2026, agent A)
- **Dropped:** the G stand-in for Lb_make_quest_tbl_local in lb_village_nm.c's G vs_square_init_init (and its G_STANDIN
  macro); **now** G's own builder (src/lobby/f/lb_guild_g.c) runs at village entry. **Why:** the elder's lists were empty.
- **Changed (G PC only):** tools/gen_rt_auto.py: a `D_xxxxxx` name that G's symbol view places (pc_names.txt /
  data_map.txt) gets that G address instead of its hex part read as an MH1 address (two names moved: D_002EC668,
  D_004FEA50). tools/mhg_pcmeta.py: a pc_names.txt line ending in `+` adds a second name at an address instead of
  renaming the one there. **Why:** CONTINUE crashed in the game menu; the reward screen's item line was blank.
- **Changed (MH1 and G):** tools/build_pc.sh aliases flfntLocate_i to flfntLocate (was a no-op stand-in).
- **To revert:** the commit "G release checklist (docs/RELEASE_G.md) ...".

### G round 7: hunting log, thrown items, Lao-Shan (15 Oct 2026, agent A)
- **Dropped (G PC build only):** the matched MH1 menu27.c / menu32.c / menu33.c / menu_dmon.c (tools/pc_g_exclude.txt);
  **now** G's hunting log (G blocks in menu_nm.c / menu_disp_nm.c), with G's order of the data pages (log first).
  MH1 builds unchanged. **Revert:** the commit "G PC: the pause menu's hunting log and its records".
- **Dropped (G PC):** MH1's shell03 / shell09 / eft12 i04-t04 C for G (the G blocks replace them under
  `VERSION_MHG && !__MWERKS__`): shell09's atck_data_no lookup (an empty work area on the G PC) is gone for G.
  **Revert:** the commit "G PC: G's thrown items and smoke".
- **Dropped:** the MH1 EvDemoInitialize (MH1 quest numbers) on the G PC; **now** G's (G quest numbers + per-kind
  demos). G quests' event demos now run on the PC (they never did): the Kut-Ku / Lao-Shan first-sight demos stop the
  hunter and the quest clock as on the PS2. **Revert:** the commit "G PC: Lao-Shan quests play out".
- **Choice:** test_all_quests.py (G) gives the time-out run 150 s instead of 60 (a quest's event demo stops the quest
  clock) and lets the Lao-Shan hunter wait on stage 12 (60000 ticks) instead of fighting it from the start.
- **Choice (PC):** rt_2d.c load_pit loads G's filedef_sys 7 into MH1's slot 0x11B for G (G loads it into 0x142).

### Prototype gunner HUD from the unused cockpit2 mock-up (14 Oct 2026, agent F)
- **Dropped:** nothing; the HUD_ANCHOR one-liners for disp_pl_vital and disp_item in rt_2d.c became small functions
  that first ask rt_hud_gun.c (unchanged behaviour with the option off, the default).
- **Now:** `hud = prototype gunner` (F10 menu / mh1pc.ini / --hud) draws a sword gauge, an ammo wheel and an item wheel
  from cockpit2.apx for bowgun users (docs/pc.md "Prototype gunner HUD").
- **Design choices for the owner to judge:** health scale 150 = full blade slot, stamina 150 = full hilt slot; the
  wheel shows next two / previous two types; baked text, icons and digits blacked out and redrawn with the game's font
  and icons (crag shot keeps the baked yellow pouch); names in the mock-up's green; tabs and the bottom gauge unused;
  "Restore display defaults" also sets the HUD back to original; the original bullet row (disp_cannon) is kept.
- **Revert:** this commit (git log --grep "prototype gunner HUD").

### G on the PC: EMW in MH1's layout, stage-timer lists expanded (14 Oct 2026, agent A)
- **Dropped:** the G -8 shift of EMW's tail (include/em.h `_pad960`) and of EM_FIELD (em_cmd.h) in the PC
  build (kept for MWCC); the raw 0xA03/0xA04 reads (hit_nm.c, em_cmd 0x71); pc_data's per-stage imports of
  em01/em15/em21 runaway and em14 stay timers.
- **Now:** EMW named fields size/meat_no/lv; the timers as G's {stage, value} lists expanded by
  rt_data.c g_stage_lists; gen_rt_tables lets pc_data win over tables.txt MH1-address lines.
- **Why:** the PC's C mixes fields and raw MH1 offsets; the old imports read lists as arrays. Revert: this
  commit (git log --grep "G hunts: monster state").

### Loading screens: the game's own instead of stand-ins (13 Oct 2026, agent F)
- **Dropped:** the empty stand-ins flFlip, Disp_load_start, Disp_NowLoading2 (rt_flow.c) and
  Disp_NowLoading (rt_village.c, "the PC loads at once, nothing to show").
- **Now:** the game's own code draws them (disp1_nm.c, src/main/font/dsp02.c added to the PC/Xbox
  build) and flFlip shows what the tick drew since the last flip as a frame at once (viewer.c
  game_flip, gfx_rec.c recording). So an area change shows the whole-area map with "NOW LOADING"
  (game2 step 2: DispWholeMap + flFlip before st_model_load), a quest start the quest title
  (Disp_load_start, called in quest_from_village where game11 calls it), the village's stage
  changes Disp_NowLoading. As on the PS2.
- **Why:** on the Xbox an area change took seconds with a frozen picture (docs/xbox.md "Area
  change loads"). Headless --shot runs draw no extra frames (RT_FLIP_SHOT=prefix writes them).
- **To revert:** the commit "Loading screens: the game's own (flFlip shows a frame)".

### Xbox: big freads and texture swizzle (13 Oct 2026, agent F)
- **Dropped:** pdclib's fread for reads of 8 KB and more on read-only streams; the per-texel
  bit-deposit swizzle writing straight into write-combined GPU memory (gfx_nv2a.c).
- **Now:** xbox_fread (xbox_libc.c) copies what pdclib's buffer holds and ReadFiles the rest; the
  swizzle steps x/y with (v - mask) & mask into cached memory, then one memcpy.
- **Why:** reads were 2.2-3.7 s of a 3-4.7 s area change in xemu (1 KB kernel reads, a byte loop).
- **To revert:** commit c9911d68.

### Release entry points: ./mh1, tools/disc_files.py, docs/PLAYING.md (13 Oct 2026, agent D)
- **Added, nothing dropped:** `./mh1 linux|windows|xbox DISC` chains the existing scripts (build_pc.sh, build_win.sh +
  package_win.sh, build_pc.sh + build_xbox.py --gfx nv2a) after verifying the disc by size and CRC32, and copies the
  disc files into `dist/<target>/`. `tools/play.sh`, `tools/xbox_upload.sh` (developer loader) and the exe's own
  ISO installer (src/pc/install.c, same CRC table as tools/disc_files.py) stay as they are; nothing was
  superseded, so no old script was removed. tools/package_win.sh got two small fixes (works outside a git
  checkout; finds llvm-strip under WINROOT).
- **Decision for the owner:** the Xbox release is a plain `default.xbe` with `data/` beside it (the game itself,
  not the network loader), and Xbox online is not offered.

### Xbox: CPU near clipping of skinned models removed (13 Oct 2026, owner's decision)
- **Dropped:** gfx_nv2a.c skinned batches near the camera skinned and clipped on the CPU
  (agent E, b85749b0 and the clipping part of 8ba1612c).
- **Why:** the owner didn't see the carcass flicker again and the frame rate felt worse; unclear
  it fixed a real problem. Kept as candidate fix 1 of 3 in docs/xbox.md "Near clipping of
  skinned models" (the other two are GPU-only).

### Browser draw-work pool: one TU lb_bswork.c (12 Oct 2026, agent B)
- **Dropped:** runs src/lobby/f/lb_{gav01,av01,gz01,av02}.c and agent F's unbuilt whole-file copy lb_av.c.
- **Now:** src/lobby/f/lb_bswork.c (0x5E9960-0x5E9EC8), all 11 functions byte-matched (BsTextureAdd new). It and
  lb_ao01.c (the browser allocator) are linked on the PC (tools/pc_lobby_matched.txt): seven functions there were
  stand-ins (BsWorkInitAll, BsWorkMove, BsWorkTrans, BsTextureFreeAll, BsMemAlloc / Free / Realloc ...).
- **To revert:** the commit "lobby: browser draw-work pool as one TU (lb_bswork.c)".

### Browser allocator: unbuilt near-match copy lb_ao.c removed (12 Oct 2026, agent B)
- **Dropped:** src/lobby/f/lb_ao.c (agent F's whole-file near-match of the browser cell allocator, not built).
- **Now:** all six functions are byte-matched in src/lobby/f/lb_ao01.c (BsMemRealloc added, 0x5E8330-0x5E867C).
- **Why:** obsolete copy. **To revert:** the commit "lobby: BsMemRealloc matched; lb_ao.c removed".

### Browser URL helpers: one TU lb_url.c (12 Oct 2026, agent B)
- **Dropped:** the runs src/lobby/f/lb_{m12,z135,z19,ap01,z106,z20,gdr12,gap01,z21,gap02,ap02}.c and agent F's
  whole-file near-match src/lobby/f/lb_ap.c (not built anywhere).
- **Now:** src/lobby/f/lb_url.c, 0x5E8890-0x5E9960, all 25 functions byte-matched (BsUrl*, bs_url_*, the
  Shift-JIS/EUC kanji conversion). The draft-style run C (F(s8, ...) offsets, int pointers) was rewritten as plain C;
  it still matches. Linked on the PC too (tools/pc_lobby_matched.txt): KanjiEuc2SjisEx and bs_url_cmp_list were
  stand-ins there.
- **Why:** the static helpers must share the TU with their callers (MWCC keeps caller values in a3/t0 across calls
  to a static whose register use it knows); one TU replaces 12 files.
- **To revert:** the commit "lobby: browser URL helpers matched as one TU (lb_url.c)".

### Lobby client: 29 b/ run files merged into lb_cli.c (12 Oct 2026, agent B)
- **Dropped:** src/lobby/b/lb_{by95,by34,bz127,bz160,bz49,bz161,bz50,by39,by01,bz51,by03,cb05,by02,cb04,rm01,by04,bz52,
  bz53,bz54,cb02,cb01,bz55,bz56,bz57,bz58,by05,bz59,cb03,by06}.c (one or a few matched functions each, 0x5C0680-0x5C1B5C:
  mail / net comment requests and the server notice callbacks) and their entries in the ONLINE=1 list of
  tools/build_pc.sh.
- **Now:** the same C (unchanged apart from per-run type names, `BrPersonalData_c24`, and aliases for objects a run
  declared with its own type: config/lobby_aliases.txt) is the tail of src/lobby/f/lb_cli.c, which now covers the
  original compilation unit 0x5B7020-0x5C1B5C. CallBack_Event_MatchEntryUser / CallBack_Event_RoomLeaver (not matched)
  are original-bytes stubs there; the PC keeps their C in src/lobby/b/nm/.
- **Why:** one original source file = one TU (BRIEF); fewer files. Bytes and the PC behaviour are unchanged.
- **To revert:** the commit "lobby: lb_cli.c covers its whole compilation unit".

### Online HTML text: PC near-match copies replaced by matched C (12 Oct 2026, agent B)
- **Dropped:** src/lobby/b/nm/html_text.c (Split_TagCode, Analysis_StringData, Display_StringData),
  src/lobby/b/nm/Analysis_TagCode.c, src/lobby/b/nm/check_halfcode.c and src/lobby/b/nm/nwDispStr_Html.c, which the
  ONLINE=1 PC build linked (written from the asm on 11 Oct, never compared with check.py).
- **Now:** the six functions are byte-matched C at the end of the lobby-client TU src/lobby/f/lb_cli.c (now
  0x5B7020-0x5C0678); check_halfcode is still the original bytes on the PS2 (2 lines off as C) and its C for the
  PC sits in src/lobby/f/lb_online_nm.c with the other lb_cli holdouts.
- **Why:** the PC runs the game's own code for server messages / error dialogs instead of a hand-written copy.
- **To revert:** the commit "lobby: HTML text renderer matched in lb_cli.c".

### Player near-matches replaced by matched C: `case 0: default:` (12 Oct 2026, agent B)
- **Dropped:** src/main/pl/pl_normal_nm.c (not built: near-match normal_char_set / to_normal); the near-match
  copies of Pl_slash_calc, Pl_basic_flagset (now pl_slash.c / pl_flagset.c, PC-linked), to_normal, basic_com_ck, pl_mv021, pl_dm003 and pl_dm008 in src/main/pl/pl_nm.c (the PC linked these
  weak); `#pragma opt_common_subs off` on normal_char_set (a workaround for the same quirk).
- **Now:** all six are byte-matched C: normal_char_set.c holds normal_char_set and to_normal; f_pl.c holds the
  other four in place of their original-bytes stubs (two basic_com_ck jump tables now come from the C, two
  main:rodata lines). The PC runs this C.
- **Why:** the "reloaded constant in the switch default" open problem in docs/STATUS.md was the source writing
  `case 0: default:`; matched C replaces near-match copies. basic_com_ck's near-match was the most different
  (its game-mode 5/6 and 7/8 tests are ifs, the stick switch lists 0..4 in order).
- **To watch:** player idle stance (to_normal), the hunter's pad handling in town and field (basic_com_ck),
  flinch / knock-down states (pl_dm003/008), pl_mv021 (cart / carry), sharpness messages (Pl_slash_calc). enemy_trans
  is matched too (weapon_em.c, PS2 only; its near-match left weapon3_nm.c, which the PC never linked). Revert: the commit "pl: case 0: default:".

### PC runs more game C: monster neck joints, s16 damage (12 Oct 2026, agent B)
- **Dropped:** the no-op stand-in for mlCalcTransEM (build/pc/rt_gen.c) that enemy_mk called on the PC; the
  near-match files emmk_nm.c (all of it matched) and the copies of old_pos_save, init_pl_work, eft13_set_pos and
  several menu draw functions in the *_nm.c files (matched now).
- **Now:** mlCalcTransEM is matched game C (em/emmk01.c) and runs on the PC when game code calls enemy_mk (the tail
  cut, em_tail_off_sub): it turns the neck joints by em->neck[] and poses kind-specific joints. It reads game.bin's
  em_neck_tbl, now named in config/main_aliases.txt (the PC had only an empty D_63FF90 stand-in: the first build
  crashed in test_activities tail_cut, fixed by the name). hit_hit_sub_em (hit_nm.c, still a near-match) keeps the
  damage in an s16 as the original does.
- **Why:** matched C instead of a stand-in; the near-match is closer to the original.
- **To watch:** monster pose right after a tail cut; damage numbers over 32767 now wrap like on the PS2.
  Revert: the commit "emmk01: mlCalcTransEM and old_pos_save".

### f_quest as one file (12 Oct 2026, agent B)
- **Dropped:** the 22 per-run files of the quest file (src/main/quest/f_quest01..04.c, f_questb..r.c, qstb07.c,
  strg01.c; f_quest.c itself is now the whole file) and in src/main/quest/f_quest_nm.c the 70 near-match copies
  of functions that were matched (as runs or now).
- **Now:** src/main/quest/f_quest.c is the whole quest file (0x226C30-0x22C66C) as one TU, byte-identical, 12
  functions kept as original bytes (c_rawfuncs), their near-match C in f_quest_nm.c (PC, weak). The PC compiles
  f_quest.c with the va_list fix and its statics global. quest_em_die passes `(s16)` casts to em_work_serch2 (the
  original's K&R definition gives no prototype, the call still extends the arguments).
- **Why:** one file per original source file (MWCC context); stolen_item_stack, Ext_pick_point_init and
  em_work_serch2 match now. Tests passed: quest_loop, progression, urgent, activities, all_quests, save.
  Revert: the commit "f_quest: the quest file as one TU".

### f_pl as one file; em_dur_set's host copy; rodata slots renamed .data (12 Oct 2026, agent B)
- **Dropped:** the 57 per-run files of the player file f_pl (src/main/pl/pl01..pl48.c, plx03/04/09/12.c,
  pl_wall.c, pldmv.c, plegg.c, plegg2.c, pl_demo.c) and their 57 text / 17 rodata lines in config/c_files.txt;
  in src/main/pl/pl_nm.c the near-match copies of player_init0, pl_work_clr, pl_dm_value_sub, wall_act_ck,
  wall_vec_set, pl_mv060, pl_at009, pl_at012, pl_demo000, egg_com_ck, pl_egg03 (stale: matched C existed) and of
  the nine functions matched now; the host em_dur_set in src/pc/rt/rt_em.c; emsrch01.c / emsrch02.c (now
  em/emsrch.c).
- **Now:** src/main/pl/f_pl.c is the whole file (0x134950-0x14D1C8) as one TU, byte-identical, with six functions
  kept as original bytes (c_rawfuncs). The PC build compiles it with its statics made global (sed in
  tools/build_pc.sh) so pl_nm.c's remaining near-matches call it. The PC runs the game's em_dur_set C (near-match,
  emsrch_nm.c) instead of the host's copy. tools/build.py split_rodata_objects also handles objects whose rodata
  is renamed .data (a rodata slot in a data region) when they have several slots. include/plf.h: blend_calc
  takes s8, blend_set u16 (proven by the callers' code).
- **Why:** functions of one original source file compile differently alone (MWCC uses what it knows about earlier
  functions of the same file); as one file nine more player functions match. Fewer, truer files.
- **Cost / to watch:** the PC's player code now comes from one object; a behaviour change in the player would
  point here first (all PC tests passed: quest_loop, progression, urgent, activities, save, soak, all_quests).
  Revert: the commit "f_pl: the player file as one TU".
### Mode menu / extras menu near-match copy replaced by omake.c (12 Oct 2026, agent C)
- **Dropped:** src/main/omake/omake_nm.c (the whole-file near-match copy the
  PC linked for the title mode menu and the extras menu) and the four
  matched run files omakeb.c, omakec.c, omaked.c, omakee.c.
- **Now:** src/main/omake/omake.c, the whole original file (0x23A0F0-0x23BE10)
  as one translation unit with its statics. 17 of 20 functions match;
  mode_sel, disp_mode_menu and disp_omake_menu link the original bytes on
  the PS2 build and use their near-match C on PC/Xbox. key_rept_du,
  Sel_back_disp and Sel_menu_disp are newly matched. The PC BOOT list in
  tools/build_pc.sh and tools/pc_patch.py's Tsk_Exit(tsk) fix now name
  omake.c; omake_nm left WEAK_B.
- **Why:** owner's rule: matched game C over near-match copies. The C is the
  same code as omake_nm with source-level rewrites that keep the meaning
  (Sel_back_disp builds the colour from one byte, Put_2TF gets only the
  sprite; key_rept_du uses ++cnt; disp_omake_menu computes x as int).
- **Checked:** tools/build_pc.sh, test_menu, test_name_entry,
  test_quest_loop, test_progression, test_movie.
- **To revert:** the agent-C commit "omake: whole-file omake.c TU".

### Host pad driver and system_w_set replaced by the matched game C (12 Oct 2026, agent C)
- **Dropped:** src/pc/rt/rt_pad.c's own copy of ioRead_sub (read_port0 built
  Psw from the host pad: bit table, stick angle/power, trg/rel, key repeat)
  and src/pc/rt/rt_boot.c's copy of system_w_set.
- **Now:** src/main/sys/f_ioread.c (the whole original f_ioread file, all 13
  functions matching) is in the PC/Xbox build for ioRead, ioRead2 (with the
  static ioRead_sub) and system_w_set (PICK_X in tools/build_pc.sh). The
  host only plays the fl pad library: it fills flpad_adr[0] (buttons, stick
  direction bits beyond half way as before, stick angle in degrees and power)
  and calls the game's ioRead once per tick; ports 1-3 read as unplugged.
  The pad structures are in include/ioread.h.
- **Why:** owner's rule: matched game C over host imitations. Behaviour is
  the same by construction (same bit table, same angle formula, same repeat);
  ports 1-3 now get the game's own "unplugged" update every tick instead of
  staying untouched zeros.
- **Checked:** test_quest_loop, test_progression, test_menu, test_name_entry,
  test_pick, test_urgent, test_frog, test_activities (37) pass; Xbox build
  links (0 failed). Not run in xemu.
- **Also dropped:** the runs sys/io01-io04, ior01 and the near-match copies
  ioread_nm.c / mmv_nm.c (replaced by f_ioread.c).
- **To revert:** the agent-C commit "sys: f_ioread.c whole-file TU; PC uses
  the game's ioRead/system_w_set".

### Model work file: 13 runs and 6 near-match copies replaced by one translation unit (12 Oct 2026, agent C)
- **Dropped:** the runs model/gm01-gm05, mkm01, hp01-hp04, light03 (get_mdlw_ptr),
  crmdl03 (Pl_model_id_set) and clay/get_clay_ptr, and the near-match working
  files mkm_nm.c, getm_nm.c, getsh_nm.c, heap_nm.c, armor_nm.c; the copies of
  pl_create_model / weapon_create_model in crmdl_nm.c and of get_mdlw_ptr in
  light_nm.c. None of them was in the PC build.
- **Now:** src/main/model/f_get.c is the whole original file 0x121B70-0x1246D4
  in address order with its statics. Newly matching: Sethierarchy, mkModel,
  mkModel3, mkModel4, model_work_set2, release_model, model_work_free. Seven
  functions stay as original bytes (config/c_rawfuncs.txt) with their
  near-match C beside them: get_start_material/hierarchy/clay/mdlw,
  pl_create_model, weapon_create_model, armor_create_model.
- **Also:** getm_nm.c had model_work_set2's parameters in the wrong order
  (skin and the mkModel flags swapped); the matched version is
  (idx, mat, tex, texlist, flags, skin, type).
- **Why:** one TU per original file is what makes MWCC allocate registers
  like the original; old runs and copies would only drift.
- **To revert:** the agent-C commit "model: f_get.c whole-file TU".

### Xbox Back + Start: the normal shutdown is skipped (12 Oct 2026, commit 583d10b5)
- **Dropped:** Back + Start set `running = 0` and let main end normally (SDL audio close,
  gfx shutdown, then the atexit log screen).
- **Now:** it calls xbox_log_quit(): the log screen, then straight to the dashboard.
- **Why:** the normal end hung on hardware while the audio thread was starved. With the GPU
  waits yielding (same commit) it would likely work again, but nothing in that shutdown
  matters when the console reboots, and the short path can't hang.

### Host caches kept for the whole run replaced by frees at stage loads (12 Oct 2026, agent F)
- **Dropped:** monster models cached per kind (em_kind_model), village NPC
  models, the common motion files (plcom / lbcom / selcom) and the NPC
  motion files (com_motion_load / em_motion_load), the hunter's own plcom
  copy and every 2D texture (flCreateTextureFromApx_mem) stayed loaded until
  the program ended. The player's weapon motions were made once at start-up.
- **Now:** at each stage load (st_model_load, load_stage_models) the host
  frees what nothing refers to any more and loads it again when the game
  asks (as the PS2 does): rt_2d_gc (2D textures no mem_tex slot holds),
  rt_village_files_gc (motion files no live motion set reads),
  host_models_gc (monster kinds with no em_work entry and no live motion set,
  NPC models outside the village). The motion table's part for a model is
  emptied before create_*_motion fills it again; rt_motion_gc no longer
  counts motion players of unused works, mot1 outside a blend, or a bset not
  mixed in. The weapon motions (wNN_tbl) are made again at every quest start.
- **Why:** free memory on the Xbox fell ~1.5 MB a quest loop (xemu: 9.9 MB
  in the village before the first quest, 6.1 MB after it) and the 2D texture
  table would have filled after ~100 loops. Also the village's lbcom motions
  run past the common part of the motion table into the hunter's weapon
  entries (500-521), so later quests played lbcom motions there.
- **Cost / to watch:** monster / NPC / motion files are read again on area
  changes (e.g. the camp's Aptonoth each time the hunter comes back): more
  disc reads on the Xbox. A motion handle kept somewhere the GC does not look
  would now point at a freed set (mset_get returns NULL: no pose).

### Viewer: catching up every lost tick dropped (12 Oct 2026, agent F)
- **Dropped:** in real-time runs the viewer ran every game tick it was behind
  the clock before drawing the next frame.
- **Now:** at most 4 ticks per drawn frame (CATCH_UP in viewer.c); beyond
  that the clock is moved on and the game slows down, with a "slow:" log line.
  Fixed-time runs (--shot, the tests) are unchanged.
- **Why:** with ticks dearer than 33 ms (xemu's opening movie) the old way
  drew one frame a second and never recovered; after a load pause it also ran
  a burst of ticks. The PS2 slows down too.

### Hunter ground height: viewer's min_y_of dropped (12 Oct 2026, commit 22c82ad3)
- **Dropped:** viewer.c min_y_of read the lowest vertex from each part's CPU
  skinning buffer (skinpos), or the unposed vertices when there was none.
- **Now:** fl_model_min_y (fl_model.c) skins the vertices on the CPU for the
  measurement itself, whatever the graphics backend does.
- **Why:** the nv2a backend skins on the GPU and keeps no skinpos, so the
  unposed vertices gave -104.6 instead of 0.3: on the Xbox the hunter floated
  about 1 m above the ground everywhere (plainly visible on the bed in the
  house, xemu 12 Oct). Same result as before on the PC (test_quest_loop OK).

### Memory card create on the Xbox: fopen "w+b" dropped (12 Oct 2026, commit 6a95cc3d)
- **Dropped:** mc_xbox.c sceMcOpen with the create flag opened a new file "w+b".
- **Now:** it makes the empty file with "wb", then opens it "r+b".
- **Why:** nxdk's pdclib maps "w+" to CreateFile(CREATE_ALWAYS |
  TRUNCATE_EXISTING), an invalid disposition, so every new save failed
  ("could not save" in xemu). A fix in xbox_libc.c's fopen wrapper would cover
  every caller; mc_xbox.c is the only "w+" user on the Xbox today.

### xemu screenshots: QMP screendump attempt dropped (11 Oct 2026, commit 44bfcddb)
- **Dropped:** tools/run_xemu.sh first asked xemu for a screenshot over its QMP
  socket, then fell back to grabbing the whole desktop.
- **Now:** xemu runs on its own hidden Xvfb display and the screenshot grabs
  that display. No window on the owner's screen.
- **Why:** xemu 0.8.136 has no screendump command, and grabbing the desktop
  meant xemu's window was on the owner's screen.

### Xbox movies: GPU YUV conversion dropped (11 Oct 2026, commit 7b611983)
- **Dropped:** gfx_nv2a.c kept one YUY2 texture per movie and let the NV2A
  convert YUV to RGB (`gfx_yuv_capable`, `gfx_create_texture_yuv`,
  `gfx_update_texture_yuv` in gfx.h and every backend).
- **Now:** the Xbox uses the same path as the PC: the decoder converts each
  frame to RGBA (sfd_rgba, integer BT.601) and it is uploaded as a normal
  texture.
- **Why:** on the real Xbox the movie colours were wrong (every hue flipped:
  green grass pink). Inverting the chroma bytes got close, but black areas
  came out teal, so the format was not understood. The CPU path is the one
  the PC already shows correctly.
- **Cost / to watch:** CPU time per movie frame (a 733 MHz CPU converting
  and swizzling ~0.3 Mpixel 30 times a second) and a texture allocation per
  frame. If the movie stutters, the GPU path is worth another look with a
  test pattern (revert 7b611983).

### Xbox network loader: polling for new builds dropped (11 Oct 2026, commit dcb0a0a3)
- **Dropped:** after the game exited, the loader stayed on screen and asked
  the laptop every 3 s for a new build, so builds could run with nobody at
  the pad.
- **Now:** the game exits to the dashboard; the next start (pad: A, A) sends
  the log and fetches the newest build. At most 3 network connections per
  start.
- **Why:** the Xbox's network froze after 5 polls (ping dead too), cause
  unknown; it then needed a power cycle each time.
- **To revisit:** likely cause found the same night: no program called
  nxNetShutdown() before XLaunchXBE, so the NIC kept running into the next
  program (the dumper then hung in nxNetInit right after the loader). The
  loader source now shuts the network down first; after the owner reinstalls
  it, polling could come back.

### nv2a: GPU memory freed at once, with a wait for the GPU (12 Oct 2026, agent E)
- **Dropped:** gfx_release_texture / gfx_release_clay waited for the GPU to go
  idle (`wait_idle`) and gave textures, palettes and vertex buffers back to
  the kernel immediately.
- **Now:** they go on a list and are freed 3 drawn frames later
  (`gpu_free_later`, `gpu_free_pending` in gfx_begin_frame); an allocation
  that fails frees the whole list first and tries again.
- **Why:** NV2A caches texels and the palette by address and has no known
  method to drop that cache; freed at once, the next screen's texture often
  landed on the address the last one had, in the same tick. Suspected cause of
  the owner's "screens flash as they fade in" on hardware (not visible in
  xemu, which re-reads memory). Also spares a GPU wait per release.
- **To watch:** peak memory at a stage change (old and new textures overlap
  until the next frames; the retry covers an allocation failure). Revert the
  commit that adds `gpu_free_later` to go back.

### Stage collision: 19 shit*.c / *_nm.c files replaced by one f_sphr.c (12 Oct 2026, agent C)
- **Dropped:** src/main/hit/shit1.c, shit2a.c, shit5.c, shit6.c, shit7.c, shit15.c,
  shit16.c, shit401.c (matched runs) and shit1_nm.c, shit3_nm.c, shit4_nm.c,
  shit8_nm.c ... shit14_nm.c, hitw_nm.c (near-match copies the PC linked).
- **Now:** src/main/hit/f_sphr.c is the whole original file (0x114AE0-0x11CDE4)
  as one translation unit, 25 of 34 functions matching C; the other 9 are
  kept as original bytes on the PS2 (config/c_rawfuncs.txt) and their
  near-match C (in the `#else` branches) is what the PC/Xbox build runs.
  GetWallHitBitPl, GetWallHitBit2, sphr_face_o3 and GetWallHitLine there are
  fresh rewrites from the asm (32-190 lines off), the rest the old copies.
- **Why:** one TU in address order with the statics is what the original
  compiler saw; it took 13 functions to matching C and replaced the PC's
  near-match collision code with the matched code (ground queries, wall
  init, floor slide, hosei_sub ...). Tests: quest_loop, progression, menu,
  name_entry, activities (38), save pass; Xbox build 0 failed.
- **To revert:** revert the commit that adds src/main/hit/f_sphr.c to config/c_files.txt.

## Release polish round 1 (agent C, 13 Oct 2026)
- **Dropped:** host stand-ins `STUB_V(vib_set_pl)` (rt_eft.c), `NOP(vib_stop_all)` (rt_flow.c), the generated no-op
  `flmatInit33`, and the generated no-op aliases `Equip_moji_color_rare_i`, `Put_PageArrow_s`.
- **Now:** vib_set_pl is the game's matched C (src/main/sys/vib.c, already linked, it was only hidden by the stub);
  vib_stop_all is a 3-line copy of the original in rt_flow.c; flmatInit33 is a C identity-3x3 in rt_flmat.c (keeps the w
  of each row like the VU0 vadd.xyz); the two `_i`/`_s` names are linker aliases (tools/build_pc.sh ALIASES) of the real
  chat_nm.c functions. flPADShockSet now stops the rumble on strength 0.
- **Also:** scripted/hidden/audio-dump runs never open or rumble a real controller (pad_set_headless); the old code
  opened any plugged-in pad in tests.
- **Why:** monster-caused rumble, Kut-Ku/Kirin joint reset, rarity colours and page arrows were missing.
- **To revert:** revert the commit "release polish: vib_set_pl, flmatInit33 ...".

### Lobby alias names resolved in every build, not only ONLINE=1 (13 Oct 2026, agent E)
- **Dropped:** `RT_GEN_LB_ALIASES=$ONLINE` in tools/build_pc.sh: the offline PC
  build (and the Xbox build, which reuses build/pc/rt_gen.c) left the lbtu3
  alias names of the lobby-client TU src/lobby/f/lb_cli.c (config/lobby_aliases.txt:
  `text_lobby_trans_ot3_o`, `pNet_c7`, `CallBack_X_o`, `ClassInfo_c43`, ...) as
  generated no-op stand-ins.
- **Now:** `RT_GEN_LB_ALIASES=1` always: each alias names the function or data at
  its address (gen_rt_auto.py), as the ONLINE=1 build already did.
- **Why:** the offline village draws its dialog layer through lb_cli.c too:
  Lbc_init_network_work puts `text_lobby_trans_ot3_o` into the ot2 prim, and that
  stand-in drew nothing, so DispDialogData (the bed's save prompt "これまでの
  ゲームデータを…保存しますか？", "ゲームを終了しますか？") was never shown. Owner's
  report from the real Xbox; the PC had it too. `pNet_c7` (read by
  text_lobby_trans_ot3) was a stand-in function whose code bytes were read as a
  pointer. Test: test_activities.py bed_prompt.
- **To revert:** set the line back to `RT_GEN_LB_ALIASES=$ONLINE` (the prompt
  disappears again).

### Village stage loads no longer start a stream from stage_bgm_set (13 Oct 2026, agent E)
- **Dropped:** rt_snd_stage (src/pc/rt/rt_snd.c) calling the game's stage_bgm_set on village stages; the start-up
  rt_snd_stage of a --boot run (src/pc/viewer.c); tools/test_audio.sh's "title" check (RT_NOMOVIE, sound from 21 s).
- **Now:** stage_bgm_set only for quest stages (the village's music is lobby.bin's own, as on the PS2); a --boot
  start-up loads no packs and plays nothing; test_audio boots with the opening movie and checks the logos are silent.
- **Why:** the owner heard the quest-clear music again in the village (stage_bgm_set picks S_CLEAR1 while
  Quest_clear_ck is 1) and the forest camp stream under the boot logos. The old title check only passed on that
  stray stream: the title screen starts no stream of its own.
- **To revert:** revert the commits "Boot: no forest camp stream under the logos" and "Village: no quest-clear
  music after a won quest".

### G PC: yn_tbl dropped from the hand list config/mhg/pc_names.txt (13 Oct 2026, agent B)
- **Dropped:** the hand entry `main yn_tbl 0x2EC178` in config/mhg/pc_names.txt.
- **Now:** tools/mhg_datamap.py finds the same address (medium: the gp load of MH1's mc_yn_ck lines
  up with G's lui/addiu in the reference sequence of the twin) and writes it to config/mhg/data_map.txt,
  which tools/mhg_pcmeta.py reads before the hand list. The other three hand entries stay (the tool
  does not find client_work / filedef_sys, and has no address for mc_sel_tbl).
- **Why:** one generated source of G data names instead of a hand list that grows per crash.
- **To revert:** put the line back into config/mhg/pc_names.txt (the hand list wins over data_map.txt).

### G hunts with G's own tables (14 Oct 2026, agent A)
- **Dropped:** the G raw `.inc` blocks of em_def_attack_set / em_def_defence_set (config/mhg/c_rawfuncs.txt);
  the PC's MH1 hit points for G (em_hp_vital_set/set2 with MH1's numbers, em_hp_vital_tbl); the D_ names with G
  addresses in pl53.c / pl73.c / pl77.c / pl_slash.c; MH1's remuneration_item_set on G; the matched MH1
  emu01.c / emu02.c / ems01.c / f_ud.c in the G PC build (tools/pc_g_exclude.txt); the 0x100-byte symbol size of
  G's stay/run-away timer lists.
- **Now:** matched G C for the two rate functions; game 0x5499A0 as C (near-match, PS2 keeps the bytes) and, on the
  PC, em_hp_vital_set(2) pick G's variant per kind (a design choice: the PC still runs MH1's em*_init, G's edited
  inits pass a variant number at the same places); named G tables (g_names.txt / pc_data.txt); G's reward list
  code (f_quest_nm.c); the near-match copies' G code (f_em_nm.c, ud_nm.c) runs; pc_data `list:N`.
  tools/mhg_pcmeta.py takes overlay words next to pointer words when they point near the neighbour's target or
  into main's gp area (34 words: em command and route lists).
- **Why:** the hunt was unfair or crashed: hp/attack/defence by MH1's rules, an invisible scale-0 Rathian, attack
  rate 0 (every hit 1), zero ammo tables, crashes when the hunter was hit or the Rathian changed area.
- **To revert:** revert the commit "G hunts: G's own tables ...".

### G quest sweep: G-only code paths replacing MH1 ones on the PC (14 Oct 2026, agent A)
- **Dropped / replaced (all under VERSION_MHG or G-only test aids; MH1 behaviour unchanged):**
  - f_pl.c search_act_set / item_action_set / tool wear: MH1's pickaxe and bug net ids 0x83-0x88 -> G's 112-117
    (PICKAXE0 / BUGNET0 macros; MH1 values unchanged).
  - em20_act_set (em20_nm.c) and em20 die00-02 (em20_ai_nm.c): MH1's code -> G's revival version (carcass
    1800 ticks, revival countdown, die act 3).
  - RT_PL_GOD on G keeps vital at 30000 (MH1 still 100): a G-rank hit took more than 100 in one tick.
  - func_0012D1B0 stand-in (always 0) -> reads game_w 0x2F4; func_0012D170 written (game_w 0x2F3 && 0x2C0).
  - mhg_pcmeta: scan pointer words inside em command programs (names em*_main_cmdNN / content_x / sub_contents_x)
    are dropped (one false word made kind 37's program loop); `noptr` pc_data objects; a pointer run that a
    pointer word points at is kept (em29's command list).
- **Why:** each was a G quest that hung, crashed or could not be cleared on the PC (tools/test_all_quests_g.sh).
- **To revert:** revert the agent-A commits "G sweep: ...", "G: em03/em04 program entries ...",
  "G em20 ...", "mhg_pcmeta: ..." on branch agent-A.

## nxdk's AC97 driver replaced by our own (agent F, 13 Oct 2026)
- **What:** src/pc/xbox/xbox_ac97.c defines XAudioInit/Play/Pause/ProvideSamples, so nxdk's hal/audio.c
  is no longer linked. **Why:** its callbacks waited on S/PDIF too and stalled for good (silent after a
  relaunch, silent in xemu); docs/xbox.md "Audio after a quit and relaunch". **To revert:** revert the commit
  "Xbox audio: own AC97 driver" (and drop the file from tools/build_xbox.py).

## G in English on the PC: quest hook, memory-card messages (agent E, 14 Oct 2026)
- **What:** (1) src/pc/viewer.c afs_entry: a G mission file (mNNNNN.mib) read through the AFS loader goes through
  rt_text_quest_g when a text table is active (the number is the file name's; the buffer gets 16 KB of room). (2)
  tools/text_map.py `linesplit_pairs`: the memory-card messages (mc_msg_NN) are paired per message and written as
  per-slot lines (`0xADDR[0] = text`, the pointer word of that slot, rt_text.c SP_SLOT), not per string, because the
  Japanese build shares one string between several messages. The US text is re-flowed over the JP lines of each paragraph
  (blank lines stay where the game puts them; 42 characters per line, then 46, 50; leftover slots get one space);
  a message is paired to the US group at the table's located address or, for an unlocated table, to the next US group
  with the same ~A00 / %s tokens. (3) rt_text.c: RT_TEXT_STAT=1 prints how many pointer words were re-pointed.
- **Why:** G's quest title/goal/failure/client text and the save / card dialogs (the first thing a player sees) stayed
  Japanese with the table. The US cuts these messages at other places and uses other line counts.
- **Left as it was:** line widths of the original layout code (the US lines fit the G boxes in the shots: 3-line card
  box, pause help box, quest pages); no wrapping was added in the PC layer. Tables with no US twin stay Japanese.
- **Not changed, seen on the way (not caused by the table, same without one):** the item window's title shows
  "(null)" (G title pointer not set), and the character-creation help box shows junk bytes.
- **To revert:** the hook is the `rt_text_quest_g` block in afs_entry; the tool changes are in tools/text_map.py only.

### G on the Xbox: memory fixes in shared runtime code (14 Oct 2026, agent B)
- **Dropped:** the G-only collision area sizes in rt_hit.c (256 / 512 KB, "not measured yet"); the whole-image
  .bss shadow as the only answer for a pointer into .bss with no sized symbol (rt_mem.c rt_bss_shadow); the
  worst-case output buffer of fmt_mwo_unpack (melt.c) kept for the life of the overlay.
- **Now:** one size for both games (96 / 192 KB: G's lwg parts have the same largest files); a shadow block for the
  gap between the named symbols around the address (whole block only when a gap is over 256 KB or a whole block
  exists already); the unpacked overlay is realloc'd to its real size.
- **Why:** G on the Xbox had 7 MB free in the village (MH1 16.5 MB); after these, 15.5-16.5 MB. MH1 PC tests
  (quest loop, progression, urgent) and the MH1 xemu quest loop pass.
- **To revert:** revert the commit "G on the Xbox: ...".

## 14 Oct 2026, agent C: G round 8
- **Dropped:** the "did not link" lines of config/mhg/c_skip_funcs.txt for src/main/pl/f_pl.c, cam/camd.c and game/f_game.c (stale since the
  parts' rodata lines were added by hand). **Now:** tools/mhg_dataparts.py writes the rodata lines itself, so mhg_parts.py can include those functions.
  **Why:** a redo of f_pl lost 3.6 KB of hand-curated parts otherwise. **To revert:** restore the lines from the previous commit.
- **Changed:** tools/mhg_parts.py now compares a part's functions as a set (not in symbol-table order) and splits runs against source order.
  **Why:** it dropped whole runs (flps0_p1, f_chat) as "bad". **To revert:** commit "G round 8" for tools/mhg_parts.py.
## Debug menu for testers (agent E, 14 Oct 2026)
- **What:** an in-game debug menu on every build (src/pc/dbgmenu.c, src/pc/rt/rt_dbg.c; docs/pc.md "Debug menu",
  docs/PLAYING.md "Debug menu for testers"). Choices made without the owner, each easy to change:
  - the combo **Back + R3** (the game uses neither for this; F10's settings menu is Back + L3) and **F9** on the PC;
  - **on by default**, off only in a build made with `MH1_RELEASE=1` (nothing builds that way yet: a release package
    script should set it); mh1pc.ini `debug_menu`, `--debug-menu`, RT_DEBUG_MENU override;
  - it draws with the game's own font (Japanese names as the game has them) rather than the F10 menu's 5x7 font, so it
    also works on the Xbox;
  - buttons held when it closes are kept from the game until released;
  - G's `item_str` imported at 523 entries (config/mhg/pc_data.txt; it was MH1's 327).
- **Refactors (no behaviour change, checked with test_quest_loop / test_menu):** rt_village.c `rt_village_leave` (the
  village exit that Local_main's end did inline) and `rt_world_reset` (the clears at the start of rt_village_enter);
  viewer.c `read_pad` replaces the three copies of "script or controller" in the tick loop; rt_player.c's RT_PL_GOD /
  RT_PL_WARP_EM blocks also run for the menu's switches; nothing dropped.
- **To revert:** revert the commits "Debug menu ..." on agent-E (four commits); test_dbgmenu.sh and dbg_keys.py go with them.

## G round 6: items, revival, stage tables (agent A, 14 Oct 2026)
- **Dropped:** the comment/decision that the four 92-stage tables stay at MH1 size (config/mhg/pc_data.txt) and quest
  10030 from the G sweep's KNOWN list. **Now:** the tables at G size with the new `allptr` pc_data flag
  (tools/mhg_pcmeta.py) so their sub_main-bss pointers are converted; 10030 passes (quest command 0x2F added).
- **Choice:** G's item code (item_action_set, Basic_item_set, Fue_item_set and the item acts) is written as
  VERSION_MHG blocks beside MH1's in the same functions, not as separate files; the em01/em15/em17 G revival uses a
  new shared header include/em_revive_g.h (em20 keeps its own copy from round 5; folding it in is a possible tidy-up).
- **Choice:** G's food-bonus random call (ran_suu with a pointer as the stream, see FINDS) uses its own LCG state on the
  PC instead of copying the stray write.
- **Not done:** G's em07 (Lao-Shan) main/init/quest event demo; shell03 (thrown items) G kinds 11-15 run MH1's C;
  eft12 G edits (smoke colours of G's kind 6).
- **To revert:** the commits "G PC: item use ported from G ...", "G quest program command 0x2F ...", "G PC: the
  92-stage tables at G size ..." on agent-A.

## G village round 2 (agent D, 14 Oct 2026)
- **Dropped:** the GTAG shop-tag stand-in of 6e6fc6da (every G shop used `shop_default_tag_00610A68 + 1`, the item shop's buy / sell
  pair, as its tab names; GTAG macros in lb_arm, lb_sh1a, lb_sh1c, lb_by47/49/54/129). **Now:** each shop uses G's own table: the armour
  shop's weapon / armour tabs `shop_armor01_tag` = main 0x2EF040, the workshop's `shop_process00_tag` = main 0x2EF050 (config/mhg/pc_data.txt;
  their pointers into lobby.bin come from config/mhg/pc_main_lobby.txt, which tools/mhg_main_lobby.py now extends by hand-found ranges),
  `shop_armor00_tag` = lobby 0x636A5C (pc_names.txt), `shop_process01_tag` is G's own symbol. **Why:** the stand-in gave the wrong tab names
  (買う / 売る on the armour shop's weapon / armour tabs). **To revert:** the commit "G village: weapon/armour shop list works on the PC".
- **Choice (guess):** G's story event 1 (the elder's first talk, src/lobby/lb/lb_talk.c Lb_event_story) also sets event flag 0. G's asm sets
  flag 0 only in event 0 (a kind-0 NPC of the online town, +1500 zenny), and the elder's mode test (get_questLevelNum(36) = event flag 0) would
  repeat the greeting for ever offline and never open the counter. Remove the `Event_flag_set(0)` line to follow the asm exactly.
- **Choice:** G's story talk modes keep MH1's mode numbers on the PC (G n = MH1 n-1 for 4..17, n-2 from 19): G's story event (49) runs as 47, its
  banner mode 48 (the hunter-registration banner, 90 ticks) is shortened to what it does at the end (event flag 4, message 15). G's second counter
  page (mode 3) is mode 2 with the counter work's page byte 2.
- **Dropped:** tools/pc_lobby_matched.txt's lb_r.c (MH1's lb_basic_master) for G (tools/pc_lobby_g_exclude.txt); **now** lb_village_nm.c's lb_basic_master
  has G's kind dispatch (story modes, G shop modes). MH1 unchanged. **To revert:** the commit "the elder's story talk".
- **Not ported (shown wrong or missing on the PC):** the equipment compare / description windows of G (main equip_exp_core, 4.7 KB, EquipmentCompareWindowA):
  the shops' weapon detail shows attack 0 and a mis-sized sharpness bar; the G "worn piece next to the new one" detail mode (x1C 3, lobby 0x554010) and the
  skill pages; the item shop's page ornaments; the farm and the cat kitchen do not exist in G's Kokoto (no spot / NPC for them in stage 87; Lb_eat is a
  chair of the online guild hall).
- **Tools:** tools/gvil_ev.py (G village run from a tick-event pad script, GDB=1 for a backtrace), tools/gpeek.py (words / bytes of G's data by address),
  tools/test_gvillage.py (all the village cases below, about 10 min), RT_POUCH=id:n,.. (village test aid), the village trace now prints the event flags.

## G in English, round 3 (agent E, 15 Oct 2026)
- **Dropped:** (1) the "still Japanese" test of tools/text_map.py counted every double-byte pair, so US lines with full-width blanks, colons or
  Latin letters were thrown away; **now** only kana (0x82 0x9F.., 0x83) and kanji (0x88..) count. (2) the US pairing of the MH1 table
  `plaza_main_msg` (its rows are shifted against the US table: three of the rows checked by eye got the English of a neighbour); **now** no English
  for it (`SHIFTED_US_TABLES`). (3) the position-paired tutorial titles (buki_str, tuto07_msg: the US has a dual-sword page and a bug-collecting
  block the JP lacks, every title after them was shifted by one); **now** hand-checked pairs in tools/text_pairs_manual.txt.
  **To revert:** the commit "G in English round 3".
- **Added, not a replacement:** tools/text_pairs_manual.txt (addresses only, read by text_map.manual_pairs: `jp` / `g` / `gf` / `drop` lines): 300
  hand-checked G -> US pairs for the offline village UI the structure could not reach (shop, forge, item box, action menu, pause menu pages,
  status, equipment, quest result and HUD messages, tutorial titles), found by aligning G's and the US build's string pools (the US lobby and
  sub_main hold the offline village UI of a later source than the JP disc, which is G's lineage). `propagate_same_text_g`: a G string no table
  paired takes the English of the same Japanese text elsewhere (+1076 strings).
- **Changed in shared runtime code:** src/pc/rt/rt_font.c flfntPrintf looks its format up in the text layer (rt_text_tr), like the other font entry
  points: G's pause menu passes page titles as the format itself, they stayed Japanese.
- **Seen, not changed:** the G status page's page counter printed garbage (the code passes two numbers, the US line has the page count built in;
  the English line shows 1/2); in G's quest-info pages the English labels are wider than the Japanese ones and touch the numbers, whose columns
  are placed for Japanese text; the talk window's title and other G-only titles have no US text.

## G round 9 (agent C, 15 Oct 2026)
- **Dropped:** raw `F8(&PitMenu, 0x1C)` / `FS8(&PitMenu, 0x1C)` accesses in f_chat.c and tu/chat_all.c (a wrong offset for G: its halt flag is behind the log);
  **now** `PIT_HALT` (include/menu.h; MH1 still byte 0x1C, named `x1C`). **To revert:** the commit "G: yn work struct".
- **Dropped:** the `(u16)quest_w.x1D4[..]` casts and the s16 type of quest_w.x16C / x1D4 (G reads them with lhu); **now** u16 fields. MH1 code unchanged
  (these fields are only used by the G block of Quest_enemy_die / Quest_start).
- **Dropped:** in the yn screens the literal `M2C_FIELD(ynw, ..)` offsets; **now** `YNF()` / `YNO()` (no-ops in MH1).

## Settings menu on the Xbox (agent F, 15 Oct 2026)
- **Dropped:** src/pc/menu.h's Xbox stubs (no settings menu on the Xbox); **now** menu.c, gfx_opts.c and the new font5x7.c are built
  for the Xbox too (tools/build_xbox.py FRONT), with the console's rows only. **To revert:** the commit "Xbox: the settings menu".
- **Dropped:** viewer.c's Xbox scene aspect `W / H` (always 4:3); **now** gfx_scene_aspect() on every backend (gfx_nv2a.c: 16:9 when
  widescreen; gfx_null.c 4:3).
- **Moved:** the 5x7 font table from pick.c (pick_font_rows) to src/pc/font5x7.c (font5x7_rows), so the Xbox menu has it without the
  PC-only bug reporter. Same glyphs.
- **Choice:** Xbox widescreen is 16:9 anamorphic in the 640 x 480 mode (no 720p); with no `widescreen` line in settings.ini the
  dashboard's 16:9 flag decides (a player with a widescreen dashboard gets 16:9 on the first start). If the owner prefers 4:3 always
  until chosen, drop the XVideoGetEncoderSettings line in gfx_opts_load.
- **Choice:** the settings combo is also taken from each game tick's pad (menu_tick_pad), so the combo's buttons never reach the
  game and scripted runs can open it; buttons held at close are hidden from the game until released (before, cross on "Close"
  reached the game).

## G on the PC for players (agent D, 15 Oct 2026, GB3)
- **Dropped:** the `./mh1 linux|windows` refusal of a G disc ("Xbox only for now"); **now** `./mh1 linux|windows|xbox DISC` detects the
  game from the disc (tools/disc_files.py) and builds G into dist/linux/MHG, dist/windows/MHG, dist/xbox/MHG. **To revert:** the commit "G: mh1
  builds the G disc for Linux and Windows".
- **Choice:** the packaged play.sh / play.bat for G set MH1_SHOW=1 so a player gets a window; the program's own hidden default for G
  (viewer.c) is unchanged, so agent and test runs stay hidden. Win G is tools/build_win.sh VERSION=mhg (tree build/winmhg) packaged by
  VERSION=mhg tools/package_win.sh (mhgpc-win32-*.zip, play_g.bat -> play.bat).
- **Dropped:** src/pc/install.c's MH1-only constants; **now** one table per game (VERSION_MHG: G's four files, SLPM_658.69, data folder
  mhgpc/data), so dragging a G ISO onto mhview_g.exe installs it.
- **Dropped:** Lb_put_shopYesNo's MH1 two-string version in the G build (lb_by44.c); **now** G's message table D_2EC5D8 by the answer state.

## G GAME MENU rows, quest clear bits (agent C, 15 Oct 2026, GB2 / GB6)
- **Dropped:** on the G PC build, MH1's omake menu (omake.c `Omake_task`, `omake_main`, `omake_play`, `omake_exit`, `disp_omake_menu`, `omake_check`: now `#if OMAKE_PART == 0 &&
  !defined(VERSION_MHG)`) which the GAME MENU's 5th row ran and crashed in (zero omake_str); **now** src/main/omake/omake_g.c: `Omake_task` is G's NETWORK OPTIONS
  (main 0x1180F0, the name G's symbol view gives it) and `sub_main_func_004A9960` is the GALLERY. MH1 and the PS2 builds are unchanged. **To revert:** the commit "G: NETWORK OPTIONS
  and GALLERY screens on the PC".
- **Choice:** the NETWORK OPTIONS rows "delete the login ID" and "network settings" (PS2 network stack, yn_scecom_reboot) answer with the "can't" sound on the PC; the 15
  gallery event scenes (G 0x4A91D0) do the same; the credits run on black (G plays movie clips behind them). The gallery does not wipe User_data as G does at its start
  (G's scene viewer plays on a blank hunter; the port has no scene viewer and the card keeps the save).
- **Dropped:** Quest_clear_bit_set / _ck's quest-number-as-bit-number on the G PC (ud_nm.c); **now** G's list of quest numbers (quest_bit_tbl, G sub_main 0x4FECA0) gives the
  bit, as G 0x4C74D0 / 0x4C7550. MH1 unchanged. **To revert:** drop the `#ifdef VERSION_MHG` block in ud_nm.c.
- **Dropped:** Omake_flag_set / _ck's MH1 s16 at option_w+0xFCC on the G PC (ud_nm.c is the copy the G PC build links); **now** G's u32 at +0x2C4C.
- **Dropped:** MH1's Disp_button on the G PC (disp2_nm.c); **now** G's (the O / X prompts and L1 / R1 icons come from G's sheet; Disp_button_col takes the colour).
- **Added, not a replacement:** `RT_OMAKE=0x7FF` (test aid: opens omake flags), the gallery pictures in build/C/gb2/.

## G monster AI, em04 sounds, shell attacks (agent A, 15 Oct 2026, GN6 / GN15 / GM2)
- **Dropped:** on the G PC build, MH1's em14/em17 move choice outside mode 3 (em14_to_normal / em17_to_normal / act_dist_select / em_act01
  searched em14_act_add / em17_act_add / em17_rail_* tables, zero on the G PC); **now** G's versions (to_normal = act_dist_select, mode 3 only;
  em_act01 waits for the motion and calls it). Plus G's em14_main / em17_main / em08_main reactions (em08_main is a whole G copy). MH1 unchanged.
  **To revert:** the commits "G: em14/em17 choose acts with G's code (GN6)" and "G: em08_main (Plesioth...".
- **Dropped:** the stand-in note `sub_main_func_004ECD00 (G em04 sound script: MH1's)` in rt_quest.c; **now** ef_move_sub_0058E500 (em04_nm.c)
  is G's script on the G build: kind 32's own sound in motion 0x3F7, and the Eft13 dust only when func_001F19C0(2) == 2 (G's overlay-slot
  byte; only the gallery event scenes load overlay 2 into slot 2, so G's quests show no em04 dust and neither does the port now).
  **Choice:** func_001F19C0 stays a stand-in (returns 0) on the PC; if someone implements it, dust follows G automatically.
- **Dropped:** shell11_i's MH1 `arg + 1` attack entry on the G PC; **now** G's `arg`, with the callers (em12 Cephalos, em19 Vespoid/Hornetaur)
  passing G's kind numbers (SH11 macro; em19 14/15 unchanged as in G). Kind 40's swing now does damage. **To revert:** the commit "G: em04 sound/dust
  scripts, part durability level bonus...".
- **Dropped:** MH1's poison_stock_set on the G PC; **now** G's (no poison build-up past tolerance - 1 while poisoned, stock emptied when poison starts).
- **Added:** part durability level bonus (G em_dur_init / em_dur_set, EMW 0x7D4 carved as dur_lv_bits in include/em.h), point_cam_sub case 25
  (demo wall-clip flag), shell08_i kinds 8 / 12.

## G quest result module (agent A, 15 Oct 2026, GN17 / GN11 / GN13)
- **Dropped:** on the G PC build, MH1's f_reward.c / f_reward2.c / f_reward3.c (result_prog, the money and point screens, movie_add_ck with
  MH1's zero movie table, result_disp) and the G block result_hunt_records_g in f_reward.c; **now** src/main/reward/f_reward_g.c, G's
  whole module from sub_main 0x4F3A50-0x4F7FDC: 20 steps (reward list, money, sale of the five G quest items, online points, record /
  training / gallery movie / gallery title announcements, error). tools/build_pc.sh swaps the files for VERSION=mhg; MH1 and the PS2
  builds are unchanged. **To revert:** the commit "G: the quest result module as G (GN17)".
- **Dropped:** func_0012D170 / func_0012D1B0 in src/pc/rt/rt_quest.c read game_w+0x2F3 / +0x2C0 / +0x2F4 (past the host's game_w) and
  tested the second byte the wrong way; **now** system_w+0x43 / +0x10 / +0x44 as G's asm (1 = 0x43 set, network flag 0x10 clear). In test
  runs the old code returned 1 (the pouch went back to its departure state after every quest, no item sale). It also feeds Get_equip_value,
  Pl_item_charge, the quest judge and the reward list: check those with the owner if anything changes there.
- **Choice:** G's reward list draws a frozen copy of the last game frame behind it (main 0x12EAF0, a GS copy); the PC draws the plain backdrop.
  G 0x12AE80 (a random number saved to User_data+0xD0A after each quest) is not ported: no reader found and the PC's Rnd_w has two channels.
- **Added:** Set_userdata (udmisc_nm.c, G) keeps the pouch at departure (G 0x32C1C0) for the special ends 7 / 8; func_00126320
  (menu_disp_nm.c, a hunting-log crown by row) for the record screen.

## G main lane round 2 (agent C, 15 Oct 2026): status pages, equipment windows, camera, timers
- **Dropped:** on the G PC build, MH1's PlayerStatusWindow / PlayerEquipmentWindow (chat_nm.c, now `#ifndef VERSION_MHG`) and MH1's two-page Menu_status_i / _mv (menu08.c);
  **now** src/main/menu/status_g.c (G main 0x222E50 and the skill-page unit 0x223610-0x224410) and menu08.c's G branch. MH1 unchanged. **To revert:** the commit "G PC: the pause menu
  STATUS pages as G has them".
- **Dropped:** MH1's equipment windows on the G PC (chat_nm.c EquipmentDescriptionWindow(A), equip_exp_core, sword_zokusei, EquipmentCompareWindow(A), slash_level_bar, Put_PageArrow,
  now `#ifndef VERSION_MHG`), the pause menu's MH1 menu_equip_get_equip (User_data offsets) in Pit_disp_menu_equipment / Menu_equipment_mv; **now** src/main/menu/equip_g.c plus the G branches in
  menu09.c / menu26.c / menu41.c. **To revert:** the commit "G PC: G's equipment windows ...".
- **Dropped:** HitWallPlayer's `scl[0..2]` for the wall spheres on the G PC; **now** G's: the base scale when the size is 60-200 %. **To revert:** the f_sphr.c hunk of that commit.
- **Dropped:** MH1's cam_sub_std on the G PC (cam_nm.c `#ifndef VERSION_MHG`); **now** G's (see RELEASE_G.md section 6). **To revert:** the commit "G PC: the follow camera ...".
- **Dropped:** MH1's life-bar recovery / stamina rules in timer_calc_sub_pl and the spot-depletion rule of St_pick_ck2 on the G PC; **now** G's (pl_nm.c, plx10.c, `#ifdef VERSION_MHG`). Not verified
  in a run. **To revert:** the commit "G PC: timers and gathering ...".
- **Added:** config/mhg/pc_data.txt G tables (gstat_*, gskill_*, gequip_*, gslash_rows; weapon_exp / armor_exp / slash_bar_color / stage_camera_data_ex at G's size). gslash_rows is `noptr`: the
  pointer scan took the sharpness lengths 0x50, 0x6E (= 0x006E0050) for a pointer and the relocation overwrote them.

## G village, second pass (agent D, 15 Oct 2026)
- **Dropped:** G's Lb_put_shopYesNo read its table through `D_2EC5D8` and G's func_0012BDD0 (lb_by44.c); **now** `yes_or_no` + font_print_sp. **Why:** the PC
  reads the hex of a D_ name as an MH1 address, so the yes / no line printed nothing. **Revert:** the commit "G village: yes/no line, fee text, banner wait".
- **Dropped:** the sprintf with a made-up format address in lb_guild_g.c lb_guild_talk (fee number in the elder's sentence); **now** Lb_num_to_str(guildPrice, p) as G.
- **Dropped:** for G the box's equip-change called Lb_equip_set (MH1 only; G removed it); **now** G's two steps (armor_set_myArmor, then Skill_set_PL).
- **Changed (G):** the story banner (mode 48) now waits its 90 ticks and then the "close" button (key 0x40), as G; before, it ended at once.

## PS2 online patch (agent B, 15 Oct 2026)
- **Dropped:** rt_online_config's own fill of the portal's work (bsCsvWork password / server / user counts, ConnectLbsId,
  BsLbsInfo, BsLbsCount, D_3A3B71) in src/pc/rt/rt_online.c; **now** mh1p_fill_server_table (src/ps2patch/online.c), the same
  code the PS2 online patch (tools/ps2_patch.py) puts into lobby.bin's internet_browser. **Why:** one copy, and the PC online
  tests exercise the PS2 patch's server table. Small differences: BsLbsInfo's name is copied with 0x20 bytes (was 0x40; the
  name is "Local test server"). **To revert:** the commit "PS2 online patch: ...".

## G main lane round 3 (agent C, 15 Oct 2026): hunter moves and hits, camera ground test, save slots
- **Dropped:** on the G PC build, the stand-in `gcam_lift` that eased the camera's eye lift (x2C) back to 0 for ever (the ground line test was missing); **now** G's func_0010AE00 as C in
  src/main/hit/f_sphr.c (`#if VERSION_MHG && !__MWERKS__`, G only, the PS2 build keeps the original bytes) and func_001DAE60 in cam_nm.c. **To revert:** the cam_nm.c / f_sphr.c hunks of
  the commit "G PC: the hunter's moves, body hits, camera ground test, save slots".
- **Dropped:** MH1's per-push-sphere loop in k_HitEmCamera (camr5_nm.c) on the G PC; **now** G's: only push01[0], body entries flagged 4 skipped. MH1 unchanged.
- **Dropped:** MH1's disp_savesel / _waku / _moji reading option_w with the 0x480 slot stride on the G PC (mcdisp_nm.c `#else` branch); **now** G's 0xE00 stride, G's strings (gsave_fmt) and the
  4th / 5th arguments. The buffer mode of G (slots drawn from the card image in data_load_ptr) is not done: the PC keeps the slots in option_w.
- **Dropped:** Default_reibun_set's MH1 offset 0xDB0 on the G PC (chat_nm.c): it wrote the chat phrases into G's second save slot; **now** +0x2A30.
- **Dropped:** game_w+0xD1 reads in pl_move_sub (net position slot) and f_em_nm.c's em init (EB(0x88E)) on the G PC (the master byte lives at +0x3A there); **now** `game_w.master`.
- **Changed:** rt_village.c called Set_userdata for the village hunter only with RT_POUCH; **now** always on the first visit (the name and equipment of the saved hunter, as init_pl_work does), and a
  test hunter without a name gets RT_NAME.

## One network loader per game (agent D, 16 Oct 2026)
- **Dropped:** the G data install writing E:\Games\MHG\default.xbe (G's game) and the loader then starting it
  ("Starting E:\Games\MHG\default.xbe in 5 s"); install_data's E:\Games\<folder from the list> target. **Now** that name is G's
  loader; G is mhg.xbe, served as /mhg.xbe and downloaded by the usual build check; the install writes into the loader's own
  folder and only for its game. **Why:** owner's request (each game updates itself). **To revert:** the commit
  "Xbox: one network loader per game".
- **Dropped:** xbox_serve.py's single build (default build/xbox/default.xbe, `/mh1.xbe` only) and single --data folder. **Now**
  both games at once, defaults dist/xbox/MH1/default.xbe and dist/xbox/MHG/default.xbe (--xbe / --xbe-g), --data once per
  game. Logs are named <time>-<game>.txt.
- **Changed:** leaving the game on the Xbox went straight to the dashboard (since 11 Oct); **now** it starts the loader in the
  game's folder with launch data "MH1X" (as before 11 Oct), which sends the log and then goes to the dashboard. An XBE that
  is itself default.xbe still goes to the dashboard.
- **Changed:** rt_log's session-log folder on the Xbox was E:\Games\MH1\logs for G too; now the game's folder (XBOX_GAME_DIR).

## G Xbox bugs GB5 (agent A, 16 Oct 2026)
- **Dropped:** rt_player_game_init writing its default weapon (156 / RT_WEAPON) into User_data at every quest start; **now** the
  save's equipped weapon is used when RT_WEAPON is unset (the default only without a save). **Why:** bought / box / debug
  weapons never reached a quest (GB5 bug 5). **To revert:** the rt_player.c hunk of the commit "G Xbox bugs GB5".
- **Dropped:** MH1's Lb_reset body for G offline (push_em_work_all, stage_free, clr_move_work, prim_init); **now** G's early
  return (lobby 0x59CCA0). **Why:** villagers vanished after the house (bug 3). lb_z133.c.
- **Dropped:** create + release of a texture per movie frame; **now** three textures updated in place (gfx_update_texture).
- **Changed:** the debug menu restores the depth test and fade colour after drawing; its jumps out of the village run
  Pit_reset + Lbc_set_prim(0,0,0); its title row stops the sound; viewer.c draws black in mode 6 before the village.

## Bug reporter on the Xbox (agent E, 16 Oct 2026)
- **Dropped:** pick.h's empty Xbox stand-ins (the reporter compiled out of the Xbox build). **Now** pick.c is built for
  the Xbox (Back + Black, pad pointer, on-screen keyboard, USB keyboard / mouse, nv2a id pass, reports sent through the
  loader; docs/xbox.md "Bug reporter on the Xbox"). **Why:** owner's request. **To revert:** the commit "Xbox: bug reporter".
- **Changed:** on the PC, a freeze made with the controller (Back/View + Start) now also takes the pad (pointer, A, the
  on-screen keyboard) and draws the panel at scale 2 with the controller's keys; an F8 freeze is unchanged.
- **Changed:** gfx_nv2a.c gfx_execute_clay checks gfx_rec_clay once at the top for both paths (execute_cpu did it for the
  CPU path, the skinned path after its own check): same behaviour, one place.

## Small fixes (agent B, 16 Oct 2026)
- **Changed:** run_xemu.sh / run_pcsx2.sh stopped the emulator only on a normal end; **now** tools/emu_lib.sh (new) stops
  the emulator's whole process tree and its Xvfb on every exit, also when the script is killed (TERM/INT/HUP) or fails.
  **Dropped:** the `pgrep -x xemu` / `pgrep -x AppRun` guards (never matched xemu; AppRun also matched PCSX2) for
  xemu_running (pgrep -f on the AppImage file name); test_three_way's and test_ps2_online's SLP (they killed run_pcsx2's
  sleep child to end it; they now kill run_pcsx2.sh itself). **To revert:** the commit "Emulator scripts stop xemu/PCSX2".
- **Dropped:** for G, MH1's disp_mc / disp_check / disp_save_info strings (select.bin literal addresses, garbage in G);
  **now** G's code (sub_main table D_5284A0, centred). g_edit_msg's rt_addr of the table words (they are host pointers)
  for rt_ptr_at. **Why:** garbage line after NEW GAME's save, empty edit help lines. **To revert:** the commit "G:
  edit/continue screen messages".
- **Dropped:** quest_back for Local_main's -1 (the bed's "quit the game" restarted the last quest); **now** village_quit:
  the game menu (Select_task) as Game_task's case -1 does. quest_back stays for RT_NO_VILLAGE and the online town.
  **To revert:** the commit "leaving the village without a quest".
- **Changed:** our server's plaza / lobby names start with a full-width space; the lobby join counts are (all, in rooms)
  clamped to 0..9 instead of (count, 100). tools/test_online.sh's plaza-name grep follows.

## G release checklist, fourth pass (agent A, 16 Oct 2026)
- **Dropped:** mhg_pcmeta's rule "every `.word SYMBOL` of the split is a pointer" for names D_xxxx below 0x100000
  (78 words of pc_ptrs.txt). **Why:** 0x0000FFFF list ends were zeroed (title strips). **To revert:** commit "G PC:
  title strips ...".
- **Dropped:** rt_font's empty gap for G's ~Bnn; **now** the icon is drawn (PutButtonICON).
- **Dropped:** for G, the plain reward-list backdrop; **now** the quest clear's frame copy (rt_2d.c rt_cap_*).
- **Dropped:** for the G PC build, MH1's Pl_damage_sub / pl_guard_set C (kept for MH1); **now** G's (pl_damageb.c).
  **Why:** G's armour skills on hits. **To revert:** the commit "G PC: hunter damage skills, trans_stage".
- **Changed:** trans_stage.c G branches (G_ONLY / MH1_ONLY labels); MH1 unchanged.

## G reference shots, fog, Dual Swords sounds (agent A, 16 Oct 2026)
- **Changed:** GFX_RS_FOG_ENABLE used to switch GL fog on for everything drawn after it (nothing ever set it, so fog was
  off); **now** it is the scene's fog switch and each clay decides (gfx_clay_desc.fog from its AMO attribute +0x20, as
  fl's per-clay RenderState 0x8000): GL_FOG (and the nv2a fog constants) only around those clays. rt_game_init calls the
  game's stage_fog_set; viewer.c turns the scene fog on after gfx_begin_frame and off before the 2D layers (rt_fog_scene).
  **Why:** the real G (PCSX2 reference shots) has stage fog, the PC had none (MH1 too). RT_NOFOG=1 turns it off.
  **To revert:** the commit "G PC: stage fog".
- **Changed:** config/mhg/pc_data.txt imports Battle_type at G's size (0xE1, was MH1's 0x7C): G weapons with a type
  from 0x7C on got their job from whatever followed the host copy. **Stand-in added:** rt_data.c turns job 6 (Dual
  Swords) into job 1 (sword and shield) until the job-6 tables and moves are ported (pl_init crashed in
  Pl_slash_lv_ck with job 6); RT_G_DUAL=1 keeps job 6.
- **Dropped:** for G, MH1's NextStage_Dir_Set (kept for MH1); **now** G's (game 0x58F820, em_cmd_nm.c).
- **Added:** G's Dual Swords case in ef_move_sub (pl_snd01.c, job 6; unreachable while the stand-in above is on) and
  G's anim-801 Eft06 call. viewer.c rejects --stage outside 0-87 (88/89 crashed in load_stage_hit).
- **Dropped:** for the G PC build, MH1's eft09_i / eft09_m / eft09_set behaviour (kept for MH1); **now** G's (subspecies
  kinds, revival ends the cut tail, ot0 when fading). **To revert:** the commit "G PC: stage fog ...".

## G Dual Swords (agent A, 16 Oct 2026)
- **Dropped:** the stand-in in rt_data.c that turned every Dual Swords weapon (job 6) into a sword and shield (job 1),
  and its RT_G_DUAL=1 switch. **Now** job 6 plays as G's Dual Swords: G's attack handlers (pl_at083..pl_at102, actions
  0x53-0x66), ds_atk_set, the job-6 cases of basic_atack_ck / ex_atk_ck / job_special_com_ck (R1 = demon mode), the
  job-indexed tables at G size, shell00_set2 (the left blade's hits), the demon mode in shell00_i, the second blade in
  rt_player_weapon, G's eft05 (two trails, the demon mode's red trails) and the demon-mode glow (eft_ds_aura_*).
  **Why:** the real class instead of a stand-in (checked against the real G in PCSX2: build/A/dual/cmp_*.png).
  **To revert:** the commit "G: the Dual Swords".
- **Dropped:** for the G PC build, MH1's eft05_i / eft05_m / eft05_d / eft05_t / Eft05_set (kept for MH1); **now** G's
  (eft05_nm.c VERSION_MHG). **Changed:** rt_weapon_afs takes G's 225 weapon models (was 124: every G weapon with a model
  from 124 on had no model on the PC, any class).

## G picture match and quest lane (agent A, 16 Oct 2026)
- **Added:** stage_spr_disp is called by the PC's rt_game_draw (MH1: game modes 2/3 as trans(); G: always, its own
  tests). **Why:** the PS2 draws this full-screen colour gradient every frame; without it every G outdoor stage was too
  bright / untinted. RT_NO_STAGE_SPR=1 leaves it out. **Changed:** the 3D clear colour is black (was a fixed blue).
- **Dropped:** for G, MH1's stage_set_set, set19_i / set19_trans (G raw bytes), set06_i / set06_m / set06_trans,
  stage_fog_set, eft13_t, em16_to_normal, em08 em_move04 / em_dmg05 behaviour (kept for MH1); **now** G's.
  set19_i / set19_trans left config/mhg/c_rawfuncs.txt (now C that matches G).
- **Changed:** rt_bind_set_model with no set model unbinds the old clays (was: kept pointers to freed clays).
  tools/test_activities.py map_item: green-pixel threshold 200 -> 150 (the gradient darkens what is behind the map).
- **To revert:** the commits "G picture match: stage screen gradient ..." and "G: stage environment block, set13 kind 8 ...".

## Lockstep referee (agent D, 18 Oct 2026)
Found by tools/referee.py against the real game in PCSX2 (docs/referee.md); every change re-checked the same way.
Both games unless marked.
- **Dropped:** the host's monster order 1..19 then 0 (viewer.c sim_tick); **now** 0..19 as move(). **Why:** random draws
  in the PS2's order (small monsters placed as on the PS2).
- **Dropped:** sim_tick's order (joints + hit_check before the monsters, body_hit after the effects); **now** player,
  monsters, body_hit, effects / sets / light, area exit, BGM, HUD, then joints + hit_check (move(), trans(),
  hit_check()). **Why:** monster attacks hit a tick late.
- **Dropped:** rt_quest_load starting game2 at once (mode 2); **now** mode 1 step 3 (game13): rt_flow.c runs one
  game_core and then spawns the stage's sets (rt_game_quest_sets; rt_game_init defers stage_set_set while game13 is
  pending), then mode 2. In a run that starts in a quest (`--quest`) the game13 tick waits for the host's first two
  ticks. **Why:** the PS2's game13; the world was three ticks behind at every quest start. Tests that count ticks from
  a quest start see the hunt start one tick later.
- **Added:** rt_sys_frame_start (the Scheduler's ran_suu(1) each frame after the boot); rounding of the stick angle
  (rt_pad.c); G: em_body_push_tbl (body_hit_sub_em's own table, pl_nm.c) and em_dur_rows noptr (pc_data.txt).
- **Added:** RT_FPU_PS2 (rt_ref.c): SSE rounding toward zero + no denormals, as the EE FPU. On by default since
  20 Oct 2026 (owner's decision, next section).
- **To revert:** the commits "referee: ..." on agent-D.

## G Xbox bugs 11-14 (agent A, 17 Oct 2026)
- **Dropped:** write_png's stdio writes (fopen/fwrite of each chunk) and pick.c write_text's fwrite; **now** the file is
  built in memory and written by viewer_write_file (Xbox: one CreateFile/WriteFile in xbox_report.c xbox_write_file).
  **Why:** pdclib's fwrite copies byte by byte and writes 1 KB at a time: a report took about 80 s on the owner's Xbox.
- **Changed:** the reporter's Enter / Start / keyboard Enter no longer save at once: one frozen frame with
  "SAVING THE BUG REPORT..." first (save_pending), input ignored meanwhile. end_session restores the game's texture.
- **Changed:** weapon_follow (the host's weapon model follows PLW+0x34C) runs in the offline village too, not only the
  online town; its trace line says "village:" or "online:".
- **Changed:** G vs_square_init's lobby_bgm_set gets the stage (was called without argument: house music bug);
  lb_village_nm.c prototypes lobby_bgm_set(int).
- **Changed:** stage_spr_disp is not drawn in the reporter's id pass.
- **To revert:** the commit "G GB5 bugs 11-14: weapon model after equip, house music, report save, reporter texture".

## Cross-game audit (agent A, 18 Oct 2026; docs/audit_cross_game.md)
- **Dropped:** the "can't faint" values split by game and caller (RT_PL_GOD on G: vital 30000 after pl_move; RT_PL_GOD on
  MH1 and the debug menu on both: 100 after pl_move). **Now:** both games, aid and menu alike: 30000 just before pl_move
  (the hit of the last hit_check is taken there), the hunter's maximum (PLW+0x792) after it. **Why:** with 100 a hit over
  100 in one tick fainted the hunter (G quest 9's Rathian; the menu on G high rank), an MH1 hunter with 150 max hp was
  cut to 100, and G's 30000 drew the life bar off the screen.
- **Dropped:** G stubs of the debug menu's hunter rank / mark-all-cleared (rows hidden on G). **Now:** G rank at
  User_data+0xC23, G clear-all over quest_bit_tbl's 98 offline quests. **Why:** the rows were MH1-only for no reason
  but the unknown G offsets.
- **Changed:** rt_player_game_init's "has the save a pouch" test reads G's pouch (+0xC24) on G, not MH1's +0x37C.
- **Changed:** tools/test_all_quests.py: the MH1 full sweep now also runs the carts check and one time-out run (quest 139);
  the G time-out check is the shared `timeout_check`.
- **To revert:** the commit "Cross-game audit ...".

## Referee, second round: PS2 rounding default, joints in move(), tutorial (agent D, 18-20 Oct 2026)
- **Changed:** PS2 float rounding (RT_FPU_PS2) is on by default, both games, PC and Xbox; RT_FPU_PS2=0 gives the old
  round-to-nearest. **Why:** owner's decision 18 Oct after the referee (docs/RELEASE_G.md section 11 R-F8).
- **Dropped:** the joint sync at the end of sim_tick (sync_joints / remote_hunters / monsters_sync(0) before
  hit_check) and the Lao-Shan-only em_pose_fn call; **now** each monster's joints right after it moves (rt_em.c,
  enemy_mk) and the hunters' after body_hit (viewer.c, player_mk); hit_check uses those. **Why:** body_hit pushed
  hunter and monster apart one tick late (R-F9).
- **Dropped:** the stand-ins func_63B0C0 (returned 0) / func_63B470 (drew nothing) in rt_menu.c; **now** the game's
  SonchoMove / DispTutorial (tuto.c in the PC build), SonchoInit at a training quest's HUD set-up. **Why:** the
  Elder's lessons were missing and the buttons were not blocked while the PS2 showed them (R-F10).
- **Changed:** viewer.c: --help / -h prints the usage and exits before SDL; with MH1_HIDDEN set (and no MH1_SHOW) a run
  without game data exits with a message instead of opening the install dialog, and an ISO install shows no dialogs.
  **Why:** an agent's `mhview --help` opened the install dialog on the owner's screen (18 Oct).
- **Added:** config/mhg/pc_noptr.txt: 94 G data words that are numbers, left out of pc_ptrs.txt (R-F12).
- **Changed:** tools/test_activities.py trader: seeds 1 5 6 8 27 (TRADER_SEEDS overrides); with PS2 rounding seed 6
  no longer offers the trade.
- **To revert:** the commits "WIP: PS2 float rounding ..." / "referee round 2 ..." on agent-D.

## Referee, third round (agent D, 20 Oct 2026)
- **Added:** move()'s wall pass after body_hit in sim_tick (viewer.c). **Why:** pushed hunters stayed in walls a tick.
- **Changed:** CameraMove from right after the player to after body_hit / the wall pass / player_mk (viewer.c).
- **Dropped:** libm sinf / cosf / atan2f in rt_flmat.c (flSin, flCos, the flmatRot*, flArcTan2) and the textbook curve
  formulas in fmt/aan.c; **now** the PS2's own algorithms (VU0 sine / cosine polynomial and Hermite / linear forms,
  rounded to nearest through rt_vu.h; the C library's atan2f / atanf) with their constants read from the disc.
  rt_flmat.c, rt_motion.c, rt_main.c and fmt/aan.c (now built as its own object, build/pc/fmt_aan.o; not in the Xbox
  FRONT list any more) are compiled with the game's SSE float flags. **Why:** the referee: facings and root motion a
  float step apart, growing into drifts (RELEASE_G R-F15, R-F16).
- **Changed:** the hunter's game joints (sync_joints -> rt_player_parts) no longer include hunter_yoff (the drawn
  model keeps it). **Why:** 0.3 units below the PS2's (R-F17).
- **To revert:** the commits "WIP referee: ..." / "referee round 3 ..." on agent-D.

## Referee, fourth round (agent D, 20-21 Oct 2026)
- **Dropped:** fl_model.c's Euler-angle lerp for blended bones; **now** flmatBlend's matrix blend per bone (rows x / y
  mixed and re-orthonormalised); RT_EULER_BLEND=1 brings the old way back for comparison. **Why:** a blend bent joints
  up to 18 units away from the PS2's (RELEASE_G R-F22).
- **Added:** fl_skel extra Y turns (monster necks, rt_em.c rt_em_neck) applied in bone_world.
- **Changed (G only):** em01_main / em15_main / em20_main needs blocks, small-monster size after their inits,
  config/mhg/pc_noptr.txt entries (vital rows, Item_get rows).
- **Added (referee):** 15 scenarios (g_*), the tick-0 sync of EMW+0x39A.
- **To revert:** the commits "WIP referee G monsters ..." / "WIP referee: motion blends ..." on agent-D.

## Referee, fifth round (agent D, 21-22 Oct 2026)
- **Moved:** the frame counter game_w+0x1E from rt_game_move (quest ticks only) to rt_game_task_tick (every game tick,
  viewer.c), which also counts the play time (User_data, G +0xC1C). **Why:** Game_task does both every frame.
- **Changed (G only):** monster story scenes by G's quest numbers (rt_story_quest) and G's own scenes; em_eye_search_set's
  kind list for G's new kinds; G's act 6 scene code (em04b.c, em09c.c, em15_nm.c, em16_nm.c, em19b.c, em20_ai_nm.c,
  all `VERSION_MHG`, matched files also `!__MWERKS__`). em04_nm.c's earlier edit was dropped: em04b.c's em04_main is the
  one built.
- **Changed (G only, 22 Oct):** lb_by153.c forge question default, em19b.c fliers' ceiling (EM19_TOP), f_em_nm.c kind 12
  base scale, f_quest_nm.c Em_direct_set stage (em_tsuushin_tbl). **Changed (both):** viewer.c skips the area-exit
  check on an area's first tick. **Referee:** ps2_rnd.txt has a 4th column, the PS2's frame counter, set on the PC.
- **Added (referee):** village lockstep scenarios (g_vil_*), g_carve / mh1_carve, the hook's lock mode word (CTL+0x1C).
- **To revert:** the commits "WIP referee: G's story-scene ..." .. "WIP: Game_task's frame counter ..." on agent-D.

## G online, round 4 (agent C, 23 Oct 2026)
- **Dropped:** src/lobby/gonl/gen/gn_cnLBS_Send_ChatBinaryTU.c and gn___cnet_SendSet_ChatMessageTU.c. **Now:** the same G
  code as gn_gonl_Send_ChatMessageTU.c (G 0x5B0690) and gn_gonl_SendSet_PlazaEntry.c (G 0x5ACC30), G_RENAME in
  tools/mhg_gonl.py. **Why:** the split gave those G addresses MH1 names of other functions; under the MH1 names they
  replaced the PC's real cnLBS_Send_ChatBinaryTU / __cnet_SendSet_ChatMessageTU, and G's status send to a newcomer
  (lb_send_dataTU) crashed with the 5-argument whisper send. **To revert:** the commit "G online PC: two G clients see
  each other in the town" on agent-C.
- **Dropped (23 Oct):** six g_main_ADDR imports in config/mhg/pc_data.txt (2EEE70, 2EFA68, 2EFA6C, 2EFAE0, 2EF078,
  2EFAC0). **Now:** the gen files use G's data names there (plazaMenuTbl, key_quest_num, key_quest, lbSendInterval,
  friend_entry_str, joinQuest; tools/mhg_c.py reads config/mhg/data_map.txt high / medium). **Why:** our name hid G's, so
  the PC's key_quest became a function stand-in and the plain G village crashed (test_save_g, test_urgent_g,
  test_all_quests_g). Also build_pc.sh's BMATCH W rule narrowed to the G online weakening (.gsyms) and gen_rt_auto.py's
  gonl names to the G online build. **To revert:** the commit "G PC: the village again ..." on agent-C.
- **Dropped (23 Oct):** gen_rt_auto.py's lookup of mhg_gonl.py names for lobby `_o` aliases (added earlier this round).
  **Now:** src/lobby/gonl/gonl_g.c defines CallBack_Result_SearchUserPlace_o as a call of G's function. **Why:** the
  generated alias depended on the previous build's objects (a plain G build then an online one failed to link in
  full_check). **To revert:** the commit "G online: CallBack_Result_SearchUserPlace_o by hand" on agent-C.

## G online, round 5 (agent C, 24 Oct 2026)
- **Replaced (G online build only):** MH1's lb_cli functions (abs translations) Lbs_MatchEntry, Lbs_MatchEntryCancel,
  Lbs_RoomExit, Lbs_InRoomCheck, Lbs_MatchStart, Lbs_GetRoomInfo, Lbs_GetClassAdd, To_EnterRoom, Lbc_SetRoomRule,
  CallBack_Result_Lobby_GuestRoomMember, and MH1's lb_select_trans / lb_select_set_data / Lbc_GetRoomRule /
  lb_guild_make_room / lb_rule_seet_set. **Now:** G's own code, generated (src/lobby/gonl/gen; some HAND-FIXED). **Why:**
  the MH1 copies pass MH1 callback addresses and read MH1 cw offsets / record layouts; in G they crashed or never advanced.
- **Renamed:** gen files gn___cnet_SendReq_RuleListHeadWord / RuleNumOfChoice / gn__cnet_CallBack_Result_RoomRuleCaption /
  RoomSetFinish -> gn_gonl_SendReq_6316 / 6317 / gn_gonl_cb_BurstFail_5ABA90 / gn_gonl_cb_PlazaCount_5ABE90 (G_RENAME).
  **Why:** under the MH1 names they replaced the PC's rule reads / burst callbacks the MH1-flow cnet code uses.
- **Dropped:** lb_guild_g.c's `return 0` for get_questLevelNum online (the online table was "not ported"). **Now:** G's
  table (generated lobby_func_00542A60, MHG_ONLINE). **Why:** the counter treated every hunter as unregistered.
- **Changed (G builds):** include/lbsys_g.h CW_OFS maps MH1 cw 0x32BE..0x35CF to G (+0xC48C). **Why:** G's owner flag /
  ready count / server message sit there (checked against G's lb_guild_make_room, lb_basic_master); identity before.
- **To revert:** the agent-C commits of 24 Oct 2026 "G online (PC): ..." (eight commits from "Lb_move_common walks em_work").

## G online, round 6 (agent C, 24 Oct 2026)
- **Replaced (G online build only):** MH1's text_lobby_trans_ot3, Set_ErrorDialog and lbc_logout_01 (lb_cli.c).
  **Now:** G's code, generated (src/lobby/gonl/gen/gn_*.c). **Why:** G's Lbc_init_network_work (generated) puts prim
  pointers at pNet+0..0x10 and G keeps the dialog flag at pNet+0x2D; MH1's ot3 read +0xC (a pointer byte), so G's system
  dialogs were not drawn; G's error dialogs have other numbers. MH1 builds keep the MH1 C.
- **Changed (G builds):** include/lbsys_g.h CW_OFS also maps 0x35E0..0x35FF (+0xC48C; identity before). **Why:** G's cmcs_03
  and Lbc_connect use cw+0xFA6C / 0xFA80 there.
- **Changed (G PC only):** netsyn08.c / netsyn09.c quest messages at G's quest_w 0x280 block; aq_nm.c pl_state via GWX
  (identity in MH1); hunter_appellation / mcsls_swin_minus_init in config/mhg/pc_data.txt.
- **To revert:** the agent-C commits of 24 Oct 2026 from "G online (PC): the hunt on mh1-server's mcs relay (WIP)".

## G online, round 7 (agent C, 24 Oct 2026)
- **Replaced (G builds):** include/netsyn.h NGW's guessed G offsets of xF8 (0x1B6) and x110 (0x1DE); include/gwx.h's
  identity placeholders GWX_0xD6 / 0xD8..0xDB. **Now:** G's 0x1C2 / 0x1E0 and 0xDA / 0x190..0x193, read from G's
  net_receive_sys / net_game_w_clear / game11 / net_start_ck. **Why:** sys 5 and the start round wrote other G fields.
- **Replaced (G builds):** include/flow.h SELECT_W kept x54 / x5C / x94 at MH1's offsets in G. **Now:** G's 0x66 / 0x70 /
  0xB6 (include/swx.h has the full map). Also include/game.h GAME_W x70 / x78 -> G's 0x7C / 0x86.
- **Replaced (G online build):** lb_guild_g.c's hard-coded G pNet offsets (PN_SUB 0x22 ...) and the "pNet is MH1's
  layout on the PC" rule (PNET_OFS identity). **Now:** PNET_OFS maps MH1 -> G pNet offsets under MHG_ONLINE and the MH1
  lobby C reaches pNet through it (50+ files edited mechanically; identity in every other build). **Why:** G's generated
  lobby code shares pNet with that C in the online build; MH1's C wrote its dialog flag into G's prim pointers.
- **Dropped (test):** tools/test_online_g.sh HUNT=1's "continue without saving?" answer (left + circle presses after the
  quest). **Now:** each client starts from a G card made at the village bed, the relogin save writes it, and the card is
  booted back. **To revert:** the agent-C commits of 24 Oct 2026 from "G online (PC): the in-hunt packets in G's layout".

## Freedom pairing by structure (agent A, 24 Oct 2026)
- **Replaced:** tools/text_map.py freedom_pairs' slot-by-slot pairing (entry i of Portable with entry i of Freedom; text
  runs in order; an entry whose slots differed was dropped whole). **Now:** psp_entry_map (entries by header bytes and
  by quest / record / section ids), psp_slots kinds quest / records / talk, pairing per block / section / file with an
  MH1 US anchor check, freedom_ok (~ codes, stray ~, numbers). **Why:** the owner's Freedom has 8 more entries and its
  quests in another order, so the old pairing gave 201 G strings instead of ~4400.
- **Changed:** a G description whose lines had MH1 US English line by line now takes Freedom's whole description (192
  lines), so a description is not half MH1 US, half Japanese. **Why:** coherent text; MH1 US still wins everywhere else.
- **Changed:** text/mhg_en_own.tsv: 48 provisional names replaced by Freedom's (Firecell Stone, Paw Pass Ticket,
  Expert Blade, Gradios, Djinn, Seiglinde+, Great Golem+, "Helm S" suffix order, ...); 31 G-only weapon / armour names
  added in Freedom's style. **To revert:** the agent-A commits of 24 Oct 2026 "text_map: pair Freedom by structure ...".

## One host object for quest_rnd_host / quest_size_lim in the G online build (agent C, 25 Oct 2026)
- **Dropped:** config/mhg/pc_data.txt's g_sub_541E30 / g_sub_541E40 (tools/mhg_gonl.py's names for G 0x541E30 / 0x541E40).
  **Now:** the generated gn_lbc_game_ready_00.c / gn_lobby_func_0059A210.c write quest_rnd_host / quest_size_lim, the names
  Quest_start reads; mhg_gonl.py's gen_name takes pc_data's existing name for an address. **Why:** two names were two host
  copies on the PC, so the match's random numbers (from the battle code) never reached the quest; a real PS2 G started
  the Kut-Ku at 2200 HP, the PC at 2000 (tools/test_ps2_g_hunt.sh). **To revert:** commit "G online: one quest_rnd_host /
  quest_size_lim on the PC; one battle code per match".
- **Changed:** tools/mh1_testserver.py's battle code (6915) is made once per match instead of per request.

## rt_code_ptr on Windows / Xbox (agent C, 26 Oct 2026)
- **Dropped:** rt_game.c's non-Linux rt_code_ptr that accepted every pointer (`return 1`). **Now:** a 2D prim's draw
  function must be the exact start of a symbol of the build's own table (rt_host_symname, offset 0). **Why:** in the G
  online town a prim's draw word that is not code (0x1DD921; the Linux PC already skipped it) lies inside the Xbox XBE's
  text, so the Xbox called it and crashed 10 s after connecting. **To revert:** commit "Xbox: a 2D prim's draw function
  must start at a symbol (rt_code_ptr), full_check builds the G Xbox online".

## G online round 10: memory and the PS2's quest card (agent C, 27 Oct 2026)
- **Dropped:** net_peer.c's static `peer[4]` (4 x 64 KB receive buffers) and its 512-packet queue `q` (470 KB of .bss).
  **Now:** allocated by np_host / np_join / the first queued packet. **Why:** a game on the server's relay (rt_mcs.c,
  every real-server game) never uses the direct-connect transport; the Xbox G online build is short of memory.
  **To revert:** commit "Online memory: net_peer's buffers on first use, one rule-table buffer ...".
- **Dropped:** the three static 0x2A000-byte rule-table buffers (MH1's Lbc_GetRoomRule in lb_online_nm.c, G's
  Lbc_GetRoomRule and Lbc_GuestReadRoom in src/lobby/gonl/gen). **Now:** one `lb_room_rule_buf` (lb_online_nm.c).
  **Why:** 336 KB of the G online build; each use is one step's copy into RoomRule. Same commit.
- **Dropped:** tools/test_ps2_g_hunt.sh LEAD=ps2's three fixed Right presses on the PS2's quest board. **Now:** the board's 8
  cards are a random pick (QLIST, G 0x653510): the test finds quest 4's card (or writes 4 over card 1 when the pick left
  it out), moves the cursor (W_SEL, G 0x654830) there one right at a time, and checks the quest taken (mhRule+0x52) and
  the room's property. **Why:** the PS2 posted quests 20 / 36 / 37 / 6 in 4 of 10 runs (lost or late presses); that is also what
  showed the PC's map_name crash. **To revert:** commit "test_ps2_g_hunt LEAD=ps2: pick the quest card ...".

## The renderer score's PS2 capture (agent D, 27 Oct 2026)
- **Dropped:** tools/render_score.py's PS2 grab of PCSX2's 640x480 window with blend deinterlacing (PCSX2_DEINTERLACE=6).
  **Now:** PCSX2's screenshot at the internal resolution without aspect correction (512x448, the game's frame buffer),
  deinterlacing off, PCSX2 anti-blur off (the game's PCRTC line merge stays in); the ports are scored with --ps2-output.
  **Why:** the old capture scaled the 512x448 frame twice and blended the fields: much softer than the console, so the
  ports' sharp pictures lost points the PS2 never had (old vs exact PS2 picture: SSIM 89.8 on g_desert). Scoreboards
  before and after are not comparable (docs/render_score.md keeps the old one as history). **To revert:** commit
  "render_score: grab the PS2's own picture (512x448 frame buffer + its PCRTC merge), not PCSX2's presentation".

## Draw-function check on the Xbox / Windows (agent C, 27 Oct 2026)
- **Dropped:** rt_game.c's rt_code_ptr rule on Windows / Xbox "a 2D prim's draw function must be the start of a global
  symbol" (rt_symtab.c, round 9). **Now:** tools/codetab.py writes every function start of the linked exe, file-statics
  included, into rt_codetab (Xbox: from lld-link's map in build_xbox.py; Windows: llvm-nm on the exe in build_pc.sh);
  rt_code_ptr looks the pointer up there (the old rule stays as the fallback when the table is empty). **Why:** static
  draw functions (set01_trans: the "村長の教えが追加されました" banner in render_score's g_desert; any other static
  `prim->trans`) were skipped on the Xbox since 26 Oct. **To revert:** commit "Xbox / Windows: draw-function check knows
  static functions too ...".

## Actor lighting (agent D, 27 Oct 2026)
- **Dropped:** the fixed light set as the default for hunters, monsters and NPCs (viewer.c `light`, now only the fallback
  while light_work is empty and with RT_LIGHT_GAME=0), and rt_light.c's RT_LIGHT_VU test branch (its literal ambient of
  about 0.002 came from a misread scale: AMB is 128 in the actors' packets). **Now:** the game's own lights (light_work
  set 1, per actor through pl_light_change + Pl_light_set) by default, turned into each actor's own space and
  renormalised (fl_light_local, as Vu1Code_0016_0004's PROG does per bone), light colours clamped at 1.0, ambient half the
  summed rows (material A = 0.5). **Why:** checked per vertex against a PCSX2 GS dump of the camp (docs/pc.md "Actor
  lighting checked against the PS2"); the host lit actors with world light directions on normals in their own space, so the
  lights turned with the actor's facing, and no fixed light could match both camp and desert. **To revert:** commit
  "Actor lighting: the PS2's actor program maths ...".

## Testers' kit (agent A, 28 Oct 2026)
- **Dropped:** mhg_pcmeta.py's hard need for the developers' disc/mh1 (split MH1 + SLPM_654.95 relocations) and
  disc/mhg (unpacked G) on every G build. **Now:** a full run also writes the G symbol view to config/mhg/pc_symbols/
  (committed, names / addresses / sizes only, beside config/mhg/pc_ptrs.txt); a build without those inputs (a fresh clone
  with only the player's disc image: ./mh1, tools/make_release.sh) copies the committed files. **Why:** a fresh clone
  could not build G at all (FileNotFoundError disc/mhg/SLPM_658.69). Both files must be committed whenever a full run
  changes them. **To revert:** commit "mhg_pcmeta: a player's G build works without the split MH1 disc".
- **Dropped:** settings.ini always in E:\Games\MH1 / E:\Games\MHG on the Xbox. **Now:** a default.xbe started from the
  dashboard (installed by hand, e.g. E:\Apps\MHG) uses its own folder; an XBE a loader starts (mh1.xbe / mhg.xbe) keeps
  the game folder. **Why:** a hand-installed copy is self-contained (data, log, reports, settings, English table next to
  it). **To revert:** commit "Testers' kit: tools/make_release.sh ...".
- **Dropped:** ./mh1 xbox / windows (MH1 disc) emptying the whole output folder. **Now:** only MH1's own folders; a G
  build in dist/xbox/MHG or dist/windows/MHG stays. Same commit.
