#!/bin/sh
# Run the owner's MH1 disc (or a test executable with it) in PCSX2, hidden: PCSX2 gets its own Xvfb display, no
# window shows on the owner's screen, no sound device is opened. Everything PCSX2 writes (settings, memory cards,
# logs) is under $PCSX2_HOME (default ~/ps2dev/pcsx2/data), outside the repo.
#
# Needs (never in the repo, never committed):
#   ~/ps2dev/pcsx2/pcsx2-v2.8.2.AppImage   the official release (github.com/PCSX2/pcsx2/releases), or PCSX2=path
#   $PCSX2_HOME/PCSX2/bios/<the owner's BIOS dump>.bin   (4 MB; any one .bin there, or BIOS=file name)
#   the owner's disc image: disc/mh1/Monster Hunter (Japan).iso, or --iso
#
# usage: tools/run_pcsx2.sh [--iso FILE] [--elf FILE] [--card CARD.ps2 | --card-from PCROOT] [--seconds N]
#                           [--shot out.png] [--after "T1 T2 ..."] [--log FILE] [--show]
#   --elf       boot this executable instead of the disc's SLPM_654.95 (the disc stays in the drive: the game
#               reads its data from it), e.g. build/ps2patch/pdtest/install/SLPM_654.95 (tools/ps2_pdtest.py)
#   --card      memory card image for slot 1 (copied to $PCSX2_HOME/PCSX2/memcards/Mcd001.ps2, which is what PCSX2 uses
#               and keeps; pass nothing to keep the card of the last run)
#   --card-from a PC port card folder (holding BISLPM-65495MH, e.g. ~/.local/share/mh1pc/memcard0): a fresh card
#               image holding that save (the PC port writes the PS2's files byte for byte; tools/pcsx2/savecard.c)
#   --seconds   stop PCSX2 after N seconds (default 120)
#   --shot      screenshots of the hidden display at the --after times (default: at the end)
#   --log       where the log goes (default $PCSX2_HOME/PCSX2/logs/emulog.txt; EE and IOP console output included)
#   --show      run on the desktop instead (debugging by hand only)
#   --cheats F  a pnach cheat file (RAM writes, e.g. a test's warp aid) for MH1 (SLPM-65495, CRC C2008A92, or the
#               --elf's CRC), its [sections] enabled; without it no cheats run
#   --net       DEV9 network adaptor on, in PCSX2's "Sockets" mode (no root, no TAP/PCAP): the guest gets an address from
#               PCSX2's own DHCP and reaches the host's LAN address (not 127.0.0.1) through ordinary host sockets
# Input: pad 1 is the keyboard (arrows = d-pad, x = cross, c = circle, z = square, s = triangle, Return = start,
# BackSpace = select, q/e = L1/R1, 1/3 = L2/R2,
# i/k/j/l = left stick up/down/left/right, t/g/f/h = right stick, w/a/d not used); build/pcsx2/xkey presses keys in the hidden display
# (DISPLAY=:N, the number printed as "display :N"; set DNUM=N to choose it), e.g. tools/ps2_online_run.sh.
# PINE=SLOT (env): PCSX2's PINE socket on (memory read/write, save states; tools/pcsx2/pine.py), at
# $XDG_RUNTIME_DIR/pcsx2.sock.SLOT (pcsx2.sock for slot 28011). Used by tools/g_ref_shot.sh.
# Fast boot (no BIOS logo), EE/IOP console logging on, the software renderer (no GPU needed), audio off.
# PCSX2_DEINTERLACE=N (env): PCSX2's deinterlace mode (0 automatic, the default; 6 blend: tools/render_score.py, whose
# held frames otherwise come out as either field from run to run in MH1)
set -e
cd "$(dirname "$0")/.."
PCSX2=${PCSX2:-$HOME/ps2dev/pcsx2/pcsx2-v2.8.2.AppImage}
PH=$(realpath -m "${PCSX2_HOME:-$HOME/ps2dev/pcsx2/data}")
D=$PH/PCSX2      # PCSX2 puts its folders in DATAPATH/PCSX2
ISO="disc/mh1/Monster Hunter (Japan).iso"; ELF=""; CARD=""; CARDFROM=""; SECS=120; SHOT=""; AFTER=""; LOG=""; SHOW=; NET=false; CHEATS=""
while [ $# -gt 0 ]; do
    case "$1" in
    --iso) ISO=$2; shift 2 ;;
    --elf) ELF=$2; shift 2 ;;
    --card) CARD=$2; shift 2 ;;
    --card-from) CARDFROM=$2; shift 2 ;;
    --seconds) SECS=$2; shift 2 ;;
    --shot) SHOT=$2; shift 2 ;;
    --after) AFTER=$2; shift 2 ;;
    --log) LOG=$2; shift 2 ;;
    --show) SHOW=1; shift ;;
    --net) NET=true; shift ;;
    --cheats) CHEATS=$2; shift 2 ;;
    *) echo "unknown option $1"; exit 2 ;;
    esac
