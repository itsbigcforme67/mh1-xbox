# English option: how the game stores text, who has translated it, and a design

Agent D, 8 Oct 2026. Research and design only; one small proof of concept (section 5).
Nothing in this file quotes game or patch text. Addresses are PS2 virtual
addresses of SLPM_654.95 (main) unless a file is named. Counts come from
scanning the owner's Japanese disc (disc/mh1) with throwaway scripts, so they
are measured, but "strings" means "NUL-terminated Shift-JIS runs found", which
over- or under-counts a little (noted per row).

## 1. How the game stores text

### 1.1 Where the text lives: in the executables, not in message files

There are **no message files** on the disc. AFS_DATA.AFS (2318 entries, all
Meltw-compressed except the overlays) holds models, textures, sounds and the
quest files, and none of the other entries contain text beyond noise. The text is:

| place | what | how it is referenced |
|---|---|---|
| main (SLPM_654.95) string pool 0x350000-0x38A000 | menus, items, weapons, armour, skills, monster names and descriptions, help pages, memory card and network messages | C string literals, mostly reached through pointer tables in .data (0x2EF000-0x35xxxx) that the ELF relocations keep |
| main string literals elsewhere in 0x358160-0x35BBBA and 0x3674E0-0x36C94C | HUD banners, quest result lines, item pick-up lines, skill names and descriptions | literals in code (addiu from lui), not in a table |
| game.bin (AFS_DATA entry 0) | village chief tutorial pages (about 160 strings, 18 KB), field NPC lines (about 55 strings, 5 KB) | literals in code |
| lobby.bin (entry 1) | the whole village and guild/online UI: about 2000 strings and 160 KB at 0xE4650-0x1100EE, 700 more around 0x127A60, shop / forge / cat / gallery text, plus embedded HTML templates | literals; the village is the lobby overlay |
| select.bin (entry 2) and yn.bin (entry 3) | title and name entry (about 35 strings), memory card / yes-no messages (about 270 strings, 8 KB) | literals |
| quest files m001..m178 (AFS_DATA entries 1999 on, `*.mib`) | quest title, goal, failure condition, client plus story text, clear message | the file header has pointers at +0x60, +0x64, +0x68, +0x6C (title, goal, fail, details) and the clear line at a later pointer; text sits at the end of the file |
| AFS_DATA entry 2179 (`ask_mh.dic`, 621 KB) | the ACCESS "ASK" kana-to-kanji dictionary of the soft keyboard (Japanese input, not to be translated) | the soft keyboard (src/main/sk) |
| dnas_net.bin / dnas_ins.bin | DNAS (to be removed anyway) | - |

The AFS name directory is shifted around the font: entry 1746 is listed as
`s20rm04.bin` but is the font (780800 bytes decompressed, the size of 7808
glyphs); trust the entry index (0x6D2, from font_set), not the name there.

### 1.2 Encoding

Strings are **Shift-JIS (cp932)**, NUL terminated, with these conventions
(seen in the font code and in the strings):

- Double-byte characters for kana and kanji; **full-width Latin letters and
  digits** for most HUD and menu text (the Japanese look), half-width ASCII
  appears in a few places (numbers, `%d` format strings).
- `\n` (0x0A) is a hard line break; boxes are laid out by the translator, not
  wrapped by the game (the tutorial pages use explicit line breaks).
- `~Cnn` (tilde, C, two digits) switches the text colour palette inside a
  string (font_print handles it); `~A..` selects another effect. A translation
  must keep these tokens.
- Many messages are `sprintf` formats; counts are inserted as full-width or
  half-width digits by the caller.
- No custom character table and no pointer-indexed message bank: the glyphs
  are looked up straight from the Shift-JIS code.

### 1.3 The font

Documented in src/pc/rt/rt_font.c and docs/pc.md ("Fonts"):

- One file, AFS_DATA entry 0x6D2 (loaded by `font_set`, src/main/font/fs2_01.c):
  **7808 glyphs, 20 x 20 px, 2 bits per pixel**, in JIS row/cell order
  (index = (jis_hi - 0x21) * 94 + jis_lo - 0x21). Colour comes from palettes
  set with `flfntSetPalData` (`nfcol_tbl`).
- Text is drawn by the fl font library (src/main/flfnt, f_flfntsjis2 at
  0x216490-0x217760) and the game's print helpers (src/main/font:
  `font_print`, `_ex`, `_sp`, `_double`, `_uf`). Prints are queued into five
  stacks by z and drawn by `flfntDraw`.
