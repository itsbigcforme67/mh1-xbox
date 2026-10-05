#!/usr/bin/env python3
"""merge_union.py FILE: resolve git conflict blocks by keeping both sides
(for append-only lists like config/c_files.txt). Check for stale duplicates
afterwards."""
import sys,re
p=sys.argv[1]; s=open(p).read()
s=re.sub(r'<<<<<<< [^\n]*\n(.*?)=======\n(.*?)>>>>>>> [^\n]*\n', lambda m: m.group(1)+m.group(2), s, flags=re.S)
open(p,'w').write(s)
