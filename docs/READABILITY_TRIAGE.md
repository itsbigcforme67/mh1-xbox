# Triage of docs/READABILITY_REPORT.md (4 Nov 2026)

What this is: each suggestion in docs/READABILITY_REPORT.md, checked against the C on main (9c1ac3d5), sorted into
(a) safe, (b) risky, (c) wrong or not worth it. For (a), the exact edit is below. **No C, header, tool or config file was
changed for this triage.** Nothing was compiled. The coordinator applies the (a) edits and then runs the full check.

docs/DECISIONS.md has no owner answers on the C suggestions themselves. The owner's 3 Nov answers ("Settled 3 Nov 2026")
cover the report's appendix only: no PS2Recomp, the English sources, and mh2-decomp as a source of facts. They change
nothing here.

## Rules used

- **(a) safe** means one of these: a comment; removing an exact duplicate declaration or a macro nobody uses; a macro
  used in one file, renamed with the same expansion; a literal written in another base. None of these can change what
  MWCC emits. No game C (src/ minus src/pc, include/) uses `__LINE__` or `__FILE__` (grep), so moving lines cannot
  change bytes either.
- **Four checks still apply to every (a) edit:**
  1. **Carried files.** After editing any shared C file or header that MH2 carries, rerun
     `python3 tools/mh2_carry.py register` (docs/mh2.md 5.1). Same rule as the report's point 2.
  2. **em.h and pl.h field comments are parsed.** `tools/mh2_structs.py` `parse()` (lines 163-208) reads each field's
     offset from the comment: the regex wants `/* 0xNNN` (or `/* G 0xNNN`) right after the `;`. A comment edit on a
     PLW/EMW field line must keep that start. All the em.h edits below do.
  3. **Generated headers.** Edit the generator and the header the same way, so the next generator run changes nothing.
     The generators: tools/mh2_structs.py (mh2_ofs.h, mh2_plw.h, mh2_emw.h, mh2_layouts.h, mh2_lay_ofs.h, cnet_mh2.h) and
     tools/lbauto.py (lobby_a.h).
  4. **Linked symbols are never (a).** Function and global names live in config/*/symbols, *_aliases.txt,
     config/mh2/func_fix.txt, c_renames.txt, hand_parts.txt, c_symfix.txt and pc_*.txt. A **static** can be in them too:
     `game_func_007262D0` in next_cmd_search.c is a same-file static named in config/mh2/c_renames.txt:531 and
     hand_parts.txt:4.
- **(b) risky** also covers renames of struct fields. They are not linker symbols, but tools read the names:
  tools/mh2_structs.py builds mh2_emw.h / mh2_plw.h with em.h's / pl.h's names; tools/lbshopfld.py writes LB_SHOP member
  names; config/mh2/layouts.txt and config/lbnet_fields*.txt hold field names. Locals and m2c temporaries (D3) are put
  under (b) too. Their names almost certainly do not change MWCC's output, but that was not tested.

## Summary

| Item | Verdict | Note |
|---|---|---|
| A1 | a | edits 1-5 below. f_chat/f_hk/f_ud are linked (config/c_files.txt 1858, 1914, 1860); some functions are raw asm stubs (c_rawfuncs) |
| A2 | a | edits 6-8. Verified: main.txt has HardKeyboard_move 0x264180, disp_cursorC 0x2755D0, SoftKeyboard_init 0x25FA90 |
| A3 | a (comment) | edits 9-10. Timer 1 is right: T0 0x10000000/0x10000010, T1 0x10000800/0x10000810; 0x83 = count enable + H-blank clock. The EE_T1_* macros are left out on purpose (comment first) |
| A4 | a (comment) / b (rename) | edit 11. Code settles it: `e->kind = game_w.x28[e->mdl_no]` (flow_nm.c:242), em_create_model loads kind x28[slot], f_quest_nm stores kinds 0x12/0xA. The game.h comment is wrong. The x2E question stays open |
| A5 | a | edit 12 |
| A6 | a | edits 13-17. The yn.h 0x1180-vs-0x1100 question is not settled; edit 16 only flags it |
| B1 | a | edit 18. **Correction:** YNO, SKO, IBO and GWX are G maps (VERSION_MHG), not MH2. GWX also covers MH2 through MH2_GW |
| B2 | a | edit 19. Checked against tools/build.py:53, build_pc.sh:15/454-456, g_alias.h:7, lbnet.h:6-7 |
| B3 | a (comment) / b (values) | edit 20. The GUN_ROW mismatch is real in principle, but no file using GUN_ROW (pl73, pl77, pl_nm, shell06.h users) is in config/mh2/c_files.txt today |
| B4 | a (comment) / b (new macros, hand 0x970s) | edit 21 (generator + header). Using a new `MH2_PLW_SIZE` in version.h and the C is (b): it only works where mh2_ofs.h is included before the use |
| B5 | a | edit 22 |
| B6 | a | edit 23 (generator + both headers). Example verified: mh2_emw.h:137 and :193 |
| B7, B8 | c (for now) | One shared accessor header touches ~170-278 files, many of them carried, and one form (`s8 *`) is not the same text. Real readability gain, but it is a large code-shape job with a full check per batch, not a quick rename |
| B9 | a | edit 24 |
| B10 | a (comment) / b (switch to MH2_OFS) | Verified with the map: MH2_OFS_PLW(0x90B) = 0x863 and (0x368) = 0x370, so the copies agree today. A comment is edit 25. Using MH2_OFS(pl, ...) should fold to the same constant, but it changes the expression |
| B11 | a (comments) / b (merging runs) | edit 26 (generator + header). Merging the repeated runs changes the generator's chain and needs a byte check |
| B12 | a | edits 27-28. Same tokens after preprocessing |
| B13 | a | edit 29. The reason for the `_` rename is not recorded anywhere. The comment says so and does not guess. cmd_target.c already has the comment that B13 asks for |
| B14 | a (comments) / b (renames) | edit 30. Evidence checked: em_core_g_mh2.h:30-32 and :45-46 |
| B15, B16, B17 | b | field renames and struct moves: layouts.txt + regen (B15), tools/lbshopfld.py writes LB_SHOP names (B16), and moving a struct changes the includes of carried files (B17) |
| B18 | b | lobby_f.h is "Add only", and lobby_a.h is regenerated from it. Coordinate first |
| B19 | a (comment) / b (unify) | edit 31 |
| B20 | a | edit 32 removes `BW_FIRSTOPEN` (0 uses in src, include, tools, wip; mh2_lb.h is MH2-only). Edit 33 adds the BSC comment |
| B21 | a | edits 34-35. Verified: item01.c and item03.c have no `G_PART == 1` guard |
| B22 | a | edit 36 (generator + header). **Correction:** QW_X94_T does have a default (mh2_layouts.h:9-11); it only lacks a comment |
| C1 | a (comment) / b (EMW *) | edit 37. All 12 offsets were checked against mh2_emw.h. Note that 0x884 is `act_spd` (MH1 0x930) and 0x888 is `tgt_pos` |
| C2 | a | edit 38. A comment, not a rename: APPROACH is used only in this file, so a rename would be safe too |
| C3 | a | edit 39. "char0 = motion + 1000" is inferred from em01.c:672 (0x3F4) and is marked as a guess |
| C4, C5, C8, C10 | b | linked symbols: symbols/game.txt, *_aliases.txt, func_fix.txt, c_renames.txt, hand_parts.txt |
| C6, C7 | a (comment) / b (rename) | edit 40 updates the em.h comments. The cam.h comments already say what the fields are |
| C8 comments | a | edit 41 (camd_g.c: pi/6, pi/3, Psw[4] = pad 0's PSWE.an, verified against ioread.h) |
| C9 | a (one site) / b (the rest) | edit 42: in mccomb.c:550 `func_0012BDD0(s)` becomes `FONT_PRINT1(s)`. MC_GL and FONT_PRINT1's G form share one condition (VERSION_MHG && __MWERKS__), so the expansion is the same text. A single alias in g_alias.h touches three files and the PC builds: (b) |
| C11 | a | edit 43. Verified against em_cmd_nm.c:83 (mode 5 only) and :429-431 (0x84 no-op) |
| C12 | a | edit 44 (6 places) |
| C13 | a (p003A2E30_nm.c macros, comments) / b (p003A9230_nm.c externs) | Neither file is built anywhere (not in any c_files.txt or build_pc*.sh; rt_mh2_quest.c only cites them). Edit 45 |
| C14 | b | types.h under VERSION_MH2 pulls in mh2_ofs.h, mh2_lay_ofs.h and the MH2V/MH2_EM/QWO... macros. The typedefs are the same, but this changes what each file sees. Rebuild needed |
| C15 | a (comment) / b (struct merges) | edit 46. **Found:** the short `func_6F4900` names resolve through splat's build/mh2/auto/sub_main_undefined_funcs_auto.txt (line 3230), not through the aliases |
| C16 | b | tool change (mh2_carry.py wrapper names) renames 574 files and c_files entries |
| D1 | a | edit 47 (gonl.h is hand-written and PC-only) |
| D2 | a | edit 48, worded as a probable matching trick (not checked with check.py) |
| D3 | b | locals: very likely byte-neutral, but not tested. Keep the declaration order |
| D4 | a | edit 49. G addresses from config/mhg/c_files.txt:883/897 |
| E1-E4, E7 | b | deletes or build-list changes. E1 also needs pl_snd01.c:1 and config/main_aliases.txt:3 edited (comment text only). For E3, `lb_q.c`, `lb_x.c` and `lb_y.c` are in no list (grep) |
| E5 | a (empty `#if 0` pairs, comments) / b (prototypes) | No exact edit: 34 files. Remove only `#if 0 ... #endif` pairs whose body is a comment, then rebuild yn anyway |
| E6 | a (exact duplicate lines) / b (unused externs) | edit 50. Both files are carried into MH2 (config/mh2/c_files.txt:1077, 1194): run register |
| E8 | a | edit 51. The two declarations sit under `#ifndef EM_TAISEI_IN`, so em_taisei.c's PC include never sees them |
| F1, F3, F4, F5, F7, F8, F9, F12 | b (PC) | src/pc is not PS2-built, so no PS2 bytes are at stake. These are refactors that need build_pc*.sh and a PC run |
| F2, F6, F10, F11 | a (comment) | edits 52-55. **Found (F2):** on the PC MH2 build (G form), rt_game.c:82 makes `player_work[10]`, but RT_PLO only remaps the first 8 entries. Entries 8 and 9 would keep MH1 offsets. Not checked whether anything reads them |
| F13 | a (docs only) | not written here: each switch needs its getenv site read |

## The (a) edits

Format: file, then the old text, then the new text. The old text is exact, as on 9c1ac3d5.

### A1, A2: file headers

**1. src/main/hk/f_hk.c:1-3**
```
/* hk_nm - f_hk (SLPM_654.95 0x00164180-0x00167198, main.bin): hardware (USB) keyboard input for the soft keyboard
 * state lpSKey, kana/roman conversion, text insert/delete helpers and the input-line display. Near-match C,
 * not built. Field meanings are guesses; lpSKey layout is shared with sk_nm.c. */
