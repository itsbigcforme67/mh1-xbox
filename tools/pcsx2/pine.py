#!/usr/bin/env python3
"""Talk to a running PCSX2 through its PINE socket (tools/run_pcsx2.sh with PINE=SLOT).

PINE (github.com/GovanifY/pine) messages: u32 total size (LE, header included), u8 opcode, arguments;
the answer is u32 size, u8 result (0 = OK, 0xFF = fail), data. Addresses are EE addresses (the game's).

usage: pine.py [--slot N] CMD ...
  r8|r16|r32|rf ADDR [COUNT]    read (rf = 32-bit floats), COUNT values in a row
  w8|w16|w32|wf ADDR VALUE...   write VALUEs in a row
  save SLOT | load SLOT         PCSX2 save state slot (its sstates folder)
  status                        0 running, 1 paused, 2 shut down
  title                         the game's title
Several commands in one call: separate them with ';'  (e.g. "r32 0x34D450 ; w8 0x34DB88 1").
"""
import os
import socket
import struct
import sys

OPS = {'r8': (0, 'B'), 'r16': (1, 'H'), 'r32': (2, 'I'), 'r64': (3, 'Q'),
       'w8': (4, 'B'), 'w16': (5, 'H'), 'w32': (6, 'I'), 'w64': (7, 'Q')}


def sock_path(slot):
    d = os.environ.get('XDG_RUNTIME_DIR') or '/tmp'
    return os.path.join(d, 'pcsx2.sock' if slot == 28011 else 'pcsx2.sock.%d' % slot)


class Pine:
    def __init__(self, slot=28011):
        self.s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        self.s.settimeout(10)
        self.s.connect(sock_path(slot))

    def call(self, op, args=b''):
        self.s.sendall(struct.pack('<IB', 5 + len(args), op) + args)
        hdr = self._recv(4)
        n = struct.unpack('<I', hdr)[0]
        body = self._recv(n - 4)
        if body[0] != 0:
            raise RuntimeError('PINE op %d failed' % op)
        return body[1:]

    def _recv(self, n):
        b = b''
        while len(b) < n:
            c = self.s.recv(n - len(b))
            if not c:
                raise RuntimeError('PINE socket closed')
            b += c
        return b

    def read(self, kind, addr):
        op, f = OPS[kind]
        return struct.unpack('<' + f, self.call(op, struct.pack('<I', addr)))[0]

    def write(self, kind, addr, v):
        op, f = OPS[kind]
        self.call(op, struct.pack('<I', addr) + struct.pack('<' + f, v))

    def batch(self, msgs, reply_sizes):
        """Several ops in one PINE message (PCSX2 runs them in order): msgs = [(op, args)], returns the data parts."""
        body = b''.join(struct.pack('<B', op) + a for op, a in msgs)
        self.s.sendall(struct.pack('<I', 4 + len(body)) + body)
        n = struct.unpack('<I', self._recv(4))[0]
        data = self._recv(n - 4)
        if data[0] != 0:
            raise RuntimeError('PINE batch failed')
        out, i = [], 1
        for k in reply_sizes:
            out.append(data[i:i + k])
            i += k
        return out

    def read_bytes(self, addr, n, chunk=0x8000):
        """n bytes from addr (8-aligned reads of 64 bits, batched)."""
        a0, a1 = addr & ~7, (addr + n + 7) & ~7
        buf = b''
        for c in range(a0, a1, chunk):
            k = min(chunk, a1 - c) // 8
            buf += b''.join(self.batch([(3, struct.pack('<I', c + 8 * i)) for i in range(k)], [8] * k))
        return buf[addr - a0:addr - a0 + n]

    def read_many(self, ranges):
        """[(addr, n)] -> [bytes], all in one PINE message (addresses 8-aligned)."""
        ops, sizes = [], []
        for a, n in ranges:
            for i in range(0, (n + 7) & ~7, 8):
                ops.append((3, struct.pack('<I', a + i)))
                sizes.append(8)
        parts = self.batch(ops, sizes)
        out, j = [], 0
        for a, n in ranges:
            k = (n + 7) // 8
            out.append(b''.join(parts[j:j + k])[:n])
            j += k
        return out

    def write_bytes(self, addr, data):
        """data to addr, 32 bits at a time (4-aligned, length a multiple of 4), one message."""
        self.batch([(6, struct.pack('<II', addr + i, struct.unpack_from('<I', data, i)[0])) for i in range(0, len(data), 4)],
                   [0] * (len(data) // 4))

    def readf(self, addr):
        return struct.unpack('<f', struct.pack('<I', self.read('r32', addr)))[0]

    def writef(self, addr, v):
        self.write('w32', addr, struct.unpack('<I', struct.pack('<f', v))[0])

    def save(self, slot):
        self.call(9, struct.pack('<B', slot))

    def load(self, slot):
        self.call(10, struct.pack('<B', slot))

    def status(self):
        return struct.unpack('<I', self.call(15))[0]

    def title(self):
        b = self.call(11)
        return b[4:].split(b'\0')[0].decode('utf-8', 'replace')


def num(s):
    return float(s) if ('.' in s and not s.lower().startswith('0x')) else int(s, 0)


def run(p, words):
    c = words[0]
    if c in ('r8', 'r16', 'r32', 'rf'):
        a = int(words[1], 0)
        cnt = int(words[2], 0) if len(words) > 2 else 1
        size = {'r8': 1, 'r16': 2, 'r32': 4, 'rf': 4}[c]
        out = []
        for i in range(cnt):
            if c == 'rf':
                out.append('%g' % p.readf(a + i * 4))
            else:
                out.append('0x%X' % p.read(c, a + i * size))
        print('%08X: %s' % (a, ' '.join(out)))
    elif c in ('w8', 'w16', 'w32', 'wf'):
        a = int(words[1], 0)
        size = {'w8': 1, 'w16': 2, 'w32': 4, 'wf': 4}[c]
        for i, v in enumerate(words[2:]):
            if c == 'wf':
                p.writef(a + i * 4, float(v))
            else:
                p.write(c, a + i * size, int(v, 0) & ((1 << (8 * size)) - 1))
    elif c == 'save':
        p.save(int(words[1]))
    elif c == 'load':
        p.load(int(words[1]))
    elif c == 'status':
        print(p.status())
    elif c == 'title':
        print(p.title())
    else:
        raise SystemExit('unknown command ' + c)


def main(argv):
    slot = 28011
    if argv[:1] == ['--slot']:
        slot = int(argv[1])
        argv = argv[2:]
    if not argv:
        raise SystemExit(__doc__)
    p = Pine(slot)
    cmd = []
    for w in argv + [';']:
        if w == ';':
            if cmd:
                run(p, cmd)
            cmd = []
        else:
            cmd.extend(w.split())


if __name__ == '__main__':
    main(sys.argv[1:])
