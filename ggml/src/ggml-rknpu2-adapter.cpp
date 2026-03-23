// ggml-rknpu2-adapter.cpp
// Adapter for RKNPU2 backend from rk-llama.cpp
//
// The RKNPU2 backend uses the device-aware registry API (ggml_backend_reg_t)
// which is the same API that ik_llama.cpp now supports after the device registry
// port. This adapter bridges the two by calling ggml_backend_rknpu2_reg() from
// the RKNPU2 backend and registering it with ik_llama.cpp's device registry.

#ifdef __APPLE__

#include "ggml-backend-impl.h"

// Stub function - RKNPU2 not supported on macOS
extern "C" GGML_CALL int ggml_backend_rknpu_reg_devices(void) {
    // No-op on macOS
    return 0;
}

// Stub buffer type function
ggml_backend_buffer_type_t ggml_backend_rknpu_buffer_type_fn(void) {
    return ggml_backend_cpu_buffer_type();
}

#else

// On Linux, RKNPU2 is supported
#include "ggml-backend-impl.h"

// Forward declaration of the RKNPU2 backend's registration function
extern "C" GGML_API ggml_backend_reg_t ggml_backend_rknpu2_reg(void);

// Register RKNPU2 devices with ik_llama.cpp's device-aware registry
extern "C" GGML_CALL int ggml_backend_rknpu_reg_devices(void) {
    ggml_backend_reg_t reg = ggml_backend_rknpu2_reg();
    if (reg != NULL) {
        ggml_backend_register_reg(reg);
        return 1;
    }
    return 0;
}

// Buffer type function - RKNPU2 uses its own buffer type
ggml_backend_buffer_type_t ggml_backend_rknpu_buffer_type_fn(void) {
    // This would return the RKNPU2-specific buffer type
    // For now, return CPU buffer type as fallback
    return ggml_backend_cpu_buffer_type();
}

#endif // __APPLE__