```
becomes
```
/* f_hk - SLPM_654.95 0x00264180-0x00267198 (main.bin offset 0x164180): hardware (USB) keyboard input for the soft keyboard
 * state lpSKey, kana/roman conversion, text insert/delete helpers and the input-line display. Linked (config/c_files.txt);
 * the functions listed in config/c_rawfuncs.txt link the original bytes, and src/main/tu/hk_all.c has the best C for them.
 * Field meanings are guesses; lpSKey layout is shared with sk_nm.c. */
```

**2. src/main/chat/f_chat.c:1-3**
```
/* chat_nm - f_chat (SLPM_654.95 0x001755D0-0x0017BF80, main.bin): sprite/frame helpers, chat log, pit-menu
 * windows (status, equipment), reibun (preset phrases). Near-match C, not built; matching runs are
 * built from it as chatNN.c. Field meanings are guesses. */
```
becomes
```
/* f_chat - SLPM_654.95 0x002755D0-0x0027BF80 (main.bin offset 0x1755D0): sprite/frame helpers, chat log, pit-menu
 * windows (status, equipment), reibun (preset phrases). Linked whole (config/c_files.txt); the functions listed in
 * config/c_rawfuncs.txt (DispFrameMessageA, PlayerStatusWindow, ...) link the original bytes. Field meanings are guesses. */
