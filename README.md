# Monster Hunter (PS2, 2004) to original Xbox: phase 0 kit

This is the starting point for the port, not the port. It holds the two tools
needed to measure the job, and the plan they feed into. Nothing here contains
Capcom code or data: you supply files from your own disc. The repo does carry
symbol names and addresses read from the game's executable
(docs/survey/mh1_symbols.csv, config/symbol_addrs.txt) and archive file
listings, as decompilation projects commonly do.

## Decisions already made

| Question | Decision | Why |
|---|---|---|
| Which build to port | Japanese (NTSC-J) | Its executable ships with a symbol table, and it is the version the community servers speak to. The US/EU builds use different netcode that nobody has revived. |
| English | Second string table plus a language toggle | English text can come from the community MH1j translation (with the authors' permission) or be retranslated. |
| Xbox SDK | nxdk (open source) | Gives a GPU library (pbkit), vertex program and register combiner compilers, an lwIP network stack, SDL2 and USB. Avoids the leaked Microsoft XDK. |
| Method | Decompile to portable C, then retarget | Static recompilation tools for PS2 are too immature and too heavy for a 733 MHz, 64 MB machine. |
| Online | Same wire protocol as the PS2 client, DNAS removed | Lets an Xbox client share lobbies with PS2 and PCSX2 players, if the server operators agree. |

## Step 1: run the survey (needs your disc)

Copy the executable out of the disc root (the SLPM_ file named in SYSTEM.CNF),
then:

    python3 tools/elf_survey.py SLPM_XXX.XX --csv symbols.csv

This answers the questions everything else depends on:

- How many bytes of game code there are, separate from Sony SDK and libc.
- Which compiler built it (decides how matching decompilation is set up).
- Whether richer debug info than names survives (.mdebug can carry types and
  line numbers, which would cut the work dramatically).
- The original source file layout, where the symbol table preserves it.

Send the terminal output and symbols.csv back and the plan below gets real
numbers and a real module map.

## Step 2: unpack the data

    python3 tools/afs_extract.py AFS_DATA.AFS --list
    python3 tools/afs_extract.py AFS_DATA.AFS -o data/

Keep `_manifest.csv`. The game is likely to address files by index, so order
matters. Many entries are themselves containers or compressed, which is the
next format to document.

## Step 3: unpack the code overlays

Most game code is not in the SLPM executable but in compressed Metrowerks
overlays at the start of AFS_DATA.AFS (game.bin, lobby.bin, sub_main.bin, ...):

    python3 tools/mwo_unpack.py AFS_DATA.AFS -o overlays/

Verified on PS2 G only so far.

## Phase 1: build the matching main executable

One-time setup (Linux):

    python3 -m venv .venv && .venv/bin/pip install "splat64[mips]"
    # binutils for the PS2 EE, built from source. The install prefix must
    # not contain spaces (binutils' build breaks on them):
    ../binutils-2.47/configure --target=mips64r5900el-ps2-elf \
        --prefix=$HOME/.local/opt/ps2-binutils-2.47 --disable-nls \
        --disable-werror --disable-gdb --disable-sim --disable-gprofng
    make all-gas all-ld all-binutils && make install-gas install-ld install-binutils
    ln -s $HOME/.local/opt/ps2-binutils-2.47 tools/binutils
    # Compiler + loader from decomp.me's release (see docs/RESEARCH.md):
    # tools/compilers/wibo, tools/compilers/mwcps2-3.0b52-030722/

Each time (needs disc/mh1/SLPM_654.95, disc/mh1/AFS_DATA.AFS and
docs/survey/mh1_symbols.csv):

    python3 tools/setup_split.py
    for m in main select game yn lobby; do
        .venv/bin/python -m splat split config/$m.yaml; done
    python3 tools/build.py
    python3 tools/progress.py

`build.py` prints OK per module when its rebuild equals the original:
`main` (the executable's code and data) and the overlays select, game, yn
and lobby (from AFS_DATA.AFS). The DNAS overlays are not built: they are
Sony network security code that gets replaced.

To decompile a function: write it in src/<module>/<name>.c, check it with
tools/matchtest/cmp.py, add `module start end name` to config/c_files.txt,
then rerun the commands above. The build must still print OK.

## Roadmap

**Phase 0. Survey (this kit).** Symbol census, data inventory, compiler ID.

**Phase 1. Buildable disassembly.** Split the executable into assembly that
reassembles to a byte-identical file (splat or decomp-toolkit, seeded from
symbols.csv). From here every function can be replaced by C one at a time and
verified.

**Phase 2. Decompile game code.** Work outward from leaf modules: math,
memory, file loading, then player, monsters, quests, UI. Leave Sony SDK
functions as stubs: they get replaced, not ported.

**Phase 3. Platform layer.** Define one thin interface the game calls, with a
PC (SDL2) backend first because it is far faster to debug than a console:

- gfx: PS2 draw packets and VU1 transform programs become NV2A vertex
  shaders and register combiner setups. Both consoles are little-endian and
  the Xbox supports paletted textures, so most assets can load unconverted.
- audio: software mixer for the PS2 ADPCM samples and streamed music.
- pad: DualShock 2 to Xbox controller, including right-stick attacks.
- fs: disc reads and memory card saves to DVD and hard drive.
- net: sockets over lwIP in place of the Sony network stack.

**Phase 4. Xbox bring-up.** nxdk backend for the same interface. Test in xemu,
then on hardware. Budget time for memory and frame-time tuning.

**Phase 5. Online.** Capture and document the client side of the lobby
protocol from the decompiled code, strip DNAS, and test against a private
server before asking anyone to let an unofficial client onto a public one.

**Phase 6. Bilingual.** Both string tables on disc, language option in the
menu, English laid out in the Japanese build's text boxes, chat input in both
scripts.

## Known unknowns

- Size of the game code (Step 1 answers this).
- How many distinct VU1 programs and framebuffer effects the renderer uses.
- Whether any intro or cutscene video needs a new decoder.
- Whether the game's network-delivered patches (stored on the memory card on
  PS2) are required for normal play.

## Ground rule

The repository holds tools, original code and patches only. Builds require
the user's own disc image for all data.
