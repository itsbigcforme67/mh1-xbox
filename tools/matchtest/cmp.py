# Compare functions in a compiled .o against SLPM_654.95, ignoring relocated fields
# (lui/addiu/load/store immediates, jal targets). Usage: cmp.py file.o FUNC...
import sys,struct,csv
import os; HERE = os.path.dirname(os.path.abspath(__file__)); ROOT = os.path.dirname(os.path.dirname(HERE)); sys.path.insert(0, os.path.join(ROOT, "tools"))
from mips_dis import elf_sections, read_vaddr, obj_function
elf=open(os.path.join(ROOT, "disc/mh1/SLPM_654.95"),"rb").read()
syms={r["name"]:(int(r["addr"],16),int(r["size"])) for r in csv.DictReader(open(os.path.join(ROOT, "docs/survey/mh1_symbols.csv"))) if r["type"]=="FUNC" and r["section"]=="main"}
def mask(w):
    op=w>>26
    if op in (2,3): return w&0xFC000000
    if op in (0x0F,0x09,0x20,0x21,0x23,0x24,0x25,0x28,0x29,0x2B,0x31,0x39) : return w&0xFFFF0000
    return w
obj=open(sys.argv[1],"rb").read()
res=[]
for f in sys.argv[2:]:
    a,n=syms[f]; orig=read_vaddr(elf,a,n); mine=obj_function(obj,f)
    if mine is None: res.append(f+":missing"); continue
    ow=struct.unpack("<%dI"%(len(orig)//4),orig); mw=struct.unpack("<%dI"%(len(mine)//4),mine)
    exact = len(ow)==len(mw) and all(mask(x)==mask(y) for x,y in zip(ow,mw))
    res.append("%s:%s"%(f,"OK" if exact else "diff(%d/%d)"%(len(mw),len(ow))))
print(" ".join(res))
