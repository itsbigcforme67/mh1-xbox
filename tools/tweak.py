#!/usr/bin/env python3
"""tweak.py NM.c FUNC [--rounds N] [--apply]   (agent E helper)

Greedy hill climb over small, semantics-preserving source rewrites of ONE near-match function, scored with
tools/alignall.py on a mini file (the declarations of NM.c plus FUNC only, so each try compiles in about a second).
Rewrites tried at every site of the function body:
  * comparison forms     x >= K <-> x > K-1, x < K <-> x <= K-1 (and the reversed operand order)
  * commutative operands a + b, a * b, a | b, a & b, a == b, a != b with simple operands swapped
  * `(u32)` / `(int)` casts put on the first operand of an addition (the `(u32)x + (int)y` trick)
  * truth tests          !x <-> x == 0, x != 0 <-> x
  * adjacent single-line statements swapped
  * adjacent local declarations swapped
Prints every improvement; with --apply the best text is written back into NM.c. Not a proof of equivalence: a rewrite is
only kept when the compiled code gets closer to the original bytes."""
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
sys.path.insert(0, os.path.join(ROOT, "tools"))


def find_func(src, name):
    m = re.search(r'^(?:static\s+)?[A-Za-z_][\w \*]*?\b%s\s*\(' % re.escape(name), src, re.M)
    if not m:
        sys.exit("function not found")
    i = src.index("{", m.end())
    d = 0
    j = i
    while True:
        c = src[j]
        if c == "{":
            d += 1
        elif c == "}":
            d -= 1
            if d == 0:
                break
        j += 1
    return m.start(), j + 1, i


def score(path, name):
    r = subprocess.run(["python3", "tools/alignall.py", path], capture_output=True, text=True)
    for l in r.stdout.split("\n"):
        p = l.split()
        if len(p) >= 3 and p[1] == name:
            return 0 if p[0] == "OK" else int(p[2])
    return 9999


