#include "../../include/tool/converter.h"

namespace tImage {

	t_err RGBA_BGRA(Image* img) {

		if (img->empty() || img->channels() != 4) {

			return t_err_InvalidArgument;

		}

		t_uint width	= img->width();
		t_uint height	= img->height();
		t_uint64 stride = img->stride();

		for (t_uint64 y = 0; y < height; y++) {

			t_uchar* img_row = img->data + (y * stride);
			for (t_uint x = 0; x < width; ++x) {
				t_uchar* pixel = img_row + x * 4;
				t_uchar red = pixel[0];
				pixel[0] = pixel[2];
				pixel[2] = red;
			}

		}

		return t_err_None;

	}

}