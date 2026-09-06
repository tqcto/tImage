#include <iostream>

#include <tImage.h>

//#include <bit>		// C++20
//#include <numbers>	// C++20
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

// using in debug
#include <stdlib.h>

#define IMG_PATH "..\\..\\..\\img.png"

using namespace tImage;

// n以下の最大の2の累乗数を計算
inline t_uint bit_floor(t_uint n) {

	// t_uint is 32bit
	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;

	return n - (n >> 1);

}

// FFT用にnを2の累乗数にパディングする
inline t_uint padding4fft(t_uint n) {

	//t_uint ceilN = std::bit_ceil(n); // C++20
	t_uint floorN = bit_floor(n);
	return floorN << (n != floorN);

}

// iは普通のインデックスで、mはビット数
t_int reverseIndex(t_int index, t_int m) {

	t_int t = 0;
	
	//ビット反転処理
	for (t_int i = m - 1; i >= 0; i--) {
		t |= (0x00000001 & (index >> i)) << ((m - 1) - i);	//一番左のビットから論理和して右に詰めていく
	}
	
	return t;

}

// 1次元FFT
// Nは2の累乗である必要がある．そのためには，padding4fft()を持ちいる．
// tmp1, tmp2はdstのサイズと一致させる．
void fft1d(t_float* src_real, t_float* src_imag, t_float* dst_real, t_float* dst_imag, t_float* tmp_real, t_float* tmp_imag, t_uint N) {

	// 2次元に拡張するときにこれを排除して，Imageクラスが空の時にエラーを吐くようにする．
	if (N == 0) return;

	// バタフライ演算の階層数(=log2(N))
	// t_uint M = std::bit_width(N) - 1; // C++20
	t_uint M = 0;
	while ( (1u << M) < N ) {
		M++;
	}

	// まずはdstをバッファとして使う．
	for (t_uint i = 0; i < N; i++) {

		t_uint r = reverseIndex(i, M);
		dst_real[i] = src_real[r];
		dst_imag[i] = src_imag[r];

	}

	t_uint step = 1;
	for (t_uint i = 0; i < M; i++) {

		// 再帰ステップを2倍していく
		step <<= 1;

		// あらかじめ必要な係数を計算
		//constexpr t_float PI2f = std::numbers::pi_v<t_float> * 2.0f;	// C++20
		constexpr t_float PI2f = M_PI * 2.0f;
		t_float tmp = PI2f / static_cast<float>(step);
		for (t_uint j = 0; j < step >> 1; j++) {
			t_float angle = tmp * static_cast<t_float>(j);
			tmp_real[j] = std::cos(angle);
			tmp_imag[j] = std::sin(angle);
		}

		// メインのバタフライ演算全体
		for (t_uint k = 0; k < N; k += step) {
			for (t_uint j = step >> 1; j < step; j++) {

				// バタフライの上側を示すインデックス
				t_int upBuff = k + j - (step >> 1);
				// バタフライの下側を示すインデックス
				t_int downBuff = k + j;
				// かける係数を示すインデックス
				t_int Wbuff = j - (step >> 1);

				// バタフライ演算
				t_float WXcosBuf = dst_real[downBuff] * tmp_real[Wbuff] - dst_imag[downBuff] * tmp_imag[Wbuff];
				t_float WXsinBuf = dst_real[downBuff] * tmp_imag[Wbuff] + dst_imag[downBuff] * tmp_real[Wbuff];

				// 演算結果を格納
				dst_real[downBuff] = dst_real[upBuff] - WXcosBuf;
				dst_imag[downBuff] = dst_imag[upBuff] - WXsinBuf;
				dst_real[upBuff] += WXcosBuf;
				dst_imag[upBuff] += WXsinBuf;

			}
		}

	}


}

