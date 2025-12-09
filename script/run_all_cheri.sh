#!/usr/bin/env bash
set -euo pipefail

# Root directories
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
WASM_ROOT="${ROOT_DIR}/wasm"

# Output CSV for CHERI runs
OUT_CSV="${ROOT_DIR}/results_cheri.csv"

# WDCLI binary on CHERI
# Default: ../WARDuino/build-purecap-hw/wdcli
# You can override with: WDCLI=/path/to/wdcli ./run_all_wasm_cheri.sh
WDCLI="${WDCLI:-${ROOT_DIR}/../WARDuino/build-purecap-hw/wdcli}"

echo "Wasm root : ${WASM_ROOT}"
echo "WDCLI     : ${WDCLI}"
echo "Output CSV: ${OUT_CSV}"
echo

# Write CSV header
echo "filename,error" > "${OUT_CSV}"

# Find all .wasm files (recursively), sorted
while IFS= read -r -d '' wasm_file; do
  # Relative path under wasm/ for nicer CSV
  rel_path="${wasm_file#"$WASM_ROOT/"}"

  echo "Running ${rel_path} ..."

  # Run wdcli and capture *all* output (stdout+stderr)
  if output="$("${WDCLI}" "${wasm_file}" --invoke start --no-debug 2>&1)"; then
    # Exit status 0 → treat as no error
    err_msg=""
  else
    # Non-zero status → record entire output as error message
    err_msg="${output}"
  fi

  # Normalise error text for CSV: remove newlines, escape quotes
  err_msg="${err_msg//$'\r'/ }"
  err_msg="${err_msg//$'\n'/ }"
  err_msg="${err_msg//\"/\"\"}"

  # Write CSV row: "filename","error"
  echo "\"${rel_path}\",\"${err_msg}\"" >> "${OUT_CSV}"

done < <(find "${WASM_ROOT}" -type f -name "*.wasm" -print0 | sort -z)

echo
echo "Done. Results written to: ${OUT_CSV}"
