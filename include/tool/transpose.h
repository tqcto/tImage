#pragma once

#include "../tImage_definition.h"
#include "../image/Image.h"

namespace tImage {

    /*
	transpose matrix of image.
	src_planes is source matrix of image.
	dst_planes is dest matrix of image.
	*/
	DLL_EXPORT t_err transpose(Image* src, Image* dst);

    /*
	transpose complex matrix.
	src_planes is source complex matrix.
	dst_planes is dest complex matrix.
	*/
    DLL_EXPORT t_err transpose(Matrix<t_float>* src_real, Matrix<t_float>* src_imag, Matrix<t_float>* dst_real, Matrix<t_float>* dst_imag);

}