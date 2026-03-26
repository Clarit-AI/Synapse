# Copilot Instructions for edgeai-llama.cpp

## Project Overview

**edgeai-llama.cpp** is a unified inference binary that integrates two upstream forks:

| Upstream | Source | Sync cadence |
|---|---|---|
| Core LLM runtime | [ikawrakow/ik_llama.cpp](https://github.com/ikawrakow/ik_llama.cpp) | Weekly |
| Rockchip NPU backend | [KHAEntertainment/rk-llama.cpp](https://github.com/KHAEntertainment/rk-llama.cpp) | As needed |
| GGML core | [ggerganov/llama.cpp](https://github.com/ggerganov/llama.cpp) | Via ik_llama.cpp |

The goal is a single binary capable of **hybrid inference** (e.g. attention on NPU, MoE experts on CPU/CUDA) that neither upstream supports standalone.

**Key capabilities added over upstream llama.cpp:**
- **Rockchip NPU (RKNPU2) backend** — hardware inference on RK3588/RK3576 SoCs (from KHA/rk-llama.cpp, significantly refactored for modern ggml compatibility)
- **CPU-MoE layer pinning** (`ncmoe`) — hybrid GPU/CPU scheduling for MoE models
- SOTA quantization types (IQK family, trellis KT, row-interleaved R4/R8/R16)
- DeepSeek MLA optimizations and fused MoE operations
- Bitnet support (`I2_S`, `IQ1_BN`, `IQ2_BN`)

**Scope limitations:** ROCm and Vulkan issues are out of scope and won't be fixed. Unsloth `_XL` models containing f16 tensors may not work. If CUDA graphs cause issues, add `-cuda_graphs=0`.

The custom code lives in `ggml/src/ggml-rknpu2/`, the `ncmoe` logic in `common/`, `include/llama.h`, and `src/llama-load-tensors.cpp`.

---

## Build Commands

```bash
# CPU only (native optimizations)
cmake -B build -DGGML_NATIVE=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

# CUDA
cmake -B build -DGGML_NATIVE=ON -DGGML_CUDA=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

# Rockchip NPU (RKNPU2)
cmake -B build -DGGML_NATIVE=ON -DGGML_RKNPU2=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

# Full build with tests + examples
cmake -B build -DGGML_NATIVE=ON -DGGML_RKNPU2=ON -DCMAKE_BUILD_TYPE=Release \
  -DLLAMA_BUILD_TESTS=ON -DLLAMA_BUILD_EXAMPLES=ON
cmake --build build -j$(nproc)
```

> **Note:** `LLAMA_CUDA`, `LLAMA_METAL`, `LLAMA_NATIVE` etc. are deprecated — use `GGML_*` equivalents.

---

## Test Commands

```bash
# Run a single test binary
./build/bin/tests/test-backend-ops
./build/bin/tests/test-quantize-fns
./build/bin/tests/test-tokenizer-0
./build/bin/tests/test-sampling

# Run all tests
make test
```

Key test binaries: `test-backend-ops`, `test-grad0`, `test-rope`, `test-sampling`, `test-quantize-fns`, `test-tokenizer-0`, `test-llama-grammar`, `test-chat-template`.

---

## Lint / Format

```bash
pre-commit run --all-files   # trailing whitespace, EOF, YAML, large files
flake8 .                      # Python linting (v7.0.0, flake8-no-print plugin)
```

---

## Architecture

### Backend Layer (`ggml/src/`)

| File / Directory | Purpose |
|---|---|
| `ggml-rknpu2/` | **Custom RKNPU2 backend** — NPU inference on RK3588/RK3576 |
| `ggml-rknpu2.cpp` | Top-level RKNPU2 entry point |
| `ggml-rknpu2-adapter.cpp` | Bridge between GGML backend API and RKNPU2 backend |
| `ggml-cuda.cu` | NVIDIA CUDA backend (upstream + patches) |
| `ggml-metal.m` | Apple Metal backend |
| `ggml.c` | Core compute graph |
| `ggml-quants.c` | Quantization (custom quant types live here) |
| `ggml-aarch64.c` | ARM64-specific optimizations |

### RKNPU2 Backend (`ggml/src/ggml-rknpu2/`)

- `ggml-rknpu2.cpp/.h` — full implementation
- `rknpu2-allocation.cpp/h` — NPU memory management
- `rknpu2-calibration.cpp/h` — quantization calibration
- `rknpu2-quantization.cpp/h` — quant support for NPU
- `rknpu2-configuration.cpp/h` — device config, MoE scheduling policy
- `libs/` — RKNN SDK headers and prebuilt libraries

### CPU-MoE Pinning (`ncmoe`)

`ncmoe` pins MoE expert layers to CPU while keeping attention/other layers on GPU/NPU. Key locations:
- `include/llama.h` — `int32_t ncmoe` in `llama_model_params`
- `common/common.cpp` — `--n-cpu-moe` / `-ncmoe` CLI argument
- `src/llama-load-tensors.cpp` — distribution logic across devices
- `src/llama-model-loader.cpp` — passes ncmoe into model loading

### Session/Phase Roadmap

See `SESSION-START.md` for which prompt to use depending on development phase:
- Unknown next step → `docs/prompts/master-roadmap-orchestrator.md`
- On-device ROCK 5 validation → `docs/prompts/rock5/`
- Backend/NPU docs → `docs/backend/RKNPU2.md`, `docs/npu/`

---

## Custom Quantization Types

This fork adds a large set of quantization types beyond upstream llama.cpp. They fall into three families:

### New base types (IDs 97–158)

| Type | Notes |
|---|---|
| `I2_S` | MS BitNet I2_S quant support |
| `Q6_0` | 6-bit base quant |
| `IQ1_BN`, `IQ2_BN` | BitNet 1-bit / 2-bit variants |
| `Q8_K64/K16/K32/KR8/K128/KV` | Q8_K with different block sizes |
| `IQ2_K`, `IQ3_K`, `IQ4_K`, `IQ5_K`, `IQ6_K` | K-scale i-quants (custom SOTA quality) |
| `IQ4_KS`, `IQ2_KS`, `IQ4_KSS`, `IQ5_KS`, `IQ3_KS` | KS ("small scale") variants |
| `IQ2_KT`, `IQ3_KT`, `IQ4_KT`, `IQ1_KT` | KT — integer-basis **trellis** quantization |
| `IQ2_KL` | KL variant |
| `Q8_0_X4`, `Q8_1_X4`, `Q8_2_X4` | X4 — SIMD dot-product acceleration variants |

### Row-interleaved `_R4` / `_R8` / `_R16` variants (IDs 200–399)

These are **memory layout transformations** of existing types that pack 4, 8, or 16 rows together for better SIMD throughput (AVX2/NEON). They are transparent at the model level — the same weights, different on-disk/in-memory packing.

Examples: `Q4_K_R4`, `Q5_K_R4`, `Q6_K_R4`, `IQ4_K_R4`, `IQ4_KS_R4`, `IQ4_XS_R8`, `BF16_R16`, `Q8_K_R8`, `Q8_KV_R8`, `Q8_K_R16`.

The suffix convention is: `_R<N>` where N is the row-interleave factor.

### Key files

- `ggml/include/ggml.h` — full `GGML_TYPE_*` enum (IDs 0–399+)
- `ggml/src/ggml-quants.c` — implementations of all custom types
- `ggml/src/ggml-quants.h` — quantization function declarations

---

## Key Conventions

### Code Style (from CONTRIBUTING.md)

- **4 spaces** indentation, **no tabs**
- Opening braces on **same line**: `if (x) {`
- Space before pointer/reference: `void * ptr`, `int & a`
- Optimize naming for **common prefix** grouping
- Avoid STL complexity — prefer simple `for` loops, no fancy templates
- No third-party dependencies unless absolutely necessary
- Cross-platform: Windows, Linux, macOS, ARM all must compile

### GGML Tensor Semantics (non-obvious)

- Tensors are **row-major**: `dim 0 = columns`, `dim 1 = rows`, `dim 2 = matrices`
- `ggml_mul_mat(ctx, A, B)` computes **`C^T = A · B^T`** — this is unconventional; double-check multiply direction before writing matmul code

### CMake Option Prefixes

- `GGML_*` — options affecting the ggml compute library layer
- `LLAMA_*` — options affecting the llama.cpp layer (tests, examples, server)
- Old `LLAMA_CUDA` / `LLAMA_METAL` / `LLAMA_NATIVE` are deprecated aliases

### PR Conventions

- Squash-merge commits; format: `<module> : <title> (#<issue>)` e.g. `ggml-rknpu2 : fix allocation for multi-NPU (#42)`
- Rate complexity (Low/Medium/High) in PR template
- Run `./build/bin/tests/test-backend-ops` before submitting backend changes

### Model Files

`Kimi-VL-A3B-Source/` contains reference imatrix files (`.imatrix.gguf`) used for quantization calibration — not model weights.
