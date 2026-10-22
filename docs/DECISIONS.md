# Decisions

Dates are when the decision was made in planning. Revise freely once real
data from the disc contradicts something here, and say why.

## Decided

**Base on a Japanese PS2 build, not the US/EU build.** (3 Oct 2026)
The Japanese MH1 executable is reported to ship with a symbol table, and the
MH Oldschool community servers support only the NTSC-J games. The US/EU
release uses different netcode that has not been revived. The Japanese build
is also reported to be easier than the US one (more forgiving monster AI),
which matters for anyone expecting US balance.

**Decompile to portable C, then retarget.** (3 Oct 2026)
Static recompilation was considered and rejected: PS2Recomp is experimental,
targets modern PCs, and its graphics and VU1 support are incomplete. Its
runtime model is also a poor fit for a 733 MHz CPU with 64 MB of RAM.

**Use nxdk for the Xbox side.** (3 Oct 2026)
Open source, includes pbkit (GPU), vertex program and register combiner
compilers, an lwIP network stack, SDL2 and USB. Avoids the leaked Microsoft
XDK entirely.

**Keep the PS2 wire protocol, remove DNAS.** (3 Oct 2026)
Lets an Xbox client share lobbies with PS2 and PCSX2 players if the server
operators agree. Needs their cooperation or a self-hosted server.

**Bilingual via two string tables and a menu toggle.** (3 Oct 2026)
Keep the Japanese font and layouts; fit English into them.

**PC backend before Xbox backend.** (3 Oct 2026)
Same platform interface, far quicker to debug.

**Base on Japanese MH1 (SLPM_654.95), not PS2 G.** (4 Oct 2026)
Survey: MH1 has 50,151 symbols with sizes and per-overlay relocations;
G (SLPM_658.69) and its overlays are fully stripped. G stays a possible
later target: it is built on the same engine (same compiler, same overlay
layout, many identical overlay names), so MH1's names should transfer.

**Matching decompilation with Metrowerks CodeWarrior for PS2, 3.0 family.** (4 Oct 2026)
Five MH1 functions byte-match with every 3.0-family mwcps2 build from
decomp.me at -O3/-O4; the 3.0.1 family does not. Working default
mwcps2-3.0b52-030722 -O4,p until bigger functions narrow it. The .comment
"2.4.1.01" string is from the linker and identifies nothing.
Rejected alternative: non-matching, behaviour-checked decompilation.
Kept as a fallback for any file that resists matching.

- 2026-10-05, owner: priority is "playable and recognizable first, polished
  afterwards". Byte-matching stays the correctness check, but agents cap time
  on stubborn functions (~10 min) and park them as near-matches whose logic is
  believed complete. Work on the platform layer (Capcom's fl graphics library
  replacement, model format) starts now in parallel instead of after the
  decomp. Whether to use static recompilation for not-yet-decompiled code to
  reach "playable on PC" sooner is still open (see Open).

- 2026-10-05, owner: the ultimate goal is player experience. Platform layers
  (rendering, audio, input, files, saves, movies, windowing, widescreen) are
  written from scratch for PC and Xbox; nothing PS2-specific (IOP, VU, DMA,
  GS, disc drive, memory card) is emulated. What players feel is kept faithful
  from the original code: game logic (monster AI, hunter movement, weapons,
  hit detection, damage, camera, quests, items, menus) and what defines the
  look (models, textures, animation timing, lighting rules, effects).
  Byte-matching is a verification tool for that game-logic C, not a goal; it
  never blocks progress, and exact equivalence is polish for later. Same
  approach as the owner's Tonic Trouble project (new native engine on the
  original data, gameplay-accurate). PS2Recomp, if used, is scaffolding to run
  not-yet-decompiled logic, never the shipped product.

**Monster Hunter G (PS2) is the main release; MH1 was the stepping stone.** (owner, 13 Oct 2026)
- Japanese MH1 was the starting point only because it kept its debug symbols. G is the
  definitive game and what the project will release: G with an English script, our
  modernizations (widescreen and the rest of the PC/Xbox platform layer), and later a
  Poogie-quest-style minigame (as in Wii G) on an ESP32 gadget.
- MH1 JP is still finished to a releasable state; fixes found in one game are applied to the
  others. MH1 US maybe too.
- MH1 US (disc/mh1_us) matters for its official English script (which covers most text G shares
  with MH1) and one weapon type new over MH1 JP, which G also has.

