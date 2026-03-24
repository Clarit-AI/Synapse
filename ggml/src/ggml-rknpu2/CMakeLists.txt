# Finding OpenMP
find_package(OpenMP REQUIRED)

# RKNN SDK options
# Set RKNN_SDK_ROOT to your Rockchip RKNN SDK installation path, or use system package
option(RKNN_USE_SYSTEM_SDK "Use system-installed RKNN SDK instead of vendored" OFF)

set(RKNN_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/include" CACHE PATH "Path to RKNN SDK headers")
set(RKNN_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs" CACHE PATH "Path to RKNN SDK libraries")

if(RKNN_USE_SYSTEM_SDK)
    # Try to find system RKNN SDK
    find_path(RKNN_INCLUDE_DIR NAMES rknn_api.h PATHS /usr/include /opt/rknn/include $ENV{RKNN_SDK_ROOT}/include)
    find_library(RKNN_LIBRARY NAMES rknnrt PATHS /usr/lib /opt/rknn/lib $ENV{RKNN_SDK_ROOT}/lib)
    if(NOT RKNN_INCLUDE_DIR OR NOT RKNN_LIBRARY)
        message(FATAL_ERROR "RKNN_USE_SYSTEM_SDK=ON but RKNN SDK not found. Install librknnrt2-dev package or set RKNN_SDK_ROOT environment variable.")
    endif()
    message(STATUS "Using system RKNN SDK: ${RKNN_INCLUDE_DIR}")
else()
    # Check if vendored SDK exists
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/libs/include/rknn_api.h")
        message(FATAL_ERROR "RKNN SDK headers not found in ${CMAKE_CURRENT_SOURCE_DIR}/libs/include/. "
            "Either install librknnrt2-dev package or download Rockchip RKNN SDK and extract to libs/ directory. "
            "See README.md for installation instructions.")
    endif()
endif()

# Adding backend files
ggml_add_backend_library(ggml-rknpu2
    ggml-rknpu2.cpp
    rknpu2-allocation.cpp
    rknpu2-quantization.cpp
    rknpu2-calibration.cpp
    rknpu2-configuration.cpp
)

# Linking rknpu2 header files
target_include_directories(ggml-rknpu2
    PRIVATE
        ${RKNN_INCLUDE_DIR}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Linking rknpu2 library
if(RKNN_USE_SYSTEM_SDK)
    target_link_libraries(ggml-rknpu2 PRIVATE ${RKNN_LIBRARY})
else()
    target_link_libraries(ggml-rknpu2 PRIVATE ${RKNN_LIB_DIR}/librknnrt.so)
endif()

# Adding OpenMP in build
if(OpenMP_FOUND)
    target_compile_definitions(ggml-rknpu2 PRIVATE HAVE_OPENMP=1)
    target_link_libraries(ggml-rknpu2 PRIVATE OpenMP::OpenMP_CXX)
endif()