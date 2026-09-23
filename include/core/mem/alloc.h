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

        #if T_MS || T_MINGW32
        
        return reinterpret_cast<T*>(_aligned_malloc(bytes, alignment));
        
        #else

        void* p = nullptr;
        posix_memalign(&p, alignment, bytes);
        
        return reinterpret_cast<T*>(p);
        
        #endif

    }

}
}
}
