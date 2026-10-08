#!/bin/sh
# Player activities the quest sweep does not cover (gathering, fishing, carving, village shops / forge / item box,
# item combining, items used in a quest, the trader): one PASS/FAIL line each, about a minute headless.
# Details and the list of names: tools/test_activities.py; findings in docs/pc.md "Activity tests".
cd "$(dirname "$0")/.."
[ -x build/pc/mhview ] || { echo "build/pc/mhview missing: run tools/build_pc.sh first"; exit 1; }
exec python3 tools/test_activities.py "$@"