**One codebase, several games; priorities G > MH1 JP > MH1 US.** (owner, 13 Oct 2026)
- One repo and one engine (platform layer, tools, server) for every version. Game C that is the
  same in two versions is written once; where a version changed a function, that version gets its
  own copy. Each game is built from the player's own disc; a player with several discs picks the
  version in a launcher at start-up (precedent: the SM64 decomp builds JP/US/EU from one tree).
- No hybrid that mixes content between versions (keeps game logic faithful and online
  compatibility with real PS2 players).
- Priorities: G is the main effort. MH1 JP is finished to release-ready, then maintenance (it
  stays the test bench: it has symbols). MH1 US is a cheap later build; now its job is supplying
  the official English script.
- Rejected: separate repos per game (triple maintenance, drift).

**How G lives in the repo: a VERSION switch** (agent B, 13 Oct 2026; fits the decision above)
- `VERSION=mhg` selects PS2 Monster Hunter G in the matching build (tools/setup_split.py,
  tools/build.py, tools/rebuild.sh, tools/progress.py); unset or `mh1` is the MH1 JP build,
  unchanged (MH1 still rebuilds all five modules OK).
- Per-version config in `config/mhg/`: splat yamls, `symbols/` (MH1 names at G addresses,
  made by tools/mhg_map.py), `c_files.txt`, aliases. Split targets in `build/mhg/split/`,
  asm in `asm/mhg/`, objects in `build/mhg/` (all gitignored, made from the player's disc).
- Game C is shared: config/mhg/c_files.txt names files relative to `src/`, so a matched MH1
  file whose functions are unchanged in G (`game/eft/eft15`) is the same file in both builds.
  A function G changed gets a G copy under `src/mhg/<module>/...` (only G builds it), or an
  `#ifdef VERSION_MHG` inside the shared file when the change is a line or two (G compiles
  with -DVERSION_MHG; MH1's flags are untouched). The MH1 build ignores src/mhg and asm/mhg.
- The PC/Xbox port follows the same rule: src/pc is shared; game-specific data addresses come
  from the version's symbol files (rt tables, gen_rt_auto, pc_abs), and the build picks the
  version's C list. A launcher picks the version at start-up when several discs are present.
- G symbol names keep MH1's spelling, including address suffixes of MH1 statics
  (`sound_call_005A3D10`), so shared C links unchanged. G-only functions are
  `func_XXXXXXXX` (main) / `<module>_func_XXXXXXXX` until named.

**MH2 (Dos) later builds on the community mh2-decomp.** (owner, 15 Oct 2026)
- https://github.com/Batenkaitos-dev/mh2-decomp: MH2 NTSC-J 1.04, all 12,551 Capcom functions
  byte-matching (stripped game: its function names are AI guesses), no PC port. When MH2's turn comes,
  start from it and port it onto our engine instead of decompiling again.
- Its repo has no license yet (the author's stated intent is collaboration): reading it and using facts
  (names, offsets, behaviour) is fine; copying its code needs a license or the author's go-ahead first.
- Possible exchange: our symbol-backed MH1/G names for shared engine code; our platform layer.
- Our repo: MIT license for the project's own code (owner, 15 Oct 2026; LICENSE).

**Online: our own server and our own open PS2 patches, PS2 + PC + Xbox together.** (owner, 15 Oct 2026)
- The MH Oldschool route (connecting original discs, skipping DNAS) uses closed-source exploits, so we do our
  own thing. Most players use patched copies anyway, PS2 included.
- PS2 parity: from our byte-matching rebuild, build patched PS2 executables (DNAS skipped, our server's
  address, small fixes), delivered as a patch tool applied to the player's own disc image (no Capcom bytes in
  the repo). DNAS is a defunct online check; skipping it is what revival servers do (owner's call, made).
- In-hunt parity: our PC/Xbox co-op speaks net_peer frames while the PS2 speaks the original mcsls records
  (docs/server.md, milestone S7): our ports switch to the original in-hunt protocol (preferred) or the
  server translates, so PS2, PC and Xbox can hunt together.
- Order, after G's single player is solid: 1) MH1 PS2 patch (DNAS skip + server address) against our
  server; 2) in-hunt protocol parity; 3) G (its online code isn't ported yet).
- Still never connect to the MH Oldschool public server without its operators' permission.
- Real-PS2 delivery (owner, 15 Oct): the game's own card-patch slot needs a Sony DNAS container (tested in
  PCSX2, docs/network.md 3.8), so instead: one open launcher ELF that Free McBoot / OpenTuna auto-runs at
  power-on; it checks the disc ID, patches MH1 (later G) in RAM and boots it, and boots any other disc
  untouched. "Run homebrew once, then just boot the disc." No DNAS crypto work of any kind.
- PS2 English (owner, 15 Oct): the same launcher gets an English option — the text table is generated on the
  player's PC from their own US disc (as for our ports), copied to the memory card, and patched into RAM at
  boot and as overlays / quest files load (no 64 KB limit). Patched ISOs (PCSX2/OPL) can carry it built in.
  After the online launcher works; shares the English pipeline and its pending source decision (Freedom for G).

**In-hunt parity: the PC / Xbox run the game's own session code; mh1-server relays the original records.** (agent C,
16 Oct 2026; step 2 of the online plan above)
- For a lobby match whose game rule (6914) is "mh1-mcs" (mh1-server `--lobby-mcs`), the PC and Xbox online builds link
  and run the PS2's own in-hunt stack (aq_nm.c, Cng, aqcmd, mcsls, the lobby's cmcs_* login, net_start_ck): the bytes
  on the wire are the PS2's, so a patched PS2, a PC and an Xbox can be in one hunt. No translator in the server.
