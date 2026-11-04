#!/bin/sh
# make_english.sh - the English text table(s) from the player's own discs (tools/text_map.py), for ./mh1 --english and
# tools/make_release.sh --english.
#
#   tools/make_english.sh US_DISC OUTDIR --jp JP_FOLDER [--g G_FOLDER [--mhp PORTABLE] [--freedom FREEDOM]]
#
#   US_DISC   your own US Monster Hunter (SLUS-20896): the .iso or a folder with its files. Its two files text_map.py
#             reads (SLUS_208.96, AFS_DATA.AFS) are copied to build/release_text/us first (kept for the next run).
#   OUTDIR    where en_mh1.txt (and with a G folder en_mhg.txt) go, plus text_map.py's caches
#   the rest  goes to tools/text_map.py as it is (folders, not .iso: the JP / G data folders ./mh1 made). Without a G
#             folder holding SLPM_658.69 only the MH1 table is made (JP + US discs are enough).
# The tables hold Capcom's text from your discs: they stay on your machine (build/ and dist/ are not tracked by git).
set -e
cd "$(dirname "$0")/.."
die() { echo "make_english: $*" >&2; exit 1; }
[ $# -ge 2 ] || die "usage: tools/make_english.sh US_DISC OUTDIR --jp JP_FOLDER [text_map.py options]"
USDISC=$1; OUTDIR=$2; shift 2
T=build/release_text/us
mkdir -p "$T"
python3 - "$USDISC" "$T" <<'EOP' || die "cannot read the US disc '$USDISC'"
import os, sys
sys.path.insert(0, "tools")
import disc_files as d
s, iso = d.open_source(sys.argv[1])
for name in ("SLUS_208.96", "AFS_DATA.AFS"):
    if name not in s.files:
        sys.exit("%s is not on %s: is it the US Monster Hunter (SLUS-20896)?" % (name, sys.argv[1]))
    out = os.path.join(sys.argv[2], name)
    if os.path.isfile(out) and os.path.getsize(out) == s.files[name][0]:
        continue
    with open(out + ".part", "wb") as f:
        for b in d.stream(s, iso, name):
            f.write(b)
    os.replace(out + ".part", out)
EOP
python3 tools/text_map.py --us "$T" -o "$OUTDIR" "$@" || die "tools/text_map.py failed"
