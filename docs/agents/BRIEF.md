# Brief for MH1 decomp worker agents

You are one of four worker agents on a matching decompilation of Monster Hunter 1
(PS2, Japanese SLPM_654.95), part of a long-term port to the original Xbox. A
coordinator (the session that started you) merges your work into the main repo
and pushes it to GitHub. The owner is a hobbyist and is offline; work on your own.

## Your workspace
- You have your OWN git worktree (path in your assignment) on branch `agent-X`.
  Run every command from that directory. NEVER cd into, edit, or commit in the
  main checkout `/home/james/claude projects/MH XBOX/mh1-xbox`, and never touch
  another agent's worktree under `.../mh1-wt/`.
- Commit to your branch after each file that matches (or each solid step).
  Do NOT push, do NOT rebase/force anything, do NOT use `git stash`.
- Commit messages end with: `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`
- `disc/`, `.venv`, `tools/compilers`, `tools/binutils`, `tools/m2c`,
  `tools/permuter` are symlinks to shared copies: read-only for you.
- `asm/`, `build/`, `assets/` are gitignored and hold Capcom code. Never commit
  Capcom bytes: no asm dumps, no copied data tables, no disassembly in docs.
  Before each commit run `git status --short` and only add src/, include/,
  config/ and docs/ files.

## Read first
- `CLAUDE.md` (repo rules), then `docs/STATUS.md` from the section
  "Update: workflow tools and first whole source file" to the end. It holds all
  the matching lessons learned so far. Look at a few finished files that resemble
  yours (src/game/shell/shell06b.c, src/game/eft/eft15.c, src/game/set/set14.c,
  src/game/shell/shell22*.c, src/game/eft/eft18*.c) for style and conventions.

## Policy (owner, 5 Oct 2026)
Goal is "playable and recognizable first, exact match polished later". Matching is still
the correctness check, but cap time on a stubborn function at ~10 minutes and at most one
short permuter run, then park it as a near-match in X_nm.c (logic complete and believed
equivalent), note how far off it is, and move on. Cover whole files before perfecting
single functions.

## Lessons from earlier agents (read before starting)
- A function whose switch compiles to a jump table also needs its `MODULE:rodata START END`
  line in config/c_files.txt; check.py cannot see this (the link fails with an undefined
  .Lxxxx). tools/lbf_jt.py finds the range. (agent F)
- tools/check.py ignores relocation addends: a wrong array index into a global, a wrong
  table symbol or a gp-relative global still shows OK. Only `tools/rebuild.sh` (byte compare
  of the linked module) proves a match; run it before registering a file. (agent E)
- Before you report back: `git merge main` once more, `tools/rebuild.sh` (all five OK),
  commit. Never refer to padding by name (`_padXXX`); fields get carved out by others.
- tools/check.py can report OK against the wrong address for a static whose name also
  exists in another file: give statics their address suffix (e.g. `foo_5341A0`). (agent B)
- SHORT STRING LITERALS (<= 8 bytes, MWCC puts them in .sdata with gp-relative access, the original has them in .rodata
  with lui/addiu): put `#pragma readonly_strings on` in the file (after the includes). Strings then go to .rodata and are
  addressed with lui/addiu; check.py shows OK. Give the object a rodata slot in c_files.txt
  (`lobby:rodata START END name`, one slot per object; each literal is 8-aligned). Long strings default to .data
  (c_renames.txt handles that); the pragma moves those to .rodata too, so use it only where the original has rodata. (agent B)
- MWCC unrolls simple counted loops 8x itself (`for (i = 0; i < n; i++) acc += *p++;`): never write an unrolled body. A shared string
  literal used by several original functions: keep it in the asm data and `extern` the literal's symbol. `x >= C` vs `x > C-1` flips
  whether the compare result goes to `at`. m2c sorts switch labels; the compare ladder in the asm is the REVERSE source order of the
  labels. (agent B, details in agent-B.md "Lobby round 2" / "Lobby UI")
docs/agents/agent-A.md, agent-B.md, agent-C.md, agent-D.md hold dozens of MWCC matching
tricks and struct conventions. Monster (em) code: follow agent-C.md (per-monster struct
cast from EMW.ex at EMW+0x444, file-static helpers with address-suffixed names are
`static` in C with the plain name). `python3 tools/align.py FILE FUNC` shows
only the real differences (ignores relocations and branch-address shifts).
Before starting a new file: `git merge main` in your worktree, then `tools/rebuild.sh`.

