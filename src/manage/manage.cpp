#include "../../include/manage/manage.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <setjmp.h>

#include <png.h>
#include <jpeglib.h>

#define SIGNATURE_NUM	8

namespace tImage {

	t_uint64 calcStride4Matrix(t_uint cols, t_uint64 size, t_uint align) {

		// check align
		assert((align & (align - 1)) == 0);

		// bytes per a row. but, this isn't considered align.
		t_uint64 bytes_per_row = static_cast<t_uint64>(cols) * size;

		// calc stride
		return (bytes_per_row + static_cast<t_uint64>(align - 1)) & ~static_cast<t_uint64>(align - 1);

	}
	t_uint64 calcStride(t_uint width, t_uint channels, t_uint depth, t_uint align) {

		// check align
		assert((align & (align - 1)) == 0);

		// bytes per a channel
		//t_uint bytes_per_channel = this->_depth >> 3; // if depth is 8bit, then that is 1byte.

		// bytes per a pixel
		t_uint bytes_per_pixel = channels * (depth >> 3);
		//if (this->format.packed) {
		//bytes_per_pixel = this->format.bytes_per_pixel;
		/* }
		else {

			t_uint* bits = reinterpret_cast<t_uint*>(&this->format.bits_per_channel.r);
			for (int i = 0; i < this->format.channels; i++) {
				bytes_per_pixel += bits[i] >> 3;//(bits[i] + 7) >> 3;
			}

		}*/

		// bytes per a row. but, this isn't considered align.
		t_uint64 bytes_per_row = width * bytes_per_pixel;

		// calc stride
		return (bytes_per_row + (align - 1)) & ~(align - 1);

	}

}