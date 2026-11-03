# Readability report on src/ and include/ (3 Nov 2026)

A list of suggestions only. **No C, header, tool or config file was changed** for this report. The owner applies these
locally, where the bytes can be checked against the disc.

- **Base:** main at commit 0a67047. Line numbers were read on that commit. Other agents are editing struct headers,
  shared game C, src/pc and tools/ on other branches, so line numbers may have moved. Search for the quoted text if a line
  does not fit.
- **How it was made:** three read-only passes (headers and macros; src/mh2 and src/mhg; src/main, game, lobby, yn, pc),
  each location read by hand. A handful were checked a second time (types.h, version.h, mh2_ofs.h:106, mh2_lb.h:7,
  f_hk.c:1, cnmsg01.c:185-197, p0080D270.c:20, the M2C_FIELD count).
- **Nothing was compiled.** The disc and the PS2 compilers are not in this repo. Every tag below is a judgement.

## Tags

- **rename/comment only:** should not change any emitted byte. A comment, a macro name that expands to the same text, a
  field or local rename. Still run the normal checks: renaming a *linked symbol* also needs the same change in
  `config/*/symbols`, `*_aliases.txt` or `config/mh2/func_fix.txt`, or the link fails.
- **changes code shape:** may change what the compiler emits (types, statements, declaration order, build lists, deleting
  a file). Needs a full byte check (`tools/rebuild.sh`, `VERSION=mhg tools/rebuild.sh`, `tools/mh2_rebuild.sh` +
  `tools/mh2_check.py`, and the PC builds).

## Four things to know before applying any of this

1. **Some headers are generated. Edit the generator, not the header.** A hand edit is lost on the next run.
   - `include/mh2_ofs.h`, `mh2_plw.h`, `mh2_emw.h`, `mh2_lay_ofs.h`, `mh2_layouts.h`, `cnet_mh2.h` come from
     `tools/mh2_structs.py` (`mh2_layouts.h` also from `config/mh2/layouts.txt`).
   - `include/lobby_a.h` comes from `include/lobby_f.h` through `tools/lbauto.py`.
   - `src/mh2/parts/*.c` are written by `tools/mh2_carry.py register`; `src/mhg/**/*_pN.c` by `tools/mhg_parts.py`.
2. **Rerun `python3 tools/mh2_carry.py register` after editing any carried C or header**, even a comment-only edit
   (docs/mh2.md 5.1). Section numbers come from the compiler, and keep_sections relies on them.
3. **"Not built" in a `*_nm.c` header means "not built for the PS2".** `tools/build_pc.sh` links many `*_nm.c` files
   (some as weak symbols). Grep build_pc.sh before deleting any of them.
4. **Comments in a shared C file are safe for the bytes**, but a shared file is compiled for MH1, G, MH2 and the PC. A
   comment that is only true for one game should say which.

## How MH2V and MH2_OFS work today (plain version)

One set of C files builds three games: MH1 (no flag), G (`-DVERSION_MHG`) and MH2 (`-DVERSION_MH2`). The C is written
with MH1's numbers, and macros swap in the other game's numbers at compile time.

- **`MH2V(mh1, mh2)`** (include/types.h:13 and :20) is a plain either/or. The MH2 build gets the second number; MH1 *and
  G* get the first.
- **`MH2_OFS(p, o)`** (include/mh2_ofs.h:106) is for code that reaches into a struct by a raw byte offset `o` instead of a
  field name. MH2 moved most fields of the player work (PLW) and monster work (EMW).
  - It looks at how big the thing `p` points at is. In the MH2 build PLW is 0x970 bytes and EMW is 0x9D0 bytes.
  - Size 0x970 picks the generated player map `MH2_OFS_PLW(o)`, size 0x9D0 picks the monster map `MH2_OFS_EMW(o)`, any
    other size leaves `o` alone.
  - Each map is a chain of "if o is below X, add or subtract Y". A one-byte range (e.g. mh2_ofs.h:11,
    `< 0x34D ? (o) - 0x34A`) is a single field that moved on its own.
  - Outside MH2, `MH2_OFS(p, o)` is just `o` (types.h:19).
- **`QWO`, `UDMO`, `NCO`, `LBM`, `PMO`** are the same idea for one global each (quest_w, User_data, net_common_w,
  lb_sys, PitMenu), so they take no pointer. **`CNO`** (CnetSys_w), **`GWX`/`MH2_GW`** (game_w), **`SKO`** (soft
  keyboard), **`YNO`** (yn.h:14) and **`IBO`** (lbsys_g.h:129) are more of the same in other headers.
- **Rule (docs/mh2.md 5.3):** inside any of these, write the MH1 offset once. `CNO(MH2V(a, b))` moves it twice.
- **Guesses, not tested:** the offsets are always constants, so the compiler should fold each chain to one number and
  the PS2 output never sees the chain. If `p` is a byte pointer, or any struct that is not exactly 0x970/0x9D0 bytes,
  the offset is silently not translated.

Use counts in src (grep): MH2V 258 (87 files), MH2_OFS 375 (278 files), MH2_EM 152 (7 files), MH2_OFS_PLW 2,
MH2_OFS_EMW 0, QWO 40, UDMO 138, NCO 705, LBM 191, PMO 52, PLW_AT 7.

## Top 10 (most help to a reader first)

| # | What | Where | Tag |
|---|---|---|---|
| 1 | Fix comments that are wrong or contradict the code (built files that say "not built", a wrong timer register, a header that names the wrong source, a misplaced comment) | A1-A6 | rename/comment only |
| 2 | One comment block that explains MH2V, MH2_OFS and all 13 offset maps | B1 | rename/comment only |
| 3 | A table of which build defines which flag (and a possible MH2 `GUN_ROW` mismatch) | B2, B3 | comment only; the value fix changes code shape |
| 4 | Name the magic sizes 0x970 / 0x9D0, and warn about the `PLM` trap | B4, B5 | rename/comment only |
| 5 | Say that MH2 layout field names are MH1 offsets, with an example | B6 | rename/comment only |
| 6 | One shared set of offset-accessor macros instead of `M2C_FIELD` in three meanings and ~278 local ones | B7, B8 | rename only |
| 7 | MH2 hand files: map their raw offsets to EMW field names; `APPROACH` hides `break` | C1-C3 | rename/comment only |
| 8 | Names with clear evidence: `em_cmd_err_set`, a wrong `BsSoftKbdTask`, EMW `hp`/`hp_max`, camera zoom fields | C4-C8 | rename only |
| 9 | Remove dead copies: `pl_snd_nm.c`, `hk_nm.c`, `lb_q/x/y.c`, ~45 superseded `*_nm.c`, the yn preambles | E1-E5 | changes code shape (deletes) |
| 10 | src/pc: same macro names with different meanings (`PF`, `PU8`), `RT_PLO` twice, six stand-in stub styles | F1-F3 | rename/comment only |

