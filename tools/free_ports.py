#!/usr/bin/env python3
"""free_ports.py [N] : print the first of N consecutive TCP ports on 127.0.0.1 that are free now (default 1).

For the online tests: a fixed port (the co-op host's 10300 + room, a relay range, an mcs port) is shared by every
worktree, and two agents' full checks running the same test at once took each other's port: "net_peer: cannot listen
on 0.0.0.0:10301", and the joiner joined the other run's host (test_online_town's "ANNA did not start the co-op quest",
agent C round 12). A random base between 20000 and 60000, each port checked by binding it; the small race until the
test binds it itself is left. Standard library only."""
import random
import socket
import sys


def free(p):
    for host in ('127.0.0.1', '0.0.0.0'):
        s = socket.socket()
        try:
            s.bind((host, p))
        except OSError:
            return False
        finally:
            s.close()
    return True


def main():
    n = int(sys.argv[1]) if len(sys.argv) > 1 else 1
    for _ in range(200):
        base = random.randrange(20000, 60000 - n)
        if all(free(base + k) for k in range(n)):
            print(base)
            return
    sys.exit('free_ports: no %d free ports in a row' % n)


if __name__ == '__main__':
    main()
