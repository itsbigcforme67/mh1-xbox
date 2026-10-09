"""relay.py - the in-hunt session relay of mh1-server (docs/server.md section 4).

Does what the hosting player's game does today in direct-connect co-op (src/pc/net/net_peer.c, docs/network.md 3.4),
as a standalone service, so players behind NAT can hunt together: every player's game connects OUT to the relay with
the existing `--join RELAY --port P` (no client change), the relay gives out the slots, collects the mini data, starts
the quest and passes each player's frames on to the others.

Wire format: net_peer.c's frames, unchanged (little endian): u16 n, u8 from (sender slot), u8 type, n - 2 bytes.
    type 1..10   a game packet for AQ channel `type`            relayed to everyone else, `from` = the sender's slot
    0x40 HELLO   joiner -> host: the joiner's mini data (0x2C)  kept for START, not relayed
    0x41 WELCOME host -> joiner: u8 your slot                   only the relay sends it
    0x42 START   host -> all: u8 quest, u8 players, mini x n    only the relay sends it
    0x43 BYE     a player leaves                                 relayed (from = the leaver)
    0x44 READY   u8 round: the start barrier                     relayed

The differences from a player's game hosting:
  * every player is a joiner; slot 0 is simply the first one in (the game treats slot 0 as the quest's host by slot
    number, not by who listens: rt_np.c, mcsls_calc_master_id), so the supply box and the first monsters work as before;
  * when slot 0 leaves, the others keep hunting (the game hands the monsters on, Em_Master_Change); in the direct
    connect mode the hosting player's leaving ended the session for everyone;
  * the relay checks what it forwards: frame sizes, types a player may send, a rate limit, an idle limit.

A session listens on its own TCP port (the clients have no field for a session code yet; docs/server.md 4.3 proposes
one). Sessions come from the command line (`--session PORT:QUEST:PLAYERS`, permanent: after a hunt the port takes the
next group) or from the lobby (`Relay.create_session`, one hunt, then the port is returned to the pool).

Python 3.8+, standard library only (asyncio).
"""
import asyncio
import ipaddress
import struct
import time

NP_MAX = 4
NP_PKT_MAX = 0x200
NP_MINI = 0x2C
HELLO, WELCOME, START, BYE, READY = 0x40, 0x41, 0x42, 0x43, 0x44
GAME_TYPES = set(range(1, 11))          # AQ channels (rt_np.c self_data_ctrl: 1-4 players, 6 chat, 7 host, 8 em, 10 sys)
CLIENT_TYPES = GAME_TYPES | {HELLO, BYE, READY}

# the MH Oldschool addresses (docs/network.md 2.1): never relayed to, never accepted as a peer (CLAUDE.md)
MHOS = {"34.75.107.68", "151.80.238.99", "151.80.238.101", "151.80.238.104"}


def frame(frm, typ, data=b""):
    return struct.pack("<HBB", len(data) + 2, frm & 0xFF, typ) + data


def is_private(host):
    try:
        ip = ipaddress.ip_address(host)
    except ValueError:
        return host == "localhost"
    return ip.is_loopback or ip.is_private or ip.is_link_local


class Limits:
    """per-member limits; the defaults are about 10x what a 4-player hunt measured (docs/server.md 3.3)"""
    rate = 300.0            # frames per second, sustained
    burst = 600             # frames, token bucket size
    hello_timeout = 30.0    # seconds from connect to HELLO
    idle_after_start = 120.0    # seconds without a frame during a hunt -> dropped
    start_wait = 300.0      # seconds after the first player before starting with whoever came (RT_NP_WAIT's default)
    grace = 0.1             # seconds between "everyone said hello" and START (the host's sleep_ms(100))
    max_backlog = 256 * 1024    # bytes waiting for a player who does not read -> dropped


class Member:
    def __init__(self, sess, reader, writer, slot):
        self.sess, self.reader, self.writer, self.slot = sess, reader, writer, slot
        self.peer = writer.get_extra_info("peername") or ("?", 0)
        self.mini = None
        self.t_last = time.monotonic()
        self.t_join = self.t_last
        self.tokens = float(Limits.burst)
        self.t_tok = self.t_last
        self.frames = 0
        self.bytes = 0
        self.alive = True

    def take_token(self):
        now = time.monotonic()
        self.tokens = min(Limits.burst, self.tokens + (now - self.t_tok) * Limits.rate)
        self.t_tok = now
        if self.tokens < 1:
            return False
        self.tokens -= 1
        return True

    def send(self, data):
        if self.alive:
            try:
                self.writer.write(data)
            except (ConnectionError, OSError):
                self.alive = False
                return
            if self.writer.transport.get_write_buffer_size() > Limits.max_backlog:
                self.sess.drop(self, "not reading (more than %d bytes waiting)" % Limits.max_backlog)