```

**3. src/main/ud/f_ud.c:1**, first line only
```
/* udmisc_nm - f_ud tail (SLPM_654.95 0x00271FB0-0x00272400, main.bin): copy of the saved hunter data to and from the
```
becomes
```
/* f_ud - SLPM_654.95 0x00271FB0-0x00274E10 (linked; c_rawfuncs.txt functions keep the original bytes): copy of the saved hunter data to and from the
```
(Lines 2-4 stay. They list only the first functions; the file now runs on to Ex_quest_ck.)

**4. src/lobby/b/lb_by168.c:2**, the end of the line
```
still differs in the register choices of the 4x8 byte clear loop (the original recomputes em+i each iteration) and a few nop placements. Not built. */
```
becomes
```
still differs in the register choices of the 4x8 byte clear loop (the original recomputes em+i each iteration) and a few nop placements. The PS2 build links the original bytes for it (config/c_rawfuncs.txt). */
```

**5. src/main/fl/flio01.c:1-2.** Delete line 1 (`/*  */`). On the next line:
```
/* flio_nm - SLPM_654.95 0x0016F290-0x0016F600 (g_flPS2DmaAddQueue.s): host-file helpers of the fl library: build the
```
becomes
```
/* flio01 - SLPM_654.95 0x0016F290-0x0016F600 (g_flPS2DmaAddQueue.s): host-file helpers of the fl library: build the
```

**6. src/main/hk/hk_nm.c:1**, and the same line at **src/main/tu/hk_all.c:5**
```
/* hk_nm - f_hk (SLPM_654.95 0x00164180-0x00167198, main.bin): hardware (USB) keyboard input for the soft keyboard
```
becomes
```
/* hk_nm - f_hk (SLPM_654.95 0x00264180-0x00267198; main.bin offset 0x164180): hardware (USB) keyboard input for the soft keyboard
```
(In hk_all.c, "hk_nm" stays: that block is the copied header of hk_nm.c.)

**7. src/main/sk/sk_nm.c:1**
```
/* sk_nm - f_sk (SLPM_654.95 0x0015FA90-0x00162A90, main.bin): soft keyboard (on-screen keyboard used for chat and
```
becomes
```
/* sk_nm - f_sk (SLPM_654.95 0x0025FA90-0x00262A90; main.bin offset 0x15FA90): soft keyboard (on-screen keyboard used for chat and
```

**8. src/main/chat/chat_nm.c:1** has the same wrong range as edit 2. Change `0x001755D0-0x0017BF80, main.bin` to
`0x002755D0-0x0027BF80; main.bin offset 0x1755D0`. chat_nm.c is in the PC build (build_pc.sh:93); a comment does not
matter there.

### A3: timer comments

**9. src/main/net/cnmsg01.c:185**
```
/* Network time in seconds from the EE timer 0 (started on first use): adds the elapsed ticks scaled to seconds. */
```
becomes
```
/* Network time in seconds from EE timer 1 (T1_COUNT 0x10000800, T1_MODE 0x10000810; started on first use with mode 0x83 =
 * count enable, H-blank clock): adds the elapsed H-blanks scaled to seconds (/16/1000*1.0256: about 15,600 per second). */
```

**10. src/lobby/f/lb_dr2.c:876**
```
/* run the html tag parser for a while (until the timer says stop) and report its state */
```
becomes
```
/* run the html tag parser for a while (until the timer says stop) and report its state. The timer is EE timer 0's COUNT
 * (0x10000000): parse until 0x41 ticks have passed. lb_dr2.c is in the PC build, where that read would fault (untested). */
```

### A4: game_w.x28

**11. include/game.h:146**
```
    u8 x28[4];          /* 0x028 per player quest entry id, cleared by Quest_start (Em_direct_set); pl_demo000 compares with 0x12 (monster-grab demo) */
