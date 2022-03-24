#!/usr/bin/env sh 

set -eu

PARENT_DIR=$(dirname "$0")

docker build -t radare2 "$PARENT_DIR/r2"

[ $# -gt 0 ] && cd "$1"

docker run --rm --volume "$PWD:/home/r2/share:ro" -it radare2
