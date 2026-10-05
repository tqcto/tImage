#include "../../include/geometry/affine.h"
#include "../../include/core/check.h"
#include "../../include/core/parallel/threadPool.h"
#include "../../include/core/simd/simd.h"

#include <string.h>

namespace tImage {

    t_err resize(const Image* src, Image* dst, t_floatpoint2d scale, _t_floatpoint2d center) {

        t_err err = core::check_args(src, dst);
        if (err != t_err_None) {
            return err;
        }

        const t_uint width = src->width();
        const t_uint height = src->height();
        const t_uint channels = src->channels();

        const t_float csx = center.x * scale.x;
        const t_float csy = center.y * scale.y;

        core::parallel::threadPool threadPool;
        threadPool.pfor(0, height, [src, dst, width, height, channels, scale, csx, csy](t_int y) {

            auto dst_rowptr = dst->rowPtr(y);

            const t_float dst_y = y * scale.y - csy;

            const t_int dst_y_int = static_cast<t_int>(dst_y);
            dst_y_int = 0 

            auto src_rowptr = src->rowPtr(
                static_cast<t_int>(dst_y)
            );

            for (t_int x = 0; x < width; x++) {

                const t_float dst_x = x * scale.x - csx;

                for (t_int c = 0; c < channels; c++) {
                    dst_rowptr[x * channels + c] = src_rowptr[static_cast<t_int>(dst_x) * channels + c];
                }
                
            }
        });

    }

}