class Session:
    def __init__(self, relay, port, quest, players, permanent=False, names=None):
        self.relay, self.port, self.quest, self.want = relay, port, quest, max(1, min(NP_MAX, players))
        self.permanent = permanent
        self.names = names or []        # the lobby's member order (hunter names), to keep its slot numbers
        self.server = None
        self.reset()

    def reset(self):
        self.members = {}               # slot -> Member
        self.started = False
        self.t_first = None
        self.start_task = None
        self.stats = dict(frames=0, bytes=0, t_start=None, t_end=None, peak_players=0)

    def log(self, *a):
        self.relay.log("session :%d" % self.port, *a)

    # ---- connections ----
    async def on_connect(self, reader, writer):
        host = (writer.get_extra_info("peername") or ("?",))[0]
        if not self.relay.peer_allowed(host):
            self.log("refused %s (not an allowed address)" % host)
            writer.close()
            return
        if self.started or len(self.members) >= self.want:
            self.log("refused %s (%s)" % (host, "hunt running" if self.started else "full"))
            writer.close()
            return
        slot = min(s for s in range(NP_MAX) if s not in self.members)
        m = Member(self, reader, writer, slot)
        self.members[slot] = m
        if self.t_first is None:
            self.t_first = time.monotonic()
        self.stats["peak_players"] = max(self.stats["peak_players"], len(self.members))
        sock = writer.get_extra_info("socket")
        if sock is not None:
            import socket
            try:
                sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
                sock.setsockopt(socket.SOL_SOCKET, socket.SO_KEEPALIVE, 1)
            except OSError:
                pass
        self.log("player from %s:%d gets slot %d" % (m.peer[0], m.peer[1], slot))
        m.send(frame(0, WELCOME, bytes([slot])))
        try:
            await self.read_loop(m)
        except (ConnectionError, OSError, asyncio.IncompleteReadError):
            pass
        finally:
            self.drop(m, "connection closed")

    async def read_loop(self, m):
        while m.alive:
            timeout = Limits.idle_after_start if self.started else (Limits.hello_timeout if m.mini is None else None)
            try:
                hdr = await asyncio.wait_for(m.reader.readexactly(4), timeout)
            except asyncio.TimeoutError:
                self.drop(m, "silent too long")
                return
            n, frm, typ = struct.unpack("<HBB", hdr)
            if n < 2 or n > NP_PKT_MAX + 2:
                self.drop(m, "bad frame length %d" % n)
                return
            data = await m.reader.readexactly(n - 2) if n > 2 else b""
            m.t_last = time.monotonic()
            m.frames += 1
            m.bytes += n + 2
            if not m.take_token():
                self.drop(m, "too many frames (rate limit)")
                return
            if not self.handle(m, typ, data):
                return
            await m.writer.drain()

    def handle(self, m, typ, data):
        if typ not in CLIENT_TYPES:
            self.drop(m, "frame type 0x%02X is not a player's to send" % typ)
            return False
        if typ == HELLO:
            if m.mini is not None or self.started:
                return True                 # a second hello: ignored
            m.mini = (data + bytes(NP_MINI))[:NP_MINI]
            self.log("slot %d hello (%s)" % (m.slot, self.relay.mini_name(m.mini)))
            self.maybe_start()
            return True
        if typ == BYE:
            self.drop(m, "left")
            return False
        f = frame(m.slot, typ, data)       # the relay knows who sent it (as the host does)
        self.stats["frames"] += 1
        self.stats["bytes"] += len(f)
        for s, o in list(self.members.items()):
            if s != m.slot:
                o.send(f)
        return True

    def drop(self, m, why):
        if not m.alive and self.members.get(m.slot) is not m:
            return
        m.alive = False
        try:
            m.writer.close()
        except (ConnectionError, OSError, RuntimeError):
            pass
        if self.members.get(m.slot) is m:
            del self.members[m.slot]
            self.log("slot %d gone: %s (%d frames, %d bytes from it)" % (m.slot, why, m.frames, m.bytes))
            if self.started:
                bye = frame(m.slot, BYE)
                for o in self.members.values():
                    o.send(bye)
            if not self.members:
                self.finish()
            elif not self.started:
                self.maybe_start()

    # ---- starting ----
    def maybe_start(self):
        if self.started:
            return
        ready = [x for x in self.members.values() if x.mini is not None]
        if ready and len(ready) == len(self.members) and len(self.members) >= self.want:
            self.schedule_start(Limits.grace)
        elif self.start_task is None and self.t_first is not None:
            left = Limits.start_wait - (time.monotonic() - self.t_first)
            self.schedule_start(max(0.0, left))

    def schedule_start(self, delay):
        if self.start_task is not None:
            self.start_task.cancel()
        self.start_task = asyncio.ensure_future(self._start_after(delay))

    async def _start_after(self, delay):
        await asyncio.sleep(delay)
        self.start_task = None
        self.start()

    def start(self):
        ms = [m for m in self.members.values() if m.mini is not None]
        if self.started or not ms:
            return
        for m in list(self.members.values()):       # no hello by now: not part of this hunt
            if m.mini is None:
                self.drop(m, "no hello before the start")
        # slots: the lobby's order when it gave one (by hunter name), else the order of arrival; always 0..n-1
        order = sorted(self.members.values(), key=lambda m: (self.lobby_rank(m), m.slot))
        renum = {}
        for new, m in enumerate(order):
            if m.slot != new:
                renum[m] = new
        if renum:
            for m, new in renum.items():
                m.slot = new
                m.send(frame(0, WELCOME, bytes([new])))     # net_peer.c takes a later WELCOME as the slot
            self.members = {m.slot: m for m in order}
        n = len(order)
        d = bytes([self.quest & 0xFF, n]) + b"".join(m.mini for m in order)
        f = frame(0, START, d)
        for m in order:
            m.send(f)
        self.started = True
        self.stats["t_start"] = time.monotonic()
        self.log("quest %d started with %d player(s): %s" % (self.quest, n, ", ".join(
            "%d=%s" % (m.slot, self.relay.mini_name(m.mini)) for m in order)))

    def lobby_rank(self, m):
        nm = self.relay.mini_name(m.mini)
        return self.names.index(nm) if nm in self.names else NP_MAX + m.slot

    def finish(self):
        st = self.stats
        if st["t_start"] is not None:
            secs = max(0.001, time.monotonic() - st["t_start"])
            self.log("hunt over: %d frames, %d bytes in %.0f s (%.1f frames/s, %.0f bytes/s relayed in)" % (
                st["frames"], st["bytes"], secs, st["frames"] / secs, st["bytes"] / secs))
            self.relay.history.append(dict(port=self.port, quest=self.quest, players=st["peak_players"],
                                           frames=st["frames"], bytes=st["bytes"], secs=secs))
        if self.start_task is not None:
            self.start_task.cancel()
        if self.permanent:
            self.reset()
        else:
            self.relay.close_session(self)


