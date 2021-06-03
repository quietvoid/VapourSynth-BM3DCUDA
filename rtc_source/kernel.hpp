/*
 * CUDA kernel code for BM3D_RTC
 * Copyright (c) 2003, 2007-14 Matteo Frigo
 * Copyright (c) 2003, 2007-14 Massachusetts Institute of Technology
 * Copyright (c) 2021 WolframRhodium
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

// functions "dct_pack8_interleave4" and "idct_pack8_interleave4"
// are modified from code generated by fftw-3.3.9
// WolframRhodium, 8 May 2021

const auto kernel_header_template = R"""(
template <bool forward>
__device__
static inline void dct(float [8]);

template <bool forward>
__device__
static inline void haar(float [8]);

template <bool forward>
__device__
static inline void wht(float [8]);

template <bool forward>
__device__
static inline void bior1_5(float [8]);
)""";

const auto kernel_source_template = R"""(
/* 
external variables: 
    int width, int height, int stride, 
    float sigma, int block_step, int bm_range, 
    int _radius, int ps_num, int ps_range, 
    float sigma_u, float sigma_v, 
    bool temporal, bool chroma, bool final_
    float FLT_MAX, float FLT_EPSILON, 
    template<bool> __device__ static inline void transform_2d(float [8]), 
    template<bool> __device__ static inline void transform_1d(float [8])
*/

#define FMA(a, b, c) (((a) * (b)) + (c))
#define FMS(a, b, c) (((a) * (b)) - (c))
#define FNMS(a, b, c) ((c) - ((a) * (b)))

__device__ static const int smem_stride = 32 + 1;

template <auto transform_impl, int stride=256, int howmany=8, int howmany_stride=32>
__device__
static inline void transform_pack8_interleave4(
    float * __restrict__ data, float * __restrict__ buffer
) {
    #pragma unroll
    for (int iter = 0; iter < howmany; ++iter, data += howmany_stride) {
        float v[8];

        #pragma unroll
        for (int i = 0; i < 8; ++i) {
            v[i] = data[i * stride];
        }

        transform_impl(v);

        #pragma unroll
        for (int i = 0; i < 8; ++i) {
            data[i * stride] = v[i];
        }
    }
}

