#include "../../include/tool/split.h"

#include <immintrin.h>

namespace tImage {

    /*
    t_err split(Image* src, Image* dst_planes) {

		if (!src || src->empty() || src->channels() < 2) return t_err_InvalidArgument;

		t_uint64 src_stride = src->stride();
		t_uint64 dst_stride = dst_planes[0].stride();

		for (int y = 0; y < src->height(); y++) {
			for (int x = 0; x < src->width(); x++) {
				for (int c = 0; c < src->channels(); c++) {
					dst_planes[c].data[y * dst_stride + x] = src->data[y * src_stride + x * src->channels() + c];
				}
			}
		}

		return t_err_None;

	}
    */

    t_err split_RGB24(Image* src, Image* dst_planes) {

        t_uint64 total_pixels = (t_uint64)src->width() * src->height();
        // バッファに余裕があると仮定（allocate時に padding を追加）

        t_uchar* src_data = src->data;
        t_uchar* dst_r = dst_planes[0].data;
        t_uchar* dst_g = dst_planes[1].data;
        t_uchar* dst_b = dst_planes[2].data;

        // AVX2: 32ピクセル（96バイト）単位で全ピクセルを処理
        // ストライド超過分は無視（バッファ末尾の padding で吸収）
        for (t_uint64 i = 0; i < total_pixels; i += 32) {
            __m256i block0 = _mm256_loadu_si256((const __m256i*)(src_data + 3*i + 0));
            __m256i block1 = _mm256_loadu_si256((const __m256i*)(src_data + 3*i + 32));
            __m256i block2 = _mm256_loadu_si256((const __m256i*)(src_data + 3*i + 64));

            // PSHUFB（256bit レーン毎に 128bit 単位で実行）
            const __m256i shuffle_r = _mm256_setr_epi8(
                0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, -1, -1, -1, -1, -1,
                0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, -1, -1, -1, -1, -1
            );
            const __m256i shuffle_g = _mm256_setr_epi8(
                1, 4, 7, 10, 13, 16, 19, 22, 25, 28, 31, -1, -1, -1, -1, -1,
                1, 4, 7, 10, 13, 16, 19, 22, 25, 28, 31, -1, -1, -1, -1, -1
            );
            const __m256i shuffle_b = _mm256_setr_epi8(
                2, 5, 8, 11, 14, 17, 20, 23, 26, 29, 32, -1, -1, -1, -1, -1,
                2, 5, 8, 11, 14, 17, 20, 23, 26, 29, 32, -1, -1, -1, -1, -1
            );
            
            __m256i r = _mm256_shuffle_epi8(block0, shuffle_r);
            __m256i g = _mm256_shuffle_epi8(block1, shuffle_g);
            __m256i b = _mm256_shuffle_epi8(block2, shuffle_b);

            _mm256_storeu_si256((__m256i*)(dst_r + i), r);
            _mm256_storeu_si256((__m256i*)(dst_g + i), g);
            _mm256_storeu_si256((__m256i*)(dst_b + i), b);
        }

        return t_err_None;
    }

    t_err split(Image* src, Image* dst_planes) {

        if (!src || src->empty() || src->channels() < 2) return t_err_InvalidArgument;

        t_uint channels = src->channels();

        // RGB24
        if (channels == 3) {
            return split_RGB24(src, dst_planes);
        }/*
        // RGBA32
        else if (channels == 4) {
            return split_RGBA32(src, dst_planes);
        }
        // other
        else {
            return split_generic(src, dst_planes);
        }*/

    }

}