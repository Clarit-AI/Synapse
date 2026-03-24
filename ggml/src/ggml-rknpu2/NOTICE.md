# Rockchip RKNN SDK Licensing Notice

The Rockchip RKNN SDK components included in this repository:

- `libs/include/rknn_api.h`
- `libs/include/rknn_matmul_api.h`
- `libs/librknnrt.so`

...are proprietary and confidential software owned by Rockchip Corporation.

## License Restrictions

**Redistribution of Rockchip RKNN SDK components is not permitted** without explicit written permission from Rockchip Corporation. The headers and library may not be reproduced, distributed, or disclosed to third parties without prior written consent.

## For Contributors

⚠️ **DO NOT COMMIT Rockchip RKNN SDK binaries or headers to the repository.**

If you are submitting changes to this repository:

1. **Do not include** Rockchip proprietary files in your commits
2. The `libs/` directory containing SDK components should **not be pushed** to GitHub
3. CI/CD builds should use `-DRKNN_USE_SYSTEM_SDK=ON` with system-installed RKNN SDK

## User Installation

Users must obtain the Rockchip RKNN SDK through official channels:

1. **System Package** (Debian/Ubuntu):
   ```bash
   sudo apt-get install librknnrt2 librknnrt2-dev
   ```

2. **Manual Download**:
   Download from Rockchip's official releases and follow installation instructions.

## Disclaimer

This notice is provided for compliance purposes. The full license terms are included with the Rockchip RKNN SDK distribution. Users are responsible for complying with Rockchip's license agreement when using this SDK.

---

*Last Updated: 2024*
