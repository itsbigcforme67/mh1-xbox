#!/usr/bin/env python3
"""hexf.py FILE FUNC - turn `<f32 lvalue> = 0x4xxxxxxx;` in FUNC into float literals
(lvalues recognised: pl->pos[N], pl->wNNN fields of type f32 are found by the caller regex)."""
import re, struct, sys

def f2s(w):
    f = struct.unpack("<f", struct.pack("<I", w))[0]
    if f == int(f) and abs(f) < 1e7:
        return "%d.0f" % int(f)
    for prec in range(1, 10):
        t = "%.*g" % (prec, f)
        if struct.unpack("<I", struct.pack("<f", float(t)))[0] == w:
            break
    if "e" not in t and "." not in t:
        t += ".0"
    return t + "f"

def conv(text):
    def rep(m):
        w = int(m.group(2), 16)
        return "%s = %s;" % (m.group(1), f2s(w))
    return re.sub(r"((?:\w+->)\w+(?:\[\d+\])?|PF32\([^;]*?\)) = (0x[0-9A-Fa-f]{8});", rep, text)

if __name__ == "__main__":
    print(conv(open(sys.argv[1]).read()))
