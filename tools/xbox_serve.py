#!/usr/bin/env python3
"""Laptop side of the Xbox network loaders (src/pc/xbox/loader, docs/xbox.md "Network loader").

    tools/xbox_serve.py [--port 8765] [--xbe MH1.xbe] [--xbe-g G.xbe] [--input FILE] [--env FILE] [--test-game mh1|mhg]
    tools/xbox_serve.py --data dist/xbox/MHG [--data dist/xbox/MH1]     (install game folders, see below)

Serves both games at once, one loader each: E:\\Games\\MH1\\default.xbe ("Monster Hunter") asks with
?game=mh1, E:\\Games\\MHG\\default.xbe ("Monster Hunter G") with ?game=mhg. Requests without ?game= (the
loader installed before 16 Oct 2026, MH1 only) count as MH1.
  GET /version?game=G   size and time of that game's build ("none" when there is none; plus
                        build/xbox/rerun.txt when present, so touching that file makes the loader start the
                        same build again), and a "DATA" line when --data has that game's folder
  GET /mh1.xbe, /mhg.xbe the build: MH1 dist/xbox/MH1/default.xbe (--xbe for another, e.g.
                        build/xbox/nv2a/default.xbe), G dist/xbox/MHG/default.xbe (--xbe-g)
  GET /test?game=G      a test run's scripted pad input (--input, the PC's --input format) and RT_* switches
                        (--env, NAME=VALUE lines): "MH1TEST <n> <n>" + both; the loader puts them in its
                        data folder (input.txt, env.txt; xbox_env.c, viewer.c) and removes them when none are
                        given. --test-game: only for that game (default both)
  POST /log?game=G      saved to build/xbox/logs/<time>-<game>.txt and printed. A log that starts with
                        "MH1DUMP <name>" (src/pc/xbox/dumper: the BIOS flash, for xemu) is saved to
                        build/xbox/dump/<name> instead; Microsoft's file, so it stays out of git.
  POST /report?game=G   bug reports from the game's Back + Black reporter (src/pc/xbox/xbox_report.c: the loader
                        sends the ones not sent yet at its start, the online build at once): each is saved to
                        build/xbox/reports/<time>-<game>/ (screenshot.png, annotated.png, idbuffer.png,
                        report.json, log_tail.txt, input.txt as the PC writes them; read them with
                        tools/show_report.py DIR). Body: "MH1REPORTS <n>", per report "R <name>", per file
                        "F <size> <name>" + bytes, "E".
  POST /save?game=G     the game's save, sent by the loader with the log: E:\\UDATA\\<title id>\\<save id>\\ (the PS2 save
                        files byte for byte, mc_xbox.c, plus SaveMeta.xbx) -> build/xbox/saves/<time>-<game>/. Body:
                        "MH1SAVE <save id> <n>", per file "F <size> <name>" + bytes, "E". Load it on the PC with
                        tools/xbox_save_to_pc.py DIR (it makes the PC card folder; then MH1_SAVE_DIR=that folder).
Only these paths are served; nothing else in the repo is reachable.

--data DIR (may be given twice, once per game): install a game folder (./mh1 xbox's dist/xbox/MHG or
dist/xbox/MH1: default.xbe + data/, or a bare folder of the four disc files) on the Xbox through that game's
loader instead of FTP (docs/xbox.md "Installing game data through the loader"). The game is G when the folder
holds SLPM_658.69, else MH1. That game's /version then ends in a "DATA" line; its loader fetches
GET /manifest?game=G ("MH1DATA <MH1|MHG> <chunk> <n>", then per file "F <size> <crc32> <path>" and
"C <crc32 of each chunk> ..."), and each missing or short file with GET /data/<path>?game=G and a Range
header, writing it into its own folder. The folder's default.xbe is never in the list (on the Xbox that name is
the loader); it is served as the game's build (/mh1.xbe, /mhg.xbe) unless --xbe / --xbe-g names another.
"""
import argparse
import http.server
import os
import re
import sys
import time
import urllib.parse
import zlib

REPO = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..')
ROOT = os.path.join(REPO, 'build', 'xbox')
RERUN = os.path.join(ROOT, 'rerun.txt')
LOGS = os.path.join(ROOT, 'logs')
DUMPS = os.path.join(ROOT, 'dump')
REPORTS = os.path.normpath(os.path.join(ROOT, 'reports'))
SAVES = os.path.normpath(os.path.join(ROOT, 'saves'))
TEST = {'input': None, 'env': None, 'game': None}
CHUNK = 4 << 20          # the loader checks and writes the data in pieces of this size
FOLDER = {'mh1': 'MH1', 'mhg': 'MHG'}
# per game: the build it serves, whether that is still the default, and the data install (None: off)
GAMES = {g: {'xbe': os.path.normpath(os.path.join(REPO, 'dist', 'xbox', FOLDER[g], 'default.xbe')),
             'xbe_default': True, 'data': None} for g in FOLDER}


