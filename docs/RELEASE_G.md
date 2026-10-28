# Release readiness: Monster Hunter G (PS2) on PC and the original Xbox

Audit by agent A, 15 Oct 2026. Monster Hunter G is the project's main release (owner, 13 Oct 2026); this is the
G counterpart of docs/RELEASE.md (MH1) and uses the same layout. Four player-visible gaps found on the way were fixed
in the same change (section 4); everything else is listed for its lane.

How things are marked:
- **[verified]**: seen today in a hidden run, a coverage run or the code (says which).
- **[docs]**: taken from docs/mhg.md, xbox.md, text_coverage.md, REVIEW.md, COORDINATOR.md or agent notes, not re-run.
- **[inferred]**: my reading of the code or docs; not tested.

Owner lanes: **quest** (agent A: quests, monsters, items, hunt), **main** (agent C: menus, title, pause menu, player),
**village** (agent D: lobby overlay, shops, elder, NPCs), **text** (agent E: English), **Xbox** (agent B/F: nxdk build,
nv2a, loader, hardware), **platform** (src/pc host code, packaging), **owner** (a decision or a hardware check).

## 1. What "release-ready G" means

| # | Criterion | How to check | Today |
|---|---|---|---|
| G1 | Boots from the player's own G disc, nothing Capcom shipped | PC: an installer for the G ISO; Xbox: a player path to get the 1.1 GB of disc files onto E:\Games\MHG | Xbox: `./mh1 xbox DISC` makes dist/xbox/MHG; the loader's HTTP data install exists (tested in xemu; the owner's first try did not start the loader) [docs]. PC: `./mh1 linux|windows DISC` build G into dist/linux/MHG and dist/windows/MHG (GB3, 15 Oct); since 20 Oct (agent E) a player package (PLAYER.txt next to the program) opens its window by itself, so dragging the G .iso onto mhview_g.exe installs and starts it; build-folder runs stay hidden; settings in G's own mhgpc.ini [verified: both packages built from the G ISO, Linux package and Windows exe under Wine run hidden from the package folder, save/settings/log paths checked, docs/PLAYING.md "Monster Hunter G"] |
| G2 | Title -> NEW GAME -> character -> save -> village -> CONTINUE | hidden runs (section 3) | title, NEW GAME, creation, card save, village intro, Kokoto village: OK. CONTINUE crashed in the game menu whenever a save was on the card: **fixed today** (section 4). The 5th row crash is fixed (GB2, 15 Oct, agent C: it was NETWORK OPTIONS, not an EXTRA menu). OPTIONS opens and leaves through the card save [verified 15 Oct] |
| G3 | Every offline quest playable from the elder to the reward | tools/test_all_quests_g.sh (98 quests + carts), the elder's lists, a human play-through | 98/98 + carts pass with test aids [verified today]. The elder's quest lists were empty ("1 / 0"): **fixed today**. No human has finished G on the port |
| G4 | The village works: shops, forge, box, elder, counter | tools/test_gvillage.py (16 cases) | 16/16 OK [verified today]. Equipment detail / skill pages not ported (REVIEW "G village round 2") [docs] |
| G5 | Saves reliable | card save, CONTINUE, bed save | new-game save + CONTINUE OK today [verified]; G save is 78032 bytes, own Xbox title id [docs]. G save tests now: tools/test_save_g.sh (PC), tools/xemu_relaunch_test.sh --save / --powercut (Xbox in xemu: bed save, quit, relaunch, CONTINUE; power cut after a save) [verified, 28 Oct]; a corrupt save gets the game's own "damaged, start anyway?" (PC) [verified] |
| G6 | No stand-in or MH1-only logic on a path a player reaches | coverage (section 3), F8 reports | 43 distinct stand-ins reached (most harmless, section 3); 945 entered functions run MH1 logic where G's code differs; 37 zero-filled G tables still read |
| G7 | English | official US text where it exists, own text for the rest | G 47% official English (docs/text_coverage.md): weapons (G table) 0%, armour (G table) 9%, items (G table) 2%, NPC lines (G table) 5%, quests 44% [docs]. Source for G-only text is an owner decision (Monster Hunter Freedom idea) |
| G8 | Frame rate on the real Xbox | game log, owner's eye | not measured on hardware. xemu (software GL on the laptop, not a measure of the Xbox): G quest 10 8.1 fps, G costs the same as MH1 in the same scene [docs] |
| G9 | Memory on the Xbox | "alive" lines in the log | xemu: village 15.5-16.5 MB free, quest 10 20.8 MB, first village load low point 10.6 MB (MH1 village 16.5 MB) [docs] |
| G10 | Online (G's plaza / gathering hall) | ONLINE=1 build, own server | **nothing exists for G** [verified: no VERSION_MHG in src/pc/net or rt_online.c; build_pc_mhg.sh never passes ONLINE]. G's online code is lobby (128 KB edited plaza code) + nethttp.bin (0.4% matched) [docs]. Owner decision pending (own server only vs PS2 servers) |
| G11 | Controls / settings | PC F10, Xbox Back+L3 menu | shared with MH1 (Xbox settings menu, 16:9) [docs]; G's own OPTIONS screen not checked |
| G12 | Install / run steps a player can follow | a player page per platform | Xbox: docs/xbox.md owner steps; no player page for G; PC: none (G1) |

## 2. Gap list, by player impact

### Blockers

| # | What the player sees | Where | Status | Lane | Basis |
|---|---|---|---|---|---|
| GB1 | Most G-only text is Japanese: weapon and armour names, G items, the G NPC lines, half the quests | rt_text, tools/text_map.py; docs/text_coverage.md | 47% official English; the rest needs a source (owner: Monster Hunter Freedom for G content, own translation in its style) | text + owner | [docs] |
| GB2 | **FIXED 15 Oct (agent C).** The GAME MENU rows are NEW GAME / CONTINUE / GALLERY / OPTIONS / NETWORK OPTIONS: G has no MH1-style omake menu. Its Omake_task (main 0x1180F0) is NETWORK OPTIONS and the real "omake" is the GALLERY (sub_main 0x4A9960); MH1's omake menu ran for both on the PC (zero tables) and crashed on the 5th row (strlen of omake_str) | src/main/omake/omake_g.c (new; MH1's omake menu is fenced out of omake.c for G); build/C/gb2/ has the pictures | done: NETWORK OPTIONS (rows 0/1 change the two network bytes, EXIT saves; rows 2/3 need the PS2 network stack and answer with the "can't" sound); GALLERY (4 pages, page-turn wipe, cell wobble, NEW markers, unlock flags, movies on the port's player, the credits scroll, the save prompt on leaving). **21 Oct (agent E): the rest ported.** The 15 event scenes (G 0x4A91D0: a training or event quest's stage and monsters under the quest's event-demo camera; viewer.c gal_scene_*, omake_g.c gal_scene) and the credits' movie clips (cue list 0x50B678: ROLL01..06.sfd at G's places, rt_movie_clip). Needed on the way, both for the gallery and for G's quests: the monsters' event-demo checks use G's quest numbers (em01/04/12/16/19/20/27; G-only demos for em02 kinds != 2, em09, em15, em16, em19, em20, em04), G's 92-stage tables and 38 demo cameras imported, the credits' page 4 line taken for a pointer (pc_noptr.txt) | main | [verified: tools/test_gallery_g.sh 15/15 + credits; PS2 references at the same scene ticks (PCSX2, tools/g_gallery_ref.sh TICKS), build/E/cmp/tk_sN.png in agent E's worktree] |
| GB3 | G is not on the PC for players: `./mh1 linux|windows` refuse a G disc; the G PC build is hidden unless MH1_SHOW=1 | mh1:63, viewer.c G hidden rule | **done 15 Oct (agent D)**: `./mh1 linux|windows|xbox` build G from the G disc into dist/<platform>/MHG, play scripts set MH1_SHOW=1; docs/PLAYING.md has a G section | platform | [verified] |
| GB4 | Nobody has played G through on the port; every quest "passes" with test aids (warps, x40 damage, god mode, RT_PL_SLAY) | tools/test_all_quests.py G mode | process gap; owner F8 play-testing | owner + all | [verified: aids in the script] |
| GB5 | On the Xbox, G has not been played on hardware (installed through the loader, pad fixed; hands-on test pending). The loader's data install did not start on the owner's first try | docs/xbox.md "Owner's first try", "G on the owner's Xbox: no pad" | owner playing it (15 Oct 2026): loader update worked, G starts with pad. Bugs seen: (1) after the debug menu's skip-to-quest, polygons draw in the wrong order, and it stays wrong after going back to the village; (2) quitting to the title keeps the village music playing; (3) no villagers when the village is reached the normal way; villagers appear only when entering through the debug menu; (4) the opening wake-up cutscene in the village shows only blue sky for some frames (camera pointing wrong; the owner saw the camera get lost there on PC earlier too); (5) debug menu weapon swap does not change the weapon (maybe its sounds change); tried swapping to a bowgun; (6) title -> GALLERY -> first movie: flickers black after a few seconds, then the game crashes; (7) debug menu "go to village (no intro)" used on the title screen crashes the game (fixed 16 Oct: the row is greyed out until a hunter is loaded); (5, more) swapping to another blade weapon does not change it either, the sound is still wrong; (11, owner 17 Oct) buying an item in a shop shows the wrong "swap this item for that item" text (owner sent a bug report);; (14, owner 17 Oct, Xbox) entering the house turned the music into static, and it stayed static after going back out; sound effects stay fine, only the music stream; (13, owner 17 Oct) the equipment screen shows the Bone Lance (attack 161) correctly, so the data is right and only the hunter model in the village is not refreshed (still a sword on the back): the village must reload the weapon model on equip, as the original does. Same photo: the half-screen tint triangle of bug (1) is back (diagonal from the lower left), after only shopping, no debug menu used as far as known, plus a blue glowing column at the left; (13, details) equipping after buying is broken: bought a lance and equipped it, but the hunter shows a sword on the back (likely the same wrong item ids as 11: the equip writes or reads MH1 item/weapon numbers or offsets; check the shop's equip prompt, the equipped-set write at G User_data+0xC74, and the village hunter model refresh); (12, owner 17 Oct) saving a bug report on the Xbox freezes the game for a long time with no message (looked like a crash): show "saving..." and make it faster; (10) after a hunt (retire) one frame shows the hunter on the ground with sky where the (training) school should be, i.e. the next area drawn before its stage is loaded; MH1 had the same one-frame flash and agent F fixed it with a black frame during the game2 load steps: check that G takes that path; (8) the save screen is invisible again (as the menus were on the PC before the GB2/GB3 fixes); (9) debug "kill the monsters in this area" leaves one Aptonoth, and that Aptonoth cannot be hurt; the owner thinks it may be a scripted scenery Aptonoth (it runs across the river into the bushes), which would make it unkillable by design: check what the original does; then in the NEXT area a group of Aptonoth died by themselves (kill-all seems to act late or on the wrong area) while one survived, and one-hit kills did kill it: kill-all, not the monster, is suspect. Owner (16 Oct): if the original makes that Aptonoth unkillable, keep it so; only fix kill-all acting on the wrong area. Likely cause [guess, from the code]: rt_dbg_hits (src/pc/rt/rt_dbg.c) picks the monsters by comparing the area byte at +0x736 of the hunter and of each monster, MH1 offsets; if G moved that field, kill-all hits whichever monsters happen to match (here the ones in the next area) and misses the ones really around the hunter. One-hit kills do not look at the area (they act on a real hit), which is why it worked. Owner photo of (1): one half (a triangle) of a full-screen tint drawn over the scene, and the village gate beam drawn behind villagers beyond it: depth test / 2D overlay state left over, guess: from the debug menu's 2D drawing on nv2a [guess, not checked]  **Agent A, 16 Oct 2026 (PC G + xemu G, hidden runs, pictures in build/A/gb5/):** (8) FIXED: the bed's save / quit dialogs (lobby dialog layer) had no draw callback in G's Lbc_init_network_work, and G's tl_etc (lobby 0x636E70) was a function stand-in (drawing the prompt crashed once wired): callback set, tl_etc in pc_names.txt, G's choice strings 3/4 for dialog kind 4 (lb_by180.c) [verified: PC and xemu shots of 'save?' with yes/no; the new-game save-slot screen was already visible in xemu]; (6) FIXED: movie frames now update three fixed textures in place (gfx_update_texture) instead of a palette try + contiguous allocation per frame on the Xbox [verified xemu: gallery movie 9 played to its end, free memory 11 MB flat (was dipping to 4.7 MB), no black frames (7 black samples before)]; hardware speed unknown; (2) FIXED for the debug menu's title row: str_stop_all + se_stop_all [verified: RT_SND_TRACE 'str 0 stops']; (3) FIXED: G's Lb_reset returns at once offline (MH1's push_em_work_all cleared the NPCs and G places them only on the first load of a visit): villagers back after leaving the house [verified PC + xemu shots]; (10) FIXED: black frame while game mode is 6 and the village is not entered yet (viewer.c) [verified: RT_SHOTS tick 477 sky+hunter -> black]; (4) NOT REPRODUCED: new game intro and house wake-up checked tick by tick on the PC and every video frame in xemu, no sky frames (the demo camera fix of 14 Oct may be it); (1) FIXED: the debug menu switched the depth test off for its 2D and never back on: everything drawn after the menu had been open had no depth test (also PC and MH1) [verified: PC shots village gate before / after the menu, xemu quest + village after the debug quest start]; also the debug jumps out of the village now do Local_main's Pit_reset + Lbc_set_prim(0,0,0); the half-screen tint not seen after the fix [inferred: the menu's dim quad drawn without the depth state]; (5) FIXED: rt_player_game_init wrote the default weapon 156 over the save's at every quest start (MH1 and G): now the save's weapon (G ids up to 380) [verified: debug swap to 135 -> next quest weapon 135 model 60 job 3]; the swap shows from the next quest or village visit, as the menu now says; (9) NOT REPRODUCED: kill-all on PC G picks by the area byte correctly (log now lists every monster with its area and hp); the PC keeps MH1's EMW layout, so +0x736 is right; next-area deaths not seen (killed in area 33, then area 32: its monsters alive). The scripted river Aptonoth not identified (left as the original). New find: after NEW GAME's save a garbage line ('慰･$d') at 140,360 (select/edit_nm.c disp_edinfo_g reads G's help table for an item index past its end)  **Agent A, 17 Oct 2026 (bugs 11-14 and the triangle; PC G + xemu G, hidden; pictures build/D/g/wb*, wl1*, hs3 log in the A worktree):** (13) FIXED (both games): the host loaded the hunter's weapon model only at a quest start and followed PLW+0x34C only in the online town; the village shop / item box equip runs Set_userdata (and the original's Lb_player_load remakes the weapon), so the model now follows in the offline village too (viewer.c weapon_follow) [verified PC G: buy Iron Sword -> 'village: slot 0 weapon model 10 (was 69)', buy Bone Lance id 103 -> model 6, lance on the back in the shot; MH1 test_activities wshop_buy/box_equip pass with the follow line]; (11) NOT A SEPARATE BUG: the owner's report (stage 87, tick 8538) is the weapon shop buying the Arbalest (2800z) with the Bone Lance he had just bought and equipped as the current weapon (attack 161 matches his equipment screen): the comparison was right, only the model on the back (13) still showed the old sword [checked: PC G, after buying the Iron Sword the next comparison shows Iron Sword 288 -> Bone Blade 336]; (14) FIXED (G only; MH1's code path differs): G's vs_square_init (lobby 0x59EF00, a stage change inside the village) called lobby_bgm_set without its argument (the stage byte is in a0 in the asm); the garbage was 0 on PC and Xbox: Snd_bgm_tbl[0] = AFS00 60 M6_TAKI1 (a waterfall loop, noise-like) at full volume, in the house and again after leaving it. G's table gives the house (86) the village stream 28 at volume 0x37 [verified PC G RT_SND_TRACE: before 'str 0 plays M6_TAKI1.adx' on entering the house, after 'stage 86 ... fade 0 -> 55' on the village stream; not heard on hardware]; (12) FIXED (both games' reporter): the frozen frame now shows 'SAVING THE BUG REPORT...' for a frame before writing, input ignored meanwhile; the files go out in one write each (viewer_write_file; on the Xbox one WriteFile: pdclib's fwrite wrote 1 KB at a time, 900 per picture), PNG built in memory, Adler-32 with one modulo per 5552 bytes; the log line gives the time [verified xemu (tools/xemu_pick_test.sh --version mhg, 15/15 PASS): written in 5832 ms before, 247 ms after; the owner's hardware took ~80 s, its new time not measured; the one 'saving' frame not caught on a screenshot]; (1 again) NOT REPRODUCED, two candidates fixed: the owner's session did use the debug menu (money, t7168) and saved / cancelled three bug reports; the reporter left the game's texture stage on its own released frozen-frame texture (the game caches the bound texture, so its next 2D draws used a freed texture on GL / no texture on nv2a: plain colour quads) -> end_session now restores the game's texture (both games); stage_spr_disp's full-screen layer is left out of the reporter's id pass (it hid every object: picks came out 'other'). A PC near-plane trace in the shop scene found no skinned model crossing the eye plane  **Owner, 21 Oct 2026:** the half-screen green/tint "triangle" in the photos is a camera artifact of photographing the CRT (the scan line moves while the photo is taken), not a game bug: nothing to fix. (The depth-order part of (1) was real and is fixed.) **Owner, 21 Oct (Xbox, new save):** the Elder's level list offers only 緊急クエスト (a new save should get ★), and pressing on after "おぬしに頼めそうな依頼はこれぐらいじゃ" crashes where the quest list should open (photos build/xbox/owner_photos/, gitignored); not FPU or stack alignment (a test build with both off crashed too); agent A on it.  **Owner, 21 Oct evening:** a black screen on boot (no Back+Start) after the previous run had not shut down cleanly; the log (build/xbox/logs/20261021-190054-mhg.txt) stops at "ac97 open": the game hung opening the AC97 sound chip; after a power cycle the same build started normally. Needed: a time-out on the AC97 open (src/pc/xbox/xbox_ac97.c, both games): give up on sound after a few seconds and run silent instead of hanging. | Xbox + owner | [owner, hardware] |
| GB6 | 945 functions the G PC enters in ordinary play are MH1 C where G's code differs (628 edited, 19 rewritten, 71 missing in G, 227 same shape with other constants); each may behave like MH1 instead of G. Many are layout-only edits the PC already absorbs (it keeps MH1's EMW / lb layouts), so the real number is lower | docs/survey/mhg_cov_g.txt (G-CHANGED list); biggest groups: game/em 102, game/set 37, main/menu 34, game/shell 33, main/ud 28, main/pl 25, lobby/b 39, plus 359 in generated copies | needs triage per lane (sort by `sim`, read the low ones against G's asm with tools/mhg_fdiff.py) | all lanes | [verified: coverage today]; the effect of each [inferred] |
| GB7 | No online play in G (plaza, gathering hall, co-op) | section 1 G10; docs/network.md 3.12 | **in progress (agent C, 20 Oct)**: the real G disc (MH1LAUNCH RAM patch) logs in to mh1-server, gets G's info file, plaza menu, walks in our town; the server keeps G and MH1 apart. PC G online build (21 Oct): G's own lobby screens (about 600 G functions made into PC C by tools/mhg_gonl.py) log in, download the info file, show G's plaza menu and town list and walk into G's town (stage 0x4C, picture build/show/g_online_pc/pc_g_town.png); two G PCs together, the PS2 G disc with a PC, a G hunt and the Xbox are not done | platform + server + owner | [verified: PCSX2 run, build/show/g_online] |

### Noticeable

| # | What the player sees | Where | Status | Lane | Basis |
|---|---|---|---|---|---|
| GN1 (FIXED today) | CONTINUE (any save on the card) crashed as soon as the game menu came up | omake.c G disp_mode_menu read D_528538 / D_528550 / D_002EC668 as zero arrays (strlen(NULL)) | fixed: config/mhg/pc_names.txt + tools/gen_rt_auto.py | main | [verified: crash before, village after] |
| GN2 (FIXED today) | The elder's quest list showed "1 / 0" and stale rows: G's list builder never ran | lb_village_nm.c vs_square_init_init (G) had a stand-in where G calls Lb_make_quest_tbl_local; G's version exists in lb_guild_g.c | fixed: the call is back; the elder shows "1 / 5" with G's quests | village | [verified: shots before/after] |
| GN3 (FIXED today) | Quest reward screen: the selected item's name and count were blank; "time left" format empty | f_rewardd.c reward_itembox / f_rewardc.c disp_reward: lit_1001 / lit_1002 / lit_843 were zero-filled (G reads them through pointer cells 0x5262BC..) | fixed: G addresses in pc_names.txt (new `+` form: a second name for one G string) | quest | [verified: zero list, code; picture of the empty line] |
| GN4 (FIXED today) | Cephalos (em12) revival on desert stages 49/51/53 used zero positions (would revive at the map origin) | em12 em_die_rev (identical in G) reads st49/51/53_pos_tbl; zero-filled on the G PC | fixed: G addresses in config/mhg/pc_data.txt | quest | [verified: reached in coverage; G floats read]; the revival itself not seen [inferred] |
| GN5 (FIXED today) | Page counters ("1/2") in pause-menu lists drawn at the last text position (MH1 too, but only G reaches it) | chat_nm.c / f_chat.c Put_PageArrow call flfntLocate_i, a no-op stand-in | fixed: alias flfntLocate_i=flfntLocate (tools/build_pc.sh), both games | main | [verified: stand-in reached in G] |
| GN6 (FIXED 15 Oct, agent A) | Diablos/Monoblos (em14) and Gravios/Basarios (em17) chose their moves with MH1's code (G's to_normal is a jump to a 25-instruction act_dist_select; MH1's paths read em14_act_add / em17_act_add, zero on the G PC) | em14_nm.c, em17_nm.c | done: G's to_normal / act_dist_select / em_act01 / em17_to_fly, and G's em14_main / em17_main reactions (case 11 guards, subspecies 43/47/48 part breaks, quest 10041/10043 demo stops). The zero tables are no longer read. Same pass: em08_main (Cephadrome, kind 8; the note said Plesioth by mistake) is G's | quest | [verified: hidden hunts quests 14, 26, 13 (150 s each, no damage aid): 105 / 52 / 99 act changes, every act ends through em_cmd_ck] |

| GN6 (FIXED 15 Oct, agent A) | Diablos/Monoblos (em14) and Gravios/Basarios (em17) chose their moves with MH1's code (G's to_normal is a jump to a 25-instruction act_dist_select; MH1's paths read em14_act_add / em17_act_add, zero on the G PC) | em14_nm.c, em17_nm.c | done: G's to_normal / act_dist_select / em_act01 / em17_to_fly, and G's em14_main / em17_main reactions (case 11 guards, subspecies 43/47/48 part breaks, quest 10041/10043 demo stops). The zero tables are no longer read. Same pass: em08_main (Plesioth) is G's | quest | [verified: hidden hunts quests 14, 26, 13 (150 s each, no damage aid): 105 / 52 / 99 act changes, every act ends through em_cmd_ck] |
| GN7 | Equipment detail windows (shop, box, pause menu): attack 0, mis-sized sharpness bar, no skill / ammo pages, zero element names | main equip_exp_core (G 4.7 KB) and its unit (G main 0x2246E0-0x226C20) | **done on the PC** (agent C, 15 Oct 2026, src/main/menu/equip_g.c): attack with G's percentage table, sharpness bar of 6 bands + skill bonus, element / defence bonus, ammo pages of guns, skill pages of armour, description pages, compare windows, the pause menu's equipment page; open: the village's "preview" of a piece you do not wear uses G's gskill_preview on a scratch PLW [shot: armour compare, 3 pages] | village / main | [verified: hidden shots build/C/g/equip_*.png]; REVIEW "G main lane round 2" Checked in the weapon shop by agent D (square key, hidden run): attack 460 / 288, 6-band sharpness bars, element line (build/D/g/det_ws_230.png). |
| GN8 | Stage set pieces of set19 on stages 67 and 73/75 placed from zero tables (objects at the origin or missing); G edited set19_i / set19_trans | src/game/set/set19*.c, set19_s67_pos_tbl, set19_s73_s75_pos_tbl | **explained 23 Oct (agent B): faithful, nothing to import.** G's tables sit at game 0x6E05E0.. past the end of the overlay file (bss) and no G code writes them; set19_i / set19_trans are G's | stage (B/D) | [verified: G asm addresses beyond game.bin, PCSX2 read in FINDS.md] |
| GN9 | Elder's talk: "契約金として z が必要" — the fee number is missing from the sentence | G talk text formatting (village) | FIXED 15 Oct (agent D): lb_guild_talk (lb_guild_g.c) filled the `$` with a sprintf whose format came from a made-up address (printed nothing); now Lb_num_to_str(guildPrice, p) as G (lobby 0x5935A0), the same call that draws the fee on the quest sheet. Not pictured: the test hunter's first quests cost 0z, so the sentence with a fee did not come up in a hidden run | village / text | [verified: G asm]; the picture [unverified] |
| GN10 | Name entry: G's soft keyboard draws its key grid only partly (G field offsets done, not byte-exact); kana -> kanji (apiask_28_OpenDic) is a stand-in | sk_nm.c, rt_gen.c | partly | main | [docs], stand-in [verified] |
| GN11 (FIXED 15 Oct, agent A) | Movie / gallery unlocks: MH1's movie_add_ck ran (missing in G) with a zero movie_add_tbl: nothing was unlocked; the "movie added" screen text was zero (lit_676) | src/main/reward/f_reward_g.c | done with GN17: G's ecology movies by cleared quest (sub_main 0x4F42A0, table 0x525FC0) and the gallery titles by event flag (0x4F41B0, rows of 0x526000) set G's Omake flags and show G's "added to the GALLERY" screens | quest | [verified: hidden runs, quests 10043 (Gravios ecology) and 10015 (title "密林の大怪鳥"), shots build/A/gn17/] |
| GN12 | Pause menu: the chat-config page (pf_chat_cnfg) and the data page frame list (pfl_menu_data) are zero; quest_condition_print's lit_2331 zero | menu_disp_nm.c | **FIXED 23 Oct (agent B)**: not missing data, G dropped them. G's data page draws one frame list (main 0x25E820, pf_item_box_base+0x10); G's chat settings are the preset list only (Menu_chatcnfg_i/_mv, Pit_disp_chat_cnfg, menu_chcnfg_reibun ported, VERSION_MHG); lit_2331 = "%s：" at sub_main 0x538958 (pc_names.txt) | main | [verified: G asm; PC G build, see agent-B.md] |
| GN13 (explained 15 Oct, agent A) | Quest-score title on the result screens shows nothing after "クエストスコア :" | G's gold_disp / point screen (sub_main 0x4F5D40 / 0x4F6DF0) | G prints the hunter's name there (User_data+8 offline, my_user_handle online); the test hunters of tools/test_all_quests.py have no name. Now G's own screen code (f_reward_g.c) | quest | [verified: G asm]; the empty name in test saves [inferred] |
| GN14 | Xbox: G frame rate and memory on hardware unknown (xemu only); rt_meta.c costs 1.5 MB of .rdata on G | docs/xbox.md "Monster Hunter G on the Xbox" | owner test | Xbox + owner | [docs] |
| GN15 (FIXED 15 Oct, agent A) | Sound: G's em04 sound script ran MH1's (stand-in sub_main_func_004ECD00); G's Eft13 dust | em04_nm.c, rt_quest.c | done: G's script differs only by kind 32's sound in motion 0x3F7; the dust script (0x4EDAA0) runs in G only while overlay 2 is in slot 2 (gallery event scenes), so no dust in quests, as on the PS2. No stand-in left | quest | [verified: G asm (0x4ECD00 decoded call by call), build]; sound by ear not checked |
| GN16 | No G save test (round trip, corrupted card, two slots) like MH1's test_save.sh | tools/ | open | platform | [verified] |
| GN17 (FIXED 15 Oct, agent A) | After a quest MH1's result_prog ran: no hand-in of G's five quest items, no record / training / gallery announcements, MH1's screens | src/main/reward/f_reward_g.c (G sub_main 0x4F3A50-0x4F7FDC, replaces f_reward.c / f_reward2.c / f_reward3.c in the G PC build) | done: G's 20-step result_prog: reward list, money, **sale of the special items** (ids 178, 260, 164, 139, 145 leave the pouch and are paid at their price), "monster hunting record updated" (picture, old / new size, crowns), "added to the training school" (User_data+0xDDC bits by kill count), gallery movie / title, online point screen and error screen as G. Found on the way: **func_0012D170 read past game_w** (rt_quest.c): in test runs it returned 1, so every quest end put the pouch back to its departure state and skipped the sale; now G's system_w test. **Not ported:** G's frozen game frame behind the reward list (done 16 Oct, section 8); the random number G saves at User_data+0xD0A (no reader found) | quest | [verified: hidden runs of quests 10, 10015, 10043 reach every offline step (9-16), shots build/A/gn17/1_reward_list .. 7_gallery_title.png; the sale paid 1682 z for 2 x 178 and 3 x 145] |

### Minor

| # | What the player sees | Where | Status | Lane | Basis |
|---|---|---|---|---|---|
| GM1 | Kind 40 (unused monster) not ported; no quest on the disc uses it | game 0x647060.. | deliberate | quest | [docs] |
| GM2 (mostly done 15 Oct, agent A) | em07_init's G calls, point_cam_sub case 25, G result_prog's other changes, poison knife ailment value | em*_init, emsrch*.c, cam_nm.c, f_reward.c | done: the G part-durability level bonus (em01/07/08/14/15/17/20/21 inits mark parts, em_dur_init / em_dur_set add base x (0.33 + 0.08 x (lv-9)/3) at level 9+: offline quests are level 2-7, so it shows after revivals and online); point_cam_sub 25 (demo eye kept out of walls); poison knife = G attack entry poison 50 per hit through dm_value_add, plus G's poison_stock_set (no build-up while poisoned). G's result module done (GN17); knife hit [verified 15 Oct: hidden run, pinned Rathian, one poison knife: hp 5500 -> 5496, stock +50 of tolerance 200, decays 10 a step] | quest | [verified: durability bits in RT_EM_TRACE, knife entry read from G's tables]; knife hit seen (build/A/knife) |
| GM3 | Stage_Hate_Add (missing in G) runs with a zero stage_hate_add_tbl: adds nothing, as G (which removed it) | em_core_nm | harmless [inferred] | quest | [verified: reached] |
| GM4 | Chat log name (lit_3181) and two NPC message strings (lit_3440/3441) zero | chat, rt_real_Disp_NPC_message | **FIXED 23 Oct (agent B)**: lit_3181 = "%s：" (0x538958), lit_3440/3441 = the yes/no lines (0x537F40 / 0x538D30), in pc_names.txt. Not ported: G's chat font-size option in Disp_NPC_message | main / village | [verified: G asm, cells 0x2EC62C, 0x527C64/68] |
| GM5 | Sprite sort tables (sort_no, spr_sort_flag, spr_sort_num) zero: draw order of some 2D sprites | trans_sprite_sort | **explained 23 Oct (agent B)**: G has no sorted sprite path (sort_sub / trans_sprite_sort missing, trans_sprite at main 0x12A180 has no softdip 0xAB check); trans2.c skips it for G. The tables were only working storage | platform | [verified: G asm] |
| GM6 | Event 0 / story talk choices of agent D (Event_flag_set(0) guess, story mode numbering) | REVIEW "G village round 2" | owner review | owner | [docs] |
| GM7 | The 'checking memory card' screen after START stays until a button is pressed in a scripted run (may be faithful) | title -> game menu | unverified vs PS2 | main | [verified: shots], faithfulness [unverified] |

## 3. Coverage run (15 Oct 2026)

Build: `COV=1 tools/build_pc_mhg.sh` (G PC with -finstrument-functions). Runs, all hidden, with
`RT_COV=file RT_STANDIN_FILE=file` (script kept in build/, not in git):
title -> NEW GAME -> character creation -> card save -> village intro -> Kokoto (newgame.txt + walk);
CONTINUE from that card -> house -> bed script; the game menu's 5th row (EXTRA: crashes, GB2);
tools/test_gvillage.py (16 cases: weapon/armour/item shops buy/sell/quantity, forge weapon/armour/upgrade,
box store/take/equip/tidy/sell, the elder's first talk), the elder's quest list with story flags 0,1,4
(accept -> "not enough money", faithful for a 0z hunter); quests 1 7 9 10 11 15 23 25 32 41 10033 10038 10041
10042 10048 through `G=1 tools/test_all_quests.py` (clear + time-out each; G-only kinds 37/44/49 among them);
tools/test_revival_g.sh. All passed except the EXTRA row.

`python3 tools/cov_report_g.py cov.txt` (new: maps entered functions to docs/survey/mhg_map.csv and checks whether
the PC's body is a G version, by a G marker in the body or the G branch of an `#if` on VERSION_MHG):
4635 functions entered: 1976 identical in G, 240 shape, 821 edited, 32 rewritten, 78 missing in G, 1488 PC-only
(host and G-only code). 945 of the shape/edited/rewritten/missing ones have no G version on the PC (GB6); full list
in docs/survey/mhg_cov_g.txt, with the zero-filled G tables an entered function reads.

Stand-ins reached (43 distinct):
- Matter to a player: `apiask_28_OpenDic` (kanji conversion), `sub_main_func_004ECD00` (G em04 sound script runs
  MH1's), `G McCardOperation op 0xB` (no MH1-hunter carry-over: NEW GAME always makes a new hunter),
  `G boot: set-up quest 10` (power-on demo quest not loaded), `func_001F19C0` (a G option byte, menu),
  `select_func_00542B50` (G select overlay, game menu).
- Fixed today: `flfntLocate_i`, the `G village: Lb_make_quest_tbl_local` stand-in.
- Harmless on PC/Xbox (host owns screen, loading, models, draw order), as in MH1's audit: flAdjustScreen, flCalcTrans(SI),
  FlushCache, flCompact, view_reset, View_init, init_view_work, load_busy_ck, load_bin_req, load_eft, load_shadow,
  flSnd* (5), set_viewproj, stage_free, ot_init, round_init, snd_joint_load_init, em_effect_pull, model_work_init,
  ADXM_Lock/Unlock, flReleaseMotionSetHandle, clr_used_heap, all_model_free, armor_model_free, edit_create_model,
  flmatTranspose33 (VU0; the host poses monsters), init_set_work / init_shell_work / init_eft_work (deliberate, MH1 M11).
- Online-side, harmless offline: lb_member_inCheck / outCheck / changeCheck.

Zero-filled G tables (build/mhg/pctree/build/pc/mhg_tables_missing.txt): 77 before today, 71 after; 37 are read by
entered functions (list in docs/survey/mhg_cov_g.txt; GN6-GN8, GN11, GN12, GM3-GM5).

## 4. Fixed in this change (15 Oct 2026)

- CONTINUE crash (GN1): config/mhg/pc_names.txt gives D_528538 / D_528550 / D_002EC668 their G addresses, and
  tools/gen_rt_auto.py no longer aliases a D_ name that G's symbol view places into the MH1 object at the same hex
  (D_002EC668 had become weapon_model_data+0xB8). Side effect, also a fix: D_004FEA50 (the 50 quests of the G story
  gate sub_main 0x4C9410, read by lb_guild_g.c) was aliased into MH1's page_tab+0x1140 and is now G's table.
- The elder's quest lists (GN2): src/lobby/f/lb_village_nm.c calls G's Lb_make_quest_tbl_local at village entry.
- Reward screen item line and time format (GN3): pc_names.txt, with a new `+` form in tools/mhg_pcmeta.py
  (add a name beside an existing one at the same G address; G merged MH1's lit_1002 and lit_4893).
- Cephalos revival points (GN4): config/mhg/pc_data.txt.
- flfntLocate_i alias (GN5): tools/build_pc.sh ALIASES (MH1 and G).
Checks (15 Oct, after the fixes, plain builds): clean `tools/rebuild.sh` 5 OK then `VERSION=mhg tools/rebuild.sh` 7 OK;
MH1 PC build, tools/test_quest_loop.sh OK, tools/test_all_quests.sh 38/38; G PC build, tools/test_all_quests_g.sh 98/98 + carts,
tools/test_dbgmenu.sh OK (MH1 + G), tools/test_gvillage.py 16/16; G nv2a Xbox build OK. Notes: docs/agents/agent-A.md "G round 8"
(a COV=1 build leaves instrumented objects that the next plain build links: delete them first).

Second pass (15 Oct 2026, agent A, quest lane): GN6, GN15, most of GM2 (above), and
- monster attacks through shell11 (Cephalos, Vespoid/Hornetaur, kind 40): G's shell11_i takes the attack entry as the kind and G's callers pass
  G's numbers; the PC mixed MH1 callers with G's table (one entry off: kind 40's swing did no damage, the others used a neighbour's attack data).
  shell08_i kinds 8 / 12 as G [verified: kind-40 run, the hunter loses 10 hp per swing].
- em08 (Cephadrome, kind 8: this line said Plesioth by mistake) main as G (part of GN6's pass). Its fly (swim) acts are G's twins: fly07/10/16 are identical in G (the symbol view only gave G addresses other names); G's dispatcher drops fly19/20/23, dmg08/12/13/14/18 and act08-13 (unreachable in G) [asm diff, corrected 15 Oct].
- Not taken: GetWallHitBit* / HitWallPlayer (f_sphr.c, main lane section 6): the three GetWallHitBit* differ by register / stack layout only;
  HitWallPlayer scales the monster's wall sphere by G's size % (EMW 0x9F8, 60-200 -> base scale 0x600) [asm diff].

Third pass (15 Oct 2026, agent A, quest lane): GN17 with GN11 / GN13 (above), the knife hit (GM2), and
- **Plesioth (em21)**: G sets the land / water state x388 through game 0x62DB60 in 63 acts, which also switches the AI's distance set
  (em_range_set: 2 while swimming, 1 with the flag, else 0), so G's Plesioth picks its moves with water distances while it swims; MH1 never
  switched. The part-3 break has its own damage act 23 (G swapped the flinch motions of parts 3 and others against MH1). [verified: quest 11
  trace, range 2 whenever x388 = 4; quest 35 passes]. **Closed 16 Oct (fourth pass):** the quest-10035 opening and em21_init's G kinds are ported; "die case 4" and
  "0x60C280" came from a wrong map pair (another monster's move05/06): G's em21 has the same four die acts as MH1 (section 8).
- **Cephadrome (em08)**: G's atk03 / atk04 (game 0x5CA620 / 0x5CA900; MH1's are empty) are three-way sand spreads (Shell08 kind 7 + Eft17).
  Ported; not seen chosen in the quest-13 trace (short fight with the x40 damage aid) [code from asm].

## 5. Suggested order [inferred]

1. GB2 EXTRA menu crash (main), GN6 em14/em17 G AI (quest), GN7 equipment windows (village), GN9 elder fee text.
2. GB6 triage: per lane, the low-`sim` entries of docs/survey/mhg_cov_g.txt that a player reaches; start with
   game/em, game/shell, main/pl, main/ud (hunt and save data).
3. GB4/GB5: the owner plays G on PC and Xbox (F8 reports / plain chat), one star level at a time.
4. GB3: `./mh1 linux|windows` for a G disc and a player page; GB1 English source decision; GB7 online decision.

## 6. GB6 triage, main lane (agent C, 15 Oct 2026)

Method: docs/survey/mhg_cov_g.txt, the 243 entries under src/main (the lobby, em*, quest and village lanes left out), each aligned against its G
twin with the register/opcode view of tools/mhg_fdiff.py (immediates and offsets masked, so struct-layout-only differences count as 0): 54 are pure
layout (nothing to port), 65 differ by less than 15 instructions, 105 by more, 19 have no G twin (missing: G removed or rewrote them). Ported or fixed this round (all [verified] in hidden runs unless said):
- **Quest clear bits** (Quest_clear_bit_set / _ck, sub_main 0x4C74D0 / 0x4C7550): G numbers the 256 bits by position in a list of quest numbers
  (sub_main 0x4FECA0, 245 of them); MH1's quest-number-as-bit-number wrote past the bits for every G quest above 255 (10041 = bit 313): memory
  next to User_data was overwritten after each won G quest. Now G's mapping (ud_nm.c, quest_bit_tbl in pc_data.txt); tools/test_all_quests_g.sh 10 10033 10041 pass.
- **Omake flags** (Omake_flag_set / _ck): the G PC build links ud_nm.c (f_ud.c is in tools/pc_g_exclude.txt) whose copy had MH1's s16 at option_w+0xFCC; G's is a
  u32 at +0x2C4C. option_w itself was 0x1200 bytes on the G PC (G's runs to 0x2E80: the flags and the network options wrote past it).
- **Disp_button** (G 0x12DBA0): G's icon sheet, kinds 20/21 (decide / cancel) are cells 0/1, a colour parameter. The G screens drew white squares
  for the O / X prompts (mode menu, gallery, card dialogs); every G caller but sub_main 0x4BF800 (not ported, G-only HUD) passes -1 through G 0x12DD90.
- **Memory card messages**: McardMsgTbl is 63 entries in G (the symbol view had 51): the OPTIONS / GALLERY save crashed at entry 51.
- G's GAME MENU title strips 7 (NETWORK OPTIONS) and 8, PIT_TEX[4] (gallery sheet), putspr_g.c (func_0012A8B0 / 0012A800 were no-op stand-ins on the PC).
- rt_movie.c reads G's own movie table (main 0x2A0DE0, 12 movies; MH1's sfd_tbl has 9).

Still MH1 logic where G differs, that a player can notice (nothing here is a crash; sizes are G's):
| What | Where (MH1 name, G twin) | What the player sees [basis] |
|---|---|---|
| Pause menu status page | PlayerStatusWindow (main 0x222E50) + the skill pages | **done** (agent C, 15 Oct 2026, src/main/menu/status_g.c): page counter, money, attack (G's percentage table), G armour / skill names, the three skill pages with the points of every piece, the hunter-title ranks up to 30 [verified: hidden shots build/C/g/status_p*.png]; the village hunter's name line (pl+0x8D4) is now filled: rt_village.c calls Set_userdata for the master on the first visit (it only did with RT_POUCH) and test hunters get RT_NAME [verified: trace line `rt_village: hunter name 'TEST'`] |
| Pause menu help line | row ステータス showed the friend-list help | **done**: G's Menu_status_i clears PitMenu.x10 (G's D_002FF2A0), so no help line under the pages |
| Equipment detail windows | equip_exp_core 4.7 KB, EquipmentCompareWindowA, sword_zokusei, slash_level_bar (6 sharpness levels), Put_receive_mark | **done** (= GN7, src/main/menu/equip_g.c); Put_receive_mark (the "received" mark of the item window) is not looked at |
| Standard camera | cam_sub_std (main 0x1D9D60), func_001DAE60 / func_0010AE00, k_HitEmCamera | **done** (agent C, src/main/cam/cam_nm.c, camr5_nm.c, src/main/hit/f_sphr.c, G branches): pad turning (0xC00) first, wall turn 1/8 per frame, angle easing 1/4, target 1/2, field-of-view easing, stage tables x5F7 / water camera, far camera jumps; **eye lift behind ridges**: G's ground line test func_0010AE00 (2 KB, G only, written from the asm as C in f_sphr.c: walks the ground grid cells along the segment, first crossed polygon wins) from the eye to the hunter's head joint, lift +20 per frame up to 300 while it is blocked and the ground at the eye is more than 150 above the ground at the target, else back by 20; **k_HitEmCamera** (G: only the first push01 sphere is tried per monster, body entries flagged 4 are skipped); **not done:** QuestClearCameraRequest, NPCZoomInCamera* still MH1 [hidden runs: camera_wall_tent.png; func_0010AE00 cast from the hunter in 8 directions with RT_GLINE=dist RT_GLINE_H RT_GLINE_DY (rt_cam.c): a ray that dips into the ground reports a hit in all 8, level rays report none; x2C is in the RT_CAM_TRACE line as `lift`; a real ridge was not staged] |
| Stage drawing | trans_stage (15 KB, +450 instructions in G), stage_set_set, stage_spr_disp | **trans_stage done 16 Oct (agent A, section 8)**; earlier note: G adds 12 blocks of about 720 instructions in total (new per-stage cases with their own matrices and clay draws: the stage-number ladders now also list G's stages 8, 15, 88-91; the first block is a model at a fixed place turned by the frame counter), each needing a G stage to compare against; the stages look right in the village and quests [asm diff only] |
| Wall collision | GetWallHitBit2 / GetWallHitBitEm / GetWallHitBitPl / HitWallPlayer (f_sphr.c) | **checked, one change** (agent C): the three GetWallHitBit* are the same logic as MH1's (G only moves the "already seen" searches into func_00106000 / 00106050 and changes registers [op-sequence compare]); HitWallPlayer now scales a monster's wall spheres by its base scale (EMW 0x600) when its size is 60-200 % and treats kind 50 like kind 7; the hunter slides along walls on the G PC [hidden run RT_PL_TRACE: build/C/w/w0.log, pos 9601,9633 -> wall at 9624,9586 -> 9042,9727] |
| Player timers and moves | timer_calc_sub_pl (+68 instructions), pl_move_sub (a new field at +0x940, Pl_act_set instead of Pl_act_set2), Pl_item_stack, St_pick_ck2, body_hit_sub_new | **timer_calc_sub_pl and St_pick_ck2 done** (pl_nm.c, plx10.c: the +0x93E timer, stamina drain state +0x93D, life-bar recovery every 60 / 20 / 15 / 180 / 240 ticks by the damage-recovery skills, gathering skills 164-167 lower the chance a spot runs dry) [asm diff; compiled, not seen in a run]. **pl_move_sub done** (agent C, 15 Oct, pl_nm.c): the status reactions use Pl_act_set (G) instead of Pl_act_set2, work601 / work917 are no longer cleared, work940 = work8F0 each step (hit_mark_set_new reads it; that function's +150 G instructions for the skills 169 / 79 are not ported, see below) and work93D clears when flag12 == 0, no camera toggle in state 6; the net position packet slot now compares with G's master byte (game_w+0x3A; the MH1 +0xD1 read the wrong byte, also in em init f_em_nm.c) [asm diff]. **body_hit_sub_new done**: body entries flagged 4 are skipped, the push never sinks the hunter below the ground line x5AC [asm diff]. **Pl_item_stack**: the only G change is the item-book bit (sub_main 0x4C8EF0 sets bit n of User_data+0xDE8 for the item's collection number from a 250-entry ladder at 0x4C8B10) at three places; nothing on the PC reads the bits, so it is left out. **hit_mark_set_new: done** (it already matched G on the PS2 since 14 Oct, commit 03aca46d, and the G PC links that C from hit.c; this row was stale) [verified: G rebuild 7 OK, the PC binary's hit_mark_set_new is hit.c's G body] |
| Card save screens | disp_savesel / disp_savesel_waku / disp_savesel_moji | **done** (agent C, src/main/mc/mcdisp_nm.c G branch): G's slots are 0xE00 bytes in option_w (MH1 0x480), each shows `1:NAME`, `TIME: h'mm" / MALE`, `MONEY: n z` (with `/ HR:rank` once the hunter has one) as G's own strings (gsave_fmt in pc_data.txt); empty slots NO DATA; waku takes the highlighted slot as 4th argument. Found on the way: **Default_reibun_set wrote the chat phrases at option_w+0xDB0 (MH1) = inside G's second save slot** (G: +0x2A30), so a fresh card showed slot 2 as used with garbage [verified: shots build/C/g/savesel_empty.png (three NO DATA after NEW GAME), savesel_continue.png (1:TEST ... after a save)]. Not ported: the PS2's buffer mode (G draws the slots from the card image while it checks; the PC keeps them in option_w) |
| Fade | Fade_busy_ck / fade_set / Fade_task (a second state byte) | none seen |
| HUD bits | disp_timer, disp_slash_level (G's levels), trans_box, player_info_sub, disp_item_list_present, disp_whole_map | small differences [asm diff only] |
| Hunter rank | Get_hunter_rank / Get_hunter_status early-out when the rank byte is 0 | none for a normal save |
| effects | eft13* / eft20* / eft06_m / eft02_t (edited 0.65-0.97; they are the effect lane of em*) | monster effects |
| Gallery scenes, credits movies | see GB2 | 15 gallery entries inactive |
The pure-layout and < 15-instruction rest (disp_*, set01_*, Info_*, Get_equip_* ...) are listed by running the method above; they only read other
struct offsets, which the G build already has (UDW / PLW / game_w G layouts).


## 7. Village lane, second pass (agent D, 15 Oct 2026)

- **Lb_put_shopYesNo** (yes / no line of the shops): was invisible. The first port read G's message table through `D_2EC5D8`, which the PC takes as
  an MH1 address; now the PC's own symbol `yes_or_no` (same G address) printed with font_print_sp. [verified: weapon shop buy, shot build/D/g/yn_230.png:
  "[yes] . no" at the bottom right of the message box]
- **GN9 elder fee text**: see GN9 above (Lb_num_to_str). Fee sentence not pictured (the first quests cost 0z).
- **Lb_equip_set**: G has no such function (map: missing). G's itembox_equipchange (lobby 0x553774) steps 0 -> 1 without it, then runs armor_set_myArmor and
  Skill_set_PL on the hunter; lb_ib.c now does the same for G (the MH1 branch that called Lb_equip_set stays for MH1). [asm read; box equip case of
  tools/test_gvillage.py]
- **Banner animation** (story mode 48, lobby 0x58DCC0 / 0x58DC30 / 0x58D7A0): G's banner is a 90-tick wait, then the "close" button until key 0x40; the
  registration card and its five sparkles are drawn only when game_w.stage is 0x4D (not Kokoto, stage 0x57), so in the village the wait and the prompt are
  all there is. Ported that (lb_talk.c, `lb_banner_close_str` in pc_names.txt). The card / sparkle drawing (0x58D7A0: one 256x240 sprite of texture
  0x119 and five spinning textured quads) is not ported: no stage 0x4D on the offline path. [asm read; not pictured: the registration NPC is not reached by the
  test scripts]
- **lb_npc_trans** (0.78): the PC draws the NPCs through the host (viewer.c), lb_npc_trans only queues the models the PC does not have (the C returns early
  when the model block is missing), so G's extra material kinds (case 3, texture 0xC1) have nothing to act on. Left as is. [code read]
- **DispDialogData** (0.70): G added dialog kinds 9-12 (loading screen, member list, a framed picture) used by the online town; the offline village uses
  the kinds the port already draws. Left as is. [asm read]
- **Disp_lb_item_box** (0.71): the G differences left are field offsets already carried by IBO() and the tidy prompt (done in round 2); no new visible piece
  found. Left as is.

Checks (15 Oct 2026, agent D, after the second pass): tools/full_check.sh all ok (rebuild 5 OK, PC builds, every test, both Xbox builds); `VERSION=mhg tools/rebuild.sh`
7 OK (3 GB cap); tools/test_gvillage.py 16/16; tools/test_all_quests_g.sh 98 quests + carts; G nv2a Xbox build OK (build_xbox.py --version mhg --gfx nv2a).

## 8. Fourth pass (agent A, 16 Oct 2026)

What a G player notices, each checked in hidden PC G runs (pictures in build/A/text, build/A/rwbg, build/A/q10035):
- **Character creation title** reads "CHARACTER CREATION" (was "...CREATIONAAAA" off the screen edge). Cause: splat named
  the data word 0x0000FFFF "D_FFFF" and tools/mhg_pcmeta.py took every `.word D_FFFF` as a pointer, so the loader zeroed
  it: 78 words of G main, each the -1 end of an s16 list (the title strips and other tables). Fixed in the tool,
  pc_ptrs.txt regenerated [verified: Sel_menu_disp trace before/after, shot edit_title_icons.png].
- **Button icons in text** (G's `~Bnn`): the circle / cross etc. are drawn (PutButtonICON with the entry G queues,
  func_0012B8F0) instead of a gap [verified: character creation help line "(O) enter (X) cancel"].
- **Plesioth, quest 10035**: G's opening scene now has the Plesioth swimming its scripted path (game 0x6345E0, mode 6
  act 1, 14 steps: placed, dives, turns, breaches with the spray at frame 114) while the demo camera runs, then it swims
  off after event flag 22, as G's em21_main (0x636C08) does [verified: trace of the 14 steps in step with the camera's
  commands, flag 22 set after the demo, shots]. Seen on the way, not fixed: ticks ~230-380 of the scene (an underwater
  shot, camera at y -129..100) show the stage from below on the blue clear colour [not compared with a PS2].
- **Reward list backdrop**: the frame of the quest clear (G: system_w+0x33 request in quest_condition_prog, copy in the
  frame's trans, drawn at half alpha by main 0x12EAF0) is copied with gfx_read_pixels into a 256x256 texture and drawn
  behind the list (src/pc/rt/rt_2d.c rt_cap_*). The PC starts the clear camera at once (MH1 timing); G waits one frame
  [verified: quest 10 with RT_STEP=1, shot q10_7100: the Kut-Ku and the hunter behind the reward window]. Test runs
  without drawn frames (fast-forwarded) get no copy and the old backdrop.
- **Hunter hit by monsters** (GB6, Pl_damage_sub / pl_guard_set, game 0x66D300 / 0x66D080): G's skills now act: guard
  skills 75-78 change the hit strength before the guard reaction; skill 110 (1 in 4: 70% damage, flash + sound) / 111
  (130%); knock-down immunity by skill 41 / 107 (type 15), roar / wind immunity by skill 114 / 113 (type 14); damage
  through a guard 1/2 with +0x3EB bits; dm_type 15 falls with its own act; the stun test of sub_main 0x4CC810; the voice
  is not cut when nothing happens. Skill names are not looked up [asm read; build + quest tests].
- **trans_stage** (G sub_main 0x4A22C0): G's per-stage changes: stage 0 not drawn; new layer cases for stages 8, 0xF,
  0x18, 0x1B/0x1D, 0x4A, 0x58, 0x59-0x5B (turning pieces, scrolling water/lava layers, translucent mode 4/1), layers
  hidden on 0x4C (4+) and 0x4D (4+), 0x55's scroll speed; set pieces: 0x18 new, 0x1D uses 0x2D's, 0x4A its own pair
  (set74_pos_tbl, pc_data) [asm read case by case; m2c drafts of both versions diffed].
  Before/after shots with the viewer's default camera (build/A/ts/ba.png, o* = MH1 code, s* = G code): **stage 24 no
  longer crashes** (`mhview_g --stage 24` died with MH1's set-piece code); stage 8 no longer shows MH1's turning piece in
  front of the camera; stage 74 no longer has a hole to the sky; stage 27's layers 2/3 are now drawn additive as G's code
  says (a pale overlay where the PC drew dark rocks) [G's code read, not compared with a PS2 picture: owner check].
  Seen: `mhview_g --stage 88 / 89` crash in load_stage_hit (not from this change).
- **Not real**: "em21 die case 4" (map error, see section 4). **hit_mark_set_new** was already G (row updated).

GB6, quest lane, what is left (docs/survey/mhg_cov_g.txt of 15 Oct re-read against today's code; ranked by how often a
player meets it) [inferred from the asm diff sizes, not run]:
1. pl01_effect_move / ef_move_sub_0024A790 (hunter weapon effects and sounds: G calls two big G-only functions; every hunt).
2. NextStage_Dir_Set (em_cmd, 0.73: where a monster heads when it changes area; every hunt with area changes).
3. Monster effects eft20_pos_set (0.65), eft13_t (0.79), eft09_m (0.75), Eft05 (rewritten in G).
4. Stage sets: set19_trans (GN8, stages 67/73/75), set04_trans, set06 (rewritten), set13_i.
5. Per-monster: em27 em_die04 (rewritten), em08 em_dmg05, em16 / em17 small acts (0.74-0.79), em07 item/part sets.
Done in this pass: Pl_damage_sub / pl_guard_set (was first: every hit on the hunter), trans_stage.

## 9. Real-G reference shots and the GB6 quest lane, fifth pass (agent A, 16 Oct 2026)

**New tool: tools/g_ref_shot.sh** (real-G pictures next to the PC's). The owner's G disc runs in PCSX2 (hidden, PINE
socket on: tools/pcsx2/pine.py reads/writes EE memory and save states). A save state "in a quest" is made once from the
real menus (NEW GAME, name, house, the Village Elder's first quest, the gate) and kept in build/ref/pcsx2home; each
shot loads it and moves the hunter to the stage the way an area exit does (PLW+0x73A/0x73C/0x570/0x738, game2 loads
the area), grabs the screen, reads G's game camera (lpView: eye, target, angle of view) and draws the same stage on the
PC from that camera (`mhview_g --stage N --view eye,target,fov`, new). `--pc-only` redraws the PC side from the saved
cameras after a PC change. ~1 min per stage. Example: `tools/g_ref_shot.sh --out build/ref 27 8 15`.
The scripted creation of the save state was checked from an empty PCSX2 home (NEWSTATE=1, ~4 min: state saved, stage
27 shot taken) [verified]; the comparison pictures below came from a state made by hand with the same steps.

**Comparison** (PS2 left, PC with fog middle, PC before right): build/A/ref/cmp_s08.png .. cmp_s85.png, all in
build/A/ref/overview.png; pairs build/A/ref/sNN.png.
- **Stage 27 (last round's additive layers 2/3)**: the real G has the same pale overlay where the PC drew it: the
  additive layers are right [verified: PS2 picture]. What differed was the fog (below) and overall brightness.
- **Stage fog was missing on the PC (MH1 and G)**: fl gives each clay a fog bit from its material attribute (+0x20:
  0 = fog, aa_fog), stage_fog_set sets the colour / start / end per stage; the PC never drew fog. **Fixed**: per-clay
  fog in the GL and nv2a backends, stage_fog_set at each stage load [verified: 27 and 08 move toward the PS2's bluish /
  grey haze; difference pictures]. The PS2's fog still looks stronger at a distance (stage 8's sky is almost all fog
  colour on the PS2) [the PS2 fog formula in the VU1 code is not read: start/end used as GL linear eye-distance fog].
- **Brighter PC**: stages 27, 15, 29 are about 1.5-2x brighter near the camera on the PC (27: ground 108,111,65 vs
  55,60,39; stage 29 is dusk-dark on the PS2). Stage 8 matches. Likely the stage lighting (light_set / pl_light_tbl
  per stage); the PC's lighting is its own approximation (docs/pc.md "Lighting") [not fixed; next step].
- **Stage 15**: the PS2 has the sun with its lens flare (set13) right in view; the PC's --stage picture has no sun
  [the --stage viewer runs no set objects; not checked in a quest].
- **Stage 24**: below the jungle edge the PS2 shows black (clear colour), the PC its blue clear colour [minor].
- **Stage 74**: the two G set pieces at set74_pos_tbl (7900,0,6030 / 5560,0,6960) are drawn by the PC as dark
  branch-like shapes and are not visible on the PS2 in the same view, although G has the set model loaded there
  (set_mdlw flag 1, 3 clays, read in PCSX2) [open: probably drawn but invisible on the PS2 (UV-scrolled drips);
  not settled].
- **Stages 76/77/85** (town / tavern / guild hall areas): match apart from the effects (candle flames) the --stage
  viewer does not run.
- **8 and 15**: shapes and set pieces match (last round's turning piece on 8 is gone on both).

**`mhview_g --stage 88/89`**: not G stages (both games have stages 0-87; G's stage_hit_data_w is 0 past 87, the file-0
link crashed load_stage_hit). The viewer now refuses stage numbers outside 0-87.

**GB6 quest lane, item 1 (hunter weapon effects and sounds)**: G's pl01_effect_move calls two functions (sub_main
0x4E30B0 effects, 0x4A9F50 sounds) that together hold MH1's ef_move_sub. Running both (a small MIPS interpreter
over the asm, per weapon class 0-5 and every motion number in the scripts) gives the same calls as MH1's except:
anim 801 (G adds an Eft06 flash when PLW+0x8D0 = 4: ported) and the other-players skill section (online only, G
adds skill 115 and five weapon ids: not ported). G's weapon switch has **7 classes: class 6 is the Dual Swords**,
which the PC had no sounds for: **ported** (pl_snd01.c case 6, generated from both kind-6 blocks; checked against the
interpreter for all 3000 motion numbers: 0 differences).
But the PC cannot play the Dual Swords yet: **Battle_type was imported at MH1's size** (0x7C; G 0xE1), so every G
weapon type from 0x7C on (G's new weapons of all classes, all Dual Swords) took its class from unrelated host memory.
Fixed (pc_data.txt); with the right class 6, pl_init crashes (Pl_slash_tbl / mv001_tbl / mv004_chr_tbl* are MH1's six
entries), so job 6 now plays as sword and shield (stand-in, rt_data.c; RT_G_DUAL=1 for porting).
**New gap (Blocker for G's weapon list): the Dual Swords class** (job-6 tables, moves, motions, the second blade's
trail Eft05 arg 2) is not ported. **Done 16 Oct 2026 (agent A, round 13):** the Dual Swords play as G's (attacks, combos,
demon mode on R1, both blades drawn, two trails and the demon mode's red trails and glow, the left blade's hits); the
stand-in is gone. Checked side by side with the real G in PCSX2 (build/A/dual/cmp_combo.png, cmp_demon.png) and in a
G quest (tools/test_all_quests_g.sh dual run). Found on the way: no G-new weapon model (index 124+, any class) was drawn
on the PC (weapon_model_data imported at MH1 size): fixed. Details: docs/agents/agent-A.md "G round 13".

**Item 2, NextStage_Dir_Set** (where a monster heads when it changes area): G picks the side from the signed offset to
the next area (MH1 compared it with w/3, 2w/3 and w/2, so a next area to the left / front always gave side 0 or the
middle). **Ported** (em_cmd_nm.c VERSION_MHG) [asm read; build and quest tests; not watched in a hunt].

**Item 3, monster effects**: **eft09 (the cut tail) ported** (game 0x56E230 / 0x56E3D0 / 0x56E940, eft09.c G
branches): the subspecies (kinds 37, 41-44, 47-49) now leave a cut tail too (MH1's kind list only had the originals),
a cut tail goes away when its monster loses a life (G's revival: the lives count is kept at eft09_set), and a fading
monster's tail is drawn in ot0 [verified: G quest 10 tail-cut run, "em-tail: kind 1 tail cut ... pick 0"; the
subspecies and revival paths: asm read, not run]. Eft05 (weapon trail) is rewritten in G for several trails per
effect (count per type, prims each, helpers 0x56C660 / 0x56C880); it matters with the Dual Swords: not done.
eft20_pos_set / eft13_t: not started.

## 10. Picture match with the real G and the GB6 quest lane, sixth pass (agent A, 16 Oct 2026)

Comparison pictures (PS2 left, PC before = round 12's viewer picture, PC now = a PC quest after the same area change),
build/A/look/cmp_sNN.png, all in build/A/look/overview.png. Mean colour difference to the PS2 picture (0-255 per
channel, HUD corners left out; the hunters still differ: the PS2 hunter wears no armour):

| stage | before | now | what changed |
|----|----|----|----|
| 8 (swamp) | 8.8 | 6.2 | the green screen haze |
| 15 (plains) | 104.4 | 12.8 | the sun's glare (set13 kind 8) |
| 24 (jungle edge) | 31.5 | 23.9 | black below the edge (the clear colour); the PS2's mist over the trees remains |
| 27 (forest river) | 35.5 | 8.4 | blue-to-dark screen gradient |
| 29 (forest at dusk) | 19.7 | 3.3 | dusk gradient (81% dark at the bottom) |
| 74 (cave) | 14.0 | 5.9 | the stage file's own fog and lights |
| 77 (tavern) | 9.2 | 13.0 | candle flames now drawn; the number grew because the quest picture has a hunter and a map the viewer picture had not |

- **"1.5-2x brighter near the camera" was not lighting or fog: it is stage_spr_disp**, a full-screen colour gradient
  the game draws after the 3D scene and before the HUD (four top and four bottom colours per stage, faded by the view's
  angle to the sun). The PC never called it (MH1 too). G reads its data from stage_work+0xC8.., filled by G's
  stage_set_set from per-stage tables [verified: PCSX2 memory reads of stage_work+0xC8.. for stages 8, 15, 24, 27, 29,
  74, 77 equal the PC's RT_SKY_TRACE=1 values]. Fog on the PS2 is the same linear eye-distance fog as the PC's
  [verified: Vu1Code_0019_0002 (the stage's unlit fogged family) computes F = 255 (end - w) / (end - start); and a PCSX2
  run with fog switched off by a memory poke barely changed the PS2 picture], so no fog calibration was needed.
- **Stage file environment block** (G only, func_0010FD10): a stage file with a third link entry carries fog, actor
  lights and (version 2) sun data that replace the tables. Stages 7 (version 2), 66 and 74 (version 1) have one
  [verified: all 88 stages scanned with RT_FOG_TRACE=1; PCSX2: stage 74 fog 0xCFCFCF 200-14000, block version 1].
  Ported: rt_stage_env, G stage_fog_set (matches G), light_init's G override.
- **Sun glare on stage 15**: G's stage_set_set (matches G) spawns set13 kind 8 there, a G-new kind (three
  camera-facing layers scrolling at their own periods); G's set13_i (new stage cases 7, 0xF, 0x1D, 0x59-0x5B) and the
  kind-8 parts of set13_m / set13_trans are ported (PC C, set13 is still asm on the PS2) [verified: cmp_s15.png].
- **Candle flames on 76/77**: G's set06 (matches G): the candles are parts of the area model, fading as the camera
  comes close; the tavern's flame flickers [verified: cmp_s77.png].
- **Stage 24's blue**: the PC cleared the 3D view to a fixed blue; the game clears to black (setBGcolor(0)) [verified:
  cmp_s24.png]. Also fixed: an area change to a stage without a set model (G 24) left set_mdlw on freed clays (crash).
- **Stage 74's two set pieces** (round 12): not visible on either side from the area-change camera; the PS2 cave was
  brighter because of the environment block, now matched. Not looked at from other cameras.
- **Hunter brightness** (the PS2 hunter is darker on 27): RT_LIGHT_GAME=1 (the game's own stage lights) makes the PC
  hunter darker too (build/A/look/lg/cmp27.png); the PS2 hunter has no armour, so not settled [open].
  27 Oct (agent D, lockstep pictures with the same armour): the PS2 lights equal the PC's RT_LIGHT_GAME lights (read in
  PCSX2); the fixed default light is too bright at the camp and too dark in the desert, the game lights match the desert
  and are too dark / directional at the camp. Still open; findings and the lead (the VU program's per-bone light matrix)
  in docs/agents/agent-D.md "Renderer score: fixes". FIXED later on 27 Oct: the host lit actors with world light
  directions on normals in the actor's own space (lights turned with the facing); docs/pc.md "Actor lighting checked
  against the PS2". The far-sand softness of the same scoreboard was the PS2's 512x448
  frame and line merge, not lighting or mip levels (docs/pc.md "PS2 output").
- G action 0x67 (Dual Swords) now runs G's pl_at024 arg 3 (matches G).

**GB6 quest lane** (section 8's list), done this pass:
- set19 (stages 67/73/75 = GN8): set19_i / set19_trans match G. G reads those stages' positions from never-filled
  tables (FINDS.md), so the pieces MH1 showed there are gone in G; the PC now does the same. Stages 11/12/21's masks
  were read from the wrong addresses (mhg_map name slip): fixed.
- set06: matches G (above). set13_i: ported (PC). set04: not done.
- eft20_pos_set: G's kind 0x24 (as kind 2), x07 10-13, arg 10's new joints, args 13/21, the nine kinds' shell01 hits
  (PC C, eft20_nm.c). eft13_t: G's types 35-39 (matches G); eft13's per-type tables imported at G's 40 types.
- em27 em_die04: **not ported** after all: G's version only checks the revival (no Quest_enemy_revival_set, no
  reposition); G calls Quest_enemy_revival_set elsewhere, so on the PC (MH1's revival flow) the Velocidrome family
  revived forever and 8 G quests failed in tools/test_all_quests_g.sh; reverted to MH1's [verified: the failing run].
  em08: G's damage act 0x14 had no handler on the PC (em08_main already
  set it): the Cephadrome family's flinch after a part-3 break could hang in mode 4 [inferred from the code, not run]; fixed with G's em_move04 and
  em_dmg05 side argument. em16_to_normal: G's per-kind action speed.
- Not done: eft05's non-Dual-Swords uses (G's eft05 already runs for every weapon; not compared with the PS2), em17's
  small acts, em07 item / hagi sets, set04.

## 11. Lockstep referee against the real G (agent D, 17-18 Oct 2026)

tools/referee.py (docs/referee.md) plays the same quest with the same pad on the owner's G disc in PCSX2 and on the
PC port, tick by tick, and compares hunter, monsters, pouch, quest state and random numbers. Scenarios run: walking in
the Elder's first quest (10001); sword and shield, great sword, hammer, lance, Dual Swords, a bowgun (quest 1, Forest
and Hills area 1); fights with the Rathian (quest 10, area 40) and the Yian Kut-Ku's quest (quest 2, area 36).
All runs [verified: build/referee/<scenario>/report.txt, pair_*.png; not committed].

**How in sync it is now.** Weapon scenarios (600 play ticks, every weapon class): hunter, monsters (incl. the
quest's own monster in quest 1), pouch and quest state identical every tick; the only differences are random-number
draws by the sound layer (channel 1, one tick apart; no gameplay effect) and the Dual Swords walk start below. Fights:
identical for 186-369 ticks (Rathian 203, Kut-Ku quest 369), then the first difference comes from N-R1 below; after
it the runs drift apart as expected (another hit lands, another knockback).

**Fixed (both games where shared; each verified by re-running the referee's PC side against the same PS2 trace):**
- R-F1 *port glue, both games*: the Scheduler's per-frame `ran_suu(1)` was missing after the boot (rt_boot.c
  rt_sys_frame_start): random channel 1 fell one draw behind every tick.
- R-F2 *port glue, both games*: stick angles were truncated (269 instead of 270 degrees for straight down): the hunter
  walked one degree off (rt_pad.c rounds now).
- R-F3 *port glue, both games*: monsters ran in the order 1..19, 0 (the PS2: 0..19): random draws in another order,
  small monsters placed elsewhere when an area loads.
- R-F4 *port glue, both games*: game_core's order: hit_check ran before the monsters moved and body_hit after the
  effects; a monster's attack (the Rathian's roar) reached the hunter one tick late. Now player, monsters, body_hit,
  effects / sets, area exit, HUD, then the joints and hit_check, as move() / trans() / hit_check().
- R-F5 *port glue, both games*: the quest start: the PS2's game13 runs one move() of the world and then spawns the
  stage's set objects; the PC started game2 at once (and spawned the sets before). Monsters and the hunter were
  three ticks behind at the start of every quest; now the PC also has the game13 tick (rt_flow.c, rt_game.c).
- R-F6 *port glue, G*: em_dur_tbl's rows (part durabilities) were relocated as pointers by the pointer scan (0x0078
  0x0050 read as G address 0x500078): the Yian Kut-Ku-sized kind 20 had a part with 22849 HP and one missing
  (config/mhg/pc_data.txt noptr). Other s16 tables could have the same fault: worth a sweep of pc_ptrs.txt entries
  that fall inside s16 tables.
- R-F7 *G still on MH1 logic*: body_hit_sub_em (monster against monster) uses G's own sphere table (game 0x673260;
  the Rathian 300 instead of the wall table's 500): small monsters were pushed off the Rathian (pl_nm.c).

**Second round (agent D, 18-20 Oct 2026; the referee's PC side re-run against the same PS2 traces):**
- R-F8 *platform, both games* (owner's decision 18 Oct, DECISIONS "PS2 float rounding on by default"): PS2 float
  rounding is the default (rt_ref.c fpu_ps2; `RT_FPU_PS2=0` turns it off): SSE rounding toward zero, flush to zero,
  denormals-are-zero only where the CPU has it (MXCSR_MASK; the Xbox's Pentium III has no DAZ and ldmxcsr with it
  would fault). The game C is SSE code (build_pc.sh / build_win.sh / build_xbox.py); the host's own float code on
  32-bit x86 (renderer, mixer, libm) is x87 and keeps round-to-nearest, so nothing had to be switched per tick. Each
  quest start logs "fpu: mxcsr ..." and sets the mode again if it changed. Test round, rounding off vs on [verified]:
  the title, logos and opening movie's sound byte-identical; stage pictures identical; G's reference shots of 4 stages
  (tools/g_ref_shot.sh --pc-only) differ in < 0.01% of pixels; fights differ only as the game takes another path;
  full_check all ok (test_activities' trader needed another seed: its offer depends on the hunter's path); Windows
  build under wine and the Xbox G / MH1 builds in xemu reach a quest start with the mode set (mxcsr FFE0 in the logs).
- R-F9 *port glue, both games*: N-R1 below was not root motion. The PS2's move() builds each monster's joint matrices
  right after it moves (enemy_mk -> flCalcTransSI) and the hunters' after body_hit (player_mk); the PC made all joints
  at the end of the tick, so body_hit pushed hunter and monster apart with last tick's joints (found with a gdb
  watchpoint on the hunter's position: body_hit_sub_new, a jumping Velociprey). Now rt_monster_tick builds them after
  enemy_mv (rt_em.c, em_pose_now) and sim_tick after body_hit (viewer.c); hit_check uses those. Rathian 203 -> 245,
  MH1 Rathian 241 -> 291.
- R-F10 *G-only feature*: N-R3, the Elder's tutorial, is ported for both games (game tuto.c, tutob.c, tuto_nm.c in
  the PC build; SonchoMove / DispTutorial behind Pit_mv's / trans_pit_1's calls by address, rt_menu.c; SonchoInit at
  the HUD set-up of a training quest, rt_quest.c). G's differences written from G's asm (game 0x66E270..0x66EDE0):
  its training quests 10001.., 7 lessons in a sub-list, the texts through a pointer table (sub_main 0x5282F0) and the
  frame title as text (pc_data.txt tuto_str_tbl / tuto_title_str) [verified: screenshots of the sign, the list and a
  page in G 10001 and MH1 131; the PS2 reference shot shows the same sign].
- R-F11 *G still on MH1 logic*: N-R4: G's pl_mv001 (game 0x54F830) blends the Dual Swords (kind 6) into the walk
  start over 6 like the great sword; MH1's code gave them the default 4 (f_pl.c). The ds scenario is identical now.
- R-F12 *port glue, G*: the sweep R-F6 asked for: 94 words of pc_ptrs.txt were numbers (s16 pairs in the weapon tree
  bou_sei_tbl and the forge's kakou_tbl, the yes/no screen's sprite positions yn_spr_data, push00, key codes, colour
  words in the effect tables). They are listed in config/mhg/pc_noptr.txt, which tools/mhg_pcmeta.py leaves out of
  pc_ptrs.txt. Found as scan-only words next to small numbers in objects whose MH1 twin has no relocation there.
  MH1 is not affected (its ELF has real relocations).

**Third round (agent D, 20 Oct 2026; same method, both games):**
- R-F13 *port glue, both games*: move()'s wall pass after body_hit (HitWallPlayer(.., 1) for every hunter in an online
  game or with PLW+0x7EC set, and every monster with +0x7EC set) was missing in the host's tick: a hunter pushed into
  a wall by a monster stayed in it for a tick and the next push started from there (viewer.c). Kut-Ku 622 -> 656.
- R-F14 *port glue, both games*: CameraMove ran right after the player; move() runs it after body_hit, the wall pass
  and player_mk, so the camera (and the stick's direction for the next tick) was a tick behind after a push. Kut-Ku
  656 -> 780 / 804.
- R-F15 *platform, both games*: the PS2's VU0 library code is now written out as the PS2 does it: flPS2SinFast /
  CosFast (range reduction + polynomial, sine as +-sqrt(|1 - cos^2|), constants at_tbl / SS5432 from the disc), the
  motion curves' Hermite and linear formulas (flFCVFcurveInterpolate*, flFCVGetValue2's "unknown interpolation gives
  0"), and VU0 operations round to nearest (rt_vu.h) while the game's FPU code rounds toward zero; the fl maths, the
  motion player and the curve reader are built with SSE math like the game C. Measured: the hunter walking at facing
  1 now drifts sideways as on the PS2 (libm's sine gave none), small monsters' facings stay identical ~20 ticks longer.
- R-F16 *platform, both games*: flArcTan2 is the PS2 C library's single-precision atan2f / atanf (newlib's, read from
  the asm: its own pi / pi_lo constants, tables from the disc), not libm's.
- R-F17 *port glue, both games*: the hunter's joint matrices for the game (body_hit, hit check, effects) carried the
  host's drawing offset that puts the model's feet on the ground (0.3 units low; REF_NODES -1 showed it).
Rathian 245 -> 286; MH1 Rathian 291 -> 365.

**Fourth round: G's other monsters, items, gathering (agent D, 20-21 Oct 2026).** New referee scenarios
(tools/referee.py): g_rathalos (quest 10017), g_gypceros (19), g_basarios (23), g_gravios (26), g_diablos (14),
g_monoblos (10041), g_khezu (25), g_plesioth (11), g_cephadrome (13), g_kind37 / g_kind44 / g_kind49 (G subspecies
quests 10038 / 10042 / 10033), g_lao (10048): the hunter is put next to the monster with a great sword and the FIGHT
pad for 450 ticks; g_items (potion, whetstone, paintball, small barrel bomb, pitfall trap by pouch pokes next to the
Rathian); g_gather (herbs, mining with a pickaxe, the bug net). Fixed:
- R-F18 *G still on MH1 logic*: G's monster needs housekeeping in em01_main / em15_main / em20_main (G's difficulty
  and quest lists, the countdown-only hunger game 0x547C20 on most quests): a Gypceros's hunger counted down on the
  PS2 only, and its next move differed at tick 49 (em20_ai_nm.c, em01_ai_nm.c, em15_nm.c, em_core_nm.c).
- R-F19 *G still on MH1 logic*: G's em03 / em04 / em12 / em19 / em29 inits apply the monster's size to small monsters
  (base scale at EMW+0x600, em_size_scale_set): those small monsters were drawn and hit-tested at size 1.0 (a kind 19 at 1.0 instead of 0.8) (f_em_nm.c).
- R-F20 *port glue, G*: more numbers taken for pointers (config/mhg/pc_noptr.txt): em_vital_tbl rows of kinds 12 and 19
  (small monsters spawned with 32648 or -12408 hit points) and Item_get_tbl rows 99 / 100 / 143 (the bug net gave item
  0xF645). The earlier sweep had filed these words under a wrong object name (st43_sun_rgba_data).
- R-F21 *port glue, both games*: monster necks did not turn on the host skeleton: enemy_mk's mlCalcTransEM turns the four
  neck joints of kinds 1 / 7 / 11 (G also 37 / 41 / 42 / 49 / 50) about Y toward the target; the host drew and
  hit-tested heads straight ahead (fl_skel extra turns, rt_em.c rt_em_neck).
- R-F22 *port glue, both games*: motion blends: the PS2's flBlendMotionEx mixes each node's local matrix (rows x and y
  mixed, normalised, z = x cross y, y = z cross x: flmatBlend); the host lerped Euler angles, which bends differently
  (a Rathalos's wing joints 18 units off mid-blend). Now the same matrix blend (fl_model.c); REF_NODES showed the
  joints within 0.01 through the blend afterwards. Affects every hunter and monster blend, both games.
- Referee line-up aid: each monster's action random number (EMW+0x39A) as the PS2 has it at play tick 0 (the first
  actions are set while the area loads, before the PC gets the PS2's random state).

| scenario | first gameplay difference after the fixes |
|---|---|
| g_rathalos | 157 (was 63): a choice in its attack program |
| g_gypceros | 114 (was 49): a small monster (kind 5) facing |
| g_basarios | 106 (was 32): a kind-19 small monster's height while flying |
| g_gravios | 48: the Gravios ends its turn one tick early and attacks (open, below) |
| g_diablos | 337: red-health recovery tick (N-R9) |
| g_monoblos | 33: a small monster (kind 13) picks another first move at the area start (open) |
| g_khezu, g_cephadrome, g_lao | none in 450 ticks |
| g_kind44 | 62: the subspecies drifts 2 units |
| g_kind49 | 33: the PC hunter walks into an area exit at its poked start (scenario position) |
| g_items | 103: red-health recovery tick (N-R9); items used identically |
| g_gather | 44: Aptonoth drift 2 units; gathered items identical (herbs none at that spot on both, ore, bug) |
| kutku | 909 (was 804) |
| g_plesioth, g_kind37 | no comparison yet: the PS2 quest did not start / stopped (referee scenario problem) |

Open from this round: N-R10 (G) Gravios turn end (em17, tick 48: the PS2 runs its command program a tick later; the
inputs compared are equal); N-R11 (G) kind 13 first action (act speed 1.2 on the PS2 at spawn, 1.0 on the PC: G's
em16 act set); N-R12 the village day (shops, box, forge) is not compared: the lockstep hook only runs in quests.

**Fifth round (agent D, 21-22 Oct 2026):**
- R-F23 *G still on MH1 logic*: the monsters' first-encounter story scenes test MH1's quest numbers (0x87 ..); G's quests
  are 10005 .. (rt_quest.c rt_story_quest maps them, em01 / em04 / em12 / em16 / em19 / em20 / em27) and G added scenes
  of its own (em03 10008, em04 / em09 / em20 10028, em15 / em19 10046, em16 10035), written from G's asm. g_monoblos:
  the Velociprey-type monsters of quest 10041 now play their scene: first difference 33 -> 219.
  The scenes' own act 6 code is G's too (em04 6/1, em09 6/0, em15 6/0, em16 6/6, em19 6/1, em20 6/1: each waits for
  G's demo flag; MH1's code there waits for MH1 flags and kept the monster in its scene for the whole quest: quest
  10046's Khezu never fought back, test_all_quests_g caught it). em04's quest check lives in em04b.c (the built file;
  em04_nm.c is a weak copy). g_scene_khezu (10046): 450 ticks, no difference; g_scene_rath (10028): the Rathalos scene
  identical for 1500 ticks, the Velociprey 33 -> 1151 (then a facing step of 0x100, open).
- R-F24 *G still on MH1 logic*: em_eye_search_set's eye joint for G's new kinds (37 41 42 49 like the Rathian, 38 39 43-48
  like their base monsters, 36, 40, 50): the subspecies looked out of their body and saw the hunter early. g_kind37
  44 -> 284 (red-health tick only), g_kind44 no difference.
- R-F25 *port glue, both games*: Game_task's frame counter (game_w+0x1E) and the play time (User_data+0x374, G +0xC1C:
  the save screen's TIME) now count every game tick in every mode; the PC counted the frame only during quest ticks and
  the play time never (rt_game.c rt_game_task_tick).
- Referee: village lockstep (the hook locks in the village's game mode, G: 0; the PC starts in the village with the
  PS2's User_data: RT_REF_MODE / RT_REF_STAGE / RT_REF_UD_OUT) and the saved hunter compared at the end:
  g_vil_shop (an item bought), g_vil_wshop (a weapon bought), g_vil_forge: User_data identical but for the play time
  (the PC run also counts its own start-up ticks). g_carve: the Rathian at 1 hit point killed and carved: the same tick,
  the same part on both sides (mh1_carve: the same gathering). Line-up aids added: the monsters' needs counters and G
  size at play tick 0.
- Still open: N-R10 Gravios notices the hunter a tick early (its head joints 31-39 are up to 20 units from the PS2's
  although every motion layer, frame and blend agree: likely a node order or a head override on the PS2 side, not found);
  N-R12 the reward screen is not compared yet (the lockstep only runs in the quest and the village; the clear / result
  modes load files at their own pace).
- R-F26 *G still on MH1 logic* (22 Oct): the forge's "equip it now?" starts on "yes" in G (G lobby 0x54CC8C; MH1 "no"):
  g_vil_forge_ore's saved hunter now identical but the play time. N-R14 closed: above 1000 G pushes kind 19/24 fliers
  down 20 a tick instead of stopping them, and em_fly05/09 have no ceiling (em19b.c; g_monoblos no difference,
  g_basarios 106 -> 144). Kind 12's base scale is em12's own table value (G em12_init; g_gather no difference).
  N-R13 closed: G's Em_direct_set gives kinds of em_tsuushin_tbl their quest stage before their init (sub_main
  0x4B5714): the Plesioth's init took the hunter's stage for its own and left the water at once (g_plesioth 0 -> 436).
- R-F27 *port glue, both games*: the area-exit check skips an area's first game tick, where the PS2's move_stage runs
  stage_i (viewer.c): a hunter put on an exit left a tick early (g_kind49 33 -> 409).
- Referee: the PC takes the PS2's frame counter (game_w+0x1E) each play tick with Rnd_w (the PS2 also counts its
  area-load frames): red-health recovery and the frame-timed effects in step. g_kind37, g_diablos, mh1_kutku: no
  difference left in 450 ticks. Still N-R7 class: g_rathalos 157, g_scene_rath 1151, g_items 297 (a monster with its
  target almost straight behind turns the other way after a one-float-step drift).
  G's offline quests have no Kirin, Rathian / Rathalos subspecies other than kinds 37 / 44 / 49 (the quest dry runs).

**Left, ranked by how much a player meets it:**
1. N-R7 (both games): position differences of one float step (0.001 units at 10000) appear on the first tick of some
   root motions (a monster starting to walk); they grow into 1-2 unit drifts after 200-300 ticks and change a push or
   a facing later. Not traced further; the PS2 side is PCSX2's emulation, whose last-bit rounding the referee cannot
   tell from the real console's.
2. N-R9 (both games): the hunter's red-health recovery ticks on game_w+0x1E, a frame counter that on the PS2 also runs
   while an area loads (dozens of ticks, one on the PC), so the recovery comes 1-60 ticks apart: the same cause as the
   quest clock (owner, 18 Oct: left as is).
3. N-R5 (both) *sound layer*: the PC's sound requests draw their random number one tick later than G's flSndRequest /
   flSndChange (channel 1 only; sound pitch variation; no gameplay effect).
4. N-R6 (both) *referee path only*: `--quest` runs show a default hunter (armour, no name in the HUD) instead of the
   save's look; the real village -> quest path uses the save. Not a player bug; it makes the referee's pictures differ.
   **Fixed 25 Oct 2026 (agent D)**: with RT_REF_UD (referee and tools/render_score.py runs) rt_player_game_init runs
   Set_userdata + armor_create_model: the save's name and look [verified: render score pictures, G and MH1].
(N-R1..N-R4 of the first round are fixed: R-F9, R-F8, R-F10, R-F11.)

## 12. Cross-game audit (agent A, 18 Oct 2026; docs/audit_cross_game.md)
Every G-only / MH1-only change since 13 Oct checked against the other game. For G:
- Debug menu: "Can't faint" now holds against any hit (30000 while the hit is taken, then the hunter's maximum; was 100:
  a high-rank hit over 100 fainted the hunter); the RT_PL_GOD aid's life bar no longer runs off the screen
  [verified: G quest 9 sweep run, shot]. "Hunter rank" and "Mark all offline quests cleared" rows now on G too (rank at
  User_data+0xC23; the 98 offline quests) [verified: tools/test_dbgmenu.sh].
- Quest start: the "save has a pouch" test reads G's pouch (+0xC24), was MH1's offset (no visible effect found).

## 13. The quest lane, seventh pass (agent A, 20 Oct 2026; details docs/agents/agent-A.md "G round 15")
PC-only round: all G code below is C written from G's asm (VERSION_MHG), not byte-checked (check.py needs wibo).
Each was compared with an m2c draft of the G asm against the MH1 draft / the PC C, the odd places read in the asm.

**Section 8 / 10's list, done:**
- **set04**: G's set04_i / _m / _trans. G's new cases are for stages 0x59-0x5B, which G's 88-entry stage tables do not
  have (FINDS.md); what changes in practice is the default branch and the first-frame draw.
- **em17 small acts** (Gravios / Basarios): G's attack table (em_move03, 12 entries; MH1's atk07 is unreachable in G),
  a new jump attack, beam variants (Shell08 types 12 / 13), acts 12 / 14 / 18 / 23 / 26 (the 5% heal now after 600
  ticks), dmg00 / 04 / 11, the quest 10043 demo (G rewrote it), type-1 sounds, the sound/effect script's new motions
  and the Black Gravios (kind 0x2F) in the Gravios checks [verified: hunts of quests 23 / 26 / 10043, every attack
  returns to normal; test_all_quests_g].
- **em07 item / hagi sets** (Lao-Shan Lung): G's item ids by kind and quest rank (MH1's ids are other items in G's
  list) [asm read; quest 10048 passes; the items themselves not looked at in a hunt].
- **eft05 for the other weapons**: nothing to port: G's callers pass the same arguments (args 2 / 3 are the Dual
  Swords'), G's Eft05_set only adds the demon-mode skip [compared by code, not with PS2 pictures].
- **em27 die04 the way G does it** (Velocidrome family): G moved the revival into em27_act_set (die act 4:
  Quest_enemy_revival_set, em_status_init, the start point), die04 only picks the act. Porting both together fixes
  last round's endless revival [verified: quest 7: three kills with two revivals, then clear; 7 / 15 / 24 / 32 / 42 /
  10014-10031 pass]. Also G's em27_main (the monster turns by the hit's side before a flinch or fall), die03 / dmg03
  motions, atk06's variant and G's attack 14.
- **eft20** (monster dust, splashes, breath dust): G's eft20_i / _m / _t (kind 0x24, the nine kinds' landing hits,
  args 13 / 21 for Gravios motion 0x45A with G's fade keys, arg 29 scaled) [asm + draft compare; 13 / 21 not seen].

**Beyond the list (ranked by how often a player meets them):**
- **em01** (Rathian / Rathalos and G's variants, kinds 0x25 / 0x2A / 0x29 / 0x31): the variants now take their
  originals' paths everywhere (MH1's code only knew kinds 1 / 11), G's em01_main (status checks by G's quest numbers,
  hunger countdown, demo stops), the airborne fireball attacks (atk09 / 10 / 17 / 22) as G, four new attacks (x15
  26-29), flight curves 15 / 16, the sound/effect script [verified: hunts of kinds 11 / 1 / 49 / 37, 400 s each;
  13 em01 quests pass; x15 26-29 not hit in those hunts].
- **em16** (Velociprey family): the same hit-side turn as em27, G's demo-stop quests and the new quest-10035 act
  (hidden during the Plesioth opening scene), init speed per kind, a new attack, kind 35.
- **Monsters with x01 = 0 were drawn on the PC (both games)**: the game only queues a monster's prim when x01 != 0;
  the PC's monster drawing ignored it. Found through em16 in quest 10035 [verified: build/A/q10035b pictures].
- **shell08's per-type tables** imported at G's 14 types (G's new types 12 / 13 read past the PC's copies).

**Left, ranked by how often a player meets it** (coverage of the whole quest suite, build/A/cov2/report.txt, then
checked by hand; "real" = G's asm differs in behaviour, not only in registers / addresses):
1. **shell08_m / _i / _trans / _impact_set / _h** (fireballs and beams of Rathian, Rathalos, Gravios, Basarios): G
   reorganised them (types 12 / 13 join the 0/11 and 2/3/9 groups, a sound for type 12, a different ground test,
   changed timings for old types). Still MH1's code on the PC; the biggest real gap left in a hunt.
2. **eft13** (smoke / dust puffs: eft13_set_pos_em, _m, _i, _set_sub_em; 0.86-0.93 similar), **eft04_m**,
   **Eft04_set_time**, **eft02_t**, **eft06 type 2**: effects on every hunt [diffs seen, not read case by case].
3. **em20_main** (Kut-Ku / Gypceros), **em14_main** (Diablos / Monoblos), **em09_main**, **em04_main**, **em03_init**,
   **em02** (Fatalis: init, hagi, fly curves): per-monster G edits like the ones ported this round.
4. **shell06** (init_sub / set), **shell04_m**, **shell14** (pos_get / m00), **set14_trans**: monster shells and a set
   piece.
5. **GetWallHitBitPl / Em / 2** (walls): flagged at 0.88-0.92 similar, but what differs looks like register and stack
   allocation; to be read before calling it a gap.
6. **em01_init's first act** (G no longer sets act 0.1 itself; left as MH1's) and **em27_init / em16_init**'s G
   motion helper (0x549FE0): small start-of-hunt differences.

## 14. Xbox: the Elder's quest list crash (agent A, 21 Oct 2026)
- Owner's bug (real Xbox, G, any save): the Elder's urgent quest -> next -> crash. Reproduced in xemu (access violation in
  vsprintf on the quest sheet). Cause: `int Lb_get_quest_type();` for an s8 function; clang returned garbage upper bytes
  (type 16385), the quest sheet used it as an index into its string table. Fixed (G and the MH1 callers); verified in the
  XBE's code, xemu re-run done 28 Oct (agent F: the test_urgent_g.sh script in xemu, G nv2a build of main 842bf3cc: Elder ->
  urgent 10001 accepted -> departed -> cleared -> village at star level 0, no crash) [verified, xemu]. New: tools/test_urgent_g.sh (board -> depart -> clear -> star level 0) in the full
  check; tools/check_kr_returns.py lists the other declarations with the same hazard (to be gone through).

## 15. Owner's Xbox report, G build 2305c47e: saving, shop icons (agent A, 21 Oct 2026)
- **Saving "does nothing"** (bought armour + weapon, equipped, bed save in the house, debug menu "Title screen (soft
  reset)"): NOT reproduced on the PC. The same sequence on PC G (equip blade 2 from the box, bed save on a new card,
  debug menu +2000z (not saved), soft reset, CONTINUE -> slot 1 -> hunter -> village) brings back blade 2 and the saved
  0z: the save is written and CONTINUE reads it (not a stale copy). New tools/test_save_g.sh runs it (full check).
  The owner's log cannot tell what happened: the Xbox card layer (mc_xbox.c) logged nothing, and after the reset the log
  never reaches "load finished" (about 80 s in the title / load screens, then the log ends). mc_xbox.c now logs every
  file saved or loaded ("memory card: saved/loaded NAME, N bytes", as the PC), a failed open / short write / failed
  close, folder creation and deletes; the next owner log will show whether the bed save reached the disk. Untested on
  hardware and in xemu (no memory / xemu in use by another session). Found on the way: a G save of an RT_VILLAGE_START
  test hunter loaded as "no character data" (slot byte 0, set at character creation, was 0); rt_village.c sets it.
- **Shop list icons**: the weapon shop's list showed the neighbouring icon (a sword as a block shape) until the
  detail view, which draws its own. G's icon sheet has one more cell before the weapon icons: Lb_put_job draws e + 0x19
  in G (MH1 + 0x18), and its small-sheet branch maps jobs 5 / 6 to 1 / 4 (G 0x593100). Fixed in lb_ag01.c; checked on a
  PC G screenshot (great sword, hammer, lance, sword-and-shield icons). The item shop's icons were already the same as
  MH1's. PC texture slots stay MH1's (0x118 / 0x157: the PC's load_pit loads them; G's 0x13F / 0x180 are the PS2's).
- **Follow-up (owner's log 20261021-194604):** the bed save was written ("saved ... 78032 bytes" at t4162), then Back +
  Start -> loader -> G -> CONTINUE loaded the old save. Likely cause: the bytes sat in the Xbox kernel's file cache
  (fflush only hands them to it) and never reached the disk before the console moved on. mc_xbox.c now writes each
  card write through (NtFlushBuffersFile, as the log), again at close, and xbox_log.c flushes any open card file
  before leaving. The save / load log lines now carry a check value (FNV-1a of the bytes moved: the next load must show
  the save's value) and, for a save, the money and wielded weapon of the hunter in play. Both games. **xemu, 28 Oct
  (agent F, tools/xemu_relaunch_test.sh --save):** the owner's path (box equip, bed save, Back + Start, loader, start
  again, CONTINUE) loads the bytes saved (same size and check value, MH1 and G), and a power cut 3 s after a G save keeps
  it on the disk [verified, xemu]; the real console's file cache is the same kernel code, so only the owner's run is left.
- **Follow-up 2 (owner, 20261021-200621): worn equipment and play time lost.** The save copied only MH1's 0x480 bytes
  per hunter slot, at MH1's slot stride (src/main/ud/udmisc_nm.c Load_userdata / Save_userdata, the PC's copy of
  f_ud): money, item box and equipment box (all below 0x480) came back, the pouch (+0xC24), the worn set (+0xC75..) and
  the play time (+0xC1C) did not. Now UD_SLOT (0xE00 in G). Same stride fix in the character select's slot checks
  (src/select/edit_nm.c). The soft-reset test had passed only because the old hunter was still in memory; test_save_g.sh
  now also starts a new process and CONTINUEs (blade 2, pouch, play time > 0), which failed before the fix.
  Open: edit_nm.c (G's character creation / select on the PC) still writes option_w[0xFCE] (MH1's "last slot" byte)
  and sets a new hunter's fields at MH1 offsets; G's own locations not checked.
- **Reward screen "no way to skip"**: G's reward screen opens on a two-line menu (receive the reward items / end
  receiving); ending it is how the 120 s wait is cut short. The menu was never seen: the G import took its frame's
  first word (s16 x, y = 0x20, 0x28) for a pointer and relocated it, so it was drawn far off screen. Now in
  config/mhg/pc_noptr.txt; checked on a PC G screenshot (menu and cursor shown). A scan of the other relocated words
  for the same frame layout found none.
- **New hunter fields / last slot (edit_nm.c)**: G's user_data_copy (select 0x544870) puts the fields at G's places
  (0xC7F, 0xD00-0xD08 = -1, 0xC95, 0x1C, 0xC23, 0xD0A); the last-chosen-slot byte is option_w+0x2C50 in G (MH1's
  0xFCE lay inside G's slot 1). test_save_g.sh passes; a G new-game run was not done.
- **Supply box cursor**: not reproduced; a scripted run did not get the camp box (stage 21, 9500,9500) to open.

## 15. GB6, eighth pass (agent D, 23 Oct 2026)
Method: docs/survey/mhg_cov_g.txt (945 entries of 15 Oct) re-read against today's code with tools/cov_report_g.py's own marker test (a copy
that takes the names from the old list): 833 still show no G marker. That count is an over-estimate, as the village pass already found
(section 7's note): the marker test misses UDO / IBO / UD_WARE_N / list: tables, the map pairs some functions by position (Get_atk_value
is G's Get_equip_value, already ported), and a function can be partly G (shell08_i had G's attack-data lines only). Ranked by how often a
hunt or the village reaches it x how different (low `sim` first), then each candidate read with tools/gdraft.py (m2c of G's asm) against the PC C
or tools/draft.py of MH1's asm, odd places read in the asm. All new code is `VERSION_MHG` (matched files also `!__MWERKS__` where G's PS2
build compiles them), the MH1 branch kept in `#else`; MH1's own C is unchanged, so the MH1 PC and PS2 builds are the same as before.

**Ported**
- **shell08 (fireballs and beams: Rathian, Rathalos, Gravios, Basarios, Lao-Shan Lung...), section 13's biggest "left" item.** shell08_m, _i, _trans
  were MH1's code (agent A had only imported the per-type tables). Now G's: types **12** (a shot falling from 5000 above the monster at one of
  24 start offsets, table 0x6C6130, a sound as it comes down through 3000, impact kind 0 only, no fade) and **13** (the 2 / 3 shot at double scale,
  effect scale 2); G's per-type time-limit and index-step tables (0x6C5D20 / 0x6C5D80; MH1 had them as constants per case, type 5's step was
  uninitialised); type 7's streaks update every tick (G's time limit 0); **type 4 (the Gravios / Basarios beam): the length of state 2 is the
  shell's x06** (G's em17 passes 20 / 45 ticks; MH1's fixed 17), the shot lives x06 + 31 ticks at most, the states change in place (G) instead of
  one tick later through a flag; shell08_trans: types 12 / 13, type 5 drawn with rview_matY. [verified: hidden hunts of quests 26 and 10043 pass;
  the beam's trace (RT_ temp trace, removed) shows states 0 / 1 / 2 / 3 lasting 12 / 8 / x06+1 / 10 (cut by the 76-tick limit) ticks; a temporary
  forced spawn of types 12 and 13 ran without a crash, type 12 falls 5173 -> 823 in 30 ticks. Types 12 / 13 / 7 / 9 were not seen in an unforced hunt.]
  config/mhg/pc_data.txt: shell08_time_g, shell08_step_g, shell08_type12_ofs_g.
- **eft13 kinds 35-39 = the Dual Swords' effects** (Eft13_set_scl in pl_snd01.c). eft13_i / eft13_m only knew MH1's 35 kinds, so the pieces were
  created without positions (the draw code, eft13_t, was G's since round 11). Now G's init / move for 35 (six pieces rising from the hunter,
  tables 0x504B58 / 0x504B68), 36 (two mirrored pairs, 0x2EC310), 37 / 38 / 39 in the existing groups, eft13_set_pos cases 35-39 (and case 6 of
  21). [verified: hidden Dual Swords hunt (quest 10, test_all_quests_g DUAL=1), shot at 3.0 s: the red ring of demon mode appears around the
  hunter, absent in the same run with the old code (build/D/e13/before_3.0.png / after_3.0.png); kinds 36-39 were not reached by the scripted attacks.]
- **eft13_set_pos_em / eft13_set_sub_em as G**: G dropped the monster kind lists (every kind not named takes the joint + ground position; MH1's
  code set nothing for kinds it did not list, so the subspecies' dust / smoke puffs had no position) and lengthened the others to kinds 35, 37-50;
  case 19 no longer falls into 20; case 7 asks game 0x54A290's kind list for shell01_set2 argument 0x2F. [asm + m2c diff; not watched on a subspecies]
- **Lb_get_armor_num** (armor shop's owned count): the loop runs over the box's 100 / 200 / 300 pieces, was 64. [asm read]

- **G's subspecies in the PC's monster checks** (the PC's C is MH1's, whose kind lists end at 34; G's kinds 35-50 are the subspecies: 36 = 2, 37 / 41 / 42 / 49 = 1 / 11,
  38 = 6, 39 = 20, 43 / 44 = 14 / 26, 45 = 15, 46 = 21, 47 / 48 = 17 / 22, 50 = 7). Found by diffing every kind constant 35-50 in G's m2c drafts of the
  list's functions against the PC's bodies (a scan, then each hit read): **em_area_move_init** (the stay / run-away timers of the area change were never
  set for kinds 37-50: the subspecies' area changes ran on zero timers), em20_local_area_move_init (38 uses 6's run-away table), **senko_ck** (the flash
  bomb: G's list of monsters it does not blind has 36, 39, 45, 46, 50, not 34; 43 / 44 follow 14 / 26), **eft15_m** (the muzzle burst's joint by kind and
  animation: the subspecies take their originals' lists; kinds 15 / 45 also fire on animation 0x41E at frame 78), Em_Yobi_Ck / em_boss_work_set
  (kind 35 joins the Velociprey pack and its leader), Bdora_hp_ck, enemy_mv (draw flag), em_cmd_end_command's 0xFE distance, basic_com_ck's mounting
  check and em_ride_sub (50 rides like 7), station_em_set (36), eft17_i00 / eft17_t00 (36 draws as 2), em14_sasari_ck and em_dmg07 (43 / 44), em20 (38 acts as
  6, 39 as 20 in dmg_to_normal, fly_act_set, the two sleep acts; attack 26: 38 fires type 3, 39 type 2 with x07 1). [asm / m2c read per place; quests 4 5 10 17 26 31 33
  10036 10043 and the Dual Swords hunt pass hidden; no subspecies hunt was watched.] False hits of the scan (macros, e.g. em_cmd_flag_set's EM_KIND_7, em17_nm's
  act 14, em01_to_normal's dead branch, em03's stage numbers) were dropped.

**Looked at, nothing to port (or not for the player)**
- The ud functions of the list (Warehouse_*, Get_equip_bit, Equip_ok_ck, Ud_u_item_stack, Get_atk_value): already G through UD_WARE_N / EQ_ARMOR's row size /
  Get_equip_value; Ud_u_item_stack's item-book bit is read by nothing. em27_init: hit points go through G's tables (em_hp_vital_set), the stay /
  runaway timers through the list: tables; only G's char-set helper (0x549FE0) differs. Eft04_set_time: G clamps the time by the owner's act speed (the
  callers already divide) [inferred negligible]. eft16 (hit blood / sparks): G drops the neck types and adds a type 15; no PC caller passes 15.
  Lb_pl_move / lb_pl_move_sub / Lb_Pl_stg_ck / eft13_se_req: online-only differences (the master byte, lbCommer). Lb_make_quest_tbl (online table: agent C's lane).
  Item_preparation_list_num (the combine list's count in the pause menu): G has no twin, arrow only. Layout-only: Get_equip_bit, Equip_ok_ck (armour row size).
- **Not looked at in detail** (still the top of the remaining list, by size): eft02_t / Eft02_set3, eft04_i / eft04_m (G 524 / 814 instructions
  vs 407 / 716), eft15_m, eft16_m (G drops the neck spray), set14_m / set14_trans, em02 (Fatalis) atk / fly, em14_main, em03_init / main, em16_init2.

**Checks** (23 Oct 2026): `tools/rebuild.sh` 5 OK, `VERSION=mhg tools/rebuild.sh` 7 OK, tools/test_all_quests_g.sh, tools/full_check.sh: see the result lines in docs/agents/agent-D.md "GB6 eighth pass".

## 16. GB6, ninth pass, partial (agent D, 23 Oct 2026)
The "not looked at in detail" list of section 15, same method (tools/mhg_fdiff.py for the asm, tools/gdraft.py for G's m2c, a per-case
call-list diff of G's draft against the PC's C for the long animation scripts). All code is `VERSION_MHG` (matched files also
`!__MWERKS__`, so PS2 builds keep their bytes), MH1's C is unchanged; G tables the PC reads at ten / sixteen entries are sized in
config/mhg/pc_data.txt.

**Ported**
- **eft04 (monster attack effects: eft04_i / _m / _t, Eft04_set_time)**: G has **ten types** and **60-byte pieces** (a velocity at +0x30).
  New type 9 = debris thrown around joint 0x22 of em15's subspecies roar: starts 80 in front of the joint, a piece falls (-8 per tick),
  on the ground a sound (Em_se_req2 30), two Eft04 type 1 / 2 bursts, then the piece is retired (no 0xFF); drawn as an upright billboard
  (rview_matY) from clay 9. Other G changes: type 0's piece sizes follow the monster's size percent (6 x size / 100 and 2 x size / 100;
  MH1 3.5 / the effect scale), type 2's pieces rise times the effect's scale and start at scale x (1.2 - 0.2 i), type 8's texture scrolls
  (0.01875 x lag), eft04_t no longer asks eft_mdlw[0], Eft04_set_time clamps the callers' time (act speed below 0.001 or time above 200: 100;
  G's callers pass the raw number and it divides by 2 x act speed, the PC's callers still divide, so the clamp alone is equivalent).
  [m2c + asm read per hit; hidden hunt of quest 41 with a temporary forced spawn: type 9 inits at the joint, falls 291 -> 9 in 8 ticks,
  lands, ends; no crash. NOT SEEN ON SCREEN: see "Found" below.]
- **em15 ef_move_sub** (G's em15 serves kind 15 and its subspecies 45): the three-shot fan at animation 0x417 frame 176 is gone, animation
  0x451 plays sound 0x5B, and 0x459 (the roar) has a **second script for kinds != 15**: its own sounds, shell16 0x27 at frame 16 and 0x28 at
  130, Eft04 type 9 every 5th tick between frames 120 and 180. [call-list diff of G's draft against the PC's C, all 40+ animations compared]
- **eft15 type 9** (one sprite, drawn like type 3: clay 66 / 75 with the type 3 uv scroll) and G's **x07 1 = start at once** (types 2, 3, 7, 8
  skip the lag); **eft15_se_req** plays G's own sounds with se_req2 (type 0: 94, 2: 95, 3 / 7: 97, 8: 98, type 4: 117; none for x07 1) instead
  of MH1's Pl_se_req2(owner, 0); **eft15_t** switches render state 0x6C off around types 2 and 3 / 7 / 8 / 9 and back on after every sprite.
  [asm + m2c; forced type 9 spawn at the hunter's side of the quest-41 monster: blue lightning arcs (build/D/te/keep/eft15_t9_thunder.png, a
  temporary hook)]
- **eft16 type 15** (hit blood, spray like type 8 without the owner's joint step, one piece) and **dark blood for kinds 8 / 34** (Eft_blood_rgb
  row 6, mode2 bit 0x40 set by Eft16_set / Eft16_set_impact, eft16_col_type_sel row 6); render state 0x6C off around types 0, 2, 5.
  [forced type 15 at the same monster: a white-yellow puff beside the hunter, absent in the run without it; the dark-blood path was not hit]
- **eft02 dark blood for kinds 8 / 34** (Eft02_set3 sets x1E 3, eft02_t takes Eft_blood_rgb row 6 for it). [asm; not hit in a run]
- **set14 stage 90** (G's own stage): set14_i / set14_m / set14_trans take the stage-51..53 mist-layer code with G's mask table (main 0x2EE948)
  and position table (game 0x6BFCF0), clay 5. [asm + m2c; no offline quest uses stage 90, not run]
- **em12 (Cephalos) loses the neck spray**: G has no Eft16_set_ex (and no types 3 / 4 body), em_dm00's frame-2 spray and ef_move_sub's
  animation 0x424 script are gone. [asm]
- pc_data.txt: eft04 / eft15 tables ten entries, eft16 sixteen, Eft_blood_rgb a seventh row, eft15 type 9's time table (main 0x2EC774),
  set14's stage 90 tables.

**Looked at, nothing to port**
- **em14_main**: already complete in the code (section 7 / the earlier subspecies passes): the x8B6 guard, the act-4 reaction chain for
  animations 6 / 13 / 14 / 17 / 18, kind 43, G's quest 10041 demo stop and info_stop, the single act-5 call: all of G's differences from
  MH1's asm are in em14_nm.c. The cov list's 0.96 was stale. [asm diff read hit by hit]
- **em02 (kind 2 and its subspecies 36)**: G's em02 is a bigger rewrite than the kind lists: seven new functions (game 0x5A9F00 a counter
  tick, 0x5AC150 a fly-away act with em02_fly_adjy2_init type 6, 0x5AC580 five variants of an attack, 0x5ACFB0, 0x5AD140, 0x5AD470, demo
  0x5AF150), fly act 11 and attack acts 12-18 in em02_fly_act_set / em02_atk_act_set (G's em_move02 / 03 dispatch to them), fly_adjy2_init
  types 4-6, em_act05 / 06 / dmg03 ending a sleep through stockEndPulldown / Em_Sleep2_End, em02_init / hagi_set / main with kind 36
  branches and ef_move_sub (15 KB, +44 instructions). **No offline quest spawns kind 2 or 36** (boss kinds of the
  98 quests: 1 6 7 8 11 14 15 17 20 21 22 26 27 28 31 37 44 49; the stage lists have no 2 / 36 either), so there is nothing to see in a
  hunt, and a half port would leave the dispatch pointing at nothing. Left whole, not started: port it together when an online or event
  quest can spawn it.
- eft04_type0_0_init / type3_init / type8_init, eft04_d, eft16_i / se_req: same shape as MH1 (the 60-byte stride is the piece struct's).

**Found, not fixed (PC platform, both games)**: `game_w.area_mdlw[]` (the monsters' own effect model sets, game_w + 0xA8 in MH1 / 0xB0 in G)
is never filled on the PC: all entries are NULL while the quest-41 monster (kind 15) is on screen. Everything that draws from it returns
without drawing: eft04 (the whole monster-attack effect family), eft01's shadow arg 4, eft20's area pieces and shell08's clay pieces
(`am = game_w.area_mdlw[Em_area_ck(...)]`). Only eft_mdlw[0..4] (rt_eft.c rt_bind_eft_model: ef_00, three shadows, ef_01) are bound. [checked:
a trace in eft04_t during the forced spawn printed eight NULL entries] A platform task: bind each loaded monster's effect model
(eft04 reads clay[0..9] of it, eft20 and shell08 their own clays) the way load_eft does for ef_00.

**Checks** (23 Oct 2026): quest suite 98 + carts + Dual PASS, rebuild 5 OK / G 7 OK, full_check stopped at 25 ok steps (no FAIL) on the coordinator's request. **Left: em02** (whole), the area_mdlw platform gap.

**Fixed (24 Oct 2026, agent D, both games): game_w.area_mdlw.** What it is: the PS2's `em_create_model` (main 0x124820,
src/main/model/crmdl05.c) loads, after a model slot's monster body, the kind's **sub model** (`load_enemy_sub_model`: AFS index
from `em_sub_model_data` main 0x2EC8C0 / G 0x2592E0, texture `ENEMY_SUB_TEX` 0x2EEEB0 / G 0x25CB10, -1 = none) and keeps its MDLW
in `sub_ptr[slot]` = game_w + 0xA8 (G + 0xB0), with sub_n / sub_no after it (so "area_mdlw[9]" really is four pointers; `Em_area_ck(kind)`
returns the model slot whose game_w+0x28 kind matches). The PC host (rt_quest.c em_create_model -> viewer.c em_model_load) only
loaded the body. Now em_model_load also loads the sub model (cached by kind like the body, freed with it in host_models_gc) and
rt_eft.c `rt_bind_area_model` binds its clays / materials into game_w.area_mdlw[slot] the same way as eft_mdlw (handles reused per
slot). Kinds with a sub model: MH1 1 2 6 8 11 14 15 17 20 21 22 26 33 34; G those plus 36-49 (read from both tables) [checked].
Seen [checked, hidden shots build/D/am/keep/, before = the binaries without the fix]: quest 41 kind 15 (Khezu) at 4.9 s: its
body discharge (eft04 type 0) now drawn as a blue aura in **MH1** (a_mh1_4.9 vs b_mh1_4.9) and in **G** (a_g_4.9 vs b_g_4.9, G's own
piece sizes); kind 33 (Kirin, RT_QEM_KIND=30:33 in MH1 quest 41): the arg-4 ground glow of eft01 under both Kirins (a_mh1_k33 vs
b_mh1_k33). Not seen on screen: eft04 type 9 (G subspecies roar), eft20, shell08 pieces, a G Kirin (the swap spawns it but on
another area). Memory (RT_MEM, G quest 41, tick 200): +57 KB CPU heap ("monster sub models" category, 291 KB peak while the file
copies are live), +65 KB textures as the Xbox keeps them; per kind 107-123 KB of files. A red horizontal line at the top-left in
the Kirin shots is in both before and after (not from this fix, not looked at).

## 17. Xbox fog vs PS2: Forest and Hills area 1 "too thick" (agent D, 25 Oct 2026)

Owner (Xbox, G): the fog in the first quest (deliver 2 Raw Meat, 10001; MH1's 131 uses the same stages) is too thick.
Camp = stage 21, area 1 (the Aptonoth) = stage 39. **Fixed, both games.**

- **Cause: after an area change the new stage's fog never reached the renderer**, so area 1 was drawn with the camp's fog
  (colour 4F4F49, start 0, end 9000) instead of its own (506060, 0-90000, the same values G's PS2 memory holds there): the far cliffs
  came out a flat 4F4F49 grey and the near grass 10-30% fogged. A load tick records its draws from its first flip (the loading
  picture, viewer.c game_flip) and drops that list unshown; stage_fog_set runs inside the load, so its three fl states were only
  recorded. Not an NV2A problem: the PC in a window had it too (hidden --shot runs record nothing, which is why the PC comparison
  pictures matched the real G). gfx_rec.c now records the fog states and applies them at once [verified: RT_FOG_TRACE in xemu, G and
  MH1: before only "nv2a fog: colour 4F4F49 start 0 end 9000" at the quest start; after also "colour 506060 start 0 end 90000" at the
  area change; PC with RT_FLIP_SHOT (records like a window): same before / after].
- Fog inputs per stage, PC and Xbox the same (RT_FOG_TRACE, both games): stage 21 colour 4F4F49 start 0 end 9000; stage 39 506060,
  0, 90000. G's PS2 (PCSX2 memory, tools/g_ref_shot.sh): 21 0x804F4F49 9000/0, 39 0x80506060 90000/0.
- Also fixed (Xbox, latent): the vertex programs pass the fog factor in COLOR1's alpha, and the NV2A forces that alpha to 1 unless
  SET_LIGHT_CONTROL has ALPHA_FROM_MATERIAL_SPECULAR (xemu models this from hardware tests). It was never set, so the picture
  depended on the register's power-on value; with the bit clear the whole screen, HUD included, is the fog colour (xemu, test aid
  RT_NV2A_LC=0). Now set explicitly. [xemu vs hardware: the real power-on value is unknown; set explicitly it no longer matters.]
- The fog formula itself matches: xemu shots of the camp from the PS2's camera (--view, RT_FIXED_CAM) agree with the PC within a few
  levels, near and far alike.
- Pictures (gitignored, build/D/fog/): G_area1_ps2_pc_xbbefore_xbafter.png (PS2 | PC / Xbox before | Xbox after; the PS2 camera of
  this area is not the PC's, see below), MH1_area1_pc_xbbefore_xbafter.png, G_camp_ps2_pc_xb.png, difference pictures q/diff_*.png,
  PC window before / after q/pcwin_before_after_39.png. Mean colour difference Xbox to PC, area 1, HUD left out: 6.1 before, 3.3 after
  (G; MH1 5.8 -> 3.4).
- Open: g_ref_shot.sh's PS2 camera for stage 39 (eye 13190,-606,9735) does not give the PS2's view on the PC (river vs cliffs), so area 1
  has no exact PS2 pair; the PS2 picture shows the cliffs clear with a light haze, as the PC and the fixed Xbox draw them. Not yet seen
  on the owner's hardware.

## 18. em02 Fatalis, G's version (agent B, 28 Oct 2026)
Section 16's "Left: em02 (whole)". **Ported; checked against the real G disc with the lockstep referee.**
- **Where G's Fatalis is**: the disc's online quests **3004-3007** (m03004-007.mib, Castle Schrade, stage 25; the PC's
  `--quest 3004` and the referee start them offline). The crimson dragon (kind 0x24 = 36) is no quest's monster on the disc
  (G's downloaded event quests); the gallery's scene 13 (m60029) shows it on stage 88.
- **Ported (VERSION_MHG; MH1's C unchanged, the PS2 builds byte-identical)**: the setters (fly 11, attacks 12-18,
  fly_adjy2 types 4-6), em02_init (kind 0x24 / event-quest hit points, the new counter), G's new tick 0x5A9F00, hagi_set's
  carve ids by quest rank, em02_main (no anger while dying, kind 0x24's part carves), the sleep / paralysis ends, dmg03,
  die02 (frame 100), the new acts: swoop (12), dive (13), fire rain (4 + 14-17, shell08 type 12 from G's offset lists),
  double rain (18), fly-away (11), the dispatch, and ef_move_sub (G's motions 0x40B-0x410, kind 0x24's hits, changed
  frames / sounds). Details: docs/agents/agent-B.md "G em02 (Fatalis) rewrite".
- **Which acts a player meets**: kind 2's command data reaches 12 and 13; 14-18 are kind 0x24's; fly 11 is reached by no
  command (unused, as far as the data shows).
- **Referee against the real G** (PCSX2, tools/referee.py; build/B/em02/ref*, not committed):
  - g_fatalis (quest 3004, 4500 play ticks: the opening demo, then the fight): the monster's mode / action / step equal on
    every tick, HP equal (32000; no hits landed), position within 4 units after the hunter drift below.
  - g_fatalis_acts (5200 ticks): swoop, dive, fire rain 14, double rain 18 and fly-away forced on both sides by pokes: every
    step on the same tick, position within 4 units (the carried drift), the fly-away climbs 1127 -> 7046 on both.
  - g_fatalis_rage (6000 ticks, HP set to 12000): same actions on every tick.
  - Not em02: the hunter is knocked by the opening roar one tick later on the PC (PS2 1505, PC 1506); from there the hunter
    is ~5 units apart and pushes the monster 2-4 units. Same one-tick hit lag as the kutku scenario (section 11); cause not
    found. The PS2 makes more sound-channel random calls (cosmetic, known).
- **Found and fixed on the way (both games' code path, G data)**: wall_tbl_add's entries for stages 84-89 / 91 point into
  main .bss and were left as raw PS2 addresses on the PC: a monster touching a wall on stage 88 crashed (gallery scene 13
  with the ported crimson dragon). Now `allptr` in config/mhg/pc_data.txt [verified: test_gallery_g 12 / 13 / 14 OK].
  - g_fatalis_kill (6500 ticks, HP set to 200, the hunter moved under it and swinging): same actions on every tick, but the
    scripted swings never connect on either side (HP stays 200), so no kill.
- **Not done / open**: the kill, the quest clear and carving on Fatalis are not compared with the PS2 yet (needs a pad
  script that reaches the dragon, or a poke that lands a hit); shell08 type 12's fire rain is not in the referee's records (shells are not compared): checked only on the
  PC (acts run to their end, no crash); kind 0x24 on the PS2 was not run (no quest; the PC was tried with RT_QEM_KIND=2:36:
  acts 12 / 14-18 from its commands, 400 s, no crash).
