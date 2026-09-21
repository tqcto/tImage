#include "../../include/tool/merge.h"

#if defined(_OPENMP)
#include <omp.h>
#endif

namespace tImage {

	void merge_RGBA32(Image* src_planes, Image* dst) {

		const t_int width = dst->width();
        const t_int height = dst->height();

		//#pragma omp prallel for
		for (t_int y = 0; y < height; y++) {

            t_uchar* src0_rowptr = src_planes[0].rowPtr(y);
            t_uchar* src1_rowptr = src_planes[1].rowPtr(y);
            t_uchar* src2_rowptr = src_planes[2].rowPtr(y);
            t_uchar* src3_rowptr = src_planes[3].rowPtr(y);
			t_uchar* dst_rowptr = dst->rowPtr(y);

			//#pragma omp simd
			for (t_int x = 0; x < width; x++) {

				dst_rowptr[x << 2] = src0_rowptr[x];
				dst_rowptr[(x << 2) + 1] = src1_rowptr[x];
				dst_rowptr[(x << 2) + 2] = src2_rowptr[x];
				dst_rowptr[(x << 2) + 3] = src3_rowptr[x];

			}

		}

	}

    t_err merge(Image* src_planes, Image* dst) {

		if (!src_planes || !dst || dst->channels() < 2) return t_err_InvalidArgument;

		t_uint channels = dst->channels();

        // RGB24
        if (channels == 3) {
            //merge_RGB24(src_planes, dst);
        }
        // RGBA32
        else if (channels == 4) {
            merge_RGBA32(src_planes, dst);
        }
        /*
        // other
        else {
            return split_generic(src_planes, dst);
        }*/

		return t_err_None;

	}

}