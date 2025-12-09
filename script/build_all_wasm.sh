#!/usr/bin/env bash
set -euo pipefail

# Root paths (repo root = parent of this script)
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.."; pwd)"
SRC_ROOT="$ROOT_DIR/c"
WASM_ROOT="$ROOT_DIR/wasm"

mkdir -p "$WASM_ROOT"

# Use wasi-sdk clang in ~/Projects/wasi-sdk-29.0-x86_64-macos by default
: "${WASI_SDK_PATH:="$HOME/wasi-sdk-29.0-x86_64-macos"}"

CC="$WASI_SDK_PATH/bin/clang"
CFLAGS=${CFLAGS:--O2}
WASM_TARGET="wasm32-wasi"

echo "Source root : $SRC_ROOT"
echo "Output root : $WASM_ROOT"
echo "Compiler    : $CC"
echo "Target      : $WASM_TARGET"
echo

# Collect all .c files recursively under c/
shopt -s nullglob
cd "$SRC_ROOT"
c_files=( $(find . -name '*.c' -print) )
cd - >/dev/null
shopt -u nullglob

if [ ${#c_files[@]} -eq 0 ]; then
  echo "No .c files found under $SRC_ROOT"
  exit 0
fi

for rel in "${c_files[@]}"; do
  # rel is like ./823_Use_of_Out_of_range_Pointer_Offset/823_1.c
  src="$SRC_ROOT/$rel"
  rel_clean="${rel#./}"             # strip leading ./ if present
  rel_dir="$(dirname "$rel_clean")"
  base="$(basename "$rel_clean" .c)"

  out_dir="$WASM_ROOT/$rel_dir"
  mkdir -p "$out_dir"
  out="$out_dir/$base.wasm"

  echo "Compiling $rel_clean -> wasm/$rel_dir/$base.wasm"

  "$CC" \
    --target="$WASM_TARGET" \
    -nostdlib \
    -Wl,--no-entry \
    -Wl,--export=start \
    -Wl,--allow-undefined \
    $CFLAGS \
    "$src" -o "$out"
done

echo
echo "Done. Generated .wasm files in: $WASM_ROOT"
