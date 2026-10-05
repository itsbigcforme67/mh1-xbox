m2c-based pipeline used for the big monster AI files (em14/15/17/20/21), agent D.
NOT a polished tool: the scripts assume a scratch directory /tmp/claude-1000/w (context header ctx.c for m2c,
copies of the original asm files in asm/, drafts F.d2.c/F.d3.c) and are kept as a record of the steps:
  draft.py (DRAFT_CTX=ctx.c) -> fres.py (float/stack args from the asm) -> d2c2.py (struct, protos) -> pipe.py
  (generic clean-ups) -> bitfix.py -> postgen.py/postgen2.py (header, helpers) -> extraNN.py (per monster hand fixes)
  -> tools/alignall.py -> tools/mkruns3.py (runall.sh/relink.sh drive it) -> tools/rebuild.sh.