// modified from fftw-3.3.9 generated code:
// fftw-3.3.9/rdft/scalar/r2r/e10_8.c and e01_8.c
// 1-D DCT-II/DCT-III (8 points)
// launched by blockDim(x=32, y=1, z=1)
template <bool forward>
__device__
static inline void dct(float v[8]) {
    if constexpr (forward) {
        float KP414213562 {+0.414213562373095048801688724209698078569671875};
        float KP1_847759065 {+1.847759065022573512256366378793576573644833252};
        float KP198912367 {+0.198912367379658006911597622644676228597850501};
        float KP1_961570560 {+1.961570560806460898252364472268478073947867462};
        float KP2_000000000 {+2.000000000000000000000000000000000000000000000};
        float KP1_414213562 {+1.414213562373095048801688724209698078569671875};
        float KP668178637 {+0.668178637919298919997757686523080761552472251};
        float KP1_662939224 {+1.662939224605090474157576755235811513477121624};
        float KP707106781 {+0.707106781186547524400844362104849039284835938};

        auto T1 = v[0];
        auto T2 = v[7];
        auto T3 = T1 - T2;
        auto Tj = T1 + T2;
        auto Tc = v[4];
        auto Td = v[3];
        auto Te = Tc - Td;
        auto Tk = Tc + Td;
        auto T4 = v[2];
        auto T5 = v[5];
        auto T6 = T4 - T5;
        auto T7 = v[1];
        auto T8 = v[6];
        auto T9 = T7 - T8;
        auto Ta = T6 + T9;
        auto Tn = T7 + T8;
        auto Tf = T6 - T9;
        auto Tm = T4 + T5;
        auto Tb = FNMS(KP707106781, Ta, T3);
        auto Tg = FNMS(KP707106781, Tf, Te);
        v[3] = KP1_662939224 * (FMA(KP668178637, Tg, Tb));
        v[5] = -(KP1_662939224 * (FNMS(KP668178637, Tb, Tg)));
        auto Tp = Tj + Tk;
        auto Tq = Tm + Tn;
        v[4] = KP1_414213562 * (Tp - Tq);
        v[0] = KP2_000000000 * (Tp + Tq);
        auto Th = FMA(KP707106781, Ta, T3);
        auto Ti = FMA(KP707106781, Tf, Te);
        v[1] = KP1_961570560 * (FNMS(KP198912367, Ti, Th));
        v[7] = KP1_961570560 * (FMA(KP198912367, Th, Ti));
        auto Tl = Tj - Tk;
        auto To = Tm - Tn;
        v[2] = KP1_847759065 * (FNMS(KP414213562, To, Tl));
        v[6] = KP1_847759065 * (FMA(KP414213562, Tl, To));
    } else {
        float KP1_662939224 {+1.662939224605090474157576755235811513477121624};
        float KP668178637 {+0.668178637919298919997757686523080761552472251};
        float KP1_961570560 {+1.961570560806460898252364472268478073947867462};
        float KP198912367 {+0.198912367379658006911597622644676228597850501};
        float KP1_847759065 {+1.847759065022573512256366378793576573644833252};
        float KP707106781 {+0.707106781186547524400844362104849039284835938};
        float KP414213562 {+0.414213562373095048801688724209698078569671875};
        float KP1_414213562 {+1.414213562373095048801688724209698078569671875};

        auto T1 = v[0];
        auto T2 = v[4];
        auto T3 = FMA(KP1_414213562, T2, T1);
        auto Tj = FNMS(KP1_414213562, T2, T1);
        auto T4 = v[2];
        auto T5 = v[6];
        auto T6 = FMA(KP414213562, T5, T4);
        auto Tk = FMS(KP414213562, T4, T5);
        auto T8 = v[1];
        auto Td = v[7];
        auto T9 = v[5];
        auto Ta = v[3];
        auto Tb = T9 + Ta;
        auto Te = Ta - T9;
        auto Tc = FMA(KP707106781, Tb, T8);
        auto Tn = FNMS(KP707106781, Te, Td);
        auto Tf = FMA(KP707106781, Te, Td);
        auto Tm = FNMS(KP707106781, Tb, T8);
        auto T7 = FMA(KP1_847759065, T6, T3);
        auto Tg = FMA(KP198912367, Tf, Tc);
        v[7] = FNMS(KP1_961570560, Tg, T7);
        v[0] = FMA(KP1_961570560, Tg, T7);
        auto Tp = FNMS(KP1_847759065, Tk, Tj);
        auto Tq = FMA(KP668178637, Tm, Tn);
        v[5] = FNMS(KP1_662939224, Tq, Tp);
        v[2] = FMA(KP1_662939224, Tq, Tp);
        auto Th = FNMS(KP1_847759065, T6, T3);
        auto Ti = FNMS(KP198912367, Tc, Tf);
        v[3] = FNMS(KP1_961570560, Ti, Th);
        v[4] = FMA(KP1_961570560, Ti, Th);
        auto Tl = FMA(KP1_847759065, Tk, Tj);
        auto To = FNMS(KP668178637, Tn, Tm);
        v[6] = FNMS(KP1_662939224, To, Tl);
        v[1] = FMA(KP1_662939224, To, Tl);
    }
}

// (normalized, scaled) Haar transform
template <bool forward>
__device__
static inline void haar(float v[8]) {
    if constexpr (forward) {
        float KP1_414213562 {+1.414213562373095048801688724209698078569671875};
        float KP2_000000000 {+2.000000000000000000000000000000000000000000000};

        auto T1 = v[0] + v[1]; // 11000000
        auto T2 = v[0] - v[1]; // 1-1000000
        auto T3 = v[2] + v[3]; // 00110000
        auto T4 = v[2] - v[3]; // 001-10000
        auto T5 = v[4] + v[5]; // 00001100
        auto T6 = v[4] - v[5]; // 00001-100
        auto T7 = v[6] + v[7]; // 00000011
        auto T8 = v[6] - v[7]; // 0000001-1

        auto T9 = T1 + T3; // 11110000
        auto T10 = KP1_414213562 * (T1 - T3); // 22-2-20000
        auto T11 = T5 + T7; // 00001111
        auto T12 = KP1_414213562 * (T5 - T7);

        auto s = KP1_414213562;
        v[0] = s * (T9 + T11);
        v[1] = s * (T9 - T11);
        v[2] = s * T10;
        v[3] = s * T12;
        v[4] = s * KP2_000000000 * T2;
        v[5] = s * KP2_000000000 * T4;
        v[6] = s * KP2_000000000 * T6;
        v[7] = s * KP2_000000000 * T8;
    } else {
        float KP1_414213562 {+1.414213562373095048801688724209698078569671875};
        float KP2_000000000 {+2.000000000000000000000000000000000000000000000};

        auto T1 = v[0] + v[1]; // 11000000
        auto T2 = v[0] - v[1]; // 1-1000000
        auto T3 = KP1_414213562 * v[2] + KP2_000000000 * v[4];
        auto T4 = KP1_414213562 * v[2] - KP2_000000000 * v[4];
        auto T5 = -KP1_414213562 * v[2] + KP2_000000000 * v[4];
        auto T6 = -KP1_414213562 * v[2] - KP2_000000000 * v[4];
        auto T7 = KP1_414213562 * v[2] + KP2_000000000 * v[4];
        auto T8 = KP1_414213562 * v[2] - KP2_000000000 * v[4];
        auto T9 = -KP1_414213562 * v[2] + KP2_000000000 * v[4];
        auto T10 = -KP1_414213562 * v[2] - KP2_000000000 * v[4];

        auto s = KP1_414213562;
        v[0] = s * (T1 + T3);
        v[1] = s * (T1 + T4);
        v[2] = s * (T1 + T5);
        v[3] = s * (T1 + T6);
        v[4] = s * (T2 + T7);
        v[5] = s * (T2 + T8);
        v[6] = s * (T2 + T9);
        v[7] = s * (T2 + T10);
    }
}

