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

  # Run wdcli in background; capture stdout+stderr of wdcli
  ( "${WDCLI}" "${wasm}" --invoke start --no-debug >"${tmp_out}" 2>&1 ) &
  local pid=$!
  local elapsed=0

  # Poll once per second up to TIMEOUT_SEC
  while kill -0 "${pid}" 2>/dev/null; do
    if (( elapsed >= TIMEOUT_SEC )); then
      # Timeout: kill process and mark as such
      kill "${pid}" 2>/dev/null || true
      sleep 0.2
      kill -9 "${pid}" 2>/dev/null || true
      wait "${pid}" 2>/dev/null || true

      local raw
      raw="$(cat "${tmp_out}" || true)"
      # Shell message "In-address space security exception ..." is not in raw;
      # keep whatever we have, may be empty.
      local out_str
      out_str="$(printf '%s' "${raw}" | tr '\n' ' ' | sed 's/"/""/g')"

      echo "\"${name}\",\"[TIMEOUT after ${TIMEOUT_SEC}s] ${out_str}\"" >> "${CSV_OUT}"
      rm -f "${tmp_out}"
      return
    fi
    sleep 1
    elapsed=$((elapsed + 1))
  done

  # Process finished before timeout
  local status=0
  if ! wait "${pid}"; then
    status=$?
  fi

  local raw
  raw="$(cat "${tmp_out}" || true)"

  # If non-zero exit and no output captured, inject CHERI message
  if (( status != 0 )) && [[ -z "${raw}" ]]; then
    raw="In-address space security exception (core dumped)"
  fi

  local out_str
  out_str="$(printf '%s' "${raw}" | tr '\n' ' ' | sed 's/"/""/g')"

  if (( status == 0 )); then
    echo "\"${name}\",\"${out_str}\"" >> "${CSV_OUT}"
  else
    echo "\"${name}\",\"[EXIT ${status}] ${out_str}\"" >> "${CSV_OUT}"
  fi

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
