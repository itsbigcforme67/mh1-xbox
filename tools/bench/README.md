# Real-Xbox bench scripts (coordinator)

Need the owner's bench: RetroKM hub (127.0.0.1:24851, the `script` pad linked to xbox:P1 with keep on), the
presser (MULTI KM/xboxpress, xbox_power.py), the capture card on /dev/video2. Screenshots go to build/bench/.

    tools/bench/xb.sh tap b [ms] | hold <hex> <ms> | shot <name>   one move / one 4:3 screenshot
    tools/bench/xb_village.sh       power on, start G from the dashboard, load the save, go to the village
    tools/bench/coldloop.sh N       N cold power-on starts of G; black screen after 45 s = hang

The dashboard's game list keeps its position, so the scripts find "Monster Hunter G" by picture
(build/bench/tmpl_mhg.png: crop 300,495-660,522 of a 960x720 shot with it highlighted; regenerate if the list
changes). Taps: ~200 ms in the game, 60 ms on the dashboard (longer double-steps).
