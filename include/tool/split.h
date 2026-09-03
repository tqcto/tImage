#pragma once

#include "../tImage_definition.h"
#include "../image/Image.h"

namespace tImage {

    /*
	Split color channels of image.
	dst_planes is array of Image splitted src image channels.
	dst_planes are sorted in src color order.
	*/
	DLL_EXPORT t_err split(Image* src_planes, Image* dst);

}