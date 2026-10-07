#!/bin/sh
# Audio check, headless (--audio-dump: the mixer's output per game tick into a
# wav, nobody listens). Three segments, each cut into 1 s windows; it FAILS
# when a window that should have music/sound is near-silent (RMS < 150 of
# 32767) or when the BGM stream stops partway (a quiet stretch of 2 s or more
# after sound had started):
#   title    power-on -> logos -> title screen BGM (--boot, ticks 640-1200)
#   village  NEW GAME -> village BGM while the hunter walks about (--boot)
#   fight    quest 10 on the Rathian's stage started WITHOUT --boot (the quest
#            BGM and the hunter's/monster's sounds)
# Bugs it was made for (agent A, 7 Oct 2026): the village BGM ran dry after a
# second; quests started without --boot were silent after the options-sound
# merge. About 2 minutes.
cd "$(dirname "$0")/.."
BIN=${BIN:-build/pc/mhview}   # RUN=wine BIN=build/win/mhview.exe: the Windows build under Wine
export RT_NOMOVIE=1
OUT=build/show/audio; mkdir -p $OUT; rm -f $OUT/*.wav
D=tools/pc_scripts
# title: logos until ~tick 600, then the title
RT_NAME=TEST $RUN $BIN disc/mh1 --boot --input "idle*1300" --size 640x480 \
    --audio-dump $OUT/title.wav --shot $OUT/title.png --time 44 2> $OUT/title.log >/dev/null
# village: the new-game script ends in the hunter's house; RT_LB_WARP puts him
# out at the weapon workshop (village tick 1300 = host tick ~2780), then he
# walks about for ~50 s
WALK=$(python3 tools/mk_input.py $D/newgame.txt "2960:up*100;3100:left*100;3250:down*150;3450:right*150;3650:up*100" 4400)
RT_NAME=TEST RT_LB_WARP="1300,2290,1000,4000;1360,9700,12120,4000" $RUN $BIN disc/mh1 --boot --input "$WALK" --size 640x480 \
    --audio-dump $OUT/village.wav --shot $OUT/village.png --time 146 2> $OUT/village.log >/dev/null
# fight: the Rathian on her stage, hunter attacking (no --boot)
ATK=$(python3 -c "print(','.join(['idle*40'] + ['triangle*2,idle*14'] * 60))")
RT_QUEST_STAGE=1 RT_PL_WARP_EM=30-1000 RT_PL_GOD=1 RT_PL_TARGET=0:0 \
    $RUN $BIN disc/mh1 --quest 10 --play --input "$ATK" --size 640x480 \
    --audio-dump $OUT/fight.wav --shot $OUT/fight.png --time 36 2> $OUT/fight.log >/dev/null
python3 - "$OUT" <<'PY'
import sys, wave, struct, array, math
out = sys.argv[1]
THR = 150.0
# name, first second, last second (None = to the end) the sound must be there
spec = [("title", 21, None), ("village", 96, None), ("fight", 3, None)]
bad = 0
for name, t0, t1 in spec:
    try:
        w = wave.open("%s/%s.wav" % (out, name))
    except Exception as e:
        print("audio FAILED: %s.wav missing (%s)" % (name, e)); bad = 1; continue
    rate, ch = w.getframerate(), w.getnchannels()
    a = array.array("h"); a.frombytes(w.readframes(w.getnframes()))
    secs = len(a) // (rate * ch)
    rms = []
    for s in range(secs):
        seg = a[s * rate * ch:(s + 1) * rate * ch]
        rms.append(math.sqrt(sum(x * x for x in seg) / max(1, len(seg))))
    end = secs if t1 is None else min(t1, secs)
    win = rms[t0:end]
    line = " ".join("%d" % r for r in rms[max(0, t0 - 2):end])
    print("%-8s %2ds, rms per second from %ds: %s" % (name, secs, max(0, t0 - 2), line))
    if len(win) < 5:
        print("audio FAILED: %s: only %d s dumped" % (name, len(win))); bad = 1; continue
    quiet = [i + t0 for i, r in enumerate(win) if r < THR]
    if quiet:
        print("audio FAILED: %s: near-silent at second(s) %s (RMS < %d)" % (name, quiet[:10], THR)); bad = 1
if bad:
    sys.exit(1)
print("audio OK: title, village and fight all have sound in every second checked")
PY
