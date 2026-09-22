#pragma once

#include "../tImage_definition.h"

#include <immintrin.h>

namespace tImage {
namespace simd {

    struct v_uint8x32 {
        
        __m256i v;

        explicit v_uint8x32(__m256i v) : v(v) {}

        v_uint8x32(
            t_char v0, t_char v1, t_char v2, t_char v3,
            t_char v4, t_char v5, t_char v6, t_char v7,
            t_char v8, t_char v9, t_char v10, t_char v11,
            t_char v12, t_char v13, t_char v14, t_char v15,
            t_char v16, t_char v17, t_char v18, t_char v19,
            t_char v20, t_char v21, t_char v22, t_char v23,
            t_char v24, t_char v25, t_char v26, t_char v27,
            t_char v28, t_char v29, t_char v30, t_char v31
        ) {
            v = _mm256_set_epi8(
                v0, v1, v2, v3, v4, v5, v6, v7, v8, v9,
                v10, v11, v12, v13, v14, v15, v16, v17, v18, v19,
                v20, v21, v22, v23, v24, v25, v26, v27, v28, v29,
                v30, v31
            );
        }

    };

}
}
