# G English: last own lines and a Freedom-terms proofread (4 Nov 2026)

Research agent, low-CPU round. Only text/mhg_en_own.tsv changed (plus the regenerated docs/text_coverage.md and
docs/survey/mhg_text_untranslated.txt). No game was run; lines were checked with `tools/text_own.py` (fit, codes) and
`tools/text_map.py` (they load). Written by Claude, not a native translator.

## Numbers (tools/text_map.py, G table "total" row)

| | official | ours | with English | of 13592 | without the 421 kana tables |
|---|---:|---:|---:|---:|---:|
| before | 11761 | 1155 | 12916 | 95.03% | 12910 / 13171 = 98.02% |
| after | 11761 | 1170 | 12931 | 95.14% | 12925 / 13171 = 98.13% |

## 1. New lines (15)

- The online browser's nine error pages (lobby 0x637620-0x637D80: repost, error, connection, timeout, DNS name,
  SSL, cancelled, proxy, transfer). HTML tags kept; each English page is no longer in bytes than the Japanese.
- Its menu: 再読み込み "Reload", ネット終了 "Disconnect"; the soft keyboard's ＯＫ "OK".
- Three debug prints (sub_main 0x52C340 / 0x52C360 / 0x52C3A0, attack-hit trace with %f). Not player text; harmless.

What is left (661) is not text: 414 keyboard kana tables, the chat word filter, scan false hits (one-character
code literals, `ｑ\n` runs, the four combo-book fragments that start inside a circled digit: the whole strings are
ours already), digit / kanji-number tables, star and dash placeholders, and the 32 "dummy" talk slots. No
`jp_left.txt` from tools/g_eng_tour.py exists on disk, so no screen trace was checked this round.

## 2. Proofread against Freedom (36 lines changed)

Checked every own line's names against the official G table (Freedom first, MH1 US second) and against Freedom's
disc text where the table had none.
- Wrong numbers: the four "resistance -40 or below" entry rules said "≦ 40" (minus sign lost): now "≦ -40".
- Wrong monster: the herbivore tutorial page said Apceros for アプトノス: now Aptonoth.
- Freedom names: Organizer Guide (was the provisional "Storage Expert", 6 talk lines), Silver Boots, Chrome Metal
  Coat / Coat+ / Coat S and Boots+ / Boots S (were "Coil" and "G/S Boots"), Recov S Lv2, Combo List, Hunter's Site,
  Guild Master (was Guildmaster), Equipment Shop and Crafting Shop (village 武具屋 / 加工屋, were "weapon shop" and
  "blacksmith"), Piercing (ピアス, was "earring", 3 lines), lower-case "guest house" as the official lines write it.
- Skill list (sub_main 0x541118 / 0x541150): Freedom's skill list runs "Torso +2, Torso Increased, ..., Alchemy,
  Guard Inc" in G's order; the box fits 11 columns, so 胴系統倍加 is "Torso Inc" (Freedom's own short form) and
  ガード強化 "Guard Inc". Heat / Cold Inc take Freedom's 【×1.5】 brackets, like its "Cold Inc【×2】".
- A missing material: the Lao-Shan Lung talk line now names its Spine too.

## 3. For the owner

- **Bug outside our file:** lobby 0x638430 and 0x638480 ("save complete / deletion complete, press ○") get
  binary garbage as *official* English in build/text/en_mhg.txt (`` ` \nj ``). Our lines cannot override official
  ones, so text_map.py needs to drop that pair (source step not traced). Suggested English when it is: "Save
  complete." / "Deletion complete." with "Press the ○ button.".
- 胴系統＋ in talk is still "Torso Plus" (flag n): Freedom only has Torso +1 / +2. Keep, or say "Torso +1/+2"?
- はらへり倍加×1.5 is "Hunger ×1.5": Freedom's form for it was not found.
- Talk lines write Freedom's abbreviated material names out ("Hornetaur Razorwing" for "Hornet Razorwing",
  "Silver Rathalos Claw"); that is the existing convention, kept.
- Official G text mixes "Friend List" and "Friends List"; our lines say Friend List.
- 達人の聖域 is just "Masters" (an 11-column room name); "Masters' Sanctum" would not fit.
- The ammo-combining elder lines (Flaming S, Thunder S, Recov S Lv2) use our own sentence form; the official ones
  beside them say "X and Y can be combined to create Z ammo for Bowguns". Could be made to match.
- `tools/text_own.py check` still lists 12 older list labels wider than their Japanese (Coma Rod, Flaming S,
  ThunderRes, the Desert Plesioth title, ...). Not changed; worth a look on screen.
