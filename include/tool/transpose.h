#pragma once

#include "../tImage_definition.h"
#include "../image/Image.h"

namespace tImage {

    /*
	transpose matrix of image.
	dst_planes is source matrix of image.
	dst_planes is dest matrix of image.
	*/
	DLL_EXPORT t_err transpose(Image* src, Image* dst);

}