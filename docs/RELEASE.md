# Release readiness: MH1 (Japanese) on PC and the original Xbox

Audit by agent A, 13 Oct 2026 (owner's direction of 13 Oct: polish MH1 into a release-ready game on PC and
real Xbox; since the same day MH G is the main release and MH1 JP is finished alongside it). No game or host code
was changed for this audit.

How things are marked:
- **[verified]**: seen today in a test run, a coverage run or the code itself (says which).
- **[docs]**: taken from docs/STATUS.md, pc.md, xbox.md, network.md, server.md, REVIEW.md or agent notes, not re-run.
- **[inferred]**: my reading of the code or the docs; not tested.

Owner column: **decomp** (game C, near-matches), **platform** (src/pc host code, PC build, packaging),
**Xbox** (nxdk build, nv2a, hardware), **server** (tools/server), **owner** (a decision or a hardware check).

## 1. What "release-ready MH1" means

Concrete things a player notices. Each line says how to check it and where it stands today.

| # | Criterion | How to check | Today |
|---|---|---|---|
| R1 | Boots from the player's own disc data, nothing Capcom shipped | PC: first start asks for the ISO, `src/pc/install.c` checks sizes/CRCs and copies the files. Xbox: a documented way to get the files onto the HDD | PC done [docs, install.c read]. Xbox: no player path (the owner copies data by FTP) |
| R2 | Every quest playable from the board to the reward | `tools/test_all_quests.sh` (38 offline quests), `test_progression`, `test_urgent`; then one human play-through of every star level | All 38 offline quests and both urgent quests pass [verified today], but with test aids (RT_PL_SLAY, warps, god mode). No human has finished the game on the port. Online guild quests: only one co-op quest is in a test |
| R3 | Saves are reliable | `test_save` (168 checks), `test_save_import`, `test_quest_loop` CONTINUE; Xbox bed save and CONTINUE on hardware | PC pass [verified today]. Xbox: the bed save's "Save?" box is invisible on hardware (owner report, agent E on it) |
| R4 | Music, sound effects, ambience right | `test_audio`; owner listening on hardware | PC pass [verified today]; Xbox sound works [docs, owner]. Open owner reports: forest ambience during the boot logos, quest-clear music again at the bed save |
| R5 | No stand-in or no-op on a path a player reaches | coverage run (section 3), F8 reports | 46 distinct stand-ins were reached in today's run; 8 matter to a player (section 2) |
| R6 | Menus and options complete | game's OPTION screen; PC F10 settings (`test_menu`) | PC done [verified today]. Xbox has no F10 menu (`src/pc/menu.h` stubs): no language or button-layout choice there [docs] |
| R7 | Frame rate on the real Xbox | game log frame counts, RT_PROF; owner's eye | ~30 fps in field and village on hardware [docs, 12-13 Oct]. Big fights, movies, area loads not measured |
| R8 | Online town against a self-hosted server | `test_online_town`, `test_online_server`, `test_coop` | PC and Windows (Wine) work against mh1-server [docs]. Xbox online never run |
| R9 | Controls | pad layouts PC / Xbox, rumble, keyboard | PC: SDL pad + keyboard, circle/cross swap, rumble for vib_set only. No key rebinding. Xbox pad mapping checked [docs] |
| R10 | Settings remembered | `mh1pc.ini`, `test_menu` | PC done [verified today]. Xbox: none beyond the game's own options on the memory card |
| R11 | Crash-free over long sessions | `test_soak`, `tools/xemu_soak.sh`, a multi-hour human session on hardware | PC soak flat over 3 loops [verified today]; xemu flat except ~130 KB lost per loop [docs]. No multi-hour run on hardware recorded |
| R12 | Install / build steps a player can follow | a player-facing guide per platform | Windows test zip has a README (`tools/package_win.sh`). No Linux package, no Xbox guide. README.md is for developers |
| R13 | Language | Japanese always; English text | Japanese only. English layer exists (`RT_TEXT_TABLE`, docs/english.md) but no text is shipped; the US disc (docs/mh1_us.md) is now the way to get it |

## 2. Gap list, by player impact

### Blockers (a release cannot go out with these)

