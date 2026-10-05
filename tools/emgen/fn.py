import re,sys
p=sys.argv[1]
s=open(p).read()
for name in sys.argv[2:]:
    m=re.search(r'(?m)^\S[^\n]*\b%s\([^\n;]*\)\s*\{$'%re.escape(name),s)
    if not m: print('NOT FOUND',name); continue
    j=s.index('\n}\n',m.end())+3
    print(s[m.start():j])
