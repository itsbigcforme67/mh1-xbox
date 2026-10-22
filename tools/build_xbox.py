#!/usr/bin/env python3
"""build_xbox.py - the original Xbox build (nxdk), from a finished PC build.

tools/build_pc.sh leaves every object's compile command in build/pc/cmd/
and the link-adapt headers in build/pc/adapt/ (weak symbols, aliases): the
same game C and runtime are compiled again here with nxdk's clang (target
i386-pc-win32), the PC front-end with Xbox stand-ins (src/pc/xbox: no
drawing yet, no memory card), a symbol table from the objects (rt_data.c),
then linked with nxdk-link and turned into build/xbox/default.xbe by cxbe.

    . ~/xboxdev/env.sh; python3 tools/build_xbox.py      # see docs/xbox.md
    python3 tools/build_xbox.py --version mhg --gfx nv2a  # Monster Hunter G: from tools/build_pc_mhg.sh's
                                                          # build (build/mhg/pctree) -> build/mhg/xbox/
    python3 tools/build_xbox.py --online --gfx nv2a       # MH1 online (the town, rooms, co-op): from an
                                                          # ONLINE=1 tools/build_pc.sh -> build/xbox/online/nv2a/
                                                          # (nxdk lwIP; docs/xbox.md "Online on the Xbox")
Standard library only.
"""
import re
import concurrent.futures as cf, glob, os, shlex, shutil, subprocess, sys

NXDK = os.environ.get('NXDK_DIR', os.path.expanduser('~/xboxdev/nxdk'))
OUT = 'build/xbox'
OBJ = OUT + '/obj'
# clang 18 makes these gcc warnings errors in C99; the decompiled C relies on them
RELAX = ['-Wno-error=implicit-function-declaration', '-Wno-error=implicit-int', '-Wno-error=int-conversion',
         '-Wno-error=incompatible-function-pointer-types', '-Wno-error=return-type', '-w']
# the PC front-end (tools/build_pc.sh PC=) with the Xbox stand-ins instead of OpenGL and the host card
FRONT = ['src/pc/viewer.c', 'src/pc/dbgmenu.c', 'src/pc/menu.c', 'src/pc/font5x7.c', 'src/pc/gfx/gfx_opts.c', 'src/pc/fl/fl_model.c', 'src/pc/xbox/gfx_null.c', 'src/pc/fmt/afs.c', 'src/pc/fmt/melt.c',
         'src/pc/fmt/amo.c', 'src/pc/fmt/apx.c', 'src/pc/fmt/ahi.c', 'src/pc/fmt/hits.c',
         'src/pc/pad/pad_sdl.c', 'src/pc/fmt/snd.c', 'src/pc/movie/sfd.c', 'src/pc/audio/audio_mix.c', 'src/pc/audio/audio_sdl.c',
         'src/pc/gfx/gfx_rec.c', 'src/pc/gfx/gfx_pal.c', 'src/pc/gfx/gfx_skin.c', 'src/pc/rt/rt_mem.c', 'src/pc/xbox/mc_xbox.c', 'src/pc/xbox/xbox_libc.c',
         'src/pc/xbox/xbox_log.c', 'src/pc/xbox/xbox_env.c', 'src/pc/xbox/xbox_ac97.c', 'src/pc/xbox/xbox_watch.c',
         'src/pc/xbox/xbox_scanf.c', 'src/pc/pick.c', 'src/pc/xbox/kbm_xbox.c', 'src/pc/xbox/xbox_report.c']
# nxdk's USB HID class driver (lib/usb, not in its nxdk_usb.lib): compiled into the game for a USB keyboard / mouse
# (kbm_xbox.c), with its debug messages (sysprintf onto the screen) switched off in a patched copy
HID_SRC = ['lib/usb/libusbohci/src_hid/hid_core.c', 'lib/usb/libusbohci/src_hid/hid_driver.c']
USB_INC = ['-I' + NXDK + '/lib/usb/libusbohci/inc', '-I' + NXDK + '/lib/usb/libusbohci_xbox',
           '-DUSBH_USE_EXTERNAL_CONFIG="usbh_config_xbox.h"']