def version(game):
    g = GAMES[game]
    if not os.path.isfile(g['xbe']):
        v = 'none'       # no build to send; the loader keeps what it has
    else:
        st = os.stat(g['xbe'])
        v = '%d-%d' % (st.st_size, int(st.st_mtime))
        if os.path.exists(RERUN):
            v += '-' + open(RERUN).read().strip()
    if g['data']:
        v += '\nDATA'
    return v


def data_fresh(game):
    """the folder changed since the manifest was made (new files copied in): make it again"""
    d = GAMES[game]['data']
    if d and any(not os.path.exists(f) or os.path.getsize(f) != n or os.path.getmtime(f) != t
                 for f, n, t in d['stamp']):
        print('== the %s data folder changed: checksums again' % FOLDER[game])
        load_data(d['src'])


def load_data(src):
    """the manifest of a game folder: G or MH1 by its program file; data/ subfolder or the bare disc files"""
    src = os.path.abspath(src)
    sub = os.path.join(src, 'data')
    base = sub if os.path.isdir(sub) else src
    game = 'mhg' if os.path.exists(os.path.join(base, 'SLPM_658.69')) else 'mh1'
    elf = 'SLPM_658.69' if game == 'mhg' else 'SLPM_654.95'
    names = [elf, 'AFS01.AFS', 'AFS_DATA.AFS', 'AFS00.AFS']    # small first: one bad link shows early
    files = [('data/' + n, os.path.join(base, n)) for n in names]
    for _, f in files:
        if not os.path.exists(f):
            sys.exit('%s missing' % f)
    # the English table (./mh1 xbox puts it in data/text/en.txt, made from the player's own discs): the game looks for it
    # in <game folder>\data\text\en.txt (gfx_opts.c gfx_text_table_find); the loader makes the text folder
    en = os.path.join(base, 'text', 'en.txt')
    if os.path.exists(en):
        files.append(('data/text/en.txt', en))
    xbe = os.path.join(src, 'default.xbe')
    if GAMES[game]['xbe_default'] and os.path.exists(xbe):
        GAMES[game]['xbe'] = xbe        # the folder's XBE is the build (never installed as default.xbe: the loader)
    out = [b'MH1DATA %s %d %d' % (FOLDER[game].encode(), CHUNK, len(files))]
    files_new = {}
    for rel, f in files:
        size, crc, chunks = 0, 0, []
        print('checksums: %s ...' % f, end=' ')
        sys.stdout.flush()
        with open(f, 'rb') as fh:
            while True:
                b = fh.read(CHUNK)
                if not b:
                    break
                size += len(b)
                crc = zlib.crc32(b, crc)
                chunks.append(zlib.crc32(b))
        print('%d bytes, crc %08x' % (size, crc))
        out.append(b'F %d %08x %s' % (size, crc, rel.encode()))
        out.append(b'C ' + b' '.join(b'%08x' % c for c in chunks))
        files_new[rel] = (f, size)
    GAMES[game]['data'] = {'files': files_new, 'src': src, 'manifest': b'\n'.join(out) + b'\n',
                           'stamp': [(f, n, os.path.getmtime(f)) for f, n in files_new.values()]}
    return game


def save_reports(body, game, root=None):
    """a /report body -> one folder per report under root (build/xbox/reports); returns the folders, or None when
    the body is malformed (nothing is kept then)"""
    root = root or REPORTS
    lines = body.split(b'\n', 1)
    m = re.fullmatch(rb'MH1REPORTS (\d+)', lines[0])
    if not m or len(lines) < 2:
        return None
    rest, reports = lines[1], []
    for _ in range(int(m.group(1))):
        head, _, rest = rest.partition(b'\n')
        mr = re.fullmatch(rb'R ([A-Za-z0-9_.-]{1,63})', head)
        if not mr:
            return None
        files = []
        while True:
            head, _, rest = rest.partition(b'\n')
            if head == b'E':
                break
            mf = re.fullmatch(rb'F (\d+) ([A-Za-z0-9_][A-Za-z0-9_.-]{0,63})', head)
            if not mf or b'..' in mf.group(2) or int(mf.group(1)) > len(rest):
                return None
            n = int(mf.group(1))
            files.append((mf.group(2).decode(), rest[:n]))
            rest = rest[n:]
        reports.append((mr.group(1).decode(), files))
    out = []
    stamp = time.strftime('%Y%m%d-%H%M%S')
    for name, files in reports:
        d, k = os.path.join(root, '%s-%s' % (stamp, game)), 2
        while os.path.exists(d) or d in out:
            d, k = os.path.join(root, '%s-%s-%d' % (stamp, game, k)), k + 1
        os.makedirs(d)
        for f, data in files:
            open(os.path.join(d, f), 'wb').write(data)
        open(os.path.join(d, 'xbox_folder.txt'), 'w').write('E:\\Games\\%s\\reports\\%s\n' % (FOLDER[game], name))
        out.append(d)
    return out


