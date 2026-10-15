# We Scanned 99 Agave Hotspots With Minotaur—Here Is What Survived Formal Verification

**Series:** VIQERO Agave Performance Engineering  
**Author:** Ahmed Alkhazaali, VIQERO, INC.  
**Field:** Validator performance engineering and compiler optimization  
**Research artifact version:** Agave `825efd18292aff6ffcf9daa0f7612f21b3531a72`  
**Evidence repository:** https://github.com/CodeRush387/agave-minotaur-evidence

## Abstract

This study tests whether an offline LLVM superoptimization workflow can discover correct, locally faster expressions in production Agave validator code without modifying consensus behavior. We selected 99 source positions from a CPU profile, mapped all positions to LLVM IR, and analyzed 1,667 function-deduplicated cuts with Minotaur. Candidate transformations were checked using Alive2 refinement proofs and then evaluated with paired A/B microbenchmarks using LLVM 22.1.2: LLVM cuts were compiled with `llc -O=3`, and the C harness with Clang `-O3 -march=native`. Minotaur produced nine candidates in six compiled functions. Alive2 proved eight and rejected one because the target did not preserve all source `undef` behaviors. For each surviving transformation, three sessions of 21 paired samples were collected, with 20 million calls per version per sample. Median paired time reductions ranged from 9.69% to 43.59%; 501 of 504 paired observations favored the transformed version. The six source functions represented approximately 0.12% of the sampled CPU profile. These results establish eight correct and repeatably faster isolated Agave-specific LLVM expressions, but do not establish an end-to-end validator speedup. The study demonstrates a reproducible separation between candidate discovery, semantic correctness, local performance, and system-level significance.

**Keywords:** Agave; Solana validator; LLVM IR; superoptimization; Minotaur; Alive2; formal verification; microbenchmarking

## 1. Research Question

Can profile-guided, offline LLVM superoptimization find transformations in real Agave validator code that are both formally correct and measurably faster after standard `-O3` optimization? The unit of analysis is an isolated LLVM expression, not a full Rust function or validator process.

## 2. Background and Scope

Performance work on a validator is full of attractive traps. A shorter function is not necessarily faster. A microbenchmark is not a validator benchmark. A compiler-generated rewrite that looks obvious may be wrong because of overflow, poison, or undefined values.

We wanted a workflow that separates promising ideas from transformations we can actually defend. We used a saved Agave CPU-profile-derived position map to select 99 concrete optimization positions, mapped them to emitted LLVM IR, searched with Minotaur, checked the proposed rewrites with Alive2, and measured the surviving cuts in paired microbenchmarks. The published package validates the isolated outputs; it is not a complete reconstruction of the profiling and discovery environment.

The result was deliberately modest: nine proposed rewrites across six Agave functions. Alive2 proved eight and rejected one. The eight valid isolated expressions showed median local speedups from 9.69% to 43.59%. The six functions represented only about 0.12% of the sampled CPU profile, so this is not evidence of a 43% faster validator.

That distinction is the most important result of the experiment.

## 3. Materials and Methods

### 3.1 Subject and Selection Corpus

The subject was Agave commit `825efd18292aff6ffcf9daa0f7612f21b3531a72`. No source-level implementation of these eight rewrites was merged into Agave. Extraction used separate analysis builds, including a disposable worktree with `#[inline(never)]` annotations to expose otherwise inlined functions. In particular, ranks 432 and 433 came from noinline analysis IR; the other four candidate-bearing functions came from emitted validator-analysis IR. This distinction prevents treating the isolated cuts as evidence of production-build gains. The experiment selected 99 ranked source positions from the recorded validator CPU profile. Mapping produced 87 unique compiled functions; 12 positions were duplicate views of functions already included.

### 3.2 Analysis Pipeline

Our analysis pipeline was:

```text
Agave CPU profile
→ 99 ranked source positions
→ Rust/LLVM IR mapping
→ 1,667 function-deduplicated Minotaur cuts
→ synthesized candidate rewrites
→ Alive2 refinement proofs
→ dynamic output checks
→ repeated paired A/B microbenchmarks
```

All 99 selected positions were mapped successfully. They corresponded to 87 unique compiled functions; 12 positions were duplicate views of functions already present in the set. The saved ledger records completed or duplicate status for all 99 positions. This status summary is not a new independent rerun of every discovery job.

