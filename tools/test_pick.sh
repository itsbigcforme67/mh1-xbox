#!/bin/sh
# The in-game bug reporter (F8, src/pc/pick.c), headless with test aids:
#  1. RT_PICK_AT=tick freezes the game at that tick, RT_PICK_CLICKS="x,y;x0,y0,x1,y1" picks a click and a box,
#     RT_PICK_NOTE the text: a report folder with screenshot / annotated / idbuffer / report.json / log_tail / input
#     must appear (objects of several kinds, game state, marks, the note, the build and seed);
#  2. the same through real SDL events (RT_PICK_UI=1: mouse down / up, text input, Enter pushed one per frozen frame);
#  3. replay: a second run fed with the first report's input.txt reaches the same game state at the same tick;
#  4. tools/show_report.py prints the report.
# BIN=... RUN=wine runs the Windows build. ~20 s.
cd "$(dirname "$0")/.."
BIN=${BIN:-build/pc/mhview}   # RUN=wine BIN=build/win/mhview.exe
OUT=build/show/pick; rm -rf $OUT; mkdir -p $OUT
export RT_NOMOVIE=1 RT_QUEST_STAGE=1 RT_PL_GOD=1
fail() { echo "pick test FAILED: $1 (see $OUT)"; exit 1; }
INP=$(python3 -c "print(','.join(['idle*40'] + ['triangle*2,idle*14'] * 40 + ['up*60']))")
env MH1_SAVE_DIR="$PWD/$OUT/r1/card" MH1_LOG_DIR="$PWD/$OUT/r1/logs" RT_PICK_AT=900 RT_PICK_CLICKS="320,240;100,300,500,450" RT_PICK_NOTE="scripted: the floor flickers" \
    $RUN $BIN disc/mh1 --quest 10 --play --input "$INP" --size 640x480 --shot $OUT/a.png --time 40 > $OUT/a.out 2>&1
R1=$(ls -d $OUT/r1/reports/report_* 2>/dev/null | head -1)
[ -n "$R1" ] || fail "no report folder"
for f in screenshot.png annotated.png idbuffer.png report.json log_tail.txt input.txt; do [ -s "$R1/$f" ] || fail "missing $f"; done
python3 - "$R1/report.json" <<'PY' || fail "report.json content"
import json, sys
d = json.load(open(sys.argv[1]))
assert d['note'] == 'scripted: the floor flickers', d['note']
assert len(d['marks']) == 2 and d['marks'][0]['type'] == 'click' and d['marks'][1]['type'] == 'box'
assert len(d['objects']) >= 2, len(d['objects'])
kinds = {o['kind'] for o in d['objects']}
assert kinds & {'stage', 'sky', 'monster', 'hunter', 'hud'}, kinds
for o in d['objects']:
    assert 'render' in o and 'blend' in o['render'] and 'world_position' in o and 'description' in o
g = d['game']
for k in ('tick', 'mode', 'stage', 'quest', 'camera', 'hunter', 'monsters_in_area', 'map_areas'):
    assert k in g, k
assert g['tick'] == 900 and g['quest'] == 10, g
assert d['build'] and 'random_seed' in d and d['replay']['ticks'] > 800
print('report ok: %d objects (%s)' % (len(d['objects']), ', '.join(sorted(kinds))))
PY
grep -q "bug report saved" $OUT/a.out || fail "no pointer line on stderr"
grep -q "bug report saved" $OUT/r1/logs/mh1_*.log || fail "no pointer line in the debug log"
# the real event path
env MH1_SAVE_DIR="$PWD/$OUT/r2/card" MH1_LOG_DIR="$PWD/$OUT/r2/logs" RT_PICK_UI=1 RT_PICK_AT=300 RT_PICK_CLICKS="320,240;100,300,500,450" RT_PICK_NOTE="ui path note" \
    $RUN $BIN disc/mh1 --quest 10 --play --size 640x480 --shot $OUT/b.png --time 20 > $OUT/b.out 2>&1
R2=$(ls -d $OUT/r2/reports/report_* 2>/dev/null | head -1)
[ -n "$R2" ] || fail "UI path: no report"
python3 -c "
import json,sys
d=json.load(open('$R2/report.json'))
assert d['note']=='ui path note', d['note']
assert len(d['marks'])==2 and d['objects'], d['marks']
" || fail "UI path report content"
# replay determinism
env MH1_SAVE_DIR="$PWD/$OUT/r3/card" MH1_LOG_DIR="$PWD/$OUT/r3/logs" RT_PICK_AT=900 RT_PICK_CLICKS="320,240" RT_PICK_NOTE=replay \
    RT_PICK_EXIT=1 RT_SEED=$(python3 -c "import json;print(json.load(open(\"$R1/report.json\"))[\"random_seed\"])") $RUN $BIN disc/mh1 --quest 10 --play --input "@$R1/input.txt" --size 640x480 > $OUT/c.out 2>&1
R3=$(ls -d $OUT/r3/reports/report_* 2>/dev/null | head -1)
[ -n "$R3" ] || fail "replay: no report"
python3 - "$R1/report.json" "$R3/report.json" <<'PY' || fail "replay differs from the recorded session"
import json, sys
a = json.load(open(sys.argv[1]))['game']; b = json.load(open(sys.argv[2]))['game']
assert a == b, (a['hunter'], b['hunter'])
PY
python3 tools/show_report.py "$R1" > $OUT/show.txt || fail "show_report.py"
grep -q "PICKED OBJECTS" $OUT/show.txt || fail "show_report output"
python3 tools/show_report.py "$R1" --replay | grep -q "RT_PICK_AT=900 RT_PICK_EXIT=1 " || fail "show_report --replay does not print the RT_PICK_AT command"
echo "pick OK: report with $(ls $R1 | wc -l) files, UI path, replay reaches the same state, show_report.py"
