#!/usr/bin/env python3
"""g_eng_tour.py - Monster Hunter G in English on the PC, hidden: a tour of G's screens for the text check.

    python3 tools/g_eng_tour.py [--mh1] [scene ...]  (all scenes without arguments; --list names them)

Runs the G PC build (build/B/mhview_g if present, else build/mhg/pc/mhview_g) with the English table
(RT_TEXT_TABLE=build/text/en_mhg.txt, made by tools/text_map.py), always hidden, one scene per run, under a 1.5 GB
address-space cap. Each scene is a pad script (the tick timing of tools/test_gvillage.py and tools/pc_scripts) with
pictures at the listed ticks (RT_SHOTS). Output: build/B/eng/<scene>/<scene>_<tick>.png, <scene>.log, and a contact
sheet build/B/eng/<scene>.html (plus build/B/eng/index.html). Standard library only (the sheets are HTML pages).
G_ENG_TABLE=/dev/null G_ENG_SUFFIX=_jp gives the same scene without the English table (build/B/eng/<scene>_jp/), to tell
our text from what the PC port draws anyway. RT_TEXT_JP=1 also writes jp_left.txt: per scene the Japanese still drawn
and the overlay ids it came from (rt_font.c / rt_text.c traces). The online town (scene town) uses the G online build
(ONLINE=1 tools/build_pc_mhg.sh) and the repo's test server on 127.0.0.1 only.
"""
import html
import os
import re
import shutil
import time
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GAME = "mh1" if "--mh1" in sys.argv else "mhg"      # --mh1: MH1 JP with its table (build/pc/mhview, build/B/eng_mh1/)
OUT = os.environ.get("G_ENG_OUT") or os.path.join(ROOT, "build", "B", "eng_mh1" if GAME == "mh1" else "eng")   # G_ENG_OUT: another folder
SCRIPTS = os.path.join(ROOT, "tools", "pc_scripts")

SHOP, WSHOP, FORGE, ELDER = "10000,13425", "9860,12076", "9960,11920", "11100,12150"
HOUSE = "30,11225,14400,0;150,1990,1160,4001"


def script(name):
    return open(os.path.join(SCRIPTS, name)).read().strip()


def ev_script(events, end):
    """{tick: 'pad*n'} after the village entry -> a pad script (tools/gvil_ev.py's timing)"""
    steps, t = [], 0
    for tick, pad in sorted(events.items()):
        if tick > t:
            steps.append("idle*%d" % (tick - t))
            t = tick
        steps.append(pad)
        t += int(pad.split("*")[1]) if "*" in pad else 1
    steps.append("idle*%d" % max(1, end - t + 60))
    return ",".join(["@village1"] + steps)


def ev(*parts):
    d = {}
    for p in parts:
        d.update(p)
    return d


def circles(first, n, step=30):
    return {first + step * i: "circle*2" for i in range(n)}


POUCH = "RT_PL_ITEMS" if GAME == "mh1" else "RT_POUCH"     # the test aid that fills the pouch
VQUEST = "131" if GAME == "mh1" else "1"                    # the quest whose village start the runs use


def village(events, end, shots, warp=None, **env):
    if "POUCH" in env:
        env[POUCH] = env.pop("POUCH")
    e = dict(RT_VILLAGE_START="1", RT_VILLAGE_SKIP_INTRO="1", RT_STEP="1", RT_MONEY="50000", **env)
    if warp:
        e["RT_LB_WARP"] = "30,%s,0" % warp
    return dict(args=["--quest", VQUEST, "--play"], input=ev_script(events, end), time=end / 30.0 + 8,
                shots=shots, env=e, card="vcard")


def boot(inp, secs, shots, card="bcard", **env):
    return dict(args=["--boot"], input=inp, time=secs, shots=shots, env=dict(RT_STEP="1", **env), card=card)


