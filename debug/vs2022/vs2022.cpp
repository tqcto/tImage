#include <iostream>

#include <tImage.h>

// using in debug
#include <stdlib.h>

#define IMG_PATH "..\\..\\..\\img.png"

using namespace tImage;

//#define _PRE

#ifdef _PRE
#define PRE_LOG	printf
#else
#define PRE_LOG
#endif

void test_Fourier1d(void) {

	//const t_uint N = 1920 * 1080 * 3;
	const t_uint N = 1920 * 3;
	//const t_uint N = 10;
	t_uint paddedN = calc_paddinhg(N);
	
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
	t_uint paddedN = calc_paddinhg(N);
	
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

int main(void) {

	test_Matrix();
	printf("matrix finish.\n");

	//test_fft();
	test_Fourier1d();
	
    Image src;
    decodePNG(&src, IMG_PATH);

	// ImageクラスをFFT用パディングにするのはユーザー側が指示する．
	// メモリ管理領域が外部の可能性を考慮すると，パディングによるデータ領域の新たな確保が必要なため．

    encodePNG(&src, "test.png");
    
    return 0;

}