Most positions produced no candidate. That matters. A superoptimizer is not a machine that guarantees a faster implementation for every function. It searches a bounded local space. Many important costs in a validator come from memory layout, synchronization, cache behavior, I/O, data structures, or algorithms—none of which can be fixed by replacing a small integer expression.

Minotaur analyzed 1,667 function-deduplicated cuts. A run was considered complete only when the position was mapped, every scheduled cut terminated without analysis error or timeout, and its result was recorded. The preserved execution ledger contains 88 `COMPLETE` rows and 11 `DUPLICATE` rows. A symbol-level audit identifies 87 unique compiled functions and 12 repeated positions: ranks 130 and 190 resolve to the identical IR file and symbol, but both were recorded as `COMPLETE`. Rank 190 therefore represents an additional execution, not an additional unique function. We preserve the original ledger and provide a separate duplicate audit. Summing `COMPLETE` rows counts 1,792 cuts; removing this repeated 125-cut function yields the reported 1,667 function-deduplicated cuts. This is deduplication by compiled-function identity, not a claim that every cut is structurally distinct across functions.

### 3.3 Candidate Validation

Every synthesized candidate was submitted to Alive2 as a source-to-target refinement problem. A candidate entered measurement only after a successful proof. Dynamic output equality was additionally checked to detect harness wiring errors; it was not treated as a substitute for the formal proof.

### 3.4 Measurement Protocol

Each proven before/after LLVM cut was renamed to a distinct externally linked symbol and compiled separately with LLVM 22.1.2 `llc -O=3 -filetype=obj`. The C harness was compiled and linked with Clang `-O3 -march=native`. The `-march=native` flag applies to the harness; the saved build script does not pass a native-CPU option to `llc`. Object separation prevents cross-function inlining in this harness without link-time optimization. Each paired sample executed 20 million calls per version on the same pre-generated input stream. Three independent sessions of 21 paired samples yielded 63 paired observations per rewrite. Within each session, execution order alternated by round: even rounds execute before then after, odd rounds execute after then before. The saved CSV `order` labels are reversed relative to this execution order; timing columns remain correctly assigned. Each version receives a one-million-call warm-up per candidate before timed rounds. CPU affinity to logical CPU 0 is attempted, but the saved harness does not check the affinity call return value, so successful pinning cannot be asserted from this code alone. We report medians, paired wins, and a 95% bootstrap confidence interval for the median paired speedup.

For paired sample i, let B_i and A_i be total before and after nanoseconds, and N_i the iteration count. We report median(B_i/N_i) and median(A_i/N_i) as ns/call. The percentage column is median(100 * (B_i - A_i) / B_i), across the same 63 pairs. It is a relative elapsed-time reduction, not a throughput multiplier and not 100 * (1 - median(A)/median(B)). For example, rank130-c3 has a median paired reduction of 43.5948%, while the ratio-of-medians reduction is 43.2244%; the two estimators need not agree. The isolated loop includes function-call and harness overhead, which has not been subtracted. It measures amortized wall time per call, not individual-call latency percentiles.

The revision provides a deterministic percentile bootstrap of the paired reductions: 10,000 resamples of 63 pairs with replacement, RNG seed 20261015, and the empirical 2.5th/97.5th percentile endpoints. These intervals capture paired-sample variation, not independent-machine or system-level uncertainty. Samples are pooled across three sessions rather than bootstrapped as session clusters.

The measurements were collected under WSL2 Linux 6.18.33.2 on an Intel Core i7-1265U with eight logical CPUs. Hardware, virtualization, compiler flags, and the isolated harness limit external validity and are therefore part of the reported experimental context.

## 4. Results

### 4.1 Candidate Discovery

The nine raw candidates came from six functions:

- `RepairWeight::get_best_weighted_repairs`
- `BlockIdRepairService::run_repair_iteration`
- `RepairWeight::set_root`
- `ClusterSlots::update`
- `ForwardingStage::run`
- `PacketContainer::pop_max`

The rewrites mostly simplified compiler-generated overflow, alignment, boundary, and Boolean expressions. Examples included:

- reducing a chain of `umax`, shift, comparisons, and Boolean OR to one unsigned comparison;
- reducing an overflow condition to extraction of a single high bit;
- replacing alignment arithmetic followed by equality with a direct comparison against one constant;
- removing redundant Boolean conditions around nested selects;
- proving that one comparison always returns false under the source IR semantics.

