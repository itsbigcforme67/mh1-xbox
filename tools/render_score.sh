#!/bin/sh
# Renderer score: the same fixed scenes on the PS2 (owner's disc in PCSX2, software renderer), the PC port and the Xbox
# port (xemu), each played twice, scored against the PS2 (SSIM %, mean colour difference, % of pixels differing).
# Scenes: config/render_scenes.json. Details, options and output: tools/render_score.py (docs/render_score.md holds
# the last scoreboard; the pictures stay in build/).
#
#   tools/render_score.sh                                  all scenes, both games, all three platforms (~1.5-2 h)
#   tools/render_score.sh --game mhg --scenes g_camp,g_cave --platforms pc,xbox --out DIR    (PC / Xbox again
#                                                          against the PS2 runs already in DIR)
#   tools/render_score.sh --md docs/render_score.md        also write the scoreboard there
#
# Emulators: each PCSX2 / xemu session takes the shared lock (flock on xemu.lock next to the main checkout, the one
# `flock xemu.lock CMD` and tools/g_ref_shot.sh use), then waits for > 2.5 GB available memory and no other pcsx2 /
# xemu process. Everything runs hidden (own Xvfb displays, no pad). Run without a memory cap (the emulators need
# that); the PC runs cap themselves at 1.5 GB. Needs: the PC builds (build/pc/mhview, build/mhg/pc/mhview_g), the
# Xbox nv2a builds (build/xbox/nv2a, build/mhg/xbox/nv2a), the referee's gate save states (tools/referee.py state).
cd "$(dirname "$0")/.."
[ "$(ulimit -v)" = unlimited ] || { echo "render_score: PCSX2 / xemu cannot start under a memory cap (ulimit -v); run uncapped"; exit 1; }
case "$1" in score|list|run) ;; *) set -- run "$@" ;; esac
exec python3 tools/render_score.py "$@"