GM = "idle*600,start*2,idle*60"          # power-on -> title -> game menu (NEW GAME / CONTINUE / GALLERY / OPTIONS)
SCENES = {
    # power-on, title, game menu, character creation, the card dialogs of a new game
    "newgame": boot(script("newgame.txt").replace("dup*2,idle*10,", "", 1) + ",idle*900", 110,
                    [640, 700, 760, 900, 1100, 1300, 1500, 1800, 2100, 2400, 2800, 3100]) | dict(fresh=True),
    # NETWORK OPTIONS: G's fifth row of the game menu (each ddown here moves one row more than expected: n presses -> row n+1)
    # (28 Oct: four downs reach it; then each row's help: NET file, "same as last", login ID, network settings, EXIT)
    "net_options": boot(GM + ",ddown*2,idle*30" * 4 + ",circle*2,idle*90" + ",ddown*2,idle*40" * 4 + ",circle*2,idle*120,cross*2,idle*60",
                        45, [700, 760, 880, 920, 960, 1000, 1045, 1140], card="ncard") | dict(fresh=True),   # no save: the menu opens on NEW GAME
    # gallery (omake): third row of the game menu
    "gallery": boot(GM + ",ddown*2,idle*10,circle*2,idle*90,ddown*2,idle*30,ddown*2,idle*60,cross*2,idle*120", 40,
                    [700, 760, 800, 860, 920]),
    # options: fourth row
    "options": boot(GM + ",ddown*2,idle*10,ddown*2,idle*10,circle*2,idle*90,ddown*2,idle*30,ddown*2,idle*60", 40,
                    [700, 790, 830, 880]),
    # continue from the new game's card: the mode menu (town / village), then the town (network) side: NET file dialogs
    "network": boot(script("continue.txt").split(",idle*150")[0] + ",idle*150,ddown*2,idle*10,circle*2,idle*200,circle*2,idle*200,circle*2,idle*300",
                    60, [700, 800, 900, 1000, 1100, 1250, 1400, 1600]),
    # village: item shop list and quantity / price prompt
    "item_shop": village(ev(circles(45, 5), {200: "ddown*2", 230: "circle*2"}), 330, [60, 110, 170, 210, 250, 300], warp=SHOP),
    # weapon shop: menu, buy list, the detail window (square) and its description page (right), for three weapons
    "weapon_shop": village(ev(circles(45, 5), {230: "square*2", 290: "dright*2", 330: "cross*2", 350: "ddown*2", 370: "square*2",
                                               420: "dright*2", 460: "cross*2", 480: "ddown*2", 500: "square*2", 550: "dright*2"}),
                           600, [60, 110, 180, 250, 300, 400, 440, 530, 580], warp=WSHOP),
    # armour shop list
    # armour shop list, then the detail window's description page (square, right) for six pieces down the list
    "armour_shop": village(ev({45: "circle*2", 75: "circle*2", 105: "circle*2", 135: "circle*2", 150: "ddown*2", 165: "circle*2"},
                              *({200 + 100 * i: "square*2", 240 + 100 * i: "dright*2", 280 + 100 * i: "cross*2", 290 + 100 * i: "ddown*2"}
                                for i in range(6))), 820,
                           [190, 230] + [270 + 100 * i for i in range(6)], warp=WSHOP),
    # forge: weapon list, upgrade tab
    "forge": village(ev(circles(45, 5), {220: "cross*2", 250: "ddown*2", 280: "circle*2"}), 360, [60, 110, 170, 210, 300, 340], warp=FORGE,
                     RT_POUCH="155:5"),
    # house: item box menu, item window, equipment change, the box tidy prompt
    "item_box": dict(village({45: "square*2", 200: "square*2", 260: "circle*2", 300: "cross*2", 330: "ddown*2", 350: "ddown*2", 370: "circle*2",
                              430: "cross*2", 460: "cross*2", 490: "ddown*2", 510: "ddown*2", 530: "ddown*2", 550: "circle*2"}, 640,
                             [230, 280, 320, 400, 450, 580, 620], RT_POUCH="129:5,1:3,155:2", RT_BOX_ITEMS="130:4,131:2", RT_WARE="6:1,6:2,2:1,2:5"),
                     env=dict(RT_VILLAGE_START="1", RT_VILLAGE_SKIP_INTRO="1", RT_STEP="1", RT_MONEY="500", RT_LB_WARP=HOUSE,
                              RT_POUCH="129:5,1:3,155:2", RT_BOX_ITEMS="130:4,131:2", RT_WARE="6:1,6:2,2:1,2:5")),
    # the elder's talk (first meeting, story event 1)
    "elder_talk": village(circles(45, 22), 720, [80, 140, 200, 260, 320, 380, 440, 500, 560, 640], warp=ELDER),
    # the elder's quest board after his talk: quest levels, the quest list, a quest's info pages
    "quest_board": village(ev(circles(45, 22), {700: "circle*2", 760: "circle*2", 830: "dright*2", 890: "dright*2", 950: "dright*2",
                                                1010: "cross*2", 1060: "ddown*2", 1090: "circle*2"}),
                           1200, [690, 740, 800, 860, 920, 980, 1040, 1080, 1130, 1180], warp=ELDER),
    # pause menu in the village: rows, status (two pages), equipment, hunting record
    # pause menu in the village: the rows' help, status (two pages), equipment details (the weapon's description page)
    "pause": village({60: "start*2", 100: "ddown*2", 130: "ddown*2", 160: "ddown*2", 190: "ddown*2", 220: "circle*2", 280: "dright*2",
                      330: "cross*2", 360: "ddown*2", 390: "circle*2", 450: "dright*2", 500: "dright*2"}, 560,
                     [80, 120, 200, 250, 310, 345, 420, 480, 540]),
    # pause menu, Reference row: the data page (G's frame list, hunting log, mix list)
    "data_page": village({60: "start*2", 100: "ddown*2", 130: "ddown*2", 160: "ddown*2", 200: "circle*2", 280: "dright*2", 340: "circle*2"}, 440,
                         [90, 150, 190, 230, 260, 320, 370, 430]),
    # in a quest: pause menu, quest info pages, the item pouch
    "quest_info": dict(args=["--quest", "1", "--play"], input="idle*200,start*2,idle*40,ddown*2,idle*20,ddown*2,idle*20,ddown*2,idle*20,circle*2,idle*60,dright*2,idle*60,"
                       "dright*2,idle*60,cross*2,idle*30,cross*2,idle*60,start*2,idle*40,circle*2,idle*90", time=30,
                       shots=[220, 260, 330, 400, 460, 560, 640], env=dict(RT_STEP="1", RT_QUEST_STAGE="1"), card="qcard"),
    # quest 10001 from its start: the "Wisdom of the Chief added" banner (sub_main 0x539AF0), the field HUD; the hunter
    # follows the monsters with the reward scene's aids
    "wisdom": dict(args=["--quest", "10001", "--play"], input="idle*60" + (",cam_u*2,idle*30" * 4 + ",circle*2,idle*28" * 6 +
                   ",cross*2,idle*14,ddown*2,idle*14,circle*2,idle*14") * 6, time=60, shots=[100, 160, 300, 600, 1200],
                   env=dict(RT_STEP="1", RT_PL_GOD="1", RT_PL_GOTO="60,f", RT_PL_TARGET="k6", RT_PL_WARP_EM="900-90000",
                            RT_DMG_MUL="40"), card="qcard"),
    # a hunt cleared with the test aids of tools/test_all_quests.py (quest 4, Yian Kut-Ku): clear banner, reward screen and list
    "reward": dict(args=["--quest", "4", "--play"], input="idle*60" + (',cam_u*2,idle*30' * 4 + ',circle*2,idle*28' * 6 + ',cross*2,idle*14,ddown*2,idle*14,circle*2,idle*14') * 40, time=430,
                   shots=[8240, 8300] + list(range(10000, 12900, 250)),   # 8240: the clear line ("All requested monsters slain.")
                   env=dict(RT_STEP="1", RT_PL_GOD="1", RT_PL_GOTO="60,f", RT_PL_TARGET="k6", RT_PL_WARP_EM="900-90000",
                            RT_DMG_MUL="40", RT_PL_SLAY="8000"), card="qcard"),
}


