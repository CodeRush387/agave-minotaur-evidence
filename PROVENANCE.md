# Provenance and verification scope

Agave reference commit: `825efd18292aff6ffcf9daa0f7612f21b3531a72`.

Candidate-bearing functions: RepairWeight::get_best_weighted_repairs, BlockIdRepairService::run_repair_iteration, RepairWeight::set_root, ClusterSlots::update, ForwardingStage::run and PacketContainer::pop_max.

Ranks432/433 used separate noinline analysis IR. Other candidate-bearing functions used emitted validator-analysis IR. The discovery worktree used inline(never) annotations; this package is not evidence that the same opportunities survive ordinary production compilation.

The saved profile-derived selection corpus is provided as sanitized data, not a complete binary/profile/build chain. Historical notes distinguish earlier instrumented and modified states from later restored-source builds. This evidence package does not resolve that full historical chain and does not claim independent reproduction of profiling or candidate discovery.

Locally checked tool revisions:

- Minotaur: `f764ac4eb2c5b7644390c6a0c35087f4ff01427c`, with local cache-handling, timeout and build-compatibility edits; discovery implementation is not distributed.
- Alive2 checkout: `913e1556032ee70a9ebf147b5a0c7e10086b7490`, with a local header-location compatibility edit; proof logs record the outputs of the installed build22 binary. A git revision alone is not a reproducible binary identifier.
- Benchmark cut objects: llc `-O=3 -filetype=obj`.
- C harness: Clang `-O3 -march=native`, separate object files without LTO.
- Historical machine: Intel i7-1265U, WSL2 Linux 6.18.33.2, eight logical CPUs.

Internal orchestration, full generated modules and operational data are intentionally omitted. Verification of published pairs and remeasurement are distinct from reproducing automatic discovery. Hardware/compiler/version differences can change timings. No production integration, consensus-level assurance or mainnet throughput claim follows from this artifact.

The package was prepared with AI coding assistance; all quantitative claims are linked to inspectable data and scripts. Saved benchmark data is not newly measured data.
