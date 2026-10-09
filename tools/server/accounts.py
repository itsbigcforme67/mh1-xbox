"""accounts.py - accounts, hunters and friends of mh1-server in SQLite (docs/server.md section 6).

What the game's login carries (docs/network.md 5.3, read from the client): an 8-digit number (the first 8 digits of
the Multi-Matching BB id, sent as two 5-digit numbers plus the packet's sequence number, mmbbc_encode) and a password
of up to 16 characters (obfuscated with "LOCKROCK", not encrypted). So an account here is a number of 8 digits ("login")
plus a password the server makes up: the player never chooses it and never reuses it elsewhere, which matters because
anyone who sees the traffic can undo the obfuscation (docs/server.md 7.1).

Passwords are stored as scrypt hashes (hashlib.scrypt; PBKDF2-SHA256 where Python's OpenSSL lacks scrypt). Failed
logins are rate limited per login and per address. Python 3.8+, standard library only.
"""
import hashlib
import hmac
import os
import secrets
import sqlite3
import string
import time

SCHEMA_VERSION = 1
SCHEMA = """
CREATE TABLE IF NOT EXISTS account (
    login       TEXT PRIMARY KEY,           -- 8 digits (what the game's login packet can carry)
    pw_hash     TEXT NOT NULL,              -- 'scrypt$n$r$p$salt$hash' or 'pbkdf2$iter$salt$hash' (hex)
    created     INTEGER NOT NULL,
    last_login  INTEGER,
    banned      INTEGER NOT NULL DEFAULT 0, -- 1: refused at login
    ban_reason  TEXT,
    note        TEXT                        -- free text for the operator (never shown to players)
);
CREATE TABLE IF NOT EXISTS hunter (
    hunter_id   TEXT PRIMARY KEY,           -- the id the server gives (6131 / 6132: 6-8 characters)
    login       TEXT NOT NULL REFERENCES account(login) ON DELETE CASCADE,
    handle      BLOB NOT NULL,              -- the hunter's name as the game sends it (Shift-JIS, up to 16 bytes)
    mini        BLOB,                       -- the 0x40-byte mini data (6190): equipment and look, public
    created     INTEGER NOT NULL,
    updated     INTEGER NOT NULL
);
CREATE INDEX IF NOT EXISTS hunter_login ON hunter(login);
CREATE TABLE IF NOT EXISTS friend (
    hunter_id   TEXT NOT NULL REFERENCES hunter(hunter_id) ON DELETE CASCADE,
    friend_id   TEXT NOT NULL REFERENCES hunter(hunter_id) ON DELETE CASCADE,
    added       INTEGER NOT NULL,
    PRIMARY KEY (hunter_id, friend_id)
);
CREATE TABLE IF NOT EXISTS mail (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    to_id       TEXT NOT NULL REFERENCES hunter(hunter_id) ON DELETE CASCADE,
    from_id     TEXT NOT NULL,
    body        BLOB NOT NULL,
    sent        INTEGER NOT NULL,
    delivered   INTEGER NOT NULL DEFAULT 0
);
CREATE TABLE IF NOT EXISTS audit (
    t           INTEGER NOT NULL,
    who         TEXT,                       -- login or hunter id
    what        TEXT NOT NULL               -- 'login ok', 'login failed', 'ban', 'report: ...'
);
"""
ID_CHARS = string.ascii_uppercase + string.digits


def _hash(password, salt=None):
    salt = salt or os.urandom(16)
    if hasattr(hashlib, "scrypt"):
        n, r, p = 1 << 14, 8, 1
        h = hashlib.scrypt(password.encode(), salt=salt, n=n, r=r, p=p, dklen=32)
        return "scrypt$%d$%d$%d$%s$%s" % (n, r, p, salt.hex(), h.hex())
    it = 200000
    h = hashlib.pbkdf2_hmac("sha256", password.encode(), salt, it)
    return "pbkdf2$%d$%s$%s" % (it, salt.hex(), h.hex())