- The server side is the faithful relay tools/server/mcs.py (0x1031 / 0x82 login, records passed on unchanged to the
  other members, health checks); the lobby registers each match's logins there at 6916.
- net_peer.c (direct connect, no server) and the "mh1-relay" mode stay as they are for now (tested, simpler to host);
  they cannot hunt with a PS2. Whether the relay mode is dropped once mcs is proven on real hardware: open.
- Why not translate in the server: both sides carry the same AQ packets, but mcsls adds sync flags, ping rounds,
  fragments and a start handshake that the PS2 waits for; reproducing them in Python would be a second, untested copy
  of code we already have decompiled.

**MH1 online on the Xbox: a separate online XBE** (agent C, 15 Oct 2026; owner may revisit)
- `tools/build_xbox.py --online` builds the ONLINE=1 PC build's objects for the Xbox (into build/xbox/online, its own
  objects) with nxdk's lwIP; the plain Xbox build stays without network code, as the plain PC build does. Why not
  online by default: build/pc holds one variant at a time (plain or ONLINE=1), the plain build is what every test and
  G use, and an always-online XBE would start the NIC code path in every run. `./mh1 xbox DISC --online` makes both.
- The server address on the Xbox: `online_server` in E:\Games\MH1\settings.ini (the PC's mh1pc.ini keys); without it
  the laptop in the loader's loader.txt, port 10200. Never a hard-coded address. MH Oldschool stays refused
  (net_cpinet.c).

## Rejected

**Porting the Wii version of Monster Hunter G instead.** (4 Oct 2026)
- It has no online servers to connect to, so a server would have to be
  written too.
- Its English patch is less complete than the PS2 G patch.
- The Wii is big-endian PowerPC: all binary data and network structures
  would need byte-swapping. PS2 and Xbox are both little-endian.
- The Wii has more RAM than the Xbox; the PS2 has less.
- No sign of a symbol table was found for it.
In its favour: GameCube/Wii decompilation tooling is more mature, and its
fixed-function graphics API would probably map onto the Xbox GPU more
cleanly than PS2 vertex microcode (a judgment, not verified). Not enough to
outweigh the missing servers.

## Open

**Where the English text comes from.**
Options: the community MH1j English patch (needs the translators' consent),
text from the US disc (supplied by the user's own copy), or a fresh
translation.

**Server.** Ask MH Oldschool about an unofficial client, or build a private
server. Their game server code did not appear to be public.
- Hybrid build: recompile the remaining PS2 code mechanically (PS2Recomp or
  our own) and link it with the native graphics/sound/input layer, swapping in
  decompiled C as it lands. Faster to playable on PC; probably too slow for the
  original Xbox, which still needs real C. Untested on this game.

**PC host is a 32-bit build (decided 5 Oct 2026).**
The decompiled game C keeps pointers in u32 fields (e.g.
`flSetRenderState(0x19, (u32)&uv)`) and its structs (SETW, PRIM, CLAY,
PLW...) must keep their PS2 offsets. Building the PC port with `gcc -m32`
gives 4-byte pointers, so that code compiles unchanged; static asserts in
src/pc/rt/rt_game.c check the struct sizes (PLW 0xA00, GAME_W 0x224, CLAY
0x8C) on every build. The original Xbox is 32-bit x86 too, so the same C
goes there. Cost: a modern 64-bit PC needs 32-bit libraries (gcc-multilib,
or tools/setup_pc32.sh without root). Alternative, not taken yet: a 64-bit
build with the pointer-holding fields widened (game C edited for the port).
Confirmed by the owner, 5 Oct 2026: 32-bit it is.

## Village before online (decided 6 Oct 2026)

The owner asked to prioritise the offline village (single player) over online multiplayer.
Both live in the lobby overlay: decompilation there takes Local_main and everything the
offline village reaches first (NPCs, shops, item box, quest counter, house, farm, village
menus), and online-only code (login/logout, plaza chat, mail, browser, rooms) last. The same
order applies to main's network code versus its single-player code.
- Update 6 Oct 2026 (evening): the offline village code in the lobby overlay has been mapped and
  mostly written; what's left there is near-matches and online-only code (login, rooms, plaza,
  browser). Lobby agents now take online code too, after their village near-matches.

## In-game web browser: paused (decided 6 Oct 2026)

The owner paused decompiling the lobby's in-game web browser (Bs*, tagAct_*, HTML tag/layout
code, PNG/BMP glue; mostly 0x5EE618-0x609700): the port will probably use its own browser
instead of the original. The owner's understanding: it was only used for Capcom's website and buying
the online subscription, not for any game function. Already-matched browser functions stay; no
new work there. If a game function turns out to call into it, stub that call on the PC side.

## Original Xbox port starts 8 Oct 2026 (decided 7 Oct 2026)

The owner will provide a real modded original Xbox to deploy and test builds on, with a capture
card attached so Claude can see the screen, and later a controller emulator Claude can drive.
Xbox work starts on 8 Oct 2026; until then the PC build stays the reference.
Plan: both. xemu (emulator) for day-to-day iteration — fast, scriptable, screenshots without
hardware — and the real Xbox for regular checks of what xemu gets wrong (64 MB memory limit, GPU
details, timing, disc/HDD speed). xemu needs the owner's own BIOS, MCPX boot ROM and HDD image,
dumped from their modded Xbox (never committed).

## Movies (Sofdec): libmpeg2 (decided 7 Oct 2026)

Owner's decision: play the movies from the player's own disc with a ready-made
decoder, libmpeg2 (GPL v2; used by XBMC on the original Xbox), plus our own
program-stream demux and the existing ADX decoder. No ffmpeg step, no
converted files. Same code for PC, ARM and Xbox, attract movies included.
The port's source is therefore shared under the GPL when distributed (the repo
is public already). Not chosen: own decoder (1-2 weeks), ffmpeg pre-convert
with pl_mpeg, capture/re-encode (quality loss, Capcom data per player).
Agent B does it after the soft keyboard.

