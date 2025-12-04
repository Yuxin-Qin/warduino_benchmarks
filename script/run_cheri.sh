#!/usr/bin/env bash
set -euo pipefail

# Directory of this script
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
# Project root = parent of script dir
ROOT_DIR="$( cd "${SCRIPT_DIR}/.." && pwd )"

WASM_DIR="${ROOT_DIR}/wasm"
OUT_CSV="${ROOT_DIR}/cheri_results.csv"

# Path to wdcli (override with: WDCLI=/path/to/wdcli ./run_all_wasm.sh)
WDCLI="${WDCLI:-${ROOT_DIR}/../WARDuino/build-purecap-hw/wdcli}"

if [[ ! -x "$WDCLI" ]]; then
  echo "Error: wdcli not found or not executable at: $WDCLI" >&2
  echo "Set WDCLI=/path/to/wdcli and try again." >&2
  exit 1
fi

if [[ ! -d "$WASM_DIR" ]]; then
  echo "Error: wasm directory not found: $WASM_DIR" >&2
  exit 1
fi

echo "Using wdcli: $WDCLI"
echo "Wasm dir:    $WASM_DIR"
echo "Output CSV:  $OUT_CSV"
echo

# Write CSV header
echo "benchmark,output" > "$OUT_CSV"

shopt -s nullglob
for wasm in "$WASM_DIR"/*.wasm; do
  base="$(basename "$wasm" .wasm)"
  echo "Running $base.wasm ..."

  # Capture stdout + stderr
  output="$("$WDCLI" "$wasm" --invoke start --no-debug 2>&1 || true)"

  # Flatten newlines and carriage returns
  sanitized="$(echo "$output" | tr '\n' ' ' | tr '\r' ' ')"
  # Escape double quotes for CSV
  sanitized="${sanitized//\"/\"\"}"

  # Append CSV line
  echo "$base,\"$sanitized\"" >> "$OUT_CSV"
done

echo
echo "Done. Results written to: $OUT_CSV"
