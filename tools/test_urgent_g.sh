#!/bin/sh
# Monster Hunter G: the first quest the way a player gets it (PC G build, hidden). A fresh hunter in the village
# (RT_VILLAGE_START, first-visit event seen), the Village Elder: his story talk, then the level list where a new save
# can only pick the urgent quest (key quest 10001), accepted; the village entrance, square: depart; the quest (deliver
# 2 x item 37: RT_PL_ITEMS + a warp to the camp's delivery box), clear, reward screen, back to the village, which must
# then show star level 0 (was -1: "rt_village: level"). Covers the board -> quest start path the --quest tests skip
# (an Xbox-only crash there, 21 Oct 2026: tools/xemu runs of the same script; G's lb_set_questpage_info).
# Usage: tools/test_urgent_g.sh   Logs and pictures: build/D/g/urg_g*.  ~1 min.
cd "$(dirname "$0")/.."
export MH1_HIDDEN=1
[ -x build/mhg/pc/mhview_g ] || { echo "urgent G: no build/mhg/pc/mhview_g (tools/build_pc_mhg.sh)"; exit 1; }
[ -d disc/mhg ] || { echo "urgent G: no disc/mhg"; exit 1; }
rm -rf build/D/g/card
EV=$(python3 - <<'PY'
e = ['%d:circle*2' % (45 + 30 * i) for i in range(28)] + ['960:square*2']
cyc = []
t = 1300
while t < 3900:                     # delivery box, reward screen (take items, end receiving), back to the village
    for d, k in ((0, 'circle*2'), (30, 'circle*2'), (60, 'cross*2'), (75, 'ddown*2'), (90, 'circle*2')):
        cyc.append('%d:%s' % (t + d, k))
    t += 120
print(','.join(e + cyc))
PY
)
RT_LB_WARP="30,11100,12150,0;920,10650,15225,0" RT_PL_ITEMS=37:2 RT_PL_WARP="100,10350,10640,7000" RT_PL_GOD=1 \
    TAILN=1 python3 tools/gvil_ev.py urg_g 4000 "$EV" RT_MONEY=500 >/dev/null
L=build/D/g/urg_g.log
fail() { echo "urgent G FAILED: $1 (see $L)"; exit 1; }
grep -aq "quest 10001 (accepted 1)" $L || fail "the Elder's urgent quest 10001 was not accepted"
grep -aq "village: quest 10001 starts on stage" $L || fail "no departure for 10001"
grep -aq "D5 3" $L || fail "10001 not cleared"
grep -a "rt_village: level" $L | tail -1 | grep -q "level 0," || fail "star level not raised to 0 after the clear"
grep -aq "EXIT 0" $L || fail "the run did not end cleanly"
echo "urgent G OK: Elder -> urgent 10001 accepted -> departed -> cleared -> back in the village at star level 0"
