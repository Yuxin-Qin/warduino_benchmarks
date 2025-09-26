# ./Script/compile_all_wasm.sh
#!/usr/bin/env bash
set -euo pipefail

# why: make paths predictable regardless of where script is invoked
SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
SRC_DIR="$ROOT_DIR/C"
OUT_DIR="$ROOT_DIR/Wasm"

command -v clang >/dev/null 2>&1 || { echo "Error: clang not found in PATH." >&2; exit 127; }
[[ -d "$SRC_DIR" ]] || { echo "Error: missing source dir: $SRC_DIR" >&2; exit 1; }
mkdir -p "$OUT_DIR"

shopt -s nullglob
found_any=0
for src in "$SRC_DIR"/*.c; do
  found_any=1
  base="$(basename "${src%.c}")"
  out="$OUT_DIR/$base.wasm"
  echo "Compiling: $src -> $out"
  clang --target=wasm32 -nostdlib \
        -Wl,--no-entry -Wl,--export-all -Wno-implicit-function-declaration -Wl,--allow-undefined \
        -O2 -o "$out" "$src"
done
shopt -u nullglob

if [[ "$found_any" -eq 0 ]]; then
  echo "No .c files found in $SRC_DIR"
else
  echo "Done. Output in $OUT_DIR"
fi