done
[ -x "$PCSX2" ] || { echo "PCSX2 not found at $PCSX2 (official AppImage from github.com/PCSX2/pcsx2/releases)"; exit 1; }
mkdir -p "$D/bios" "$D/inis" "$D/memcards" "$D/logs" "$D/snaps"
BIOSF=${BIOS:-$(cd "$D/bios" && ls *.bin 2>/dev/null | head -1)}
[ -n "$BIOSF" ] && [ -f "$D/bios/$BIOSF" ] || { echo "no BIOS in $D/bios (the owner's own dump; never in the repo)"; exit 1; }
[ -f "$ISO" ] || { echo "no disc image $ISO (--iso)"; exit 1; }
[ -z "$ELF" ] || [ -f "$ELF" ] || { echo "no $ELF"; exit 1; }
if [ -n "$CARDFROM" ]; then
    mkdir -p build/pcsx2
    gcc -std=gnu99 -O1 -o build/pcsx2/savecard tools/pcsx2/savecard.c src/pc/fmt/ps2save.c src/pc/fmt/lzari.c \
        src/pc/rt/rt_save.c
    rm -f "$D/memcards/Mcd001.ps2"
    build/pcsx2/savecard "$CARDFROM" --export-save "$D/memcards/Mcd001.ps2"
elif [ -n "$CARD" ]; then
    cp "$CARD" "$D/memcards/Mcd001.ps2"
