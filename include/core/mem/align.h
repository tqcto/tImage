#pragma once

#include "../../tImage_definition.h"

namespace tImage {
namespace core {
namespace mem {

    // check memory is aligned
    inline t_bool check_align(const void* ptr, t_uint64 align) noexcept {

        return (reinterpret_cast<uintptr_t>(ptr) & (align - 1)) == 0;

    }

}   
}
}
