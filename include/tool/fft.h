#pragma once

#include "..\tImage_definition.h"
#include "..\image\Image.h"

namespace tImage {

    // FFT用にn以上の最小び2の累乗数を計算
    DLL_EXPORT t_uint calc_paddinhg(t_uint n);

    // 1次元FFT/IFFTクラス
    class Fourier1d {

    public:

        // データ数
        t_uint N;

        // メモリ
        t_float *src_real, *src_imag;   // 入力データ列
        t_float *dst_real, *dst_imag;   // 出力データ列
        t_float *tmp_real, *tmp_imag;   // メモリバッファ

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

        // ベンチマーク用の全く最適化していない離散フーリエ変換
        /*
        DLL_EXPORT void dft(void);
        DLL_EXPORT void idft(void);
        */

    };

    class Fourier2d {

    private:
        Matrix<t_float> *src_real, *src_imag;
        Matrix<t_float> *dst_real, *dst_imag;
        Matrix<t_float> *trans1_real, *trans1_imag;
        Matrix<t_float> *trans2_real, *trans2_imag;

        // 各1dfft用
        t_float *buffer4cols, *buffer4rows;

        // 各方向の長さ(余白部分も含む)
        t_uint cols_length, rows_length;

        // 事前計算
        void PreCalc(void);

    public:
        // _src_real, _src_imag : 入力画像データ(float型)
        // _dst_real, _dst_imag : 出力画像データ(float型)
        // _trans1_real, _trans1_imag, _trans2_real, _trans2_imag : 転置用バッファメモリ(横幅と縦幅を転置して確保したもの)
        // reserved1 : パフォーマンス向上用バッファ(trans1の余白を含む横幅分のメモリ)
        // reserved2 : パフォーマンス向上用バッファ(trans2の余白を含む横幅分のメモリ)
        DLL_EXPORT Fourier2d(
            Matrix<t_float>* _src_real, Matrix<t_float>* _src_imag,
            Matrix<t_float>* _dst_real, Matrix<t_float>* _dst_imag,
            Matrix<t_float>* _trans1_real, Matrix<t_float>* _trans1_imag,
            Matrix<t_float>* _trans2_real, Matrix<t_float>* _trans2_imag,
            t_float* reserved1, t_float* reserved2
        );

        DLL_EXPORT ~Fourier2d();

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