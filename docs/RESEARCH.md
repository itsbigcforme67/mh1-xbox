# Research notes

Gathered by web search on 3-4 October 2026. Each finding lists its source.
Community pages change, so recheck anything a decision depends on.

## The game and its builds

- The Japanese release of Monster Hunter is on a community list of PS2 games
  that shipped with debug symbols. Not yet confirmed against the actual file.
  https://tcrf.net/User:Kojin/PS2_Games_With_Debug_Symbols
- The disc contains unused "xb" versions of the HUD ("cockpit") textures
  with Xbox button graphics, next to the "ps" ones that are used. PS2 G has
  the same leftovers.
  https://tcrf.net/Monster_Hunter
  https://tcrf.net/Monster_Hunter_G_(PlayStation_2)
- Notes on how the builds descend from each other (JP MH1, US, PAL, PS2 G,
  then Wii G, MH2 and the portable games):
  https://tcrf.net/User:2Tie/monhun
- Data is held in an AFS archive (AFS_DATA.AFS on PS2 G), whose entries
  include packed files such as sub_main.bin and lobby.bin that need a second
  unpacking step. Existing community tools:
  https://github.com/GReinoso96/GDataTool
  https://github.com/MaikelChan/AFSPacker
- An MH2 mod project shows that game's unpacked module layout, including
  separate DNAS and game modules, and uses armips for patching. Useful as a
  hint for how the series splits its code.
  https://github.com/GReinoso96/MH2Plus
- A write-up of reverse engineering MH2's asset compression, with the
  decompression routine. The same scheme may or may not apply to MH1.
  https://break-arts.com/posts/mh2_re/

## Online play

- MH Oldschool runs private servers for the NTSC-J versions of MH1, G and
  MH2. Clients connect by changing the DNS setting in the game's network
  configuration. The project began in 2019 as an offshoot of a Biohazard
  Outbreak fan server. The original service ran on KDDI's Multi-Matching BB.
  Capcom also distributed patches over the network that were stored on the
  memory card.
  https://mholdschool.com/viewtopic.php?t=120
- The US and EU versions use a different server structure that had not been
  worked out, and as of February 2026 nothing was in progress. The same
  thread says the Japanese build is easier than the US one.
  https://mholdschool.com/viewtopic.php?t=121
  https://mholdschool.com/viewtopic.php?p=3826
- All MH1 event quests (JP and US/EU) were implemented server-side by
  December 2024.
  https://mholdschool.com/viewtopic.php?p=3355
- Their public GitHub has a DNS server and file tools. The game server
  itself was not seen there.
  https://github.com/MH-Oldschool

## English text

- MH1 Japanese build English patch, v9 at time of research:
  https://mholdschool.com/viewtopic.php?t=122
- PS2 G "English Remix" patch, Rev. 15, drawing its official terms from the
  US MH1 and Monster Hunter Freedom. Its author states the files may be
  reused by others:
  https://mholdschool.com/viewtopic.php?t=1143
- Wii G English patch. States that Wii G has no online servers and that the
  PS2 G patch is more complete. Much of Wii G's text sits inside main.dol.
  https://mholdschool.com/viewtopic.php?t=1205
  https://mholdschool.com/viewtopic.php?t=205

## Other decompilation work on the same game (checked 5 Oct 2026)

- 2Tie/mh1j: a matching decompilation of the same Japanese release
  (SLPM_654.95), also MetroWerks + splat, using decomp.me presets. Its README
  (read via a summarising fetch, not checked line by line) puts it at about
  1% of the main ELF and ~0.5% overall, and says it does not accept code
  produced by AI/LLMs. So we cannot contribute to it, and we should not copy
  its C into this repo; at most use it as a reference for names, with credit,
  and ideally after asking its author. The tcrf page User:2Tie/monhun above is
  probably the same person (unverified).
  https://github.com/2Tie/mh1j
- GReinoso96/MH1Plus: a control/balance patch for the US release
  (SLUS_208.96) by armips injection; documents AFS and PZZ compression.
  https://github.com/GReinoso96/MH1Plus
- No public model viewer or Blender/Noesis importer for the PS2 games' model
  format turned up in a web search (only later games: MHFU, MHGU, MHW, MHRise).
- PS2Recomp (static recompiler) is still experimental; its author says the GS
  (graphics) side is the main thing that does not work yet.

## A planned Xbox version (owner, 5 Oct 2026)

- The owner says an Xbox version of Monster Hunter was planned. Not checked
  against a source here. Possibly related: agent A found paired cockpit (HUD)
  textures in AFS_DATA such as cpit1ps.apx and cpit1xb.apx (docs/formats/
  graphics.md); "xb" may mean Xbox. Unverified until the xb textures are
  looked at.

## Tooling

- nxdk, the open-source original Xbox SDK: pbkit, lwIP, SDL2, USB, vertex
  program and register combiner compilers.
  https://github.com/XboxDev/nxdk
- PS2Recomp, a static recompiler for PS2 executables. Experimental; graphics
  need an external implementation and VU1 support is incomplete.
  https://github.com/ran-j/PS2Recomp
- Reference PS2 matching-decompilation projects, for build setup and
  workflow:
  https://github.com/theonlyzac/sly1
- decomp-toolkit, for GameCube/Wii only. Relevant if the Wii route is ever
  reconsidered.
  https://github.com/encounter/decomp-toolkit
- MH3SP, a Monster Hunter Tri server project. Different game and protocol,
  but an example of an open Capcom lobby server reimplementation.
  https://github.com/sepalani/MH3SP

## Metrowerks PS2 compiler availability (checked 4 Oct 2026)

- decomp.me defines 15+ Metrowerks PS2 compilers (ids mwcps2-<version>-
  <yymmdd>), from 2.3 (1999-12-02) through 3.0.1b119 (2004-09-14), run on
  Linux through the wibo Win32 loader. Builds dated around MH1's ELF date
  (2004-01-28): 3.0.1b75-030916, 3.0.1b87-031208, 3.0.1b95-040309.
  Around G's (2004-11-20): 3.0.1b103-040528, 3.0.1b119-040914.
  https://github.com/decompme/decomp.me/blob/main/backend/coreapp/compilers.py
- Binaries are published as release archives (about 1.5-2 MB each):
  https://github.com/decompme/compilers/blob/main/values.yaml
  https://github.com/decompme/compilers/releases/tag/compilers
- Not yet known: which build writes "MW MIPS C Compiler (2.4.1.01)" into
  .comment. That string looks like an internal core version, not the
  product version. Plan: compile a test file with each candidate and
  compare .comment, then confirm by matching a few real MH1 functions.

## Stated from general knowledge, not checked during this research

- Hardware: PS2 is little-endian MIPS with 32 MB of main RAM and transforms
  vertices in VU1 microcode. The Xbox is a 733 MHz Pentium III with 64 MB
  and an NV2A GPU (vertex shaders plus register combiners), and supports
  paletted textures. The Wii is big-endian PowerPC with 88 MB.
- The AFS container layout implemented in tools/afs_extract.py.
- That splat is the usual tool for splitting PS2 executables.

## Not known

- The disc serial and executable name of each build.
- Which compiler built the game, and whether debug info beyond names exists.
- Whether PS2 G's executable has symbols.
- How much of the code is game logic versus Sony SDK.
- How many VU1 programs and framebuffer effects the renderer uses.
- Whether there is video that needs a decoder.
- Whether the network-delivered patches are needed for normal play.
- Anything about the lobby protocol's actual format.
