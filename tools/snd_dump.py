#!/usr/bin/env python3
"""
snd_dump.py - decode MH1 (PS2) sound data to .wav for checking.

Formats (docs/formats/audio.md):
  AFS01.AFS  .snd / .snp sound packs: "MOMO" container holding an SCEI
             HD (instrument header), BD (PS2 ADPCM body) and, for .snd,
             a "TSDR"/"Tseq" table and a "TSBD" sound-effect table.
  AFS00.AFS  .adx music / ambience streams (CRI ADX, 4-bit), .sfd movies.

Output goes to build/audio/ (gitignored): never commit decoded audio.

Usage:
  python3 tools/snd_dump.py --list                 every pack: programs, codes
  python3 tools/snd_dump.py --pack 6               pack 6 (snd_map): codes.txt + one wav per SE code
  python3 tools/snd_dump.py --pack 6 --vags        ... + one wav per raw VAG
  python3 tools/snd_dump.py --adx 57 --seconds 20  AFS00 entry 57 (M6_CAMP1) -> wav
  python3 tools/snd_dump.py --adx-list             ADX headers (rate, length, loop)
"""
import argparse
import math
import os
import struct
import sys
import wave

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
from afs_extract import read_table  # noqa: E402

DISC = os.path.join(ROOT, "disc/mh1")
OUT = os.path.join(ROOT, "build/audio")


def u32(d, o):
    return struct.unpack_from("<I", d, o)[0]


def u16(d, o):
    return struct.unpack_from("<H", d, o)[0]


class Afs:
    def __init__(self, name):
        path = os.path.join(DISC, name)
        self.f = open(path, "rb")
        self.entries, self.names = read_table(self.f, os.path.getsize(path))

    def read(self, i, n=None):
        off, size = self.entries[i]
        self.f.seek(off)
        return self.f.read(size if n is None else min(n, size))


# ---------------------------------------------------------------- packs