## Toolchain
- Compiler MWCC mwcps2 3.0b52-030722 -O4,p run via wibo; build.py handles it.
- `python3 tools/draft.py game --file f_eft04` gives an m2c first draft (a start, never a match).
- Function asm: `asm/<module>/text/<file>.s` (no nops shown in some dumps; check.py -v shows full).
- `python3 tools/check.py src/.../file.c [-v]`: per-function compare against the original.
  `-v` shows side-by-side diffs (left = original, right = yours).
- Register a file in `config/c_files.txt`:
  - `game START END path` for a text range (END = end of the last C function, no padding).
  - `game:rodata START END path` for its jump tables (exact end, no padding). Jump tables are
    in `asm/game/data/data/*.data.s`, named lit_NNN_ADDR.
  - Then `tools/rebuild.sh game` (or `main`) MUST print "OK" for that module (byte-identical).
    A game-only rebuild takes a few minutes; don't run it more often than needed.
  - `config/c_renames.txt` and `config/*.yaml` are regenerated by the build: commit them too.
- Only fully matching functions may be linked. Split pattern for a file with near-matches:
  `X.c` (matching run), `Xb.c`, `Xc.c` ... for further matching runs, and `X_nm.c` holding the
  whole file's near-match C (not built, kept for later). File-static functions/data become
  global when a file is split. The unmatched functions stay as asm.
- Helpers:
  - `python3 tools/declbf.py FILE FUNC [skip]` brute-forces declaration order
    (very often fixes register-allocation diffs).
  - `python3 tools/dfilt.py FUNC MINADDR CTX` filters check.py -v output.
  - Permuter: `timeout 1200 python3 tools/perm.py game FUNC FILE -j2 --stop-on-zero`
    (results in build/perm/FUNC/output-SCORE-N/). The machine has 8 cores and 7 GB RAM
    shared by four agents: run AT MOST ONE permuter at a time, with -j2, always under a
    timeout, in the background. Stop one with `tools/killperm.sh FUNC` run alone.
    Don't sink more than ~30 minutes into one stubborn function: leave it as a near-match
    and move on; the coordinator may return to it later.

