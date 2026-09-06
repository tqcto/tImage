#pragma once

#include "..\tImage_definition.h"
#include "..\image\Image.h"

namespace tImage {

    // FFT用にn以上の最小び2の累乗数を計算
    DLL_EXPORT t_uint padding4fft(t_uint n);

    // 1次元FFT
    // Nは2の累乗である必要がある．そのためには，padding4fft()を持ちいる．
    // tmp1, tmp2はdstのサイズと一致させる．
    DLL_EXPORT void fft1d(t_float* src_real, t_float* src_imag, t_float* dst_real, t_float* dst_imag, t_float* tmp_real, t_float* tmp_imag, t_uint N);

    // 1次元IFFT
    // Nは2の累乗である必要がある．そのためには，padding4fft()を持ちいる．
    // tmp1, tmp2はdstのサイズと一致させる．
    DLL_EXPORT void ifft1d(t_float* src_real, t_float* src_imag, t_float* dst_real, t_float* dst_imag, t_float* tmp_real, t_float* tmp_imag, t_uint N);

}