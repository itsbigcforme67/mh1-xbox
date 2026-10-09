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

## 6. Open questions

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
