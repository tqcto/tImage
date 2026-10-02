#pragma once

#include "../../tImage_definition.h"

#if defined(TIMAGE_ARCH_X86) || defined(_M_IX86) || defined(_M_X64) || \
    defined(__i386__) || defined(__x86_64__)
#include "intrin_avx.h"
#include "intrin_ssse3.h"
#elif defined(TIMAGE_ARCH_ARM64) || defined(_M_ARM64) || defined(__aarch64__)
#include "intrin_neon.h"
#elif defined(TIMAGE_ARCH_WASM) || defined(__wasm_simd128__)
#include "intrin_wasm.h"
#endif