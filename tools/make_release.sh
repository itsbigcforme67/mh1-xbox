#!/bin/sh
# make_release.sh - the testers' kit: from your own disc image(s) to a folder you copy to your Xbox, plus the PC build.
#
#   tools/make_release.sh G_DISC [--mh1 MH1_DISC] [--out DIR] [--online] [--windows] [--no-pc] [--setup]
#                                [--english US_DISC [--freedom FREEDOM_ISO --portable PORTABLE_ISO]]
#
#   G_DISC      your own image of the Japanese Monster Hunter G (SLPM-65869): the .iso, or a folder with its files
#   --mh1 DISC  also Monster Hunter 1 (Japanese, SLPM-65495)
#   --out DIR   where the kit goes (default: dist/release)
#   --online    also the online XBE(s) (for the online lobby test; needs your own server, docs/TESTERS.md)
#   --windows   also the Windows PC build (downloads the cross-compiler with --setup, ~100 MB)
#   --no-pc     skip the Linux PC build
#   --xemu-test (developers) after building, a hardware smoke test of each kit Xbox folder in xemu: installed as the
#               testers install it, started twice through a stand-in dashboard, a bed save, then CONTINUE on the second
#               start (tools/xemu_relaunch_test.sh --kit DIR --save; ~12 min a game, hidden; needs the owner's xemu files
#               and waits for the shared emulator lock). The kit itself is unchanged by it.
#   --setup     fetch missing build parts that can be fetched without root (32-bit compiler files, Windows tools)
#   --english US_DISC [--freedom FREEDOM_ISO --portable PORTABLE_ISO]
#               make the English text table from your own Monster Hunter (US, SLUS-20896) disc and, for most of G's
#               text, your own Monster Hunter Freedom (PSP, ULUS-10084) and Monster Hunter Portable (PSP JP,
#               ULJM-05066) images (Freedom is matched through Portable). Needs --mh1. Without --english the game is
#               in Japanese (docs/english_g.md: the US disc alone gives about half of G's text).
#
# Result (DIR = dist/release):
#   DIR/xbox/MHG/            default.xbe + data/  -> copy the whole folder to the Xbox, e.g. E:\Games\MHG
#   DIR/xbox/MH1/            (with --mh1) the same for Monster Hunter 1 -> E:\Games\MH1
#   DIR/xbox/MHG-online/     (with --online) default.xbe of the online build (uses the data of E:\Games\MHG)
#   DIR/linux/MHG/           the PC build: ./play.sh       DIR/linux/MH1/ (with --mh1)
#   DIR/windows/MHG/         (with --windows) play.bat
#   DIR/TESTERS.md           what to test and how to report; DIR/VERSION.txt which build this is
# Nothing of the game is in this repository: everything under DIR/*/data comes from your disc at build time.
# DIR holds your copy of the game files: do not share it. The PC folders share the Xbox folders' data files
# (hard links) when they are on the same disk, so the kit needs about 1.2 GB per game, not 2.4.
#
# Dependencies (docs/TESTERS.md, "What you need"): Linux (tested on Ubuntu 24.04 only), python3 >= 3.8 (standard
# library only), git, gcc + make + binutils, SDL2 headers, 32-bit gcc support and 32-bit SDL2/GL runtime libraries,
# and the Xbox SDK nxdk built (clang, lld, llvm, bison, flex, cmake). ./mh1 checks each and says what to install.
set -e
cd "$(dirname "$0")/.."
TOP=$PWD
die() { echo "make_release: $*" >&2; exit 1; }
say() { echo "== $*"; }

GDISC=""; M1DISC=""; OUT=dist/release; ONLINE=""; WIN=""; PC=1; SETUP=""; XTEST=""; USDISC=""; FRDISC=""; PODISC=""
while [ $# -gt 0 ]; do
    case "$1" in
        --mh1) [ $# -ge 2 ] || die "--mh1 needs a disc image"; M1DISC=$2; shift ;;
        --out) [ $# -ge 2 ] || die "--out needs a folder"; OUT=$2; shift ;;
        --english) [ $# -ge 2 ] || die "--english needs the US disc image"; USDISC=$2; shift ;;
        --freedom) [ $# -ge 2 ] || die "--freedom needs the Freedom image"; FRDISC=$2; shift ;;
        --portable) [ $# -ge 2 ] || die "--portable needs the Portable image"; PODISC=$2; shift ;;
        --online) ONLINE=1 ;;
        --windows) WIN=1 ;;
        --no-pc) PC="" ;;
        --xemu-test) XTEST=1 ;;
        --setup) SETUP=--setup ;;
        -h|--help) sed -n '2,37p' "$0" | sed 's/^# \{0,1\}//'; exit 0 ;;
        -*) die "unknown option $1 (tools/make_release.sh --help)" ;;
        *) [ -z "$GDISC" ] || die "one G disc only (MH1 goes with --mh1)"; GDISC=$1 ;;
    esac
    shift
