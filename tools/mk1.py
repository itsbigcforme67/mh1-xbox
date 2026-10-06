#!/usr/bin/env python3
"""mk1.py NM.c OUT.c "header comment" func [func...]: write OUT.c holding only the named function
definitions of NM.c plus every top-level non-function chunk (includes, typedefs, externs, prototypes,
defines), found by brace matching. Unlike mkrun2.py it copes with comments inside signatures.
Static helpers must be listed by name too. Then run check.py on OUT.c."""
import re, sys
nm, out, hdr = sys.argv[1:4]
want = set(sys.argv[4:])
s = open(nm).read()
i, n = 0, len(s)
chunks = []          # (kind, name, text)
cur = i
def skip_ws(j):
    while j < n and s[j] in ' \t\r\n':
        j += 1
    return j
while i < n:
    j = skip_ws(i)
    if j >= n:
        break
    start = j
    # a top-level item: comment, preprocessor line, or declaration/definition up to ';' or matching '}'
    if s.startswith('/*', j):
        e = s.index('*/', j) + 2
        chunks.append(('comment', None, s[start:e])); i = e; continue
    if s[j] == '#':
        e = j
        while True:
            e = s.index('\n', e)
            if s[e-1] == '\\':
                e += 1; continue
            break
        chunks.append(('pp', None, s[start:e])); i = e; continue
    # scan to ';' or '{' at depth 0 (skipping comments/strings)
    k = j; depth = 0; kind = None
    while k < n:
        c = s[k]
        if s.startswith('/*', k):
            k = s.index('*/', k) + 2; continue
        if s.startswith('//', k):
            k = s.index('\n', k); continue
        if c == '"':
            k += 1
            while s[k] != '"':
                k += 2 if s[k] == '\\' else 1
            k += 1; continue
        if c == "'":
            k += 1
            while s[k] != "'":
                k += 2 if s[k] == '\\' else 1
            k += 1; continue
        if c == '{':
            depth += 1
        elif c == '}':
            depth -= 1
            if depth == 0:
                # end of a brace group; if the item is a struct/typedef/array init it ends at ';'
                head = s[start:k]
                if re.search(r'\)\s*(\n[\w \*;,\[\]]+;\n)*\s*\{', head.split('{')[0] + '{') and not re.match(r'\s*(typedef|struct|union|enum|extern|static\s+\w[\w \*]*\[|\w[\w \*]*\[[^\]]*\]\s*=)', head):
                    kind = 'func'; k += 1; break
        elif c == ';' and depth == 0:
            kind = 'decl'; k += 1; break
        k += 1
    text = s[start:k]
    if kind == 'func':
        head = text.split('{')[0]
        m = re.findall(r'(\w+)\s*\(', head)
        name = m[0] if m else None
        chunks.append(('func', name, text))
    else:
        chunks.append(('decl', None, text))
    i = k
res = [] ; pend = []
for kind, name, text in chunks:
    if kind == 'comment':
        pend.append(text); continue
    if kind == 'func':
        if name in want:
            res.extend(pend); res.append(text)
        pend = []
        continue
    res.extend(pend); pend = []
    res.append(text)
open(out, 'w').write('/* ' + hdr + ' */\n' + '\n'.join(res) + '\n')
