#!/usr/bin/env sh 

set -eu

PARENT_DIR=$(dirname "$0")

SCRIPT_PATH="/home/ghidra"
SCRIPT_NAME=Decompile.java

PROJECT_DIR="/tmp"
PROJECT_NAME="$1"
PROJECT_IMPORT="$1"

SOURCE_NAME="$1.c"

shift

docker build -t ghidra "$PARENT_DIR/ghidra"

docker run --init --rm \
	--volume "$PWD:/home/ghidra/share:rw" \
	-it ghidra \
	/ghidra/support/analyzeHeadless "$PROJECT_DIR" "$PROJECT_NAME" \
		-import "$PROJECT_IMPORT" \
		-scriptPath "$SCRIPT_PATH" -postscript "$SCRIPT_NAME" "$SOURCE_NAME" \
		"$@"