// (normalized, scaled) Walsh-Hadamard transform
template <bool forward>
__device__
static inline void wht(float v[8]) {
    float KP1_414213562 {+1.414213562373095048801688724209698078569671875};

    auto T1 = v[0] + v[1]; // 11000000
    auto T2 = v[0] - v[1]; // 1-1000000
    auto T3 = v[2] + v[3]; // 00110000
    auto T4 = v[2] - v[3]; // 001-10000
    auto T5 = v[4] + v[5]; // 00001100
    auto T6 = v[4] - v[5]; // 00001-100
    auto T7 = v[6] + v[7]; // 00000011
    auto T8 = v[6] - v[7]; // 0000001-1

    auto T9 = T1 + T3; // 11110000
    auto T10 = T1 - T3; // 11-1-10000
    auto T11 = T2 + T4; // 1-11-10000
    auto T12 = T2 - T4; // 1-1-110000
    auto T13 = T5 + T7; // 00001111
    auto T14 = T5 - T7; // 000011-1-1
    auto T15 = T6 + T8; // 00001-11-1
    auto T16 = T6 - T8; // 00001-1-11

    float s = KP1_414213562;
    v[0] = s * (T9 + T13);
    v[1] = s * (T9 - T13);
    v[2] = s * (T10 - T14);
    v[3] = s * (T10 + T14);
    v[4] = s * (T12 + T16);
    v[5] = s * (T12 - T16);
    v[6] = s * (T11 - T15);
    v[7] = s * (T11 + T15);
}

