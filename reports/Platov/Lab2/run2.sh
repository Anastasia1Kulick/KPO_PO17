#!/usr/bin/env bash
set -e

LAB_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SRC="$LAB_DIR/TestLab2.cpp"
OUT_BIN="/tmp/lab2_tests"

if [[ ! -f "$SRC" ]]; then
    echo "ERROR: File $SRC not found!" >&2
    exit 1
fi

g++ -std=c++17 -Wall -Wextra -I "$LAB_DIR" -o "$OUT_BIN" "$SRC"
"$OUT_BIN"