#!/bin/sh
# Cross-compile check of the PC build's game C for the original Xbox (nxdk).
# Records every compile command of tools/build_pc.sh (FULL=1, via a CC
# wrapper), then replays the -c commands with nxdk-cc. Objects and the log go
# to build/xbox/ (gitignored). Needs the toolchain from docs/xbox.md
# (~/xboxdev/env.sh). Nothing is linked.
cd "$(dirname "$0")/.."
[ -f "$HOME/xboxdev/env.sh" ] || { echo "no ~/xboxdev/env.sh (see docs/xbox.md)"; exit 1; }
REC=build/xbox/cc; rm -rf build/xbox; mkdir -p $REC
W="$HOME/xboxdev/ccrec.sh"     # CC must be a path without spaces (build_pc.sh does not quote it)
cat > "$W" <<EOF
#!/bin/sh
printf '%s\0' "\$@" > "$PWD/$REC/\$(date +%s%N).\$\$.args"
exec gcc "\$@"
EOF
chmod +x "$W"
FULL=1 CC="$W" tools/build_pc.sh > build/xbox/build_pc.log 2>&1 || { echo "build_pc.sh failed"; exit 1; }
. "$HOME/xboxdev/env.sh"
python3 tools/xbox_replay.py $REC
