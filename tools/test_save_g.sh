#!/bin/sh
# Monster Hunter G: save round trip the way the owner plays it (PC G build, hidden; owner report 21 Oct 2026, Xbox:
# "bought armour and a weapon, equipped, saved at the bed, debug menu 'Title screen (soft reset)', nothing kept").
# A fresh hunter (RT_VILLAGE_START) with two blades in the box: in the house, box -> change equipment -> blade 2;
# the bed: save on a new card (slot 1, "create game data": yes); then the debug menu adds 2000z (NOT saved) and goes
# back to the title; CONTINUE (the cursor is on it once a save exists) -> slot 1 -> the hunter -> yes -> village.
# Pass: the card was written and read back, and the reloaded hunter wields blade 2 with 0z (the saved state, not the
# 2000z left in memory). Usage: tools/test_save_g.sh   Logs: build/D/g/save_g.log, build/D/g/save_g_logs/.  ~1 min.
cd "$(dirname "$0")/.."
export MH1_HIDDEN=1
[ -x build/mhg/pc/mhview_g ] || { echo "save G: no build/mhg/pc/mhview_g (tools/build_pc_mhg.sh)"; exit 1; }
[ -d disc/mhg ] || { echo "save G: no disc/mhg"; exit 1; }
rm -rf build/D/g/card build/D/g/save_g_logs; mkdir -p build/D/g/save_g_logs
export MH1_LOG_DIR="$PWD/build/D/g/save_g_logs"
S=$(python3 - <<'PY'
import subprocess
def evs(d, end):
    out, t = [], 0
    for k in sorted(d):
        if k > t: out.append("idle*%d" % (k - t)); t = k
        out.append(d[k]); t += int(d[k].split("*")[1])
    out.append("idle*%d" % (end - t)); return out
v1 = {45: 'square*2', 200: 'square*2',                                  # house door, box
      250: 'ddown*2', 265: 'ddown*2', 290: 'circle*2', 340: 'dright*2', 370: 'circle*2', 420: 'circle*2', 470: 'circle*2',
      510: 'cross*2', 540: 'cross*2', 570: 'cross*2',                     # out of the box
      650: 'square*2', 750: 'circle*2', 850: 'circle*2', 950: 'circle*2', 1050: 'circle*2', 1100: 'dleft*2', 1130: 'circle*2'}
dbg = subprocess.check_output(["python3", "tools/dbg_keys.py", "open d6 r r open w30 open d3 x w30"]).decode().strip()
cont = ("idle*80,circle*2,idle*600,start*2,idle*80,circle*2,idle*100,circle*2,idle*100,circle*2,idle*100,circle*2,"
        "idle*100,circle*2,idle*250,ddown*2,idle*20,circle*2,idle*100,circle*2,idle*100,circle*2,idle*600")
print(",".join(["@village1"] + evs(v1, 1400) + [dbg, cont, "@village2", "idle*300"]))
PY
)
RT_LB_WARP="30,11225,14400,0;150,1990,1160,4001;600,2259,745,4001" RT_WARE="6:1,6:2,2:1,2:5" RT_POUCH="129:5" TAILN=1 \
    sh tools/gvil.sh save_g "$S" 140 >/dev/null
L=build/D/g/save_g.log
G=$(ls build/D/g/save_g_logs/*.log 2>/dev/null | head -1)
fail() { echo "save G FAILED: $1 (see $L, $G)"; exit 1; }
[ -n "$G" ] || fail "no debug log"
grep -q "memory card: saved BISLPM-65869MHG/BISLPM-65869MHG" "$G" || fail "the bed save wrote nothing"
grep -q "back to the title" "$G" || fail "the debug menu's soft reset did not run"
grep -q "load finished" "$G" || fail "CONTINUE did not reach the village"
n=$(grep -ac "rt_village: enter" $L)
[ "$n" = 2 ] || fail "village entered $n times (want 2)"
last=$(grep -a "rt_village: tick [0-9]* ud" $L | tail -1)
echo "$last" | grep -q "wear: w6/2/" || fail "reloaded hunter does not wield blade 2: $last"
echo "$last" | grep -q "ud money 0 " || fail "reloaded money is not the saved 0z: $last"
grep -aq "EXIT 0" $L || fail "the run did not end cleanly"
# 2. a new start (power-on, the card auto-loaded, CONTINUE), as after the owner's Back + Start: the whole hunter must come
# back, also what G keeps past MH1's 0x480 bytes (pouch +0xC24, worn equipment +0xC75, play time +0xC1C): the save
# copied only 0x480 bytes per slot (udmisc_nm.c), so the owner's worn armour / weapon and play time were lost (21 Oct 2026)
C="idle*80,circle*2,idle*600,start*2,idle*80,circle*2,idle*100,circle*2,idle*100,circle*2,idle*100,circle*2,idle*100,circle*2,idle*250,ddown*2,idle*20,circle*2,idle*100,circle*2,idle*100,circle*2,idle*400"
L2=build/D/g/save_g2.log
( ulimit -v 3000000 2>/dev/null
  MH1_SAVE_DIR="$PWD/build/D/g/card" RT_NOMOVIE=1 RT_QUEST_TRACE=1 timeout 300 build/mhg/pc/mhview_g disc/mhg --boot --input "$C" \
      --shot build/D/g/save_g2.png --time 75 > $L2 2>&1 )
first=$(grep -a "rt_village: tick [0-9]* ud" $L2 | head -1)
[ -n "$first" ] || fail "new start: CONTINUE did not reach the village (see $L2)"
echo "$first" | grep -q "wear: w6/2/" || fail "new start: worn weapon not restored: $first"
echo "$first" | grep -q "pouch: 129:5 " || fail "new start: pouch not restored: $first"
t=$(echo "$first" | sed 's/.*| time \([0-9]*\).*/\1/')
[ "${t:-0}" -gt 0 ] 2>/dev/null || fail "new start: play time 0: $first"
echo "save G OK: blade 2 equipped -> bed save -> soft reset -> CONTINUE: blade 2 and the saved 0z came back; new start + CONTINUE: blade 2, pouch, play time $t"