def _check(password, stored):
    f = stored.split("$")
    if f[0] == "scrypt":
        n, r, p, salt, want = int(f[1]), int(f[2]), int(f[3]), bytes.fromhex(f[4]), f[5]
        got = hashlib.scrypt(password.encode(), salt=salt, n=n, r=r, p=p, dklen=32).hex()
    elif f[0] == "pbkdf2":
        got = hashlib.pbkdf2_hmac("sha256", password.encode(), bytes.fromhex(f[2]), int(f[1])).hex()
        want = f[3]
    else:
        return False
    return hmac.compare_digest(got, want)


class RateLimit:
    """failed attempts per key within a window; `blocked` once there were `limit` of them"""
    def __init__(self, limit=5, window=300.0):
        self.limit, self.window, self.fails = limit, window, {}

    def blocked(self, key, now=None):
        now = now or time.monotonic()
        lst = [t for t in self.fails.get(key, []) if now - t < self.window]
        self.fails[key] = lst
        return len(lst) >= self.limit

    def fail(self, key, now=None):
        self.fails.setdefault(key, []).append(now or time.monotonic())

    def clear(self, key):
        self.fails.pop(key, None)


class AccountDB:
    def __init__(self, path):
        self.db = sqlite3.connect(path, check_same_thread=False, isolation_level=None)
        self.db.execute("PRAGMA foreign_keys = ON")
        self.db.execute("PRAGMA journal_mode = WAL") if path != ":memory:" else None
        v = self.db.execute("PRAGMA user_version").fetchone()[0]
        if v > SCHEMA_VERSION:
            raise RuntimeError("%s is from a newer mh1-server (schema %d)" % (path, v))
        self.db.executescript(SCHEMA)
        self.db.execute("PRAGMA user_version = %d" % SCHEMA_VERSION)
        self.by_login = RateLimit(5, 300.0)
        self.by_addr = RateLimit(20, 300.0)

    def audit(self, who, what):
        self.db.execute("INSERT INTO audit VALUES (?, ?, ?)", (int(time.time()), who, what))

    # ---- accounts ----
    def create_account(self, login=None, password=None, note=None):
        """a new account; login: 8 digits (made up when None); password: made up when None (16 characters, the most
        the game sends). Returns (login, password): the only time the password is visible."""
        if login is None:
            for _ in range(100):
                login = "%08d" % secrets.randbelow(10 ** 8)
                if not self.db.execute("SELECT 1 FROM account WHERE login = ?", (login,)).fetchone():
                    break
        if len(login) != 8 or not login.isdigit():
            raise ValueError("a login is 8 digits (what the game's login packet carries)")
        if password is None:
            password = "".join(secrets.choice(ID_CHARS) for _ in range(16))
        if not 1 <= len(password) <= 16 or not all(32 < ord(c) < 127 for c in password):
            raise ValueError("a password is 1-16 printable ASCII characters (what the game sends)")
        self.db.execute("INSERT INTO account (login, pw_hash, created, note) VALUES (?, ?, ?, ?)",
                        (login, _hash(password), int(time.time()), note))
        self.audit(login, "account created")
        return login, password

    def set_password(self, login, password=None):
        password = password or "".join(secrets.choice(ID_CHARS) for _ in range(16))
        if not self.db.execute("UPDATE account SET pw_hash = ? WHERE login = ?", (_hash(password), login)).rowcount:
            raise KeyError(login)
        self.audit(login, "password changed")
        return password

    def verify(self, login, password, addr="?"):
        """'ok', 'bad' (unknown login or wrong password: the same answer for both), 'banned' or 'limited'"""
        if self.by_login.blocked(login) or self.by_addr.blocked(addr):
            self.audit(login, "login refused: rate limit (%s)" % addr)
            return "limited"
        row = self.db.execute("SELECT pw_hash, banned FROM account WHERE login = ?", (login,)).fetchone()
        if row is None:
            _check(password, _hash("x"))     # the same work as a real check: no timing hint that a login exists
        if row is None or not _check(password, row[0]):
            self.by_login.fail(login)
            self.by_addr.fail(addr)
            self.audit(login, "login failed (%s)" % addr)
            return "bad"
        if row[1]:
            self.audit(login, "login refused: banned")
            return "banned"
        self.by_login.clear(login)
        self.db.execute("UPDATE account SET last_login = ? WHERE login = ?", (int(time.time()), login))
        self.audit(login, "login ok")
        return "ok"

    def ban(self, login, reason, banned=True):
        if not self.db.execute("UPDATE account SET banned = ?, ban_reason = ? WHERE login = ?",
                               (1 if banned else 0, reason if banned else None, login)).rowcount:
            raise KeyError(login)
        self.audit(login, ("ban: " if banned else "unban: ") + (reason or ""))

    def delete_account(self, login):
        """everything about the account goes (its hunters, their friends and mail); the audit keeps the login only"""
        self.db.execute("DELETE FROM account WHERE login = ?", (login,))
        self.audit(login, "account deleted")

    def accounts(self):
        return self.db.execute("SELECT login, created, last_login, banned, ban_reason, note FROM account ORDER BY login").fetchall()

    # ---- hunters (6131 / 6132 / 6190) ----
    def hunters(self, login):
        """the account's hunters for 6131: [(hunter_id, handle, mini)]"""
        return self.db.execute("SELECT hunter_id, handle, mini FROM hunter WHERE login = ? ORDER BY created",
                               (login,)).fetchall()

    def add_hunter(self, login, handle, max_per_account=3):
        if len(self.hunters(login)) >= max_per_account:
            raise ValueError("an account has at most %d hunters" % max_per_account)
        now = int(time.time())
        for _ in range(100):
            hid = "".join(secrets.choice(ID_CHARS) for _ in range(6))
            try:
                self.db.execute("INSERT INTO hunter VALUES (?, ?, ?, NULL, ?, ?)", (hid, login, bytes(handle[:16]), now, now))
                return hid
            except sqlite3.IntegrityError:
                continue
        raise RuntimeError("no free hunter id")

    def hunter_owner(self, hunter_id):
        r = self.db.execute("SELECT login FROM hunter WHERE hunter_id = ?", (hunter_id,)).fetchone()
        return r[0] if r else None

    def set_mini(self, hunter_id, mini):
        self.db.execute("UPDATE hunter SET mini = ?, updated = ? WHERE hunter_id = ?", (bytes(mini[:0x40]), int(time.time()), hunter_id))

    # ---- friends and mail (6703-6705; formats not traced yet, docs/server.md 3.2) ----
    def add_friend(self, hunter_id, friend_id):
        self.db.execute("INSERT OR IGNORE INTO friend VALUES (?, ?, ?)", (hunter_id, friend_id, int(time.time())))

    def friends(self, hunter_id):
        return [r[0] for r in self.db.execute("SELECT friend_id FROM friend WHERE hunter_id = ? ORDER BY added", (hunter_id,))]

    def queue_mail(self, to_id, from_id, body):
        self.db.execute("INSERT INTO mail (to_id, from_id, body, sent) VALUES (?, ?, ?, ?)", (to_id, from_id, bytes(body), int(time.time())))

    def take_mail(self, hunter_id):
        rows = self.db.execute("SELECT id, from_id, body, sent FROM mail WHERE to_id = ? AND delivered = 0 ORDER BY id",
                               (hunter_id,)).fetchall()
        self.db.execute("UPDATE mail SET delivered = 1 WHERE to_id = ?", (hunter_id,))
        return [(r[1], r[2], r[3]) for r in rows]

    def prune(self, mail_days=30, audit_days=90):
        """data minimisation (docs/server.md 9): delivered mail and old audit lines go"""
        now = int(time.time())
        self.db.execute("DELETE FROM mail WHERE delivered = 1 OR sent < ?", (now - mail_days * 86400,))
        self.db.execute("DELETE FROM audit WHERE t < ?", (now - audit_days * 86400,))

    def export(self, login):
        """everything stored about one account (a player's data request)"""
        acc = self.db.execute("SELECT login, created, last_login, banned, ban_reason FROM account WHERE login = ?", (login,)).fetchone()
        hs = self.db.execute("SELECT hunter_id, hex(handle), hex(mini), created, updated FROM hunter WHERE login = ?", (login,)).fetchall()
        ids = [h[0] for h in hs]
        fr = [(i, self.friends(i)) for i in ids]
        return dict(account=acc, hunters=hs, friends=fr)
