#include "../../include/tool/fft.h"
#include "../../include/tool/transpose.h"

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

    Fourier1d::Fourier1d(
        t_uint _N,
        t_float* _src_real, t_float* _src_imag,
        t_float* _dst_real, t_float* _dst_imag,
        t_float* buffer
    ) {

        this->N = _N;
        this->src_real = _src_real;
        this->src_imag = _src_imag;
        this->dst_real = _dst_real;
        this->dst_imag = _dst_imag;

        this->tmp_real = buffer;
        this->tmp_imag = buffer + (_N >> 1);

    }

    t_err Fourier1d::PreCalc(void) {

        const t_uint halfN = N >> 1;
        constexpr t_float PI2f = static_cast<t_float>(M_PI * 2.0);
        const t_float angle_step = PI2f / static_cast<t_float>(N);

        for (t_uint j = 0; j < halfN; j++) {

            t_float angle = angle_step * static_cast<t_float>(j);
            tmp_real[j] = std::cos(angle);
            tmp_imag[j] = std::sin(angle);

        }

        return t_err_None;

    }

    void Fourier1d::fft(void) {

        // バタフライ演算の階層数(=log2(N))
        // t_uint M = std::bit_width(N) - 1; // C++20
        t_uint M = 0;
        while ( (1u << M) < this->N ) {
            M++;
        }

        // まずはdstをバッファとして使う．
        for (t_int i = 0; i < this->N; i++) {

            t_uint r = reverseIndex(i, M);
            this->dst_real[i] = this->src_real[r];
            this->dst_imag[i] = this->src_imag[r];

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
                    t_float WXcosBuf = this->dst_real[downBuff] * this->tmp_real[Wbuff] - this->dst_imag[downBuff] * this->tmp_imag[Wbuff];
                    t_float WXsinBuf = this->dst_real[downBuff] * this->tmp_imag[Wbuff] + this->dst_imag[downBuff] * this->tmp_real[Wbuff];

                    // 演算結果を格納
                    this->dst_real[downBuff] = this->dst_real[upBuff] - WXcosBuf;
                    this->dst_imag[downBuff] = this->dst_imag[upBuff] - WXsinBuf;
                    this->dst_real[upBuff] += WXcosBuf;
                    this->dst_imag[upBuff] += WXsinBuf;

                }
            }

        }

    }

    void Fourier1d::ifft(void) {

        // バタフライ演算の階層数(=log2(N))
        // t_uint M = std::bit_width(N) - 1; // C++20
        t_uint M = 0;
        while ( (1u << M) < this->N ) {
            M++;
        }

        // まずはdstをバッファとして使う．
        for (t_int i = 0; i < this->N; i++) {

            t_uint r = reverseIndex(i, M);
            this->src_real[r] = this->dst_real[i];
            this->src_imag[r] = this->dst_imag[i];

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
                    t_float WXcosBuf = this->src_real[downBuff] * this->tmp_real[Wbuff] + this->src_imag[downBuff] * this->tmp_imag[Wbuff];
                    t_float WXsinBuf = this->src_imag[downBuff] * this->tmp_real[Wbuff] - this->src_real[downBuff] * this->tmp_imag[Wbuff];

                    // 演算結果を格納
                    this->src_real[downBuff] = this->src_real[upBuff] - WXcosBuf;
                    this->src_imag[downBuff] = this->src_imag[upBuff] - WXsinBuf;
                    this->src_real[upBuff] += WXcosBuf;
                    this->src_imag[upBuff] += WXsinBuf;

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
                    t_float WXcosBuf = this->src_real[downBuff] * this->tmp_real[Wbuff] + this->src_imag[downBuff] * this->tmp_imag[Wbuff];
                    t_float WXsinBuf = this->src_imag[downBuff] * this->tmp_real[Wbuff] - this->src_real[downBuff] * this->tmp_imag[Wbuff];

                    // 演算結果を格納
                    // 正規化係数をかける
                    // そうすれば，芋づる式にすべての係数も正規化される
                    this->src_real[downBuff]  = (this->src_real[upBuff] - WXcosBuf) * scale;
                    this->src_imag[downBuff]  = (this->src_imag[upBuff] - WXsinBuf) * scale;
                    this->src_real[upBuff]    = (this->src_real[upBuff] + WXcosBuf) * scale;
                    this->src_imag[upBuff]    = (this->src_imag[upBuff] + WXsinBuf) * scale;

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

    void Fourier2d::PreCalc(void) {

        // 行方向用のFFT
        Fourier1d pre1(this->cols_length, nullptr, nullptr, nullptr, nullptr, this->buffer4cols);
        pre1.PreCalc();

        // 列方向用のFFT
        Fourier1d pre2(this->rows_length, nullptr, nullptr, nullptr, nullptr, this->buffer4rows);
        pre2.PreCalc();

    }

    Fourier2d::Fourier2d(
        Matrix<t_float>* _src_real, Matrix<t_float>* _src_imag,
        Matrix<t_float>* _dst_real, Matrix<t_float>* _dst_imag,
        Matrix<t_float>* _trans1_real, Matrix<t_float>* _trans1_imag,
        Matrix<t_float>* _trans2_real, Matrix<t_float>* _trans2_imag,
        t_float* reserved1, t_float* reserved2
    ) {

        this->src_real = _src_real;
        this->src_imag = _src_imag;
        this->dst_real = _dst_real;
        this->dst_imag = _dst_imag;

        this->trans1_real = _trans1_real;
        this->trans1_imag = _trans1_imag;
        this->trans2_real = _trans2_real;
        this->trans2_imag = _trans2_imag;

        this->cols_length = this->src_real->stride() / sizeof(t_float);
        this->rows_length = this->trans1_real->stride() / sizeof(t_float);

        this->buffer4cols = reserved1;
        this->buffer4rows = reserved2;

        this->PreCalc();

    }

    Fourier2d::~Fourier2d() {



    }

    void Fourier2d::fft(void) {

        t_uint w = this->src_real->cols();
        t_uint h = this->src_real->rows();
        t_uint64 stride = this->src_real->stride();

        #pragma omp parallel default(none) \
                shared(h, w, stride, src_real, src_imag, dst_real, dst_imag)
        {
            
            // preで計算済みなのでlocal_fft.PreCalc()の計算は不要
            Fourier1d local_fft(this->cols_length, nullptr, nullptr, nullptr, nullptr, this->buffer4cols);

            #pragma omp for schedule(static)
            for (t_int i = 0; i < h; i++) {
                t_uint64 ptr = stride * i;
                local_fft.src_real = this->src_real->data + ptr;
                local_fft.src_imag = this->src_imag->data + ptr;
                local_fft.dst_real = this->dst_real->data + ptr;
                local_fft.dst_imag = this->dst_imag->data + ptr;
                local_fft.fft();
            }
        }

        transpose(dst_real, dst_imag, trans1_real, trans1_imag);
        stride = this->trans1_real->stride();
        
        #pragma omp parallel default(none) \
                shared(h, w, stride, trans1_real, trans1_imag, trans2_real, trans2_imag)
        {
            
            // preで計算済みなのでlocal_fft.PreCalc()の計算は不要
            Fourier1d local_fft(this->rows_length, nullptr, nullptr, nullptr, nullptr, this->buffer4rows);

            #pragma omp for schedule(static)
            for (t_int i = 0; i < w; i++) {

                t_uint64 ptr = stride * i;
                
                local_fft.src_real = this->trans1_real->data + ptr;
                local_fft.src_imag = this->trans1_imag->data + ptr;
                local_fft.dst_real = this->trans2_real->data + ptr;
                local_fft.dst_imag = this->trans2_imag->data + ptr;

                local_fft.fft();
            
            }
        }

        transpose(trans2_real, trans2_imag, dst_real, dst_imag);

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