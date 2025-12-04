#!/usr/bin/env bash
set -euo pipefail

# Run all .wasm in ../wasm with wdcli on Raspberry Pi
# Output CSV: ../results_pi.csv  (benchmark,output)
# Each run: up to 5 seconds, or stop earlier if it finishes.

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ROOT_DIR="$( cd "${SCRIPT_DIR}/.." && pwd )"

WASM_DIR="${ROOT_DIR}/wasm"
CSV_OUT="${ROOT_DIR}/results_pi.csv"

# Adjust this to your Pi build of wdcli
WDCLI="${ROOT_DIR}/../WARDuino/build-native-sw/wdcli"

TIMEOUT_SEC=5

if [[ ! -x "${WDCLI}" ]]; then
  echo "Error: wdcli not found or not executable at: ${WDCLI}" >&2
  exit 1
fi

if [[ ! -d "${WASM_DIR}" ]]; then
  echo "Error: wasm directory not found: ${WASM_DIR}" >&2
  exit 1
fi

echo "Writing Pi CSV to: ${CSV_OUT}" >&2
echo "Using wdcli: ${WDCLI}" >&2
echo "Timeout per benchmark: ${TIMEOUT_SEC}s" >&2

echo "benchmark,output" > "${CSV_OUT}"

run_one() {
  local wasm="$1"
  local name
  name="$(basename "${wasm}")"

  echo "Running ${name} ..." >&2

  local tmp_out
  tmp_out="$(mktemp)"

  ( "${WDCLI}" "${wasm}" --invoke start --no-debug >"${tmp_out}" 2>&1 ) &
  local pid=$!
  local elapsed=0

  while kill -0 "${pid}" 2>/dev/null; do
    if (( elapsed >= TIMEOUT_SEC )); then
      kill "${pid}" 2>/dev/null || true
      sleep 0.2
      kill -9 "${pid}" 2>/dev/null || true
      wait "${pid}" 2>/dev/null || true

      local raw
      raw="$(cat "${tmp_out}" 2>/dev/null || true)"
      local out_str
      out_str="$(printf '%s' "${raw}" | tr '\n' ' ' | sed 's/"/""/g')"

      echo "\"${name}\",\"[TIMEOUT after ${TIMEOUT_SEC}s] ${out_str}\"" >> "${CSV_OUT}"
      rm -f "${tmp_out}"
      return
    fi
    sleep 1
    elapsed=$((elapsed + 1))
  done

  local status=0
  if ! wait "${pid}"; then
    status=$?
  fi

  local raw
  raw="$(cat "${tmp_out}" 2>/dev/null || true)"
  local trimmed="${raw//[[:space:]]/}"

  # If the program exits non zero and prints nothing, record that explicitly
  if (( status != 0 )) && [[ -z "${trimmed}" ]]; then
    raw="(non zero exit, no output)"
  fi

  local out_str
  out_str="$(printf '%s' "${raw}" | tr '\n' ' ' | sed 's/"/""/g')"

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

echo "Done. Pi results written to ${CSV_OUT}" >&2
