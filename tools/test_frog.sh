#!/bin/sh
# Frog bait fishing on the PC build (quest 165, stage 54, the Plesioth's lake): the hunter
# casts item 125 (frog bait) from the shore, the idle Plesioth takes it (em21 acts 2/17,
# Kaeru_ck finds the frog float), the hunter reels (circle, player act 0/83), the Plesioth is
# pulled out (2/18) and lands (4/15). RT_EM_BLIND=1 keeps the Plesioth idle: without it the
# Plesioth sees the hunter when it turns toward him at the end of its first idle script cycle
# (tick ~352) and goes into combat, where the fishing check (em_cmd_pl_fishing_ck) never runs
# (docs/pc.md "Frog bait fishing"). Headless, ~1.5 min. The bite tick (1179 here) is
# deterministic for this script.
cd "$(dirname "$0")/.."
export RT_NOMOVIE=1
OUT=build/show/frog; mkdir -p $OUT
S=$(python3 tools/mk_input.py - "14:square*2;1200:circle*2" 2500)
RT_EM_BLIND=1 RT_PL_TRACE=1 RT_EM_TRACE=1 RT_PL_ITEMS="125:5" RT_PL_WARP="10,11200,10850,C667" \
    timeout 400 build/pc/mhview disc/mh1 --quest 165 --stage 54 --play --input "$S" --shot $OUT/end.png --time 85 \
    > $OUT/run.log 2>&1
for p in "^pl: act 0/80 step 1" "^em0: .* act 2/17/" "^pl: act 0/83 " "^em0: .* act 2/18/" "^em0: .* act 4/15/"; do
    grep -q "$p" $OUT/run.log || { echo "frog FAILED: no '$p' in $OUT/run.log"; exit 1; }
done
echo "frog OK: cast, bite (2/17), reel (0/83), Plesioth pulled out (2/18) and landed (4/15)"