These are isolated peephole transformations rather than changes to networking policy, data structures, or scheduling. Their proof results do not establish safety of an unimplemented source-level or production integration.

### 4.2 Formal Verification and the Rejected Candidate

Random testing is useful, but it cannot prove an LLVM rewrite correct for every input. LLVM IR also has semantic details that ordinary algebra can miss: poison propagation, flags such as `nuw` and `nsw`, unreachable paths, and undefined values.

Alive2 checks whether the target transformation refines the source. Eight Minotaur candidates passed. One candidate did not.

The rejected transformation replaced an expression equivalent to a rounded unsigned half with a subtraction-based form. On normal concrete inputs, the two can appear equivalent. Alive2 found that the source and target did not agree for all allowed `undef` behaviors. We excluded it before benchmarking.

This rejection is not a failure of the workflow. It is evidence that the verification stage did its job.

### 4.3 Local Performance

| Rewrite | Median before | Median after | Median paired time reduction | 95% bootstrap CI | Paired wins |
|---|---:|---:|---:|---:|---:|
| rank130-c1 | 1.1155 ns | 0.9405 ns | 15.28% | 13.56–16.49% | 62/63 |
| rank130-c3 | 1.5067 ns | 0.8555 ns | 43.59% | 43.00–44.19% | 63/63 |
| rank130-c4 | 1.2410 ns | 0.8301 ns | 32.71% | 31.97–33.55% | 63/63 |
| rank152 | 1.7033 ns | 1.4975 ns | 11.86% | 11.17–12.99% | 62/63 |
| rank203 | 1.1510 ns | 0.9233 ns | 20.17% | 19.23–20.92% | 63/63 |
| rank429 | 1.2825 ns | 0.8508 ns | 32.62% | 31.22–33.66% | 63/63 |
| rank432 | 0.9794 ns | 0.8812 ns | 9.69% | 8.62–10.39% | 62/63 |
| rank433 | 1.0304 ns | 0.8155 ns | 20.72% | 19.93–21.80% | 63/63 |

The before and after cuts are compiled into separate object functions; the supplied runner allows reviewers to inspect their assembly with their own matching LLVM toolchain.

Across the eight transformations, 501 of 504 paired samples favored the target. This consistency supports the narrow claim that the isolated targets were faster in this environment; it does not enlarge the scope of the measurement.

### Concrete LLVM transformation

Candidate rank130-c3 was extracted from `RepairWeight::get_best_weighted_repairs`. Here is the exact saved source cut:

```llvm
declare i64 @llvm.umax.i64(i64, i64)
define i1 @cut(i64 %n5, i16 %n6) {
  %n0 = call i64 @llvm.umax.i64(i64 %n5, i64 4)
  %n1 = shl i64 %n0, 4
  %n2 = icmp ugt i64 %n5, 1152921504606846975
  %n3 = icmp ugt i64 %n1, 9223372036854775800
  %n4 = or i1 %n2, %n3
  ret i1 %n4
}
```

The synthesized target is:

```llvm
define i1 @cut(i64 %n5, i16 %n6) {
  %r = icmp ugt i64 %n5, 576460752303423487
  ret i1 %r
}
```

The target replaces a maximum, shift, two comparisons and an OR with one unsigned comparison. The unused i16 argument is retained to preserve the isolated function interface. The first source comparison catches large inputs before wrapped shift results could obscure the condition; the combined condition reduces to n5 > 2^59 - 1. Alive2, rather than this informal explanation, supplies the refinement check. The local 43.59% paired time reduction applies to this cut and harness only.

## 5. Discussion

The experiment answers the research question affirmatively at the isolated-expression level. It also produced a negative result of practical importance: most selected positions yielded no candidate, including functions with substantially greater profile weight. `StatusCache::get_status` (5.72%), `check_transactions_with_processed_slots` (2.14%), `get_transaction` (1.70%), `check_transaction_age` (1.60%), and `recent_blockhash` (0.50%) together represented 11.66% of the sample but yielded no local rewrite. The absence of a local candidate does not identify their bottleneck. Algorithmic, memory-layout, cache, synchronization, and data-flow changes are separate hypotheses to investigate.

## 6. Limitations and Threats to Validity

