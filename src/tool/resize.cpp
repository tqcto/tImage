#include "../../include/tool/resize.h"
#include "../../include/core/check.h"
#include "../../include/core/parallel/threadPool.h"
#include "../../include/core/simd/simd.h"
#include "../../include/core/ip/nearest.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace tImage {

    t_uintpoint2d calcDstScale(t_uint width, t_uint height, t_floatpoint2d scale) {

        const t_uint dst_width = static_cast<t_uint>(std::round(static_cast<t_float>(width) * scale.x));
        const t_uint dst_height = static_cast<t_uint>(std::round(static_cast<t_float>(height) * scale.y));

        return t_uintpoint2d(dst_width, dst_height);

    }
    t_uintpoint2d calcDstScale(const Image* src, t_floatpoint2d scale) {

        const t_uint dst_width = static_cast<t_uint>(std::round(static_cast<t_float>(src->width()) * scale.x));
        const t_uint dst_height = static_cast<t_uint>(std::round(static_cast<t_float>(src->height()) * scale.y));

        return t_uintpoint2d(dst_width, dst_height);

    }

    t_err resize(const Image* src, Image* dst, t_floatpoint2d scale, _t_floatpoint2d center) {

        if (!src || !dst || src->empty() || dst->empty() || src == dst
            || src->channels() != dst->channels() || src->depth() != dst->depth() || src->colorType() != dst->colorType()
            || !std::isfinite(scale.x) || !std::isfinite(scale.y)
            || scale.x <= 0.f || scale.y <= 0.f
            || !std::isfinite(center.x) || !std::isfinite(center.y)) {
            return t_err_InvalidArgument;
        }

        if (scale.x == 1.f && scale.y == 1.f) {
            return t_err_None;
        }

        const t_uint width = src->width();
        const t_uint height = src->height();
        const t_uint channels = src->channels();

        const t_uint dst_width = dst->width();
        const t_uint dst_height = dst->height();

        const t_float destination_center_x = center.x * scale.x;
        const t_float destination_center_y = center.y * scale.y;

        core::parallel::threadPool threadPool;
        threadPool.pfor(0, dst_height, [src, dst, width, height, dst_width, channels, scale, center, destination_center_x, destination_center_y](t_int y) {

            const t_float source_y = std::clamp(
                (static_cast<t_float>(y) - destination_center_y) / scale.y + center.y,
                0.f, static_cast<t_float>(height - 1)
            );
            const t_uint source_y_index = static_cast<t_uint>(std::round(source_y));
            auto src_rowptr = src->rowPtr(source_y_index);
            auto dst_rowptr = dst->rowPtr(static_cast<t_uint>(y));

            for (t_int x = 0; x < dst_width; x++) {

                const t_float source_x = std::clamp(
                    (static_cast<t_float>(x) - destination_center_x) / scale.x + center.x,
                    0.f, static_cast<t_float>(width - 1)
                );
                const t_uint source_x_index = static_cast<t_uint>(std::round(source_x));

                for (t_int c = 0; c < channels; c++) {
                    dst_rowptr[x * channels + c] = src_rowptr[source_x_index * channels + c];
                }

            }

        });

        return t_err_None;
        
    }

}