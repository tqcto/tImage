#include <iostream>

#include <tImage.h>

// using in debug
#include <stdlib.h>

#define IMG_PATH "..\\..\\..\\img.png"

using namespace tImage;

#define _PRE

#ifdef _PRE
#define PRE_LOG	printf
#else
#define PRE_LOG
#endif

void test_fft(void) {

	//const t_uint N = 1920 * 1080 * 3;
	const t_uint N = 10;
	t_uint paddedN = padding4fft(N);
	
	printf("source data:\n");
	t_float* src_real = (t_float*)malloc(sizeof(t_float) * paddedN);
	t_float* src_imag = (t_float*)malloc(sizeof(t_float) * paddedN);
	for (t_uint i = 0; i < paddedN; i++){
		src_real[i] = (t_float)i + 1.f;
		src_imag[i] = 0.f;
		PRE_LOG("[%u] : %f\n", i, src_real[i]);
	}

	printf("start fft.\n");

	// paddedNに合わせて出力メモリを確保
	t_float* dst_real = (t_float*)malloc(sizeof(t_float) * paddedN);
	t_float* dst_imag = (t_float*)malloc(sizeof(t_float) * paddedN);
	// 一時メモリも確保
	t_float* tmp_real = (t_float*)malloc(sizeof(t_float) * paddedN);
	t_float* tmp_imag = (t_float*)malloc(sizeof(t_float) * paddedN);

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

int main(void) {

	test_fft();
	
    Image src;
    decodePNG(&src, IMG_PATH);

	// ImageクラスをFFT用パディングにするのはユーザー側が指示する．
	// メモリ管理領域が外部の可能性を考慮すると，パディングによるデータ領域の新たな確保が必要なため．

    encodePNG(&src, "test.png");
    
    return 0;

}