## A. Comments that are wrong or misleading

A wrong comment costs a reader more than a missing one, so these come first.

**A1. Built files whose first line says "Near-match C, not built".** rename/comment only.
- `src/main/hk/f_hk.c:1` ("hk_nm"), `src/main/chat/f_chat.c:1-3` ("chat_nm"), `src/main/ud/f_ud.c:1` ("udmisc_nm"). All
  three are linked: config/c_files.txt lines 1914, 1858, 1860.
- `src/lobby/b/lb_by168.c:1-2`: line 1 says it is kept for the PC build, line 2 (copied) says "Not built".
- `src/main/fl/flio01.c:1` is an empty `/*  */` above the real header, which still says "flio_nm".
- f_ud.c's header gives 0x271FB0-0x272400; the file now covers 0x271FB0-0x274E10.
- Fix: rewrite line 1 in the usual style ("name - range: what it holds").

**A2. Header ranges that are main.bin offsets, not addresses.** rename/comment only.
- `src/main/hk/f_hk.c:1`, `src/main/hk/hk_nm.c:1`, `src/main/tu/hk_all.c:5` say `0x00164180-0x00167198`;
  config/symbols/main.txt:6419 has `HardKeyboard_move = 0x00264180`.
- `src/main/chat/f_chat.c:1` says `0x001755D0`; the symbol file has `disp_cursorC = 0x002755D0`.
- `src/main/sk/sk_nm.c:1` says `0x0015FA90`; the symbol file has `SoftKeyboard_init = 0x0025FA90`.
- These are offsets inside main.bin (main loads at 0x100000). Other headers give addresses. Either convert them or write
  "main.bin offset" next to them.

**A3. Wrong hardware register: `src/main/net/cnmsg01.c:185-197`.** rename/comment only.
- The comment says "EE timer 0", but `0x10000800` / `0x10000810` are timer 1's COUNT / MODE (timer 0 is at 0x10000000).
- `0x83` means "count enable, H-blank clock"; `/16.0/1000.0*1.0256` converts H-blanks to seconds.
- Suggest `#define EE_T1_COUNT ((volatile s32 *)0x10000800)` and `EE_T1_MODE` (a macro with the same text is
  byte-neutral) and a one-line note on the scaling.
- Related: `src/lobby/f/lb_dr2.c:884-888` reads `0x10000000` (timer 0 COUNT) and waits for 0x41 ticks. Add a comment.
  lb_dr2.c is in the PC build (tools/build_pc.sh:181): that read would fault if BsParseCheck ever ran on the PC (untested).

**A4. `game_w.x28` is described two ways.** rename/comment only (once settled).
- `include/game.h:146` says "per player quest entry id".
- `src/pc/rt/rt_quest.c:41-50` treats `game_w.x28[slot]` as the monster kind in model slot `slot`;
  `src/main/quest/f_quest_nm.c:265,274` stores 0x12 / 0xA into it, which looks like a kind.
- `x2E`: game.h:148 describes it through em20, but `rt_quest.c:71` and src/main/quest/qstb05.c:45 pass it as a level.
- Settle which is right, then rename (e.g. `em_kind[4]`). docs/mh2/hunt.md also uses `game_w.x2E` as an index into the
  em_cmd tables.

**A5. Comment in the wrong place: `include/version.h:14`.** rename/comment only.
- `/* sizeof EMW (monster work): G is 8 + 8 bytes smaller */` sits at the end of the `IPREP_NUM` line. It belongs to
  `EMW_STRIDE` at lines 8-12.

**A6. Header comments that point at the wrong source.** rename/comment only.
- `include/lobby_a.h:4` says "generated from lobby.h"; tools/lbauto.py:81-89 generates it from **lobby_f.h**. Also say
  it is overwritten (do not edit) and why it uses the old K&R form (it changes how arguments are passed: a matching
  trick).
- `include/g_alias.h:2` says the file is "empty for ... the G PC build", but the G PC online build gets G values from
  `GV` (:31-34), and a PS2 MH2 file built in G form gets `MHG_PS2`.
- `include/yn.h:12` says the shifted zone runs "to ... 0x1180", but `YNO` stops at 0x1100 (:14). One of them is wrong.
- tools/mh2_structs.py:19 and docs/mh2/structs.md:10 say `MH2_OFS`/`QWO` live in types.h; they are in mh2_ofs.h and
  mh2_lay_ofs.h (types.h only includes them).
- `src/mhg/main/cam/camd_g.c`: the header says "stage 0x4D", the code uses `77` (the same number in decimal; pick one).
- `src/mh2/game/p0075FBF0.c:281,288,295`: `% 100 < 0x4C`, `0x33`, `0x3D` are 76%, 51%, 61%. Decimal reads better.

## B. Macros and headers (include/)

**B1. One overview comment for all the version macros: `include/types.h:10`.** rename/comment only.
- Today one dense line explains everything. Replace it with a short block: what MH2V does; what MH2_OFS does and how it
  picks (0x970 = PLW, 0x9D0 = EMW in MH2); that the `#else` branch makes all of them plain values for MH1, G and the PC.
- List every offset map and what it covers: `MH2_OFS`/`MH2_EM` (PLW/EMW), `QWO` quest_w, `UDMO` User_data, `NCO`
  net_common_w, `LBM` lb_sys, `PMO` PitMenu, `CNO` CnetSys_w (lbnet.h:371, cnet_mh2.h:105), `GWX`/`MH2_GW` game_w
  (gwx.h:6-9), `SKO` (sk_g.h), `YNO` (yn.h:14), `IBO` (lbsys_g.h:129). There are 13 offset-map macros in 8 headers
  and no list of them anywhere.
- Repeat the "write the MH1 offset once" rule there.
- `types.h:13`: say that the first value of `MH2V` is what MH1 **and G** use; nested forms like `MH2V(GV(2400, 80), 0x50)`
  (src/lobby/b/lb_bz73.c:9) are otherwise hard to read.
- `types.h:16`: `qno_t` "lhu in MH2" → "MH2 reads it as unsigned 16-bit".

**B2. Which build defines which flag: add a table to `include/version.h:3-4`.** rename/comment only.
Today it only mentions MH1 and G.

