#!/usr/bin/env python3
"""Download the native mwccps2 3.0 build 52 (for MH_PS2_COMPILER=native, see
tools/build.py) from mweppc's release into tools/compilers/mwccps2-b52-native64.

    python3 tools/get_native_mwcc.py [--release TAG]
"""
import argparse
import io
import os
import platform
import sys
import tarfile
import urllib.request

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
REPO = "sozud/mweppc"
RELEASE = "mwccps2-b52-v1.0.0"


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--release", default=RELEASE)
    a = ap.parse_args()
    osname = {"darwin": "darwin", "linux": "linux"}.get(sys.platform)
    arch = {"x86_64": "amd64", "amd64": "amd64", "arm64": "arm64", "aarch64": "arm64"}.get(platform.machine().lower())
    if not osname or not arch:
        sys.exit("no native build for %s/%s: use the default MH_PS2_COMPILER=wibo" % (sys.platform, platform.machine()))
    name = "mwccps2-b52-native64-%s-%s" % (osname, arch)
    url = "https://github.com/%s/releases/download/%s/%s.tar.gz" % (REPO, a.release, name)
    with urllib.request.urlopen(url, timeout=120) as r:
        data = r.read()
    with tarfile.open(fileobj=io.BytesIO(data), mode="r:gz") as tar:
        exe = tar.extractfile(name).read()
    dst = os.path.join(ROOT, "tools/compilers/mwccps2-b52-native64")
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    with open(dst, "wb") as f:
        f.write(exe)
    os.chmod(dst, 0o755)
    print("%s (%s)" % (os.path.relpath(dst, ROOT), url))


if __name__ == "__main__":
    main()
