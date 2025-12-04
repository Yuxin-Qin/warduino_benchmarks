#!/usr/bin/env bash
set -euo pipefail

# ---------- Config ----------
TIMEOUT_SEC="${TIMEOUT_SEC:-5}"  # default 5 seconds

# Directory of this script
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
# Project root = parent of script dir
ROOT_DIR="$( cd "${SCRIPT_DIR}/.." && pwd )"

WASM_DIR="${ROOT_DIR}/wasm"
OUT_CSV="${ROOT_DIR}/result_cheri.csv"

# Adjust this path if needed
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

echo "Using wdcli:  $WDCLI"
echo "Wasm dir:     $WASM_DIR"
echo "Output CSV:   $OUT_CSV"
echo "Timeout each run: ${TIMEOUT_SEC}s"
echo

# ---------- helper: run with timeout, capture output ----------
run_with_timeout() {
  local timeout="$1"; shift
  local tmp_out
  local tmp_flag
  tmp_out="$(mktemp)"
  tmp_flag="$(mktemp)"

  # Run command in background, capture stdout+stderr
  "$@" >"$tmp_out" 2>&1 &
  local cmd_pid=$!

  # Killer in background
  (
    sleep "$timeout"
    if kill -0 "$cmd_pid" 2>/dev/null; then
      echo "TIMEOUT after ${timeout}s" >"$tmp_flag"
      kill "$cmd_pid" 2>/dev/null || true
    fi
  ) &

  local killer_pid=$!

  # Wait for command
  wait "$cmd_pid" 2>/dev/null || true

  # Stop killer if still around
  kill "$killer_pid" 2>/dev/null || true

  # Read output
  local out_text
  out_text="$(cat "$tmp_out")"

  # If timeout flag exists, append marker
  if [[ -s "$tmp_flag" ]]; then
    out_text="${out_text} [TIMEOUT]"
  fi

  rm -f "$tmp_out" "$tmp_flag"

  # Echo for caller to capture
  echo "$out_text"
}

# ---------- main loop ----------
echo "benchmark,output" > "$OUT_CSV"

shopt -s nullglob
for wasm in "$WASM_DIR"/*.wasm; do
  base="$(basename "$wasm" .wasm)"
  echo "Running $base.wasm ..."

  output="$(run_with_timeout "$TIMEOUT_SEC" "$WDCLI" "$wasm" --invoke start --no-debug)"

  # Flatten and escape for CSV
  sanitized="$(echo "$output" | tr '\n' ' ' | tr '\r' ' ')"
  sanitized="${sanitized//\"/\"\"}"

  echo "$base,\"$sanitized\"" >> "$OUT_CSV"
done

echo
echo "Done. Results written to: $OUT_CSV"
