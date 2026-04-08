# Synapse

<div align="center">
  <img src="assets/synapse-banner.png" alt="Synapse banner" width="720" />

  <p>
    <a href="https://opensource.org/licenses/MIT">
      <img src="https://img.shields.io/badge/license-MIT-blue.svg" alt="License: MIT" />
    </a>
    <a href="https://deepwiki.com/Clarit-AI/Synapse">
      <img src="https://deepwiki.com/badge.svg" alt="Ask DeepWiki" />
    </a>
  </p>

  <h3>Heterogeneous LLM inference for the edge</h3>
</div>

## Why Synapse?

Modern large language models demand more compute than most edge devices can comfortably deliver. On Rockchip boards such as the RK3588, that often means the CPU carries most of the load while the integrated NPU sits underused.

Synapse closes that gap. It is a high-performance fork in the `llama.cpp` family that brings together:

- The core runtime and quantization work from [ikawrakow/ik_llama.cpp](https://github.com/ikawrakow/ik_llama.cpp)
- A modernized Rockchip RKNPU2 backend derived from [KHAEntertainment/rk-llama.cpp](https://github.com/KHAEntertainment/rk-llama.cpp)
- Hybrid routing that lets CPU, CUDA, and Rockchip NPU execution coexist in one binary

By combining those pieces, Synapse can offload supported attention and dense layers to the NPU while keeping unsupported or better-suited workloads on CPU or CUDA. The goal is simple: make efficient local inference practical on real-world edge hardware.

Synapse is part of the Clarit.AI open-source ecosystem. Synapse focuses on execution and acceleration, while related projects like Engram focus on persistent state and agent workflows on constrained hardware.

## Key Features

- Rockchip RKNPU2 support for RK3588 and RK3576-class NPUs
- Hybrid CPU/NPU routing driven by deterministic manifest files
- Advanced IQK and trellis quantization inherited from `ik_llama.cpp`
- BitNet, DeepSeek, Flash Attention, and MLA-related optimizations from upstream
- Ongoing upstream sync strategy to stay close to modern `ggml` and model support
- Cross-platform CPU and CUDA support alongside Rockchip-specific acceleration

## Quick Start

### 1. Clone the repository

```bash
git clone https://github.com/Clarit-AI/Synapse.git
cd Synapse
git submodule update --init --recursive
```

### 2. Install prerequisites

On Debian or Ubuntu:

```bash
sudo apt-get update
sudo apt-get install build-essential cmake git libcurl4-openssl-dev libgomp1
```

### 3. Build for your target backend

CPU-only build:

```bash
cmake -B build -DGGML_NATIVE=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build -j"$(nproc)"
```

CUDA build:

```bash
cmake -B build -DGGML_NATIVE=ON -DGGML_CUDA=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build -j"$(nproc)"
```

Rockchip NPU build:

```bash
sudo apt-get install librknpu2-rk3588

cmake -B build -DGGML_NATIVE=ON -DGGML_RKNPU2=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build -j"$(nproc)"
```

For backend details, see [docs/backend/RKNPU2.md](docs/backend/RKNPU2.md).

### 4. Run a model

```bash
./build/bin/llama-server \
  --model /path/to/model.gguf \
  --ctx-size 4096
```

For GPU offload, add `-ngl 999` where appropriate.

Then open [http://127.0.0.1:8080](http://127.0.0.1:8080) in your browser.

## Performance Quickstart

The fastest setup depends on your hardware and model family, but a strong CPU baseline looks like this:

```bash
./build/bin/llama-server \
  --model /path/to/model.gguf \
  --ctx-size 4096 \
  -t "$(nproc)" \
  -fa \
  -fmoe \
  -ctk q8_0 -ctv q8_0 \
  -b 2048 -ub 2048 \
  -rtr \
  -mla 3
```

Those flags enable Flash Attention, fused MoE kernels, quantized KV cache, larger batches, runtime repacking, and MLA support where available.

> **Not sure whether CPU, NPU, or Hybrid is best for your model?** Ask DeepWiki for a recommended starting point, then benchmark CPU, NPU, and Hybrid performance on your device.

For a fuller tuning guide, see [docs/cpu-arm-optimization.md](docs/cpu-arm-optimization.md).

## Hybrid CPU + NPU Mode

Hybrid mode lets you route supported tensors to the Rockchip NPU while keeping the rest on CPU. The simplest way to get started is to use one of the example manifests in [examples/hybrid-manifests](examples/hybrid-manifests):

- `dense-balanced.json`
- `dense-npu-heavy.json`
- `moe-balanced.json`

Example:

```bash
./build/bin/llama-server \
  --model /path/to/model.gguf \
  --hybrid-manifest examples/hybrid-manifests/dense-balanced.json \
  --ctx-size 4096 \
  -fa -fmoe -ctk q8_0 -ctv q8_0 -b 2048 -ub 2048 -rtr
```

If you want startup to fail instead of silently falling back when the manifest cannot be satisfied, add `--hybrid-strict`.

You can also place a sidecar manifest next to the model and point Synapse at it explicitly with `--hybrid-manifest`.

## Notes & Warnings

### Supported Backends

The only fully functional and performance-focused compute backends in Synapse are:

- CPU (`AVX2` or better, `ARM_NEON` or better)
- CUDA
- Rockchip NPU (via `RKNPU2`)

Metal support is inherited from upstream and may work, but it is not currently a primary optimization target.

Please do not open issues for ROCm, Vulkan, or other backends unless you are actively contributing to bring them up to speed.

### Quantized Model Warning

Do not use quantized models from Unsloth that have `_XL` in their name unless you know they do not contain `f16` tensors.

To be precise: the `_XL` variants most likely to fail are the ones that include `f16` tensors. Models without those tensors are generally fine.

### Partial Offload and Graph Mode Warning

Some users have reported gibberish or incoherent output when using graph parallel mode (split mode `graph`) or partial GPU offload configurations such as:

- `--cpu-moe`
- `--n-cpu-moe`
- tensor override workflows

If you run into that behavior, try:

```bash
-cuda graphs=0
```

## Upstream Sync Status

| Component | Source | Sync Frequency |
| --- | --- | --- |
| CPU / Quantization | `ik_llama.cpp` | Weekly |
| Rockchip NPU | `rk-llama.cpp` | As needed |
| GGML Core | upstream `llama.cpp` via `ik` tracking | Indirect / inherited |

## Architecture Overview

Synapse is organized around three major layers:

- `ggml` core: the tensor runtime and model loading foundation
- Core runtime: CPU and CUDA kernels, quantization logic, and CLI behavior inherited primarily from `ik_llama.cpp`
- RKNPU2 backend: Rockchip-specific execution, routing, and compatibility work for modern `ggml`

That structure allows Synapse to stay close to upstream performance work while still evolving a dedicated hybrid path for Rockchip edge devices.

## Benchmarks and Tuning Resources

- [docs/cpu-arm-optimization.md](docs/cpu-arm-optimization.md) for CPU and hybrid runtime tuning
- [docs/backend/RKNPU2.md](docs/backend/RKNPU2.md) for Rockchip backend details
- [docs/prompts/rock5/README.md](docs/prompts/rock5/README.md) for RK3588 and Rock 5 benchmarking workflows
- [docker/README.md](docker/README.md) for container-based setup
- [docs/parameters.md](docs/parameters.md) for CLI flags and runtime options

## Contributing

Contributions are welcome. If you are improving hardware support, quantization, manifests, or documentation, open an issue or pull request and include enough detail for someone else to reproduce your environment and results.

General contribution guidance lives in [CONTRIBUTING.md](CONTRIBUTING.md).

## License

Synapse is released under the MIT license. See [LICENSE](LICENSE) for details.

## Acknowledgements

Synapse stands on the work of several upstream projects and communities:

- [ikawrakow/ik_llama.cpp](https://github.com/ikawrakow/ik_llama.cpp) for the performance-focused runtime, quantization, and CPU/CUDA optimization work
- [KHAEntertainment/rk-llama.cpp](https://github.com/KHAEntertainment/rk-llama.cpp) and earlier Rockchip integration efforts for the original RKNPU2 backend direction
- [rockchip-linux/rknpu2](https://github.com/rockchip-linux/rknpu2) for Rockchip's runtime and low-level NPU support
- [rockchip-linux/rknn-toolkit2](https://github.com/rockchip-linux/rknn-toolkit2) for model conversion workflows required for NPU execution

Their work makes modern local inference on constrained hardware much more practical.
