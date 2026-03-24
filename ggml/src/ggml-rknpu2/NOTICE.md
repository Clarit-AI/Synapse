# Rockchip RKNN SDK Licensing Notice

The Rockchip RKNN SDK (`librknnrt.so` and headers in `libs/include/`) is proprietary
and confidential software provided by Rockchip. Users must comply with Rockchip's
license agreement when using this SDK.

## Redistribution Rights

**IMPORTANT**: Before redistributing software that includes or links to the Rockchip RKNN SDK,
you must obtain written permission from Rockchip. The RKNN SDK may not be reproduced,
distributed, or disclosed without Rockchip's prior written consent.

## Alternative: System Installation

Instead of using the vendored SDK, you can install the Rockchip RKNN runtime from
Rockchip's official sources:

```bash
# On Debian/Ubuntu-based systems (RK3588):
sudo apt-get install librknnrt2-dev

# Or manually download from Rockchip's official release
```

When using system-installed RKNN SDK, remove the vendored `libs/` directory and
rebuild the project.

## For Contributors

If you are submitting changes to this repository:
- Do not commit the Rockchip SDK binaries
- Ensure any CI/CD builds use system RKNN SDK
- Document any changes to SDK dependencies

---

*This notice is provided for compliance purposes. The full license terms are
included with the Rockchip RKNN SDK distribution.*
