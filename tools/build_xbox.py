#!/usr/bin/env python3
"""build_xbox.py - the original Xbox build (nxdk), from a finished PC build.

tools/build_pc.sh leaves every object's compile command in build/pc/cmd/
and the link-adapt headers in build/pc/adapt/ (weak symbols, aliases): the
same game C and runtime are compiled again here with nxdk's clang (target
i386-pc-win32), the PC front-end with Xbox stand-ins (src/pc/xbox: no
drawing yet, no memory card), a symbol table from the objects (rt_data.c),
then linked with nxdk-link and turned into build/xbox/default.xbe by cxbe.

    . ~/xboxdev/env.sh; python3 tools/build_xbox.py      # see docs/xbox.md
Standard library only.
"""
import concurrent.futures as cf, glob, os, shlex, shutil, subprocess, sys

NXDK = os.environ.get('NXDK_DIR', os.path.expanduser('~/xboxdev/nxdk'))
OUT = 'build/xbox'
OBJ = OUT + '/obj'
# clang 18 makes these gcc warnings errors in C99; the decompiled C relies on them
RELAX = ['-Wno-error=implicit-function-declaration', '-Wno-error=implicit-int', '-Wno-error=int-conversion',
         '-Wno-error=incompatible-function-pointer-types', '-Wno-error=return-type', '-w']
# the PC front-end (tools/build_pc.sh PC=) with the Xbox stand-ins instead of OpenGL and the host card
FRONT = ['src/pc/viewer.c', 'src/pc/fl/fl_model.c', 'src/pc/xbox/gfx_null.c', 'src/pc/fmt/afs.c', 'src/pc/fmt/melt.c',
         'src/pc/fmt/amo.c', 'src/pc/fmt/apx.c', 'src/pc/fmt/ahi.c', 'src/pc/fmt/aan.c', 'src/pc/fmt/hits.c',
         'src/pc/pad/pad_sdl.c', 'src/pc/fmt/snd.c', 'src/pc/audio/audio_mix.c', 'src/pc/audio/audio_sdl.c',
         'src/pc/gfx/gfx_rec.c', 'src/pc/rt/rt_mem.c', 'src/pc/xbox/mc_null.c', 'src/pc/xbox/xbox_libc.c']
COMPAT = 'src/pc/xbox/xbox_compat.h'     # fopen with '/' -> '\\' (xbox_libc.c)
SKIP = {'rt_mc', 'rt_symtab', 'rt_memstat'}
LIBS = ['xboxkrnl/libxboxkrnl.lib', 'libpdclib.lib', 'winmm.lib', 'libwinapi.lib', 'libnxdk_hal.lib', 'libnxdk.lib',
        'libnxdk_automount_d.lib', 'libpbkit.lib', 'nxdk_usb.lib', 'libxboxrt.lib', 'libzlib.lib', 'libSDL2.lib']

def xcmd(cmd, out):
    """a recorded gcc command -> nxdk-cc arguments"""
    a = shlex.split(cmd)
    n, skip = [], 0
    for x in a[1:]:
        if skip:
            skip -= 1
            continue
        if x in ('-m32', '-D_REENTRANT', '-g', '-Wall', '-Wextra'):
            continue
        if x in ('-idirafter', '-o'):
            skip = 1
            continue
        if x.startswith(('-B', '-L', '-I/usr/include', '-D_POSIX_C_SOURCE', '-fno-aggressive')):
            continue
        n.append(x)
    return ['nxdk-cc'] + RELAX + n + ['-o', out]

def run(job):
    name, args = job
    r = subprocess.run(args, capture_output=True, text=True)
    return name, r.returncode, r.stderr

def shaders():
    """src/pc/xbox/shaders/*.cg -> build/xbox/shaders/*.inl (nxdk's cgc + vp20/fp20compiler)"""
    out = OUT + '/shaders'
    os.makedirs(out, exist_ok=True)
    import platform
    cgc = NXDK + '/tools/cg/linux/' + ('cgc' if platform.machine() == 'x86_64' else 'cgc.i386')
    for src in glob.glob('src/pc/xbox/shaders/*.cg'):
        name, kind = os.path.basename(src).split('.')[:2]
        prof, conv = ('vp20', 'vp20compiler') if kind == 'vs' else ('fp20', 'fp20compiler')
        tmp = '%s/%s.%s' % (out, name, prof)
        subprocess.run([cgc, '-profile', prof, '-o', tmp, src], check=True, capture_output=True)
        inl = subprocess.run([NXDK + '/tools/%s/%s' % (conv, conv), tmp], check=True, capture_output=True, text=True).stdout
        open('%s/%s.inl' % (out, name), 'w').write(inl)
    return out

