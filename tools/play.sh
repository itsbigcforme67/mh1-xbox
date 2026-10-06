#!/bin/sh
# Play-test the PC build with a controller or the keyboard.
#   tools/play.sh            from power-on: logos, title, new game (character
#                            creation) or continue (load), then Kokoto village
#   tools/play.sh quest      straight into quest 10 (Rathian), normal rules
#   tools/play.sh easy       same, but the hunter can't faint
#   tools/play.sh village    straight into Kokoto village (no save data)
# Saves (memory card) go to ~/.local/share/mh1pc/memcard0 (MH1_SAVE_DIR
# overrides): save in the house bed or after a quest, load with CONTINUE.
# Xbox / other SDL game controllers work when plugged in (also mid-game):
#   left stick move, right stick attack, A roll, B sheathe, X use item,
#   RB guard, LB camera reset, d-pad camera, Start pause menu.
#   In menus: B (circle) confirms, A (cross) cancels, as on the Japanese PS2.
# Keyboard: W/A/S/D move, arrow keys attack, K roll (cross), L sheathe
#   (circle, confirm), J item (square), E guard, Q camera reset, T/F/G/H
#   d-pad, Enter start / pause menu. Name entry: type, Enter to finish.
# Esc or closing the window quits.
cd "$(dirname "$0")/.."
RUN=build/pc/mhview; BUILD=tools/build_pc.sh; SIZE=1024x768
if [ "$(uname -m)" = aarch64 ]; then    # ARM box: armhf build via tools/build_arm.sh
    RUN=tools/run_arm.sh; BUILD=tools/build_arm.sh; SIZE=${MH_SIZE:-960x720}
fi
[ -x build/pc/mhview ] || $BUILD || exit 1
case "$1" in
    quest)   set -- --quest 10 ;;
    easy)    export RT_PL_GOD=1; set -- --quest 10 ;;
    village) export RT_VILLAGE_START=1 RT_VILLAGE_SKIP_INTRO=1; set -- --quest 10 ;;
    *)       set -- --boot ;;
esac
exec $RUN disc/mh1 --play --size $SIZE "$@"