## Shared headers (merge conflicts)
Other agents edit include/*.h too. Keep edits minimal: name a field by
carving it out of existing padding (`u8 _padXX[...]`), keep offsets in comments, never
rename or retype an existing field unless it is required and you checked every user
(grep src/). Prefer file-local typedefs/externs for types only your files use, or a new
header named after your file (like include/shell06.h). Mention every shared-header edit in
the commit message.

## Shared struct fields (EMW, PLW, GAME_W, SHLW)
Several agents add fields to the same structs, and merges have produced duplicate
names and wrong signedness. Before naming a field, grep include/ and src/ (after
`git merge main`) for its offset: if it already exists, use that name. Only change a
field's type when a load in a function you match proves it (lb vs lbu, lh vs lhu), and
say so in the comment, e.g. `/* 0x8BB (s8: Em_Damage_Stock) */`. Keep the
`/* 0xOFFSET ...` comment on every field: the coordinator merges structs by offset with
tools/merge_struct.py.

## Notes
- Don't append to docs/STATUS.md (everyone would conflict). Put what you learned in
  `docs/agents/agent-X.md` (create it): per file, what matched, how verified
  (check.py + rebuild OK), what's left as near-match and how far off, and any new
  matching lesson (with the function that shows it). Also put a short header comment in
  each C file saying the address range and what the code does. Say plainly what is a guess.
- Do not connect to any network game server.

## When you finish your assignment (or get truly stuck)
Make sure everything good is committed on your branch, then reply with a short report:
files done (fully matching / near-match + how far off), shared headers you edited,
anything the coordinator must know to merge. Then stop.
- One stubborn function no longer blocks a file: list it in config/c_rawfuncs.txt and write
  `asm` + the generated build/raw/NAME.inc in its place (see mc_sel_ck in src/main/mc/mccomb.c;
  the .inc comes from the disc at build time and is never committed). It still counts as
  unmatched; use it only after a real attempt, so the rest of the file can link.
- Wrap every c_rawfuncs `asm` block in `#ifdef __MWERKS__ ... #endif`: gcc (the PC build) can't
  compile it and takes the near-match C instead (keep that in a *_nm.c the PC build links weak).
- HARD RULE: never `pkill -f`/`killall` a pattern (rebuild.sh, build.py, permuter, python, mwcc).
  Note the PID when you start a job (`cmd & echo $!`) and kill only that PID.
- Never `pkill -f` a broad pattern (permuter, python, mwcc): other agents' jobs match too, and the
  pattern can match your own shell. Kill your own jobs by PID.
- The c_rawfuncs fallback is only for one or two holdouts in a file whose OTHER functions are real
  C matches. A file made only of raw functions is not progress (progress.py does not count it):
  don't link functions that way, spend the time on real matches instead.
- Big lesson (agent E, 7 Oct 2026): functions that came from one original source file must be
  compiled as ONE translation unit, in original address order, with their statics defined
  (static) before their callers. MWCC's register allocation and inlining depend on it; it fixed
  dozens of "40-70 off" functions at once. Before polishing a stubborn function, check whether its
  neighbours/static helpers belong in the same file (bind column in docs/survey/mh1_symbols.csv).
- Lesson (agent C, 7 Oct 2026): m2c drafts often access globals through literal addresses
  (`*(u8 *)0x3F33F1`). MWCC schedules stores differently for those than for named objects:
  replace them with the real symbol and field (game_w.step, ConnWork.x, ...). That closed many
  "3-10 off" near-matches at once.

- Before deleting a run file that a whole-file TU replaces, grep tools/build_pc.sh
  and tools/pc_lobby_matched.txt for it. If the PC build lists it, keep the file
  (unregistered) or update the PC list in the same commit, then run build_pc.sh.

- Investigated (agent C, Oct 2026): the `slti v1,x,4 ; bne v1 ; daddiu v1,zero,4 (delay slot) ; b END ; nop ; L: addiu v1,x,1` shape of `v = x > 3 ? 4 : x + 1` (value_result, Gun_level_up, Gun_option_ck, itembox_cursor_mv `daddiu t0,zero,9`). It is NOT a compiler setting: the same tiny test compiled with all ten mwccps2 builds in tools/compilers (3.0b38 .. 3.0.1b95), -O1..-O4 with p/s, -inline on/auto, -lang c++, and the PS2 pragmas peephole/schedule/tailcall/conditional_move/padloop/nomacro/dividefix/fastmath/far_call/optimization_level gives the same output (`slti at; bnez at; nop; b; li`) or something clearly different. mwccps2 has no -m gp64 style command line option (those strings are IDE prefs). `daddiu` in a delay slot IS produced by the compiler, but only for `v = 4; if (c) v = x + 1;` where the then-block stays inline (`slti at; beqz at,END; daddiu v1,zero,4; addiu ...`): it is the scheduler hoisting the pre-assigned constant. A generated search of 2000 source spellings (conds >=,>,<,3<l; if/else, empty-then, pre-assign, ?:; int/u16/u32 temps; several store expressions) never gave the original block order together with `slti v1` (compare in v1 only appears for `x >= C` forms, with the else block inline).
- What did help a little: the final combine of Gun_level_up as `(v & 0xFFFF) | (o & 0x70)` (operand order of the `|`, 9 -> 8 differing instructions). The remaining difference: the original keeps l in a0 (the dead parameter register) and the compare result in v1; mine puts l in v1 and the compare in `at`.
- Tooling: a generated multi-function test file compiled once (`wibo mwccps2.exe -c -O4,p -nostdinc file.c` + objdump) and grep'd for an instruction signature is much faster than 1 s per spelling through check.py; useful to search for "which source makes this exact sequence".

- `addiu -1; pcpyld; bne` (often shown as a `.word` before a -1 compare) is a 64-bit
  compare of an unsigned 64-bit value with -1: declare the field `unsigned long long`
  (u64), not s64. (agent C, round 19; fixed four IME functions)

- Keep it tidy (owner's request): when your fix makes an older workaround,
  duplicate code path, stand-in, build-list entry or test aid obsolete, remove
  the old one in the same change and say so in the commit message.

- When you change a trace line or log format, grep every test script for the old text
  (`grep -rn 'old text' tools/`) and update them in the same commit. Another agent's
  branch may add a new test that reads it too, so say so in your report. (11 Oct:
  F changed the money trace and B's new test_online_event still grepped the old line.)