- **ASCII is drawn with the font's own half-width glyphs**:
  `flnecAscii2Sjis` maps a byte to Shift-JIS row 0x85 (0x851F + c for
  0x21-0x5F, 0x8520 + c above) and the glyph is drawn from that row. I
  measured these 94 glyphs from the font: **all ink lies in the left 10
  columns of the 20 x 20 cell** (columns 0-9), and the pen advances by
  exactly w/2 for ASCII (`flfntSetHalftype(1)`; the other mode is 2w/3) and
  by w for double-byte. So English already renders, but **monospaced at half
  the cell width** ("i" and "W" take the same 10 px at size 20, 9 px at
  size 18, the size most menus use). There is **no width table**.
- Common sizes: 18 (menus), 20, with the HUD using fixed boxes.
- A second, tiny 8 x 8 font (`FONT8_8.APX`, `sfont.apx`) exists for debug text.

### 1.4 Text drawn as images

Not text-drawn but baked into textures (these need replacement art or
leaving Japanese): the title logo and title menu art (select_tex.bin), the
end logo (end_logo_tex.bin), the "net connect" screens (net_connect_tex.bin),
the demo/attract art (demo_tex.bin), cockpit/HUD sprites (cpit*.apx,
icon*.bin; mostly icons), stage-name caption and area textures (stNN_tex.bin,
needs a visual check), the ACCESS notice at boot (a screenshot of the PC build
shows it is a texture) and the quest-clear style banners that are *text* in
literals but drawn big. The movies (Sofdec) are separate. A visual sweep with
`RT_TEX_DUMP` (docs/pc.md) over a full playthrough would turn this into a
list; I only checked the boot screen.

### 1.5 Size inventory (measured, Japanese, SLPM-65495)

Pointer-table rows are exact (entries that point into the string pool and
contain Shift-JIS); bytes = Shift-JIS bytes, characters are about half.

| category | where | strings | bytes |
|---|---|---|---|
| item names | `item_str` 0x33AB40 | 327 | 3.4 K |
| item help text | `pit_help_item_str` 0x3518E0 | 337 | 20.4 K |
| weapon names (blade 234, bowgun 26) | `Ken_data`, `Gun_data` (a pointer in each record) | 260 | 3.7 K |
| weapon descriptions | `weapon_exp` 0x351EA0 | 765 | 18.9 K |
| armour names (head 75, body 78, arms 77, waist 68, legs 68) | `Armor_*_Data` | 366 | 5.5 K |
| armour descriptions | `armor_exp` 0x352EE0 | 1083 | 25.8 K |
| skills | `Skill_name` 0x334FC0 | 54 | 0.5 K |
| monster names and descriptions | `enemy_name`, `em_name` (35 each), `monster_data` (30 pages) | 100 | 5.3 K |
| hunter titles, status, menu labels, option labels | `menu_str`, `option_*`, `menu_status_str`, `info_str*`, `quest_str`, `hunter_appellation`, ... | about 250 | 4 K |
| memory card and error messages | `mc_msg_*`, `ms_err_str*`, `McardMsgTbl`, `ncm_*` | about 280 | 7 K |
| online connection messages | `net_con_msg*` | about 400 | 8 K |
| quest text, 153 quest files | m001..m178 headers | 153 x (4 + 2) | about 34 K |
| main literals: HUD banners, pick-up lines, quest result, skill help | 0x358160-0x35BBBA, 0x3674E0-0x36C94C | about 1200 | about 20 K |
| main: help / tutorial pages (online guide etc.) | 0x370E80-0x385084 | about 2260 | 64 K |
| game.bin: tutorial pages and field NPC lines | game.bin | about 215 | 23 K |
| lobby.bin: village, shops, forge, guild, online UI, HTML | lobby.bin | about 3100 | 209 K |
| select.bin + yn.bin | overlays | about 300 | 8.7 K |

Order of magnitude: **about 10 000 strings, about 480 KB of Shift-JIS
(about 240 000 characters)**, of which roughly 40% is the online/lobby
overlay. The part needed to *play the offline game in English* (everything
except net_con_msg, the online guide pages, HTML, DNAS, the soft keyboard
dictionary) is about 6000 strings and 200 KB, dominated by item / weapon /
armour descriptions (65 K), quest text (34 K), village text and tutorials.

### 1.6 What a replacement has to deal with

- Table strings (names, descriptions) are reached through pointer tables; the
  PC port already keeps these tables as host copies with relocated pointers
  (rt_data.c), so replacing a slot is trivial.