| Build | Flags |
|---|---|
| PS2 MH1 | none |
| PS2 G | `VERSION_MHG`; with `__MWERKS__` also `MHG_PS2` (g_alias.h:7) |
| PS2 MH2 | `VERSION_MH2` only (tools/build.py:53). Some files are retried with `VERSION_MHG` too, through a wrapper (tools/mh2_carry.py:537) |
| PC MH2 | `VERSION_MH2` + `VERSION_MHG` (tools/build_pc.sh:15, :454-456) |
| PC G online | also `MHG_ONLINE` → `MHG_NET` (lbnet.h:6-7) → `LBNET_G` |

**B3. MH2 values in version.h are layered on with `#undef` after the G/MH1 blocks: `include/version.h:22-25, 41-49`.**
Comment: rename/comment only. Fixing values: **changes code shape**.
- The PS2 MH2 build has no `VERSION_MHG`, so it gets MH1's `GUN_ROW` (0x14), `STG_POS_N`, `MAHI_MAX`, `IPREP_NUM` and
  `EM_SLEEP_END` mapping, while the PC MH2 build gets G's values.
- **Possible real mismatch, not checked:** include/plequip.h:10 makes MH2's `EQ_GUN` 0x18 bytes (docs/mh2.md: "EQ_GUN
  0x18"), but PS2-MH2 gets `GUN_ROW` 0x14 (used at src/main/pl/pl73.c:8, pl77.c:5, pl_nm.c:13). It may simply be that no
  MH2-carried file uses `GUN_ROW` yet.
- Suggest a comment now; later one `#if VERSION_MH2 / #elif VERSION_MHG / #else` chain once MH2's values are confirmed.

**B4. Name the magic sizes in `MH2_OFS`: `include/mh2_ofs.h:106` (generator: tools/mh2_structs.py:304).**
rename/comment only.
- Comment: 0x970 is `sizeof(PLW)` in MH2 and 0x9D0 is `sizeof(EMW)` (and `EMW_STRIDE`, version.h:24). Evidence: `SIZES`
  in tools/mh2_structs.py:40 and the `_Static_assert` at src/pc/rt/rt_game.c:69.
- Warn that a pointer to anything else gets no translation, and that any other struct of exactly 0x970 or 0x9D0 bytes
  would be translated by mistake.
- Have the generator also emit `#define MH2_PLW_SIZE 0x970` and `#define MH2_EMW_SIZE 0x9D0`, and use them where 0x970
  is typed by hand: version.h:46 (`PLW_AT`), src/pc/rt/rt_player.c:48, src/pc/rt/rt_pl.c:30, src/lobby/f/lb_n10.c:12,
  src/lobby/f/lb_f01.c:60, src/main/aq/aqrun01.c:242, src/main/weapon/weapon3.c:94; and `EMW_SIZE 0x9D0` in
  src/main/emw/emw01.c:16, emw02.c:24. Also
  src/mh2/sub_main/p003A2E30_nm.c:51,91,243,253. Same constant after preprocessing.

**B5. A trap to document: `PLM` in `include/mh2_lb.h:7`.** rename/comment only.
- `PLM` is 0x970 bytes (the same size as MH2's PLW) but its fields already use MH2 offsets. `MH2_OFS(plm, o)` would
  translate twice. No such use exists today. Add one line at the struct.

**B6. MH2 layout field names are MH1 offsets, not MH2 ones: `include/mh2_emw.h:3` (in the generator).**
rename/comment only.
- The header says "Field names are MH1's" with no example, and it misleads in practice: `x7EE` sits at 0x73A
  (mh2_emw.h:193) while a different field named `x73A` sits at 0x684; `x8D4` sits at 0x828 (:296) while `x828` is at
  0x77A (:214).
- Put that example in the generated comment, and the same note in mh2_plw.h, mh2_layouts.h:2 and cnet_mh2.h:3.

**B7. `M2C_FIELD` means three different things.** rename only (if the expansions stay identical).
- include/main.h:9 and include/yn.h:10 define it without translation. 170 src files each redefine it locally with
  `MH2_OFS` (e.g. src/lobby/f/lb_village_nm.c:19). One file has a third form with `s8 *`.
- So the same name translates in some files and not in others. Suggest one shared header, with the translating form
  under its own name (e.g. `FIELD_MH2`).

**B8. About 278 files define their own tiny accessor on top of `MH2_OFS`.** rename only.
- `EMF` x40, `FX` x19, `PLB`, `PLH`, `EW`, `EB`, `EF`, `EH`, ... A shared set (`OFS_U8/S16/F32(p, o)`) in one header
  would let a reader learn it once. Byte-neutral as long as each expansion is the same text.

**B9. `MH2_EM` has no player twin: `include/types.h:14`.** rename/comment only.
- An alias of `MH2_OFS_EMW` (152 uses, all in 7 emw files; 0 direct uses of `MH2_OFS_EMW`). The two rt files call
  `MH2_OFS_PLW` directly.
- Comment it as "MH2 offset of an EMW field where there is no pointer, e.g. array sizes in local partial EMW structs
  (emw01.c:35)", or rename it `MH2_OFS_EM` and add `MH2_OFS_PL`.

**B10. Hand-written copies of the generated map: `src/lobby/f/lb_j02.c:34, 46`.** rename/comment only if equal;
recheck matching anyway.
- `MH2V(0x90B, 0x863)` and `MH2V(0x368, 0x370)` equal `MH2_OFS_PLW(0x90B)` and `MH2_OFS_PLW(0x368)`. Use
  `MH2_OFS(pl, ...)` (or a comment) so the copies cannot drift when the map is regenerated.

**B11. `include/mh2_lay_ofs.h:1-13`.** rename/comment only.
- `QWO` and `NCO` have no comment saying which global they map (only `UDMO` has, at :4); `LBM` and `PMO` (:11, :13)
  neither.
- `QWO`/`NCO` repeat ranges with the same shift dozens of times (`< 0x1 ? +0 : < 0x2 ? +0 ...`). Merging runs in the
  generator would make them readable. Every `QWO`/`NCO` argument in src is a constant; only `UDMO` has a non-constant
  use (src/pc/rt/rt_dbg.c:473, PC only).
- `NCO` (net_common_w) and `CNO` (CnetSys_w) differ only by letter order. Spell both out in B1's list. A rename (e.g.
  `NCW_OFS` / `CNET_OFS`) is byte-neutral but touches 705 + 16 sites.

**B12. `HEAD(n)` hides two function signatures: `include/em_core_g_mh2.h:12`.** rename only.
- grep and tags cannot find the definitions at :19 and :342. Write `void cmd_target_kind_set(EMW *em, f32 *pos) {` (and
  the other) out in full and drop the `#undef HEAD` at :645. The preprocessor output is the same.
