import os as _os
ROOT=_os.path.dirname(_os.path.dirname(_os.path.dirname(_os.path.abspath(__file__))))
SCR=_os.environ.get('B_SCRATCH',_os.path.join(ROOT,'build/b_scratch'))
_os.makedirs(SCR,exist_ok=True)
import re,sys
# mkstatic.py IN OUT name...   : prefix 'static ' on prototypes and definitions of the given function names
inp,out=sys.argv[1],sys.argv[2]; names=sys.argv[3:]
s=open(inp).read().split('\n')
r=[]
for l in s:
    for n in names:
        if re.match(r'^(?!static)[A-Za-z_][\w \*]*?\b%s\('%n,l) :
            l='static '+l
    r.append(l)
open(out,'w').write('\n'.join(r))
