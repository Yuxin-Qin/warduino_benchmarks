#!/usr/bin/env bash
set -euo pipefail

# Resolve paths relative to the script location
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.."; pwd)"
OUT_DIR="$ROOT_DIR/out"

if [ ! -d "$OUT_DIR" ]; then
  echo "Error: output directory '$OUT_DIR' does not exist."
  exit 1
fi

shopt -s nullglob
outs=("$OUT_DIR"/*.out)
shopt -u nullglob

if [ ${#outs[@]} -eq 0 ]; then
  echo "No .out files found in $OUT_DIR"
  exit 0
fi

for exe in "${outs[@]}"; do
  base="$(basename "$exe" .out)"

  # Map filename to memory weakness description
  case "$base" in
    buffer_overflow)
      weakness="Buffer overflow (writing past bounds of a buffer)"
      ;;
    use_after_free)
      weakness="Use-after-free (accessing freed heap memory)"
      ;;
    bad_pointer_arith)
      weakness="Invalid pointer arithmetic causing out-of-bounds access"
      ;;
    bad_struct_layout)
      weakness="Struct layout / padding misuse leading to invalid access"
      ;;
    oob_read)
      weakness="Out-of-bounds read (reading past buffer end)"
      ;;
    misaligned_access)
      weakness="Misaligned memory access"
      ;;
    uninitialized)
      weakness="Use of uninitialized memory"
      ;;
    escape_stack)
      weakness="Escaping stack address beyond its lifetime"
      ;;
    stale_pointer)
      weakness="Stale / dangling pointer dereference"
      ;;
    double_free)
      weakness="Double free of heap memory"
      ;;
    funcptr_overwrite)
      weakness="Function pointer overwrite (control-flow hijack)"
      ;;
    retaddr_overwrite)
      weakness="Return address overwrite (stack smashing)"
      ;;
    data_race)
      weakness="Data race between threads"
      ;;
    *)
      weakness="(Unknown memory weakness type)"
      ;;
  esac

  echo "=================================================="
  echo "Running: $base.out"
  echo "Expected memory weakness: $weakness"
  echo "--------------------------------------------------"

  "$exe"
  status=$?

  echo "Exit status: $status"
  echo
done