class Pack:
    """One MOMO pack: sections [HD, BD, (Tseq, TSBD)]."""

    def __init__(self, d):
        if d[:4] != b"MOMO":
            raise ValueError("not a MOMO pack")
        self.d = d
        n = u32(d, 4)
        self.sec = [(u32(d, 8 + 8 * i), u32(d, 12 + 8 * i)) for i in range(n)]
        self.hd = self.sec[0][0]
        self.bd = self.sec[1][0]
        head = self.hd + 0x10                  # "SCEIHead" chunk
        assert d[head:head + 8] == b"IECSdaeH", "no SCEIHead"
        self.bd_size = u32(d, head + 0x10)
        prog, sset, smpl, vagi = (u32(d, head + 0x14 + 4 * k) for k in range(4))
        self.prog = self._table(prog, b"IECSgorP")
        self.sset = self._table(sset, b"IECStesS")
        self.smpl = self._table(smpl, b"IECSlpmS")
        self.vagi = self._table(vagi, b"IECSigaV")
        self.tsbd = []
        if n >= 4:
            t, size = self.sec[3]
            assert d[t:t + 4] == b"TSBD"
            for k in range((size - 16) // 16):
                self.tsbd.append(d[t + 16 + 16 * k:t + 32 + 16 * k])

    def _table(self, off, magic):
        d = self.d
        c = self.hd + off
        assert d[c:c + 8] == magic, (magic, d[c:c + 8])
        n = u32(d, c + 12) + 1
        out = []
        for i in range(n):
            o = u32(d, c + 16 + 4 * i)
            out.append(None if o == 0xFFFFFFFF else c + o)
        return out

    def vag(self, i):
        """(bd offset, rate, loop attr) of VAG i."""
        o = self.vagi[i]
        return u32(self.d, o), u16(self.d, o + 4), self.d[o + 6]

    def splits(self, p):
        o = self.prog[p]
        if o is None:
            return []
        sb, ns, sz = u32(self.d, o), self.d[o + 4], self.d[o + 5]
        out = []
        for k in range(ns):
            s = o + sb + k * sz
            out.append(dict(sset=u16(self.d, s), lo=self.d[s + 2], hi=self.d[s + 4],
                            vol=self.d[s + 16], pan=self.d[s + 17],
                            transpose=struct.unpack_from("b", self.d, s + 18)[0]))
        return out

    def sample(self, i):
        o = self.smpl[i]
        d = self.d
        return dict(vag=u16(d, o), base=d[o + 11], detune=struct.unpack_from("b", d, o + 12)[0],
                    pan=d[o + 13], vol=d[o + 16])

    def sset_samples(self, i):
        o = self.sset[i]
        n = self.d[o + 3]
        return [u16(self.d, o + 4 + 2 * k) for k in range(n)]

    def resolve(self, prog, note):
        """program + note -> (vag index, pitch ratio, volume 0..1) or None."""
        if prog >= len(self.prog):
            return None
        for sp in self.splits(prog):
            if sp["lo"] <= note <= sp["hi"]:
                smp = self.sample(self.sset_samples(sp["sset"])[0])
                semis = note - smp["base"] + smp["detune"] / 128.0
                vol = (sp["vol"] / 127.0) * (smp["vol"] / 127.0)
                return smp["vag"], 2 ** (semis / 12.0), vol
        return None


VAG_COEF = [(0, 0), (60, 0), (115, -52), (98, -55), (122, -60)]


def decode_vag(d, off, end=None, max_frames=1 << 20):
    """PS2 SPU ADPCM from d[off:] until the end flag; returns (pcm list, loop start sample or None)."""
    h1 = h2 = 0
    out = []
    loop = None
    if end is None:
        end = len(d)
    for f in range(max_frames):
        p = off + 16 * f
        if p + 16 > end:
            break
        sf, flags = d[p], d[p + 1]
        shift, filt = sf & 0xF, (sf >> 4) & 0xF
        c0, c1 = VAG_COEF[filt] if filt < 5 else (0, 0)
        if flags & 4:
            loop = len(out)
        for b in d[p + 2:p + 16]:
            for nib in (b & 0xF, b >> 4):
                s = (nib << 12) & 0xFFFF
                if s & 0x8000:
                    s -= 0x10000
                s = (s >> shift) + ((h1 * c0 + h2 * c1) >> 6)
                s = max(-32768, min(32767, s))
                out.append(s)
                h2, h1 = h1, s
        if flags & 1:                      # end of sample (1 = end, 3 = loop end)
            if not (flags & 2):
                loop = None
            break
    return out, loop


def write_wav(path, rate, chans, pcm):
    with wave.open(path, "wb") as w:
        w.setnchannels(chans)
        w.setsampwidth(2)
        w.setframerate(rate)
        w.writeframes(struct.pack("<%dh" % len(pcm), *pcm))


def stats(pcm):
    if not pcm:
        return "empty"
    peak = max(abs(x) for x in pcm)
    rms = math.sqrt(sum(x * x for x in pcm) / len(pcm))
    return "peak %d rms %.0f" % (peak, rms)


def pack_dump(afs, i, vags, outdir):
    pk = Pack(afs.read(i))
    name = os.path.splitext(afs.names[i])[0]
    od = os.path.join(outdir, name)
    os.makedirs(od, exist_ok=True)
    lines = ["%s: %d vags, programs %s, %d TSBD codes" % (
        afs.names[i], len(pk.vagi), [p for p, o in enumerate(pk.prog) if o is not None],
        sum(1 for e in pk.tsbd if e[0] != 0xFF))]
    offs = sorted(set(pk.vag(k)[0] for k in range(len(pk.vagi)) if pk.vagi[k] is not None))
    offs.append(pk.bd_size)

    def vag_pcm(k):
        off, rate, attr = pk.vag(k)
        nxt = offs[offs.index(off) + 1]
        pcm, loop = decode_vag(pk.d, pk.bd + off, pk.bd + nxt)
        return pcm, rate, loop

    if vags:
        for k in range(len(pk.vagi)):
            if pk.vagi[k] is None:
                continue
            pcm, rate, loop = vag_pcm(k)
            write_wav(os.path.join(od, "vag%03d.wav" % k), rate, 1, pcm)
            lines.append("vag %3d rate %5d %6.2fs loop %s %s" % (
                k, rate, len(pcm) / rate, loop, stats(pcm)))
    for c, e in enumerate(pk.tsbd):
        if e[0] == 0xFF:
            continue
        prog, note = e[2], e[3]
        r = pk.resolve(prog, note)
        desc = "code 0x%02X prog %d note 0x%02X vol %3d pan %3d vrand %d prand %d next %s flags %02X/%02X" % (
            c, prog, note, e[8], e[9], e[12], e[13], "-" if e[15] == 0xFF else "0x%02X" % e[15], e[6], e[7])
        if r is None:
            lines.append(desc + "  (program not in this pack: merged at run time)")
            continue
        k, ratio, vol = r
        pcm, rate, loop = vag_pcm(k)
        write_wav(os.path.join(od, "code%02X.wav" % c), int(rate * ratio), 1, pcm)
        lines.append(desc + "  -> vag %d rate %d x%.3f %.2fs loop %s %s" % (
            k, rate, ratio, len(pcm) / rate / ratio, loop, stats(pcm)))
    open(os.path.join(od, "codes.txt"), "w").write("\n".join(lines) + "\n")
    print("\n".join(lines[:60]))
    print("-> %s" % od)


# ---------------------------------------------------------------- ADX

def adx_header(h):
    if h[0] != 0x80 or h[4] != 3:
        raise ValueError("not a 4-bit ADX")
    data = struct.unpack(">H", h[2:4])[0] + 4
    ch, rate, total = h[7], struct.unpack(">I", h[8:12])[0], struct.unpack(">I", h[12:16])[0]
    cutoff, ver = struct.unpack(">H", h[16:18])[0], h[18]
    loop = None
    base = 0x14 if ver == 3 else 0x20
    if data >= base + 0x18:
        w = struct.unpack(">6I", h[base:base + 0x18])
        if w[1] == 1:
            loop = (w[2], w[3], w[4], w[5])   # start sample, start byte, end sample, end byte
    return dict(data=data, ch=ch, rate=rate, total=total, cutoff=cutoff, ver=ver,
                block=h[5], loop=loop, enc=h[0x13])


def adx_coefs(cutoff, rate):
    z = math.cos(2.0 * math.pi * cutoff / rate)
    a = math.sqrt(2.0) - z
    b = math.sqrt(2.0) - 1.0
    c = (a - math.sqrt((a + b) * (a - b))) / b
    return int(c * 8192), int(c * c * -4096)


def decode_adx(d, hd, nsamples):
    c1, c2 = adx_coefs(hd["cutoff"], hd["rate"])
    ch, bs = hd["ch"], hd["block"]
    hist = [[0, 0] for _ in range(ch)]
    out = []
    p = hd["data"]
    frames = (min(nsamples, hd["total"]) + 31) // 32
    for _ in range(frames):
        blk = []
        for c in range(ch):
            fr = d[p:p + bs]
            p += bs
            if len(fr) < bs:
                return out
            scale = struct.unpack(">H", fr[:2])[0] + 1
            h1, h2 = hist[c]
            s = []
            for b in fr[2:]:
                for nib in (b >> 4, b & 0xF):
                    v = nib - 16 if nib & 8 else nib
                    v = v * scale + ((c1 * h1 + c2 * h2) >> 12)
                    v = max(-32768, min(32767, v))
                    s.append(v)
                    h2, h1 = h1, v
            hist[c] = [h1, h2]
            blk.append(s)
        for k in range(32):
            for c in range(ch):
                out.append(blk[c][k])
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--list", action="store_true")
    ap.add_argument("--pack", type=int)
    ap.add_argument("--vags", action="store_true")
    ap.add_argument("--adx", type=int)
    ap.add_argument("--adx-list", action="store_true")
    ap.add_argument("--seconds", type=float, default=15.0)
    ap.add_argument("-o", default=OUT)
    a = ap.parse_args()
    os.makedirs(a.o, exist_ok=True)
    if a.list:
        afs = Afs("AFS01.AFS")
        for i in range(len(afs.entries)):
            pk = Pack(afs.read(i))
            print("%3d %-20s vags %3d bd %7d programs %s codes %d" % (
                i, afs.names[i], len(pk.vagi), pk.bd_size,
                [p for p, o in enumerate(pk.prog) if o is not None],
                sum(1 for e in pk.tsbd if e[0] != 0xFF)))
    if a.pack is not None:
        pack_dump(Afs("AFS01.AFS"), a.pack, a.vags, a.o)
    if a.adx_list:
        afs = Afs("AFS00.AFS")
        for i, n in enumerate(afs.names):
            if not n.endswith(".adx"):
                continue
            h = adx_header(afs.read(i, 0x800))
            print("%3d %-13s v%d ch %d %5d Hz %7.1fs loop %s" % (
                i, n, h["ver"], h["ch"], h["rate"], h["total"] / h["rate"],
                "-" if h["loop"] is None else "%.1fs..%.1fs" % (h["loop"][0] / h["rate"], h["loop"][2] / h["rate"])))
    if a.adx is not None:
        afs = Afs("AFS00.AFS")
        n = int(a.seconds * 48000)
        d = afs.read(a.adx, 0x1000 + n * 18 // 16 + 0x1000)
        h = adx_header(d)
        pcm = decode_adx(d, h, int(a.seconds * h["rate"]))
        path = os.path.join(a.o, os.path.splitext(afs.names[a.adx])[0] + ".wav")
        write_wav(path, h["rate"], h["ch"], pcm)
        print("%s: %d Hz %d ch, %.1fs of %.1fs, %s -> %s" % (
            afs.names[a.adx], h["rate"], h["ch"], len(pcm) / h["ch"] / h["rate"],
            h["total"] / h["rate"], stats(pcm), path))


if __name__ == "__main__":
    main()
