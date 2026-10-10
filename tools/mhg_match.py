"""mhg_match.py: how many MH1 functions appear unchanged in PS2 Monster Hunter G (addresses and
immediates masked). Needs disc/mh1/split and disc/mhg/overlays."""
import struct,csv,sys,collections
import os
R=os.path.dirname(os.path.abspath(__file__))+"/../"
def words(b): return list(struct.unpack('<%dI'%(len(b)//4),b[:len(b)//4*4]))
MASKLO={8,9,0xd,0xf,0x1e,0x1f}|set(range(0x20,0x30))|{0x31,0x35,0x39,0x3d,0x37,0x3f}
def m(w):
    op=w>>26
    if op in (2,3): return w&0xfc000000
    if op in MASKLO: return w&0xffff0000
    return w
# MH1 modules: name -> (bytes, base addr)
ov1={'game.bin':0x533980,'lobby.bin':0x533980,'select.bin':0x533980,'yn.bin':0x533980}
mh1={'main':(open(R+'disc/mh1/split/main.bin','rb').read(),0x100000)}
for k,b in ov1.items(): mh1[k]=(open(R+'disc/mh1/split/'+k,'rb').read()[0x40:],b)
# G modules
ge=open(R+'disc/mhg/SLPM_658.69','rb').read()
G={'main':ge[0x200:0x200+0x1ef200]}
for k in ['game.bin','lobby.bin','sub_main.bin','select.bin','yn.bin']:
    G[k]=open(R+'disc/mhg/overlays/'+k,'rb').read()[0x40:]
K=6
gw={};idx=collections.defaultdict(list);graw={}
for k,b in G.items():
    w=words(b);graw[k]=w;mw=[m(x) for x in w];gw[k]=mw
    for i in range(len(mw)-K): idx[tuple(mw[i:i+K])].append((k,i))
st=collections.defaultdict(lambda:[0,0,0,0,0,0]) # nfunc, nmatch, bytes, bytesmatch, exact, small
where=collections.Counter()
for r in csv.DictReader(open(R+'docs/survey/mh1_symbols.csv')):
    if r['type']!='FUNC': continue
    sec=r['section']
    if sec not in mh1: continue
    sz=int(r['size']); a=int(r['addr'],16)
    if sz<4: continue
    b,base=mh1[sec]; off=a-base
    fw=words(b[off:off+sz]); s=st[sec]; s[0]+=1; s[2]+=sz
    if len(fw)<K: s[5]+=1; continue
    mf=[m(x) for x in fw]
    hit=None
    for (k,i) in idx.get(tuple(mf[:K]),()):
        if gw[k][i:i+len(mf)]==mf: hit=(k,i);break
    if hit:
        s[1]+=1;s[3]+=sz;where[(sec,hit[0])]+=1
        if graw[hit[0]][hit[1]:hit[1]+len(fw)]==fw: s[4]+=1
print("%-11s %6s %6s %6s %8s %8s %5s"%("mh1 mod","funcs","found","exact","bytes","found%","tiny"))
for k,s in st.items(): print("%-11s %6d %6d %6d %8d %7.1f%% %5d"%(k,s[0],s[1],s[4],s[2],100*s[3]/s[2],s[5]))
for k,v in sorted(where.items()): print(k,v)
