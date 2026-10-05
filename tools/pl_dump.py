#!/usr/bin/env python3
"""
pl_dump.py - assemble an MH1 hunter (6 armour parts) and pose it, to .obj.

Proof that the player model setup in docs/formats/player.md is understood.
Standard library only. Reads the user's AFS_DATA.AFS and main.bin (for the
part -> skeleton bone tables); writes only where -o points (use build/).

How the game does it (addresses in docs/formats/player.md):
  armor_create_model (0x124310) loads six parts per hunter, slot order
  0 reg (legs), 1 face, 2 hair, 3 body, 4 arm, 5 wst (waist), file names
  {m,f}_{reg,face,hair,body,arm,wst}NNN_amh.bin (tables armor_model_m/f).
  Each part has its own small AHI skeleton. Only slot 0 (legs) is animated:
  its 21-bone AHI is the master skeleton. SetPartsTrans (0x163E40)
  gives every bone of the other parts the world matrix of a master bone,
  looked up in ptmat_tbl[slot][bone] (read here from main.bin).

Usage:
    python3 tools/pl_dump.py disc/mh1/AFS_DATA.AFS -o build/pl/hunter.obj
    python3 tools/pl_dump.py disc/mh1/AFS_DATA.AFS --sex f --parts 3,1,2,3,3,3 \\
        --motion 1,101 --frame 10 -o build/pl/f.obj
"""
import argparse
import os
import re
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import clay_dump as cd  # noqa: E402

SLOTS = ["reg", "face", "hair", "body", "arm", "wst"]
MAIN_BASE = 0x100000


def symbols(path):
    sym = {}
    for line in open(path):
        m = re.match(r"(\w+) = 0x([0-9A-Fa-f]+);", line)
        if m:
            sym[m.group(1)] = int(m.group(2), 16)
    return sym


def ptmat_tables(main_bin, sym):
    """ptmat_tbl: 6 pointers (one per slot) to s16 arrays indexed by the
    part's bone number: >0 and <64 = take master bone N's world matrix;
    >=64 = own animation under a master-relative rotation (hair/cloth);
    -1 = no master bone (the part root)."""
    d = open(main_bin, "rb").read()
    ptrs = struct.unpack_from("<6I", d, sym["ptmat_tbl"] - MAIN_BASE)
    return [lambda b, p=p: struct.unpack_from("<h", d, p - MAIN_BASE + 2 * b)[0] for p in ptrs]


def load_part(afs, name):
    raw = cd.read_entry(afs, name)[1]
    d = cd.melt(raw)
    (ao, asz), (ho, hs) = cd.link_entries(d)[:2]
    return d[ao:ao + asz], d[ho:ho + hs]


def main():
    here = os.path.dirname(os.path.abspath(__file__))
    root = os.path.dirname(here)
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("afs")
    ap.add_argument("-o", "--out", required=True, help="output .obj (under build/)")
    ap.add_argument("--sex", choices="mf", default="m")
    ap.add_argument("--parts", default="1,0,1,1,1,1",
                    help="model numbers for reg,face,hair,body,arm,wst (default 1,0,1,1,1,1)")
    ap.add_argument("--motion", default="1,101",
                    help="motion ids for the legs' groups 0 and 1 (default 1,101; "
                         "bank = id/100, slot = id%%100 of plcom_tbl.bin)")
    ap.add_argument("--frame", type=float, default=0.0)
    ap.add_argument("--tbl", default="plcom_tbl.bin")
    ap.add_argument("--main", default=os.path.join(root, "disc/mh1/split/main.bin"))
    ap.add_argument("--symbols", default=os.path.join(root, "config/symbols/main.txt"))
    ap.add_argument("--bind", action="store_true", help="no motion: bind pose")
    args = ap.parse_args()

    nums = [int(x) for x in args.parts.split(",")]
    ids = {g: int(x) for g, x in enumerate(args.motion.split(","))}
    ptmat = ptmat_tables(args.main, symbols(args.symbols))
    tbl = cd.melt(cd.read_entry(args.afs, args.tbl)[1])

    parts = []
    for slot, n in enumerate(nums):
        nm = "%s_%s%03d" % (args.sex, SLOTS[slot], n)
        amo, ahi = load_part(args.afs, nm + "_amh.bin")
        try:
            tex = cd.melt(cd.read_entry(args.afs, nm + ".apx")[1])
        except ValueError:
            tex = None
        parts.append((nm, amo, cd.read_ahi(ahi), tex))

    # master skeleton = the legs part (slot 0), posed by the motion
    mbones = parts[0][2]
    mbind = [list(b[2]) + list(b[3]) + list(b[4]) for b in mbones]
    mpose = mbind if args.bind else cd.motion_channels(mbones, tbl, ids, args.frame)
    mworld = cd.world_matrices(mbones, mpose)

    stem = os.path.splitext(args.out)[0]
    os.makedirs(os.path.dirname(os.path.abspath(args.out)), exist_ok=True)
    obj = ["# MH1 hunter by tools/pl_dump.py", "mtllib %s.mtl" % os.path.basename(stem)]
    mtl = []
    base = vtbase = 1
    for slot, (nm, amo, bones, tex) in enumerate(parts):
        bind = [list(b[2]) + list(b[3]) + list(b[4]) for b in bones]
        wb = cd.world_matrices(bones, bind)
        world = [None] * len(bones)

        def get(i):
            if world[i] is None:
                v = ptmat[slot](i) if slot else i
                if slot == 0:
                    world[i] = mworld[i]
                elif 0 < v < 64:
                    world[i] = mworld[v]
                else:
                    # part root (-1) keeps its bind matrix; >=64 (hair/cloth
                    # bones) follow their parent with their bind offset
                    par = bones[i][0]
                    loc = cd.local_matrix(*[bind[i][k:k + 3] for k in (0, 3, 6)])
                    world[i] = cd.matmul(loc, get(par)) if par >= 0 else wb[i]
            return world[i]
        skin = [cd.matmul(cd.invert_rigid(wb[i]), get(i)) for i in range(len(bones))]
        lines, m, nv, nvt, np_, nt = cd.mesh_obj(amo, tex, skin, stem, SLOTS[slot] + "_", base, vtbase)
        obj += lines
        mtl += m
        base += nv
        vtbase += nvt
        print("%s: %d vertices, %d triangles, %d bones" % (nm, nv, nt, len(bones)))
    with open(args.out, "w") as f:
        f.write("\n".join(obj) + "\n")
    with open(stem + ".mtl", "w") as f:
        f.write("\n".join(mtl))
    print("wrote", args.out)


if __name__ == "__main__":
    main()