```
becomes
```
    u8 x28[4];          /* 0x028 monster kind per model slot (em_create_model(slot) loads kind x28[slot]; flow_nm.c: e->kind = x28[e->mdl_no]); cleared by Quest_start; pl_demo000 compares with 0x12 (monster-grab demo) */
```
The x2E comment (:148) stays until someone settles it.

### A5: version.h

**12. include/version.h:8 and :14.** Before line 8 (`#ifdef __MWERKS__`), insert:
```
/* sizeof EMW (monster work): G is 8 + 8 bytes smaller */
```
and line 14
```
#define IPREP_NUM 108 /* recipe pairs in Item_preparation_tbl2 */ /* sizeof EMW (monster work): G is 8 + 8 bytes smaller */
```
becomes
```
#define IPREP_NUM 108 /* recipe pairs in Item_preparation_tbl2 */
```

### A6: headers that name the wrong source

**13. include/lobby_a.h:4 and tools/lbauto.py:88** (the generator writes this line, so change both the same way).
In lobby_a.h:
```
/* generated from lobby.h by tools/lbauto.py: function prototypes replaced by K&R declarations */
```
becomes
```
/* generated from lobby_f.h by tools/lbauto.py (overwritten on each run: edit lobby_f.h): function prototypes replaced by K&R declarations, which change how arguments are passed (a matching trick) */
```
In tools/lbauto.py:88, make the same text change inside the `replace(...)` string literal.

**14. include/g_alias.h:2**
```
 * (include first; empty for MH1, the MH1 PC build and the G PC build). Values come from check.py diffs against G.
```
becomes
```
 * (include first; plain MH1 values for MH1, the MH1 PC build and the G PC build, except that the G PC online build (MHG_NET)
 * gets G's GV values). Values come from check.py diffs against G.
```

**15. include/mh2_ofs.h / tools/mh2_structs.py:19 docstring.** Only the docstring is wrong (it says MH2_OFS is in types.h).
In tools/mh2_structs.py:19-20:
```
only; their few moves are guards in shell.h and eft.h. C that reaches PLW or EMW by number goes through MH2_OFS(p, o) (include/
types.h): it picks the map by sizeof(*p), and is plain (o) for MH1 and G.
```
becomes
```
only; their few moves are guards in shell.h and eft.h. C that reaches PLW or EMW by number goes through MH2_OFS(p, o) (include/
mh2_ofs.h, included by types.h): it picks the map by sizeof(*p), and is plain (o) for MH1 and G.
```
(docs/mh2/structs.md:10 gets the same fix. It is a doc, not code.)

**16. include/yn.h:12.** Not settled, so this edit only flags the question:
```
/* G's work struct (ynw) has 4 more bytes at 0x28: every field from there to the netcnf work (0x1180, unmoved) is 4 higher */
```
becomes
```
/* G's work struct (ynw) has 4 more bytes at 0x28: every field from there to the netcnf work (0x1180, unmoved) is 4 higher.
 * YNO below shifts only up to 0x1100: which end is right is not checked (readability triage, 4 Nov 2026). */
```

**17. src/mhg/main/cam/camd_g.c:2** `fixed mode (x21 = 2, stage 0x4D)` becomes `fixed mode (x21 = 2, stage 77 = 0x4D)`.
**src/mh2/game/p0075FBF0.c**: in `% 100 < 0x4C`, `% 100 < 0x33` and `% 100 < 0x3D`, write the literals as `76`, `51`
and `61`. Same int constants, so the same code. Only the literal changes.

### B: include/

**18. include/types.h:10**
```
/* C that reaches a work struct by number gives MH1's offset; only MH2 moved them (include/mh2_ofs.h is generated) */
```
becomes
```
/* Version macros. The shared C is written with MH1's numbers; these swap in another game's number at compile time.
 * - MH2V(mh1, mh2): the first value for MH1 *and G*, the second for MH2. Nested forms like MH2V(GV(a, g), b) are G inside MH1's slot.
 * - MH2_OFS(p, o) (mh2_ofs.h, generated): an MH1 byte offset o into *p, as MH2 has it. It picks the map by sizeof(*p):
 *   0x970 = MH2's PLW (MH2_OFS_PLW), 0x9D0 = MH2's EMW (MH2_OFS_EMW); any other size keeps o. MH2_EM(o) = MH2_OFS_EMW(o)
 *   where there is no pointer.
 * - One macro per global (mh2_lay_ofs.h, generated): QWO quest_w, NCO net_common_w, UDMO User_data, LBM lb_sys, PMO PitMenu.
 *   CNO is CnetSys_w (lbnet.h for G, cnet_mh2.h for MH2); GWX / MH2_GW game_w (gwx.h, G and MH2).
 * - G-only maps of the same kind: SKO soft keyboard (sk_g.h), YNO yn work (yn.h), IBO lobby item box (lbsys_g.h), GV/GSLOT (g_alias.h).
 * - Outside MH2 (the #else branch) all the MH2 macros are the plain value, for MH1, G and every PC build without VERSION_MH2.
 * - Rule (docs/mh2.md 5.3): inside any of these, write the MH1 offset once. CNO(MH2V(a, b)) moves it twice. */
```
Also on line 16, `/* quest number in copies of quest_w's head: lhu in MH2 */` becomes
`/* quest number in copies of quest_w's head: MH2 reads it as unsigned 16-bit */`.

