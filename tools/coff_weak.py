#!/usr/bin/env python3
"""coff_weak.py - GNU-ld weak-symbol rules for lld-link (the Xbox build).

GNU ld takes the first strong definition of a symbol, else the first weak
one, and quietly drops the others. lld-link (18) reports "duplicate symbol"
when two COFF weak definitions of one name meet before a strong one. So,
for the objects in link order: pick the winner as GNU ld would, and in every
other object that defines the symbol weakly, turn its weak external into a
plain undefined reference. That object's own calls then go to the winner,
as on ELF; its copy of the code stays in the object under the
".weak.NAME.default.X" name and is unused.

    import coff_weak; coff_weak.resolve(objs_in_link_order, out_dir) -> new list
Standard library only.
"""
import os, struct

WEAK_EXTERNAL, EXTERNAL, STATIC = 105, 2, 3

def _symbols(data):
    mach, nsec, _, symptr, nsym = struct.unpack_from('<HHIII', data, 0)
    if mach != 0x14C:
        raise ValueError('not an i386 COFF object')
    strtab = symptr + nsym * 18
    i = 0
    while i < nsym:
        off = symptr + i * 18
        raw, value, sec, typ, cls, naux = struct.unpack_from('<8sIhHBB', data, off)
        if raw[:4] == b'\0\0\0\0':
            so = strtab + struct.unpack_from('<I', raw, 4)[0]
            name = data[so:data.index(b'\0', so)].decode()
        else:
            name = raw.rstrip(b'\0').decode()
        yield i, off, name, sec, cls, naux
        i += 1 + naux

def definitions(data):
    """name -> 'T' (strong) or 'W' (weak external with a default)"""
    out = {}
    for i, off, name, sec, cls, naux in _symbols(data):
        if cls == EXTERNAL and sec != 0:
            out[name] = 'T'
        elif cls == WEAK_EXTERNAL and naux:
            out.setdefault(name, 'W')
    return out

def unweaken(data, names):
    """weak externals NAMES -> undefined externals (the aux slot becomes an
    absolute static symbol, so symbol indices do not move)"""
    b = bytearray(data)
    for i, off, name, sec, cls, naux in list(_symbols(data)):
        if cls == WEAK_EXTERNAL and name in names:
            struct.pack_into('<IhHBB', b, off + 8, 0, 0, 0x20, EXTERNAL, 0)
            for k in range(naux):
                struct.pack_into('<8sIhHBB', b, off + 18 * (k + 1), b'$xweak', 0, -1, 0, STATIC, 0)
    return bytes(b)

def resolve(objs, out_dir):
    defs = []
    for o in objs:
        defs.append(definitions(open(o, 'rb').read()))
    winner = {}
    for o, d in zip(objs, defs):
        for n, k in d.items():
            w = winner.get(n)
            if w is None or (k == 'T' and w[1] == 'W'):
                winner[n] = (o, k)
    res = []
    os.makedirs(out_dir, exist_ok=True)
    nfix = 0
    for o, d in zip(objs, defs):
        lose = {n for n, k in d.items() if k == 'W' and winner[n][0] != o}
        if not lose:
            res.append(o)
            continue
        p = os.path.join(out_dir, os.path.basename(o))
        open(p, 'wb').write(unweaken(open(o, 'rb').read(), lose))
        res.append(p)
        nfix += len(lose)
    return res, nfix