### Earlier notes (agent A, round 22)

The movies are Sofdec files in AFS00.AFS: OPENING.sfd (79.6 MB, about 3.5 min),
sp_mh.sfd (84 MB) and seven short attract movies (WTR, FEED, HORN, SQURE, PUB,
VOL, CAV; 7-32 MB). Read from the opening's first megabyte: an MPEG program
stream; the video is MPEG-2 Main Profile @ Main Level, progressive sequence,
4:2:0, 29.97 fps, about 3 Mbit/s (the sequence header says 256x512, which looks
odd and needs a second look); the audio is the stream 0xC0 holding a stereo
48 kHz 4-bit ADX, which the port already decodes. The PC now skips the movie.

Options (estimates, not measured):
- A. Own decoder in the repo, plain C, no library: program-stream demux
  (~150 lines), ADX into the existing stream mixer (~100), MPEG-2 video for
  progressive MP@ML only (I/P/B frames, VLC tables, IDCT, motion compensation;
  field pictures / dual prime can be left out because the stream is
  progressive): ~2500-3500 lines, about one to two weeks of agent time
  including checking it against a reference decoder frame by frame. Speed:
  MPEG-2 SD in C is roughly 5-15 ms per frame on a desktop core; the ARM box's
  Cortex-A53 cores and the 733 MHz Xbox CPU are several times slower, so 30 fps
  there is likely but not certain (the game is not running during the movie,
  so the whole frame budget is available; YUV->RGB can go to the GPU).
  Upside: works from the disc alone, like the rest of the port.
