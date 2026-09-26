#include <iostream>

#include <tImage.h>
#include <core/cpu.h>
#include <core/mem/align.h>
#include <core/simd/intrin_ssse3.h>
#include <core/simd/intrin_avx.h>

// using in debug
#include <stdlib.h>

#define IMG_PATH_PNG "..\\..\\..\\img.png"
#define IMG_PATH_JPG "..\\..\\..\\img.jpg"

using namespace tImage;

//#define _PRE

#ifdef _PRE
#define PRE_LOG	printf
#else
#define PRE_LOG
#endif

void test_Fourier1d(void) {

	//const t_uint N = 1920 * 1080 * 3;
	//const t_uint N = 1920 * 3;
	const t_uint N = 10;
	t_uint paddedN = calcPaddingSize(N);
	
	printf("source data:\n");
	t_float* src_real = (t_float*)malloc(sizeof(t_float) * paddedN);
	t_float* src_imag = (t_float*)malloc(sizeof(t_float) * paddedN);
	for (t_uint i = 0; i < paddedN; i++){
		src_real[i] = (t_float)i + 1.f;
		src_imag[i] = 0.f;
		PRE_LOG("[%u] : %f\n", i, src_real[i]);
	}
	
	// paddedNに合わせて出力メモリを確保
	t_float* fftdst_real = (t_float*)malloc(sizeof(t_float) * paddedN);
	t_float* fftdst_imag = (t_float*)malloc(sizeof(t_float) * paddedN);
	t_float* ifftdst_real = (t_float*)malloc(sizeof(t_float) * paddedN);
	t_float* ifftdst_imag = (t_float*)malloc(sizeof(t_float) * paddedN);

	// 作業用メモリ確保
	t_float* rot_buffer = (t_float*)malloc(sizeof(t_float) * paddedN);
	t_int* rev_table = (t_int*)malloc(sizeof(t_int) * paddedN);

	Fourier1d fourier1d;
	fourier1d.PrePlan(
		paddedN,
		src_real, src_imag,
		fftdst_real, fftdst_imag,
		ifftdst_real, ifftdst_imag,
		rot_buffer, rev_table
	);

	printf("start fft.\n");
	fourier1d.fft();

	printf("fft dest data:\n");
	#ifdef _PRE
	for (t_uint i = 0; i < paddedN; i++){
		PRE_LOG("[%u] : %f + %fi\n", i, fftdst_real[i], fftdst_imag[i]);
	}
	#endif

	printf("start ifft.\n");
	fourier1d.ifft();

	printf("ifft dest data:\n");
	#ifdef _PRE
	for (t_uint i = 0; i < paddedN; i++){
		PRE_LOG("[%u] : %f + %fi\n", i, ifftdst_real[i], ifftdst_imag[i]);
	}
	#endif

	/*
	printf("start dft:\n");
	fourier1d.dft();
	printf("start idft:\n");
	fourier1d.idft();
	printf("end.\n");
	*/

	free(src_real);
	free(src_imag);
	free(fftdst_real);
	free(fftdst_imag);
	free(ifftdst_real);
	free(ifftdst_imag);
	free(rot_buffer);
	free(rev_table);

}

void test_fft(void) {

	//const t_uint N = 1920 * 1080 * 3;
	const t_uint N = 1920 * 3;
	t_uint paddedN = calcPaddingSize(N);
	
	printf("source data:\n");
	t_float* src_real = (t_float*)malloc(sizeof(t_float) * paddedN);
	t_float* src_imag = (t_float*)malloc(sizeof(t_float) * paddedN);
	for (t_uint i = 0; i < paddedN; i++){
		src_real[i] = (t_float)i + 1.f;
		src_imag[i] = 0.f;
		PRE_LOG("[%u] : %f\n", i, src_real[i]);
	}

	// paddedNに合わせて出力メモリを確保
	t_float* dst_real = (t_float*)malloc(sizeof(t_float) * paddedN);
	t_float* dst_imag = (t_float*)malloc(sizeof(t_float) * paddedN);
	// 一時メモリも確保
	t_float* tmp_real = (t_float*)malloc(sizeof(t_float) * paddedN);
	t_float* tmp_imag = (t_float*)malloc(sizeof(t_float) * paddedN);

	printf("start fft.\n");

	fft1d(src_real, src_imag, dst_real, dst_imag, tmp_real, tmp_imag, paddedN);

	printf("dest data:\n");
	#ifdef _PRE
	for (t_uint i = 0; i < paddedN; i++){
		PRE_LOG("[%u] : %f + %fi\n", i, dst_real[i], dst_imag[i]);
	}
	#endif

	/*
	printf("apply low pass filter:\n");
	for (t_uint i = 0; i < paddedN; i++) {
		if (i > 10) {
			dst_real[i] = 0.f;
			dst_imag[i] = 0.f;
		}
		printf("[%u] : %f + %fi\n", i, dst_real[i], dst_imag[i]);
	}
	*/

	ifft1d(dst_real, dst_imag, src_real, src_imag, tmp_real, tmp_imag, paddedN);
	
	printf("source data:\n");
	#ifdef _PRE
	for (t_uint i = 0; i < paddedN; i++){
		PRE_LOG("[%u] : %f + %fi\n", i, src_real[i], src_imag[i]);
	}
	#endif

	free(src_real);
	free(src_imag);
	free(dst_real);
	free(dst_imag);
	free(tmp_real);
	free(tmp_imag);

}

