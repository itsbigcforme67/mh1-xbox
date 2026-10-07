#!/bin/sh
# Scripted check of the opening movie on the PC build: power-on, the logos,
# then OPENING.sfd plays (libmpeg2 + ADX from AFS00.AFS). Headless (the
# game tick is the clock, no audio device). Passes when the movie opened,
# frames reached the screen (RT_MOVIE_DUMP writes every 100th frame as PPM
# in build/show/movie/; the test checks they are not blank) and the decode
# time was logged. Look at the PPMs too. About 1-2 minutes.
cd "$(dirname "$0")/.."
OUT=build/show/movie; rm -rf $OUT; mkdir -p $OUT
RT_MOVIE_TRACE=1 RT_MOVIE_DUMP=$OUT timeout 300 build/pc/mhview disc/mh1 --boot --shot $OUT/screen.png --time ${MOVIE_TIME:-75} \
    2> $OUT/run.log >/dev/null
grep -q "movie: 0 = OPENING.sfd" $OUT/run.log || { echo "movie FAILED: OPENING.sfd was not opened (see $OUT/run.log)"; exit 1; }
N=$(ls $OUT/m0_*.ppm 2>/dev/null | wc -l)
[ "$N" -ge 3 ] || { echo "movie FAILED: only $N frames dumped"; exit 1; }
python3 - $OUT <<'PY' || exit 1
import glob, sys
bad = 0
for f in sorted(glob.glob(sys.argv[1] + "/m0_*.ppm"))[2:]:
    d = open(f, "rb").read()
    body = d[d.index(b"255\n") + 4:]
    if max(body[::97]) - min(body[::97]) < 40:
        bad += 1
        print("blank frame:", f)
sys.exit(1 if bad > 1 else 0)
PY
echo "movie OK: $N frames dumped, decode time in $OUT/run.log"
