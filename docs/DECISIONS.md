# Decisions

Dates are when the decision was made. Revise freely once real data from the disc contradicts something here, and say why.
Each entry says who made it (the owner, or an agent while the owner was away) and when. Later decisions that change an
earlier one are noted in both places.

## Short version (as of 3 Nov 2026)

| Topic | Current decision | Entry |
|---|---|---|
| Which game is released | Monster Hunter G (PS2, Japanese) is the main release; MH1 JP is finished too; MH1 US later | 13 Oct |
| Repo layout | One codebase for every version, a `VERSION` switch, shared C | 13 Oct |
| Method | Decompile to portable C, byte-match as the correctness check, never as a blocker; no static recompiler | 3 Oct, 4 Oct, 5 Oct, 3 Nov |
| Compiler | mwcps2-3.0b52-030722 -O4,p (MH2 adds `-sdatathreshold 0`) | 4 Oct, 2 Nov |
| Platform | Written from scratch for PC and Xbox, nothing PS2-specific emulated; PC first; 32-bit host | 3 Oct, 5 Oct |
| Xbox SDK | nxdk | 3 Oct |
| Online | Our own server and our own open PS2 patches; PS2, PC and Xbox together; wire protocol kept, DNAS removed | 3 Oct, 15 Oct, 16 Oct |
| English for G | Monster Hunter Freedom first, MH1 US second, our own lines last; "Dual Blades". The community MH1j patch is a very-low-priority option for later | 25 Oct, 28 Oct, 3 Nov |
| MH2 (Dos) | We split and decompile it ourselves; PC and Xbox ports like MH1 and G. The community mh2-decomp repo is an allowed source of facts | 1 Nov, 2 Nov, 3 Nov |
| MH Dos on the Xbox | Its own title: id 4D480003, "Monster Hunter Dos", E:\Games\MH2; the next project after G | 4 Nov |
| Movies | libmpeg2 | 7 Oct |
| License | MIT for the project's own code | 15 Oct |

Items still open are in "Open" at the end. Three were settled by the owner on 3 Nov 2026 ("Settled 3 Nov 2026" below).

## Decided

### MH Dos gets its own Xbox title; it is the next project after G (owner, 4 Nov 2026)

- Owner: "Yes MH dos gets its own space. It's the next project after G." Accepts docs/mh2/xbox.md's proposal: title id
  4D480003, name "Monster Hunter Dos", folder E:\Games\MH2 (data in E:\Games\MH2\data), loader game id `mh2`.
- G stays the release in progress; MH Dos work continues alongside, and becomes the main project once G is done.

### Base on a Japanese PS2 build, not the US/EU build (3 Oct 2026)

- The Japanese MH1 executable is reported to ship with a symbol table.
- The MH Oldschool community servers support only the NTSC-J games. The US/EU release uses different netcode that has not
  been revived.
- The Japanese build is also reported to be easier than the US one (more forgiving monster AI). That matters for anyone
  expecting US balance.

### Decompile to portable C, then retarget (3 Oct 2026)

Static recompilation was considered and rejected:
- PS2Recomp is experimental, targets modern PCs, and its graphics and VU1 support are incomplete.
- Its runtime model is a poor fit for a 733 MHz CPU with 64 MB of RAM.

