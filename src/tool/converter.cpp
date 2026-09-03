#include "../../include/tool/converter.h"

namespace tImage {

	t_err RGBA_BGRA(Image* img) {

		if (img->empty() || img->channels() != 4) {

			return t_err_InvalidArgument;

		}

		t_uint width	= img->width();
		t_uint height	= img->height();
		t_uint depth	= img->depth();
		t_uint64 stride = img->stride();

		t_uint64 total_bytes = stride * (t_uint64)height;

		__m256i mask = _mm256_setr_epi8(

			2,	1,	0,	3,
			6,	5,	4,	7,
			10,	9,	8,	11,
			14,	13,	12,	15,

			2,	1,	0,	3,
			6,	5,	4,	7,
			10,	9,	8,	11,
			14,	13,	12,	15

		);

		// 1行あたり，画像データとして有効な範囲，つまりピッチ分を考えない実質のメモリサイズ
		t_uint64 row_valid_bytes = width * (depth >> 3) << 2; // << 2 = * 4 = * channels()

		for (t_uint64 y = 0; y < height; y++) {

			const t_uchar* img_row = img->data + (y * stride);

			t_uint x_bytes = 0;
			for (; x_bytes <= row_valid_bytes - 32; x_bytes += 30) {

				// メモリから32バイト（10画素 + (いらないけどとってきちゃう分)）をロード
				__m256i rgb = _mm256_loadu_si256((const __m256i*)(img_row + x_bytes));

				// AVX2シャッフル命令
				__m256i bgr = _mm256_shuffle_epi8(rgb, mask);

			}

		}

	}

}