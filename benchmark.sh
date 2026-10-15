#!/usr/bin/env bash
set -euo pipefail
root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
llvm="${LLVM_BIN:-/usr/lib/llvm-22/bin}"
output="${OUTPUT_DIR:-$root/generated-benchmark}"
mkdir -p "$output"
for spec in '130-c1:130c1' '130-c3:130c3' '130-c4:130c4' '152:152' '203:203' '429:429' '432:432' '433:433'; do
 proof="${spec%%:*}"; symbol="${spec##*:}"
 sed "s/@cut/@b_${symbol}/" "$root/minotaur-rank${proof}-before.ll" | "$llvm/llc" -O=3 -filetype=obj -o "$output/b${symbol}.o"
 sed "s/@cut/@a_${symbol}/" "$root/minotaur-rank${proof}-after.ll" | "$llvm/llc" -O=3 -filetype=obj -o "$output/a${symbol}.o"
done
"$llvm/clang" -O3 -march=native "$root/benchmark.c" "$output"/*.o -o "$output/bench"
"$output/bench" "${1:-20000000}" "${2:-21}" > "$output/results.csv" 2> "$output/stderr.txt"
cat "$output/stderr.txt"