(The owner's notes of 5 Oct below left a recompiler open as scaffolding. On 3 Nov 2026 the owner closed that: "we're long
past PS2Recomp". See "Settled 3 Nov 2026".)

### Use nxdk for the Xbox side (3 Oct 2026)

Open source. It includes pbkit (GPU), vertex program and register combiner compilers, an lwIP network stack, SDL2 and USB.
It avoids the leaked Microsoft XDK entirely.

### Keep the PS2 wire protocol, remove DNAS (3 Oct 2026)

Lets an Xbox client share lobbies with PS2 and PCSX2 players if the server operators agree. Needs their cooperation or a
self-hosted server. (15 Oct: we run our own server; see "Online: our own server" below.)

### Bilingual via two string tables and a menu toggle (3 Oct 2026)

Keep the Japanese font and layouts; fit English into them.

### PC backend before Xbox backend (3 Oct 2026)

Same platform interface, far quicker to debug.

### Base on Japanese MH1 (SLPM_654.95), not PS2 G (4 Oct 2026)

- Survey: MH1 has 50,151 symbols with sizes and per-overlay relocations. G (SLPM_658.69) and its overlays are fully
  stripped.
- G stays a possible later target. It is built on the same engine (same compiler, same overlay layout, many identical
  overlay names), so MH1's names should transfer.
- (13 Oct: G became the main release, with MH1 as the stepping stone. See below.)

### Matching decompilation with Metrowerks CodeWarrior for PS2, 3.0 family (4 Oct 2026)

- Five MH1 functions byte-match with every 3.0-family mwcps2 build from decomp.me at -O3/-O4. The 3.0.1 family does not.
- Working default: mwcps2-3.0b52-030722 -O4,p, until bigger functions narrow it.
- The .comment "2.4.1.01" string is from the linker and identifies nothing.
- Rejected alternative: non-matching, behaviour-checked decompilation. Kept as a fallback for any file that resists
  matching.

### Playable and recognizable first, polished afterwards (owner, 5 Oct 2026)

- Byte-matching stays the correctness check, but agents cap time on stubborn functions (~10 min) and park them as
  near-matches whose logic is believed complete.
- Work on the platform layer (Capcom's fl graphics library replacement, model format) starts now, in parallel, instead of
  after the decomp.
- Whether to use static recompilation for not-yet-decompiled code, to reach "playable on PC" sooner, is still open (see
  Open).

### The goal is player experience (owner, 5 Oct 2026)

- Platform layers (rendering, audio, input, files, saves, movies, windowing, widescreen) are written from scratch for PC
  and Xbox. Nothing PS2-specific (IOP, VU, DMA, GS, disc drive, memory card) is emulated.
- What players feel is kept faithful from the original code: game logic (monster AI, hunter movement, weapons, hit
  detection, damage, camera, quests, items, menus) and what defines the look (models, textures, animation timing, lighting
  rules, effects).
- Byte-matching is a verification tool for that game-logic C, not a goal. It never blocks progress, and exact equivalence
  is polish for later.
- Same approach as the owner's Tonic Trouble project (new native engine on the original data, gameplay-accurate).
- PS2Recomp, if used, is scaffolding to run not-yet-decompiled logic, never the shipped product.

### PC host is a 32-bit build (5 Oct 2026; confirmed by the owner the same day)

- The decompiled game C keeps pointers in u32 fields (e.g. `flSetRenderState(0x19, (u32)&uv)`), and its structs (SETW,
  PRIM, CLAY, PLW...) must keep their PS2 offsets.
- Building the PC port with `gcc -m32` gives 4-byte pointers, so that code compiles unchanged.
- Static asserts in src/pc/rt/rt_game.c check the struct sizes (PLW 0xA00, GAME_W 0x224, CLAY 0x8C) on every build.
- The original Xbox is 32-bit x86 too, so the same C goes there.
- Cost: a modern 64-bit PC needs 32-bit libraries (gcc-multilib, or tools/setup_pc32.sh without root).
- Alternative, not taken: a 64-bit build with the pointer-holding fields widened (game C edited for the port).

(This entry used to sit under "Open" although it was decided. It was moved here in the readability pass of 3 Nov 2026.)

### Village before online (owner, 6 Oct 2026)

- The owner asked to prioritise the offline village (single player) over online multiplayer.
- Both live in the lobby overlay. Decompilation there takes Local_main and everything the offline village reaches first
  (NPCs, shops, item box, quest counter, house, farm, village menus), and online-only code (login/logout, plaza chat,
  mail, browser, rooms) last.
- The same order applies to main's network code versus its single-player code.
- Update, 6 Oct 2026 (evening): the offline village code in the lobby overlay has been mapped and mostly written. What is
  left there is near-matches and online-only code (login, rooms, plaza, browser). Lobby agents now take online code too,
  after their village near-matches.

### In-game web browser: paused (owner, 6 Oct 2026)

- The owner paused decompiling the lobby's in-game web browser (Bs*, tagAct_*, HTML tag/layout code, PNG/BMP glue; mostly
  0x5EE618-0x609700). The port will probably use its own browser instead of the original.
- The owner's understanding: it was only used for Capcom's website and buying the online subscription, not for any game
  function.
- Already-matched browser functions stay; no new work there. If a game function turns out to call into it, stub that call
  on the PC side.

### Original Xbox port starts 8 Oct 2026 (owner, 7 Oct 2026)

- The owner will provide a real modded original Xbox to deploy and test builds on, with a capture card attached so Claude
  can see the screen, and later a controller emulator Claude can drive.
- Xbox work starts on 8 Oct 2026. Until then the PC build stays the reference.
- Plan: both. xemu (emulator) for day-to-day iteration (fast, scriptable, screenshots without hardware), and the real Xbox
  for regular checks of what xemu gets wrong (64 MB memory limit, GPU details, timing, disc/HDD speed).
- xemu needs the owner's own BIOS, MCPX boot ROM and HDD image, dumped from their modded Xbox (never committed).

### Movies (Sofdec): libmpeg2 (owner, 7 Oct 2026)

- Play the movies from the player's own disc with a ready-made decoder, libmpeg2 (GPL v2; used by XBMC on the original
  Xbox), plus our own program-stream demux and the existing ADX decoder.
