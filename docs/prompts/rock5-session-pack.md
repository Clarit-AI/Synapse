# ROCK 5 Session Pack

This file contains specialized prompts for on-device ROCK 5 / RK3588 sessions where runtime validation matters more than desktop-only code plumbing.

Use the section that matches the session goal.

---

## Prompt A: ROCK 5 Build And Environment Validation

You are running on or against a ROCK 5 / RK3588 environment for the repository:

`/Users/bbrenner/Documents/Scripting Projects/PrepperTech/rockchip-dev/edgeai-llama.cpp`

### Goal
Validate that the current branch builds and runs correctly on the target device before deeper hybrid experiments begin.

### Required Work
- Inspect the local branch and summarize what hybrid-related code is present.
- Verify toolchain, runtime libraries, and environment assumptions for RK3588 / RKNPU2.
- Build the project on-device.
- Confirm the relevant binaries start successfully.
- Run the narrowest possible smoke tests for:
  - CPU-only inference
  - legacy RKNPU2 path
  - manifest-driven hybrid dry-run
- Capture any target-specific issues:
  - missing runtime libraries
  - device selection failures
  - NPU backend initialization issues
  - tensor packing / alignment failures

### Deliverables
- build status
- runtime smoke test status
- target-specific blockers
- recommended next experimental step

### Constraints
- Do not make speculative architecture changes during this session unless they are necessary to unblock the target environment.
- Prefer diagnosis and minimal fixes.

---

## Prompt B: ROCK 5 Hybrid Runtime Benchmark Session

You are running on or against a ROCK 5 / RK3588 environment for the repository:

`/Users/bbrenner/Documents/Scripting Projects/PrepperTech/rockchip-dev/edgeai-llama.cpp`

### Goal
Benchmark and compare CPU-only, legacy NPU, and manifest-driven hybrid execution on real hardware.

### Required Work
- Identify the available benchmark binaries and model files.
- Choose at least one dense model and, if available, one MoE model.
- Run the comparison matrix for each target model:
  1. CPU-only baseline
  2. legacy RKNPU2 baseline
  3. manifest-driven hybrid profile
- Collect:
  - prompt processing throughput
  - token generation throughput
  - manifest/profile attribution
  - fallback observations
  - repeated-run consistency
- Summarize where hybrid mode helps, hurts, or falls back.

### Deliverables
- compact benchmark table
- fallback summary
- recommendation for manifest tuning or code changes

### Constraints
- Keep runs attributable with `HYBRID_MANIFEST`, `HYBRID_PROFILE`, and `HYBRID_STRICT` where relevant.
- Do not claim conclusions from a single noisy run if repeated runs disagree.

---

## Prompt C: ROCK 5 Manifest Tuning Session

You are running on or against a ROCK 5 / RK3588 environment for the repository:

`/Users/bbrenner/Documents/Scripting Projects/PrepperTech/rockchip-dev/edgeai-llama.cpp`

### Goal
Tune manifest rules based on measured RK3588 behavior.

### Required Work
- Inspect the current example manifests.
- Identify which layers/tensors are actually succeeding or falling back.
- Adjust manifest rules to improve either:
  - throughput
  - determinism
  - fallback clarity
- Re-run targeted benchmarks after each meaningful manifest change.
- Prefer the smallest manifest edits that produce measurable gains.

### Deliverables
- revised manifest files
- before/after benchmark comparison
- explanation of which rules improved results
- list of unresolved bottlenecks

### Constraints
- Do not broaden runtime support beyond current matmul scope.
- Do not change multiple unrelated variables at once.

---

## Prompt D: ROCK 5 Strict-Mode And Fallback Audit

You are running on or against a ROCK 5 / RK3588 environment for the repository:

`/Users/bbrenner/Documents/Scripting Projects/PrepperTech/rockchip-dev/edgeai-llama.cpp`

### Goal
Audit strict-mode behavior and fallback correctness on real hardware.

### Required Work
- Exercise invalid manifest cases on-device.
- Verify strict mode fails loudly and specifically.
- Verify non-strict mode falls back to CPU in an attributable way.
- Confirm unsupported source types and impossible alignments do not silently route to NPU.
- Confirm no-manifest mode still behaves like legacy mode.

### Deliverables
- pass/fail list for strict-mode cases
- pass/fail list for fallback cases
- notes on any ambiguous or misleading logs
- recommended logging or validation improvements

### Constraints
- This is a correctness session first, not a tuning session.

---

## Prompt E: ROCK 5 Phase-Advance Decision Session

You are running on or against a ROCK 5 / RK3588 environment for the repository:

`/Users/bbrenner/Documents/Scripting Projects/PrepperTech/rockchip-dev/edgeai-llama.cpp`

### Goal
Decide whether the project is ready to advance to the next roadmap phase.

### Required Work
- Review the current code state.
- Review the benchmark evidence available on-device.
- Review fallback, strict-mode, and determinism results.
- Decide whether the repo is ready to advance:
  - from phase 2 to phase 3
  - from phase 3 to phase 4
  - from phase 4 to phase 5
- State the decision with explicit evidence.

### Deliverables
- current phase assessment
- go / no-go decision for next phase
- blockers if no-go
- concrete next session recommendation

### Constraints
- Do not make large code changes in this session unless a tiny unblocker is required to complete the decision.
