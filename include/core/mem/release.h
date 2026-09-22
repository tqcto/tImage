#include "../../tImage_definition.h"

#if defined(_MSC_VER) || defined(__MINGW32__)
#include <malloc.h>
#else
#include <cstdlib>
#endif

namespace tImage {
namespace core {
namespace mem {

    // release aligned memory
    inline void alignedFree(void* ptr) {

        #if T_MS || T_MINGW32
        
        _aligned_free(ptr);
        
        #else

        std::free(ptr);
        
        #endif

    }

}
}
}