- Same pattern in src/mh2/game/em_dmg_sys_nm.c:5,10,45 (`HEAD` defined identically in both `#ifdef` branches).

**B13. Unexplained renaming trick: `include/em_core_g_mh2.h:5-11`.** rename/comment only.
- When built alone, the prototypes from em_core_g.h are renamed to `*_` before the definitions. No commit message
  (be27c7b) or doc says why. Ask whoever matched it, then add one line.
- `em_core_g_mh2.h:1-2`: say plainly that src/game/em/em_core_g.c from line 9 on is the MH1 twin of this code (about
  57 of 440 lines differ), so a fix in one should be checked in the other.
- src/mh2/game/cmd_target.c is a 2-line file whose code lives in this 645-line header. A comment in cmd_target.c saying
  so would help.

**B14. EMW target fields with evidence for names: `include/em.h:274-276` (x827-x829), `:321-323` (x881-x883),
`:179` (x617), `:141` (x3B0).** Comments: rename/comment only. Renames: rename only, but ~1,000 sites across worktrees.
- em_core_g_mh2.h:17-18: x827 is the target kind, x828/x829 its arguments; :30-32 copies them into x881-x883. Note that
  x828/x829 are u16 in MH2 (mh2_emw.h:214) but u8 in MH1.
- em_core_g_mh2.h:45-46 does `em->x617 = ...; em->x3B0 = &player_work[em->x617]`; em.h:141 says "target player";
  em.h:360 says x8D4 is indexed by x617. Suggest `tgt_pl_no` and `tgt_pl`, or at least the comment "target player
  index, -1 none".
- Start with comments; rename later when no agent has the header open.

**B15. Request lists with two-letter names: `include/mh2_layouts.h:120-121, 135-138` (`ia`, `ib`, `qa`, `ka`, `qb`,
`kb`).** rename only.
- include/netcw.h:37-54 says request list A/B count, ids and kinds; src/main/net/f_disp.c:85-94 switches on `ka[i]` and
  shows `qa[i]` as a message id. Suggest `req_a_num`, `req_a_id`, `req_a_kind` and the same for B (88 uses). Edit
  netcw.h and config/mh2/layouts.txt.

**B16. `LB_SHOP` fields: `include/lobby.h:78-123`.** rename only (about 370 uses; rename all copies together).
- The comments already name them: `f20` init, `f24` tag decide, `f2C` select, `f30` item select, `f34` decide, `f3C` put
  item detail, `f40` put help, `f44` list icon; `x6C` page, `x6D` page count; `x15` sub step (lobby.h:81).

**B17. Identical structs in two headers.** rename only (moving a struct to a shared header is byte-neutral).
- `LB_SHOP` at include/lobby.h:78-123 and include/lobby_s.h:24-69 (diff empty); also lbshop_g.h:7. `LB_PIT` copied
  (lobby.h:9, lobby_s.h:11; only comments differ). Move each into one small header both include.

**B18. Duplicate prototypes in `include/lobby_f.h`.** rename only.
- `Gold_add` at :113 and :238, `NPCZoomInCameraCancel` at :116 and :249 (copied into lobby_a.h). The file says "Add
  only" (:1), so coordinate with the agents before removing.

**B19. Conflicting declarations.** Comment: rename/comment only. Unifying: **changes code shape**.
- `player_work` is `PLW player_work[]` in pl.h:416 and plf.h:23 but `u8 player_work[]` in lbui.h:144 and
  netsyn.h:395. That is why `PLW_AT` (version.h:46) casts to `u8 *`. Add a comment at `PLW_AT`.
- `s64`/`u64` are `long` in main.h:7-8 and yn.h:8-9 but `long long` in lbnet.h:365-366. A file including both will not
  compile, and on the 32-bit PC `long` is 32 bits. In built files 64-bit types are typedef'd locally five different
  ways, e.g. `typedef long u64;` (signed) in src/main/fl/flrs01.c:6, flrs02.c:5, flrs05.c:6. Comment: "EE `long` is
  64-bit; the signedness was picked for matching".

**B20. Unused macros (zero uses in src, include, tools, wip).** rename/comment only.
- `BW_FIRSTOPEN` (mh2_lb.h:30) can go. `PLS8` (lobby_f.h:75, lobby_a.h:76), `BSC8` (lobby_f.h:328, lobby_a.h:329) and
  `FLDF` (flow.h:15) belong to families of related macros; keeping them is reasonable.
- The `BSC*` family (lobby_f.h:320-329) needs one line: "element i of an array of N-byte records, offset d; written
  this way so the compiler emits the original address math". Only `BSCELL` has a comment.

