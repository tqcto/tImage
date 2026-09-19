#include "../../include/image/MatrixConverter.h"

namespace tImage {

    /*
    t_err Image2Matrix(Image* __restrict src, Matrix<t_float>* __restrict dst) {

        constexpr t_float scale = 1.f / 255.f;

        const t_int width = src->width();
        const t_int height = src->height();

        #pragma omp parallel for
        for (t_int y = 0; y < height; y++) {
            
            const t_uchar* src_rowptr = src->rowPtr(y);
            t_float* dst_rowptr = dst->rowPtr(y);
            
            #pragma omp simd
            for (t_int x = 0; x < width; x++) {

                dst_rowptr[x] = static_cast<t_float>(src_rowptr[x]) * scale;

            }
        }

    }
    */

}