The largest number in the table, 43.59%, describes one isolated LLVM expression. It does not describe the full Rust function and certainly does not describe the full validator.

The six distinct functions contributed roughly 0.12% in the saved profile-derived position map, using rounded percentages. This is not a fresh measurement or a verified mainnet-workload share. Even eliminating all of their sampled cost would impose a small Amdahl’s-law ceiling on whole-program speedup under that workload. In practice, each rewrite covers only part of its function, so the realistic end-to-end effect is smaller.

Different workloads may make repair, cluster-slot processing, or forwarding hotter. That is a reason to test realistic scenarios—not a reason to extrapolate from the microbenchmark.

Additional limitations include profile dependence, WSL2 scheduling and frequency effects, a single CPU model, compiler-version dependence, and selection bias toward cuts expressible to the synthesis workflow. Bootstrap intervals quantify sampling uncertainty in the paired runs; they do not quantify these external-validity limits.

## 7. Related Work and the Correct Novelty Claim

Minotaur, Souper, Alive2, and LLVM InstCombine establish substantial prior art for superoptimization, formal refinement checking, and peephole canonicalization. We did not invent those methods or the general optimization classes.

Our documented search did not find these exact eight Agave-specific instances, constants, proofs, and measurements in the public corpus examined. We present these as Agave-specific cases documented by this study, not as established globally novel or legally protectable optimization patterns.

## 8. Reproducibility and Data Availability

Public evidence: https://github.com/CodeRush387/agave-minotaur-evidence. The repository provides all eight accepted source/target LLVM pairs and the excluded ninth pair, fresh proof logs, the historical three-session raw measurements, the original benchmark harness, portable validation/measurement scripts, a sanitized 99-position corpus table, and a deterministic statistical analysis script.

The repository supports independent verification and isolated microbenchmarking of the reported transformations. It does not include the internal discovery orchestration, full generated Agave LLVM modules, or a complete profiling/build environment, and does not claim to reproduce candidate discovery from scratch. Absolute machine paths and unrelated operational material are excluded. Corpus IR identities are opaque hashes used only to audit duplicate membership; they are not downloadable module hashes or substitutes for the omitted modules.

The saved discovery environment used locally patched Minotaur cache handling, compatibility adjustments and query limits. The evidence package does not depend on executing that discovery environment: reviewers directly check the published source/target pairs and benchmark them. A clean-host, full-pipeline reconstruction and independent hardware replication remain future work.

The historical measurements are preserved unchanged. New smoke-test outputs are kept separate and are not substituted for the original performance results.

## 9. Future Work

The next technical step is not to merge eight magic constants into production code. It is to generalize each useful family into a compiler rewrite or a source-level formulation, verify the generalized rule, run the LLVM and Agave test suites, and then compare full Agave builds under an identical validator workload.

For any production claim we need at least:

- source-level or compiler-level integration;
- identical original and experimental configurations;
- repeated end-to-end throughput, latency, and CPU measurements;
- confidence intervals and regression checks;
- confirmation that the optimized path is exercised at meaningful frequency.

## 10. Conclusion

The experiment found eight correct and measurably faster local rewrites. More importantly, it produced a reproducible filter for performance claims:

```text
profile first
→ synthesize candidates
→ prove correctness
→ measure locally
→ integrate carefully
→ measure the whole system
```

That pipeline gives us something more useful than a dramatic percentage: it tells us exactly what has been proven, exactly what has only been measured, and exactly what remains unknown.

## Author Contributions

Ahmed Alkhazaali, VIQERO: project direction and authorship. AI coding assistance was used for computational workflow implementation, artifact auditing, analysis, and manuscript preparation. The published files identify the exact inputs and outputs supporting the technical claims. Minotaur, Alive2, LLVM, and Agave are credited to their respective authors.

## Funding and Conflicts of Interest

The work was conducted for VIQERO, INC., which has an interest in validator performance engineering. This article is a technical case study, not evidence of endorsement by Anza, Solana, LLVM, Minotaur, or HackerNoon.

## References

- Minotaur repository: https://github.com/minotaur-toolkit/minotaur
- Minotaur paper: https://arxiv.org/abs/2306.00229
- Alive2: https://github.com/AliveToolkit/alive2
- Souper: https://github.com/google/souper
- Agave: https://github.com/anza-xyz/agave
