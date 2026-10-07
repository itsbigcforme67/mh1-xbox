#!/bin/sh
# Make a bug report: zips the newest two debug logs and the save's metadata (file names, sizes, dates:
# NOT the save itself unless you pass --with-save) into mh1_bug_report_<date>.zip in the current
# folder, and prints where to send it. Nothing is uploaded. Logs hold no user name or home path.
#   tools/bug_report.sh [--with-save]
if [ -n "$MH1_SAVE_DIR" ]; then BASE=$(dirname "$MH1_SAVE_DIR"); CARD=$MH1_SAVE_DIR
else BASE=${XDG_DATA_HOME:-$HOME/.local/share}/mh1pc; CARD=$BASE/memcard0; fi
LOGS=${MH1_LOG_DIR:-$BASE/logs}
WITH=0; [ "$1" = "--with-save" ] && WITH=1
STAMP=$(date +%Y%m%d_%H%M%S)
TMP=$(mktemp -d) || exit 1
mkdir -p "$TMP/mh1_bug_report"
N=0
for f in $(ls -1 "$LOGS"/mh1_*.log 2>/dev/null | sort | tail -2); do cp "$f" "$TMP/mh1_bug_report/"; N=$((N + 1)); done
if [ $N -eq 0 ]; then echo "No debug log found in $LOGS - run the game once first."; rm -rf "$TMP"; exit 1; fi
{
    echo "Save folder listing (names, sizes, modified times; no contents)"
    if [ -d "$CARD" ]; then (cd "$CARD" && find . -mindepth 1 -printf '%P  %s bytes  %TY-%Tm-%TdT%TH:%TM:%TS\n' | sort); else echo "(no save folder)"; fi
} > "$TMP/mh1_bug_report/save_info.txt"
[ $WITH = 1 ] && [ -d "$CARD" ] && cp -r "$CARD" "$TMP/mh1_bug_report/save"
ZIP="$PWD/mh1_bug_report_$STAMP.zip"
if command -v zip >/dev/null 2>&1; then (cd "$TMP" && zip -qr "$ZIP" mh1_bug_report)
else python3 - "$TMP" "$ZIP" <<'PY'
import os, sys, zipfile
root, out = sys.argv[1], sys.argv[2]
with zipfile.ZipFile(out, 'w', zipfile.ZIP_DEFLATED) as z:
    for d, _, fs in os.walk(root):
        for f in fs:
            p = os.path.join(d, f)
            z.write(p, os.path.relpath(p, root))
PY
fi
rm -rf "$TMP"
echo
echo "Made $ZIP"
echo "It holds the newest $N log(s) and save_info.txt$([ $WITH = 1 ] && echo ', and your save' || echo ' (your save is NOT included; --with-save adds it)')."
echo "The logs hold no user name or home path; they are plain text, check them if you like."
echo
echo "Open a bug report and attach the zip:"
echo "  https://github.com/itsbigcforme67/mh1-xbox/issues/new?template=bug_report.md"