COMPAT = 'src/pc/xbox/xbox_compat.h'     # fopen with '/' -> '\\' (xbox_libc.c)
SKIP = {'rt_mc', 'rt_save', 'rt_symtab', 'rt_memstat'}
# --online: the ONLINE=1 PC build's network front-end (docs/network.md) over nxdk's lwIP (lib/net)
NET_FRONT = ['src/pc/net/net_cpinet.c', 'src/pc/net/net_dnas.c', 'src/pc/net/net_netcnf.c', 'src/pc/net/net_peer.c',
             'src/pc/xbox/xnet_http.c']      # (the bug reporter sends reports itself when the network is up)
NET_INC = ['-I' + NXDK + '/lib/net/lwip/src/include', '-I' + NXDK + '/lib/net/nforceif/include', '-I' + NXDK + '/lib/net/nvnetdrv']
LIBS = ['xboxkrnl/libxboxkrnl.lib', 'libpdclib.lib', 'winmm.lib', 'libwinapi.lib', 'libnxdk_hal.lib', 'libnxdk.lib',
        'libnxdk_automount_d.lib', 'libpbkit.lib', 'nxdk_usb.lib', 'libxboxrt.lib', 'libzlib.lib', 'libSDL2.lib']

def title_identity(version):
    """title id and name from src/pc/xbox/xbox_title.h (the same constants the save code uses):
    the VERSION_MHG block for G, the #else block for MH1"""
    h = open('src/pc/xbox/xbox_title.h').read()
    g, e = h.index('#ifdef VERSION_MHG'), h.index('#else')
    h = h[g:e] if version == 'mhg' else h[e:]
    tid = int(re.search(r'XBOX_TITLE_ID\s+(0x[0-9A-Fa-f]+)', h).group(1), 16)
    name = re.search(r'XBOX_TITLE_NAME\s+"([^"]*)"', h).group(1)
    return tid, name

def patch_xbe_title_id(path, tid):
    """cxbe writes a fixed title id (0xFFFF0002) into the certificate; set ours.
    XBE header: base address at 0x104, certificate address at 0x118; the title id is
    at certificate +8. (No signature is checked by xemu or a softmodded console.)"""
    import struct
    b = bytearray(open(path, 'rb').read())
    assert b[:4] == b'XBEH'
    base, cert = struct.unpack_from('<II', b, 0x104)[0], struct.unpack_from('<I', b, 0x118)[0]
    off = cert - base
    struct.pack_into('<I', b, off + 8, tid)
    open(path, 'wb').write(b)

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
        if x == '-msse2':       # the recorded x86 commands use SSE2 for single-precision floats; the Xbox Pentium III has SSE1 only
            n.append('-msse')
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

# where gfx_nv2a.c uploads each program's constants: cgc's allocation must match
VP_CONSTS = {
    'vs': {'m_mvp': 0, 'm_tex': 4, 'fog_kb': 8, 'fog_col': 9, 'fade': 10, 'const': 11},
    'skin': {'m_mvp': 0, 'm_tex': 4, 'fog_kb': 8, 'fog_col': 9, 'ldir[0]': 10, 'lcol[0]': 13, 'amb': 16,
             'tint': 17, 'mode': 18, 'fade': 19, 'bones[0]': 20, 'const': 92},
}

def check_constants(name, text):
    import re
    want = VP_CONSTS.get(name)
    if not want:
        return
    got = {m.group(1): int(m.group(2)) for m in re.finditer(r'#var \S+ (\S+) :\s*: c\[(\d+)\]', text)}
    got.update({'const': int(m.group(1)) for m in re.finditer(r'#const c\[(\d+)\]', text)})
    bad = [k for k, v in want.items() if got.get(k) != v]
    if bad:
        sys.exit('build_xbox: %s: cgc put %s elsewhere than gfx_nv2a.c expects: %s'
                 % (name, ', '.join(bad), {k: got.get(k) for k in bad}))

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
        check_constants(name, open(tmp).read())
        inl = subprocess.run([NXDK + '/tools/%s/%s' % (conv, conv), tmp], check=True, capture_output=True, text=True).stdout
        open('%s/%s.inl' % (out, name), 'w').write(inl)
    return out