void test_Matrix(void) {

	t_uint cols = 10;
	t_uint rows = 10;

	Matrix<t_float> mat(cols, rows);

	for (t_uint i = 0; i < cols; i++) {
		t_float* row_ptr = mat.rowPtr(i);
		for (t_uint j = 0; j < rows; j++) {

			row_ptr[j] = static_cast<t_float>(j * i);

		}
	}

	for (t_uint i = 0; i < cols; i++) {
		const t_float* row_ptr = mat.rowPtr(i);
		for (t_uint j = 0; j < rows; j++) {

			printf("%lf ", row_ptr[j]);

		}
		printf("\n");
	}

	mat.release();

}

void test_transpose(void) {

	const t_uint cols = 3;
	const t_uint rows = 5;

	Matrix<t_float> real(cols, rows);
	Matrix<t_float> imag(cols, rows);

	Matrix<t_float> dst_real(rows, cols);
	Matrix<t_float> dst_imag(rows, cols);

	for (t_int i = 0; i < rows; i++) {
		auto real_rowptr= real.rowPtr(i);
		auto imag_rowptr= imag.rowPtr(i);		
		for (t_int j = 0; j < cols; j++) {

			real_rowptr[j] = j + 1;
			imag_rowptr[j] = i - j;

		}
	}

	printf("src real:\n");
	for (t_int i = 0; i < rows; i++) {
		auto rowptr= real.rowPtr(i);		
		for (t_int j = 0; j < cols; j++) {

			printf("%lf ", rowptr[j]);

		}
		printf("\n");
	}
	printf("src imag:\n");
	for (t_int i = 0; i < rows; i++) {
		auto rowptr= imag.rowPtr(i);		
		for (t_int j = 0; j < cols; j++) {

			printf("%lf ", rowptr[j]);

		}
		printf("\n");
	}

	transpose(&real, &imag, &dst_real, &dst_imag);

	printf("dst real:\n");
	for (t_int i = 0; i < dst_real.rows(); i++) {
		auto rowptr = dst_real.rowPtr(i);		
		for (t_int j = 0; j < dst_real.cols(); j++) {

			printf("%lf ", rowptr[j]);

		}
		printf("\n");
	}

	printf("dst imag:\n");
	for (t_int i = 0; i < dst_imag.rows(); i++) {
		auto rowptr = dst_imag.rowPtr(i);		
		for (t_int j = 0; j < dst_imag.cols(); j++) {

			printf("%lf ", rowptr[j]);

		}
		printf("\n");
	}

}

