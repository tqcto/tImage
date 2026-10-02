#pragma once

#include "../../tImage_definition.h"

namespace tImage {
namespace core {
namespace simd {

    #if defined(TIMAGE_ARCH_X86)
    #include "intrin_avx.h"
    #include "intrin_ssse3.h"

    // #elif defined(TIMAGE_ARCH_ARM) || defined(TIMAGE_ARCH_ARM64)
    #elif defined(TIMAGE_ARCH_ARM64)
    #include "intrin_neon.h"
    #elif defined(TIMAGE_ARCH_WASM)
    #include "intrin_wasm.h"

    #endif

}
}
}