#!/bin/sh
# Play-test the PC build with a controller or the keyboard.
#   tools/play.sh            quest 10 (Rathian), normal rules
#   tools/play.sh easy       same, but the hunter can't faint
#   tools/play.sh village    start in Kokoto village
# Xbox / other SDL game controllers work when plugged in (also mid-game):
#   left stick move, right stick attack, A roll, B sheathe, X use item,
#   RB guard, LB camera reset, d-pad camera, Start pause menu.
# Keyboard: W/A/S/D move, arrow keys attack, K roll, L sheathe, J item,
#   E guard, Q camera reset, T/F/G/H camera, Enter pause menu.
# Esc or closing the window quits.
cd "$(dirname "$0")/.."
RUN=build/pc/mhview; BUILD=tools/build_pc.sh; SIZE=1024x768
if [ "$(uname -m)" = aarch64 ]; then    # ARM box: armhf build via tools/build_arm.sh
    RUN=tools/run_arm.sh; BUILD=tools/build_arm.sh; SIZE=${MH_SIZE:-960x720}
fi
[ -x build/pc/mhview ] || $BUILD || exit 1
case "$1" in
    easy)    export RT_PL_GOD=1; set -- --quest 10 ;;
    village) export RT_VILLAGE_START=1 RT_VILLAGE_SKIP_INTRO=1; set -- --quest 10 ;;
    *)       set -- --quest 10 ;;
esac
exec $RUN disc/mh1 --play --size $SIZE "$@"