def main():
    global OUT, OBJ
    # --version mhg: Monster Hunter G. Its PC build (tools/build_pc_mhg.sh) lives in build/mhg/pctree, a
    # tree of symlinks to src/ include/ tools/ ... with its own build/pc; the same steps run there, the
    # output goes to build/mhg/xbox (through the tree's build/mhg link).
    version = sys.argv[sys.argv.index('--version') + 1] if '--version' in sys.argv else os.environ.get('VERSION', 'mh1')
    if version == 'mhg':
        if not os.path.isdir('build/mhg/pctree/build/pc'):
            sys.exit('build_xbox: no G PC build (build/mhg/pctree); run tools/build_pc_mhg.sh first')
        os.chdir('build/mhg/pctree')
        OUT = 'build/mhg/xbox'
        OBJ = OUT + '/obj'
    elif version != 'mh1':
        sys.exit('build_xbox: --version mh1 or mhg')
    # --online (MH1): the objects of an ONLINE=1 PC build (build/pc holds one variant at a time, build/pc/.variant),
    # compiled into their own tree build/xbox/online so the plain Xbox build's objects stay as they are
    online = '--online' in sys.argv
    exe = 'mhview'
    if online:
        if version != 'mh1':
            sys.exit('build_xbox: --online is MH1 only for now')
        exe = 'mhview_online'
        variant = open('build/pc/.variant').read().strip() if os.path.exists('build/pc/.variant') else ''
        if variant != exe:
            sys.exit('build_xbox: --online needs the ONLINE=1 PC build in build/pc (it holds %s); run ONLINE=1 tools/build_pc.sh'
                     % (variant or 'nothing'))
        OUT = 'build/xbox/online'
        OBJ = OUT + '/obj'
        FRONT.extend(NET_FRONT)
    iso_name = 'mhg.iso' if version == 'mhg' else 'mh1.iso'
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
    if 'rt_gen' not in linked or not os.path.exists('build/pc/' + exe) \
            or os.path.getmtime('build/pc/' + exe) < os.path.getmtime('build/pc/objs.txt'):
        sys.exit('build_xbox: the PC build did not finish (build/pc/objs.txt has no rt_gen.o or is newer '
                 'than build/pc/%s); run %stools/build_pc.sh first' % (exe, 'ONLINE=1 ' if online else ''))
    nocmd = [b for b in linked if b not in SKIP and not os.path.exists('build/pc/cmd/%s.sh' % b)]
    if nocmd:       # e.g. rt_gen (gen_rt_auto's tables) skipped -> dozens of undefined symbols
        sys.exit('build_xbox: no recorded compile command for %s; run tools/build_pc.sh (to the end) first'
                 % ' '.join(nocmd[:10]))
    for b in linked:
        c = 'build/pc/cmd/%s.sh' % b
        if b in SKIP:
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
        if b.startswith('mpeg2_'):             # libmpeg2: its MMX / MMXEXT paths (the P3 has both)
            args.insert(1, '-DARCH_X86')
        jobs.append((b, args + ['-include', COMPAT]))
        if b == 'mpeg2_idct':                  # and the MMX files the PC build does not compile
            for extra in ('idct_mmx', 'motion_comp_mmx'):
                a2 = [x.replace('third_party/libmpeg2/idct.c', 'third_party/libmpeg2/%s.c' % extra)
                       .replace('/mpeg2_idct.obj', '/mpeg2_%s.obj' % extra) for x in args]
                jobs.append(('mpeg2_' + extra, a2 + ['-include', COMPAT]))
    sdl = ['-I' + NXDK + '/lib/sdl/SDL2/include', '-DXBOX', '-DMH1_MPEG2_MMX', '-Ithird_party/libmpeg2'] + sdl_extra
    if online:
        sdl += ['-DMH1_ONLINE=1'] + NET_INC
    for f in FRONT + ['src/pc/rt/rt_memstat.c']:
        b = 'x_' + os.path.basename(f)[:-2]
        args = ['nxdk-cc', '-std=gnu99', '-O2', '-Iinclude', '-Isrc/pc'] + RELAX + sdl
        if version == 'mhg':
            args.append('-DVERSION_MHG')     # as tools/build_pc.sh's CFLAGS for VERSION=mhg
        if os.environ.get('MH1_RELEASE', '0') != '0':
            args.append('-DMH1_RELEASE')     # a release build: the debug menu off unless asked for (dbgmenu.c)
        if f != 'src/pc/rt/rt_memstat.c':
            args += ['-include', 'src/pc/rt/rt_memstat.h']
        if 'xbox_libc' not in f and 'xbox_log' not in f:
            args += ['-include', COMPAT]
        if 'kbm_xbox' in f:
            args += USB_INC
        jobs.append((b, args + ['-c', f, '-o', '%s/%s.obj' % (OBJ, b)]))
    for f in HID_SRC:
        src = open(NXDK + '/' + f).read()
        lines = src.split('\n')
        last = max(i for i, l in enumerate(lines) if l.startswith('#include'))
        lines.insert(last + 1, '#undef HID_DBGMSG\n#define HID_DBGMSG(...) ((void)0)')
        b = 'x_usb_' + os.path.basename(f)[:-2]
        cp = '%s/usbhid/%s' % (OUT, os.path.basename(f))
        os.makedirs(os.path.dirname(cp), exist_ok=True)
        open(cp, 'w').write('\n'.join(lines))
        jobs.append((b, ['nxdk-cc', '-O2', '-w'] + USB_INC + ['-I' + NXDK + '/lib/usb/libusbohci/src_hid', '-c', cp,
                         '-o', '%s/%s.obj' % (OBJ, b)]))
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
    # main thread stack: 256 KB. The PC build's deepest main-thread stack in the
    # three PC tests and the title / Rathian runs was 35 KB (RT_STACK=1); nxdk's
    # default is 64 KB.
    libs = [NXDK + '/lib/' + l for l in LIBS]
    if online:
        # lwIP's own tcp_init (its TCP module start) has the name of the game's tcp_init (lb_tcp01.c, the lobby
        # connection): a copy of nxdk's network library with lwIP's renamed (inside the library only)
        netlib = OUT + '/libnxdk_net_mh1.lib'
        subprocess.run(['llvm-objcopy', '--redefine-sym', '_tcp_init=_lwip_tcp_init', NXDK + '/lib/libnxdk_net.lib', netlib],
                       check=True)
        libs.insert(LIBS.index('libnxdk.lib'), netlib)
    link = ['nxdk-link', '-include:_automount_d_drive', '-stack:0x40000', '-out:' + dst + '/main.exe', '-map:' + dst + '/main.map'] \
        + objs + libs
    r = subprocess.run(link, capture_output=True, text=True)
    open(dst + '/link.log', 'w').write(r.stdout + r.stderr)
    if r.returncode:
        und = sorted(set(l.split('undefined symbol: ')[1] for l in (r.stdout + r.stderr).splitlines() if 'undefined symbol:' in l))
        print('link failed: %d undefined symbols (%s/link.log): %s' % (len(und), dst, ' '.join(und[:40])))
        sys.exit(1)
    tid, tname = title_identity(version)
    subprocess.run([NXDK + '/tools/cxbe/cxbe', '-OUT:' + dst + '/default.xbe', '-TITLE:' + tname, dst + '/main.exe'],
                   check=True, capture_output=True)
    patch_xbe_title_id(dst + '/default.xbe', tid)
    print('XBE certificate: title id %08X, title name "%s"' % (tid, tname))
    print('built %s/default.xbe (%d bytes)' % (dst, os.path.getsize(dst + '/default.xbe')))
    # an XISO holding only the XBE (the game's data files are not shipped; docs/xbox.md)
    iso_dir = dst + '/iso'
    os.makedirs(iso_dir, exist_ok=True)
    shutil.copy(dst + '/default.xbe', iso_dir + '/default.xbe')
    iso = dst + '/' + iso_name
    if os.path.exists(iso):
        os.remove(iso)
    subprocess.run([NXDK + '/tools/extract-xiso/build/extract-xiso', '-c', iso_dir, iso],
                   check=True, capture_output=True)
    print('built %s (%d bytes)' % (iso, os.path.getsize(iso)))

if __name__ == '__main__':
    main()
