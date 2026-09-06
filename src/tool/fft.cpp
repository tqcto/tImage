#include "../../include/tool/fft.h"

//#include <bit>		// C++20
//#include <numbers>	// C++20
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <omp.h>

namespace tImage {

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

    t_uint padding4fft(t_uint n) {

        //t_uint ceilN = std::bit_ceil(n); // C++20
        t_uint floorN = bit_floor(n);
        return floorN << (n != floorN);

    }

    // iは普通のインデックスで、mはビット数
    inline t_int reverseIndex(t_int index, t_int m) {

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
        #pragma omp parallel for
        for (t_int i = 0; i < N; i++) {

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
            for (t_int k = 0; k < N; k += step) {
                for (t_int j = step >> 1; j < step; j++) {

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
        for (t_int i = 0; i < N; i++) {

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
            for (t_int j = 0; j < step >> 1; j++) {
                t_float angle = tmp * static_cast<t_float>(j);
                tmp_real[j] = std::cos(angle);
                tmp_imag[j] = std::sin(angle);
            }

            // メインのバタフライ演算全体
            for (t_int k = 0; k < N; k += step) {
                for (t_int j = step >> 1; j < step; j++) {

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
        for (t_int i = 0; i < N; i++) {
            dst_real[i] *= scale;
            dst_imag[i] *= scale;
        }

    }

}