**B21. `G_PART` is used in 874 files but only explained in the tools/mhg_parts.py docstring.** rename/comment only.
- Add three lines to include/g_alias.h (most of those files include it): it is defined only by the src/mhg/*_pN.c
  wrappers; the normal build compiles everything; a function's `G_PART == N` is its contiguous G run.
- src/mhg/main/item/item01_a.c and item03_a.c use `#define G_PART 1` although their source has no `G_PART == 1` guard; it
  works only because any defined value drops the `!defined(G_PART)` blocks. Say so in the wrapper.

**B22. `QW_X64_T` / `QW_X94_T`: `include/mh2_layouts.h:6-11`.** rename/comment only.
- Say that they let a file retype two QUEST_W pointers before including (reward.h:37-38, eft23.c:64). `QW_X94_T` has no
  default override.

## C. MH2 and G hand-written C (src/mh2, src/mhg)

Of the 1,286 .c files in these two trees, 1,215 are 2-3 line wrappers that include a shared file (574 in
src/mh2/parts, 659 `*_pN.c` in src/mhg). The hand-written C is about 15 files under src/mh2/{game,sub_main,plsel} and
9 under src/mhg/main. Most of this section is about those.

**C1. Raw EMW offsets where the field names exist: `src/mh2/game/p0075FBF0.c:13-18`, `src/mh2/game/p0080D270.c:11-13`.**
Mapping comment: rename/comment only. Switching to `EMW *`: **changes code shape** (docs/mh2/quest.md warns a cast can
change register use).
- The macros `B()`, `SB()`, `U16()`, `I32()`, `F32()`, `WDIST` reach offsets that include/mh2_emw.h already names:
  0x817 = `x8C3`, 0x997 = `x881`, 0x998 = `x882`, 0x59B = `x617`, 0xAC = `pos`, 0x888 = `tgt_pos`, 0x3A2 = `x39A`,
  8 = `work08`, 0x7DC = `x888`, 0x2C2 = `x302`, 0x6DE = `x792`, 0x884 = `act_spd`.
- Next to MH1's em03_act_set (src/game/em/em03.c:1313), `B(0x997) == 1 && U16(0x998) == 0` is the test
  `em->x881 == 1 && em->x882 == 0`, but nothing tells the reader. Add a mapping block at the top of each file.

**C2. `APPROACH` hides `break` statements: `src/mh2/game/p0075FBF0.c:37-95`, used at :222 and :261.**
rename/comment only.
- The macro holds six `break;` and ends in one. At `common: APPROACH(v, 500)` followed by `case 0xC:`, the code looks
  like a fall-through, but is not. Rename it `APPROACH_AND_BREAK`, or add `/* ends in break */` at both uses.

**C3. Same raw-offset pattern: `src/mh2/game/em01_senkai_sub.c:30-37` and the signature at :48.** rename/comment only.
- `ANG(i)` = `ang[i]` (0xA0), `ADJ` = `adj_z` (0x3C0), `CHAR0` = `char0` (0x29C), `U16(0x680)` = `stg`,
  `I32(0x154)` = `x194`, `F32(0x574)` = `x5AC`. MH1's version (src/game/em/em01.c:549) uses these names.
- One-line comments for the magic numbers: :190, :221, :252 `p->c_rise + 1000 == CHAR0` (char0 holds the motion number
  plus 1000; MH1 compares it with 0x3F4 = 1000 + 12, em01.c:672); :168 `6.2831855f * a / 65536.0f` converts angle units
  to radians; :151 `TURN += 0x10000 - TURN` is "set to 0x10000, wrapped to 0 by the `(u16)` at :159".

**C4. `game_func_007262D0` is the command-error setter.** rename only (linked symbol: also config/mh2/func_fix.txt).
- `src/mh2/game/em_cmd_ck.c:52,71`, `src/mh2/game/next_cmd_search.c:7-9,796`.
- Evidence: docs/mh2/quest.md says it stores v in EMW byte 0x95E; mh2_emw.h:342 puts `x9DA` at 0x95E; MH1's loop
  (src/game/em/em_cmd_nm.c:91-92) writes `em->x9DA = 2` inline where MH2 calls this function with 2. Suggest
  `em_cmd_err_set`.
- In next_cmd_search.c:8, `((u8 *)em)[0x95E] = v` could become `em->x9DA = v`: **changes code shape** (probably the
  same store).

**C5. `BsSoftKbdTask` in a monster function is a wrong map name: `src/mh2/game/p0080D270.c:20, 114-116, 278-280`.**
rename only (config/mh2/symbols/game.txt:3430 and the C).
- BsSoftKbdTask is the lobby browser's soft-keyboard task (src/lobby/f/lb_al07.c:1). Here it is called as
  `(em, 0/1/2)` for monster kind 0x34. docs/survey/mh2_map.csv:3430 shows the name at game 0x80DE70 came from a 0.90
  n-gram guess against a lobby function. Rename to `game_func_0080DE70`.

**C6. EMW `x302` and `x792` are current and maximum HP: `include/em.h:121, 215`.** rename only (about 400 sites).
- em.h still marks this a guess. src/mh2/game/em_dmg_sys_nm.c:61 `x302 <= 0 && !dm_flag` returns 2 (dead); :66-71
  `x302 = heal + (x302 - stock)`, clamped to `x792`. p0080D270.c:141 compares `x302` with `x792 * rate`. Suggest `hp`
  and `hp_max`.

**C7. `CAMZOOM` fields `x21`, `x24`, `x28`, `x2C`: `include/cam.h:153-161`.** rename only.
- The header comments already say mode, height, distance, field of view; src/mhg/main/cam/camd_g.c:108,124-150,159
  confirms (`cs->fov = z->x2C * t + ...`). Suggest `mode`, `height`, `dist`, `fov`.

**C8. G helpers whose purpose is already in their comments.** rename only (each touches about 10 alias files, so a
`#define` alias, as in src/main/font/fs2_02.c:4, may be cheaper).

| Location | Now | Suggested | Evidence |
|---|---|---|---|
| src/mhg/main/cam/camd_g.c:16 | `func_001DC0D0` | `NPCZoomInCameraRequest` | its comment; config/mhg/g_names.txt:43. Check the PC G build does not also compile src/main/cam/camd.c:93, which defines that name |
| src/mhg/main/cp/cp01_g.c:7 | `func_00111040` | `flvecSetLength` | normalize, then scale |
| src/mhg/main/fl/flpad02_g.c:6 | `func_00151CB0` | `flGetNumValidPads` | returns `NumOfValidPads` |
| src/mhg/main/pl/pl_guard_g.c:4 | `sub_main_func_004F0900` | `pl_kind_can_guard` | header comment |
| src/mhg/main/item/item_g.c:11, src/main/item/item01.c:192 | `func_0010DD20` | `Pl_recipe_ok_ck` | header comment: the master hunter may use this recipe |
| src/mhg/main/fl/flvu05_g.c:4 | `func_0013E750` | `flvecApplyMat44` | header comment |
| src/main/ud/f_ud.c:110,231,254 | `D_0035178C` | `hr_point_floor_tbl` | points are clamped up to `[rank]` |

Also camd_g.c: `func_001DC160` (:34) sets up the fixed camera (mode 2) for stage 77; `func_001DC1F0` (:54) is a zoom
state check (-1/1/0). Comment both. `0.5235988f` (:21) is pi/6, `1.0471976f` (:48) pi/3; `Psw[4]` (:122-139) is
`PSWE.an`, the stick directions held (include/ioread.h:24-31). `extern volatile u16 Psw[]` (:13) differs from the
ioread.h declaration: if the `volatile` is a matching trick, say so.

**C9. One G function, three names: `func_0012BDD0`.** rename only.
- src/main/font/fs2_02.c:4 `#define font_print_sp func_0012BDD0`; src/main/chat/f_chat.c:50-51 `font_print_sp_G`;
  src/main/mc/mccomb.c:155-157 `FONT_PRINT1/2`, but still `func_0012BDD0(s)` directly at :550. One alias in
  include/g_alias.h covers all three.

**C10. Placeholder names for MH2's new command opcodes are mixed: `src/mh2/game/em_cmd_ck.c:25-50`.** rename only
(through config/mh2/func_fix.txt).
- 0x1F, 0x37, 0x73, 0x76 use `em_cmd_opNN` (func_fix.txt:3,34-37). 0x69, 0x71, 0x72, 0x74, 0x75, 0x77-0x7A, 0x7C, 0x99
  stay `game_func_...`, and 0x36 is `em_cmd_near_pos_ck_mh2_00729CE0`. Use `em_cmd_opNN` for all.
