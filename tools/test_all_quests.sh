#!/bin/sh
# Plays every offline (village) quest of the Elder's star levels 1-5 (131-171) on the PC build, headless, from `--quest N`
# (no village): clear (game_w+0xD5 = 3), reward list, money counted, village entered, no crash.
#   delivery quests: the items are put in the pouch with RT_PL_ITEMS and delivered at the camp box (spot kind 21, circle);
#   hunt quests: RT_PL_GOTO walks the area exits (several stages, or "f" = follow the boss), RT_PL_TARGET=kN, RT_PL_WARP_EM,
#   RT_DMG_MUL, RT_PL_GOD fight and carve; a boss that stays out of reach gets RT_PL_SLAY (a lethal hit);
#   the pad pattern also takes the reward items and picks "end receiving".
# Not automated (SKIP) and known failures (KNOWN) are listed by tools/test_all_quests.py and in docs/pc.md.
# usage: tools/test_all_quests.sh [quest numbers, decimal]   (~3 minutes for all). Logs: build/show/allq/qNNN.log
cd "$(dirname "$0")/.."
[ -x build/pc/mhview ] || tools/build_pc.sh >/dev/null || exit 1
exec python3 tools/test_all_quests.py "$@"
