#!/usr/bin/env python3
"""xbox_fatx.py - format and read the E: partition of an xemu hard disk image.

xemu's hard disk (xbox_files/hdd.img, a raw image; tools/run_xemu.sh makes a
blank sparse one) starts out unformatted, so the game has no E: drive: no
saves (E:\\UDATA), no log file. The retail Xbox disk has no partition table;
the kernel uses fixed offsets, E: (\\Device\\Harddisk0\\Partition1) is
0x1312D6000 bytes at 0xABE80000. This writes an empty FATX file system there
the way nxdk's nxFormatVolume (lib/nxdk/format.c) does: a 4 KB superblock,
the cluster table (16 KB clusters; 32-bit entries above 65519 clusters),
then the root directory cluster. Nothing else on the image is touched.

    tools/xbox_fatx.py format xbox_files/hdd.img      # E: empty (all data on E: lost)
    tools/xbox_fatx.py ls xbox_files/hdd.img [DIR]     # e.g. Games/MH1
    tools/xbox_fatx.py get xbox_files/hdd.img Games/MH1/mh1log.txt [OUT]
    FATX_PART=c tools/xbox_fatx.py format|ls|get|put ...   # the same on C: (Partition2)
    tools/xbox_fatx.py put xbox_files/hdd.img LOCALFILE Games/MH1/loader.txt [BYTES]
        (copies LOCALFILE, or only its first BYTES, streamed; replaces a file of that name, makes the
        folders; for test set-ups such as a half-copied game folder)

Run them only while xemu is not running (it caches the disk). Standard library only.
"""
import os, struct, sys, time

E_OFFSET, E_SIZE = 0xABE80000, 0x1312D6000
# FATX_PART=c: the C: partition (\\Device\\Harddisk0\\Partition2, 500 MB at 0x8CA80000; the dashboard's, C:\\xboxdash.xbe)
# instead of E: (tools/xemu_relaunch_test.sh puts a stand-in dashboard there)
if os.environ.get('FATX_PART', 'e').lower() == 'c':
    E_OFFSET, E_SIZE = 0x8CA80000, 0x1F400000
SECTOR, CLUSTER, PAGE = 512, 16384, 4096


def geometry():
    """cluster count, entry size, cluster-table bytes (as nxFormatVolume and the kernel compute them)"""
    n = E_SIZE // CLUSTER + 1
    ent = 2 if n < 65520 else 4
    table = (n * ent + PAGE - 1) & ~(PAGE - 1)
    return n, ent, table