- `game_func_005885E0` (em_cmd_ck.c:28, 224-229) carries G's address, not MH2's (it lives at MH2 0x72B240). Comment at
  cases 0x2C and 0x70: G moved em_cmd_myemtype_sel from 0x2C to 0x70 (docs/mh2/code.md).

**C11. Comments for em_cmd_ck.** rename/comment only unless marked.
- :62, :70-72: `0x3E8` is a 1000-step limit against a runaway program. MH2 also skips mode 6; MH1 skipped only mode 5
  (src/game/em/em_cmd_nm.c:83).
- :116, :194, :227, :293: the cases are out of numeric order on purpose (new opcodes sit beside the MH1 handler they
  vary, func_fix.txt:34-37). Say the order is the original's.
- :419-423: 0x84 was a no-op in MH1 (em_cmd_nm.c:429-431); MH2 calls `game_func_007275D0` for both 0x84 and 0x7B.
- :93 `*(u8 *)&em->cmd_idx = 0`: `cmd_idx` is already u8 in mh2_emw.h:208. At least comment that the cast is left over
  from MH1's u16. Removing the cast: **changes code shape** (trivially).

**C12. Unmarked deliberate fall-throughs.** rename/comment only.
- src/mh2/game/next_cmd_search.c:107-118 (case 0x15: sub-case 0 falls into 1), and the same at 0x1C, 0x79, 0x7A.
- src/mh2/game/em01_senkai_sub.c:74-77 (case 0 falls into 1 when `ADJ > 130`).
- src/mhg/main/cam/camd_g.c:58-63.
- Add `/* fall through */` at each.

**C13. Address-named macros for quest_w: `src/mh2/sub_main/p003A2E30_nm.c:12-29`.** rename only.
- `D_005D8260` expands to `quest_w + 0x0`. docs/mh2/quest.md says `D_5D8260` is the quest state (0 running, 1/2
  ending, 3 result) and `D_5D827E` the script index: `QW_STATE`, `QW_SCRIPT_IDX`.
- src/mh2/sub_main/p003A9230_nm.c:12-16 declares the same globals as plain `extern s8`: two representations of one
  thing in sibling files.
- p003A9230_nm.c:61 `static sub_main_func_003A8990` is the same-file static trick that next_cmd_search.c:5-6 explains.
  Copy that comment.
- p003A9230_nm.c:7 defines `QCMD {op,a,b,c}`, which shadows include/quest.h:35's `QCMD {cmd,a,b,c}`. Comment, or
  rename the local one.

**C14. MH2 hand files repeat the type block instead of including types.h.** rename/comment only (check macro names
first).
- The same `typedef unsigned char u8; ...` block is in 14 files: all 8 in src/mh2/plsel/, p0075FBF0.c, p0080D270.c,
  em01_senkai_sub.c, p00378A30.c, p003A2E30_nm.c, p003A9230_nm.c. include/types.h:3-9 is identical.
- Caution: types.h pulls in mh2_ofs.h under VERSION_MH2. Check that its macros do not clash with local `B`, `H`, `U16`.

**C15. `src/mh2/sub_main/p00378A30.c:30-54` calls game functions as `func_6F4900` etc.** rename/comment only.
- config/mh2/symbols/game.txt already names several (`set09_set`, `Set05_set`, `Set04_set`, `Set20_set`, `Eft14_set2`,
  `Set22_set`, `set00_set`, `Set13_set`). The short form may be needed because sub_main cannot see game's names; where
  those names resolve was not found. If so, a header comment saying so is enough.
- p00378A30.c:12-23 `STGW2` copies G's `STGW` (include/flow.h:61-91) and only adds `xF9`, `xFD`: **changes code shape**
  to merge.
- `D_006AE018` is `u8 *` in four plsel files but `PLSEL_W *` in p00D6FE90.c:6-12. A shared struct (size 0x3B88,
  p00D6F8B0.c:10; fields at 0x39FC and 0x3984) would make them agree: **changes code shape**.

**C16. Wrapper file names in src/mh2/parts.** rename only (it is the generator, tools/mh2_carry.py, that changes).
- `_g` means two opposite things: `game_em_em_core_g_142.c` is MH1's form of em_core_g.c; `main_flfnt_flfnt02_g_516.c` is
  G's form of flfnt02.c. Cause: `wrapper_name` (tools/mh2_carry.py:528-530) appends `_g` after a path that may already
  end in `_g`. Use a separator that cannot be confused, e.g. `__G`.
- Part numbers come from a global counter, so names change on every regeneration (commit 2c52896 renamed dozens, e.g.
  `game_em_em01_g_508.c` → `_511.c`, content unchanged). 395 of 574 wrappers are byte-identical to another wrapper; which
  functions each keeps exists only as section numbers in config/mh2/c_parts.txt. Number parts per file, and have
  `write_wrapper` (mh2_carry.py:533) write the kept function names and address range into the header comment.
- In src/mhg, `cp01_a`, `cp01_b`, `cp01_g` read like parts a, b, g, but `_g` is a hand-written G-only source (same for
  `putspr_g`, `flvu05_g`, `flpad02_g`); `camd_g_p1` is part 1 of camd_g.c. A README line in src/mhg would do.

## D. MH1 game C (src/main, src/game, src/lobby, src/yn)

**D1. `M2C_ERROR(x)` silently becomes `0`: `src/lobby/gonl/gonl.h:15`.** rename/comment only.
- 2,491 uses in the generated G-online files, e.g. src/lobby/gonl/gen/gn_Plaza_add_friend.c:114 passes four "unset register" arguments as
  0. Comment: each use marks an unknown value that the PC replaces with 0, so G-online behaviour can differ there. Same
  for `M2C_CARRY 0` (:17).

**D2. Matching tricks with no comment.** rename/comment only.
- src/main/ud/f_ud.c:550: `... + GV(0x1E8,0x1F6)) - (-(no * 8))` needs "`- -x` kept for matching" (confirm with
  tools/check.py first).
- The flrs* `u64` typedefs (B19).

**D3. m2c temporaries in built files.** rename only (keep the declaration order: it sets register order).
- About 30k `temp_*`/`var_*` uses in 823 built files. Highest value where readers land: src/lobby/f/lb_dr2.c:877-890
  (`temp_s0` is the timer start, `temp_v1_2` the parse result), and the 25 files with `new_var` (e.g.
  src/main/cam/camarea03.c, 21 uses).

**D4. Stale header in src/mhg copies: `src/mhg/main/pl/pl78_main.c:2`, `pl78_sub.c:2`.** rename/comment only.
- Line 2 gives SLPM_654.95 (MH1) addresses and lists all three functions. See also E6.