done
[ -n "$GDISC" ] || die "give your Monster Hunter G disc image: tools/make_release.sh /path/to/MonsterHunterG.iso (--help)"
abspath() { case "$1" in /*) echo "$1" ;; *) echo "$TOP/$1" ;; esac; }
GDISC=$(abspath "$GDISC"); OUT=$(abspath "$OUT")
[ -z "$M1DISC" ] || M1DISC=$(abspath "$M1DISC")
[ -z "$USDISC" ] || USDISC=$(abspath "$USDISC")
[ -z "$FRDISC" ] || FRDISC=$(abspath "$FRDISC")
[ -z "$PODISC" ] || PODISC=$(abspath "$PODISC")
[ -z "$FRDISC" ] || [ -n "$PODISC" ] || die "--freedom also needs --portable (Monster Hunter Portable, the Japanese PSP G): Freedom's text is matched through it"
[ -z "$USDISC" ] || [ -n "$M1DISC" ] || die "--english also needs the Japanese Monster Hunter 1 disc (--mh1): the English is matched through it"

# which disc is which (also catches swapped arguments before anything is built)
command -v python3 >/dev/null || die "python3 is needed (3.8 or newer): sudo apt install python3"
G=$(python3 tools/disc_files.py game "$GDISC") || die "cannot read '$GDISC' as a Monster Hunter disc (see above)"
[ "$G" = mhg ] || die "'$GDISC' is the Monster Hunter 1 disc, not G: give G first and MH1 with --mh1"
if [ -n "$M1DISC" ]; then
    M=$(python3 tools/disc_files.py game "$M1DISC") || die "cannot read '$M1DISC' as a Monster Hunter disc (see above)"
    [ "$M" = mh1 ] || die "'$M1DISC' (--mh1) is not the Japanese Monster Hunter 1 disc"
fi
mkdir -p "$OUT" build
LOG=$OUT/make_release.log
: > "$LOG"
say "kit folder: $OUT (log of every step: $LOG)"

run_mh1() {   # run_mh1 TARGET DISC OUTDIR [options]: ./mh1 with its output in the log, and its last lines on failure
    t=$1; d=$2; o=$3; shift 3
    say "./mh1 $t ($(basename "$d")) -> $o"
    if ! sh ./mh1 "$t" "$d" --out "$o" $SETUP "$@" >> "$LOG" 2>&1; then
        tail -15 "$LOG"
        die "./mh1 $t failed (full log: $LOG)"
    fi
}

# a PC folder's data/: hard links to the Xbox folder's copy when possible (./mh1 then finds them complete)
link_data() {   # link_data FROM_DATA TO_DATA
    mkdir -p "$2"
    for f in "$1"/*; do
        [ -f "$f" ] || continue
        [ -e "$2/$(basename "$f")" ] || ln "$f" "$2/" 2>/dev/null || true
    done
}

# ---- Monster Hunter G
run_mh1 xbox "$GDISC" "$OUT/xbox"                       # -> OUT/xbox/MHG
if [ -n "$PC" ]; then
    link_data "$OUT/xbox/MHG/data" "$OUT/linux/MHG/data"
    run_mh1 linux "$GDISC" "$OUT/linux/MHG"
fi
if [ -n "$WIN" ]; then
    link_data "$OUT/xbox/MHG/data" "$OUT/windows/MHG/data"
    run_mh1 windows "$GDISC" "$OUT/windows/MHG"
fi
if [ -n "$ONLINE" ]; then
    # (./mh1 has no --online for G yet: the same steps as tools/full_check.sh)
    say "the online G build (PC objects, then the Xbox game)"
    { ONLINE=1 sh tools/build_pc_mhg.sh && sh -c '[ -f "${MH1_XBOX_ENV:-$HOME/xboxdev/env.sh}" ] && . "${MH1_XBOX_ENV:-$HOME/xboxdev/env.sh}"; python3 tools/build_xbox.py --version mhg --online --gfx nv2a'; } >> "$LOG" 2>&1 \
        || { tail -15 "$LOG"; die "the online G build failed (log: $LOG)"; }
    mkdir -p "$OUT/xbox/MHG-online"
    cp build/mhg/xbox/online/nv2a/default.xbe "$OUT/xbox/MHG-online/default.xbe"
fi

# ---- Monster Hunter 1 (optional)
if [ -n "$M1DISC" ]; then
    run_mh1 xbox "$M1DISC" "$OUT/xbox" ${ONLINE:+--online}  # -> OUT/xbox/MH1 (+ MH1-online)
    if [ -n "$PC" ]; then
        link_data "$OUT/xbox/MH1/data" "$OUT/linux/MH1/data"
        run_mh1 linux "$M1DISC" "$OUT/linux/MH1"
    fi
    if [ -n "$WIN" ]; then
        link_data "$OUT/xbox/MH1/data" "$OUT/windows/MH1/data"
        run_mh1 windows "$M1DISC" "$OUT/windows/MH1"
    fi
fi

# ---- English text (optional): built from the player's own discs, never shipped
if [ -n "$USDISC" ]; then
    say "the English text table (tools/text_map.py; the first run takes a while)"
    T=build/release_text
    mkdir -p $T/us
    # text_map.py reads folders: the US disc's two files it needs, the JP and G folders the kit already holds
    python3 - "$USDISC" $T/us <<'EOP' >> "$LOG" 2>&1 || { tail -15 "$LOG"; die "cannot read the US disc '$USDISC'"; }
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
    python3 tools/text_map.py --jp "$OUT/xbox/MH1/data" --us $T/us --g "$OUT/xbox/MHG/data" -o $T/out \
        --mhp "${PODISC:-$TOP/$T/none}" ${FRDISC:+--freedom "$FRDISC"} >> "$LOG" 2>&1 || { tail -15 "$LOG"; die "tools/text_map.py failed (log: $LOG)"; }
    for g in MHG MH1; do
        src=$T/out/en_$(echo $g | tr 'A-Z' 'a-z').txt
        [ -f "$src" ] || continue
        for d in "$OUT/xbox/$g" "$OUT/linux/$g" "$OUT/windows/$g"; do
            [ -d "$d" ] || continue
            mkdir -p "$d/text"; cp "$src" "$d/text/en.txt"
        done
    done
    [ -d "$OUT/xbox/MHG-online" ] && mkdir -p "$OUT/xbox/MHG-online/text" && cp $T/out/en_mhg.txt "$OUT/xbox/MHG-online/text/en.txt"
fi

# ---- the papers
cp docs/TESTERS.md "$OUT/TESTERS.md"
{
    echo "Monster Hunter PC / Xbox port, testers' kit"
    echo "build: $(git rev-parse --short=10 HEAD 2>/dev/null || echo unknown)$(git diff --quiet HEAD 2>/dev/null || echo ' (with local changes)')"
    echo "made:  $(date '+%Y-%m-%d %H:%M')"
    echo "games: G${M1DISC:+, MH1}${ONLINE:+ (+ online XBE)}${USDISC:+ (+ English text)}"
} > "$OUT/VERSION.txt"
for x in "$OUT"/xbox/*/; do cp "$OUT/VERSION.txt" "$x/VERSION.txt"; done

if [ -n "$XTEST" ]; then
    LOCK=$(cd "$(git rev-parse --git-common-dir)/../.." && pwd)/xemu.lock     # one emulator at a time (docs/agents/BRIEF.md)
    for x in "$OUT"/xbox/MHG "$OUT"/xbox/MH1; do
        [ -f "$x/default.xbe" ] || continue
        say "xemu smoke test of $(basename "$x") (two starts, a save, CONTINUE; log: $OUT/xemu_$(basename "$x").log)"
        if flock "$LOCK" tools/xemu_relaunch_test.sh --kit "$x" --save > "$OUT/xemu_$(basename "$x").log" 2>&1; then
            grep -E '^PASS|^   ' "$OUT/xemu_$(basename "$x").log" | sed 's/^/   /'
        else
            grep -E '^(PASS|FAIL)|^   ' "$OUT/xemu_$(basename "$x").log" | sed 's/^/   /'
            die "the xemu smoke test of $(basename "$x") failed (log: $OUT/xemu_$(basename "$x").log)"
        fi
    done
fi

say "done. What is where:"
for x in "$OUT"/xbox/*/ "$OUT"/linux/*/ "$OUT"/windows/*/; do
    [ -d "$x" ] || continue
    echo "   $(echo "$x" | sed "s|^$OUT/||; s|/$||")   $(du -sh "$x" 2>/dev/null | cut -f1)"
done
echo "   The Xbox folders: copy each one whole to your Xbox (E:\\Games\\MHG and E:\\Games\\MH1), see TESTERS.md."
[ ! -d "$OUT/linux/MHG" ] || echo "   Linux: $OUT/linux/MHG/play.sh     (do not share this folder: it holds your copy of the game files)"
