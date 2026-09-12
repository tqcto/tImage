#pragma once

#include "../tImage_definition.h"
#include "../image/Image.h"

namespace tImage {

    typedef struct {
        t_float *real, *imag;
    } ComplexMems;

    // Fourier2d が使用する外部ワークスペース
    // block_* 配列のサイズは blockSize * blockSize、column_*、rotation_buffer、
    // index_buffer のサイズは blockSize。
    typedef struct {
        t_float* block_src_real;
        t_float* block_src_imag;
        t_float* block_fft_real;
        t_float* block_fft_imag;
        t_float* column_src_real;
        t_float* column_src_imag;
        t_float* column_fft_real;
        t_float* column_fft_imag;
        t_float* rotation_buffer;
        t_int* index_buffer;
    } Fourier2dWorkspace;

    // FFT用にn以上の最小び2の累乗数を計算
    DLL_EXPORT t_uint calc_paddinhg(t_uint n);

    // n以上でblockSizeの倍数となるサイズを計算
    DLL_EXPORT t_uint calcPaddedSize(t_uint n, t_uint blockSize);

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

        // FFT及びIFFTの入力・出力複素数メモリの再指定
        // この関数により再指定する場合，事前計算済みであることを前提とする
        // _fft_src_real, _fft_src_imag : FFT用入力データ列
        // _fft_dst_real, _fft_dst_imag : FFT用出力データ列
        // _ifft_src_real, _ifft_src_imag : IFFT用入力データ列
        // _ifft_dst_real, _ifft_dst_imag : IFFT用出力データ列
        DLL_EXPORT void RePlan(
            t_float* _fft_src_real, t_float* _fft_src_imag,
            t_float* _fft_dst_real, t_float* _fft_dst_imag,
            t_float* _ifft_dst_real, t_float* _ifft_dst_imag
        );

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

        Matrix<t_float> *fft_src_real, *fft_src_imag;
        Matrix<t_float> *fft_dst_real, *fft_dst_imag;
        Matrix<t_float> *ifft_dst_real, *ifft_dst_imag;

        Fourier2dWorkspace workspace = {};
        Fourier1d block_fourier;
        t_bool block_plan_ready = false;

        t_uint cols = 0;
        t_uint rows = 0;
        t_uint blockSize = 0;

        // ブロック単位の2次元FFT
        void block_fft(t_uint block_x, t_uint block_y);
        void block_ifft(t_uint block_x, t_uint block_y);

    public:
        DLL_EXPORT Fourier2d(void);

        // データサイズの指定と，FFT及びIFFTの入力・出力複素行列メモリ，一時メモリの指定
        // 行列の各行・各列は2の累乗数である必要がある
        // また，すべての行列のalignが同じ値である必要がある
        // _fft_src_real, _fft_src_imag : FFT用入力行列
        // _fft_dst_real, _fft_dst_imag : FFT用出力行列
        // _ifft_src_real, _ifft_src_imag : IFFT用入力行列
        // _ifft_dst_real, _ifft_dst_imag : IFFT用出力行列
        // blockSize は align / sizeof(t_float)。block 配列は blockSize * blockSize 要素、
        // その他の配列は blockSize 要素。
        DLL_EXPORT void PrePlan(
            Matrix<t_float>* _fft_src_real, Matrix<t_float>* _fft_src_imag,
            Matrix<t_float>* _fft_dst_real, Matrix<t_float>* _fft_dst_imag,
            Matrix<t_float>* _ifft_dst_real, Matrix<t_float>* _ifft_dst_imag,
            Fourier2dWorkspace* _workspace
        );

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