- Literals in code (about 2/3 of the strings by count: HUD, village, tutorial,
  lobby) are referenced as `lit_...` arrays in the decompiled C, or by
  `lui/addiu` in not-yet-decompiled code. These need either an edit of the
  C (a lookup call), or a pass in the loader that gives every literal its own
  host buffer (so the C's reference resolves to a replaceable one). A
  loader-level hook keyed on the original string's address is the least
  invasive.
- Strings written with `sprintf` formats need `%d`-order care: English and
  Japanese word order differ ("Delivered X of Y").
- Boxes and selection rectangles are sized for Japanese in code constants.

## 2. Community English work on Japanese MH1

Treated as data; forum text was read through a summarising fetch.

- **MH1j English Patch (v9 as of December 2023)**, from the MH Oldschool
  community, by Grender (the admin) with community help. Covers tutorials,
  NPC lines, online menu strings, pop-ups and weapon descriptions; delivered
  as an **xdelta** to apply to a clean dump of the Japanese game. The thread
  shows **no licence, readme or reuse terms**, only free distribution.
  https://mholdschool.com/viewtopic.php?t=122 . A web search also found an
  ISO labelled "[SLPM-65495]-English-Patched" of unclear origin; do not use.
- **MHG "English Remix" (Rev. 15, Amaillo, Nov 2023)** is for *Monster Hunter
  G* (SLPM-65869), not MH1, but is the one patch whose author **explicitly
  allows reuse by anyone** ("files can be used by anyone"), and credits
  earlier translators (Yuzucchi, ViciousShadow, Dixdros). Much G text is
  shared with MH1, so it is a lead for item and weapon names, but G adds
  text and renumbers things, and the permission is the author's for his
  own work, not necessarily for the earlier translators' work inside it.
  https://mholdschool.com/viewtopic.php?t=1143
- Wii MHG English patch (threads 1205, 205): different platform, not useful.
- 2Tie/mh1j (decomp) and GReinoso96/MH1Plus (US-disc patching, documents the
  AFS/PZZ formats of the US build) are in docs/RESEARCH.md; MH1Plus's
  tooling is the best lead for reading the US disc.

Conclusion: **no patch here carries a licence that lets us ship its text.**
The MH1j v9 patch is the community's work and reuse needs Grender's (and the
named translators') consent. Ask first; if refused, we do not use it. No
patch text may be committed in any case.

**Official English:** Capcom released MH1 in North America for PS2 as
SLUS-20896 (the same code base as 2.x JP; MH1Plus targets it). It carries
official English text and a Latin font/layout. That text is Capcom's: **we
cannot ship it or copy it into the repo**, but the installer can read it from
a player's own US disc, exactly as it reads the Japanese one. I did not have
a US disc, so everything about its layout below is unverified.
**Update 13 Oct 2026:** the owner supplied the US disc; docs/mh1_us.md measures where its English lives (ASCII, same
places as the JP text, quest files by number), its font and how US strings map to ours. It supersedes the
unverified guesses about the US layout below.

## 3. Designs

All three produce the same thing at run time: a **string table file** (data,
not code) in the player's install directory, mapping a stable string id to
English text, loaded by a small text layer. The id is the original string's
**location** (table address + index, or literal address in main/game.bin/
lobby.bin, or quest number + field), which is build-specific but stable for
the Japanese build we port; Japanese stays the default.

Shared work (needed for any of them, about the same size):

1. **Text layer** in the port: `T(id, original)` lookup used by the
   table-patching loader (done as PoC for pointer tables, section 5) plus a
   literal-redirect pass; a language setting in the options menu.
2. **Proportional font drawing** (section 4).
3. **Layout fixes** for boxes, plus `sprintf` format sets with English order.
4. A **string dump tool** (`tools/text_dump.py`, reads the user's disc, writes an
   id/Japanese skeleton to the user's disk only, never to git) so translators
   have a worklist.

### (a) English from the player's own US disc

- Work: an installer step that opens the US disc (SLUS-20896: SLUS ELF,
  AFS, overlays), extracts its strings, and **maps them to our ids**. Tables
  with the same layout (items, weapons, armour, skills, monsters: same record
  order) map by index. Literals do not: the US ELF is a different link, so
  addresses differ, and the US game may have renumbered, merged, dropped
  (online/DNAS-specific text, HTML) or added text. We would need a mapping
  table (our id -> US address) built once by us by comparing the two ELFs
  (function-by-function alignment, which the decomp project already
  makes possible) and stored as pure address/length data with no text.
  Quest files are in the same format; the US quests may differ in count and
  order.
- Font: the US text is ASCII, so it uses our half-width glyphs; if it
  carries its own font entry, use it only for display. US boxes were laid out
  for proportional English, so their original widths are a hint for ours.
  The US disc's English text bytes are Latin-1/ASCII, not Shift-JIS.
- Risks: legal comfort is the best of the three (we ship none of it; this
  is the same stance as reading the Japanese disc), but the mapping effort is
  large and only works for people who own the US disc (the owner does not
  appear to have it: disc/ has only Japanese discs); US text exists only
  where the US game had that feature (no MH1j-only text, e.g. online
  guide); gaps fall back to Japanese or to (c). The US build is the
  *post-patch* 2.x-style game, so quest and balance differences exist
  beyond text.

### (b) A community patch applied at install time

- Work: get the translators' written permission and the credit line they want;
  then ship **a tool** (not the text) that reads the xdelta, or extracts the
  strings from a patched ISO the player makes, and maps the translated
  strings to our ids. An xdelta patches bytes of the JP ELF/overlays, so the
  installer must apply it to a temporary copy of the player's disc and
  then **diff the result against the Japanese original to extract the changed
  strings by address**: the community patch uses the same addresses as our JP
  build (it patches the very same SLPM-65495), so mapping is exact and cheap,
  unlike (a). Strings that the patch lengthened may have been moved to free
  space with pointer edits; the diff tool must follow the pointers.
- Font: the patch (like the original) relies on the monospaced half-width
  glyphs, so our proportional rendering is an improvement over it. Box sizes
  in the patched game are already adapted to what the patch fits (mostly
  by line breaks it inserted), so layouts mostly work as is.
- Risks: permission (none stated; must be asked: Grender / MH Oldschool,
  which we also need for server permission in docs/network.md, so ask once),
  incompleteness (v9 covers tutorials, NPC, online menus, pop-ups and weapon
  descriptions; likely not all 153 quests, items or armour: not verified),
  translator credit and a "do not redistribute the patch" stance by the
  patch, and the patch version drifting. Best quality per effort **if
  permission arrives**.

### (c) Our own translation

- Work: about 6000 strings, 200 KB of Japanese to translate, offline-game
  part first. Items, weapons, armour and monsters (about 1200 short names)
  are quick; descriptions (about 1850 strings) and quests (153) next; village
  and tutorial text (3 K strings) is the bulk. Stored in the repo as
  `data/text/en.tsv` (id, English). It is a translation of the facts of a
  game, not a copy of the game text, but Capcom-owned *names* (Rathalos
  as the official English name, etc.) are best taken from official
  sources, which brings the legal question back for names. Recommend:
  standard fan names, no verbatim official sentences.
- Font: same as everyone.
- Risks: effort (large; machine-aided draft plus a human pass, the owner
  reads no Japanese well, so quality control is hard), consistency with
  community naming (players will expect the usual English names), and no
  verification of meaning beyond the JP source. Cleanest legally, slowest.

### Cross-cutting recommendation

Do **(b) first for what exists, (c) for gaps, (a) as a bonus if cheap**:
1. build the text layer and proportional font now (needed by all);
2. write the dump tool and send the permission request to Grender / MH
   Oldschool (also asking about the earlier translators credited in v9);
3. while waiting, start (c) with names (items, weapons, armour, monsters,
   menus, HUD) because every option needs those short strings, fitting the
   existing boxes;
4. if the owner gets a US disc, evaluate (a) with a diff of the two ELFs.

## 4. Font and width changes for English

- Fixed advance is the problem, not the glyphs. Half-width ASCII glyphs fill
  columns 0-9 of the 20-px cell; English at 10 px per character is wide
  (the same as one Japanese character per two letters), so long names
  overflow boxes that held 6-8 kanji (a 6 kanji box is 120 px: 12 English
  letters at monospace 10 px, or about 16-18 proportional).
- **Proportional text without new art**: measure each ASCII glyph's real ink
  extent from the bitmap (min/max column, computed once at font load; 94
  glyphs) and advance by ink width + 1 (space = 4 px). This is a small change
  to `flfntFontPuts` (src/pc/rt/rt_font.c on PC; the same loop in the Xbox
  build) gated by the language setting, and roughly halves the width of
  English text. Double-byte glyphs keep w. Width measurement belongs in the
  loader so it is ready for strings passed in any font size.
- Text boxes: most are fixed rectangles defined by constants in menu code
  (columns and `font_print` coordinates). Allow per-string overrides: a
  max-width in the string table, and an automatic word wrap using `\n` when a
  string is longer than its box. Where the Japanese string has a hard-coded
  box (lists with right-aligned prices, cursor rectangles, the 5-column
  monster pages), re-lay out by hand.
- Optional: a nicer Latin font. The font file is replaceable (it is just
  glyph data we read from the disc); a freely licensed bitmap or vector font
  rasterised at 18 and 20 px at install time would look better than the
  Japanese-style half-width glyphs, but changes the look: keep the original
  glyphs as default.
- Text in textures (1.4): leave Japanese at first; re-draw only the title
  logo and the central banners later if wanted, with our own art.
- The `~Cnn` colour codes and `\n` must pass through unchanged.

## 5. Proof of concept

What is built (committed, no game text): `RT_TEXT_TABLE=file` makes the PC
build replace pointer-table slots after the data tables are imported
(`rt_text_override`, src/pc/rt/rt_data.c). File format, one per line:

    0x2EF860[0] = <replacement string>

(table at PS2 address, slot index; `\n` allowed, `~Cnn` works). The test uses
the pause menu table `menu_str` (0x2EF860, 25 slots), which holds the labels
Item / Mix / Records / Quest / Options and the status entries, drawn by the
game's own `font_print` -> `flfntDraw` code. The test table contains only my
own strings; see the result below.

PoC result (8 Oct 2026, quest 10, Start pressed, 640x448 shot; the image is not committed): the five
menu labels render in English inside the existing menu box, drawn by the unmodified game font code
(RT_FONT_TRACE shows the replaced strings at the original positions). Run:
`RT_TEXT_TABLE=my.txt build/pc/mhview disc/mh1 --quest 10 --input "idle*120,start*3,idle*40" --shot out.png --frames 400 --size 640x448`.
The label is drawn by the unmodified font code using
the half-width glyphs at 9 px advance (size 18), confirming that pointer
tables can be re-pointed with no code changes and that English already
renders (monospaced). The literals-in-code part of the design is not tested.

### 5.1 The text layer (built 8 Oct 2026, agent D)

Code: src/pc/rt/rt_text.c (table, registry, quest text, wrapping), rt_text_sp.c
(sprintf / strcpy / strcat for the game C only: tools/build_pc.sh compiles the
decompiled game C with `-Dsprintf=rt_text_sprintf -Dstrcpy=rt_text_strcpy
-Dstrcat=rt_text_strcat`, host code keeps libc), hooks in rt_data.c (map_ptr /
map_lb / map_sel re-point pointer words, `rt_data_hosts`), rt_font.c (the
font_print family looks its format up; proportional ASCII), rt_hit.c
(`load_file_mdl` hands mission files to `rt_text_quest`).

Use: `python3 tools/text_dump.py` writes `text/template_ja.txt` (gitignored;
ids with the Japanese as a comment: main 4017, game 223, lobby 2994, select 33,
yn 269, quest 560). Copy it to `text/en.txt`, fill in the entries, run with
`RT_TEXT_TABLE=text/en.txt` (empty or missing table: Japanese, unchanged).
Ids: `main:0xVA`, `game:`, `lobby:`, `select:` (PS2 address of the string),
`quest:N:0xFILEOFFSET`, `0xTABLE[idx]` (a pointer slot). Value: `\n`, `~Cnn`,
`{w=px}` prefix = wrap at spaces to that width (size 20). `@proportional = 0`
or `RT_PROP=0` keeps fixed-width ASCII.

Proportional ASCII: ink widths measured from the font at load, advance = ink
width + 1 (space 8) for strings with no double-byte character; mixed strings
keep the fixed advance.

Verified (own test strings, x86 PC build, screenshots in scratchpad only): the
five pause-menu labels; three item names in the item list (box fits, narrow
letters narrow); quest 10 title / goal / failure / client text on the pause
menu's quest-check pages (the client text wrapped to six lines by `{w=250}`);
the quest-clear banner (RT_FONT_TRACE shows the replacement during a scripted
quest 131 clear). All PC tests pass with no table loaded (urgent, progression,
name_entry, movie, frog, audio, log, pick, activities 35/35, quest_loop,
all_quests 38/38); build_win.sh and build_xbox.py build.

Not verified / known gaps: the Windows build and the Xbox build with a table;
the village quest board and shop text (lobby overlay: ids are dumped and the
lobby pointer relocation is hooked, but I only tested main and quest text);
strings used through other string functions (memcpy, strncpy) are not caught;
a string the code takes the address of inside another string is not.

## 6. Open questions (section 7 answers some)

- Ask the owner: do they have a US PS2 disc (SLUS-20896)? Do they want the
  title/banner art redrawn?
- Permission request to Grender / MH Oldschool (MH1j patch v9), and to the
  named translators in the MHG remix if G text is wanted.
- Verify v9's coverage (does it include all 153 quests, items, armour?):
  needs the patch applied to a private copy and compared to the JP strings;
  nothing from it may be committed.
- A visual texture sweep to list every texture with Japanese text.
- Whether the US build changes quest numbering (needed for (a)).

## Sources

- https://mholdschool.com/viewtopic.php?t=122 (MH1j English patch v9; no stated licence)
- https://mholdschool.com/viewtopic.php?t=1143 (MHG English Remix; reuse allowed by its author)
- https://github.com/GReinoso96/MH1Plus (US build tooling, AFS/PZZ)
- docs/RESEARCH.md, docs/pc.md "Fonts", src/pc/rt/rt_font.c, src/main/font/
- Measurements: disc/mh1 (the owner's Japanese disc), scanned with throwaway scripts.

## 7. English from the player's own discs: tools/text_map.py (agent E, 13 Oct 2026)

G's coverage by category, the G-only text rules (agent B, 20 Oct 2026: 47% -> 51%), and what Monster Hunter
Freedom / Portable is expected to add: docs/english_g.md.

Owner's decision (13 Oct): G is the main release and its English starts from the official US text. `tools/text_map.py`
reads the three discs (disc/mh1, disc/mh1_us, disc/mhg: the executables, the overlays unpacked in memory, the quest
files of AFS_DATA.AFS), ties each US string to its MH1 JP string and that one to its G string by structure, and writes
two table files in the format rt_text.c already reads, into `build/text/` (git-ignored; the text never enters the
repo):

    python3 tools/text_map.py --report docs/text_coverage.md --ids docs/survey/mhg_text_untranslated.txt
    build/text/en_mh1.txt    ids of the Japanese MH1 build: RT_TEXT_TABLE=build/text/en_mh1.txt build/pc/mhview disc/mh1 ...
    build/text/en_mhg.txt    ids of G (see "What G needs")

About 3 minutes and 1 GB of RAM (the code-pair search is cached in build/text/cache_*.pkl; `--no-cache` redoes it).
The committed report (counts and ids only): docs/text_coverage.md and docs/survey/mhg_text_untranslated.txt (every
G string id without English, with its area and reason).

### How the strings are tied (nothing is paired by guessing at meaning)

1. **Code reference.** Each MH1 JP function is looked up in the US / G code by masked 6-word n-grams (as
   tools/carry_match.py); edited functions are aligned on the n-grams that still match and the words between them. The
   `lui` + `addiu/lw/..` pairs (and gp-relative loads) at the same position give JP address -> other address. Nearly
   all text is not reached this way (docs/english.md 1.1 said "literals in code"; measured: only 56 of 2994 lobby
   strings and 67 of 4017 main strings are loaded by a lui/addiu pair; the rest are pointer cells of data objects, all
   of them inside a named symbol of the JP build). So code references mainly give the **address of each data object**
   in the other build.
2. **Table index.** JP object (symbol with size) -> other build: from (1), for G from the MH1 names tools/mhg_map.py put
   at G addresses, from pointers inside objects already found (a parent table holding a pointer to the child), from
   the identical-Japanese-text anchors (G only: a >= 12-byte string whose bytes are identical and unique in JP and in G
   gives its table's shift), or from the neighbouring object's shift. Each placement is checked against the whole
   object: its word classes (string pointer / zero / data pointer / integer) must agree 95% (layout guesses) or at
   least 25-80% (code / name hits; edited tables keep their place but change contents). Then the string cells are
   paired by row: by position for plain arrays, by the number fields where rows were inserted (US: Item_data aligns
   the item name table, weapon_exp / armor_exp follow the Ken_data / Gun_data / Armor_*_Data rows), by the Japanese
   text for G (identical text first, then fuzzy; rows moved by G are found anywhere in the table).
3. **Quest number.** JP quest files <-> US files by the non-text data of the file (128 of 153; the village quests
   131-177 were rebalanced and stay out). JP <-> G quests by identical fields (71 of 245).
4. **Pool order** (US only, 139 strings): between two paired strings of one string pool, the same number of strings on
   both sides pair one to one.

Checks that drop a pair: different printf conversions; the "English" is still Japanese; equal to the JP bytes; for prose
tables a length correlation below 0.3 between the JP and US strings of the table (catches misaligned tables); the
memory-card / yes-no dialogs are left out on purpose (the US cuts these messages into other lines: 220 strings, need a
per-message alignment). G strings whose Japanese text changed from MH1 take the US text only when the change is small
(similarity >= 0.8, same digits and conversions).

Quality (13 Oct, two random samples of 30 pairs each, read against the Japanese by the agent): about 90% plausible
at the pair level; the wrong ones were multi-line descriptions cut at other places (line 2 of the JP armour text paired
with the English line 2 whose split differs), memory-card message lines and one item name from a fuzzy Item_data row.
This is a sample, not a proof: the table is a good base for review, not final text.

### Result (docs/text_coverage.md has the table by area)

- MH1 JP (round 2, 13 Oct): 6183 of 8148 strings (75%) with official English: village NPC lines 96%, quest board 94%, tutorial
  89%, quests 83%, skills 100%, items 99% (was 19%), weapons 99% (72%), armour 100% (67%), menus / HUD 71%, village UI 66%,
  monsters 61%, code literals 9%, memory-card / yes-no dialogs 8%, online / keyboard 0% (US online text is another service).
  What fixed items / weapons / armour: (1) the ordered row alignment gave up where the US reordered ids; rows left over are now
  matched anywhere by identical records, then by best similarity (`moved_rows`); (2) the weapon / armour / monster name tables
  were dropped by the prose length check, which says nothing about tables tied by their own record numbers (`RECORD_ALIGNED`);
  (3) the item help table has 24 prompt rows before item id 0 (`SLAVE_OFF`) and rows that hold an empty string no longer end the
  other build's table early; (4) two file-local symbols called `menu_str` shared one key (the 100-byte string table and a
  28-byte index table): duplicates now get an address suffix, so the 25 menu labels are located (US: sub_main 0x5ADAC0);
  a neighbour's shift is refined by colour codes (`~Cnn`) when a table gained entries (`ObjMap.refine`); (5) the code pairing now
  also maps the words of a function that no unique 6-gram covers (shared prologues) on the winning diagonal; (6) a JP string no
  table paired takes the English of identical Japanese text when all paired uses agree (`propagate_same_text`, 69 strings; this is
  how the item window's title "Items" is reached: the US code no longer reads that table).
- G: 4943 of 13263 string ids (37%, was 29%); 6824 are G-only (no MH1 twin), 1252 have a JP twin but no US text, 244 were edited
  in G. G quests tied to MH1 quests: 128 of 245 (was 71), the new ones by the non-text data (`quest_data_ties`).
- Left over, by area: see the report. For all G-only text the candidate source is the MHG "English Remix" patch
  (section 2; Amaillo allows reuse). It was **not downloaded or used**.

### Hidden MH1 run in English (proof that the US strings render)

`RT_TEXT_TABLE=build/text/en_mh1.txt build/pc/mhview disc/mh1 --quest 10 --input "idle*120,start*3,idle*40" --shot x.png
--frames 400 --size 640x448` (headless shot, nothing on the owner's screen; shot taken with an earlier build of the table, the final one loads 4935 strings,
0 bad lines, and still holds that line): the
pause menu's help line shows the US sentence ("View a list of the items you are currently carrying.") in the unmodified font
code, inside its box, two lines as the US laid them out. The menu labels were Japanese in that first shot; round 2 locates `menu_str` (see Result) and the pause menu now shows Items / Combine /
Reference / Quest Info / Options, and the item window's title "Items" (shots in build/text/shots/, not committed).
The item list and the quest-check pages could not be shot reliably (the scripted input timing varies with the frame rate).

### What G needs on top (checked on the discs; the G PC build was not run with a table)

- **Font: G's font file is byte-identical to the US one** (AFS_DATA entry 2478 in G, 1745 in US: all 7808 glyphs equal;
  MH1 JP's differs in all but 6). So G's half-width Latin glyphs are the ones the US layouts were drawn for; the MH1 JP
  run above used the JP set. Nothing to convert; check the ASCII advance (rt_font.c measures it from the loaded font, so it
  follows).
- **ids.** G's resident `sub_main` (0x4A0C80-0x541xxx) lies between main and the overlays, so rt_data.c's `v < LB_VRAM` test
  already counts it as main: `en_mhg.txt` writes `main:0x...` for it. game/lobby/select share 0x542600; yn and nethttp
  0xA06200 (unused by the text layer). rt_data_hosts(main) must be able to answer for sub_main addresses (rt_addr / the
  G meta); not tested.
- **Quests.** rt_hit.c hooks file indices 1999..2177; G's mNNNNN.mib start at AFS index 2934 (files in file-name order) and the
  number in the id is the file name's number. G's header (0x48 bytes) holds the text strings inline from 0x68 up to the offset
  in word 0x60, and AT that offset (checked on all 245 files) a table of the strings' file offsets (title, goal, failure,
  client, ...). So `rt_text_quest_g` (src/pc/rt/rt_text.c) does what rt_text_quest does for MH1: appends the replacement at
  the end of the file and changes the table entry; nothing shifts. It compiles; it is **not hooked up** (the G PC build does
  not load quests yet; the hook needs the file-name number of AFS index n, which the PC loader has no table for) and not run.
- **Boxes.** The US strings carry the line breaks the US designers chose for these boxes, so they are used as they are
  (no `{w=}` wrapping is generated). Text G changed from MH1 (the 224 left out and the near matches) may not fit; the
  `~Cnn` colour codes come from the US string unchanged (the US kept 516 of them) and are not compared with the JP ones.
- **Not covered**: tables whose US counterpart was not located, code literals, G-only text (Remix), memory-card
  dialogs (need grouping of the lines of one message), the item ids that the US reordered (needs a better row key than
  Item_data), textures with text.

Open for later: group-wise pairing of multi-line messages (mc_msg, yn_dialog, weapon / armour lines) instead of line by line;
an explicit US <-> G alignment of Item_data and the quest headers (G is closer to the US build than MH1 JP in item ids?
not checked: the G item table location came from layout names and its record layout did not match); a sub_main space in
rt_text.c if the main:/sub_main distinction is ever needed.

### G on the PC with the table (agent E, 14 Oct 2026)

`RT_TEXT_TABLE=build/text/en_mhg.txt build/mhg/pc/mhview_g disc/mhg ...` (hidden, `--shot`): the table loads (5151 lines,
0 bad), 5100+ pointer words re-pointed (RT_TEXT_STAT=1). Seen in English in unmodified G code: pause menu labels and the
help box (quest 1), quest-info pages (title, goal, failure text: the AFS loader hook for mNNNNN.mib), the memory-card
dialogs of new game / continue (slot labels, "Please select a save location.", the 3-line "Using memory card ..." box,
"Character confirmed ..."). Still Japanese: window titles (menu, quest info, request), options page, the village's house
hint, item window title shows "(null)", HUD hints. Lines fit their boxes (US breaks kept or re-flowed at 42 characters).
The village HUD, item box and an NPC talk could not be reached by script (the first-visit event runs a camera tour and
RT_VILLAGE_START frames do not take input). Memory-card messages are written as per-slot lines (see docs/REVIEW.md).

### G round 3: the village in English (agent E, 15 Oct 2026)

G with `RT_TEXT_TABLE=build/text/en_mhg.txt`, hidden (tools/gvil_ev.py scenes, shots in build/D/g/, not committed): 37% -> 47% of G's strings (6363 of
13263). Seen in English in the unmodified G code: item shop (buy / sell tabs, list, price prompt, the not-enough-money line), weapon and armour shop (tabs, list,
the current hunter type line, prompts), forge (create / improve), the item box (menu, item window, prompts), the elder's story pages, the pause menu (all
rows, the help box), the Status page (title, rows, page 1/2 and 2/2), the equipment page, the quest-info pages in a quest
(page titles, labels, area, quest name), the bench action menu and hints. What it took:
- **Same Japanese text elsewhere (G):** a G string no table paired takes the English of the identical Japanese text (JP build or G) when all uses agree
  (`propagate_same_text_g`, +1076: the G-only quest tables whose text is MH1's, labels, armour names). Blanks-only differences count as agreeing.
- **The US build holds G's offline UI:** the US lobby / sub_main strings of the offline village (action menu, item box, cooking, price chart, quest result
  and HUD messages, status and equipment pages) are the text of a later source than the JP MH1 disc. The structural pairing cannot reach them (the
  functions differ). They were found by aligning the string pools of G and the US build by order (tools/pool_align.py: a dynamic-programming alignment
  between anchors that are already paired, scored by printf conversions, colour codes, newline count, length ratio, and a small dictionary of names from
  the paired item / monster / weapon tables) and then **checked pair by pair by reading both sides**: 300 pairs in tools/text_pairs_manual.txt
  (`g` G -> US, `jp` JP -> US, `gf` forced when the US line prints fewer numbers, `drop` no English), addresses only. The aligner alone was not good enough:
  about one pair in three was wrong where G edited a line.
- **lb_menu_msg and its siblings** (G lobby 0x6368D0 / 0x636910 / 0x636970; the online lobby's menu rows): the code reference of MH1's
  functions does not reach the US ones; the rows are paired by hand now (first row .. logout; the mail row has no US twin).
- **Wrong rows found and removed:** the MH1 table `plaza_main_msg` (shifted), the tutorial titles (US has extra pages), G status rows that had English of
  the next row (name / weapon / HR / HRP / money). A semantic check (a dictionary of ~100 UI words) flags such rows, but it flags many right ones too (543 of
  6400, mostly item descriptions); it was used to find these, not to filter the table. Checked 15 Oct by eye: roughly 25 of ~100 flagged rows in the village UI were wrong.
- **rt_font.c:** `flfntPrintf` now looks its format up in the text layer: G's pause menu passes the page titles (Status, Equipment) as the format.
- `text_map.is_japanese` counts only kana and kanji now.
- **Still Japanese in the village:** the talk window's title, G-only labels (some hunter rank names, a HUD toggle, the hunting-record page),
  G's new NPC lines (447 G-only lines, 123 edited: no official text; the candidate source is the MHG "English Remix", not used), armour / weapon names of
  the G-only pieces (2500+), the G-only item names, the memory-card dialogs of G-only slots, online text (US has another service), G's quest counter
  (not reached by a script: its NPC position is not in the docs). Not looked at: the options page's value rows, the character creation help box
  (no text in the select overlay, so the "junk bytes" box is not a table string).
