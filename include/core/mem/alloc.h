#include "../../tImage_definition.h"

#if defined(_MSC_VER) || defined(__MINGW32__)
#include <malloc.h>
#else
#include <cstdlib>
#endif

namespace tImage {
namespace core {
namespace mem {

    // allocate aligned memory
    template<typename T = void>
    inline T* alignedMalloc(
        t_uint64 bytes, t_uint64 alignment = alignof(T)
    ) noexcept {

        constexpr t_uint64 min_align = sizeof(void*);
        if (alignment < min_align) alignment = min_align;

        #if T_MS || T_MINGW32
        
        return (T*)(_aligned_malloc(bytes, alignment));
        
        #else

        void* p = nullptr;
        if (posix_memalign(&p, alignment, bytes) != 0) return nullptr;
        
        return (T*)(p);
        
        #endif

    }

}
}
}
