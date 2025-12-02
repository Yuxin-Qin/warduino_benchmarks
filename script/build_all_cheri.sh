#!/usr/bin/env bash
set -euo pipefail

# Directory of this script
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
# Project root = parent of script dir
ROOT_DIR="$( cd "${SCRIPT_DIR}/.." && pwd )"

SRC_DIR="${ROOT_DIR}/c"
OUT_DIR="${ROOT_DIR}/cheri"

mkdir -p "${OUT_DIR}"

echo "Building CHERI purecap benchmarks..."
echo "  Source dir: ${SRC_DIR}"
echo "  Output dir: ${OUT_DIR}"
echo

for cfile in "${SRC_DIR}"/*.c; do
    base="$(basename "${cfile}" .c)"
    out="${OUT_DIR}/${base}_purecap"

    echo "Compiling ${base}.c -> $(basename "${out}")"
    clang -march=morello -mabi=purecap -O2 \
          -nostartfiles -nostdlib \
          -Wl,-e,start \
          "${cfile}" -o "${out}"
done

echo
echo "All CHERI binaries built in: ${OUT_DIR}"
