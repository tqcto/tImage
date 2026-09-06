#pragma once

#include "..\tImage_definition.h"
#include "..\image\Image.h"

namespace tImage {

    // FFT用にn以上の最小び2の累乗数を計算
    DLL_EXPORT t_uint calc_paddinhg(t_uint n);

    // 1次元FFT/IFFTクラス
    class Fourier1d {

    private:

        // データ数
        t_uint N;

        // メモリ
        t_float *src_real, *src_imag;   // 入力データ列
        t_float *dst_real, *dst_imag;   // 出力データ列
        t_float *tmp_real, *tmp_imag;   // メモリバッファ

    public:

        // _N : データ数
        // _src_real, _src_imag : 入力データ列
        // _dst_real, _dst_imag : 出力データ列
        // buffer : バッファメモリ(サイズ: sizeof(t_float) * _N)
        DLL_EXPORT Fourier1d(
            t_uint _N,
            t_float* _src_real, t_float* _src_imag,
            t_float* _dst_real, t_float* _dst_imag,
            t_float* buffer
        );
        //DLL_EXPORT ~Fourier();

        // 事前に計算できるものを一度に計算．
        DLL_EXPORT t_err PreCalc(void);

        DLL_EXPORT void fft(void);
        DLL_EXPORT void ifft(void);

    };

    // 1次元FFT
    // Nは2の累乗である必要がある．そのためには，padding4fft()を持ちいる．
    // tmp1, tmp2はdstのサイズと一致させる．
    DLL_EXPORT void fft1d(t_float* src_real, t_float* src_imag, t_float* dst_real, t_float* dst_imag, t_float* tmp_real, t_float* tmp_imag, t_uint N);

    // 1次元IFFT
    // Nは2の累乗である必要がある．そのためには，padding4fft()を持ちいる．
    // tmp1, tmp2はdstのサイズと一致させる．
    DLL_EXPORT void ifft1d(t_float* src_real, t_float* src_imag, t_float* dst_real, t_float* dst_imag, t_float* tmp_real, t_float* tmp_imag, t_uint N);

}