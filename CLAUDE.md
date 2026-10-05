# Monster Hunter (PS2, 2004) to original Xbox

## What this project is

A hobby preservation project to port the first Monster Hunter from the
PlayStation 2 to the original Xbox, running on real hardware, with Japanese
and English text and working network play.

This is a multi-year reverse-engineering effort, not a recompile. The owner
knows that. Progress comes from small steps that can each be verified, so
prefer finishing and checking one thing over starting five.

The plan was worked out in a claude.ai chat on 3-4 October 2026. That chat had
no access to the game files, so everything about the game's internals in
these docs is either sourced from public research (docs/RESEARCH.md) or marked
as unknown. Treat unknowns as unknown until the files in front of you say
otherwise.

## Where things stand

Phase 1 done (main and the game overlays select, game, yn, lobby rebuild
byte-identical); Phase 2 (decompilation) under way: about 8% of all code
matches as of 5 Oct 2026 (run tools/progress.py). The owner's priority since
5 Oct 2026 is "playable and recognizable first, polished afterwards" (see
docs/DECISIONS.md): matching stays the correctness check, but stubborn
functions are parked as near-matches (*_nm.c) and platform work runs in
parallel. Model, texture and motion formats are understood well enough to
render posed, textured monsters (docs/formats/, tools/clay_dump.py).
PS2Recomp (static recompiler) is being tested as a stop-gap for code not yet
decompiled (docs/STATUS.md).

- Work is split between worker agents in git worktrees (../mh1-wt/A..D,
  branches agent-A..D) and a coordinating session that merges into main.
  Agents follow docs/agents/BRIEF.md and keep notes in docs/agents/.
- The owner's discs live in disc/ (gitignored): mh1 (base), mhg (PS2 G),
  mhg_wii (Wii MH G), mhp (PSP Portable), mhf (Frontier 1.0 installer).
- The owner describes their reverse-engineering experience as "so-so" and
  will rely on Claude for most of the technical work. Keep them in the loop:
  explain findings in plain language and check in before big decisions.

## What to do first

Read docs/STATUS.md (latest entries at the end) and docs/DECISIONS.md. The
original setup steps below were all done on 4 Oct 2026; kept for reference.


1. Ask the owner where their disc files are (Japanese MH1, and PS2 Monster
   Hunter G if they have it). Keep them outside the repo or in `disc/`, which
   is gitignored.
2. Run the survey on each executable and save the output:
   `python3 tools/elf_survey.py <SLPM file> --csv docs/survey/<game>_symbols.csv > docs/survey/<game>.txt`
3. Read the results and update docs/DECISIONS.md: confirm or revise the base
   game (see the open decision there), record code size, compiler, and what
   debug info exists.
4. List the data archive with `tools/afs_extract.py --list` and start a
   format inventory in docs/.
5. Only then set up the splittable, byte-matching disassembly (Phase 1 in
   README.md).

## Decisions and research

- docs/DECISIONS.md: what has been decided, what was rejected, what is open.
- docs/RESEARCH.md: findings with source links, and a list of things that
  were assumed from general knowledge rather than checked.
- README.md: the six-phase roadmap and the known unknowns.

The short version: base the port on a Japanese PS2 build (it has symbols and
is the version the community servers support), use nxdk for the Xbox side,
decompile to portable C with a PC backend before the Xbox backend, and keep
the network wire protocol identical to the PS2 client with DNAS removed.

## Rules for this repo

- Never commit Capcom code or data: no disc images, executables, extracted
  assets, or raw disassembly dumps. Builds must require the user's own disc.
  This keeps the project distributable. It is not legal advice, and the legal
  position of decompilation projects varies by country.
- Do not connect any client build to the MH Oldschool public server without
  their operators' permission. Use a private test server.
- English text from community patches belongs to its translators. Check each
  patch's stated terms and credit them before reusing anything.
- When you learn something about the game, write it down in docs/ along with
  how it was verified (address, file, test). Future sessions start from
  nothing but these files.
- Say plainly when something is untested or a guess. A wrong "verified" in
  the docs costs more than a gap.

## Working conventions

- GitHub: https://github.com/itsbigcforme67/mh1-xbox (public). The owner
  asked for it to be kept up to date: commit and push at the end of each
  meaningful step (a tool works, a function matches, docs updated). Before
  each push, check `git status` for anything that could hold Capcom bytes.

- Python tools: standard library only, Python 3.8+, runnable as scripts.
- Keep a running log in docs/STATUS.md: what was done, what was verified,
  what is next. Update it at the end of each session.
- Once Phase 1 exists, every decompiled function is checked against the
  original bytes before it counts as done.
- Platform code goes behind one thin interface (gfx, audio, pad, fs, net).
  Bring up a PC backend first: it is much faster to debug than a console.
- Xbox testing happens in xemu first, then on hardware by the owner.

## Layout

    CLAUDE.md            this file
    README.md            roadmap and how to run the tools
    docs/DECISIONS.md    decided / rejected / open
    docs/RESEARCH.md     sourced findings
    docs/STATUS.md       session log (create on first session)
    docs/survey/         survey outputs (create on first run)
    config/              splat config, symbol list, c_files.txt
    src/                 decompiled C (byte-matching; *_nm.c = near-matches, not built)
    docs/formats/        file formats (graphics, motion, ...)
    docs/agents/         worker-agent brief and notes
    tools/               elf_survey.py, afs_extract.py, mwo_unpack.py,
                         mips_dis.py, matchtest/, compilers/ (gitignored)
    disc/                user's own game files, gitignored