void test_Fourier2dBlock(void) {

	const t_uint cols = 3840;
	const t_uint rows = 2160;
	Matrix<t_float> size_probe(1, 1);
	const t_uint block_size = size_probe.align() / sizeof(t_float);
	const t_uint padded_cols = calcPaddingBlockSize(cols, block_size);
	const t_uint padded_rows = calcPaddingBlockSize(rows, block_size);
	Matrix<t_float> src_real(padded_cols, padded_rows);
	Matrix<t_float> src_imag(padded_cols, padded_rows);
	Matrix<t_float> dst_real(padded_cols, padded_rows);
	Matrix<t_float> dst_imag(padded_cols, padded_rows);
	Matrix<t_float> ifft_real(padded_cols, padded_rows);
	Matrix<t_float> ifft_imag(padded_cols, padded_rows);

	printf("source matrix:\n");
	/*
	for (t_int y = 0; y < padded_rows; y++) {
		auto rowptr = src_real.rowPtr(y);
		for (t_int x = 0; x < padded_cols; x++) {

			rowptr[x] = (x < cols && y < rows) ? x + y * cols : 0.0f;
			src_imag.rowPtr(y)[x] = 0.0f;
			if (x < cols && y < rows) printf("%lf ", rowptr[x]);

		}
		if (y < rows) printf("\n");
	}
	*/

	Fourier2dWorkspace ws{};
	ws.block_src_real = (t_float*)malloc(sizeof(t_float) * block_size * block_size);
	ws.block_src_imag = (t_float*)malloc(sizeof(t_float) * block_size * block_size);
	ws.block_fft_real = (t_float*)malloc(sizeof(t_float) * block_size * block_size);
	ws.block_fft_imag = (t_float*)malloc(sizeof(t_float) * block_size * block_size);
	ws.column_src_real = (t_float*)malloc(sizeof(t_float) * block_size);
	ws.column_src_imag = (t_float*)malloc(sizeof(t_float) * block_size);
	ws.column_fft_real = (t_float*)malloc(sizeof(t_float) * block_size);
	ws.column_fft_imag = (t_float*)malloc(sizeof(t_float) * block_size);
	ws.rotation_buffer = (t_float*)malloc(sizeof(t_float) * block_size);
	ws.index_buffer = (t_int*)malloc(sizeof(t_int) * block_size);

	Fourier2dBlock fourier;
	fourier.PrePlan(&src_real, &src_imag, &dst_real, &dst_imag, &ifft_real, &ifft_imag, &ws);
	fourier.fft();

	printf("trasformed matrix real:\n");
	/*
	for (t_int y = 0; y < rows; y++) {
		auto rowptr = dst_real.rowPtr(y);
		for (t_int x = 0; x < cols; x++) {

			printf("%lf ", rowptr[x]);

		}
		printf("\n");
	}
	*/
	printf("trasformed matrix imag:\n");
	/*
	for (t_int y = 0; y < rows; y++) {
		auto rowptr = dst_imag.rowPtr(y);
		for (t_int x = 0; x < cols; x++) {

			printf("%lf ", rowptr[x]);

		}
		printf("\n");
	}
	*/
	
	fourier.ifft();

	printf("inverse matrix real:\n");
	/*
	for (t_int y = 0; y < rows; y++) {
		auto rowptr = ifft_real.rowPtr(y);
		for (t_int x = 0; x < cols; x++) {
			printf("%lf ", rowptr[x]);
		}
		printf("\n");
	}
	*/
	
	free(ws.block_src_real);
	free(ws.block_src_imag);
	free(ws.block_fft_real);
	free(ws.block_fft_imag);
	free(ws.column_src_real);
	free(ws.column_src_imag);
	free(ws.column_fft_real);
	free(ws.column_fft_imag);
	free(ws.rotation_buffer);
	free(ws.index_buffer);

}

void test_Fourier2d(void) {

	const t_uint cols = 1920;
	const t_uint rows = 1080;
	
	const t_uint padded_cols = calcPaddingSize(cols);
	const t_uint padded_rows = calcPaddingSize(rows);

	Matrix<t_float> src_real(padded_cols, padded_rows);
	Matrix<t_float> src_imag(padded_cols, padded_rows);
	Matrix<t_float> dst_real(padded_cols, padded_rows);
	Matrix<t_float> dst_imag(padded_cols, padded_rows);
	Matrix<t_float> ifft_real(padded_cols, padded_rows);
	Matrix<t_float> ifft_imag(padded_cols, padded_rows);

	printf("elements row:%d, col:%d\n", src_real.elementsRow(), src_real.rows());

	printf("source matrix:\n");
	// /*
	for (t_int y = 0; y < padded_rows; y++) {
		auto rowptr = src_real.rowPtr(y);
		for (t_int x = 0; x < padded_cols; x++) {

			rowptr[x] = static_cast<t_float>(x + y * cols);
			src_imag.rowPtr(y)[x] = 0.0f;
			PRE_LOG("%f ", rowptr[x]);

		}
		printf("\n");
	}
	// */

	Fourier2d fourier;
	t_uint64 size = fourier.PreSetup(
		&src_real, &src_imag,
		&dst_real, &dst_imag,
		&ifft_real, &ifft_imag
	);
	printf("buffer size:%u\n", size);
	void* buffer = malloc(size);
	printf("buffer : %p\n", buffer);
	fourier.Setup(buffer);

	fourier.fft();

	printf("transformed real:\n");
	#ifdef _PRE
	for (t_int y = 0; y < padded_rows; y++) {
		auto rowptr = dst_real.rowPtr(y);
		for (t_int x = 0; x < padded_cols; x++) {

			printf("%f ", rowptr[x]);

		}
		printf("\n");
	}
	#endif

	printf("transformed imag:\n");
	#ifdef _PRE
	for (t_int y = 0; y < padded_rows; y++) {
		auto rowptr = dst_imag.rowPtr(y);
		for (t_int x = 0; x < padded_cols; x++) {

			printf("%f ", rowptr[x]);

		}
		printf("\n");
	}
	#endif

	fourier.ifft();

	printf("inverse transformed real:\n");
	#ifdef _PRE
	for (t_int y = 0; y < padded_rows; y++) {
		auto rowptr = ifft_real.rowPtr(y);
		for (t_int x = 0; x < padded_cols; x++) {

			printf("%f ", rowptr[x]);

		}
		printf("\n");
	}
	#endif

	free(buffer);

}