if GAME == "mh1":
    # MH1 JP: the same screens (MH1's game menu has no NETWORK OPTIONS row; its item ids: herb 65, antidote 66, 67, ore 109;
    # its new-game script is tools/pc_scripts/newgame.txt as it is), plus the online town (ONLINE=1 build, test server)
    ITEMS = {"129": "65", "130": "66", "131": "67", "155": "109"}

    def mh1_items(v):
        return ",".join(":".join([ITEMS.get(a.split(":")[0], a.split(":")[0])] + a.split(":")[1:]) for a in v.split(","))
    for _n, _sc in SCENES.items():
        for _k in ("RT_POUCH", "RT_BOX_ITEMS"):
            if _k in _sc["env"]:
                _sc["env"]["RT_PL_ITEMS" if _k == "RT_POUCH" else _k] = mh1_items(_sc["env"].pop(_k))
    SCENES.pop("net_options")
    SCENES["newgame"]["input"] = script("newgame.txt") + ",idle*900"
    SCENES["gallery"]["input"] = GM + ",ddown*2,idle*10,circle*2,idle*90,ddown*2,idle*30,ddown*2,idle*60,cross*2,idle*120"
    # the online town (tools/test_online_town.sh's screens): connecting, Capcom ID, login, the plaza and its lobby list, the
    # town; then the plaza menu (start), the guild counter talk (warp + circle)
    SCENES["town"] = dict(args=["--quest", "10", "--play", "--online", "--mute"], online=True, time=60,
                          input="idle*200,circle*2,idle*100,circle*2,idle*100,circle*2,idle*150,circle*2,idle*400,start*2,idle*60,"
                                "ddown*2,idle*40,ddown*2,idle*40,cross*2,idle*40,cross*2,idle*300",
                          shots=[150, 260, 360, 470, 620, 700, 800, 900, 1000, 1080, 1150, 1230, 1300, 1400, 1600],
                          env=dict(RT_STEP="1", RT_VILLAGE_START="1", RT_VILLAGE_SKIP_INTRO="1", RT_ONLINE_TRACE="1"), card="tcard")