- No ffmpeg step, no converted files. Same code for PC, ARM and Xbox, attract movies included.
- Consequence: the port's source is shared under the GPL when distributed (the repo is public already).
- Not chosen: own decoder (1-2 weeks), ffmpeg pre-convert with pl_mpeg, capture/re-encode (quality loss, Capcom data per
  player).
- Agent B does it after the soft keyboard.
- Note: agent A's recommendation below was option A (own decoder). The owner chose libmpeg2 instead.

#### Earlier notes (agent A, round 22)

The movies are Sofdec files in AFS00.AFS: OPENING.sfd (79.6 MB, about 3.5 min), sp_mh.sfd (84 MB) and seven short attract
movies (WTR, FEED, HORN, SQURE, PUB, VOL, CAV; 7-32 MB). Read from the opening's first megabyte:
- an MPEG program stream;
- the video is MPEG-2 Main Profile @ Main Level, progressive sequence, 4:2:0, 29.97 fps, about 3 Mbit/s (the sequence
  header says 256x512, which looks odd and needs a second look);
- the audio is the stream 0xC0 holding a stereo 48 kHz 4-bit ADX, which the port already decodes.

The PC skipped the movie at that time.

Options (estimates, not measured):
- A. Own decoder in the repo, plain C, no library: program-stream demux (~150 lines), ADX into the existing stream mixer
  (~100), MPEG-2 video for progressive MP@ML only (I/P/B frames, VLC tables, IDCT, motion compensation; field pictures /
  dual prime can be left out because the stream is progressive): ~2500-3500 lines, about one to two weeks of agent time
  including checking it against a reference decoder frame by frame. Speed: MPEG-2 SD in C is roughly 5-15 ms per frame on
  a desktop core. The ARM box's Cortex-A53 cores and the 733 MHz Xbox CPU are several times slower, so 30 fps there is
  likely but not certain (the game is not running during the movie, so the whole frame budget is available; YUV->RGB can
  go to the GPU). Upside: works from the disc alone, like the rest of the port.
- B. A pre-converted file the player makes from their own disc (a script that extracts OPENING.sfd and runs ffmpeg into
  something trivial to play, e.g. MPEG-1 for a small MPEG-1-only decoder, or low-resolution frames). Cheaper for us (~2-4
  days with a small decoder), but every player needs ffmpeg and an extra setup step, and the converted file is Capcom data
  the player must keep next to the port. The Xbox build would still need some decoder.
- C. Keep skipping the movies (the state then).

Recommendation from agent A: A, after the single-player game is complete, since it keeps "own disc only, no extra tools"
and also covers the attract movies. Owner to decide. (Decided: libmpeg2, above.)

### Debug logs, bug reports and Windows test builds (owner, 8 Oct 2026)

- Every run writes a local debug log automatically (no memory dumps, no usernames/home paths, nothing sent anywhere).
- Players report bugs by attaching the zip from tools/bug_report to a GitHub issue (issue template). No automatic upload
  from the game (it would need an embedded GitHub token). A relay server can be revisited later.
- Windows (32-bit, cross-compiled) builds are made for testing.
- Update the same day: the owner approved public test builds on GitHub Releases (the repo is public already).
  - The exe contains no Capcom data. A one-time install copies the game files from the player's own Japanese ISO.
  - Releases are marked as test builds, say a Japanese disc is required, and credit Capcom.
  - Keep a backup of the repo elsewhere in case of a takedown.

### Monster Hunter G (PS2) is the main release; MH1 was the stepping stone (owner, 13 Oct 2026)