void test_Image2Matrix(void) {

	const t_uint width = 10;
	const t_uint height = 10;
	const t_uint channels = 1;

	Image src(width, height, channels);
	Matrix<t_float> dst(width, height);

	const t_uchar s = 255 / (width - 1);
	printf("src:\n");
	for (t_int y = 0; y < height; y++) {
		auto* rowptr = src.rowPtr(y);
		for (t_int x = 0; x < width; x++) {

			rowptr[x] = x * s;
			printf("%d ", rowptr[x]);

		}
		printf("\n");
	}

	Image2Matrix(&src, &dst);

	printf("dst:\n");
	for (t_int y = 0; y < height; y++) {
		auto* rowptr = dst.rowPtr(y);
		for (t_int x = 0; x < width; x++) {

			printf("%f ", rowptr[x]);

		}
		printf("\n");
	}

	Matrix2Image(&dst, &src);

	printf("pullback:\n");
	for (t_int y = 0; y < height; y++) {
		auto* rowptr = src.rowPtr(y);
		for (t_int x = 0; x < width; x++) {

			printf("%d ", rowptr[x]);

		}
		printf("\n");
	}

}

void grayscale(Image* src, Image* dst) {

	//#pragma omp prallel for
	for (t_int y = 0; y < src->height(); y++) {

		auto* src_rowptr = src->rowPtr(y);
		auto* dst_rowptr = dst->rowPtr(y);

		for (t_int x = 0; x < src->width(); x++) {

			t_int sum = 0;
			for (t_int c = 0; c < 3; c++) {
				sum += src_rowptr[x * src->channels() + c];
			}
			dst_rowptr[x] = static_cast<t_uchar>(sum / src->channels());

		}

	}

}
void gs3(Image* src, Image* dst) {

	for (t_int y = 0; y < dst->height(); y++) {

		auto* src_rowptr = src->rowPtr(y);
		auto* dst_rowptr = dst->rowPtr(y);

		for (t_int x = 0; x < dst->width(); x++) {

			for (t_int c = 0; c < 3; c++) {

				dst_rowptr[x * dst->channels() + c] = src_rowptr[x];
				
			}

		}

	}

}
// void test_fft4Image(void) {

// 	Image input;
// 	if (decodePNG(&input, IMG_PATH_PNG) != t_err_None) {
// 		return;
// 	}

// 	const t_uintpoint2d img_size = calcPaddingSize2d(&input);

// 	Image plane[] = {
// 		Image(img_size.x, img_size.y, 1),
// 		Image(img_size.x, img_size.y, 1),
// 		Image(img_size.x, img_size.y, 1),
// 		Image(img_size.x, img_size.y, 1),
// 	};

// 	// padding
// 	calcPaddingSize2d()

// 	// FFT
// 	Matrix<t_float> dst_real(padded_cols, padded_rows);
// 	Matrix<t_float> dst_imag(padded_cols, padded_rows);
// 	Matrix<t_float> ifft_real(padded_cols, padded_rows);
// 	Matrix<t_float> ifft_imag(padded_cols, padded_rows);

// 	Fourier2d fourier;
// 	t_uint64 size = fourier.PreSetup(
// 		&src_real, &src_imag,
// 		&dst_real, &dst_imag,
// 		&ifft_real, &ifft_imag
// 	);
// 	void* buffer = malloc(size);
// 	fourier.Setup(buffer);
// 	fourier.fft();