def main():
    # --gfx null (default: draws nothing, for a first headless boot) or nv2a (pbkit, gfx_nv2a.c)
    gfx = sys.argv[sys.argv.index('--gfx') + 1] if '--gfx' in sys.argv else 'null'
    FRONT[FRONT.index('src/pc/xbox/gfx_null.c')] = 'src/pc/xbox/gfx_%s.c' % gfx
    dst = OUT if gfx == 'null' else OUT + '/' + gfx      # where main.exe / default.xbe / the ISO go
    os.makedirs(OBJ, exist_ok=True)
    os.makedirs(dst, exist_ok=True)
    sdl_extra = ['-I' + shaders()] if gfx == 'nv2a' else []
    jobs = []
    # only the objects the PC build links (build/pc/objs.txt, link order); build/pc
    # can hold stale objects of files no longer built
    linked = [os.path.basename(l.strip())[:-2] for l in open('build/pc/objs.txt') if l.strip()]
    for b in linked:
        c = 'build/pc/cmd/%s.sh' % b
        if b in SKIP or not os.path.exists(c):
            continue
        args = xcmd(open(c).read().strip(), '%s/%s.obj' % (OBJ, b))
        h = 'build/pc/adapt/%s.h' % b
        if os.path.exists(h):
            # clang names a COFF weak definition's default ".weak.SYM.default.FIRST"
            # after the object's first external definition; when that is a shared
            # constant (__real@...) two objects collide. A unique first function fixes it.
            t = '%s/tag/%s.h' % (OUT, b)
            os.makedirs(os.path.dirname(t), exist_ok=True)
            open(t, 'w').write('void __xtag_%s(void) {}\n' % b.replace('-', '_'))
            args += ['-include', t, '-include', h]
        jobs.append((b, args + ['-include', COMPAT]))
    sdl = ['-I' + NXDK + '/lib/sdl/SDL2/include', '-DXBOX'] + sdl_extra
    for f in FRONT + ['src/pc/rt/rt_memstat.c']:
        b = 'x_' + os.path.basename(f)[:-2]
        args = ['nxdk-cc', '-std=gnu99', '-O2', '-Iinclude', '-Isrc/pc'] + RELAX + sdl
        if f != 'src/pc/rt/rt_memstat.c':
            args += ['-include', 'src/pc/rt/rt_memstat.h']
        if 'xbox_libc' not in f:
            args += ['-include', COMPAT]
        jobs.append((b, args + ['-c', f, '-o', '%s/%s.obj' % (OBJ, b)]))
    res = list(cf.ThreadPoolExecutor(4).map(run, jobs))
    bad = [r for r in res if r[1]]
    with open(OUT + '/compile.log', 'w') as f:
        for n, c, e in res:
            if c or e.strip():
                f.write('== %s\n%s\n' % (n, e))
    print('compiled %d objects, %d failed' % (len(res), len(bad)))
    for n, c, e in bad:
        print('FAIL', n, [l for l in e.splitlines() if 'error' in l][:2])
    if bad:
        sys.exit(1)
    objs = ['%s/%s.obj' % (OBJ, j[0]) for j in jobs]
    objs = [o for o in objs if os.path.basename(o) not in ('x_gfx_null.obj', 'x_gfx_nv2a.obj')] + ['%s/x_gfx_%s.obj' % (OBJ, gfx)]
    nm = subprocess.run(['llvm-nm', '--defined-only', '-g'] + [o for o in objs if 'x_xbox_' not in o], capture_output=True, text=True).stdout
    subprocess.run([sys.executable, 'tools/gen_symtab.py', OUT + '/rt_symtab.c', '--prefix', '_'], input=nm, text=True, check=True)
    subprocess.run(['nxdk-cc', '-O2', '-w', '-c', OUT + '/rt_symtab.c', '-o', OBJ + '/rt_symtab.obj'], check=True)
    objs.append(OBJ + '/rt_symtab.obj')
    sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
    import coff_weak                            # GNU-ld weak rules for lld-link
    objs, nfix = coff_weak.resolve(objs, OUT + '/linkobj')
    print('coff_weak: %d losing weak definitions made references' % nfix)
    link = ['nxdk-link', '-include:_automount_d_drive', '-stack:0x100000', '-out:' + dst + '/main.exe', '-map:' + dst + '/main.map'] \
        + objs + [NXDK + '/lib/' + l for l in LIBS]
    r = subprocess.run(link, capture_output=True, text=True)
    open(dst + '/link.log', 'w').write(r.stdout + r.stderr)
    if r.returncode:
        und = sorted(set(l.split('undefined symbol: ')[1] for l in (r.stdout + r.stderr).splitlines() if 'undefined symbol:' in l))
        print('link failed: %d undefined symbols (build/xbox/link.log): %s' % (len(und), ' '.join(und[:40])))
        sys.exit(1)
    subprocess.run([NXDK + '/tools/cxbe/cxbe', '-OUT:' + dst + '/default.xbe', '-TITLE:MH1 port', dst + '/main.exe'],
                   check=True, capture_output=True)
    print('built %s/default.xbe (%d bytes)' % (dst, os.path.getsize(dst + '/default.xbe')))
    # an XISO holding only the XBE (the game's data files are not shipped; docs/xbox.md)
    iso_dir = dst + '/iso'
    os.makedirs(iso_dir, exist_ok=True)
    shutil.copy(dst + '/default.xbe', iso_dir + '/default.xbe')
    if os.path.exists(dst + '/mh1.iso'):
        os.remove(dst + '/mh1.iso')
    subprocess.run([NXDK + '/tools/extract-xiso/build/extract-xiso', '-c', iso_dir, dst + '/mh1.iso'],
                   check=True, capture_output=True)
    print('built %s/mh1.iso (%d bytes)' % (dst, os.path.getsize(dst + '/mh1.iso')))

if __name__ == '__main__':
    main()