- B. A pre-converted file the player makes from their own disc (a script that
  extracts OPENING.sfd and runs ffmpeg into something trivial to play, e.g.
  MPEG-1 for a small MPEG-1-only decoder, or low-resolution frames). Cheaper
  for us (~2-4 days with a small decoder), but every player needs ffmpeg and
  an extra setup step, and the converted file is Capcom data the player must
  keep next to the port. The Xbox build would still need some decoder.
- C. Keep skipping the movies (today's state).
Recommendation from agent A: A, after the single-player game is complete, since
it keeps "own disc only, no extra tools" and also covers the attract movies.
Owner to decide.

## Debug logs, bug reports and Windows test builds (owner, 8 Oct 2026)

- Every run writes a local debug log automatically (no memory dumps, no
  usernames/home paths, nothing sent anywhere).
- Players report bugs by attaching the zip from tools/bug_report to a GitHub
  issue (issue template); no automatic upload from the game (it would need an
  embedded GitHub token). A relay server can be revisited later.
- Windows (32-bit, cross-compiled) builds are made for testing. Update the
  same day: the owner approved public test builds on GitHub Releases (the repo
  is public already). The exe contains no Capcom data; a one-time install
  copies the game files from the player's own Japanese ISO. Releases are
  marked as test builds, say a Japanese disc is required, and credit Capcom.
  Keep a backup of the repo elsewhere in case of a takedown.

## Xbox: one network loader per game (owner's request, 16 Oct 2026)

- Each game folder holds its own loader as default.xbe (E:\Games\MH1 "Monster Hunter",
  E:\Games\MHG "Monster Hunter G"); the game lives next to it as mh1.xbe / mhg.xbe. Starting
  either one updates that game from the laptop, without the laptop it starts the copy on the disk.
- One program, two XBE titles (the dashboard lists folders by title). The game is picked at run
  time from the loader's folder: a "game=" line in its loader.txt, else the folder name. Rejected:
  a compile-time game switch (two different objects to keep apart in one build directory) and
  only the folder name (a renamed folder would silently become MH1).
- Requests carry `?game=`; requests without it are MH1, so the loader installed before 16 Oct
  keeps working with the new server. One server process serves both games.
- Games return to their own loader on exit again (it only sends the log, then the dashboard):
  the 11 Oct network freeze came from a polling loader, not from the return itself.

### Lockstep referee against the real game; fixes go to both games (owner, 17 Oct 2026)
- Matching effort goes where G differs from MH1 (ranked by how often a player meets it), not into bulk matching of
  functions G shares with MH1: those already run as our MH1 code.
- Next: an automatic referee. The real game (owner's disc in PCSX2) and the port run side by side with the same pad
  input; the game state (hunter/monster positions, HP, items, flags) is compared each tick. A difference points at a
  game-code gap (G still on MH1 logic) or at the port's glue (table sizes, host stand-ins). Built for G first, and for
  MH1 JP with the same tool.
- Owner rule: every fix found this way (or any other way) that also applies to MH1 goes into MH1 too, and the reverse.

### PS2 float rounding on by default; quest clock left as is (owner, 18 Oct 2026)
- The referee (docs/referee.md) showed the PS2 rounds floats toward zero and the PC rounds to nearest; with the PC set
  to round like the PS2 (`RT_FPU_PS2`), fights stay identical to the real game much longer. Decided: make it the default
  for MH1 and G, on PC and Xbox, after a test round that drawing, sound, movies and the online/co-op paths are unaffected
  (`RT_FPU_PS2=0` keeps the old behaviour for comparison).
- The PS2 quest clock runs during area loads; the port loads almost instantly, so players get a few seconds more per
  quest. Decided: leave it (a side effect of faster loading, not a change to the game's rules).

### Idea (owner, 21 Oct 2026): all three side by side on real hardware, synced
Run the real PS2 (own disc), the real Xbox (our port) and the PC port side by side with the same scripted input, to see
on real hardware what still differs (the referee does this with PCSX2). Input: the owner's PadLink (PS2 pad emulator,
PAD LINK/) and Xbox pad dongle (MULTI KM/xboxpad/) replay one script into each console, the PC takes --input. Start:
the same save and quest, from power-on (no memory pokes on a real PS2). Video: capture both consoles (the capture card
used before for the Xbox), make a side-by-side and a per-frame difference to find the first divergence. Later, after the
current Xbox bug round.
