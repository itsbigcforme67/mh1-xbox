#!/usr/bin/env python3
"""vtry.py FILE FUNC_START_MARK FUNC < variants ('\n=====\n' separated, each the FULL function text
 from the mark through its closing brace). Prints the check.py diff count for each."""
import sys,subprocess,os
f,sm,fn=sys.argv[1:4]
src=open(f).read()
a=src.index(sm); b=src.index("\n}\n",a)+3
vs=sys.stdin.read().split('\n=====\n')
z=os.path.join(os.path.dirname(f),'zz.c')
for i,v in enumerate(vs):
    open(z,'w').write(src[:a]+v.rstrip('\n')+'\n'+src[b:])
    out=subprocess.run(['./tools/cnt.sh',z,fn],capture_output=True,text=True).stdout.strip()
    al=subprocess.run(['python3','tools/align.py',z,fn],capture_output=True,text=True).stdout.split('\n')
    n=sum(1 for l in al if l.startswith('   - ') or l.startswith('   + '))
    print(i,out or 'COMPILE ERROR/NONE','align-lines',n)
os.remove(z)
