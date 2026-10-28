#!/bin/bash
# The coordinator's check before a push: byte-matching rebuild (all five modules OK),
# PC build, every PC test, both Xbox builds. Prints one line per step; logs in build/check/.
cd "$(dirname "$0")/.." || exit 1
mkdir -p build/check
fail=0
export MH1_HIDDEN=1   # every game run hidden (no window on the owner's screen, no real controller), even a test that forgets
# tests that start an emulator (PCSX2) are not run here: emulators run alone, by hand (docs/agents/BRIEF.md)
EMU_TESTS="test_pdtest test_ps2_online test_three_way test_ps2_hunt test_ps2_g_online test_ps2_g_hunt"
# memory caps per step (docs/agents/BRIEF.md): 1.5 GB for PC builds/tests, 3 GB for the PS2 rebuild (wibo),
# none for test_save (AddressSanitizer). Don't run this script itself under a cap: a cap can't be raised again.
step() {  # step NAME CMD...
    local n=$1; shift
    local cap=1500000
    case $n in test_save|test_coop) cap=unlimited ;;   # test_coop: its Windows joiner runs under Wine, which cannot start under ulimit -v
    test_ps2_patch|test_ps2_online|test_pdtest) cap=3000000 ;; esac   # ASan; wibo (PS2 compiler)
    if (ulimit -v $cap; "$@") > "build/check/$n.log" 2>&1; then echo "ok   $n"; else echo "FAIL $n (build/check/$n.log)"; fail=1; fi
}
(ulimit -v 3000000; tools/rebuild.sh) > build/check/rebuild.log 2>&1
n=$(grep -cE '^[a-z]+ +OK ' build/check/rebuild.log)
if [ "$n" = 5 ]; then echo "ok   rebuild (5 OK)"; else echo "FAIL rebuild ($n OK, build/check/rebuild.log)"; fail=1; fi
step build_pc tools/build_pc.sh
step kr_returns python3 tools/check_kr_returns.py --implicit build/pc/cmd --mwcc mh1   # Xbox ABI hazard: char/short/float functions declared or used as int
step build_pc_online env ONLINE=1 tools/build_pc.sh
step xbox_online bash -c ". ~/xboxdev/env.sh && python3 tools/build_xbox.py --online --gfx nv2a"   # (reads the online objs.txt)
step build_pc_mhg tools/build_pc_mhg.sh   # tests that also run G use this binary
step kr_returns_g python3 tools/check_kr_returns.py --implicit build/mhg/pctree/build/pc/cmd --mwcc mhg
step g_tables python3 tools/check_g_tables.py build/mhg/pctree   # a G table kept smaller than G's own (round 10's mem_tex overflow)
step build_pc_mhg_online env ONLINE=1 tools/build_pc_mhg.sh   # test_online_g.sh (G online PC client)
step xbox_g_online bash -c ". ~/xboxdev/env.sh && python3 tools/build_xbox.py --version mhg --online --gfx nv2a"   # (build/mhg/pctree_online's objects)
for t in tools/test_quest_loop.sh $(ls tools/test_*.sh | grep -v test_quest_loop); do   # test_quest_loop first: it makes the save test_dbgmenu / test_menu load
    n=$(basename "$t" .sh)
    case " $EMU_TESTS " in *" $n "*) echo "skip $n (starts an emulator: run it alone by hand)"; continue ;; esac
    step "$n" "$t"
done
step build_pc_plain tools/build_pc.sh   # the ONLINE=1 build rewrote objs.txt; the Xbox build reads the plain one
step xbox_null bash -c ". ~/xboxdev/env.sh && python3 tools/build_xbox.py"
step xbox_nv2a bash -c ". ~/xboxdev/env.sh && python3 tools/build_xbox.py --gfx nv2a"
exit $fail