// 	Matrix2Image(&dst_real, &src);
// 	gs3(&src, &input);
// 	encodePNG(&input, "fourierTransformedRealPart.png");

// 	// Low Pass Filter
// 	t_int lp_size = 50;
// 	for (t_int y = 0; y < lp_size; y++) {
// 		auto real_rowptr = dst_real.rowPtr(y);
// 		auto imag_rowptr = dst_imag.rowPtr(y);
// 		for (t_int x = 0; x < lp_size; x++) {
// 			real_rowptr[x] = imag_rowptr[x] = 0.f;
// 		}
// 	}

// 	fourier.ifft();
// 	// FFT

// 	Matrix<t_float> ifft_real_unpadded(cols, rows);
// 	for (t_uint y = 0; y < rows; ++y) {
// 		const t_float* src_row = ifft_real.rowPtr(y);
// 		t_float* dst_row = ifft_real_unpadded.rowPtr(y);
// 		for (t_uint x = 0; x < cols; ++x) {
// 			dst_row[x] = src_row[x];
// 		}
// 	}

// 	if (Matrix2Image(&ifft_real_unpadded, &src) != t_err_None) {
// 		free(buffer);
// 		return;
// 	}

// 	gs3(&src, &input);
// 	encodePNG(&input, "pullback.png");
	
// 	free(buffer);

// }

void test_split_merge(void) {

	Image input;
	if (decodePNG(&input, IMG_PATH_PNG) != t_err_None) {
		return;
	}

	printf("input img channels=%d\n", input.channels());

	Image dst_planes[] = {
		Image(input.width(), input.height(), input.channels()),
		Image(input.width(), input.height(), input.channels()),
		Image(input.width(), input.height(), input.channels()),
		Image(input.width(), input.height(), input.channels())
	};
	split(&input, dst_planes);

	merge(dst_planes, &input);

	encodePNG(&input, "split_and_merged.png");

}

void test_avx(void) {

	t_uchar* src = core::mem::alignedMalloc<t_uchar>(sizeof(t_uchar) * 32, 32);
	t_uchar* dst = core::mem::alignedMalloc<t_uchar>(sizeof(t_uchar) * 32, 32);

	for (t_int i = 0; i < 32; i++) {
		src[i] = (t_int)i;
		dst[i] = 0xFF;
	}

	printf("src is %s\ndst is %s\n", core::mem::check_align(src, 32) ? "aligned" : "not aligned", core::mem::check_align(dst, 32) ? "aligned" : "not aligned");

	core::simd::v_uint8x32 mem;
	core::simd::v256_load_8x32(src, mem);

	core::simd::v256_store_8x32(mem, dst);

	for (t_int i = 0; i < 32; i++) {
		printf("%x, ", dst[i]);
	}
	printf("\n");

}

void test_jpeg(void) {

	Image src;
	decodeJPEG(&src, IMG_PATH_JPG);

	encodeJPEG(&src, "test.jpg");
	encodePNG(&src, "test_jpg2.png");

}

t_int main(void) {

	//test_Matrix();
	
	//test_fft();
	//test_Fourier1d();

	// test_transpose();
	// test_Fourier2dBlock();

	// test_Fourier2d();

	// test_Image2Matrix();

	// test_fft4Image();

	test_split_merge();

	printf("vendor:%s\n", core::t_CPU_INFO.vendor == core::t_cpu_vendor_Intel ? "Intel": "others");
	printf("processors:\n");
	printf("SSE4_2 : %d\n", core::t_CPU_INFO.processor & core::t_cpu_processor_SSE4_2);
	printf("AVX : %d\n", core::t_CPU_INFO.processor & core::t_cpu_processor_AVX);
	printf("AVX2 : %d\n", core::t_CPU_INFO.processor & core::t_cpu_processor_AVX2);
	printf("AVX512f : %d\n", core::t_CPU_INFO.processor & core::t_cpu_processor_AVX512f);

	test_jpeg();

    Image src, dst;
    decodePNG(&src, IMG_PATH_PNG);
	dst.allocate(src.width(), src.height(), src.channels());

	if (core::t_CPU_INFO.processor & core::t_cpu_processor_AVX) {

		test_avx();

	}

	printf("start.\n");
	swap(&src, &dst);
	printf("finish.\n");

	// ImageクラスをFFT用パディングにするのはユーザー側が指示する．
	// メモリ管理領域が外部の可能性を考慮すると，パディングによるデータ領域の新たな確保が必要なため．

    encodePNG(&dst, "test.png");
    
    return 0;

}