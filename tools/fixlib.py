"""fixlib.py: helpers for hand-fix scripts on a ported near-match file (see tools/port_em.py).
rep(s, name, new)   replace the definition of function NAME (not its prototype) by the text NEW; returns the new text.
sub1(s, old, new)   str.replace that insists on exactly one hit.
ensure_spd(s)       add `u32 spd;` to every function that assigns spd without declaring it."""
import re
def rep(s, name, new):
    m = re.search(r'(?m)^(?:/\*KNR\*/\n)?(?:static )?(?:void|int|s32|u8|u16|s16|f32) %s\([^;{]*\) \{\n' % re.escape(name), s)
    if not m:
        raise SystemExit('rep: no definition of ' + name)
    b = s.index('\n}\n', m.start()) + 3
    return s[:m.start()] + new + s[b:]
def sub1(s, old, new):
    if s.count(old) != 1:
        raise SystemExit('sub1: %d hits for %r' % (s.count(old), old[:60]))
    return s.replace(old, new)
def ensure_spd(s):
    parts = re.split(r'(?m)^(?=(?:static )?(?:void|int|s32|u8|f32) \w+\([^;]*\) \{$)', s)
    out = []
    for part in parts:
        if re.search(r'\bspd = ', part) and not re.search(r'\bu32 spd;', part):
            i = part.index(') {\n') + 4
            part = part[:i] + '    u32 spd;\n' + part[i:]
        out.append(part)
    return ''.join(out)