## E. Dead or duplicated code

All of these **change code shape** in the sense that they change what is compiled or linked. Grep tools/build_pc.sh
and config/*/c_files.txt before each.

**E1. `src/main/pl/pl_snd_nm.c` (2,548 lines) is fully replaced.**
- pl_snd01.c covers the whole range 0x24A240-0x2542E0 (config/c_files.txt:8), has no raw asm and no c_rawfuncs entry;
  pl_snd_nm.c is not in build_pc.sh. pl_snd01.c:1 still says "Whole file in pl_snd_nm.c", and config/main_aliases.txt:3
  cites it; update both.

**E2. `src/main/hk/hk_nm.c` (1,374 lines) is the older copy of `src/main/tu/hk_all.c`.**
- Same body; hk_all.c adds `static` and a header. Delete hk_nm.c and point f_hk.c at hk_all.c.

**E3. Lobby whole-file copies not named `_nm` and in no build: `src/lobby/f/lb_q.c` (540 lines), `lb_x.c` (496),
`lb_y.c` (560).**
- Not in c_files.txt; build_pc.sh only globs `lb_[a-p].c lb_z*.c` (tools/build_pc.sh:141); no tool references them.
  Their functions also exist as matched splits (e.g. `Lb_stage_load` in both lb_q.c and the built lb_gq01.c:98).
  Rename to `*_nm.c` or delete. Add a comment at build_pc.sh:141 saying why the glob stops at `p`.

**E4. About 45 more `*_nm.c` files whose every function also has a non-nm definition and that are not in the PC build.**
- Examples: src/main/net/cpinet_nm.c, ms_nm.c, src/main/cp/cpmath_nm.c, src/main/emw/emwork_nm.c,
  src/lobby/lb/lb_mix_nm.c, lbshop2_nm.c. Found by a script; check each by hand (the regex misses multi-line signatures,
  and "defined elsewhere" can mean a raw-asm stub).

**E5. The yn split files repeat ~330 lines of m2c leftovers each: `src/yn/uiNN.c` (34 files).**
- Each starts with the full ui_nm.c preamble before its one function (e.g. ui12.c is 420 lines; `yn_dialog_draw` starts at
  :340): ~130 unused K&R prototypes, 4-5 "Decompilation failure ... jump table" blocks, 17 empty
  `#if 0 /* ... m2c draft */ #endif` pairs (ui12.c:214-300).
- Removing the comments and the empty `#if 0` pairs: rename/comment only. Trimming unused prototypes: changes code shape
  in principle (rebuild yn to confirm).
- ui_nm.c (2,864 lines) still holds ~29 functions that now match in uiNN.c (`yn_dialog_draw` bodies identical).

**E6. `src/mhg/main/pl/pl78_main.c` and `pl78_sub.c` duplicate `src/main/pl/pl78.c`.**
- pl78.c:38-50 already has `#ifndef VERSION_MHG` around the item-work calls; pl78_main.c is exactly its G form;
  pl78_sub.c is `Pl_view_reset`. Both copy pl78.c's doubled declarations (lines 6-14 repeated at 19-27) and unused
  externs (`flSqrt`, `Gun_data`, `adx_se_set`). They are the only src/mhg files not named `_pN`.
- Switch to `G_PART` guards, or at least remove the dead declarations (that part is byte-neutral).

**E7. `func_0010DD20` is registered twice for the same range: `config/mhg/c_files.txt:1073-1074`.**
- Both `mhg/main/item/item03_b` (which includes src/main/item/item03.c:177, `G_PART 2`) and `mhg/main/item/item_g`
  cover 0x10DD20-0x10DD98. The bodies differ only by a `(s16)` cast. How the linker resolves this was not tested; one
  should go.

**E8. Dead declarations: `src/mh2/game/em_dmg_sys_nm.c:18-19`.** rename/comment only.
- `em_eye_dmg_act_set` and `em_hinshi_end2` are declared, never used.

## F. The PC platform layer (src/pc)

Not byte-matched, so these are about behaviour only. All are rename/comment only (or refactors that keep behaviour).

**F1. Same macro names, different meanings.**
- `PF(p,o)` is a float at an offset in rt_em.c:30; `PF(pl,T,o)` is a typed field that goes through the MH2 remap in
  rt_player.c:52, but not in rt_np.c:32 or rt_hud_gun.c:57.
- `PU8`/`PS16` remap in rt_pl.c:34-40 but not in rt_em.c:25-31, rt_menu.c:19-20, rt_quest.c:19.
- One shared header with distinct names (raw `PU8` vs remapped `PLU8`) would remove the trap. docs/mh2/hunt.md already
  says "other raw offsets in src/pc/rt were not audited": this is where to start.

**F2. `RT_PLO` is defined twice, with an unexplained `8 * 0x970`: `src/pc/rt/rt_pl.c:30`, `src/pc/rt/rt_player.c:48`.**
- 0x970 is MH2's `sizeof(PLW)`; the 8 (slot count) is not documented. Define once in rt.h with the size from B4 and a
  named slot count.

**F3. The "stand-in called once" stub is written six ways.**
- rt_flow.c:28-30, rt_em.c:22-23, rt_eft.c:942-949, rt_pl.c:44, rt_menu.c:244-245, rt_2d.c:371. Each logs through
  `rt_log_standin` and RT_TRACE, with different wording ("not ported", "is a stub", "no-op") and, in rt_pl, a different
  trace switch. One `RT_STANDIN_V/I(name, args)` macro in rt_log.h.

**F4. Five copies of a little-endian 32-bit read.**
- rt_mem.c:22, rt_movie.c:73-74, install.c:187 (`le32`), fmt/ps2save.c:48-49, rt_data.c:155 (`rd32c`). fmt/fmt.h:26-36
  already has `fmt_u32(p, FMT_LE)` / `fmt_u16`.

**F5. Six cached `getenv` trace helpers.**
- rt_quest.c:24 (`qtrace`), rt_mc.c:82 (`trace`), rt_pl.c:82, and 23 raw `getenv("RT_TRACE")` calls. viewer.c:930-946
  (`light_mul`) calls getenv twice per actor drawn. One `rt_env_flag(name)` with a cache fixes both.

**F6. `viewer.c:1-11` describes the first demo.**
- It says it "shows stage 4 with the Rathian ... free-fly camera". The file is now the 4,202-line entry point of the whole
  port (boot, quests, online, MH2); `main()` alone is :2998-4202. Rewrite the header as a map of the file. Splitting
  `main` into option parsing, boot and loop is a further behaviour-neutral step.

