Getting Started with RKNPU2
===========================

This guide walks through building and running llama.cpp with NPU acceleration on Rockchip hardware.

Prerequisites
-------------

### Hardware

- Radxa Rock 5C (RK3588S) or similar Rockchip board
- Model with RK3588/RK3588S/RK3576 SoC
- eMMC or SD card with Linux

### Software

- Linux kernel with NPU driver (0.9.7+ recommended)
- RKNN runtime library (`librknnrt.so`)
- CMA heap size >= 256MB (1GB recommended)
- CMake 3.14+, GCC 9+

Step 1: Prepare Your System
---------------------------

### Check NPU Driver

```bash
# Check if NPU driver is loaded
lsmod | grep npu

# Check driver version
cat /sys/module/npu/version
```

### Verify CMA Size

```bash
cat /proc/meminfo | grep -i cma
```

Should show at least 256MB available. If CMA is too small:

```bash
# Edit kernel cmdline (on RadxaOS)
sudo nano /etc/kernel/cmdline
# Add: cma=1024M
sudo u-boot-update
sudo reboot
```

Step 2: Install RKNN Runtime
----------------------------

### From Rockchip SDK

Copy `librknnrt.so` to system library path:

```bash
sudo cp librknnrt.so /usr/lib/
sudo ldconfig
```

### Or from Debian package

```bash
# For RK3588
sudo apt-get install librknpu2-rk3588

# Verify installation
ldconfig -p | grep rknn
```

Step 3: Build llama.cpp with RKNPU2
------------------------------------

```bash
# Clone repository
git clone https://github.com/ikawrakow/ik_llama.cpp.git
cd ik_llama.cpp

# Create build directory
mkdir build && cd build

# Configure with RKNPU2 enabled
cmake -DGGML_RKNPU2=ON -DCMAKE_BUILD_TYPE=Release ..

# Build
make -j$(nproc)
```

Step 4: Run Inference
---------------------

### Basic NPU Inference

```bash
# Run with NPU acceleration
./build/bin/llama-cli \
    -m models/your-model.gguf \
    -p "Hello, how are you?" \
    --n-gpu-layers 99
```

### Verify NPU is Being Used

The first run will show NPU initialization messages:

```
RKNPU2: Initializing RKNN device...
RKNPU2: Device RK3588 selected
RKNPU2: Using 3 NPU cores
```

Step 5: Optimize for Your Model
-------------------------------

### For Large Models (IOVA Exhaustion)

If you see "IOVA exhaustion" errors:

```bash
RKNN_SPLIT_FACTOR=2 ./build/bin/llama-cli -m large-model.gguf --n-gpu-layers 99
```

### For Multi-Core Performance

```bash
# Use all cores explicitly
RKNN_CORE_MASK=0x7 ./build/bin/llama-cli -m model.gguf --n-gpu-layers 99
```

### For Hybrid Quantization

```bash
# Pattern: first layers FP16, rest INT8
HYBRID_PATTERN=FP16_STANDARD,INT8_STANDARD ./build/bin/llama-cli -m model.gguf --n-gpu-layers 99
```

Troubleshooting
---------------

### "Failed to find NPU device"

1. Check `lsmod | grep npu` - driver not loaded
2. Check CMA size with `cat /proc/meminfo | grep Cma`
3. Verify `librknnrt.so` is in `/usr/lib`

### "IOVA exhaustion"

1. Increase CMA size in kernel cmdline
2. Use `RKNN_SPLIT_FACTOR=2` or higher
3. Quantize model to use less memory

### Slow Performance

1. Verify model quantization type is NPU-compatible
2. Check `--n-gpu-layers 99` is set
3. Try different `HYBRID_PATTERN` values

Next Steps
----------

- Read [Environment Variables](ENVIRONMENT.md) for fine-tuning
- Read [Performance Tuning](PERFORMANCE.md) for benchmarking
- See [RKNPU2 Backend](../backend/RKNPU2.md) for detailed architecture
