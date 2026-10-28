#!/usr/bin/env python3
"""gs_dump_verts.py - list the vertices a PCSX2 GS dump (.gs, zstd-decompressed) sends to the GS, per GIF packet.

Used to check the ports' actor lighting against the PS2's own vertex colours (docs/agents/agent-D.md "Actor lighting"):
the VU1 programs output ST / RGBAQ / XYZF2 per vertex, so a dump of one frame holds every vertex colour the PS2 drew.

    zstd -d dump.gs.zst -o dump.gs
    python3 tools/gs_dump_verts.py dump.gs [--box X0,Y0,X1,Y1] [--path 1]
Prints, per GIF packet with vertices in the box (screen pixels of the frame), its path, vertex count, TEX0, and the
mean / min / max RGB. Standard library only; reads the user's own capture (never commit its output).
"""
import struct
import sys


def packets(d):
    assert struct.unpack_from('<I', d, 0)[0] == 0xFFFFFFFF, 'old dump format'
    hsize = struct.unpack_from('<I', d, 4)[0]
    state_size = struct.unpack_from('<I', d, 12)[0]
    o = 8 + hsize + state_size + 8192
    while o < len(d):
        t = d[o]
        o += 1
        if t == 0:
            path = d[o]
            n = struct.unpack_from('<I', d, o + 1)[0]
            yield path, d[o + 5:o + 5 + n]
            o += 5 + n
        elif t == 1:
            o += 1
        elif t == 2:
            o += 4
        elif t == 3:
            o += 8192
        else:
            raise ValueError('packet type %d at %#x' % (t, o - 1))


def gif_verts(buf):
    """yield (prim, tex0, rgba, (x, y, z), st, fog) for every vertex kick in a GIF stream (PACKED / REGLIST)"""
    o, prim, tex0, rgba, st = 0, 0, 0, (0, 0, 0, 0), (0.0, 0.0)
    while o + 16 <= len(buf):
        lo, hi = struct.unpack_from('<QQ', buf, o)
        o += 16
        nloop = lo & 0x7FFF
        pre = (lo >> 46) & 1
        flg = (lo >> 58) & 3
        nreg = (lo >> 60) & 15 or 16
        regs = [(hi >> (4 * i)) & 15 for i in range(nreg)]
        if pre:
            prim = (lo >> 47) & 0x7FF
        if flg == 0:
            for _ in range(nloop):
                for r in regs:
                    a, b, c, e = struct.unpack_from('<4I', buf, o)
                    q = buf[o:o + 16]
                    o += 16
                    if r == 0:
                        prim = a & 0x7FF
                    elif r == 1:
                        rgba = (a & 255, b & 255, c & 255, e & 255)
                    elif r == 2:
                        st = struct.unpack_from('<2f', q, 0)
                    elif r in (4, 5):        # XYZF2 / XYZF3
                        yield prim, tex0, rgba, ((a & 0xFFFF) / 16.0, (b & 0xFFFF) / 16.0, c >> 4), st, (e >> 4) & 255
                    elif r in (12, 13):      # XYZ3 ... (XYZ2 = 0xD? keep simple)
                        pass
                    elif r == 14:            # A+D
                        v, addr = struct.unpack_from('<QQ', q, 0)
                        addr &= 0xFF
                        if addr in (6, 7):
                            tex0 = v
                        elif addr == 0:
                            prim = v & 0x7FF
                        elif addr == 1:
                            rgba = (v & 255, (v >> 8) & 255, (v >> 16) & 255, (v >> 24) & 255)
                        elif addr in (4, 5, 0xC, 0xD):
                            yield prim, tex0, rgba, ((v & 0xFFFF) / 16.0, ((v >> 16) & 0xFFFF) / 16.0, v >> 32), st, 0
        elif flg == 1:
            o += 8 * nloop * nreg
            o = (o + 15) & ~15
        else:
            o += 16 * nloop
        if (lo >> 15) & 1 and False:
            break


def main(argv):
    path = argv[0]
    box = None
    want = None
    if '--box' in argv:
        box = [float(x) for x in argv[argv.index('--box') + 1].split(',')]
    if '--path' in argv:
        want = int(argv[argv.index('--path') + 1])
    d = open(path, 'rb').read()
    for i, (p, buf) in enumerate(packets(d)):
        if want is not None and p != want:
            continue
        vs = list(gif_verts(buf))
        if not vs:
            continue
        xs = [v for v in vs]
        if box:
            ox, oy = 2048 - 256, 2048 - 224     # (frame offset: the game's XYOFFSET centres 512 x 448 on 2048)
            xs = [v for v in vs if box[0] <= v[3][0] - ox <= box[2] and box[1] <= v[3][1] - oy <= box[3]]
        if not xs:
            continue
        n = len(xs)
        m = [sum(v[2][k] for v in xs) / n for k in range(4)]
        print('pkt %d path %d verts %d/%d tex0 %016x rgba mean %s min %s max %s z %d..%d fog %d..%d' % (
            i, p, n, len(vs), xs[0][1], ' '.join('%.1f' % x for x in m),
            tuple(min(v[2][k] for v in xs) for k in range(3)), tuple(max(v[2][k] for v in xs) for k in range(3)),
            min(v[3][2] for v in xs), max(v[3][2] for v in xs), min(v[5] for v in xs), max(v[5] for v in xs)))


if __name__ == '__main__':
    main(sys.argv[1:])