- Japanese MH1 was the starting point only because it kept its debug symbols. G is the definitive game and what the
  project will release: G with an English script, our modernizations (widescreen and the rest of the PC/Xbox platform
  layer), and later a Poogie-quest-style minigame (as in Wii G) on an ESP32 gadget.
- MH1 JP is still finished to a releasable state. Fixes found in one game are applied to the others. MH1 US maybe too.
- MH1 US (disc/mh1_us) matters for its official English script (which covers most text G shares with MH1) and one weapon
  type new over MH1 JP, which G also has.

### One codebase, several games; priorities G > MH1 JP > MH1 US (owner, 13 Oct 2026)

- One repo and one engine (platform layer, tools, server) for every version.
- Game C that is the same in two versions is written once. Where a version changed a function, that version gets its own
  copy.
- Each game is built from the player's own disc. A player with several discs picks the version in a launcher at start-up
  (precedent: the SM64 decomp builds JP/US/EU from one tree).
- No hybrid that mixes content between versions (keeps game logic faithful and online compatibility with real PS2
  players).
- Priorities: G is the main effort. MH1 JP is finished to release-ready, then maintenance (it stays the test bench: it has
  symbols). MH1 US is a cheap later build; for now its job is supplying the official English script.
- Rejected: separate repos per game (triple maintenance, drift).

### How G lives in the repo: a VERSION switch (agent B, 13 Oct 2026; fits the decision above)

- `VERSION=mhg` selects PS2 Monster Hunter G in the matching build (tools/setup_split.py, tools/build.py,
  tools/rebuild.sh, tools/progress.py). Unset or `mh1` is the MH1 JP build, unchanged (MH1 still rebuilds all five modules
  OK).
- Per-version config in `config/mhg/`: splat yamls, `symbols/` (MH1 names at G addresses, made by tools/mhg_map.py),
  `c_files.txt`, aliases.