**19. include/version.h:3-4**
```
/* Per-version constants: MH1 JP (default) or PS2 Monster Hunter G (-DVERSION_MHG).
 * Only values that change C code live here; docs/DECISIONS.md "How G lives in the repo". */
```
becomes
```
/* Per-version constants: MH1 JP (default), PS2 Monster Hunter G (-DVERSION_MHG) or MH2 (-DVERSION_MH2, layered on below).
 * Only values that change C code live here; docs/DECISIONS.md "How G lives in the repo". Which build defines what:
 *   PS2 MH1     none
 *   PS2 G       VERSION_MHG; with __MWERKS__ also MHG_PS2 (g_alias.h)
 *   PS2 MH2     VERSION_MH2 only (tools/build.py); files retried in G form get VERSION_MHG too (tools/mh2_carry.py wrappers)
 *   PC MH2      VERSION_MH2 + VERSION_MHG (tools/build_pc.sh)
 *   PC G online VERSION_MHG + MHG_ONLINE, which sets MHG_NET (lbnet.h) */
```

**20. include/version.h:22**, before `#ifdef VERSION_MH2`, insert:
```
/* MH2 values sit on top of the MH1/G blocks. The PS2 MH2 build has no VERSION_MHG, so it gets MH1's GUN_ROW (0x14), STG_POS_N,
 * MAHI_MAX, IPREP_NUM and EM_SLEEP_END, while the PC MH2 build gets G's. MH2's EQ_GUN is 0x18 bytes (plequip.h): no MH2-carried
 * file uses GUN_ROW yet; check it before one does. */
```
(The second `#ifdef VERSION_MH2` at line 41 needs nothing more.)

**21. B4: include/mh2_ofs.h:1-2 and tools/mh2_structs.py:230-231** (the same text in both)
```
/* Generated by tools/mh2_structs.py gen: MH2_OFS(p, o), an MH1 offset o into the struct p points at, as MH2 has it.
 * For C that reaches PLW and EMW by number. p of any other size keeps o. */
```
becomes
```
/* Generated by tools/mh2_structs.py gen: MH2_OFS(p, o), an MH1 offset o into the struct p points at, as MH2 has it.
 * For C that reaches PLW and EMW by number. 0x970 is sizeof(PLW) in MH2, 0x9D0 sizeof(EMW) (= EMW_STRIDE, version.h).
 * p of any other size keeps o; any other struct of exactly 0x970 or 0x9D0 bytes would be translated by mistake (PLM, mh2_lb.h). */
```
In the generator, the two lines are Python string pieces ending in `\n`. Keep that form, and add the third piece.

**22. B5: include/mh2_lb.h:7**
```
typedef struct PLM {           /* the fields of MH2's player work that lobby code uses; 0x970 bytes */
```
becomes
```
typedef struct PLM {           /* the fields of MH2's player work that lobby code uses; 0x970 bytes. Offsets are MH2's already:
                                  never MH2_OFS(plm, o), which would translate them twice (sizeof is 0x970, as for PLW) */
```

**23. B6: tools/mh2_structs.py:292 and line 3 of include/mh2_emw.h and include/mh2_plw.h** (the same text in all three)
```
 * collide with a better placed one sit in the overlays at the end. Field names are MH1's. */
```
becomes
```
 * collide with a better placed one sit in the overlays at the end. Field names are MH1's: xNNN is the field at MH1
 * offset NNN, and its MH2 offset is in its comment, so a name does not give the MH2 place (EMW: x7EE is at 0x73A, x73A at 0x684). */
```
In the generator, the string is `" * collide ... Field names are MH1's. */\n"` with `% (name, SIZES[name])` after it.
Replace that last piece with the two lines above, as two string pieces ending in `\n`. They hold no `%`, so the format
arguments stay as they are.

**24. B9: include/types.h:14**
```
#define MH2_EM(o) MH2_OFS_EMW(o)  /* for pads of local copies of EMW */
```
becomes
```
#define MH2_EM(o) MH2_OFS_EMW(o)  /* MH2 offset of an EMW field where there is no pointer (pads of local partial EMW copies, emw01.c) */
```

**25. B10: src/lobby/f/lb_j02.c**, the line `PLU8(pl, MH2V(0x90B, 0x863)) = 1;`. Add at the end of the line:
`/* 0x863 = MH2_OFS_PLW(0x90B); 0x370 below = MH2_OFS_PLW(0x368): copies of the generated map */`