if GAME == "mhg":
    # G's online side on the repo's test server (127.0.0.1 only; tools/test_online_g.sh's screens): login, Capcom ID, the
    # info file, the plaza class menu, the plaza and its town list, town 1 (frames 0-2620 of that test's input), then the
    # guild hall door (square, warped there) and the counter's talk, quest levels and first card (warp at town tick 300)
    _IN = ["idle*200"] + ["circle*2", "idle*150"] * 14 + ["circle*2", "idle*120", "dleft*2", "idle*30", "circle*2", "idle*150"] * 12
    _b, _f = [], 0
    for _it in _IN:
        _k, _c = _it.split("*")
        if _f + int(_c) > 2620:
            _b.append("idle*%d" % (2620 - _f))
            break
        _b.append(_it)
        _f += int(_c)
    SCENES["town"] = dict(args=["--quest", "10", "--play", "--online", "--mute"], online=True, time=175,
                          input=",".join(_b) + ",square*2,idle*270" + ",circle*2,idle*148" * 9 + ",cross*2,idle*100" * 3 + ",idle*300",
                          shots=[260, 420, 570, 720, 870, 1020, 1170, 1320, 1600, 1900, 2300, 2700, 2950, 3100, 3250, 3400, 3550,
                                 3700, 3850, 4000, 4300],
                          env=dict(RT_STEP="1", RT_VILLAGE_START="1", RT_VILLAGE_SKIP_INTRO="1", RT_ONLINE_TRACE="1",
                                   RT_NET_REGISTERED="1", RT_LB_WARP="60,5545,2750,0;300,1120,1950"), card="tcard")


def exe(sc=None):
    if GAME == "mh1":
        p = "build/pc/mhview_online" if sc and sc.get("online") else "build/pc/mhview"
        if not os.path.exists(os.path.join(ROOT, p)):
            sys.exit("g_eng_tour: no %s (tools/build_pc.sh%s)" % (p, ", ONLINE=1" if sc and sc.get("online") else ""))
        return os.path.join(ROOT, p)
    if sc and sc.get("online"):
        p = "build/mhg/pc/mhview_g_online"
        if not os.path.exists(os.path.join(ROOT, p)):
            sys.exit("g_eng_tour: no %s (ONLINE=1 tools/build_pc_mhg.sh)" % p)
        return os.path.join(ROOT, p)
    for p in ("build/B/mhview_g", "build/mhg/pc/mhview_g"):
        if os.path.exists(os.path.join(ROOT, p)):
            return os.path.join(ROOT, p)
    sys.exit("g_eng_tour: no G PC build (tools/build_pc_mhg.sh)")


def run(name, sc):
    d = os.path.join(OUT, name + os.environ.get("G_ENG_SUFFIX", ""))
    os.makedirs(d, exist_ok=True)
    for x in os.listdir(d):
        if x.endswith(".png"):
            os.remove(os.path.join(d, x))
    if sc.get("fresh"):         # a new game needs an empty memory card
        shutil.rmtree(os.path.join(OUT, sc["card"]), ignore_errors=True)
    table = "en_mh1.txt" if GAME == "mh1" else "en_mhg.txt"
    env = dict(os.environ, RT_NOMOVIE="1", RT_NAME="TEST", RT_QUEST_TRACE="1",
               RT_TEXT_TABLE=os.environ.get("G_ENG_TABLE", os.path.join(ROOT, "build", "text", table)),
               MH1_SAVE_DIR=os.path.join(OUT, sc["card"]), RT_SHOTS=",".join(map(str, sc["shots"])), **sc["env"])
    env.pop("MH1_SHOW", None)
    server = None
    if sc.get("online"):        # the repo's test server on 127.0.0.1 only (tools/mh1_testserver.py), as test_online_town.sh
        shutil.rmtree(env["MH1_SAVE_DIR"], ignore_errors=True)
        slog = open(os.path.join(d, "server.log"), "w")
        server = subprocess.Popen([sys.executable, os.path.join(ROOT, "tools", "mh1_testserver.py"), "-v", "--port", "0"],
                                  stdout=slog, stderr=subprocess.STDOUT, cwd=ROOT)
        port = None
        for _i in range(100):
            m = re.search(r"listening on 127\.0\.0\.1:(\d+)", open(os.path.join(d, "server.log")).read())
            if m:
                port = m.group(1)
                break
            time.sleep(0.1)
        if not port:
            server.kill()
            sys.exit("g_eng_tour: the test server did not start")
        env["RT_NET_PORT"] = port
    cmd = ["sh", "-c", 'ulimit -v 1500000; exec "$@"', "sh", exe(sc), os.path.join(ROOT, "disc", "mh1" if GAME == "mh1" else "mhg")] + \
          sc["args"] + ["--input", sc["input"], "--shot", os.path.join(d, name + ".png"), "--time", str(sc["time"])]
    try:
        with open(os.path.join(d, name + ".log"), "wb") as f:
            r = subprocess.run(cmd, cwd=ROOT, env=env, stdout=f, stderr=subprocess.STDOUT, timeout=1200)
    finally:
        if server:
            server.kill()
    pics = sorted((x for x in os.listdir(d) if x.endswith(".png")), key=lambda x: (len(x), x))
    sheet(os.path.basename(d), pics, r.returncode)
    return r.returncode, len(pics)


