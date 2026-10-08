#!/usr/bin/env python3
"""show_report.py - print an in-game bug report (F8) readably.

    python3 tools/show_report.py REPORT_DIR_OR_report.json [--log] [--replay]

REPORT_DIR is a reports/report_<date>/ folder (next to the logs folder; bug_report.sh packs the newest ones).
--log prints the last log lines, --replay prints the command that replays the session headless to that tick
(needs your own disc folder: DISC=path). Standard library only.
"""
import json, os, sys

def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    if not args:
        print(__doc__)
        return 1
    p = args[0]
    d = p if os.path.isdir(p) else os.path.dirname(p) or '.'
    rep = json.load(open(os.path.join(d, 'report.json') if os.path.isdir(p) else p))
    g = rep.get('game') or {}
    print('REPORT %s   build %s   %s' % (rep.get('report'), rep.get('build'), rep.get('created')))
    print('NOTE: %s' % rep.get('note'))
    print('window %sx%s   random seed %s   args: %s' % (rep['window'][0], rep['window'][1], rep.get('random_seed'), rep.get('arguments')))
    if g:
        h = g.get('hunter', {})
        c = g.get('camera', {})
        print('GAME  tick %s  mode %s step %s  stage %s  quest %s  map areas %s' % (
            g.get('tick'), g.get('mode'), g.get('step'), g.get('stage'), g.get('quest'), g.get('map_areas')))
        print('      hunter at %s angle %s  motion %s frame %s  mode %s step %s  hp %s' % (
            h.get('position'), h.get('angle'), h.get('motion_id'), h.get('motion_frame'), h.get('mode'), h.get('step'), h.get('hp')))
        print('      camera eye %s target %s fov %s' % (c.get('eye'), c.get('target'), c.get('fov')))
        for m in g.get('monsters_in_area', []):
            print('      %s slot %2d kind %2d at %s motion %s frame %s mode %s/%s hp %s' % (
                'npc    ' if m.get('npc') else 'monster', m['slot'], m['kind'], m['position'], m['motion_id'], m['motion_frame'], m['mode'], m['step'], m['hp']))
    print('MARKS')
    for m in rep.get('marks', []):
        if m['type'] == 'click':
            print('  %d. click at (%d,%d), %d new object(s)' % (m['n'], m['x'], m['y'], m['picked']))
        else:
            print('  %d. box (%d,%d)-(%d,%d), %d new object(s)' % (m['n'], m['x0'], m['y0'], m['x1'], m['y1'], m['picked']))
    print('PICKED OBJECTS (%d)' % len(rep.get('objects', [])))
    for i, o in enumerate(rep.get('objects', []), 1):
        r = o.get('render', {})
        b = r.get('blend', {})
        tex = r.get('texture')
        print('  %d. [%s] %s' % (i, o['kind'], o['description']))
        print('     ids %s  world %s  mark %s%s' % (o.get('ids'), o.get('world_position'), o.get('picked_by_mark'),
              '  hit %s' % o['hit_world'] if 'hit_world' in o else ''))
        if 'nearest_bone' in o:
            print('     nearest bone %(index)d (%(distance).0f away)' % o['nearest_bone'])
        for k in ('motion_id', 'motion_frame', 'hp', 'slot', 'model_kind', 'model_part', 'hunter_slot'):
            if k in o:
                print('     %s %s' % (k, o[k]), end='')
        if 'motion_id' in o:
            print()
        print('     draw: %s verts%s  blend %s %s/%s op %s  z-test %s z-write %s  alpha %s>%s  filter %s clamp %s  scroll %s' % (
            r.get('vertices'), ' tex %sx%s' % (tex['width'], tex['height']) if tex else ' untextured',
            'on' if b.get('on') else 'off', b.get('src'), b.get('dst'), b.get('operation'), r.get('z_test'), r.get('z_write'),
            r.get('alpha_func'), r.get('alpha_ref'), r.get('filter'), r.get('clamp'), r.get('scroll_matrix')))
        if 'draw_function' in o:
            print('     draw function %s' % o['draw_function'])
    rp = rep.get('replay', {})
    print('REPLAY %d ticks of pad input in input.txt, seed %s' % (rp.get('ticks', 0), rep.get('random_seed')))
    if '--replay' in sys.argv:
        disc = os.environ.get('DISC', 'disc/mh1')
        print('  RT_SEED=%s build/pc/mhview %s %s --input @%s --shot replay.png --time %.1f' % (
            rep.get('random_seed'), disc, rep.get('arguments', '').replace(' --input', ' #--input'), os.path.join(d, 'input.txt'),
            rp.get('ticks', 0) / 30.0))
        print('  (use the same --quest / --boot / --stage arguments as above, drop the original --input; RT_PICK_AT=%s stops it at the reported tick)' % g.get('tick'))
    if '--log' in sys.argv:
        print('LOG TAIL')
        print(open(os.path.join(d, 'log_tail.txt'), errors='replace').read())
    print('files: ' + ', '.join(sorted(os.listdir(d))))
    return 0

if __name__ == '__main__':
    sys.exit(main())