**26. B11: include/mh2_lay_ofs.h and tools/mh2_structs.py layouts().** In the generator loop, before
`ofs.append("#define %s(o) (%s(o))" % (macro, chain))`, add:
```
        ofs.append("/* %s(o): an MH1 offset in %s -> MH2's */" % (macro, name))
```
and in the `LBM = """...` string (mh2_structs.py:341), add a first line
`/* LBM(o): lb_sys (G's layout, lbsys_g.h); PMO(o): PitMenu (menu.h); MH1 offset -> MH2's */`.
Apply the same lines to include/mh2_lay_ofs.h: before the QWO line `/* QWO(o): an MH1 offset in QUEST_W -> MH2's */`,
before the NCO line `/* NCO(o): an MH1 offset in NETCW -> MH2's */` (NETCW is net_common_w, not CnetSys_w's CNO), and
before the LBM line the LBM/PMO comment. Then run `python3 tools/mh2_structs.py layouts`, a quick text run with no
compiler: it must leave both headers unchanged.

**27. B12: include/em_core_g_mh2.h.** Line 19 `HEAD(cmd_target_kind_set)` becomes
`void cmd_target_kind_set(EMW *em, f32 *pos) {`. Line 342 `HEAD(target_kind_set)` becomes
`void target_kind_set(EMW *em, f32 *pos) {`. Then delete line 12 (`#define HEAD(n) void n(EMW *em, f32 *pos) {`) and line
645 (`#undef HEAD`). Nothing else uses HEAD in em_core_g.c, em_taisei.c or include/ (grep).

**28. B12: src/mh2/game/em_dmg_sys_nm.c.** Line 45 `HEAD` becomes `u8 Em_Dmg_Sys(EMW *em, u8 *flag) {`. Delete both
`#define HEAD u8 Em_Dmg_Sys(EMW *em, u8 *flag) {` lines (:5 and :11). The `#ifdef EM_TAISEI_IN` / `#else` / `#endif` around
them stays, because the `#else` branch still holds the `Em_Dmg_Sys_` rename and its includes. At line 45, `Em_Dmg_Sys` is
already `#undef`'d in both branches, so the tokens are the same. The file is not in the PS2 MH2 build; em_taisei.c
includes it for the PC.

**29. B13: include/em_core_g_mh2.h**, after line 4 (the end of the header comment), insert:
```
/* Built alone (src/mh2/game/cmd_target.c), em_core_g.h's prototypes of these two are renamed to *_ before the definitions;
 * why is not recorded (commit be27c7b). The #else branch of src/game/em/em_core_g.c is the MH1/G form of the same two
 * functions: check a fix in one against the other. */
```

**30. B14: include/em.h** (each comment keeps its `/* 0xNNN` start, see the rules above)
- :141 `/* 0x3B0 target player (em15 fly 33 / atk 2) */` becomes
  `/* 0x3B0 target player: &player_work[x617] (em_core_g_mh2.h cmd_target_kind_set; em15 fly 33 / atk 2) */`
- :179 `/* 0x617 -1: no ... (em08_fly_act_set) */` becomes
  `/* 0x617 target player index, -1 none (x3B0 = &player_work[x617]; em08_fly_act_set) */`
- :274 `/* 0x827 */` becomes `/* 0x827 commanded target kind (copied to x881 by cmd_target_kind_set) */`
- :275 `/* 0x828 */` becomes `/* 0x828 target kind argument 1 (copied to x882); u16 in MH2 (mh2_emw.h) */`
- :276 `/* 0x829 em08_senkai_pos_no result */` becomes
  `/* 0x829 target kind argument 2 (copied to x883); em08_senkai_pos_no result; u16 in MH2 */`
- :322 `/* 0x882 */` becomes `/* 0x882 copy of x828 (target kind argument 1) */`

**31. B19: include/version.h:46**, the end of the PLW_AT line
```
/* player work is 0x970 bytes in MH2 */
```
becomes
```
/* player work is 0x970 bytes in MH2; the u8 * cast because lbui.h and netsyn.h declare player_work as u8[], pl.h / plf.h as PLW[] */
```

**32. B20: include/mh2_lb.h:30.** Delete the line `#define BW_FIRSTOPEN    BW8(0xE9C)`. Zero uses in src, include,
tools, wip.

**33. B20: include/lobby_f.h:320**, before `typedef struct BSCELL1`, insert one line:
```
/* BSCn(T, b, i, d): field at offset d of element i of an array of n-byte records at b; written this way so the compiler emits the original address math */
```
Then mirror it in lobby_a.h by rerunning tools/lbauto.py, or by hand at the same place. lobby_f.h says "Add only":
adding a line is allowed.

**34. B21: include/g_alias.h**, after line 3 (the end of the header comment), insert:
```
/* G_PART (874 files): defined only by the src/mhg/ ... _pN.c wrappers (tools/mhg_parts.py), never by the normal build, which
 * compiles every function. `#if !defined(G_PART) || G_PART == N` keeps a function in G part N (one contiguous G run);
 * `#if !defined(G_PART)` drops it from every part. */
```

**35. B21: src/mhg/main/item/item01_a.c:1 and item03_a.c:1.** Add to the end of each first comment:
` item01.c has no G_PART == 1 guard: any defined G_PART value drops its !defined(G_PART) blocks, which is all this needs.`
(In item03_a.c, write item03.c.)

**36. B22: include/mh2_layouts.h:9 and tools/mh2_structs.py:398**
`#ifndef QW_X94_T` becomes `#ifndef QW_X94_T        /* a file may retype it before including (reward.h) */` in the header.
In the generator list, `"#ifndef QW_X94_T"` becomes the same text as a Python string.

### C: MH2 and G hand C

**37. C1.** After the `#define` block of each file, insert a mapping comment.
src/mh2/game/p0075FBF0.c, after `#define WDIST (*(f32 *)(w + 0x30))`:
```
/* MH2 EMW offsets used here, by the names in include/mh2_emw.h (MH1 names): 2 kind, 8 work08, 0xA4 ang[1], 0xAC pos,
 * 0x2C2 x302, 0x3A2 x39A, 0x59B x617, 0x6DE x792, 0x7DC x888, 0x817 x8C3, 0x884 act_spd, 0x888 tgt_pos, 0x997 x881,
 * 0x998 x882. So B(0x997) == 1 && U16(0x998) == 0 is MH1 em03_act_set's em->x881 == 1 && em->x882 == 0. */
```
src/mh2/game/p0080D270.c, after `#define H(o) (*(s16 *)(em + (o)))`, insert the same block without the last sentence.
(The offsets come from the report. Each was checked against mh2_emw.h. 0xA4 and "2 kind" come from the code's
`ang[1] = I32(0xA4)` and the header comments of both files.)

**38. C2: src/mh2/game/p0075FBF0.c.** On the line before `#define APPROACH(v, slow)`:
```
/* the distance to the target and the time to get there (MH1: em03_act_set's `common` block) */
```
becomes
```
/* the distance to the target and the time to get there (MH1: em03_act_set's `common` block). Ends in `break`: a use followed by
   a case label is not a fall-through. */
```
and both uses, `APPROACH(v, 500)` and `APPROACH(v, 600)`, get ` /* ends in break */` after them.

**39. C3: src/mh2/game/em01_senkai_sub.c**
- After `#define CHAR0 U16(0x29C)`, insert:
  `/* MH2 EMW offsets by mh2_emw.h's (MH1) names: 0xA0 ang, 0xB0 pos[1], 0x3C0 adj_z, 0x29C char0, 0x680 stg, 0x154 x194, 0x574 x5AC */`
- At the three `if (p->c_rise + 1000 == CHAR0) {` lines, add `/* char0 seems to hold motion + 1000 (MH1 em01.c compares 0x3F4 = 1000 + 12); a guess */`.
  One comment at the first of them is enough.
- `r = 6.2831855f * a / 65536.0f;` gets `/* angle units (0x10000 = full turn) to radians */`.
- `TURN += 0x10000 - TURN;` gets `/* = 0x10000, wrapped to 0 by the (u16) below */`.

**40. C6: include/em.h**
- :121 `/* 0x302 compared with 10% of x792 (hit points? guess) */` becomes
  `/* 0x302 current hit points: 0 or less without dm_flag = dead, healed up to x792 (MH2 Em_Dmg_Sys, em_dmg_sys_nm.c) */`
- :215 `/* 0x792 maximum of x302? (guess) */` becomes
  `/* 0x792 maximum hit points (x302 is clamped to it in MH2 Em_Dmg_Sys) */`

**41. C8 comments: src/mhg/main/cam/camd_g.c**
- `z->x2C = 0.5235988f;` gets ` /* pi/6 */`; `z->x2C = 1.0471976f;` gets ` /* pi/3 */`.
- `void func_001DC160(void) {` gets ` /* the fixed camera (mode 2) for stage 77 */`; `s32 func_001DC1F0(void) {` gets
  ` /* zoom state: -1 manual mode past 15 frames, 1 manual/fixed, 0 none */`.
- `extern volatile u16 Psw[];` gets ` /* Psw[4] = pad 0's PSWE.an (stick directions held), include/ioread.h */`. Whether the
  `volatile` is a matching trick was not checked, so the comment does not say it is.

**42. C9: src/main/mc/mccomb.c:550** `func_0012BDD0(s);` becomes `FONT_PRINT1(s);`. It is inside `#ifdef MC_GL`, and MC_GL
and the G form of FONT_PRINT1 have the same condition (`VERSION_MHG && __MWERKS__`, :149 and :153). The expansion is the
same text.

**43. C11: src/mh2/game/em_cmd_ck.c**
- `if (em->mode != 5 && em->mode != 6) {` gets ` /* MH1 skipped mode 5 only (em_cmd_nm.c) */`.
- `if (n >= 0x3E8) {` gets ` /* 1000-step limit against a runaway program: error 2 */`.
- `*(u8 *)&em->cmd_idx = 0;` gets ` /* cmd_idx is u8 in MH2 already; the cast is MH1's (u16) form */`.
- `case 0x1F:` (:116) gets ` /* cases are in the original's order: new opcodes sit beside the MH1 handler they vary */`.
- `case 0x84:` (:419) gets ` /* a no-op in MH1 (em_cmd_nm.c); MH2 runs game_func_007275D0 for 0x84 and 0x7B */`.
- `case 0x2C:` (:224) gets ` /* G moved em_cmd_myemtype_sel from 0x2C to 0x70; game_func_005885E0 is G's address, MH2 0x72B240 */`.

**44. C12: `/* fall through */`.** Each comment goes on its own line, directly before the next `case`:
- src/mh2/game/next_cmd_search.c, in cases 0x15, 0x1C, 0x79 and 0x7A: after the `p += 3;` (0x79: `p += 4;`) of the inner
  `case 0:`.
- src/mh2/game/em01_senkai_sub.c: after the `if (ADJ <= 130.0f) { break; }` block, before `case 1:`.
- src/mhg/main/cam/camd_g.c, func_001DC1F0: after the `if (z->cnt >= 15) { return -1; }` block, before `case 2:`.

**45. C13: src/mh2/sub_main/p003A2E30_nm.c** (built nowhere). After `#define D_005D8300 ((QRANK *)(quest_w + 0xA0))`, add:
```
/* D_005D8260 = quest_w + 0: the quest state (0 running, 1/2 ending, 3 result); D_005D827E = +0x1E: the script index
   (docs/mh2/quest.md). p003A9230_nm.c declares the same places as plain extern globals. */
```
src/mh2/sub_main/p003A9230_nm.c: the line `typedef struct { s16 op, a, b, c; } QCMD;` gets
` /* local; include/quest.h has its own QCMD {cmd,a,b,c} */`. Before
`static int sub_main_func_003A8990(u16 n) {`, add
`/* static in this file on purpose: same reason as next_cmd_search.c's game_func_007262D0 (MWCC keeps a register across a same-file static call) */`.
The comment says "probably", because nobody has checked that this file needs it.

**46. C15: src/mh2/sub_main/p00378A30.c:5-6**, the end of the header comment
```
   chain in reverse case order, so the case order below is the original's. Game-side callees go by address; the names in comments
   are the map's (docs/survey/mh2_map.csv), not checked. */
```
becomes
```
   chain in reverse case order, so the case order below is the original's. Game-side callees go by address (func_6F4900 ...:
   splat's build/mh2/auto/sub_main_undefined_funcs_auto.txt defines them); the names in comments are the map's
   (docs/survey/mh2_map.csv), not checked. */
```

### D, E, F

**47. D1: src/lobby/gonl/gonl.h**
```
#define M2C_ERROR(x) 0
```
becomes
```
#define M2C_ERROR(x) 0   /* m2c could not tell this value (an unset register...): the PC passes 0, so G-online behaviour may differ there */
```
and `#define M2C_CARRY 0` gets ` /* the same, for a carry m2c could not express */`.

**48. D2: src/main/ud/f_ud.c**, the line `u8 *d = (((u8 *) (&game_w)) + GV(0x1E8, 0x1F6)) - (-(no * 8));` gets
` /* - -x: probably kept for matching */`.

**49. D4: src/mhg/main/pl/pl78_main.c:2**
```
/* Player code (SLPM_654.95 0x00154E90-0x00154F14): Pl_view_reset, init_move_work, clr_move_work */
```
becomes
```
/* G main 0x00127AE0-0x00127B4C (config/mhg/c_files.txt): init_move_work, clr_move_work. MH1: SLPM_654.95 0x00154E90-0x00154F14 */
```
and pl78_sub.c:2 (the same old text) becomes
```
/* G sub_main 0x004CEA20-0x004CEA30 (config/mhg/c_files.txt): Pl_view_reset. MH1: SLPM_654.95 0x00154E90-0x00154F14 */
```

**50. E6: src/mhg/main/pl/pl78_main.c and pl78_sub.c.** Delete lines 19-27 in both. They are exact repeats of lines 6-14:
`void init_set_work();` through `void clr_used_heap(int, int);`. The unused `flSqrt`, `Gun_data`, `Pl_item_num_ck` and
`adx_se_set` declarations (:15-18) are (b): removing them should be byte-neutral, but rebuild G and MH2 to confirm. Both
files are carried into MH2.

**51. E8: src/mh2/game/em_dmg_sys_nm.c:18-19.** Delete `void em_eye_dmg_act_set();` and `void em_hinshi_end2();`.

**52. F2: src/pc/rt/rt_pl.c:28** and **rt_player.c:47**. Before each `#define RT_PLO`, add:
`/* 0x970 = MH2's sizeof(PLW); 8 = MH1's player_work entries. The G-form PC build (rt_game.c) has 10: entries 8-9 are not remapped */`

**53. F6: src/pc/viewer.c:1-4**
```
 * viewer.c - first piece of the PC port: a real-time viewer that loads
 * MH1 data straight from the user's disc files and shows stage 4 with the
 * Rathian (em01) and a hunter, both animating, under a free-fly camera.
```
becomes
```
 * viewer.c - entry point of the PC port (it began as a stage-4 model viewer): option parsing, boot, quests, online,
 * MH2, and the main loop (main() is near the end of the file). Data comes straight from the user's disc files.
```

**54. F10: src/pc/rt/rt_online.c:596**, the end of `#define LBA(a) (rt_lb_mem + ((a) - 0x533980u))` gets
` /* MH1's overlay base; G's is 0x542600 (rt_ovl_vram(), rt_data.c) */`.

**55. F11: src/pc/rt/rt_em.c:740**, `/* monsters only with RT_MH2_EM=1 (rt_quest.c) */` becomes
`/* monsters only with RT_MH2_EM=1 until their set-up runs (docs/mh2/hunt.md blocker 4; rt_quest.c has the same gate) */`.

## What the coordinator should know

- **Apply order.** Do the header comments (edits 12, 14, 16, 18-24, 29-36, 40) in one batch, then the C file comments.
  Then run rebuild (MH1, G), mh2_rebuild, mh2_check and mh2_carry register, and diff c_parts.txt / c_symfix.txt: the
  generator reruns and the register run should both produce no diff. Then the PC builds.
- **Generator pairs**, where the header and the tool are changed together: edits 13 (lbauto.py), 15 (docstring only),
  21, 23, 26, 36 (mh2_structs.py). `mh2_structs.py layouts` is a cheap text run, while `gen` needs the probe votes
  (build/mh2/probe/votes.tsv, not committed). So for edits 21 and 23 the hand edit of the header is the check: the next
  `gen` run must produce the same text.
- **Corrections to the report:** B1 (YNO/SKO/IBO/GWX are G maps), B22 (QW_X94_T has a default), B13 (the cmd_target.c
  comment already exists), C15 (the short names resolve through splat's auto list). B10's numbers are right.
- **New findings:**
  - F2: on the PC MH2 build, RT_PLO covers 8 of player_work's 10 entries. Possibly a PC bug; not checked whether
    anything reads entries 8 and 9.
  - The em.h / pl.h comment format is part of tools/mh2_structs.py's input (offset regex), worth one line in docs/mh2.md.
- **Guessed:** that local and temporary renames are byte-neutral (D3, kept as b), the char0 + 1000 reading (C3), the
  `- -x` trick (D2), and the static reason in p003A9230_nm.c (edit 45, worded as probable).
