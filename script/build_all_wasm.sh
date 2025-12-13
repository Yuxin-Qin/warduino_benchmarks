#!/usr/bin/env bash
set -euo pipefail

# Root paths (repo root = parent of this script)
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.."; pwd)"
SRC_ROOT="$ROOT_DIR/c"
WASM_ROOT="$ROOT_DIR/wasm"

mkdir -p "$WASM_ROOT"

###############################################################################
# Choose compiler
###############################################################################
# Preferred wasi-sdk location (you can override with WASI_SDK_PATH in env)
: "${WASI_SDK_PATH:="$HOME/Projects/wasi-sdk-29.0-x86_64-macos"}"

PREF_CC="$WASI_SDK_PATH/bin/clang"

if [ -x "$PREF_CC" ]; then
  CC="$PREF_CC"
else
  # Fallback to whatever 'clang' is in PATH
  CC="$(command -v clang || true)"
  if [ -z "$CC" ]; then
    echo "Error: no usable clang found (neither $PREF_CC nor system clang)." >&2
    exit 1
  fi
fi

# You can override CFLAGS / WASM_TARGET via env if needed
CFLAGS=${CFLAGS:--O2}
WASM_TARGET="${WASM_TARGET:-wasm32}"

echo "Source root : $SRC_ROOT"
echo "Output root : $WASM_ROOT"
echo "Compiler    : $CC"
echo "Target      : $WASM_TARGET"
echo

###############################################################################
# Collect all .c files recursively under c/
###############################################################################
shopt -s nullglob
cd "$SRC_ROOT"
c_files=( $(find . -name '*.c' -print) )
cd - >/dev/null
shopt -u nullglob

if [ ${#c_files[@]} -eq 0 ]; then
  echo "No .c files found under $SRC_ROOT"
  exit 0
fi

###############################################################################
# Compile each .c → corresponding .wasm under wasm/ with mirrored dirs
###############################################################################
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
    -I"$SRC_ROOT" \
    -nostdlib \
    -Wl,--no-entry \
    -Wl,--export=all \
    -Wno-implicit-function-declaration \
    -Wl,--allow-undefined \
    $CFLAGS \
    "$src" -o "$out"
done

echo
echo "Done. Generated .wasm files in: $WASM_ROOT"
