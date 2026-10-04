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
