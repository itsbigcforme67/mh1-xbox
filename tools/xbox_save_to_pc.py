#!/usr/bin/env python3
"""xbox_save_to_pc.py SAVEDIR [--to CARDDIR] : put an Xbox save (as tools/xbox_serve.py stores it: build/xbox/saves/<time>-<game>/)
into the PC port's card layout, so the owner's exact save can be played or debugged on the PC.

The Xbox keeps the PS2 save files byte for byte (src/pc/xbox/mc_xbox.c): E:\\UDATA\\<title id>\\<save id>\\ holds the data file
(BISLPM-65495MH for Monster Hunter, BISLPM-65869MHG for G), icon.sys and icon00.ico, plus the dashboard's SaveMeta.xbx. The PC's
card (src/pc/rt/rt_mc.c) is a folder (MH1_SAVE_DIR, default ~/.local/share/mh1pc/memcard0 or .../mhgpc/memcard0) holding the
PS2 save directory: CARDDIR/BISLPM-65495MH/{BISLPM-65495MH, icon.sys, icon00.ico}. This tool makes that folder (default
SAVEDIR/pc_card) and prints how to start the PC build on it, e.g.

    MH1_SAVE_DIR=build/xbox/saves/20261021-203000-mhg/pc_card build/mhg/pc/mhview_g disc/mhg --boot --play ...

It never writes into the player's own save folder unless --to names it (an existing save there is moved to <dir>.bak-<time>).
"""
import argparse
import os
import shutil
import sys
import time

GAMES = {'BISLPM-65495MH': 'Monster Hunter (mh1pc)', 'BISLPM-65869MHG': 'Monster Hunter G (mhgpc)'}


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('savedir', help="a folder from build/xbox/saves/ (or a copy of E:\\UDATA\\<title id>\\<save id>)")
    ap.add_argument('--to', help='the PC card folder to make (default SAVEDIR/pc_card)')
    a = ap.parse_args()
    names = os.listdir(a.savedir)
    data = [n for n in names if n in GAMES]
    if len(data) != 1:
        sys.exit('%s: no Monster Hunter save data file (%s) in it' % (a.savedir, ' / '.join(GAMES)))
    ps2dir = data[0]
    card = a.to or os.path.join(a.savedir, 'pc_card')
    dest = os.path.join(card, ps2dir)
    if os.path.exists(dest):
        bak = dest + '.bak-' + time.strftime('%Y%m%d-%H%M%S')
        os.rename(dest, bak)
        print('the save already there was moved to %s' % bak)
    os.makedirs(dest)
    for n in names:
        if n in (ps2dir, 'icon.sys') or n.endswith('.ico'):      # the PS2 save's own files (not SaveMeta.xbx / save_id.txt)
            shutil.copyfile(os.path.join(a.savedir, n), os.path.join(dest, n))
    print('%s save -> %s' % (GAMES[ps2dir], dest))
    exe = 'build/mhg/pc/mhview_g disc/mhg' if ps2dir.endswith('MHG') else 'build/pc/mhview disc/mh1'
    print('play it:  MH1_SAVE_DIR=%s %s --boot --play   (then CONTINUE)' % (card, exe))


if __name__ == '__main__':
    main()
