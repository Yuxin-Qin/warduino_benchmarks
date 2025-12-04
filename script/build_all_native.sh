#!/usr/bin/env bash
set -euo pipefail

# Resolve paths relative to this script: repo_root / c / out
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.."; pwd)"
SRC_DIR="$ROOT_DIR/c"
OUT_DIR="$ROOT_DIR/out"

# Compiler and flags (can override by env: CC=... CFLAGS=...)
CC=${CC:-clang}
CFLAGS=${CFLAGS:--O0 -g}

# macOS: use start() as entry symbol (C symbol -> _start)
ENTRY_FLAGS="-Wl,-e,_start -nostartfiles"

mkdir -p "$OUT_DIR"

for src in "$SRC_DIR"/*.c; do
    base="$(basename "$src" .c)"
    out="$OUT_DIR/$base.out"

    # Extra flags per benchmark (data_race needs pthread)
    extra_flags=""
    if [[ "$base" == "data_race" ]]; then
        extra_flags="-pthread"
    fi

    echo "Compiling $src -> $out"
    $CC $CFLAGS $extra_flags $ENTRY_FLAGS "$src" -o "$out"
done

echo "All benchmarks built into: $OUT_DIR"