class Relay:
    def __init__(self, bind="127.0.0.1", allow_public=False, ports=(10301, 10399), allow=(), verbose=True, logf=None):
        if not is_private(bind) and bind not in ("0.0.0.0", "::") and not allow_public:
            raise ValueError("refusing to bind %s: only loopback / private addresses unless --allow-public" % bind)
        if bind in ("0.0.0.0", "::") and not allow_public:
            raise ValueError("binding every interface needs --allow-public")
        self.bind = bind
        self.allow_public = allow_public
        self.allow = set(allow)
        self.ports = list(range(ports[0], ports[1] + 1))
        self.sessions = {}              # port -> Session
        self.history = []
        self.verbose = verbose
        self.logf = logf

    def log(self, *a):
        if self.verbose:
            line = " ".join(str(x) for x in (time.strftime("%H:%M:%S"),) + a)
            print(line, flush=True, file=self.logf) if self.logf else print(line, flush=True)

    @staticmethod
    def mini_name(mini):
        """the hunter's name in the mini data (0x18.., Shift-JIS, our addition in net_peer's 0x2C layout), as hex"""
        if not mini:
            return "?"
        n = mini[0x18:0x2A].split(b"\0")[0]
        return n.hex(" ").upper() if n else "unnamed"

    def peer_allowed(self, host):
        if host in MHOS:
            return False
        return self.allow_public or is_private(host) or host in self.allow

    async def open_session(self, port, quest, players, permanent=False, names=None):
        if port in self.sessions:
            raise ValueError("port %d already has a session" % port)
        s = Session(self, port, quest, players, permanent, names)
        s.server = await asyncio.start_server(s.on_connect, self.bind, port)
        s.port = s.server.sockets[0].getsockname()[1]
        self.sessions[s.port] = s
        self.log("session :%d open: quest %d, %d player(s)%s" % (s.port, quest, s.want, ", permanent" if permanent else ""))
        return s

    async def create_session(self, quest, players, names=None):
        """for the lobby (6916 MatchMcsIpAddr): a one-hunt session on a free port of the pool; returns its port"""
        for p in self.ports:
            if p not in self.sessions:
                try:
                    s = await self.open_session(p, quest, players, names=names)
                    return s.port
                except OSError:
                    continue
        raise RuntimeError("no free relay port")

    def close_session(self, s):
        if self.sessions.get(s.port) is s:
            del self.sessions[s.port]
        if s.server is not None:
            s.server.close()
        self.log("session :%d closed" % s.port)

    async def close(self):
        for s in list(self.sessions.values()):
            for m in list(s.members.values()):
                s.drop(m, "server stopping")
            if s.server is not None:
                s.server.close()
                await s.server.wait_closed()
        self.sessions.clear()