- Split targets go in `build/mhg/split/`, asm in `asm/mhg/`, objects in `build/mhg/` (all gitignored, made from the
  player's disc).
- Game C is shared. config/mhg/c_files.txt names files relative to `src/`, so a matched MH1 file whose functions are
  unchanged in G (`game/eft/eft15`) is the same file in both builds.
- A function G changed gets either:
  - a G copy under `src/mhg/<module>/...` (only G builds it), or
  - an `#ifdef VERSION_MHG` inside the shared file, when the change is a line or two (G compiles with -DVERSION_MHG;
    MH1's flags are untouched).
- The MH1 build ignores src/mhg and asm/mhg.
- The PC/Xbox port follows the same rule: src/pc is shared; game-specific data addresses come from the version's symbol
  files (rt tables, gen_rt_auto, pc_abs), and the build picks the version's C list. A launcher picks the version at
  start-up when several discs are present.
- G symbol names keep MH1's spelling, including address suffixes of MH1 statics (`sound_call_005A3D10`), so shared C links
  unchanged. G-only functions are `func_XXXXXXXX` (main) / `<module>_func_XXXXXXXX` until named.
- (MH2 uses the same scheme with `VERSION=mh2`; see "MH2 (Dos)" below.)

### License: MIT for the project's own code (owner, 15 Oct 2026)

See LICENSE. No Capcom code or data is included or covered.

### Online: our own server and our own open PS2 patches, PS2 + PC + Xbox together (owner, 15 Oct 2026)

- The MH Oldschool route (connecting original discs, skipping DNAS) uses closed-source exploits, so we do our own thing.
  Most players use patched copies anyway, PS2 included.
- PS2 parity: from our byte-matching rebuild, build patched PS2 executables (DNAS skipped, our server's address, small
  fixes), delivered as a patch tool applied to the player's own disc image (no Capcom bytes in the repo). DNAS is a
  defunct online check; skipping it is what revival servers do (owner's call, made).
- In-hunt parity: our PC/Xbox co-op speaks net_peer frames, while the PS2 speaks the original mcsls records
  (docs/server.md, milestone S7). Our ports switch to the original in-hunt protocol (preferred) or the server translates,
  so PS2, PC and Xbox can hunt together. (16 Oct: the ports run the original protocol; see "In-hunt parity" below.)
- Order, after G's single player is solid: 1) MH1 PS2 patch (DNAS skip + server address) against our server; 2) in-hunt
  protocol parity; 3) G (its online code isn't ported yet).
- Still never connect to the MH Oldschool public server without its operators' permission.
- Real-PS2 delivery (owner, 15 Oct): the game's own card-patch slot needs a Sony DNAS container (tested in PCSX2,
  docs/network.md 3.8). So instead: one open launcher ELF that Free McBoot / OpenTuna auto-runs at power-on. It checks the
  disc ID, patches MH1 (later G) in RAM and boots it, and boots any other disc untouched. "Run homebrew once, then just
  boot the disc." No DNAS crypto work of any kind.
- PS2 English (owner, 15 Oct): the same launcher gets an English option. The text table is generated on the player's PC
  from their own US disc (as for our ports), copied to the memory card, and patched into RAM at boot and as overlays /
  quest files load (no 64 KB limit). Patched ISOs (PCSX2/OPL) can carry it built in. This comes after the online launcher
  works; it shares the English pipeline and its pending source decision (Freedom for G; decided 28 Oct, below).

### MH1 online on the Xbox: a separate online XBE (agent C, 15 Oct 2026; the owner may revisit)

- `tools/build_xbox.py --online` builds the ONLINE=1 PC build's objects for the Xbox (into build/xbox/online, its own
  objects) with nxdk's lwIP. The plain Xbox build stays without network code, as the plain PC build does.
- Why not online by default: build/pc holds one variant at a time (plain or ONLINE=1); the plain build is what every test
  and G use; and an always-online XBE would start the NIC code path in every run.
- `./mh1 xbox DISC --online` makes both.
- The server address on the Xbox: `online_server` in E:\Games\MH1\settings.ini (the PC's mh1pc.ini keys). Without it, the
  laptop in the loader's loader.txt, port 10200. Never a hard-coded address. MH Oldschool stays refused (net_cpinet.c).

### In-hunt parity: the PC / Xbox run the game's own session code; mh1-server relays the original records (agent C, 16 Oct 2026; step 2 of the online plan above)

- For a lobby match whose game rule (6914) is "mh1-mcs" (mh1-server `--lobby-mcs`), the PC and Xbox online builds link and
  run the PS2's own in-hunt stack (aq_nm.c, Cng, aqcmd, mcsls, the lobby's cmcs_* login, net_start_ck). The bytes on the
  wire are the PS2's, so a patched PS2, a PC and an Xbox can be in one hunt. No translator in the server.
- The server side is the faithful relay tools/server/mcs.py (0x1031 / 0x82 login, records passed on unchanged to the other
  members, health checks). The lobby registers each match's logins there at 6916.
- net_peer.c (direct connect, no server) and the "mh1-relay" mode stay as they are for now (tested, simpler to host). They
  cannot hunt with a PS2. Whether the relay mode is dropped once mcs is proven on real hardware: open.
- Why not translate in the server: both sides carry the same AQ packets, but mcsls adds sync flags, ping rounds, fragments
  and a start handshake that the PS2 waits for. Reproducing them in Python would be a second, untested copy of code we
  already have decompiled.

### Xbox: one network loader per game (owner's request, 16 Oct 2026)

- Each game folder holds its own loader as default.xbe (E:\Games\MH1 "Monster Hunter", E:\Games\MHG "Monster Hunter G");
  the game lives next to it as mh1.xbe / mhg.xbe. Starting either one updates that game from the laptop; without the
  laptop it starts the copy on the disk.
- One program, two XBE titles (the dashboard lists folders by title). The game is picked at run time from the loader's
  folder: a "game=" line in its loader.txt, else the folder name.
- Rejected: a compile-time game switch (two different objects to keep apart in one build directory), and only the folder
  name (a renamed folder would silently become MH1).
- Requests carry `?game=`. Requests without it are MH1, so the loader installed before 16 Oct keeps working with the new
  server. One server process serves both games.
- Games return to their own loader on exit again (it only sends the log, then the dashboard). The 11 Oct network freeze
  came from a polling loader, not from the return itself.

### Lockstep referee against the real game; fixes go to both games (owner, 17 Oct 2026)

- Matching effort goes where G differs from MH1 (ranked by how often a player meets it), not into bulk matching of
  functions G shares with MH1: those already run as our MH1 code.
- Next: an automatic referee. The real game (owner's disc in PCSX2) and the port run side by side with the same pad input;
  the game state (hunter/monster positions, HP, items, flags) is compared each tick. A difference points at a game-code
  gap (G still on MH1 logic) or at the port's glue (table sizes, host stand-ins). Built for G first, and for MH1 JP with
  the same tool.
- Owner rule: every fix found this way (or any other way) that also applies to MH1 goes into MH1 too, and the reverse.

### PS2 float rounding on by default; quest clock left as is (owner, 18 Oct 2026)

- The referee (docs/referee.md) showed the PS2 rounds floats toward zero and the PC rounds to nearest. With the PC set to
  round like the PS2 (`RT_FPU_PS2`), fights stay identical to the real game much longer.
- Decided: make it the default for MH1 and G, on PC and Xbox, after a test round that checks drawing, sound, movies and the
  online/co-op paths are unaffected (`RT_FPU_PS2=0` keeps the old behaviour for comparison).
- The PS2 quest clock runs during area loads. The port loads almost instantly, so players get a few seconds more per quest.
  Decided: leave it (a side effect of faster loading, not a change to the game's rules).

### G: "Dual Blades", not "Dual Swords" (owner, 25 Oct 2026)

- MH1 US named the class Dual Swords. Freedom uses both (checked 28 Oct: 6 of its G-paired lines say Dual Swords, 5 Dual
  Blades). The later series says Dual Blades, and a US G could have gone either way, so G follows what most players know.
- G's text table rewrites the term (tools/text_map.py G_TERMS). MH1's English keeps its own disc's wording.

### G English: Monster Hunter Freedom first, MH1 US second, our own lines last (owner, 28 Oct 2026)

- Where a G string has a Freedom pair, Freedom's line is used, also where MH1 US had English. (Before: MH1 US first,
  Freedom only for strings MH1 US lacked.)
- MH1 US is the fallback. Our own lines (text/mhg_en_own.tsv) fill what neither has.
- Freedom's names for items, skills and the like replace MH1 US's in G lines from the other sources, so one thing is not
  named two ways. Kept: G_TERMS (Dual Blades).
- Exceptions, by rule. The MH1 US line stays in place when the Freedom line:
  - would not fit where the MH1 US line fits (more lines, or wider than the US line, G's Japanese and the table's widest
    US line; talk excepted);
  - drops the US line's colour codes or page counter; or
  - is Freedom's placeholder "dummy".
- MH1 JP keeps its own US disc text, unchanged. tools/text_map.py; numbers in docs/english_g.md.

### MH2 (Dos): from "build on the community repo" to "decompile it ourselves" (owner, 15 Oct and 1 Nov 2026)

What was decided on 15 Oct (owner), kept for the record:
- https://github.com/Batenkaitos-dev/mh2-decomp: MH2 NTSC-J 1.04, all 12,551 Capcom functions byte-matching (stripped
  game: its function names are AI guesses), no PC port. When MH2's turn comes, start from it and port it onto our engine
  instead of decompiling again.
- Its repo has no license yet (the author's stated intent is collaboration). Reading it and using facts (names, offsets,
  behaviour) is fine; copying its code needs a license or the author's go-ahead first.
- Possible exchange: our symbol-backed MH1/G names for shared engine code; our platform layer.

**Changed 1 Nov 2026 (owner):** the author never answered, so we split and decompile MH2 ourselves from the owner's disc
with our tools, and do not start from their code.
- Their repo stays a reference for facts only (what a function does, offsets, formats). Their C, names, comments and file
  layout stay out of ours.
- Not legal advice; re-read their license state first.
- Confirmed by the owner, 3 Nov 2026: the repo **is** an allowed source of facts. The MH2 docs written so far did not
  open it (they say so), and the readability-pass brief kept away from it; neither of those is a rule. What stays out
  is their C, names, comments, text and file layout.

How MH2 lives in the repo (agent, 2 Nov 2026, owner asleep; docs/mh2.md):
- MH2 lives beside G in the same tree (VERSION=mh2, config/mh2, src/mh2 for MH2-only C), and shares unchanged MH1 C
  files.
- dnas_net/dnas_ins are split too, unlike MH1/G, because the task asked for every overlay: bytes only, no work on them.
- Compiler 3.0b52 with -sdatathreshold 0.
- The community repo was not opened.

### MH Dos gets PC and Xbox ports (owner, 2 Nov 2026)

- MH Dos (MH2, JP 1.04, disc/mh2) gets PC and Xbox ports like MH1 and G. PC first; the Xbox follows through the same
  platform layer.
- Same method: matched C plus the host runtime in src/pc/rt, no recompiler, and it does not need to be 100% decompiled.
- Decided for the first step, by the agent:
  - VERSION=mh2 builds on G's form of the shared C (-DVERSION_MHG -DVERSION_MH2) in its own tree, tools/build_pc_mh2.sh.
  - `rt_ver_mhg()` is true on MH2 until each use is reviewed.
  - MH2's boot is its own PC file, src/pc/title_mh2.c, because MH2 rewrote it.
  - Decompile agents work on what the port needs: ranked list in docs/mh2_port.md section 6.
- Open for the owner: first town, or first quest through a `--quest` shortcut as G did (docs/mh2_port.md section 6). See
  "Open" below.

## Rejected

### Porting the Wii version of Monster Hunter G instead (4 Oct 2026)

- It has no online servers to connect to, so a server would have to be written too.
- Its English patch is less complete than the PS2 G patch.
- The Wii is big-endian PowerPC: all binary data and network structures would need byte-swapping. PS2 and Xbox are both
  little-endian.
- The Wii has more RAM than the Xbox; the PS2 has less.
- No sign of a symbol table was found for it.

In its favour: GameCube/Wii decompilation tooling is more mature, and its fixed-function graphics API would probably map
onto the Xbox GPU more cleanly than PS2 vertex microcode (a judgment, not verified). Not enough to outweigh the missing
servers.

Other rejections are listed inside their decisions above: static recompilation as the method (3 Oct), non-matching
decompilation as the default (4 Oct), separate repos per game (13 Oct), a compile-time game switch for the Xbox loader
(16 Oct), the other movie options (7 Oct).

## Settled 3 Nov 2026 (owner)

### Static recompilation (PS2Recomp, hybrid build): no

The owner: "we're long past PS2Recomp". The port is matched or near-matched C plus the host runtime, for MH1, G and MH2.
This closes the "hybrid build" question that was open since 5 Oct.

What was open, kept for the record: recompile the remaining PS2 code mechanically (PS2Recomp or our own) and link it with
the native graphics/sound/input layer, swapping in decompiled C as it lands. Faster to playable on PC; probably too slow
for the original Xbox, which still needs real C. Untested on this game. (PS2Recomp was tried once, 5 Oct, docs/STATUS.md.)

### English text: official sources first, the community MH1j patch much later if at all

For G the order is Freedom, then MH1 US, then our own lines (28 Oct). MH1 JP uses its own US disc text. The community
MH1j English patch stays a possible option far down the line, very low priority. If it is ever used, it needs the
translators' consent and credit (CLAUDE.md rules).

What was open, kept for the record: the community MH1j English patch (needs the translators' consent), text from the US
disc (supplied by the user's own copy), or a fresh translation.

### The community mh2-decomp repo: an allowed source of facts

See "MH2 (Dos)" above. Facts (what a function does, offsets, formats) may be taken from it. Its C, names, comments, text
and file layout stay out of ours while it has no license.

## Open

### Server

Ask MH Oldschool about an unofficial client, or build a private server. Their game server code did not appear to be
public.

**Mostly settled:** on 15 Oct the owner chose our own server ("Online: our own server" above). What is left open is only
whether to ask MH Oldschool about letting an unofficial client in. Never connect to their public server without their
operators' permission.

### mh1-relay mode

Whether the "mh1-relay" mode is dropped once mcs is proven on real hardware (from "In-hunt parity", 16 Oct).

### MH2: first town or first quest (from 2 Nov)

First town, or first quest through a `--quest` shortcut as G did (docs/mh2_port.md section 6). The HUNT agent of 3 Nov
worked on `--quest` (docs/mh2/hunt.md); no owner decision is recorded.

### Idea: all three side by side on real hardware, synced (owner, 21 Oct 2026)

Run the real PS2 (own disc), the real Xbox (our port) and the PC port side by side with the same scripted input, to see on
real hardware what still differs (the referee does this with PCSX2).
- Input: the owner's PadLink (PS2 pad emulator, PAD LINK/) and Xbox pad dongle (MULTI KM/xboxpad/) replay one script into
  each console; the PC takes --input.
- Start: the same save and quest, from power-on (no memory pokes on a real PS2).
- Video: capture both consoles (the capture card used before for the Xbox), make a side-by-side and a per-frame difference
  to find the first divergence.
- When: later, after the current Xbox bug round.