def set_xbe(game, path):
    GAMES[game]['xbe'] = os.path.abspath(path) if path else ''
    GAMES[game]['xbe_default'] = False


class Handler(http.server.BaseHTTPRequestHandler):
    def reply(self, body, ctype='application/octet-stream'):
        self.send_response(200)
        self.send_header('Content-Type', ctype)
        self.send_header('Content-Length', str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def game(self):
        """the path without the query, and the game it names (no ?game=: the old MH1 loader); None if unknown"""
        u = urllib.parse.urlsplit(self.path)
        g = urllib.parse.parse_qs(u.query).get('game', ['mh1'])[0]
        return u.path, (g if g in GAMES else None)

    def do_GET(self):
        path, game = self.game()
        if not game:
            self.send_error(404)
            return
        if path in ('/version', '/manifest'):
            data_fresh(game)
        d = GAMES[game]['data']
        if path == '/version':
            self.reply(version(game).encode(), 'text/plain')
        elif path == '/test':
            on = TEST['game'] in (None, game)
            ins = open(TEST['input'], 'rb').read() if on and TEST['input'] else b''
            env = open(TEST['env'], 'rb').read() if on and TEST['env'] else b''
            self.reply(b'MH1TEST %d %d\n' % (len(ins), len(env)) + ins + env, 'text/plain')
        elif path in ('/mh1.xbe', '/mhg.xbe') and os.path.isfile(GAMES[path[1:4]]['xbe']):
            self.reply(open(GAMES[path[1:4]]['xbe'], 'rb').read())     # named by the file, ?game= or not
        elif path == '/manifest' and d:
            self.reply(d['manifest'], 'text/plain')
        elif path.startswith('/data/') and d and path[6:] in d['files']:
            self.send_data(*d['files'][path[6:]])
        else:
            self.send_error(404)

    def send_data(self, f, size):
        """the file from the Range start (bytes=N- or bytes=N-M) on, in 1 MB pieces"""
        start, end = 0, size - 1
        m = re.fullmatch(r'bytes=(\d+)-(\d*)', self.headers.get('Range', '') or '')
        if m:
            start = int(m.group(1))
            if m.group(2):
                end = min(end, int(m.group(2)))
            if start > end:
                self.send_error(416)
                return
            self.send_response(206)
            self.send_header('Content-Range', 'bytes %d-%d/%d' % (start, end, size))
        else:
            self.send_response(200)
        self.send_header('Content-Type', 'application/octet-stream')
        self.send_header('Content-Length', str(end + 1 - start))
        self.end_headers()
        t0, left = time.time(), end + 1 - start
        try:
            with open(f, 'rb') as fh:
                fh.seek(start)
                while left > 0:
                    b = fh.read(min(left, 1 << 20))
                    if not b:
                        break
                    self.wfile.write(b)
                    left -= len(b)
        except (BrokenPipeError, ConnectionResetError) as e:
            print('== %s cut at %d MB (%s)' % (self.path, (end + 1 - start - left) >> 20, e.__class__.__name__))
            return
        dt = max(time.time() - t0, 0.001)
        print('== %s: %d MB from %d MB in %.0f s (%.1f MB/s)' % (self.path, (end + 1 - start) >> 20, start >> 20,
                                                             dt, (end + 1 - start) / dt / 1e6))

    def do_POST(self):
        body = self.rfile.read(int(self.headers.get('Content-Length', 0)))
        path, game = self.game()
        if path == '/log' and body.startswith(b'MH1DUMP '):    # src/pc/xbox/dumper: name line + data
            head, _, data = body.partition(b'\n')
            name = head[8:].decode('ascii', 'replace').strip()
            if not re.fullmatch(r'[a-z0-9_]{1,24}\.bin', name):
                self.send_error(400)
                return
            os.makedirs(DUMPS, exist_ok=True)
            open(os.path.join(DUMPS, name), 'wb').write(data)
            print('== Xbox dump -> %s (%d bytes)' % (os.path.join(DUMPS, name), len(data)))
        elif path == '/log' and game:
            os.makedirs(LOGS, exist_ok=True)
            name = os.path.join(LOGS, time.strftime('%Y%m%d-%H%M%S') + '-%s.txt' % game)
            open(name, 'wb').write(body)
            print('== Xbox %s log -> %s (%d bytes)' % (FOLDER[game], name, len(body)))
            sys.stdout.write(body.decode('utf-8', 'replace')[-3000:] + '\n')
        elif path == '/report' and game:
            dirs = save_reports(body, game)
            if dirs is None:
                self.send_error(400)
                return
            for d in dirs:
                note = ''
                try:
                    import json
                    note = json.load(open(os.path.join(d, 'report.json'))).get('note', '')
                except (OSError, ValueError):
                    pass
                print('== Xbox %s bug report -> %s  note: %s  (python3 tools/show_report.py %s)'
                      % (FOLDER[game], d, note, os.path.relpath(d, os.getcwd())))
        elif path == '/save' and game:
            d = save_save(body, game)
            if d is None:
                self.send_error(400)
                return
            print('== Xbox %s save -> %s  (on the PC: python3 tools/xbox_save_to_pc.py %s)'
                  % (FOLDER[game], d, os.path.relpath(d, os.getcwd())))
        elif path == '/note':
            print('== Xbox: %s' % body.decode('utf-8', 'replace'))
        else:
            self.send_error(404)
            return
        self.reply(b'ok', 'text/plain')

    def log_message(self, fmt, *args):
        print('%s %s' % (time.strftime('%H:%M:%S'), fmt % args))
        sys.stdout.flush()


def save_save(body, game):
    """a /save body -> build/xbox/saves/<time>-<game>/<files>; returns the folder, or None when the body is malformed"""
    head, _, rest = body.partition(b'\n')
    m = re.fullmatch(rb'MH1SAVE ([0-9A-F]{16}) (\d+)', head)
    if not m:
        return None
    files = []
    for _ in range(int(m.group(2))):
        line, _, rest = rest.partition(b'\n')
        f = re.fullmatch(rb'F (\d+) ([A-Za-z0-9._-]{1,63})', line)
        if not f or int(f.group(1)) > len(rest):
            return None
        n = int(f.group(1))
        files.append((f.group(2).decode('ascii'), rest[:n]))
        rest = rest[n:]
    if not rest.startswith(b'E'):
        return None
    d = os.path.join(SAVES, time.strftime('%Y%m%d-%H%M%S') + '-%s' % game)
    os.makedirs(d, exist_ok=True)
    for name, data in files:
        open(os.path.join(d, name), 'wb').write(data)
    open(os.path.join(d, 'save_id.txt'), 'w').write(m.group(1).decode('ascii') + '\n')
    return d


def main():
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--port', type=int, default=8765)
    ap.add_argument('--xbe', help="MH1's build (default dist/xbox/MH1/default.xbe)")
    ap.add_argument('--xbe-g', help="G's build (default dist/xbox/MHG/default.xbe)")
    ap.add_argument('--input', help="a test run's scripted input (PC --input format)")
    ap.add_argument('--env', help="a test run's RT_* switches (NAME=VALUE lines)")
    ap.add_argument('--test-game', choices=sorted(GAMES), help='send --input/--env only to this game (default both)')
    ap.add_argument('--data', action='append', default=[],
                    help='a game folder to install through its loader (dist/xbox/MHG, dist/xbox/MH1); once per game')
    a = ap.parse_args()
    TEST['input'] = a.input and os.path.abspath(a.input)
    TEST['env'] = a.env and os.path.abspath(a.env)
    TEST['game'] = a.test_game
    if a.xbe:
        set_xbe('mh1', a.xbe)
    if a.xbe_g:
        set_xbe('mhg', a.xbe_g)
    for d in a.data:
        game = load_data(d)
        print('installing %s to E:\\Games\\%s (%d files) through its loader: start %s on the Xbox'
              % (os.path.abspath(d), FOLDER[game], len(GAMES[game]['data']['files']),
                 'Monster Hunter G' if game == 'mhg' else 'Monster Hunter'))
    for g in sorted(GAMES):
        x = GAMES[g]['xbe']
        print('%s: %s (version %s)' % (FOLDER[g], os.path.normpath(x) + ' as /%s.xbe' % g if os.path.isfile(x)
                                       else 'no build', version(g).replace('\n', ' ')))
    print('serving on port %d' % a.port)
    sys.stdout.flush()
    http.server.ThreadingHTTPServer(('0.0.0.0', a.port), Handler).serve_forever()


if __name__ == '__main__':
    main()
