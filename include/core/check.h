#pragma once
#include "../tImage_definition.h"
#include "../image/Image.h"

namespace tImage {
namespace core {

    inline t_err check_args(Image* src, Image* dst) {

        return (!src || !dst
            || src->empty() || dst->empty()
            || src->width() != dst->width()
            || src->height() != dst->height()
            || src->channels() != dst->channels()
            || src->depth() != dst->depth()
        ) ? t_err_InvalidArgument : t_err_None; 

    }
    inline t_err check_args(const Image* src, Image* dst) {

        return (!src || !dst
            || src->empty() || dst->empty()
            || src->width() != dst->width()
            || src->height() != dst->height()
            || src->channels() != dst->channels()
            || src->depth() != dst->depth()
        ) ? t_err_InvalidArgument : t_err_None; 

    }

}
}