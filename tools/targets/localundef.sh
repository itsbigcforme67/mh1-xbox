#!/bin/sh
# matched objects that call something which exists only as a file-static function in another PC object
cd "$(dirname "$0")/../.."
for o in $(for f in $MATCHED_FILES; do echo build/pc/$(basename $f .c).o; done); do
  nm -u $o | awk '{print $2}'
done | LC_ALL=C sort -u > build/.u.txt
for o in build/pc/*.o; do nm --defined-only $o | awk '$2=="t"{print $3, "'"$(basename $o)"'"}'; done | LC_ALL=C sort > build/.t.txt
LC_ALL=C join build/.u.txt build/.t.txt