def sheet(name, pics, rc):
    rows = "".join('<figure><img src="%s/%s"><figcaption>%s</figcaption></figure>' % (name, html.escape(p), html.escape(p)) for p in pics)
    page = ("<!doctype html><meta charset=utf-8><title>%s</title><style>body{background:#222;color:#ddd;font:13px sans-serif}"
            "figure{display:inline-block;margin:4px}img{width:320px;image-rendering:pixelated}</style><h1>%s (exit %d)</h1>%s"
            % (name, name, rc, rows))
    open(os.path.join(OUT, name + ".html"), "w").write(page)


def jp_left(b):
    """True when a drawn string has Japanese words (kana / kanji), not only full-width marks, blanks or digits"""
    i = 0
    while i < len(b):
        c = b[i]
        if 0x81 <= c <= 0x9F or 0xE0 <= c <= 0xEF:
            w = (c << 8) | (b[i + 1] if i + 1 < len(b) else 0)
            if 0x829F <= w <= 0x82F1 or 0x8340 <= w <= 0x8396 or c >= 0x88:
                return True
            i += 2
        else:
            i += 1
    return False


def jp_report(names):
    """RT_TEXT_JP=1: OUT/jp_left.txt, per scene the Japanese still drawn (rt_font.c 'textjp' lines, the first 64 bytes,
    times drawn) and the overlay ids it came from without a table entry (rt_text.c 'textsrc')"""
    out = []
    for n in names:
        d = os.path.join(OUT, n + os.environ.get("G_ENG_SUFFIX", ""))
        try:
            data = open(os.path.join(d, n + ".log"), "rb").read()
        except OSError:
            continue
        seen, src = {}, {}
        for line in data.split(b"\n"):
            if line.startswith(b"textjp:"):
                b = bytes(int(x, 16) for x in line.split()[1:])
                if jp_left(b):
                    seen[b] = seen.get(b, 0) + 1
            elif line.startswith(b"textsrc:"):
                k = line.split()[1].decode()
                src[k] = src.get(k, 0) + 1
        out.append("%s: %d Japanese strings drawn" % (n, len(seen)))
        out += ["  %5d  %s" % (c, b.decode("cp932", "replace")) for b, c in sorted(seen.items(), key=lambda x: -x[1])]
        if src:
            out.append("  from overlay ids without a table entry: " + " ".join("%s(%d)" % kv for kv in sorted(src.items())))
    open(os.path.join(OUT, "jp_left.txt"), "w").write("\n".join(out) + "\n")
    return out


def index():
    names = [n for n in SCENES if os.path.exists(os.path.join(OUT, n + ".html"))]
    open(os.path.join(OUT, "index.html"), "w").write(
        "<!doctype html><meta charset=utf-8><title>%s in English</title><h1>%s in English: contact sheets</h1><ul>%s</ul>"
        % (("MH1" if GAME == "mh1" else "G",) * 2 + ("".join('<li><a href="%s.html">%s</a></li>' % (n, n) for n in names),)))


if __name__ == "__main__":
    if "--list" in sys.argv:
        print(" ".join(SCENES))
        sys.exit(0)
    names = [a for a in sys.argv[1:] if not a.startswith("--")] or list(SCENES)
    for n in names:
        rc, k = run(n, SCENES[n])
        print("%-12s exit %d, %d pictures" % (n, rc, k))
        sys.stdout.flush()
    index()
    if os.environ.get("RT_TEXT_JP"):
        print("\n".join(l for l in jp_report(names) if not l.startswith(" ")))
        print("Japanese still drawn: %s" % os.path.join(OUT, "jp_left.txt"))
