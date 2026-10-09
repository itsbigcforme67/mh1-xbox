#!/usr/bin/env python3
"""mh1-server: our own open-source server for the PC / Xbox port of Monster Hunter (PS2). docs/server.md.

NOT an MH Oldschool server and not Capcom's: written from the game client's code. Early skeleton:

  relay     the in-hunt session relay (relay.py): players' games join it with `--join SERVER --port P` and hunt
            together through it, as with a player hosting but with nobody needing an open port
  lobby     the lobby protocol (lobby_stub.py over tools/mh1_testserver.py) with accounts from SQLite
  accounts  manage the SQLite account store

    python3 tools/server/mh1_server.py serve [--session 10301:131:2] [--lobby-port 10200] [--db mh1.sqlite3] ...
    python3 tools/server/mh1_server.py account add|list|ban|unban|passwd|delete|export ...

Binds loopback by default; a public address needs --allow-public (and an operator who has read docs/server.md 8-10).
Python 3.8+, standard library only.
"""
import argparse
import asyncio
import json
import os
import signal
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from relay import Relay, Limits                # noqa: E402
from accounts import AccountDB                 # noqa: E402


def parse_session(s):
    try:
        port, quest, players = (int(x) for x in s.split(":"))
    except ValueError:
        raise argparse.ArgumentTypeError("--session PORT:QUEST:PLAYERS, e.g. 10301:131:2")
    if not 1 <= players <= 4:
        raise argparse.ArgumentTypeError("players: 1-4")
    return port, quest, players


async def serve(a):
    Limits.start_wait = a.start_wait
    lo, hi = (int(x) for x in a.relay_ports.split("-"))
    relay = Relay(a.bind, a.allow_public, (lo, hi), a.allow)
    for port, quest, players in a.session:
        s = await relay.open_session(port, quest, players, permanent=True)
        print("mh1-server relay session on %s:%d (quest %d, %d players)" % (a.bind, s.port, quest, players), flush=True)
    lobby = None
    if a.lobby_port is not None:
        from lobby_stub import Lobby
        db = AccountDB(a.db) if a.db else None
        lobby = Lobby(a.bind, a.lobby_port, accounts=db, relay=relay, relay_host=a.public_address or a.bind,
                      relay_matches=a.lobby_relay, loop=asyncio.get_event_loop())
        print("mh1-server lobby on %s:%d%s" % (a.bind, lobby.start(), " (accounts: %s)" % a.db if db else " (no accounts: any login)"),
              flush=True)
    stop = asyncio.Event()
    for sig in (signal.SIGINT, signal.SIGTERM):
        try:
            asyncio.get_event_loop().add_signal_handler(sig, stop.set)
        except (NotImplementedError, RuntimeError):
            pass        # Windows: Ctrl+C ends asyncio.run with KeyboardInterrupt
    await stop.wait()
    if lobby:
        lobby.stop()
    await relay.close()
    for h in relay.history:
        print("mh1-server hunt: %s" % json.dumps(h), flush=True)
    print("mh1-server stopped", flush=True)


def account_cmd(a):
    db = AccountDB(a.db)
    if a.what == "add":
        login, pw = db.create_account(a.login, a.password, a.note)
        print("login %s password %s" % (login, pw))
        print("(the password is shown only now; the player enters both in the game's server settings)")
    elif a.what == "list":
        for r in db.accounts():
            print("%s created %s last %s %s%s" % (r[0], r[1], r[2], "BANNED " + (r[4] or "") if r[3] else "", r[5] or ""))
    elif a.what in ("ban", "unban"):
        db.ban(a.login, a.note or "", a.what == "ban")
    elif a.what == "passwd":
        print("login %s new password %s" % (a.login, db.set_password(a.login, a.password)))
    elif a.what == "delete":
        db.delete_account(a.login)
    elif a.what == "export":
        print(json.dumps(db.export(a.login), indent=1))


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    sub = ap.add_subparsers(dest="cmd")
    s = sub.add_parser("serve", help="run the relay (and the lobby)")
    s.add_argument("--bind", default="127.0.0.1")
    s.add_argument("--allow-public", action="store_true", help="bind / accept public addresses (a real server)")
    s.add_argument("--allow", action="append", default=[], help="one public player address allowed on purpose")
    s.add_argument("--session", action="append", default=[], type=parse_session, help="PORT:QUEST:PLAYERS (permanent)")
    s.add_argument("--relay-ports", default="10301-10399", help="the pool for sessions the lobby makes")
    s.add_argument("--start-wait", type=float, default=300.0, help="seconds before a session starts with whoever came")
    s.add_argument("--lobby-port", type=int, help="also run the lobby (0 = any free port)")
    s.add_argument("--lobby-relay", action="store_true", help="matches go through the relay (6916; needs a client that joins it)")
    s.add_argument("--public-address", help="the address players reach this server at (for 6916)")
    s.add_argument("--db", help="SQLite account store; without it the lobby accepts any login")
    c = sub.add_parser("account", help="manage accounts")
    c.add_argument("what", choices=["add", "list", "ban", "unban", "passwd", "delete", "export"])
    c.add_argument("login", nargs="?")
    c.add_argument("--password")
    c.add_argument("--note", help="ban reason / operator note")
    c.add_argument("--db", default="mh1-server.sqlite3")
    a = ap.parse_args()
    if a.cmd == "serve":
        if not a.session and a.lobby_port is None:
            ap.error("nothing to serve: give --session and / or --lobby-port")
        try:
            asyncio.run(serve(a))
        except ValueError as e:
            sys.exit(str(e))
        except KeyboardInterrupt:
            pass
    elif a.cmd == "account":
        if a.what != "list" and a.what != "add" and not a.login:
            ap.error("which login?")
        account_cmd(a)
    else:
        ap.print_help()


if __name__ == "__main__":
    main()
