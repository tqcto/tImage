#pragma once

#include "..\tImage_definition.h"
#include "..\image\Image.h"

namespace tImage {

    typedef struct {
        t_float *real, *imag;
    } ComplexMems;

    // FFT用にn以上の最小び2の累乗数を計算
    DLL_EXPORT t_uint calc_paddinhg(t_uint n);

    // 1次元FFT/IFFTクラス
    class Fourier1d {

    private:

        // データ数
        t_uint N;
        
        // メモリ
        ComplexMems fftsrc, fftdst, ifftdst, rot_buffer;

        // バタフライ演算の階層数(事前計算)
        t_uint M = 0;
        // ビット反転用インデックステーブル(事前計算)
        t_int* rev_table = nullptr;
        // 事前計算
        void PreCalc(void);

    public:

        DLL_EXPORT Fourier1d(void);
        //DLL_EXPORT ~Fourier();

        // データサイズの指定と，FFT及びIFFTの入力・出力複素数メモリ，一時メモリの指定
        // _N : データ数（2の累乗数である必要あり）
        // _fft_src_real, _fft_src_imag : FFT用入力データ列
        // _fft_dst_real, _fft_dst_imag : FFT用出力データ列
        // _ifft_src_real, _ifft_src_imag : IFFT用入力データ列
        // _ifft_dst_real, _ifft_dst_imag : IFFT用出力データ列
        // _rot_buffer : 回転因子用バッファメモリ(サイズ: sizeof(t_float) * _N)
        // _index_buffer : ビット反転用インデックステーブルのメモリ(サイズ: sizeof(t_int) * _N)
        DLL_EXPORT void PrePlan(
            t_uint _N,
            
            t_float* _fft_src_real, t_float* _fft_src_imag,
            t_float* _fft_dst_real, t_float* _fft_dst_imag,
            t_float* _ifft_dst_real, t_float* _ifft_dst_imag,

            t_float* _rot_buffer, t_int* _index_buffer
        );

        DLL_EXPORT void fft(void);
        DLL_EXPORT void ifft(void);

        // ベンチマーク用の全く最適化していない離散フーリエ変換
        /*
        DLL_EXPORT void dft(void);
        DLL_EXPORT void idft(void);
        */

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