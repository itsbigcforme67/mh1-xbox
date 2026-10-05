#!/usr/bin/env python3
"""split_runs.py ALL.c OUT_PREFIX NAME:FIRST-LAST ...  (agent E helper)
ALL.c holds a whole source file's C (all functions, in address order). For each
NAME:FIRST-LAST argument, write OUT_PREFIXNAME.c containing the shared
declarations (everything outside function bodies) plus the functions from FIRST
to LAST (inclusive, in file order). Use it to extract the matching runs of a
file whose other functions are near-matches."""
import re, sys

def parse(src):
    lines = src.split('\n'); n = len(lines); funcs = []; i = 0
    while i < n:
        l = lines[i]
        m = re.match(r'^(?:\w[\w\s\*]*?)\b(\w+)\(([^;]*)\)\s*$', l)
        if m and not l.startswith(('extern', 'typedef', '#', 'static')) or (m and l.startswith('static')):
            j = i
            while j < n and lines[j] != '{':
                j += 1
            if j - i < 6 and all(';' not in lines[k] or k > i for k in range(i, j)):
                k = j
                while lines[k] != '}':
                    k += 1
                funcs.append((m.group(1), i, k)); i = k + 1; continue
        i += 1
    return lines, funcs

def main():
    allc, prefix = sys.argv[1], sys.argv[2]
    lines, funcs = parse(open(allc).read())
    inbody = set()
    for f in funcs:
        inbody.update(range(f[1], f[2] + 1))
    decl = [lines[k] for k in range(len(lines)) if k not in inbody]
    names = [f[0] for f in funcs]
    for spec in sys.argv[3:]:
        name, rng = spec.split(':'); first, last = rng.split('-')
        a, b = names.index(first), names.index(last)
        body = '\n\n'.join('\n'.join(lines[f[1]:f[2] + 1]) for f in funcs[a:b + 1])
        hdr = re.sub(r'\n{3,}', '\n\n', '\n'.join(decl)).rstrip() + '\n\n'
        out = prefix + name + '.c'
        open(out, 'w').write(hdr + body + '\n')
        print(out, [f[0] for f in funcs[a:b + 1]])

main()
