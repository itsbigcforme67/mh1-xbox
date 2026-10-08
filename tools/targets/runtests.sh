#!/bin/sh
# run the whole PC test set in the documented order; prints one summary line per test
cd "$(dirname "$0")/../.."
for t in test_quest_loop test_progression test_urgent test_name_entry test_movie test_frog test_audio test_activities test_all_quests test_log test_pick; do
  if sh tools/$t.sh > build/tt_$t.log 2>&1; then echo "PASS $t"; else echo "FAIL $t (build/tt_$t.log)"; fi
done
echo ALLDONE
