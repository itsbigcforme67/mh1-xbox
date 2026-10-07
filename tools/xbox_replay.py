#!/usr/bin/env python3
"""xbox_replay.py - replay recorded gcc -c commands with nxdk-cc (see
tools/xbox_compile_check.sh). Standard library only."""
import os, sys, glob, subprocess, concurrent.futures as cf

def main():
    rec = sys.argv[1]
    out = 'build/xbox/obj'
    os.makedirs(out, exist_ok=True)
    jobs = []
    for f in sorted(glob.glob(rec + '/*.args')):
        a = [x.decode() for x in open(f, 'rb').read().split(b'\0')[:-1]]
        if '-c' not in a:
            continue
        o = a[a.index('-o') + 1]
        src = [x for x in a if x.endswith('.c')][-1]
        n, skip = [], 0
        for x in a:
            if skip:
                skip -= 1
                continue
            if x in ('-m32', '-D_REENTRANT', '-rdynamic', '-g'):
                continue
            if x in ('-idirafter', '-o'):
                skip = 1
                continue
            if x.startswith(('-B', '-L', '-I/usr/include', '-D_POSIX_C_SOURCE', '-fno-aggressive')):
                continue
            n.append(x)
        jobs.append((src, n, os.path.join(out, os.path.basename(o)[:-2] + '.obj')))
    # clang 18 makes these gcc warnings errors in C99; the decompiled C relies on them
    extra = ['-Wno-error=implicit-function-declaration', '-Wno-error=implicit-int', '-Wno-error=int-conversion',
             '-Wno-error=incompatible-function-pointer-types', '-Wno-error=return-type']
    def run(j):
        src, n, o = j
        r = subprocess.run(['nxdk-cc'] + extra + n + ['-o', o], capture_output=True, text=True)
        return src, r.returncode, r.stderr
    res = list(cf.ThreadPoolExecutor(4).map(run, jobs))
    with open('build/xbox/compile.log', 'w') as f:
        for s, c, e in res:
            if c or e.strip():
                f.write('== %s\n%s\n' % (s, e))
    bad = [r for r in res if r[1]]
    print('%d files, %d compile, %d fail (log: build/xbox/compile.log)' % (len(res), len(res) - len(bad), len(bad)))
    for s, c, e in bad:
        print('FAIL', s, [l for l in e.splitlines() if 'error' in l][:1])

if __name__ == '__main__':
    main()
