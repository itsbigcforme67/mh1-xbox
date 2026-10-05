import re,sys
def load():
    s=open('/home/james/claude projects/MH XBOX/mh1-wt/D/include/em.h').read()
    i=s.index('typedef struct EMW {'); j=s.index('} EMW;',i)
    m={}
    for l in s[i:j].split('\n'):
        mm=re.match(r'\s*(\w+(?: \*)?)\s+\*?\s*(\w+)(\[[^\]]*\])?;\s*/\*\s*0x([0-9A-F]+)',l)
        if mm and not mm.group(2).startswith('_pad'):
            m[int(mm.group(4),16)]=(mm.group(1),mm.group(2),mm.group(3))
    return m
F=load()
def size(t):
    t=t.replace('*','').strip()
    return {'s8':1,'u8':1,'s16':2,'u16':2,'s32':4,'u32':4,'f32':4,'int':4}.get(t,4)
def conv(s):
    def sub(mm):
        T=mm.group(1).replace('*','').strip(); off=int(mm.group(2),0)
        if off in F and not F[off][2]:
            ft,fn,_=F[off]
            if size(ft)==size(T) or T in ('s32','u32') and ft=='s32':
                if ft.replace(' ','')!=T and not (size(ft)==size(T)): return mm.group(0)
                return 'em->'+fn
        return mm.group(0)
    return re.sub(r'M2C_FIELD\(em, (\w+ \*), (0x[0-9A-Fa-f]+|\d+)\)',sub,s)
if __name__=='__main__':
    p=sys.argv[1]
    s=open(p).read(); n=s.count('M2C_FIELD(em,')
    s=conv(s); print(n,'->',s.count('M2C_FIELD(em,'))
    open(p,'w').write(s)
