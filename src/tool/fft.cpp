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

    // iは普通のインデックスで、mはビット数
    inline t_int reverseIndex(t_int index, t_int m) {

        t_int t = 0;
        
        //ビット反転処理
        for (t_int i = m - 1; i >= 0; i--) {
            t |= (0x00000001 & (index >> i)) << ((m - 1) - i);	//一番左のビットから論理和して右に詰めていく
        }
        
        return t;

    }

    t_uint calc_paddinhg(t_uint n) {

        //t_uint ceilN = std::bit_ceil(n); // C++20
        t_uint floorN = bit_floor(n);
        return floorN << (n != floorN);

    }

    Fourier1d::Fourier1d(void) {



    }

    void Fourier1d::PreCalc(void) {

        // バタフライ演算の階層数(=log2(N))
        // t_uint M = std::bit_width(N) - 1; // C++20
        this->M = 0;
        while ( (1u << this->M) < this->N ) {
            this->M++;
        }

        const t_int halfN = N >> 1;
        constexpr t_float PI2f = static_cast<t_float>(M_PI * 2.0);
        const t_float angle_step = PI2f / static_cast<t_float>(N);

        #pragma omp parallel for simd
        for (t_int j = 0; j < halfN; j++) {

            t_float angle = angle_step * static_cast<t_float>(j);
            rot_buffer.real[j] = std::cos(angle);
            rot_buffer.imag[j] = std::sin(angle);

        }

        for (t_int i = 0; i < this->N; i++) {

            this->rev_table[i] = reverseIndex(i, this->M);

        }

    }

    void Fourier1d::PrePlan(
        t_uint _N,
        
        t_float* _fft_src_real, t_float* _fft_src_imag,
        t_float* _fft_dst_real, t_float* _fft_dst_imag,
        t_float* _ifft_dst_real, t_float* _ifft_dst_imag,

        t_float* _rot_buffer, t_int* _index_buffer
    ) {

        this->N = _N;

        this->fftsrc.real = _fft_src_real;
        this->fftsrc.imag = _fft_src_imag;
        this->fftdst.real = _fft_dst_real;
        this->fftdst.imag = _fft_dst_imag;
        this->ifftdst.real = _ifft_dst_real;
        this->ifftdst.imag = _ifft_dst_imag;

        this->rot_buffer.real = _rot_buffer;
        this->rot_buffer.imag = _rot_buffer + (this->N >> 1);

        this->rev_table = _index_buffer;

        this->PreCalc();

    }

    void Fourier1d::fft(void) {

        for (t_int i = 0; i < this->N; i++) {

            t_uint r = this->rev_table[i];
            this->fftdst.real[i] = this->fftsrc.real[r];
            this->fftdst.imag[i] = this->fftsrc.imag[r];

        }

        t_uint step = 1;
        for (t_uint i = 0; i < M; i++) {

            // 再帰ステップを2倍していく
            step <<= 1;

            t_int N_step = N / step;

            // メインのバタフライ演算全体
            for (t_int k = 0; k < N; k += step) {
                #pragma omp simd
                for (t_int j = step >> 1; j < step; j++) {

                    // バタフライの上側を示すインデックス
                    t_int upBuff = k + j - (step >> 1);
                    // バタフライの下側を示すインデックス
                    t_int downBuff = k + j;
                    // かける係数を示すインデックス
                    t_int Wbuff = (j - (step >> 1)) * N_step;

                    // バタフライ演算
                    t_float WXcosBuf = this->fftdst.real[downBuff] * this->rot_buffer.real[Wbuff] - this->fftdst.imag[downBuff] * this->rot_buffer.imag[Wbuff];
                    t_float WXsinBuf = this->fftdst.real[downBuff] * this->rot_buffer.imag[Wbuff] + this->fftdst.imag[downBuff] * this->rot_buffer.real[Wbuff];

                    // 演算結果を格納
                    this->fftdst.real[downBuff] = this->fftdst.real[upBuff] - WXcosBuf;
                    this->fftdst.imag[downBuff] = this->fftdst.imag[upBuff] - WXsinBuf;
                    this->fftdst.real[upBuff] += WXcosBuf;
                    this->fftdst.imag[upBuff] += WXsinBuf;

                }
            }

        }

    }

    void Fourier1d::ifft(void) {

        for (t_int i = 0; i < this->N; i++) {

            t_uint r = this->rev_table[i];
            this->ifftdst.real[r] = this->fftdst.real[i];
            this->ifftdst.imag[r] = this->fftdst.imag[i];

        }

        t_uint step = 1;
        // 最後の一番最初のバタフライ以外
        for (t_uint i = 0; i < M - 1; i++) {

            // 再帰ステップを2倍していく
            step <<= 1;
            t_uint half_step = step >> 1;

            const t_int N_step = N / step;

            // メインのバタフライ演算全体
            for (t_int k = 0; k < this->N; k += step) {
                #pragma omp simd
                for (t_int j = half_step; j < step; j++) {

                    // バタフライの上側を示すインデックス
                    t_int upBuff = k + j - half_step;
                    // バタフライの下側を示すインデックス
                    t_int downBuff = k + j;
                    // かける係数を示すインデックス
                    t_int Wbuff = (j - half_step) * N_step;

                    // バタフライ演算
                    t_float WXcosBuf = this->ifftdst.real[downBuff] * this->rot_buffer.real[Wbuff] + this->ifftdst.imag[downBuff] * this->rot_buffer.imag[Wbuff];
                    t_float WXsinBuf = this->ifftdst.imag[downBuff] * this->rot_buffer.real[Wbuff] - this->ifftdst.real[downBuff] * this->rot_buffer.imag[Wbuff];

                    // 演算結果を格納
                    this->ifftdst.real[downBuff] = this->ifftdst.real[upBuff] - WXcosBuf;
                    this->ifftdst.imag[downBuff] = this->ifftdst.imag[upBuff] - WXsinBuf;
                    this->ifftdst.real[upBuff] += WXcosBuf;
                    this->ifftdst.imag[upBuff] += WXsinBuf;

                }
            }

        }

        // 最初のバタフライだけ
        if (M > 0) {

            step = this->N;
            t_uint half_step = step >> 1;

            // 正規化係数
            t_float scale = 1.f / static_cast<t_float>(N);

            const t_int N_step = N / step;

            // メインのバタフライ演算全体
            for (t_int k = 0; k < this->N; k += step) {
                #pragma omp simd
                for (t_int j = half_step; j < step; j++) {

                    // バタフライの上側を示すインデックス
                    t_int upBuff = k + j - half_step;
                    // バタフライの下側を示すインデックス
                    t_int downBuff = k + j;
                    // かける係数を示すインデックス
                    t_int Wbuff = (j - half_step) * N_step;

                    // バタフライ演算
                    t_float WXcosBuf = this->ifftdst.real[downBuff] * this->rot_buffer.real[Wbuff] + this->ifftdst.imag[downBuff] * this->rot_buffer.imag[Wbuff];
                    t_float WXsinBuf = this->ifftdst.imag[downBuff] * this->rot_buffer.real[Wbuff] - this->ifftdst.real[downBuff] * this->rot_buffer.imag[Wbuff];

                    // 演算結果を格納
                    // 正規化係数をかける
                    // そうすれば，芋づる式にすべての係数も正規化される
                    this->ifftdst.real[downBuff]  = (this->ifftdst.real[upBuff] - WXcosBuf) * scale;
                    this->ifftdst.imag[downBuff]  = (this->ifftdst.imag[upBuff] - WXsinBuf) * scale;
                    this->ifftdst.real[upBuff]    = (this->ifftdst.real[upBuff] + WXcosBuf) * scale;
                    this->ifftdst.imag[upBuff]    = (this->ifftdst.imag[upBuff] + WXsinBuf) * scale;

                }
            }

        }

    }

    /*
    void Fourier1d::dft(void) {
        if (this->N == 0) return;

        constexpr t_float PI2f = static_cast<t_float>(M_PI * 2.0);
        const t_float angle_step = PI2f / static_cast<t_float>(this->N);

        for (t_uint k = 0; k < this->N; k++) {
            t_float sum_real = 0.0f;
            t_float sum_imag = 0.0f;

            for (t_uint n = 0; n < this->N; n++) {
                t_float angle = angle_step * static_cast<t_float>(k * n);
                t_float cos_val = std::cos(angle);
                t_float sin_val = std::sin(angle);

                // W = cos(angle) - i * sin(angle)
                sum_real += this->src_real[n] * cos_val + this->src_imag[n] * sin_val;
                sum_imag += this->src_imag[n] * cos_val - this->src_real[n] * sin_val;
            }

            this->dst_real[k] = sum_real;
            this->dst_imag[k] = sum_imag;
        }
    }

    void Fourier1d::idft(void) {
        if (this->N == 0) return;

        constexpr t_float PI2f = static_cast<t_float>(M_PI * 2.0);
        const t_float angle_step = PI2f / static_cast<t_float>(this->N);
        const t_float scale = 1.0f / static_cast<t_float>(this->N);

        for (t_uint n = 0; n < this->N; n++) {
            t_float sum_real = 0.0f;
            t_float sum_imag = 0.0f;

            for (t_uint k = 0; k < this->N; k++) {
                t_float angle = angle_step * static_cast<t_float>(k * n);
                t_float cos_val = std::cos(angle);
                t_float sin_val = std::sin(angle);

                // W = cos(angle) + i * sin(angle)
                sum_real += this->dst_real[k] * cos_val - this->dst_imag[k] * sin_val;
                sum_imag += this->dst_imag[k] * cos_val + this->dst_real[k] * sin_val;
            }

            this->src_real[n] = sum_real * scale;
            this->src_imag[n] = sum_imag * scale;
        }
    }
    */

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
                #pragma omp simd
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
        // 最後の一番最初のバタフライ以外
        for (t_uint i = 0; i < M - 1; i++) {

            // 再帰ステップを2倍していく
            step <<= 1;
            t_uint half_step = step >> 1;

            // あらかじめ必要な係数を計算
            //constexpr t_float PI2f = std::numbers::pi_v<t_float> * 2.0f;	// C++20
            constexpr t_float PI2f = M_PI * 2.0f;
            t_float tmp = PI2f / static_cast<float>(step);
            for (t_int j = 0; j < half_step; j++) {
                t_float angle = tmp * static_cast<t_float>(j);
                tmp_real[j] = std::cos(angle);
                tmp_imag[j] = std::sin(angle);
            }

            // メインのバタフライ演算全体
            for (t_int k = 0; k < N; k += step) {
                #pragma omp simd
                for (t_int j = half_step; j < step; j++) {

                    // バタフライの上側を示すインデックス
                    t_int upBuff = k + j - half_step;
                    // バタフライの下側を示すインデックス
                    t_int downBuff = k + j;
                    // かける係数を示すインデックス
                    t_int Wbuff = j - half_step;

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

        // 最初のバタフライだけ
        if (M > 0) {

            step = N;
            t_uint half_step = step >> 1;

            // あらかじめ必要な係数を計算
            //constexpr t_float PI2f = std::numbers::pi_v<t_float> * 2.0f;	// C++20
            constexpr t_float PI2f = M_PI * 2.0f;
            t_float tmp = PI2f / static_cast<float>(step);
            for (t_int j = 0; j < half_step; j++) {
                t_float angle = tmp * static_cast<t_float>(j);
                tmp_real[j] = std::cos(angle);
                tmp_imag[j] = std::sin(angle);
            }

            // 正規化係数
            t_float scale = 1.f / static_cast<t_float>(N);

            // メインのバタフライ演算全体
            for (t_int k = 0; k < N; k += step) {
                #pragma omp simd
                for (t_int j = half_step; j < step; j++) {

                    // バタフライの上側を示すインデックス
                    t_int upBuff = k + j - half_step;
                    // バタフライの下側を示すインデックス
                    t_int downBuff = k + j;
                    // かける係数を示すインデックス
                    t_int Wbuff = j - half_step;

                    // バタフライ演算
                    t_float WXcosBuf = dst_real[downBuff] * tmp_real[Wbuff] + dst_imag[downBuff] * tmp_imag[Wbuff];
                    t_float WXsinBuf = dst_imag[downBuff] * tmp_real[Wbuff] - dst_real[downBuff] * tmp_imag[Wbuff];

                    // 演算結果を格納
                    // 正規化係数をかける
                    // そうすれば，芋づる式にすべての係数も正規化される
                    dst_real[downBuff]  = (dst_real[upBuff] - WXcosBuf) * scale;
                    dst_imag[downBuff]  = (dst_imag[upBuff] - WXsinBuf) * scale;
                    dst_real[upBuff]    = (dst_real[upBuff] + WXcosBuf) * scale;
                    dst_imag[upBuff]    = (dst_imag[upBuff] + WXsinBuf) * scale;

                }
            }

        }

    }

}