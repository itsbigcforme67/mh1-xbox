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

**PC host is a 32-bit build (proposed 5 Oct 2026, agent A).**
The decompiled game C keeps pointers in u32 fields (e.g.
`flSetRenderState(0x19, (u32)&uv)`) and its structs (SETW, PRIM, CLAY,
PLW...) must keep their PS2 offsets. Building the PC port with `gcc -m32`
gives 4-byte pointers, so that code compiles unchanged; static asserts in
src/pc/rt/rt_game.c check the struct sizes (PLW 0xA00, GAME_W 0x224, CLAY
0x8C) on every build. The original Xbox is 32-bit x86 too, so the same C
goes there. Cost: a modern 64-bit PC needs 32-bit libraries (gcc-multilib,
or tools/setup_pc32.sh without root). Alternative, not taken yet: a 64-bit
build with the pointer-holding fields widened (game C edited for the port).
Open until the owner confirms.