fi
[ -f "$D/memcards/Mcd001.ps2" ] || echo "note: no memory card in slot 1 yet (--card / --card-from); PCSX2 makes an empty one"
LOG=${LOG:-$D/logs/emulog.txt}
mkdir -p "$D/cheats"
rm -f "$D/cheats/"SLPM-65495_*.pnach
CHEATON=false; CHEATLIST=""
if [ -n "$CHEATS" ]; then
    cp "$CHEATS" "$D/cheats/SLPM-65495_C2008A92.pnach"
    if [ -n "$ELF" ]; then  # a boot ELF (the launcher) keeps its own CRC (PCSX2's: the XOR of the file's words) for the game
        CRC=$(python3 -c "import struct,sys; d=open(sys.argv[1],'rb').read(); d+=bytes(-len(d)%4); c=0
for (w,) in struct.iter_unpack('<I', d): c ^= w
print('%08X' % c)" "$ELF")
        cp "$CHEATS" "$D/cheats/SLPM-65495_$CRC.pnach"
    fi
    CHEATON=true
    CHEATLIST=$(sed -n 's/^\[\(.*\)\]$/Enable = \1/p' "$CHEATS")
fi
PINEON=false; [ -z "$PINE" ] || PINEON=true

# settings: written fresh each run (PCSX2 adds its defaults for the rest)
cat > "$D/inis/PCSX2.ini" <<INI
[UI]
SettingsVersion = 1
SetupWizardIncomplete = false
ConfirmShutdown = false
StartFullscreen = false
HideMouseCursor = false
RenderToSeparateWindow = false
HideMainWindowWhenRunning = true
[AutoUpdater]
CheckAtStartup = false
[Folders]
Bios = bios
MemoryCards = memcards
Logs = logs
Snapshots = snaps
[Filenames]
BIOS = $BIOSF
[EmuCore]
EnableFastBoot = true
HostFs = true
EnablePatches = false
EnableCheats = $CHEATON
EnableDiscordPresence = false
EnablePINE = $PINEON
PINESlot = ${PINE:-28011}
[EmuCore/GS]
Renderer = 13
VsyncEnable = 0
OsdShowMessages = false
deinterlace_mode = ${PCSX2_DEINTERLACE:-0}
[SPU2/Output]
Backend = Null
OutputModule = nullout
[MemoryCards]
Slot1_Enable = true
Slot1_Filename = Mcd001.ps2
Slot2_Enable = false
[Logging]
EnableSystemConsole = false
EnableFileLogging = true
EnableTimestamps = true
EnableVerbose = false
EnableEEConsole = true
EnableIOPConsole = true
[InputSources]
SDL = false
[Achievements]
Enabled = false
[Cheats]
$CHEATLIST
[DEV9/Eth]
EthEnable = $NET
EthApi = Sockets
EthDevice = Auto
InterceptDHCP = true
AutoMask = true
AutoGateway = true
ModeDNS1 = Auto
ModeDNS2 = Auto
EthLogDHCP = true
EthLogDNS = true
[Pad1]
Type = DualShock2
Up = Keyboard/Up
Right = Keyboard/Right
Down = Keyboard/Down
Left = Keyboard/Left
Cross = Keyboard/X
Circle = Keyboard/C
Square = Keyboard/Z
Triangle = Keyboard/S
Start = Keyboard/Return
Select = Keyboard/Backspace
L1 = Keyboard/Q
R1 = Keyboard/E
L2 = Keyboard/1
R2 = Keyboard/3
LUp = Keyboard/I
LDown = Keyboard/K
LLeft = Keyboard/J
LRight = Keyboard/L
RUp = Keyboard/T
RDown = Keyboard/G
RLeft = Keyboard/F
RRight = Keyboard/H
[Pad2]
Type = None
INI

set -- -batch -nogui -fastboot -datapath "$PH" -logfile "$LOG" -earlyconsolelog
[ -z "$ELF" ] || set -- "$@" -elf "$(realpath "$ELF")"
set -- "$@" -- "$(realpath "$ISO")"

# Everything this script starts (PCSX2, its Xvfb) is stopped when it ends, also when it is killed (TERM/INT/HUP) or
# fails: tools/emu_lib.sh. To end a run early, kill this script's PID (never PCSX2 by name).
. tools/emu_lib.sh
PID=; XPID=
stop() { emu_stop $PID $XPID; }
emu_traps stop
XVFB=${XVFB:-$HOME/xboxdev/xvfb/root/usr/bin/Xvfb}
if [ -z "$SHOW" ]; then
    [ -x "$XVFB" ] || { echo "no Xvfb at $XVFB (see tools/run_xemu.sh's header; or --show)"; exit 1; }
    DNUM=${DNUM:-$(( 90 + $$ % 9 ))}
    echo "display :$DNUM"
    # (DISPLAY must not point at the new display while it starts: Xvfb then hangs, and everything that connects)
    env -u DISPLAY "$XVFB" :$DNUM -screen 0 1280x960x24 -nolisten tcp >/dev/null 2>&1 &
    XPID=$!
    export DISPLAY=:$DNUM
    unset WAYLAND_DISPLAY
    export QT_QPA_PLATFORM=xcb
    emu_sleep 2
fi
export SDL_AUDIODRIVER=dummy
[ -x build/pcsx2/xkey ] || { mkdir -p build/pcsx2; gcc -O1 -o build/pcsx2/xkey tools/pcsx2/xkey.c -lX11 \
    /usr/lib/x86_64-linux-gnu/libXtst.so.6 2>/dev/null || echo "note: xkey not built (no libX11/libXtst)"; }
"$PCSX2" "$@" >"$D/logs/stdout.txt" 2>&1 &
PID=$!
T=0
for A in ${AFTER:-$SECS}; do
    emu_sleep $(( A - T )); T=$A
    if [ -n "$SHOT" ] && [ -z "$SHOW" ]; then
        OUT=$SHOT; [ -z "$AFTER" ] || [ "$A" = "${AFTER##* }" ] || OUT="${SHOT%.png}_${A}s.png"
        ffmpeg -nostdin -v error -y -f x11grab -i "$DISPLAY" -frames:v 1 "$OUT" && echo "screenshot: $OUT"
    fi
    kill -0 $PID 2>/dev/null || { echo "PCSX2 ended early (see $D/logs/stdout.txt)"; break; }
done
[ $T -ge $SECS ] || emu_sleep $(( SECS - T ))
echo "log: $LOG"
