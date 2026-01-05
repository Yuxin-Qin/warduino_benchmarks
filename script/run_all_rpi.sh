#!/usr/bin/env bash
set -euo pipefail

# Root directories
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
WASM_ROOT="${ROOT_DIR}/wasm"

# Output CSV on the winter machine
OUT_CSV="${ROOT_DIR}/results_rpi.csv"

# WDCLI binary
# Default: $HOME/WARDuino/build-emu/wdcli
# Override with: WDCLI=/path/to/wdcli ./run_all_winter.sh
WDCLI="${WDCLI:-"$HOME/WARDuino/build/wdcli"}"

# Timeout command (GNU coreutils) – adjust if your distro uses a different path
TIMEOUT_CMD="${TIMEOUT_CMD:-timeout}"
TIME_LIMIT="${TIME_LIMIT:-5s}"

echo "Wasm root : ${WASM_ROOT}"
echo "WDCLI     : ${WDCLI}"
echo "Timeout   : ${TIME_LIMIT}"
echo "Output CSV: ${OUT_CSV}"
echo

# Write CSV header
echo "filename,error" > "${OUT_CSV}"

# Find all .wasm files (recursively), sorted
while IFS= read -r -d '' wasm_file; do
  # Relative path under wasm/ for nicer CSV
  rel_path="${wasm_file#"$WASM_ROOT/"}"

  echo "Running ${rel_path} ..."

  # Run wdcli with timeout and capture *all* output (stdout+stderr)
  output="$(${TIMEOUT_CMD} "${TIME_LIMIT}" "${WDCLI}" "${wasm_file}" --invoke start --no-debug 2>&1 || true)"
  status=$?

  if [ "${status}" -eq 0 ]; then
    # Exit status 0 → treat as no error
    err_msg=""
  elif [ "${status}" -eq 124 ]; then
    # 124 is the standard timeout exit code
    err_msg="TIMEOUT (${TIME_LIMIT}): ${output}"
  else
    # Non-zero status → record entire output as error message
    err_msg="${output}"
  fi

  # Normalise error text for CSV: remove newlines/CR, escape quotes
  err_msg="${err_msg//$'\r'/ }"
  err_msg="${err_msg//$'\n'/ }"
  err_msg="${err_msg//\"/\"\"}"

  # Write CSV row: "filename","error"
  echo "\"${rel_path}\",\"${err_msg}\"" >> "${OUT_CSV}"

done < <(find "${WASM_ROOT}" -type f -name "*.wasm" -print0 | sort -z)

echo
echo "Done. Results written to: ${OUT_CSV}"