// 1次元IFFT
// Nは2の累乗である必要がある．そのためには，padding4fft()を持ちいる．
// tmp1, tmp2はdstのサイズと一致させる．
void ifft1d(t_float* src_real, t_float* src_imag, t_float* dst_real, t_float* dst_imag, t_float* tmp_real, t_float* tmp_imag, t_uint N) {

	// 2次元に拡張するときにこれを排除して，Imageクラスが空の時にエラーを吐くようにする．
	if (N == 0) return;

	// バタフライ演算の階層数(=log2(N))
	// t_uint M = std::bit_width(N) - 1; // C++20
	t_uint M = 0;
	while ( (1u << M) < N ) {
		M++;
	}

	// まずはdstをバッファとして使う．
	for (t_uint i = 0; i < N; i++) {

		t_uint r = reverseIndex(i, M);
		dst_real[i] = src_real[r];
		dst_imag[i] = src_imag[r];

	}

	t_uint step = 1;
	for (t_uint i = 0; i < M; i++) {

		// 再帰ステップを2倍していく
		step <<= 1;

		// あらかじめ必要な係数を計算
		//constexpr t_float PI2f = std::numbers::pi_v<t_float> * 2.0f;	// C++20
		constexpr t_float PI2f = M_PI * 2.0f;
		t_float tmp = PI2f / static_cast<float>(step);
		for (t_uint j = 0; j < step >> 1; j++) {
			t_float angle = tmp * static_cast<t_float>(j);
			tmp_real[j] = std::cos(angle);
			tmp_imag[j] = std::sin(angle);
		}

		// メインのバタフライ演算全体
		for (t_uint k = 0; k < N; k += step) {
			for (t_uint j = step >> 1; j < step; j++) {

				// バタフライの上側を示すインデックス
				t_int upBuff = k + j - (step >> 1);
				// バタフライの下側を示すインデックス
				t_int downBuff = k + j;
				// かける係数を示すインデックス
				t_int Wbuff = j - (step >> 1);

				// バタフライ演算
				t_float WXcosBuf = dst_real[downBuff] * tmp_real[Wbuff] + dst_imag[downBuff] * tmp_imag[Wbuff];
				t_float WXsinBuf = dst_imag[downBuff] * tmp_real[Wbuff] - dst_real[downBuff] * tmp_imag[Wbuff];

				// 演算結果を格納
				dst_real[downBuff] = dst_real[upBuff] - WXcosBuf;
				dst_imag[downBuff] = dst_imag[upBuff] - WXsinBuf;
				dst_real[upBuff] += WXcosBuf;
				dst_imag[upBuff] += WXsinBuf;

			}
		}

	}

	// 正規化係数
	t_float scale = 1.f / static_cast<t_float>(N);
	for (t_uint i = 0; i < N; i++) {
		dst_real[i] *= scale;
		dst_imag[i] *= scale;
	}

}

void test_fft(void) {

	const t_uint N = 20;
	t_uint paddedN = padding4fft(N);
	
	printf("source data:\n");
	t_float* src_real = (t_float*)malloc(sizeof(t_float) * paddedN);
	t_float* src_imag = (t_float*)malloc(sizeof(t_float) * paddedN);
	for (t_uint i = 0; i < paddedN; i++){
		src_real[i] = (t_float)i + 1.f;
		src_imag[i] = 0.f;
		printf("[%u] : %f\n", i, src_real[i]);
	}

	// paddedNに合わせて出力メモリを確保
	t_float* dst_real = (t_float*)malloc(sizeof(t_float) * paddedN);
	t_float* dst_imag = (t_float*)malloc(sizeof(t_float) * paddedN);
	// 一時メモリも確保
	t_float* tmp_real = (t_float*)malloc(sizeof(t_float) * paddedN);
	t_float* tmp_imag = (t_float*)malloc(sizeof(t_float) * paddedN);

	fft1d(src_real, src_imag, dst_real, dst_imag, tmp_real, tmp_imag, paddedN);

	printf("dest data:\n");
	for (t_uint i = 0; i < paddedN; i++){
		printf("[%u] : %f + %fi\n", i, dst_real[i], dst_imag[i]);
	}

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
	for (t_uint i = 0; i < paddedN; i++){
		printf("[%u] : %f + %fi\n", i, src_real[i], src_imag[i]);
	}

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