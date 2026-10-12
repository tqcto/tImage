#pragma once

#include "../tImage_definition.h"
#include "../image/Image.h"

namespace tImage {

    /*
	transpose matrix of image.
	src_planes is source matrix of image.
	dst_planes is dest matrix of image.
	*/
	LIB_EXPORT t_err transpose(Image* src, Image* dst);

}