def fmt(img):
    n, ent, table = geometry()
    with open(img, 'r+b') as f:
        f.seek(0, 2)
        if f.tell() < E_OFFSET + E_SIZE:
            sys.exit('%s is smaller than a retail Xbox disk (8 GB); no E: partition' % img)
        sb = bytearray(b'\xff' * PAGE)
        struct.pack_into('<4sIIIH', sb, 0, b'FATX', int(time.time()) & 0xFFFFFFFF, CLUSTER // SECTOR, 1, 0)
        f.seek(E_OFFSET)
        f.write(sb)
        fat = bytearray(table)
        if ent == 2:
            struct.pack_into('<HH', fat, 0, 0xFFF8, 0xFFFF)
        else:
            struct.pack_into('<II', fat, 0, 0xFFFFFFF8, 0xFFFFFFFF)
        f.write(fat)
        f.write(b'\xff' * CLUSTER)     # root directory: no entries
    print('E: formatted (FATX, %d clusters of 16 KB, %d-bit table)' % (n, ent * 8))


class Vol:
    def __init__(self, img, write=False):
        self.f = open(img, 'r+b' if write else 'rb')
        self.f.seek(E_OFFSET)
        sig, _vid, spc, root = struct.unpack('<4sIII', self.f.read(16))
        if sig != b'FATX':
            sys.exit('E: is not formatted (no FATX signature); see "format"')
        self.cl = spc * SECTOR
        self.n, self.ent, table = geometry()
        self.f.seek(E_OFFSET + PAGE)
        self.fat = bytearray(self.f.read(table))
        self.data = E_OFFSET + PAGE + table
        self.root = root

    def chain(self, c):
        end = 0xFFF0 if self.ent == 2 else 0xFFFFFFF0
        while 0 < c < end:
            yield c
            c = struct.unpack_from('<H' if self.ent == 2 else '<I', self.fat, c * self.ent)[0]

    def read(self, first, size=None):
        out = bytearray()
        for c in self.chain(first):
            self.f.seek(self.data + (c - 1) * self.cl)
            out += self.f.read(self.cl)
            if size is not None and len(out) >= size:
                break
        return bytes(out[:size]) if size is not None else bytes(out)

    def entries(self, first):
        d = self.read(first)
        for i in range(0, len(d), 64):
            ln, attr = d[i], d[i + 1]
            if ln in (0x00, 0xFF):
                break
            if ln == 0xE5 or ln > 42:
                continue
            name = d[i + 2:i + 2 + ln].decode('latin-1')
            c, size = struct.unpack_from('<II', d, i + 0x2C)
            yield name, attr, c, size

    def find(self, path):
        c, attr, size = self.root, 0x10, 0
        for part in [p for p in path.replace('\\', '/').split('/') if p]:
            for name, a, fc, sz in self.entries(c) if attr & 0x10 else ():
                if name.lower() == part.lower():
                    c, attr, size = fc, a, sz
                    break
            else:
                sys.exit('E:/%s: not found' % path)
        return c, attr, size


    # ---- writing (put) ----
    def setfat(self, c, v):
        struct.pack_into('<H' if self.ent == 2 else '<I', self.fat, c * self.ent, v)

    def alloc(self, n):
        out, end = [], (0xFFFF if self.ent == 2 else 0xFFFFFFFF)
        fmt_ = '<H' if self.ent == 2 else '<I'
        c = 1
        while len(out) < n:
            c += 1
            if c >= self.n:
                sys.exit('E: is full')
            if struct.unpack_from(fmt_, self.fat, c * self.ent)[0] == 0:
                out.append(c)
        for a, b in zip(out, out[1:] + [None]):
            self.setfat(a, b if b is not None else end)
        return out

    def free(self, first):
        for c in list(self.chain(first)):
            self.setfat(c, 0)

    def flush(self):
        self.f.seek(E_OFFSET + PAGE)
        self.f.write(self.fat)

    def entry(self, dirc, name, attr, first, size):
        """write (or replace) the entry NAME in the directory starting at cluster DIRC"""
        t = time.localtime()
        stamp = ((t.tm_year - 2000) << 25) | (t.tm_mon << 21) | (t.tm_mday << 16) | (t.tm_hour << 11) | \
            (t.tm_min << 5) | (t.tm_sec // 2)
        e = bytearray(b'\xff' * 64)
        nb = name.encode('latin-1')
        e[0], e[1] = len(nb), attr
        e[2:2 + len(nb)] = nb
        struct.pack_into('<IIIII', e, 0x2C, first, size, stamp, stamp, stamp)
        slot = None
        clusters = list(self.chain(dirc))
        for c in clusters:
            self.f.seek(self.data + (c - 1) * self.cl)
            d = self.f.read(self.cl)
            for i in range(0, self.cl, 64):
                ln = d[i]
                if ln <= 42 and ln not in (0, 0xE5) and d[i + 2:i + 2 + ln].decode('latin-1').lower() == name.lower():
                    slot = (c, i)
                    break
                if slot is None and ln in (0, 0xFF, 0xE5):
                    slot = (c, i)
                    if ln != 0xE5:
                        break
            else:
                continue
            break
        if slot is None:                        # directory full: one more cluster
            c = self.alloc(1)[0]
            self.setfat(clusters[-1], c)
            self.f.seek(self.data + (c - 1) * self.cl)
            self.f.write(b'\xff' * self.cl)
            slot = (c, 0)
        self.f.seek(self.data + (slot[0] - 1) * self.cl + slot[1])
        self.f.write(e)

    def put(self, src, dest, nbytes=None):
        parts = [p for p in dest.replace('\\', '/').split('/') if p]
        dirc = self.root
        for part in parts[:-1]:
            hit = [(fc, a) for name, a, fc, _ in self.entries(dirc) if name.lower() == part.lower()]
            if hit and hit[0][1] & 0x10:
                dirc = hit[0][0]
            else:
                c = self.alloc(1)[0]
                self.f.seek(self.data + (c - 1) * self.cl)
                self.f.write(b'\xff' * self.cl)
                self.entry(dirc, part, 0x10, c, 0)
                dirc = c
        for name, a, fc, _ in self.entries(dirc):
            if name.lower() == parts[-1].lower() and not a & 0x10 and fc:
                self.free(fc)
        size = os.path.getsize(src) if nbytes is None else min(nbytes, os.path.getsize(src))
        cl = self.alloc((size + self.cl - 1) // self.cl) if size else []
        with open(src, 'rb') as fh:
            for c in cl:
                self.f.seek(self.data + (c - 1) * self.cl)
                self.f.write(fh.read(min(self.cl, size - fh.tell())))
        self.entry(dirc, parts[-1], 0, cl[0] if cl else 0, size)
        self.flush()
        print('E:/%s: %d bytes' % ('/'.join(parts), size))


def main():
    if len(sys.argv) >= 5 and sys.argv[1] == 'put':
        return Vol(sys.argv[2], True).put(sys.argv[3], sys.argv[4], int(sys.argv[5]) if len(sys.argv) > 5 else None)
    if len(sys.argv) < 3 or sys.argv[1] not in ('format', 'ls', 'get'):
        sys.exit(__doc__)
    cmd, img = sys.argv[1], sys.argv[2]
    if cmd == 'format':
        return fmt(img)
    v = Vol(img)
    c, attr, size = v.find(sys.argv[3] if len(sys.argv) > 3 else '')
    if cmd == 'ls':
        if not attr & 0x10:
            sys.exit('not a directory')
        for name, a, _c, sz in v.entries(c):
            print('%s%s' % (name, '/' if a & 0x10 else '  %d' % sz))
        return
    if attr & 0x10:
        sys.exit('a directory')
    data = v.read(c, size)
    if len(sys.argv) > 4:
        open(sys.argv[4], 'wb').write(data)
    else:
        sys.stdout.buffer.write(data)


if __name__ == '__main__':
    main()
