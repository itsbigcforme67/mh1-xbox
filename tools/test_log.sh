#!/bin/sh
# The automatic debug log (src/pc/rt/rt_log.c): a normal run creates logs/mh1_*.log next to
# the save directory with the header and a clean "session end"; old logs are rotated (20 kept);
# RT_CRASH_TEST=1 (a null write after 60 frames) leaves a CRASH section with the reason,
# a backtrace with symbol names, the last log lines and the game state; no home directory or
# user name appears in a log. Headless, ~10 s. BIN=... runs another build (e.g. build/win under wine).
cd "$(dirname "$0")/.."
export MH1_HIDDEN=1   # no window on the owner's screen, no real controller
BIN=${BIN:-build/pc/mhview}   # RUN=wine for the Windows exe
OUT=build/show/log; rm -rf $OUT; mkdir -p $OUT/logs
export MH1_SAVE_DIR="$PWD/$OUT/card" MH1_LOG_DIR="$PWD/$OUT/logs" RT_NOMOVIE=1
fail() { echo "log test FAILED: $1 (see $OUT)"; exit 1; }
i=0; while [ $i -lt 24 ]; do : > "$OUT/logs/mh1_20200101_10$(printf %02d $i)00.log"; i=$((i + 1)); done
$RUN $BIN disc/mh1 --boot --frames 80 --time 20 --shot $OUT/a.png > $OUT/a.out 2>&1
L=$(ls -t $OUT/logs/mh1_2026*.log 2>/dev/null | head -1)
[ -n "$L" ] || fail "no log file was created"
grep -q "^ *[0-9.]* t[0-9]* *I build: " $L || fail "no build line in the header"
grep -q "platform: " $L || fail "no platform line"
grep -q "GPU: " $L || fail "no GPU line"
grep -q "session end: clean exit" $L || fail "no clean session end"
grep -q "stand-in called" $L || fail "no stand-in line"
grep -q "controller: " $L && fail "a scripted/hidden run looked at real controllers (pad_set_headless)"
n=$(ls $OUT/logs/mh1_*.log | wc -l)
[ "$n" -le 20 ] || fail "rotation: $n logs kept"
RT_CRASH_TEST=1 $RUN $BIN disc/mh1 --boot --frames 200 --time 20 --shot $OUT/b.png > $OUT/b.out 2>&1
C=$(ls -t $OUT/logs/mh1_2026*.log | head -1)
[ "$C" != "$L" ] || fail "the crash run made no new log"
grep -q "===== CRASH =====" $C || fail "no crash section"
grep -q "^reason: " $C || fail "no crash reason"
grep -q "^game: mode " $C || fail "no game state in the crash section"
grep -q "  #0 .* rt_log_frame" $C || grep -q "  #[0-9] " $C || fail "no backtrace"
grep -q "last .* log lines" $C || fail "no ring buffer dump"
if grep -qE "/home/|/Users/|[A-Z]:.Users" $OUT/logs/mh1_2026*.log; then fail "a home directory path in a log"; fi
# --help prints the usage and opens nothing; a hidden run without game data exits with a message instead of the install
# dialog (18 Oct 2026: an agent's `mhview --help` opened that dialog on the owner's screen). No display here, so a
# regression fails on SDL instead of showing a window; the message checks below catch it.
H=$(DISPLAY= WAYLAND_DISPLAY= timeout 20 $RUN $BIN --help 2>&1); rc=$?
[ $rc = 0 ] || fail "--help exit code $rc"
echo "$H" | grep -q "usage: " || fail "--help printed no usage"
mkdir -p $OUT/home
N=$(env DISPLAY= WAYLAND_DISPLAY= HOME="$PWD/$OUT/home" XDG_DATA_HOME="$PWD/$OUT/home" timeout 20 $RUN $BIN 2>&1); rc=$?
[ $rc = 1 ] || fail "no-disc hidden run: exit code $rc (want 1)"
echo "$N" | grep -q "no install dialog" || fail "no-disc hidden run: no 'no install dialog' message"
echo "log OK: $(basename $L), crash section in $(basename $C), $n logs after rotation"
