#!/bin/bash
# fn.sh FUNC [file] : print function definition (first line containing "FUNC(" whose line or next line has "{")
S=${B_SCRATCH:-$(cd "$(dirname "$0")/../.." && pwd)/build/b_scratch}
python3 - "$1" "${2:-$S/full_static.c}" <<'PY'
import sys,re
n,f=sys.argv[1],sys.argv[2]
s=open(f).read()
for m in re.finditer(r'^[A-Za-z_][\w \*]*\b%s\([^;{]*\)(?:\n[^;{]*;)*\s*\{\n'%re.escape(n),s,re.M):
    e=s.index('\n}\n',m.end())+3
    print(s[m.start():e]); break
PY
