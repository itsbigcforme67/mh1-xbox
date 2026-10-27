#!/usr/bin/env python3
"""codetab.py EXE (--map MAP | --nm NM) : write every function start of a linked Xbox / Windows build, file-static
functions included, into its rt_codetab array (src/pc/rt/rt_game.c), so rt_code_ptr can tell a real draw function from
a stray table word.

Why: on the Xbox (and the Windows build) the code sits at low addresses that G's raw PS2 addresses overlap. Until
27 Oct rt_code_ptr only accepted the starts of global symbols (tools/gen_symtab.py); a static draw function such as
set01_trans (the "村長の教えが追加されました" banner) was then skipped. Statics have no name another file could
reference, so the table is written after the link, from the linker's map (Xbox: lld-link -map, text is section 0001)
or the exe's symbol table (Windows: llvm-nm, types T / t).

The array starts with the words 'RTCO' 'DETB' capacity count; this tool fills count and the sorted addresses in place
(same size, so nothing moves). Standard library only."""
import re
import struct
import subprocess
import sys


def from_map(path):
    out = set()
    for ln in open(path, errors='replace'):
        p = ln.split()
        if len(p) >= 4 and p[0].startswith('0001:') and re.fullmatch(r'[0-9a-fA-F]{8,16}', p[2]):
            out.add(int(p[2], 16))
    return out


def from_nm(nm, exe):
    out = set()
    for ln in subprocess.run([nm, '--defined-only', exe], capture_output=True, text=True, check=True).stdout.splitlines():
        p = ln.split()
        if len(p) == 3 and p[1] in 'Tt' and not p[2].startswith('.'):    # not section symbols (.text)
            out.add(int(p[0], 16))
    return out


def main():
    if len(sys.argv) != 4 or sys.argv[2] not in ('--map', '--nm'):
        sys.exit(__doc__)
    exe = sys.argv[1]
    addrs = sorted(from_map(sys.argv[3]) if sys.argv[2] == '--map' else from_nm(sys.argv[3], exe))
    data = bytearray(open(exe, 'rb').read())
    magic = struct.pack('<II', 0x4F435452, 0x42544544)
    at = data.find(magic)
    if at < 0 or data.find(magic, at + 1) >= 0:
        sys.exit('codetab: rt_codetab not found exactly once in %s' % exe)
    cap, n = struct.unpack_from('<II', data, at + 8)
    if not addrs:
        sys.exit('codetab: no function addresses for %s' % exe)
    if len(addrs) > cap:
        sys.exit('codetab: %d functions, rt_codetab holds %d (raise RT_CODETAB_CAP in rt_game.c)' % (len(addrs), cap))
    struct.pack_into('<I%dI' % len(addrs), data, at + 12, len(addrs), *addrs)
    open(exe, 'wb').write(data)
    print('codetab: %d function starts written into %s (capacity %d)' % (len(addrs), exe, cap))


if __name__ == '__main__':
    main()
