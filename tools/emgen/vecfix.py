import re,struct,sys
def f2s(v):
    r=repr(v)
    if '.' not in r and 'e' not in r: r+='.0'
    return r+'f'
def val(t):
    if t.startswith('0x'):
        b=int(t,16)
        if b==0: return '0.0f'
        return f2s(struct.unpack('<f',struct.pack('<I',b))[0])
    return t
def fix(s):
    parts=re.split(r'(?m)^(?=\S[^\n]*\)\s*\{$)',s)
    out=[]
    for f in parts:
        pat=re.compile(r'(?m)^( +)sp([0-9A-F]+) = ([^;\n]+);\n(?:\1sp([0-9A-F]+) = ([^;\n]+);\n)(?:\1sp([0-9A-F]+) = ([^;\n]+);\n)')
        names=['v','v2','v3','v4','v5','v6','v7','v8']
        k=[0]; decls=[]; byoff={}
        def rep(m):
            offs=[int(m.group(2),16),int(m.group(4),16),int(m.group(6),16)]
            lo=min(offs)
            if sorted(offs)!=[lo,lo+4,lo+8]: return m.group(0)
            if lo in byoff: nm=byoff[lo]
            else:
                nm=names[k[0]]; k[0]+=1; byoff[lo]=nm
            ind=m.group(1)
            res=''
            for o,v in ((offs[0],m.group(3)),(offs[1],m.group(5)),(offs[2],m.group(7))):
                res+='%s%s[%d] = %s;\n'%(ind,nm,(o-lo)//4,val(v.strip()))
            if (nm,lo) not in decls: decls.append((nm,lo))
            f_ren.append(('sp%X'%lo,nm))
            return res
        f_ren=[]
        f2=pat.sub(rep,f)
        if not decls: out.append(f); continue
        for old,nm in f_ren:
            f2=f2.replace('&%s,'%old,'%s,'%nm).replace('&%s)'%old,'%s)'%nm)
        for nm,lo in decls:
            for o in (lo,lo+4,lo+8):
                f2=re.sub(r'    (?:s32|f32|u32) sp%X;\n'%o,'',f2)
        decl=''.join('    f32 %s[3];\n'%nm for nm,lo in decls)
        f2=re.sub(r'\{\n',r'{\n'+decl,f2,count=1)
        out.append(f2)
    return ''.join(out)
if __name__=='__main__':
    p=sys.argv[1]; s=open(p).read(); s2=fix(s); open(p,'w').write(s2); print(len(s),len(s2))
