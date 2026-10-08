#!/bin/sh
# bt5.sh "FUNCS" ACTIVITY : cp01 with those functions renamed away (host versions stay), then the activity
cd "$(dirname "$0")/../.."
export CP01_OFF="$1"; shift
rm -f build/pc/cp01.o
tools/targets/pcbuild.sh | tail -1
tools/test_activities.sh "$@" 2>&1 | tail -2
