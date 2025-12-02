#!/usr/bin/env bash
set -euo pipefail

# Resolve paths relative to this script: repo_root / c / out
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.."; pwd)"
SRC_DIR="$ROOT_DIR/c"
OUT_DIR="$ROOT_DIR/out"

# Compiler and flags (can override by env: CC=... CFLAGS=...)
CC=${CC:-clang}
CFLAGS=${CFLAGS:--O0 -g}

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
    $CC $CFLAGS $extra_flags "$src" -o "$out"
done

echo "All benchmarks built into: $OUT_DIR"
