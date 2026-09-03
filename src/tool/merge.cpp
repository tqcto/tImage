#include "../../include/tool/merge.h"

namespace tImage {

    t_err merge(Image* src_planes, Image* dst) {

		if (!src_planes || !dst || dst->channels() < 2) return t_err_InvalidArgument;

		t_uint64 src_stride = src_planes[0].stride();
		t_uint64 dst_stride = dst->stride();

		for (int y = 0; y < dst->height(); y++) {
			for (int x = 0; x < dst->width(); x++) {
				for (int c = 0; c < dst->channels(); c++) {
					dst->data[y * dst_stride + x * dst->channels() + c] = src_planes[c].data[y * src_stride + x];
				}
			}
		}

		return t_err_None;

	}

}