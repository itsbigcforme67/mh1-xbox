"""mcs.py - the PS2-faithful game-session relay ("Mcs", the mcsls records), a skeleton. docs/server.md 4.2.

The real game's in-hunt traffic (docs/network.md 1a, read from the client: cmcs_04 in lb_cmcs04.c, CnInetMcsReceive and
CnInetCheckMcsPacket2 in cnmsg01.c, mcsls_recv / mcsls_send in mcsls_r0_nm.c):

  1. the client opens TCP to the address the lobby gave in 6916;
  2. the server sends command 0x1031 (12-byte header, sequence number at bytes 6-7);
  3. the client answers once with a 0x82 packet: header (magic 0x82, cat 2, cmd 0x1031, len, the server's sequence),
     payload `u16 10` + 10 digits = mmbbc_encode(login key, seq), as in the lobby login;
  4. from then on the client sends only records: [u8 len][u8 cmd << 4 | sender id] + len - 2 bytes; len counts the whole
     record. cmd 0x2 ping, 0x3 pong, 0x4 / 0x5 app data (whole / fragment), 0x7 / 0x8 the same padded, 0x9 sync flag,
     0xF keep-alive (also the answer to a health check). Records carry no destination: everything is for everyone;
  5. the server's own packets in the stream start with the byte 0x28 and have the 12-byte header (payload length at
     bytes 4-5, command at 2-3): 0x1021 health check (the client answers with a keep-alive record), 0x1032 (meaning
     unknown, the client ignores it). A client never makes a record of length 0x28 (that is how the two are told apart);
  6. the server passes each client's records on, unchanged, to the other members, never back to the sender (a record
     with the receiver's own id is a sync error for it, mcsls_recv).

Not used by any client yet: the PC build replaces this layer with net_peer.c (relay.py). It is here so that real PS2s
(and a future "faithful" PC mode that links mcsls) have a server to talk to. Untested against a real client: only the
synthetic tests in test_server.py. Python 3.8+, standard library only.
"""
import asyncio
import struct
import time

SRV_MAGIC = 0x28
CLI_MAGIC = 0x82
CMD_LOGIN = 0x1031
CMD_HEALTH = 0x1021
CMD_ACK = 0x1032
REC_KEEPALIVE = 0xF0


def server_packet(cmd, seq=0, payload=b"", cat=1):
    """a server packet as the client's session receive expects it: 0x28, cat, cmd, len, seq, result 0, FF FF FF"""
    return struct.pack(">BBHHHB3s", SRV_MAGIC, cat, cmd, len(payload), seq, 0, b"\xff\xff\xff") + payload


class ClientStream:
    """splits what one client sends: first its 0x82 login packet, then records"""
    def __init__(self):
        self.buf = b""
        self.logged_in = False

    def feed(self, data):
        """yields ('login', cmd, seq, payload) once, then ('rec', bytes) per record; raises ValueError on garbage"""
        self.buf += data
        while True:
            if not self.logged_in:
                if len(self.buf) < 12:
                    return
                magic, cat, cmd, ln, seq = struct.unpack(">BBHHH", self.buf[:8])
                if magic != CLI_MAGIC:
                    raise ValueError("first packet is not a 0x82 packet")
                if len(self.buf) < 12 + ln:
                    return
                payload, self.buf = self.buf[12:12 + ln], self.buf[12 + ln:]
                self.logged_in = True
                yield ("login", cmd, seq, payload)
                continue
            if len(self.buf) < 2:
                return
            n = self.buf[0]
            if n < 2 or n == SRV_MAGIC:
                raise ValueError("bad record length %d" % n)
            if len(self.buf) < n:
                return
            rec, self.buf = self.buf[:n], self.buf[n:]
            yield ("rec", rec)


class McsMember:
    def __init__(self, writer, pid):
        self.writer, self.id = writer, pid
        self.t_last = time.monotonic()


class McsSession:
    """one match: the lobby says which login keys belong to it and their player ids (6912 MatchPlSide - 1)"""
    def __init__(self, keys):
        self.keys = dict(keys)          # 8-digit login -> player id
        self.members = {}               # id -> McsMember
        self.relayed = 0

    def relay(self, sender, rec):
        """pass one record on; the sender id in the record must be the sender's own (no spoofing)"""
        if rec[1] & 0x0F != sender.id:
            return False
        for pid, m in self.members.items():
            if pid != sender.id:
                m.writer.write(rec)
        self.relayed += 1
        return True


class McsServer:
    """accepts game-session connections, puts each into its match by login key, relays, sends health checks"""
    health_every = 10.0
    silent_limit = 40.0

    def __init__(self, decode_key, log=print):
        self.decode_key = decode_key    # (10 digits, seq) -> 8-digit login (lobby_stub.decode_login_key)
        self.sessions = []
        self.log = log
        self.seq = 1

    def add_match(self, keys):
        s = McsSession(keys)
        self.sessions.append(s)
        return s

    async def on_connect(self, reader, writer):
        self.seq = (self.seq + 1) & 0xFFFF or 1
        writer.write(server_packet(CMD_LOGIN, self.seq))
        stream, sess, me = ClientStream(), None, None
        hc = asyncio.ensure_future(self.health(writer))
        try:
            while True:
                data = await asyncio.wait_for(reader.read(4096), self.silent_limit)
                if not data:
                    break
                for item in stream.feed(data):
                    if item[0] == "login":
                        _, cmd, seq, p = item
                        n = struct.unpack(">H", p[:2])[0] if len(p) >= 2 else 0
                        login = self.decode_key(p[2:2 + n], seq) if cmd == CMD_LOGIN else None
                        sess = next((s for s in self.sessions if login in s.keys), None)
                        if sess is None:
                            self.log("mcs: unknown login %s" % login)
                            return
                        me = McsMember(writer, sess.keys[login])
                        sess.members[me.id] = me
                        writer.write(server_packet(CMD_ACK, seq, cat=2))     # guess: 0x1032 acknowledges the login
                    elif not sess.relay(me, item[1]):
                        self.log("mcs: player %d sent a record with another id" % me.id)
                        return
                await writer.drain()
        except (asyncio.TimeoutError, ConnectionError, OSError, ValueError) as e:
            self.log("mcs: connection dropped (%s)" % (type(e).__name__,))
        finally:
            hc.cancel()
            if sess is not None and me is not None and sess.members.get(me.id) is me:
                del sess.members[me.id]
            writer.close()

    async def health(self, writer):
        while True:
            await asyncio.sleep(self.health_every)
            writer.write(server_packet(CMD_HEALTH))
