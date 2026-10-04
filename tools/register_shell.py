#!/usr/bin/env python3
"""register_shell.py NN - register a hand-finished src/game/shell/shellNN.c:
check it, add its text range and its move-switch jump table data slot."""
import os
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import gen_shell  # noqa: E402

ROOT = gen_shell.ROOT
for nn in sys.argv[1:]:
    _cases, (tbl, a, sz) = gen_shell.move_cases(nn, gen_shell.symbols())
    r = subprocess.run(["python3", os.path.join(ROOT, "tools/check.py"),
                        "src/game/shell/shell%s.c" % nn, "--add", "game", "shell/shell%s" % nn],
                       capture_output=True, text=True, cwd=ROOT)
    print(r.stdout.strip().splitlines()[-1])
    if r.returncode == 0:
        with open(os.path.join(ROOT, "config/c_files.txt"), "a") as f:
            f.write("game:rodata 0x%08X 0x%08X shell/shell%s\n" % (a, a + sz, nn))