def mutations(body):
    """yield (description, new_body) for every single rewrite site in body"""
    out = []
    # comparisons with an integer literal on the right
    for m in re.finditer(r'(\b[\w\.\->\[\]\(\)]+?)\s*(>=|<=|>|<)\s*(0x[0-9A-Fa-f]+|\d+)\b', body):
        a, op, k = m.group(1), m.group(2), m.group(3)
        kv = int(k, 0)
        alt = {">=": (">", kv - 1), ">": (">=", kv + 1), "<=": ("<", kv + 1), "<": ("<=", kv - 1)}[op]
        ks = hex(alt[1]) if k.startswith("0x") and alt[1] >= 0 else str(alt[1])
        out.append(("cmp %s%s%s" % (a, op, k), body[:m.start()] + "%s %s %s" % (a, alt[0], ks) + body[m.end():]))
    # literal on the left variants: K > x etc.  x < K  -> K > x
    for m in re.finditer(r'(\b[\w\.\->\[\]]+)\s*(>=|<=|>|<)\s*(0x[0-9A-Fa-f]+|\d+)\b', body):
        a, op, k = m.group(1), m.group(2), m.group(3)
        flip = {">=": "<=", "<=": ">=", ">": "<", "<": ">"}[op]
        out.append(("flip %s%s%s" % (a, op, k), body[:m.start()] + "%s %s %s" % (k, flip, a) + body[m.end():]))
    # commutative swaps of simple operands
    simple = r'[A-Za-z_][\w\.\->\[\]]*|0x[0-9A-Fa-f]+|\d+'
    for m in re.finditer(r'(%s)\s*(\+|\*|\||&|\^|==|!=)\s*(%s)' % (simple, simple), body):
        pre = body[:m.start()].rstrip()
        post = body[m.end():].lstrip()
        if pre and pre[-1] not in "(=,[{;" and not pre.endswith("return") and not pre.endswith("&&") and not pre.endswith("||"):
            continue
        if post and post[0] not in ");,]":
            if not (post.startswith("&&") or post.startswith("||")):
                continue
        a, op, b = m.group(1), m.group(2), m.group(3)
        out.append(("swap %s%s%s" % (a, op, b), body[:m.start()] + "%s %s %s" % (b, op, a) + body[m.end():]))
    # casts on the first operand of an addition
    for m in re.finditer(r'(?<![\w\)\]])((?:\(\w+\s*\*?\))?[A-Za-z_][\w\.\->\[\]]*)\s*\+\s*((?:\(\w+\s*\*?\))?[A-Za-z_(][\w\.\->\[\]\)]*)', body):
        a, b = m.group(1), m.group(2)
        if a.startswith("("):
            continue
        for cast in ("(u32)", "(int)", "(s32)"):
            out.append(("cast %s%s" % (cast, a), body[:m.start()] + "%s%s + %s" % (cast, a, b) + body[m.end():]))
    # truth tests
    for m in re.finditer(r'\bif \(!([A-Za-z_][\w\.\->\[\]]*)\)', body):
        out.append(("!x", body[:m.start()] + "if (%s == 0)" % m.group(1) + body[m.end():]))
    for m in re.finditer(r'\bif \(([A-Za-z_][\w\.\->\[\]]*) == 0\)', body):
        out.append(("x==0", body[:m.start()] + "if (!%s)" % m.group(1) + body[m.end():]))
    for m in re.finditer(r'\bif \(([A-Za-z_][\w\.\->\[\]]*) != 0\)', body):
        out.append(("x!=0", body[:m.start()] + "if (%s)" % m.group(1) + body[m.end():]))
    # adjacent single-line statements / declarations
    lines = body.split("\n")
    for i in range(len(lines) - 1):
        a, b = lines[i], lines[i + 1]
        if re.match(r'^\s+[^{}\n]*;\s*$', a) and re.match(r'^\s+[^{}\n]*;\s*$', b) and not re.match(r'^\s*(return|break|continue|goto|case|default)\b', a):
            if a.strip().endswith(":;"):
                continue
            nl = lines[:i] + [b, a] + lines[i + 2:]
            out.append(("stmt swap %d" % i, "\n".join(nl)))
    return out


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    nm, name = args[0], args[1]
    rounds = 10
    if "--rounds" in sys.argv:
        rounds = int(sys.argv[sys.argv.index("--rounds") + 1])
    apply_ = "--apply" in sys.argv
    os.makedirs("build/tweak", exist_ok=True)
    mini = os.path.join("build/tweak", "zztw_%s.c" % name)
    subprocess.run(["python3", "tools/mkrun2.py", nm, mini, "tweak", name], check=True)
    try:
        src = open(mini).read()
        a, b, bi = find_func(src, name)
        body = src[bi:b]
        cur = score(mini, name)
        print("start", cur)
        sys.stdout.flush()
        best_text = body
        for r in range(rounds):
            if cur == 0:
                break
            cands = mutations(best_text)
            bestsc, bestc = cur, None
            seen = set()
            for desc, nb in cands:
                if nb in seen or nb == best_text:
                    continue
                seen.add(nb)
                open(mini, "w").write(src[:bi] + nb + src[b:])
                sc = score(mini, name)
                if sc < bestsc:
                    bestsc, bestc = sc, (desc, nb)
                    print("  round %d: %s -> %d" % (r, desc, sc))
                    sys.stdout.flush()
                    if sc == 0:
                        break
            if not bestc:
                break
            cur = bestsc
            best_text = bestc[1]
            src_full = src[:bi] + best_text + src[b:]
            # positions shift: recompute the body span for the next round
            src = src_full
            a, b, bi = find_func(src, name)
            best_text = src[bi:b]
        print("final", cur)
        if apply_ and cur < score_orig(nm, name):
            s = open(nm).read()
            a2, b2, bi2 = find_func(s, name)
            open(nm, "w").write(s[:bi2] + best_text + s[b2:])
            print("applied to", nm)
    finally:
        if os.path.exists(mini):
            os.remove(mini)


def score_orig(nm, name):
    return score(nm, name)


if __name__ == "__main__":
    main()
