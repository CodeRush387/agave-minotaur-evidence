# Agave LLVM transformation evidence

Research evidence accompanying Ahmed Alkhazaali's VIQERO case study, “We Scanned 99 Agave Hotspots With Minotaur—Here Is What Survived Formal Verification.”

This package supports verification and isolated microbenchmarking of eight accepted LLVM transformations and inspection of one excluded candidate. It is not a production Agave patch or a complete candidate-discovery pipeline. No full-validator acceleration is established.

## Contents

- Eighteen LLVM files: nine exact source and target pairs.
- Nine saved verification logs: eight successful refinements, one counterexample.
- Three unchanged historical benchmark CSVs: 63 pairs per accepted transformation.
- Original `benchmark.c` harness, portable `benchmark.sh`, `verify.py`, and `analyze.py`.
- `corpus.csv`: sanitized position map and recorded statuses, including the duplicate-classification discrepancy.
- `ARTICLE.md`, provenance and rights notices.

No internal search scheduler, Minotaur cache modifications, full Agave IR modules, validator keys, credentials, or operational configuration are included.

## Run

Use Python 3.10+ and LLVM/Clang 22.1.2. Set `LLVM_BIN` if llc/clang are installed elsewhere.

```sh
python3 analyze.py
ALIVE_TV=/path/to/compatible/alive-tv python3 verify.py
bash benchmark.sh 20000000 21
```

Alive2 must support the supplied LLVM syntax; see `PROVENANCE.md` for the locally checked revision. Inspect textual proof status rather than exit code alone. The rejected candidate must fail verification; it is excluded from the benchmark.

For three new measurement sessions, run with distinct `OUTPUT_DIR` values. Do not replace the historical CSVs. Small iteration counts are smoke tests, not evidence of performance. Timing values are amortized elapsed loop times including call/harness cost; results can differ across machines.

The original C harness uses a deterministic 65,536-element input pool, one-million-call warm-ups and alternating execution order. Its CSV AB/BA labels are reversed relative to execution; before_ns/after_ns columns are correctly assigned. CPU0 affinity is attempted without checking success. Rank433's supplementary dynamic equality check masks the sign bit, whereas its timing loop uses the wider stream; the LLVM proof, not randomized testing, supports refinement under poison/overflow semantics.

`analyze.py` computes median paired elapsed-time reduction, not reduction from marginal medians. Bootstrap intervals resample 63 pairs 10,000 times with seed 20261015, pooled across sessions. They do not establish independent hardware replication.

## Scope

99 positions map to 87 unique compiled functions and 12 repeated positions. The historical ledger had 88 COMPLETE and 11 DUPLICATE entries because rank190 repeated rank130 but was recorded COMPLETE. COMPLETE-row cuts total 1,792; excluding that extra 125-cut execution gives 1,667 function-deduplicated cuts. The independent outputs verify 501 wins out of 504 pairs.

## Rights

See `NOTICE.md` and `LICENSE-AGAVE.txt`. Publishing results does not imply endorsement by upstream projects or an exclusive claim to established optimization methods. The package is intentionally limited to supporting evidence, not the internal discovery implementation.
