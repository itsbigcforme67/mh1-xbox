#!/bin/sh
# bt4.sh "CPFILES" ACTIVITY : build with only those cp files linked and run the activity
cd "$(dirname "$0")/../.."
export CPFILES="$1"; shift
tools/targets/pcbuild.sh | tail -1
tools/test_activities.sh "$@" 2>&1 | tail -2
