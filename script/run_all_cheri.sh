#!/usr/bin/env bash
set -euo pipefail

# Run all .wasm in ../wasm with CHERI wdcli on amarena
# Output CSV: ../results_cheri.csv  (benchmark,output)
# Each run: up to 5 seconds, or less if it finishes earlier.

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ROOT_DIR="$( cd "${SCRIPT_DIR}/.." && pwd )"

WASM_DIR="${ROOT_DIR}/wasm"
CSV_OUT="${ROOT_DIR}/results_cheri.csv"

# Adjust this path if your CHERI wdcli lives elsewhere
WDCLI="${ROOT_DIR}/../WARDuino/build-purecap-hw/wdcli"

TIMEOUT_SEC=5

if [[ ! -x "${WDCLI}" ]]; then
  echo "Error: wdcli not found or not executable at: ${WDCLI}" >&2
  exit 1
fi

if [[ ! -d "${WASM_DIR}" ]]; then
  echo "Error: wasm directory not found: ${WASM_DIR}" >&2
  exit 1
fi

echo "Writing CSV to: ${CSV_OUT}" >&2
echo "Using wdcli: ${WDCLI}" >&2
echo "Timeout per benchmark: ${TIMEOUT_SEC}s" >&2

# CSV header
echo "benchmark,output" > "${CSV_OUT}"

run_one() {
  local wasm="$1"
  local name
  name="$(basename "${wasm}")"

  echo "Running ${name} ..." >&2

  local tmp_out
  tmp_out="$(mktemp)"

  # Run wdcli in background; capture stdout+stderr
  ( "${WDCLI}" "${wasm}" --invoke start --no-debug >"${tmp_out}" 2>&1 ) &
  local pid=$!
  local elapsed=0

  # Manual timeout (5s)
  while kill -0 "${pid}" 2>/dev/null; do
    if (( elapsed >= TIMEOUT_SEC )); then
      # Timed out: kill and record timeout
      kill "${pid}" 2>/dev/null || true
      sleep 0.2
      kill -9 "${pid}" 2>/dev/null || true
      wait "${pid}" 2>/dev/null || true

      local raw
      raw="$(cat "${tmp_out}" 2>/dev/null || true)"
      # Collapse newlines and escape quotes
      local out_str
      out_str="$(printf '%s' "${raw}" | tr '\n' ' ' | sed 's/"/""/g')"

      echo "\"${name}\",\"[TIMEOUT after ${TIMEOUT_SEC}s] ${out_str}\"" >> "${CSV_OUT}"
      rm -f "${tmp_out}"
      return
    fi
    sleep 1
    elapsed=$((elapsed + 1))
  done

  # Finished before timeout
  local status=0
  if ! wait "${pid}"; then
    status=$?
  fi

  local raw
  raw="$(cat "${tmp_out}" 2>/dev/null || true)"

  # Treat whitespace-only as empty
  local trimmed="${raw//[[:space:]]/}"

  # If it crashed (non-zero exit) and produced no meaningful output,
  # synthesize the CHERI error message.
  if (( status != 0 )) && [[ -z "${trimmed}" ]]; then
    raw="In-address space security exception (core dumped)"
  fi

  # Collapse newlines and escape quotes for CSV
  local out_str
  out_str="$(printf '%s' "${raw}" | tr '\n' ' ' | sed 's/"/""/g')"

  # For the CHERI CSV we just store the message itself, no [EXIT ...] prefix
  echo "\"${name}\",\"${out_str}\"" >> "${CSV_OUT}"

  rm -f "${tmp_out}"
}

shopt -s nullglob
wasms=( "${WASM_DIR}"/*.wasm )

if (( ${#wasms[@]} == 0 )); then
  echo "No .wasm files found in ${WASM_DIR}" >&2
  exit 0
fi

for w in "${wasms[@]}"; do
  run_one "${w}"
done

echo "Done. Results written to ${CSV_OUT}" >&2
