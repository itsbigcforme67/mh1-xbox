#!/usr/bin/env python3
"""pc_memstat.py - static memory (.data, .bss, .rodata) of the PC build by
group, from the object files in build/pc (nm -S). The heap side is the
RT_MEM=ticks report of mhview (src/pc/rt/rt_memstat.c). Standard library only.

    python3 tools/pc_memstat.py [--top N]
"""
import glob, os, subprocess, sys

def group(o):
    b = os.path.basename(o)
    if b.startswith('rt_') or b in ('rt_gen.o', 'rt_tables.o'):
        return 'port runtime (src/pc/rt, generated tables)'
    return 'game C (main/game/lobby/select)'

def main():
    top = int(sys.argv[sys.argv.index('--top') + 1]) if '--top' in sys.argv else 15
    tot, syms = {}, []
    for o in glob.glob('build/pc/*.o'):
        out = subprocess.run(['nm', '-S', '--defined-only', o], capture_output=True, text=True).stdout
        for l in out.splitlines():
            p = l.split()
            if len(p) != 4 or p[2] not in 'bBdDrRgGsS':
                continue
            n = int(p[1], 16)
            kind = {'b': 'bss', 'B': 'bss', 's': 'bss', 'S': 'bss', 'd': 'data', 'D': 'data', 'g': 'data',
                    'G': 'data', 'r': 'rodata', 'R': 'rodata'}[p[2]]
            k = (group(o), kind)
            tot[k] = tot.get(k, 0) + n
            syms.append((n, p[3], os.path.basename(o), kind))
    for k in sorted(tot):
        print('%-46s %-6s %8d KB' % (k[0], k[1], tot[k] // 1024))
    print('largest objects:')
    for n, s, o, kind in sorted(syms, reverse=True)[:top]:
        print('  %8d KB  %-6s %-32s %s' % (n // 1024, kind, s, o))

if __name__ == '__main__':
    main()
