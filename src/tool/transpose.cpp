#include "../../include/tool/transpose.h"

namespace tImage {

    // 汎用
    void REC_TRANSPOSE_any(
        const Image* src, Image* dst,
        t_uint am_start, t_uint an_start,
        t_uint bm_start, t_uint bn_start,
        t_uint m, t_uint n
    ) {


        if (m == 1 && n == 1) {

            for (t_uint c = 0; c < src->channels(); ++c) {
             
                dst->data[bm_start * dst->stride() + bn_start * dst->channels() + c] =
                    src->data[am_start * src->stride() + an_start * src->channels() + c];

            }

            return;

        }

        // このとき，行列srcを左右に，行列dstを上下に分割
        if (n >= m) {

            const t_uint floor = n >> 1;
            const t_uint ceil = (n + 1) >> 1;

            REC_TRANSPOSE_any(src, dst, am_start, an_start, bm_start, bn_start, m, floor);                  // (A_1, B_1)
            REC_TRANSPOSE_any(src, dst, am_start, an_start + floor, bm_start + floor, bn_start, m, ceil);   // (A_2, B_2)

        }
        // このとき，行列srcを上下に，行列dstを左右に分割
        else {

            const t_uint floor = m >> 1;
            const t_uint ceil = (m + 1) >> 1;

            REC_TRANSPOSE_any(src, dst, am_start, an_start, bm_start, bn_start, floor, n);                  // (A_1, B_1)
            REC_TRANSPOSE_any(src, dst, am_start + floor, an_start, bm_start, bn_start + floor, ceil, n);   // (A_2, B_2)

        }

    }

    // 1チャンネル
    void REC_TRANSPOSE_1ch(
        const Image* src, Image* dst,
        t_uint am_start, t_uint an_start,
        t_uint bm_start, t_uint bn_start,
        t_uint m, t_uint n
    ) {


        // **************************************************************************** //
        // ここをSIMD化したい
        // 具体的には，1x1ピクセルになるよりも手前の，3x3ピクセルなどの時点でSIMD化
        if (m == 1 && n == 1) {

            dst->data[bm_start * dst->stride() + bn_start] =
                    src->data[am_start * src->stride() + an_start];

            return;

        }
        // **************************************************************************** //

        // このとき，行列srcを左右に，行列dstを上下に分割
        if (n >= m) {

            const t_uint floor = n >> 1;
            const t_uint ceil = (n + 1) >> 1;

            REC_TRANSPOSE_1ch(src, dst, am_start, an_start, bm_start, bn_start, m, floor);                  // (A_1, B_1)
            REC_TRANSPOSE_1ch(src, dst, am_start, an_start + floor, bm_start + floor, bn_start, m, ceil);   // (A_2, B_2)

        }
        // このとき，行列srcを上下に，行列dstを左右に分割
        else {

            const t_uint floor = m >> 1;
            const t_uint ceil = (m + 1) >> 1;

            REC_TRANSPOSE_1ch(src, dst, am_start, an_start, bm_start, bn_start, floor, n);                  // (A_1, B_1)
            REC_TRANSPOSE_1ch(src, dst, am_start + floor, an_start, bm_start, bn_start + floor, ceil, n);   // (A_2, B_2)

        }

    }

    // 1チャンネル
    void REC_TRANSPOSE_4ch(
        const Image* src, Image* dst,
        t_uint am_start, t_uint an_start,
        t_uint bm_start, t_uint bn_start,
        t_uint m, t_uint n
    ) {


        if (m == 1 && n == 1) {

            // **************************************************************************** //
            // ここをSIMD化したい
            for (t_uint c = 0; c < 4; ++c) {
             
                dst->data[bm_start * dst->stride() + bn_start << 1 + c] =
                    src->data[am_start * src->stride() + an_start << 1 + c];

            }
            // **************************************************************************** //

            return;

        }

        // このとき，行列srcを左右に，行列dstを上下に分割
        if (n >= m) {

            const t_uint floor = n >> 1;
            const t_uint ceil = (n + 1) >> 1;

            REC_TRANSPOSE_4ch(src, dst, am_start, an_start, bm_start, bn_start, m, floor);                  // (A_1, B_1)
            REC_TRANSPOSE_4ch(src, dst, am_start, an_start + floor, bm_start + floor, bn_start, m, ceil);   // (A_2, B_2)

        }
        // このとき，行列srcを上下に，行列dstを左右に分割
        else {

            const t_uint floor = m >> 1;
            const t_uint ceil = (m + 1) >> 1;

            REC_TRANSPOSE_4ch(src, dst, am_start, an_start, bm_start, bn_start, floor, n);                  // (A_1, B_1)
            REC_TRANSPOSE_4ch(src, dst, am_start + floor, an_start, bm_start, bn_start + floor, ceil, n);   // (A_2, B_2)

        }

    }

    t_err transpose(Image* src, Image* dst) {

        const t_uint width = src->width();
        const t_uint height = src->height();

        dst->release();
        t_err err = dst->allocate(height, width, src->channels(), src->depth());
        if (err != t_err_None) return err;

        REC_TRANSPOSE_any(src, dst, 0, 0, 0, 0, height, width);

        return err;

    }

}