**F7. Cryptic globals in viewer.c.**
- `lp` (:623, 41 uses) is the local player slot: `local_slot`. `rh` / `rw` / `rh_ok` / `rh_wmem` (:1828-1832) are the
  remote hunters and their weapons: `remote_hunter`, `remote_weapon`. `light_for_` / `light_mul` (:948, :923) are a
  light lookup and a test-aid scale: `light_lookup`, `light_test_adjust`.

**F8. Local prototypes instead of headers.**
- viewer.c has 41; `rt_motion_refs` is declared twice (:1781 and :2353); `rt_np_*` is a block at :1819-1827 with no
  header. src/pc/title_mh2.c:33 re-declares `rt_log_standin` instead of including rt/rt_log.h.

**F9. Repeated quest-number constants.**
- `0xB2` (first number past the disc's quests) in rt_em.c:507, rt_dbg.c:152, rt_village.c:126; `0xC8` (first event quest)
  in rt_em.c:478, :519. Named constants in rt.h.

**F10. MH1-only address: `src/pc/rt/rt_online.c:596`.**
- `#define LBA(a) (rt_lb_mem + ((a) - 0x533980u))` hard-codes MH1's overlay address; rt_data.c:150 uses `rt_ovl_vram()`
  (G: 0x542600). Comment "MH1 only", or use the accessor.

**F11. `RT_MH2_EM` gate in two places, two explanations.**
- rt_quest.c:59 points at docs/mh2/hunt.md blocker 4; rt_em.c:740 points back at rt_quest.c. One
  `rt_mh2_em_enabled()` with one comment.

**F12. `rt_pl.c` vs `rt_player.c`.**
- rt_pl.c holds stand-ins for player helpers not yet decompiled; rt_player.c is the hunter tick. A name like
  `rt_pl_standins.c` makes the split obvious.

**F13. 13 `RT_*` switches documented nowhere.**
- RT_ADX_SECONDS, RT_COL_TRACE, RT_DEMO_NOWALL, RT_EFT_CHECK, RT_FH_EVERY, RT_FH_TRACE, RT_GAL_SHOTS, RT_NO_MAT,
  RT_NO_TRIM, RT_QCLEAR_ALL, RT_REF_NODES_OUT, RT_SKIP_INTROS, RT_VT_FINE. viewer.c:2992-2995 sends readers to
  docs/pc.md for these; the other 205 are documented there.

## Checked and found clean

- No `#if 0` blocks and no commented-out code lines in src/main, src/game, src/lobby (only the empty m2c-draft pairs in
  src/yn, E5).
- No unused static functions in src/pc. Most magic addresses in src/pc already have a comment.

## Appendix: disagreements found in the docs during this pass

The docs pass kept both sides of each and marked them **Conflict** (or **Later** where one side is simply newer). Listed
here so they can be settled in one place.

1. **game_w size (MH2):** 0x2C0 in docs/mh2.md "Guessed"; 0x2BC proven by seven memsets (docs/mh2/structs.md and
   docs/mh2.md 5.4).
2. **Files left out for ties (MH2):** 118 files in the carry table (docs/mh2.md 5.1); 109 files in section 7.
3. **Two-address files (MH2):** 14 files / 44 functions (docs/mh2.md 5.1); 17 files / 51 functions (section 6 item 7).
4. **config/mh2/c_symfix.txt size:** 145 lines (docs/mh2.md 5.1 "Renames"); 213 lines (5.4 "Name map errors").
5. **"No opcode moved" (em_cmd):** docs/mh2/data.md says no opcode moved; docs/mh2/code.md shows opcode 0x2C's handler
   moved to 0x70 in G (MH2 keeps G's layout). Related: docs/mh2/quest.md lists 0x70-0x7C as "new in MH2", but 0x70 is
   already G's.
6. **Em_Dmg_Sys near-match:** "one register off" (docs/mh2/quest.md); 214 instructions aligned, 8 short, "one register"
   too kind (docs/mh2/code.md); "214 of 667 instructions off" (docs/mh2/hunt.md).
7. **0x3A2E30 near-match:** "about 8 bytes short" (docs/mh2/quest.md); 117 → 96 instructions differ (docs/mh2/code.md).
8. **MH2 sleep tables:** data_map.txt has em_sleep2_data_tbl 0x8BCCB0 and em_sleep_data_tbl 0x8BD550; docs/mh2/hunt.md,
   by the EMW field each fills, reads sleep 0x8BDA00, poison 0x8BCCB0, sleep2 0x8BDB50, new 0x8BD550.
9. **quest_w byte 0x9D:** config/mh2/layouts.txt calls it x168; docs/mh2/hunt.md thinks x168 and x169 are swapped
   (guess).
10. **TIM2 alpha:** docs/mh2_port.md section 3 says doubled (GS 0x80 = opaque); docs/mh2/platform.md says 0-255, not
    doubled (commit c2671f0 "TIM2 alpha, (C) box fixed" suggests platform.md is the later reading).
11. **MH1 match percentage:** CLAUDE.md says "about 8% of all code matches as of 5 Oct"; docs/STATUS.md has 13.1% on
    5 Oct evening and 59.668% on 12 Oct. (CLAUDE.md was not edited: it is outside this pass's file list.)
12. **Full check on the 2 Nov MH2 merge:** the long STATUS entry says full_check.sh was not run; the short one (now
    folded in) says 47 of 47 passed after a fix. Probably run later by the coordinator; the log does not say.
13. **STATUS entry "2026-10-05 (afternoon)":** its numbers (35%) come after the 5 Oct evening entry (13.1%), so the date is
    probably 6 Oct. Heading now says "5 or 6 Oct".
14. **Static recompilation:** rejected as the method (DECISIONS, 3 Oct); "hybrid build" still open (DECISIONS, Open);
    CLAUDE.md says PS2Recomp "is being tested as a stop-gap"; the MH Dos decision (2 Nov) says "no recompiler".
15. **Community mh2-decomp repo:** DECISIONS (1 Nov) allows it as a reference for facts; all MH2 docs say it was never
    opened, and the brief for this pass says not to use it at all.
16. **English sources:** README's first-decisions table and DECISIONS "Open" still list the community MH1j patch as an
    option; the 28 Oct decision set Freedom → MH1 US → our own lines for G.
17. **Server:** DECISIONS "Open" still asks whether to ask MH Oldschool or build a private server; 15 Oct decided our own
    server.
18. **MH2 "11 files written fresh":** docs/mh2.md said 11; after the CODE round there are 15 matched hand files plus 3
    near-matches. docs/mh2.md now says "11 at the 3 Nov merge" and lists the four later ones.
