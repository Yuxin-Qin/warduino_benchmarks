#!/usr/bin/env bash
set -euo pipefail

# Root paths
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.."; pwd)"
SRC_DIR="$ROOT_DIR/c"
WASM_DIR="$ROOT_DIR/wasm"

mkdir -p "$WASM_DIR"

# Choose compiler and flags (edit these if you use a different toolchain)
CC="${CC:-clang}"
CFLAGS="${CFLAGS:--O2}"
# Typical bare WebAssembly target (adjust if you use wasi or emcc)
WASM_TARGET="${WASM_TARGET:-wasm32-unknown-unknown}"

echo "Source dir : $SRC_DIR"
echo "Output dir : $WASM_DIR"
echo "Compiler   : $CC"
echo "Target     : $WASM_TARGET"
echo

shopt -s nullglob
c_files=("$SRC_DIR"/*.c)
shopt -u nullglob

if [ ${#c_files[@]} -eq 0 ]; then
  echo "No .c files found in $SRC_DIR"
  exit 0
fi

for src in "${c_files[@]}"; do
  base="$(basename "$src" .c)"
  out="$WASM_DIR/$base.wasm"

  echo "Compiling $base.c -> wasm/$base.wasm"

  "$CC" \
    --target="$WASM_TARGET" \
    -nostdlib \
    -Wl,--no-entry \
    -Wl,--export-all \
    -Wl,--allow-undefined \
    $CFLAGS \
    "$src" -o "$out"
done

echo
echo "Done. Generated .wasm files in: $WASM_DIR"