// (normalized, scaled) Bi-orthogonal spline transform (1.5)
template <bool forward>
__device__
static inline void bior1_5(float v[8]) {
    if constexpr (forward) {
        float KP1_414213562 {+1.414213562373095048801688724209698078569671875};
        float KP877670597 {+0.877670597010003062405456290501163901200537360};
        float KP1_797135031 {+1.797135031972863413496886690073811797696338403};
        float KP2_277437593 {+2.277437593371384746027611934034934695963515886};
        float KP1_609389232 {+1.609389232649111887192845766718020518480884560};
        float KP334024180 {+0.334024180361136429417383083658457088741315663};
        float KP2_828427124 {+2.828427124746190097603377448419396157139343751};

        auto T1 = v[0] + v[3];
        auto T2 = v[0] - v[3];
        auto T3 = v[1] + v[2];
        auto T4 = v[1] - v[2];
        auto T5 = v[4] + v[7];
        auto T6 = v[4] - v[7];
        auto T7 = v[5] + v[6];
        auto T8 = v[5] - v[6];
        auto T9 = v[0] - v[1];
        auto T10 = v[2] - v[3];
        auto T11 = v[4] - v[5];
        auto T12 = v[6] - v[7];

        v[0] = KP1_414213562 * (T1 + T5 + T3 + T7);
        v[1] = KP877670597 * (T1 - T5) + KP1_797135031 * (T3 - T7);
        v[2] = KP2_277437593 * T2 + KP1_609389232 * T4 + KP334024180 * (T8 - T6);
        v[3] = KP2_277437593 * T6 + KP1_609389232 * T8 + KP334024180 * (T4 - T2);
        v[4] = KP2_828427124 * T9;
        v[5] = KP2_828427124 * T10;
        v[6] = KP2_828427124 * T11;
        v[7] = KP2_828427124 * T12;
    } else {
        float KP1_414213562 {+1.414213562373095048801688724209698078569671875};
        float KP1_495435764 {+1.495435764250674860795011090214036706658653686};
        float KP2_058234225 {+2.058234225009388964222454285384072231477027482};
        float KP486135912 {+0.486135912065751423025580498947083714508324707};
        float KP2_828427124 {+2.828427124746190097603377448419396157139343751};

        auto T1 = KP1_414213562 * v[0];
        auto T2 = KP1_495435764 * v[1];
        auto T3 = KP2_058234225 * v[2];
        auto T4 = KP2_058234225 * v[3];
        auto T5 = KP2_828427124 * v[4];
        auto T6 = KP486135912 * v[4];
        auto T7 = KP2_828427124 * v[5];
        auto T8 = KP486135912 * v[5];
        auto T9 = KP2_828427124 * v[6];
        auto T10 = KP486135912 * v[6];
        auto T11 = KP2_828427124 * v[7];
        auto T12 = KP486135912 * v[7];

        auto T13 = T1 + T2;
        auto T14 = T1 - T2;
        auto T15 = T8 - T12;
        auto T16 = T6 - T10;

        v[0] = (T13 + T3) + (T5 - T15);
        v[1] = (T13 + T3) - (T5 + T15);
        v[2] = (T13 - T3) + (T16 + T7);
        v[3] = (T13 - T3) + (T16 - T7);
        v[4] = (T14 + T4) + (T15 + T9);
        v[5] = (T14 + T4) + (T15 - T9);
        v[6] = (T14 - T4) - (T16 - T11);
        v[7] = (T14 - T4) - (T16 + T11);
    }
}
)""" R"""(
// 2-D transposition
template <int stride=256, int howmany=8, int howmany_stride=32>
__device__
static inline void transpose_pack8_interleave4(
    float * __restrict__ data, float * __restrict__ buffer
) {

    int lane_id;
    asm("mov.u32 %0, %%laneid;" : "=r"(lane_id));

    #pragma unroll
    for (int iter = 0; iter < howmany; ++iter, data += howmany_stride) {
        #pragma unroll
        for (int i = 0; i < 8; ++i) {
            buffer[i * smem_stride + lane_id] = data[i * stride];
        }

        __syncwarp();

        #pragma unroll
        for (int i = 0; i < 8; ++i) {
            data[i * stride] = buffer[(lane_id % 8) * smem_stride + (lane_id & -8) + i];
        }
    }
}

// hard thresholding
// launched by blockDim(x=32, y=1, z=1)
template <int stride=32>
__device__
static inline float hard_thresholding(float * data, float sigma) {
    int lane_id;
    asm("mov.u32 %0, %%laneid;" : "=r"(lane_id));

    // number of non-zero coefficients
    #if __CUDA_ARCH__ == 750 || __CUDA_ARCH__ == 860
    float ks[4] {};
    #else
    float k {};
    #endif

    #pragma unroll
    for (int i = 0; i < 64; ++i) {
        auto val = data[i * stride];

        float thr;
        if (i == 0) {
            thr = (lane_id % 8) ? sigma * 2.0f : 0.0f; // protects DC component
        } else if (i < 8 || i % 8 == 0) {
            thr = (lane_id % 8) ? sigma * sqrtf(2.0f) : sigma * 2.0f;
        } else {
            thr = (lane_id % 8) ? sigma : sigma * sqrtf(2.0f);
        }

        float flag = fabsf(val) >= thr;

        #if __CUDA_ARCH__ == 750 || __CUDA_ARCH__ == 860
        ks[i % 4] += flag;
        #else
        k += flag;
        #endif
        data[i * stride] = flag ? (val * (1.0f / 4096.0f)) : 0.0f;
    }

    #if __CUDA_ARCH__ == 750 || __CUDA_ARCH__ == 860
    float k { (ks[0] + ks[1]) + (ks[2] + ks[3]) };
    #endif

    #pragma unroll
    for (int i = 4; i >= 1; i /= 2) {
        k += __shfl_xor_sync(0xFFFFFFFF, k, i, 8);
    }

    return 1.0f / k;
}

// hard thresholding
// launched by blockDim(x=32, y=1, z=1)
__device__
static inline float collaborative_hard(
    float * __restrict__ denoising_patch, float sigma, float * __restrict__ buffer
) {

    constexpr int stride1 = 1;
    constexpr int stride2 = stride1 * 8;
    
    for (int ndim = 0; ndim < 2; ++ndim) {
        transform_pack8_interleave4<transform_2d<true>, stride1, 8, stride2>(denoising_patch, buffer);

        transpose_pack8_interleave4<stride1, 8, stride2>(denoising_patch, buffer);
    }

    transform_pack8_interleave4<transform_1d<true>, stride2, 8, stride1>(denoising_patch, buffer);

    float adaptive_weight = hard_thresholding<stride1>(denoising_patch, sigma);

    for (int ndim = 0; ndim < 2; ++ndim) {
        transform_pack8_interleave4<transform_2d<false>, stride1, 8, stride2>(denoising_patch, buffer);

        transpose_pack8_interleave4<stride1, 8, stride2>(denoising_patch, buffer);
    }

    transform_pack8_interleave4<transform_1d<false>, stride2, 8, stride1>(denoising_patch, buffer);

    return adaptive_weight;
}

// soft thresholding, a.k.a. wiener filtering
// launched by blockDim(x=32, y=1, z=1)
template <int stride=32>
__device__
static inline float soft_thresholding(
    float * __restrict__ data, float * __restrict__ ref, float sigma
) {
    int lane_id;
    asm("mov.u32 %0, %%laneid;" : "=r"(lane_id));

    // squared l2-norm of coefficients
    #if __CUDA_ARCH__ == 750 || __CUDA_ARCH__ == 860
    float ks[4] {};
    #else
    float k {};
    #endif

    #pragma unroll
    for (int i = 0; i < 64; ++i) {
        auto val = data[i * stride];
        auto ref_val = ref[i * stride];
        float coeff = (ref_val * ref_val) / (ref_val * ref_val + sigma * sigma);
        if (i == 0) {
            coeff = (lane_id % 8) ? coeff : 1.f; // protects DC component
        }
        val *= coeff;
        #if __CUDA_ARCH__ == 750 || __CUDA_ARCH__ == 860
        ks[i % 4] += coeff * coeff;
        #else
        k += coeff * coeff;
        #endif
        data[i * stride] = val * (1.0f / 4096.f);
    }

    #if __CUDA_ARCH__ == 750 || __CUDA_ARCH__ == 860
    float k { (ks[0] + ks[1]) + (ks[2] + ks[3]) };
    #endif

    #pragma unroll
    for (int i = 4; i >= 1; i /= 2) {
        k += __shfl_xor_sync(0xFFFFFFFF, k, i, 8);
    }

    return 1.0f / k;
}

// soft thresholding, a.k.a. wiener filtering
// launched by blockDim(x=32, y=1, z=1)
__device__
static inline float collaborative_soft(
    float * __restrict__ denoising_patch, float * __restrict__ ref_patch, 
    float sigma, float * __restrict__ buffer
) {

    constexpr int stride1 = 1;
    constexpr int stride2 = stride1 * 8;
    
    for (int ndim = 0; ndim < 2; ++ndim) {
        transform_pack8_interleave4<transform_2d<true>, stride1, 8, stride2>(denoising_patch, buffer);
        transpose_pack8_interleave4<stride1, 8, stride2>(denoising_patch, buffer);
    }

    transform_pack8_interleave4<transform_1d<true>, stride2, 8, stride1>(denoising_patch, buffer);

    for (int ndim = 0; ndim < 2; ++ndim) {
        transform_pack8_interleave4<transform_2d<true>, stride1, 8, stride2>(ref_patch, buffer);
        transpose_pack8_interleave4<stride1, 8, stride2>(ref_patch, buffer);
    }
    transform_pack8_interleave4<transform_1d<true>, stride2, 8, stride1>(ref_patch, buffer);

    float adaptive_weight = soft_thresholding<stride1>(denoising_patch, ref_patch, sigma);

    for (int ndim = 0; ndim < 2; ++ndim) {
        transform_pack8_interleave4<transform_2d<false>, stride1, 8, stride2>(denoising_patch, buffer);
        transpose_pack8_interleave4<stride1, 8, stride2>(denoising_patch, buffer);
    }
    transform_pack8_interleave4<transform_1d<false>, stride2, 8, stride1>(denoising_patch, buffer);

    return adaptive_weight;
}

)""" R"""(
// BM3D kernel
extern "C"
__global__
#if __CUDA_ARCH__ == 750 || __CUDA_ARCH__ == 860
__launch_bounds__(32, 16)
#else
__launch_bounds__(32, 32)
#endif
void bm3d(
    /* shape: [(chroma ? 3 : 1), (2 * radius + 1), 2, height, stride] */
    float * __restrict__ res, 
    /* shape: [(final_ ? 2 : 1), (chroma ? 3 : 1), (2 * radius + 1), height, stride] */
    const float * __restrict__ src
) {

    __shared__ float buffer[8 * smem_stride];

    int lane_id;
    asm("mov.u32 %0, %%laneid;" : "=r"(lane_id));

    const int sub_lane_id = lane_id % 8; // 0 ~ 7
    int x = (4 * blockIdx.x + lane_id / 8) * block_step;
    int y = block_step * blockIdx.y;
    if (x >= width || y >= height) {
        return;
    }

    x = min(x, width - 8);
    y = min(y, height - 8);

    int radius = 0;
    if constexpr (temporal) {
        radius = _radius;
    }

    int temporal_stride = height * stride;
    int temporal_width = 2 * radius + 1;
    int plane_stride = temporal_width * temporal_stride;
    int clip_stride = (chroma ? 3 : 1) * temporal_width * temporal_stride;

    float current_patch[8];
    const float * const srcpc = &src[radius * temporal_stride + sub_lane_id];

    {
        const float * srcp = &srcpc[y * stride + x];

        #pragma unroll
        for (int i = 0; i < 8; ++i) {
            current_patch[i] = srcp[i * stride];
        }
    }

    #if __CUDA_ARCH__ >= 700
    int membermask = 
        ((x & -32) >= bm_range && (x & -32) + bm_range <= width - 32) 
        ? 0xFFFFFFFF 
        : 0xFF << (lane_id & -8);
    #endif

    float errors8 = FLT_MAX;
    int index8_x = 0;
    int index8_y = 0;

    {
        int left = max(x - bm_range, 0);
        int right = min(x + bm_range, width - 8);
        int top = max(y - bm_range, 0);
        int bottom = min(y + bm_range, height - 8);

        const float * srcp_row = srcpc + (top * stride + left);
        for (int row_i = top; row_i <= bottom; ++row_i) {
            const float * srcp_col = srcp_row;
            for (int col_i = left; col_i <= right; ++col_i) {
                float errors[2] { 0.0f };

                const float * srcp = srcp_col;

                #pragma unroll
                for (int i = 0; i < 8; ++i) {
                    float val = current_patch[i] - srcp[i * stride];
                    errors[i % 2] += val * val;
                }

                float error = errors[0] + errors[1];

                #if __CUDA_ARCH__ >= 700
                auto active_mask = membermask;
                #else
                auto active_mask = __activemask(); // lock-step execution
                #endif

                error += __shfl_xor_sync(active_mask, error, 1, 8);
                error += __shfl_xor_sync(active_mask, error, 2, 8);
                error += __shfl_xor_sync(active_mask, error, 4, 8);

                auto pre_error = __shfl_up_sync(active_mask, errors8, 1, 8);
                int pre_index_x = __shfl_up_sync(active_mask, index8_x, 1, 8);
                int pre_index_y = __shfl_up_sync(active_mask, index8_y, 1, 8);

                int flag = error < errors8;
                int pre_flag = __shfl_up_sync(active_mask, flag, 1, 8);

                if (flag) {
                    int first = (sub_lane_id == 0) || (!pre_flag);
                    errors8 = first ? error : pre_error;
                    index8_x = first ? col_i : pre_index_x;
                    index8_y = first ? row_i : pre_index_y;
                }

                ++srcp_col;
            }

            srcp_row += stride;
        }
    }
    [[maybe_unused]] int index8_z = radius;

    if /* constexpr */ (temporal) {
        #if __CUDA_ARCH__ >= 700
        membermask = 0xFF << (lane_id & -8); // Only sub-warp convergence is guaranteed
        #endif

        int center_index8_x = index8_x;
        int center_index8_y = index8_y;

        #pragma unroll
        for (int direction = -1; direction <= 1; direction += 2) {
            int last_index8_x = center_index8_x;
            int last_index8_y = center_index8_y;

            for (int t = 1; t <= radius; ++t) {
                /*
                membermask = 
                    (((x & -32) >= bm_range + t * ps_range) && 
                     ((x & -32) + bm_range + t * ps_range <= width - 32) && 
                     (y >= bm_range + t * ps_range) && 
                     (y + bm_range + t * ps_range <= height - 8)) 
                    ? 0xFFFFFFFF 
                    : 0xFF << (lane_id & -8);
                */
                int temporal_index = radius + direction * t;
                float frame_errors8 = FLT_MAX;
                int frame_index8_x = 0;
                int frame_index8_y = 0;

                const float * temporal_srcpc = &src[temporal_index * temporal_stride + sub_lane_id];

                for (int i = 0; i < ps_num; ++i) {
                    int xx = __shfl_sync(0xFFFFFFFF, last_index8_x, i, 8);
                    int yy = __shfl_sync(0xFFFFFFFF, last_index8_y, i, 8);

                    int left = max(xx - ps_range, 0);
                    int right = min(xx + ps_range, width - 8);
                    int top = max(yy - ps_range, 0);
                    int bottom = min(yy + ps_range, height - 8);

                    const float * srcp_row = &temporal_srcpc[top * stride + left];
                    for (int row_i = top; row_i <= bottom; ++row_i) {
                        const float * srcp_col = srcp_row;
                        for (int col_i = left; col_i <= right; ++col_i) {
                            float errors[2] {0.f};

                            const float * srcp = srcp_col;

                            #pragma unroll
                            for (int j = 0; j < 8; ++j) {
                                float val = current_patch[j] - srcp[j * stride];
                                errors[j % 2] += val * val;
                            }

                            float error = errors[0] + errors[1];

                            #if __CUDA_ARCH__ >= 700
                            auto active_mask = membermask;
                            #else
                            auto active_mask = __activemask(); // lock-step execution
                            #endif

                            error += __shfl_xor_sync(active_mask, error, 1, 8);
                            error += __shfl_xor_sync(active_mask, error, 2, 8);
                            error += __shfl_xor_sync(active_mask, error, 4, 8);

                            float pre_error = __shfl_up_sync(
                                active_mask, frame_errors8, 1, 8);
                            int pre_index_x = __shfl_up_sync(
                                active_mask, frame_index8_x, 1, 8);
                            int pre_index_y = __shfl_up_sync(
                                active_mask, frame_index8_y, 1, 8);

                            int flag = error < frame_errors8;
                            int pre_flag = __shfl_up_sync(active_mask, flag, 1, 8);

                            if (flag) {
                                int first = (sub_lane_id == 0) || (!pre_flag);
                                frame_errors8 = first ? error : pre_error;
                                frame_index8_x = first ? col_i : pre_index_x;
                                frame_index8_y = first ? row_i : pre_index_y;
                            }

                            ++srcp_col;
                        }

                        srcp_row += stride;
                    }
                }

                for (int i = 0; i < ps_num; ++i) {
                    float tmp_error = __shfl_sync(0xFFFFFFFF, frame_errors8, i, 8);
                    int tmp_x = __shfl_sync(0xFFFFFFFF, frame_index8_x, i, 8);
                    int tmp_y = __shfl_sync(0xFFFFFFFF, frame_index8_y, i, 8);

                    int flag = tmp_error < errors8;
                    int pre_flag = __shfl_up_sync(0xFFFFFFFF, flag, 1, 8);
                    float pre_error = __shfl_up_sync(0xFFFFFFFF, errors8, 1, 8);
                    int pre_index_x = __shfl_up_sync(0xFFFFFFFF, index8_x, 1, 8);
                    int pre_index_y = __shfl_up_sync(0xFFFFFFFF, index8_y, 1, 8);
                    int pre_index_z = __shfl_up_sync(0xFFFFFFFF, index8_z, 1, 8);

                    if (flag) {
                        int first = (sub_lane_id == 0) || (!pre_flag);
                        errors8 = first ? tmp_error : pre_error;
                        index8_x = first ? tmp_x : pre_index_x;
                        index8_y = first ? tmp_y : pre_index_y;
                        index8_z = first ? temporal_index : pre_index_z;
                    }
                }

                last_index8_x = frame_index8_x;
                last_index8_y = frame_index8_y;
            }
        }
    }

    // insert center block
    {
        auto active_mask = 0xFFFFFFFF;

        int flag;
        if constexpr (temporal) {
            flag = index8_x == x && index8_y == y && index8_z == radius;
        } else {
            flag = index8_x == x && index8_y == y;
        }

        flag += __shfl_xor_sync(active_mask, flag, 1, 8);
        flag += __shfl_xor_sync(active_mask, flag, 2, 8);
        flag += __shfl_xor_sync(active_mask, flag, 4, 8);

        float pre_error = __shfl_up_sync(active_mask, errors8, 1, 8);
        int pre_index_x = __shfl_up_sync(active_mask, index8_x, 1, 8);
        int pre_index_y = __shfl_up_sync(active_mask, index8_y, 1, 8);
        [[maybe_unused]] int pre_index_z;
        if constexpr (temporal) {
            pre_index_z = __shfl_up_sync(active_mask, index8_z, 1, 8);
        }
        if (!flag) {
            int first = (sub_lane_id == 0);
            errors8 = first ? 0.0f : pre_error;
            index8_x = first ? x : pre_index_x;
            index8_y = first ? y : pre_index_y;
            if constexpr (temporal) {
                index8_z = first ? radius : pre_index_z;
            }
        }
    }

    float denoising_patch[64];
    [[maybe_unused]] float ref_patch[64];

    int num_planes = 1;
    if constexpr (chroma) {
        num_planes = 3;
    }
    #pragma unroll
    for (int plane = 0; plane < num_planes; ++plane) {
        float sigma;
        if (plane == 0) {
            sigma = sigma_y;
        } else if (plane == 1) {
            sigma = sigma_u;
        } else if (plane == 2) {
            sigma = sigma_v;
        }

        if constexpr (chroma) {
            if (sigma < FLT_EPSILON) {
                src += plane_stride;
                res += plane_stride * 2;
                continue;
            }
        }

        float adaptive_weight;
        if /* constexpr */ (final_) {
            #pragma unroll
            for (int i = 0; i < 8; ++i) {
                int tmp_x = __shfl_sync(0xFFFFFFFF, index8_x, i, 8);
                int tmp_y = __shfl_sync(0xFFFFFFFF, index8_y, i, 8);
                const float * refp;
                if constexpr (temporal) {
                    int tmp_z = __shfl_sync(0xFFFFFFFF, index8_z, i, 8);
                    refp = &src[tmp_z * temporal_stride + tmp_y * stride + tmp_x + sub_lane_id];
                } else {
                    refp = &src[tmp_y * stride + tmp_x + sub_lane_id];
                }
                const float * srcp = &refp[clip_stride];

                #pragma unroll
                for (int j = 0; j < 8; ++j) {
                    ref_patch[i * 8 + j] = refp[j * stride];
                    denoising_patch[i * 8 + j] = srcp[j * stride];
                }
            }

            adaptive_weight = collaborative_soft(denoising_patch, ref_patch, sigma, buffer);
        } else {
            #pragma unroll
            for (int i = 0; i < 8; ++i) {
                int tmp_x = __shfl_sync(0xFFFFFFFF, index8_x, i, 8);
                int tmp_y = __shfl_sync(0xFFFFFFFF, index8_y, i, 8);
                const float * srcp;
                if constexpr (temporal) {
                    int tmp_z = __shfl_sync(0xFFFFFFFF, index8_z, i, 8);
                    srcp = &src[tmp_z * temporal_stride + tmp_y * stride + tmp_x + sub_lane_id];
                } else {
                    srcp = &src[tmp_y * stride + tmp_x + sub_lane_id];
                }

                #pragma unroll
                for (int j = 0; j < 8; ++j) {
                    denoising_patch[i * 8 + j] = srcp[j * stride];
                }
            }

            adaptive_weight = collaborative_hard(denoising_patch, sigma, buffer);
        }

        float * const wdstpc = &res[sub_lane_id];
        float * const weightpc = &res[temporal_stride + sub_lane_id];

        #pragma unroll
        for (int i = 0; i < 8; ++i) {
            int tmp_x = __shfl_sync(0xFFFFFFFF, index8_x, i, 8);
            int tmp_y = __shfl_sync(0xFFFFFFFF, index8_y, i, 8);
            int offset;
            if constexpr (temporal) {
                int tmp_z = __shfl_sync(0xFFFFFFFF, index8_z, i, 8);
                offset = tmp_z * 2 * temporal_stride + tmp_y * stride + tmp_x;
            } else {
                offset = tmp_y * stride + tmp_x;
            }

            float * wdstp = &wdstpc[offset];
            float * weightp = &weightpc[offset];

            #pragma unroll
            for (int j = 0; j < 8; ++j) {
                atomicAdd(&wdstp[j * stride], adaptive_weight * denoising_patch[i * 8 + j]);
                atomicAdd(&weightp[j * stride], adaptive_weight);
            }
        }

        src += plane_stride;
        res += plane_stride * 2;
    }
}
)""";
