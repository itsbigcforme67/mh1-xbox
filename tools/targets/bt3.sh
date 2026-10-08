#!/bin/sh
# bt3.sh SKIPREGEX [ACTIVITY...] : build with those matched files skipped and run tools/test_activities.sh on the given activities
cd "$(dirname "$0")/../.."
export MATCHED_SKIP="$1"; shift
tools/targets/pcbuild.sh | tail -1
tools/test_activities.sh "$@" 2>&1 | tail -3