| # | What the player sees | Where | Status | Owner | Basis |
|---|---|---|---|---|---|
| B1 | Xbox: no way for a player to install. The Xbox DVD drive cannot read a PS2 disc, so ~925 MB must be copied from a PC to `E:\Games\MH1\data`; there is no tool, check or guide for that (the PC's ISO installer, `install.c`, is SDL/PC only) | docs/xbox.md "Risks" and the platform table; src/pc/install.c | not started | Xbox + platform | [docs], [verified: no Xbox install code] |
| B2 | Xbox: the "Save?" box at the village bed is invisible, so saving at the bed is blind | owner report 12-13 Oct; nv2a 2D path | agent E working on it | Xbox | [docs] |
| B3 | No English. A western player sees Japanese only | src/pc/rt/rt_text.c (layer works), docs/english.md; no table shipped | design done; the US disc now gives a source read from the player's own disc (docs/mh1_us.md) | platform + decomp | [docs] |
| B4 | No human has played the port through. Every quest "passes" only with test aids (`RT_PL_SLAY` kills swarm/boss targets, warps, god mode, `RT_DMG_MUL`), and fights were never compared with the PS2 | tools/test_all_quests.py, docs/pc.md "All offline quests played" | process gap | owner (play-test with F8), decomp (fixes) | [verified: all 38 pass today, aids in the scripts] |
| B5 | Player-facing install and run steps are missing outside the Windows test zip (Linux package, Xbox package, one "how to install" page) | tools/package_win.sh, README.md | not started | platform + Xbox | [verified] |
| B6 | Xbox online: the town has never run on the Xbox (Winsock and nxdk paths of `net_cpinet.c` written, untested) | src/pc/net/net_cpinet.c, docs/network.md 5 and 3.5 | untested | Xbox | [docs]. Blocker only if online on Xbox is part of the release |

### Noticeable (a player will see it; fix before calling it polished)

| # | What the player sees | Where | Status | Owner | Basis |
|---|---|---|---|---|---|
| N1 | Long pause on every area change with no "now loading" screen; on the Xbox it looks like a hang (owner report) | `Disp_load_start` and `Disp_NowLoading2` are no-ops (src/pc/rt/rt_flow.c:161-162). Disp_NowLoading2 was reached in today's run | open (owner idea: show the loading screen only when loads are slow) | platform + Xbox | [verified: no-op reached] |
| N2 | Lighting: every model uses one fixed light set; the game's per-stage and per-actor lights are opt-in (`RT_LIGHT_GAME=1`) because nobody compared them with a PS2 capture | viewer.c, src/pc/rt/rt_light.c, docs/pc.md "Lighting" rounds 24-26 | needs one PS2 reference capture (owner) | owner + platform | [docs] |
| N3 | Weapon models do not change shape per motion: great sword, lance, hammer and bowgun node scaling (`weapon_dat_make` tables) is not applied by the host weapon draw | src/pc/rt/rt_player.c:585-597 (comment says not done) | not started | platform (or wire weapon_trans from weapon3_nm.c) | [verified: code]; what it looks like [inferred] |
| N4 (DONE 13 Oct, agent C: vib_set_pl is the game's vib.c, vib_stop_all real, rumble stops on strength 0; headless runs never touch real pads) | No rumble from monster-side shakes (Plesioth, Cephadrome and others call `vib_set_pl`); the hunter's own `vib_set` rumbles | `STUB_V(vib_set_pl)` src/pc/rt/rt_eft.c:909, reached in 57 runs today; `vib_stop_all` (rt_flow.c:153) also a no-op, reached | small port from the asm (0x169920, 0x68 bytes) | decomp | [verified: reached]. Xbox rumble through nxdk SDL [inferred, untested] |
| N5 (DONE 13 Oct, agent C: flmatInit33 in rt_flmat.c) | Monster joint overrides start from a wrong matrix: `flmatInit33` is a generated stand-in that does nothing and returns 0, so mlCalcTransEM's "reset then rotate" for Yian Kut-Ku's ears (kind 6, joints 0x25-0x27) and Kirin (kind 0x21) composes onto whatever the matrix held | build/pc/rt_gen.c stand-in; callers src/main/em/emmk01.c:77-104 (enemy_mk, game C); flmatInit33 is VU0 inline asm in src/main/fl/flvu02.c, which gcc cannot build | reached in 3 runs today; fix = a C flmatInit33 (identity 3x3) in rt_main.c or rt_flmat.c | platform | [verified: reached]; whether it changes the drawn pose (the host poses monsters itself) [inferred, check with a Kut-Ku shot] |
| N6 | Owner reports from Xbox play (12-13 Oct): forest ambience plays during the boot logos; the quest-clear music plays again in the village when saving | sound flow (rt_snd.c, bgm_nm.c) | open | platform | [docs] |
| N7 | Near-match C runs in play. Today's coverage run entered 204 functions that are not byte-matched (233 KB: main 121, game 35, lobby 47, select 1; some are host replacements of same-named originals such as Game_task or SpritePut). Three review rounds (docs/agents/targets.md) fixed ~30 real differences; still "not decided": eft20_m, eft06_m, set13_trans, em_neck_move_sub, neck_ang_set, Em_Master_Change, shell08_m/_trans; `disp_item_sub_normal` may repeat the sprite-layout bug fixed in disp_cannon; build_pc.sh `rtabi_*` adaptors could hide a dropped argument | src/**/*_nm.c, tools/build_pc.sh | each needs semdiff + a read against the asm, or a byte match | decomp | [verified: coverage today], verdicts [docs] |
| N8 | Online/co-op gaps: capture quests, giving items between players, in-quest chat (channel 6), two big monsters on one area at once are not done for co-op; direct-connect co-op starts at the quest, not from the board; a host leaving a direct session splits it (fine through the relay) | docs/network.md 3.4 "Not done yet" | open | platform + server | [docs] |
| N9 | Public server not safe yet: passwords readable on the wire, no TLS, no rate limits, no moderation tools, no sign-up | docs/server.md 15 | owner decisions pending (hosting, name, PS2 cross-play) | server + owner | [docs]. Not a blocker for a private, invite-only server |
| N10 | Xbox memory falls ~130 KB per quest loop in xemu; a very long session may run out (64 MB, no swap) | docs/xbox.md "Memory stays flat" | open (allocator suspected) | Xbox | [docs] |
| N11 (DONE 15 Oct, agent F: Back + L3 settings menu on the Xbox, settings.ini on E:, 16:9 anamorphic; docs/xbox.md "Settings menu on the Xbox") | Xbox: no settings menu (language, button layout, volumes outside the game's own OPTION screen); picture always 4:3 | src/pc/menu.h stubs, gfx_nv2a.c ignores 0x105 | not started | Xbox | [docs] |
| N12 | Windows build never ran on real Windows, only Wine | docs/pc.md "Windows build" | owner or a tester | owner | [docs] |
| N13 | Xbox frame rate not measured in big fights, movies or on area loads; the skinned-model near-clip smear has no fix (candidates 2/3 in docs/xbox.md, GPU-only) | gfx_nv2a.c | measure on hardware | Xbox + owner | [docs] |

### Minor

| # | What the player sees | Where | Status | Owner | Basis |
|---|---|---|---|---|---|
| M1 | Japanese name entry and chat cannot convert kana to kanji (`apiask_*` kanji dictionary API of the soft keyboard is a stand-in) | build/pc/rt_gen.c, src/main/sk, src/main/ime | reached today (name entry) | platform | [verified: reached] |
| M2 (DONE 13 Oct, agent C: ALIASES in build_pc.sh) | Rarity colours of equipment names and the page arrows in some lists missing (`Equip_moji_color_rare_i`, `Put_PageArrow_s`: rename aliases of chat_nm.c functions that need linker aliases) | docs/pc.md "Stand-ins wired"; build/pc/rt_gen.c | reached today (2 and 1 runs) | platform | [verified: reached] |
| M3 | Reverb is an approximation | src/pc/audio | accepted | platform | [docs] |
| M4 | Hunter hair and cloth bones (ptmat >= 64) keep their bind offset | fl_model / rt_player | not re-checked since 7 Oct | platform | [docs] |
| M5 | Xbox dashboard shows no save icon (SaveImage.xbx / TitleImage.xbx not written) | docs/xbox.md "XBE identity and save icons" | needs a reference .xbx | Xbox | [docs] |
| M6 | No key rebinding on PC (only circle/cross swap) | src/pc/menu.c | not started | platform | [verified: no remap code] |
| M7 | Frog-bait fishing only bites if the Plesioth is idle when the float lands (may be faithful) | docs/pc.md "Frog fishing" | unverified vs PS2 | owner | [docs] |
| M8 | Untested shop variants: selling from the pouch at the house box, "organise items", trader buy/sell beyond one case, shock trap, a capture quest | docs/pc.md 2101 | untested | platform | [docs] |
| M9 | Xbox picture ~25-30 levels darker on the capture card than xemu/PC (probably the video chain, not the game) | docs/xbox.md "violence warning brightness" | owner check with a retail game | owner | [docs] |
| M10 | Dev-only, not seen by players: the network loader sometimes freezes after Evolution X starts it | docs/xbox.md "Loader download froze again" | theory, next loader install | Xbox | [docs] |
| M12 (DONE 17 Oct, agent A; found through G's GB5) | A weapon bought or equipped in the village (shop, house box) kept the old weapon model on the hunter's back until the next quest; the bug reporter left the game's texture stage on its released frozen-frame texture after saving / cancelling, and wrote its files a kilobyte at a time on the Xbox (about a minute per report on hardware) with no message | src/pc/viewer.c weapon_follow / viewer_write_file, src/pc/pick.c, src/pc/xbox/xbox_report.c | fixed: model follows in the village, texture restored, 'SAVING THE BUG REPORT...' then one write per file | platform | [verified: PC shop/box tests, PNGs checked; Xbox save time in xemu] |
| M11 | Character editor: `init_eft_work` / `init_shell_work` are no-ops on purpose (making them real moves the boot's random stream) | docs/agents/agent-F.md | deliberate | platform | [docs] |

Possibly stale (written 6 Oct, before the player code became one matched file; check in a play session before acting):
"water: GetWaterHit runs but nothing reacts", "the fall action", "pl_wall_mat use" (docs/pc.md:1038).

## 3. Coverage run (13 Oct 2026)

`COV=1 tools/build_pc.sh`, then with `RT_COV=file RT_STANDIN_FILE=file`: test_quest_loop, progression, urgent,
name_entry, movie, frog, audio, save, save_import, menu, pick, log, all_quests (38/38), activities (38/38),
soak (3 loops, flat), and `tools/sweep_random.py 90` (90 s of random input in ten quests and five village spots).
All passed. `tools/cov_report.py` over `tools/unmatched.py` lists: 7358 functions entered, 204 of them unmatched
(above, N7). The plain build was restored afterwards. Online (ONLINE=1) was not covered: its stand-ins are the
browser / portal side by design (docs/pc.md "Stand-ins wired").

Stand-ins reached (46 distinct; number of runs that hit each in brackets):
- Matter to a player: `vib_set_pl` (57), `vib_stop_all` (34), `Disp_NowLoading2` (6), `flmatInit33` (3),
  `apiask_28_OpenDic` (6), `Equip_moji_color_rare_i` (2), `Put_PageArrow_s` (1); `init_shell_work`/`init_eft_work` (28, deliberate).
- Harmless on PC/Xbox (the host owns screen, loading, draw order, model memory): flAdjustScreen, flCalcTrans,
  flCalcTransSI, FlushCache, flCompact, view_reset, load_busy_ck, load_bin_req, init_set_work, flSndPortStop,
  flSndPackLoadStatus, flSndPackLoadBG, flSndPackLoadBG2, set_viewproj, load_shadow, load_eft, flFlip, stage_free,
  ot_init, round_init, snd_joint_load_init, em_effect_pull, model_work_init, ADXM_Lock/Unlock, View_init,
  init_view_work, flReleaseMotionSetHandle, clr_used_heap, all_model_free, release_texture, edit_create_model,
  armor_model_free [inferred from the names and docs/pc.md "Quest 154 ... stand-in sweep"; init_set_work and
  clr_used_heap were not in that earlier list and deserve one look].
- Online-side, harmless offline: lb_member_inCheck / outCheck / changeCheck, text_lobby_trans_ot3_o.

Code found while auditing:
- `src/pc/rt/rt_em.c:37-44`: the weak `em02/03/04/09/10/12/16/18/19/29/33_local_area_move_init` and
  `em10/em18_act_set` stand-ins. None of these names exists in the original's symbol list (config/symbols/game.txt
  has local_area_move_init only for em01/07/08/14/15/17/20/21/27) and nothing references them except the host
  symbol table; none was reached today. Probably dead [inferred]; agent F's note says entries of em_prog_tbl are
  resolved by name at run time, so check rt_data.c's table import before removing them.

## 4. Next games

- **MH1 International (US, SLUS-208.96)**: surveyed in docs/mh1_us.md (supplied by the owner on 13 Oct, disc/mh1_us/).
  Headline: stripped, same compiler; 66% of our matched C appears unchanged in shape (game 81%, main 63%, village UI
  only 33% because it was re-laid out for English); 2067 of 2156 shared data files byte-identical; English is plain
  ASCII in the same places as the JP text plus the quest files (120 of 153 quests line up by number); a redrawn font;
  the US adds the Dual Swords weapon class, 9 items and quest numbers 178-193 (159 quest files vs 153). Its script is the base for G's English.
- **PS2 Monster Hunter G**: now the project's main release (owner, 13 Oct); its release checklist is docs/RELEASE_G.md. Its survey and bring-up belong to another
  agent. Earlier numbers: docs/survey/mhg_vs_mh1.md. tools/carry_match.py mhg measures the same
  carry-over for G (50.9% of matched bytes on 13 Oct).

## 5. Suggested order [inferred]

1. Blockers that only need fixes: B2 (E), N1 loading screen, N5 flmatInit33, N4 vib_set_pl, N6 owner reports.
2. B4 human play-through on PC and Xbox with F8 / plain reports, one star level at a time; agents fix.
3. B1 + B5: Xbox install path (a PC-side tool that copies the player's ISO files to the Xbox, or reads the ISO from
   the HDD) and a player install page per platform.
4. B3: English from the player's US disc (docs/mh1_us.md 5 (a)).
5. N2 lighting check against one PS2 capture; N3 weapon node scaling; N7 remaining near-match reviews.
6. B6 / N8 / N9 if online is in the release.

## 6. Lockstep referee against the real MH1 (agent D, 18 Oct 2026)

tools/referee.py (docs/referee.md) runs MH1 JP the same way as G: the owner's disc in PCSX2 (CONTINUE with the PC
port's save from tools/test_quest_loop.sh, the Elder's first quest card) and build/pc/mhview, same quest, same pad,
compared tick by tick. Scenarios: walk (quest 1), sword and shield, great sword (quest 1, area 22), fights with the
Rathian (quest 10, area 40) and the Kut-Ku quest 4 (area 36) [verified: build/referee/mh1_*/report.txt].

**How in sync:** walk / weapons: identical every tick (only sound-layer random draws differ). Rathian: identical for
241 ticks, Kut-Ku: 318; the first difference is the one-tick-late root motion (below).

**Fixed (shared code, the same changes as docs/RELEASE_G.md section 11 R-F1..R-F5; MH1 verified by these runs):** the
Scheduler's ran_suu(1), stick angle rounding, the monster order 0..19, game_core's order (hit_check after the
monsters, body_hit before the effects), the game13 tick at the quest start.

**Second round (agent D, 18-20 Oct 2026; the same fixes as docs/RELEASE_G.md section 11 R-F8..R-F10):** PS2 float
rounding is the default (RT_FPU_PS2=0 turns it off); joints built in move() as enemy_mk / player_mk do (Rathian
241 -> 291, Kut-Ku 318 -> 319); the Elder's tutorial ("△ PUSH", lessons, pages, buttons blocked while open) ported
[verified: MH1 quest 131 screenshots]. G's Dual Swords and pc_noptr fixes are G-only.

**Third round (20 Oct 2026; RELEASE_G R-F13..R-F17, all shared code):** the wall pass after body_hit, CameraMove after
it, the PS2's own sine / cosine / curve formulas and atan2f, the hunter's joints without the drawing offset. Rathian
291 -> 365, Kut-Ku 319 -> 364; both now first differ in the red-health recovery tick (N-R9: a frame counter that also runs
during PS2 area loads).

**Fourth round (21 Oct 2026; RELEASE_G R-F21, R-F22, shared code):** monster necks turn on the host skeleton (Rathian,
Rathalos, Lao-Shan) and motion blends mix matrices as flBlendMotionEx does; MH1 Rathian's later differences moved from
~290 to 733 [verified: referee pc runs].

**Left, ranked:** (1) float-step drifts after 200-300 ticks (RELEASE_G N-R7); (2) N-R9; (2) sound-layer random draws
a tick late; (3) the quest clock while loading (owner, 18 Oct: left as is).

## 7. Cross-game audit (agent A, 18 Oct 2026; docs/audit_cross_game.md)
Every G-only / MH1-only change since 13 Oct checked against MH1. For MH1:
- Debug menu "Can't faint" / RT_PL_GOD: holds against a hit over 100 in one tick and keeps the hunter's own maximum
  (was put back to 100 each tick: a hunter with 150 hp was cut to 100) [verified: MH1 trace, full_check].
- tools/test_all_quests.sh (full run) now also checks the failure paths offline: three faints and a time-out on quest 139
  (both PASS: no fault, a test gap the G sweep already covered).
