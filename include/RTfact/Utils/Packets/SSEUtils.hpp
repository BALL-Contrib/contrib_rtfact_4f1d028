/*
 *  SSEUtils.hpp
 *
 *  RTfact - Real-Time Ray Tracing Library
 *
 *  Copyright (C) 2009  Saarland University
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Created on: 2008-11-02 21:38:37 +0100
 *  Author(s): Iliyan Georgiev
 */

 #ifndef RTFACT__SSEVEC4_HPP
#define RTFACT__SSEVEC4_HPP

#include <RTfact/Config/Common.hpp>

#include <memory.h>
#include <xmmintrin.h>
#include <emmintrin.h>

#ifdef RTFACT_HAVE_SSE41
#   include <smmintrin.h>
#endif

#include <RTfact/Utils/Math/MathUtils.hpp>

RTFACT_NAMESPACE_BEGIN

#define RTFACT_SSE_ASSERT(x)        RTFACT_ASSERT(x)
#define RTFACT_SSE_STATIC_ASSERT(x) RTFACT_STATIC_ASSERT(x)

#define RTFACT_SIMD_MASK_TRUE  15
#define RTFACT_SIMD_MASK_FALSE 0

#pragma region Constant declaration and initialization macros

#define RTFACT__SSE_DECLARE_CONST(aName)                                       \
    static const t_This aName;

#define RTFACT_SSE_INIT_CONST(aType, aName, aX, aY, aZ, aW)                    \
    const aType aType::aName = aType::getInstance(aX, aY, aZ, aW);

#define RTFACT_INT4_INIT_CONST(aName, aX, aY, aZ, aW)                          \
    RTFACT_SSE_INIT_CONST(int4, aName, aX, aY, aZ, aW)

#define RTFACT_FLOAT4_INIT_CONST(aName, aX, aY, aZ, aW)                        \
    RTFACT_SSE_INIT_CONST(float4, aName, aX, aY, aZ, aW)

#define RTFACT_SSE_INIT_CONSTS                                                 \
                                                                               \
    namespace Detail {                                                         \
        const uint _mi = 2147483647; /* = 0111 1111... */                      \
        const float _mf = *((float *) &_mi);                                   \
    }                                                                          \
                                                                               \
    RTFACT_INT4_INIT_CONSTS                                                    \
    RTFACT_FLOAT4_INIT_CONSTS                                                  \
                                                                               \
    namespace Detail {                                                         \
        const RTFACT_ALIGN_SIMD int4 int4Masks[16] =                           \
        {                                                                      \
            int4::C_0_0_0_0(), int4::C_T_0_0_0,                                \
            int4::C_0_T_0_0,   int4::C_T_T_0_0,                                \
            int4::C_0_0_T_0,   int4::C_T_0_T_0,                                \
            int4::C_0_T_T_0,   int4::C_T_T_T_0,                                \
            int4::C_0_0_0_T,   int4::C_T_0_0_T,                                \
            int4::C_0_T_0_T,   int4::C_T_T_0_T,                                \
            int4::C_0_0_T_T,   int4::C_T_0_T_T,                                \
            int4::C_0_T_T_T,   int4::C_T_T_T_T                                 \
        };                                                                     \
    }                                                                          \

#define RTFACT__INT4_DECLARE_CONSTS                                            \
                                                                               \
    RTFACT__SSE_DECLARE_CONST(C_0_0_0_T)                                       \
    RTFACT__SSE_DECLARE_CONST(C_0_0_T_0)                                       \
    RTFACT__SSE_DECLARE_CONST(C_0_0_T_T)                                       \
    RTFACT__SSE_DECLARE_CONST(C_0_T_0_0)                                       \
    RTFACT__SSE_DECLARE_CONST(C_0_T_0_T)                                       \
    RTFACT__SSE_DECLARE_CONST(C_0_T_T_0)                                       \
    RTFACT__SSE_DECLARE_CONST(C_0_T_T_T)                                       \
    RTFACT__SSE_DECLARE_CONST(C_T_0_0_0)                                       \
    RTFACT__SSE_DECLARE_CONST(C_T_0_0_T)                                       \
    RTFACT__SSE_DECLARE_CONST(C_T_0_T_0)                                       \
    RTFACT__SSE_DECLARE_CONST(C_T_0_T_T)                                       \
    RTFACT__SSE_DECLARE_CONST(C_T_T_0_0)                                       \
    RTFACT__SSE_DECLARE_CONST(C_T_T_0_T)                                       \
    RTFACT__SSE_DECLARE_CONST(C_T_T_T_0)                                       \
    RTFACT__SSE_DECLARE_CONST(C_T_T_T_T)                                       \
                                                                               \
    RTFACT__SSE_DECLARE_CONST(C_1_1_1_0)                                       \
    RTFACT__SSE_DECLARE_CONST(C_1_1_1_1)                                       \
    RTFACT__SSE_DECLARE_CONST(C_2_2_2_2)                                       \
    RTFACT__SSE_DECLARE_CONST(C_3_3_3_3)                                       \

#define RTFACT_INT4_INIT_CONSTS                                                \
                                                                               \
    RTFACT_INT4_INIT_CONST(C_0_0_0_T,  0,  0,  0, -1)                          \
    RTFACT_INT4_INIT_CONST(C_0_0_T_0,  0,  0, -1,  0)                          \
    RTFACT_INT4_INIT_CONST(C_0_0_T_T,  0,  0, -1, -1)                          \
    RTFACT_INT4_INIT_CONST(C_0_T_0_0,  0, -1,  0,  0)                          \
    RTFACT_INT4_INIT_CONST(C_0_T_0_T,  0, -1,  0, -1)                          \
    RTFACT_INT4_INIT_CONST(C_0_T_T_0,  0, -1, -1,  0)                          \
    RTFACT_INT4_INIT_CONST(C_0_T_T_T,  0, -1, -1, -1)                          \
    RTFACT_INT4_INIT_CONST(C_T_0_0_0, -1,  0,  0,  0)                          \
    RTFACT_INT4_INIT_CONST(C_T_0_0_T, -1,  0,  0, -1)                          \
    RTFACT_INT4_INIT_CONST(C_T_0_T_0, -1,  0, -1,  0)                          \
    RTFACT_INT4_INIT_CONST(C_T_0_T_T, -1,  0, -1, -1)                          \
    RTFACT_INT4_INIT_CONST(C_T_T_0_0, -1, -1,  0,  0)                          \
    RTFACT_INT4_INIT_CONST(C_T_T_0_T, -1, -1,  0, -1)                          \
    RTFACT_INT4_INIT_CONST(C_T_T_T_0, -1, -1, -1,  0)                          \
    RTFACT_INT4_INIT_CONST(C_T_T_T_T, -1, -1, -1, -1)                          \
                                                                               \
    RTFACT_INT4_INIT_CONST(C_1_1_1_0,  1,  1,  1,  0)                          \
    RTFACT_INT4_INIT_CONST(C_1_1_1_1,  1,  1,  1,  1)                          \
    RTFACT_INT4_INIT_CONST(C_2_2_2_2,  2,  2,  2,  2)                          \
    RTFACT_INT4_INIT_CONST(C_3_3_3_3,  3,  3,  3,  3)                          \

#define RTFACT__FLOAT4_DECLARE_CONSTS                                          \
                                                                               \
    RTFACT__SSE_DECLARE_CONST(C_1_0_0_0)                                       \
    RTFACT__SSE_DECLARE_CONST(C_0_1_0_0)                                       \
    RTFACT__SSE_DECLARE_CONST(C_0_0_1_0)                                       \
    RTFACT__SSE_DECLARE_CONST(C_0_0_0_1)                                       \
    RTFACT__SSE_DECLARE_CONST(C_0_1_0_1)                                       \
    RTFACT__SSE_DECLARE_CONST(C_1_1_1_0)                                       \
    RTFACT__SSE_DECLARE_CONST(C_1_1_1_1)                                       \
    RTFACT__SSE_DECLARE_CONST(C_2_2_2_2)                                       \
    RTFACT__SSE_DECLARE_CONST(C_3_3_3_3)                                       \
    RTFACT__SSE_DECLARE_CONST(C_4_4_4_4)                                       \
    RTFACT__SSE_DECLARE_CONST(C_0_1_2_3)                                       \
    RTFACT__SSE_DECLARE_CONST(C_0_1_1_0)                                       \
    RTFACT__SSE_DECLARE_CONST(C_0_0_1_1)                                       \
    RTFACT__SSE_DECLARE_CONST(C_0_2_2_0)                                       \
    RTFACT__SSE_DECLARE_CONST(C_0_0_2_2)                                       \
    RTFACT__SSE_DECLARE_CONST(C_0_4_4_0)                                       \
    RTFACT__SSE_DECLARE_CONST(C_0_0_4_4)                                       \
    RTFACT__SSE_DECLARE_CONST(C__0_5__0_5__0_5__0_5)                           \
    RTFACT__SSE_DECLARE_CONST(C__1_5__1_5__1_5__1_5)                           \
    RTFACT__SSE_DECLARE_CONST(C__2_5__2_5__2_5__2_5)                           \
    RTFACT__SSE_DECLARE_CONST(C__3_5__3_5__3_5__3_5)                           \
    RTFACT__SSE_DECLARE_CONST(C__0_5__1_5__2_5__3_5)                           \
    RTFACT__SSE_DECLARE_CONST(C__0_5__1_5__0_5__1_5)                           \
    RTFACT__SSE_DECLARE_CONST(C__0_5__0_5__1_5__1_5)                           \
                                                                               \
    RTFACT__SSE_DECLARE_CONST(C_ABS_MASK)                                      \
    RTFACT__SSE_DECLARE_CONST(C_EPS_EPS_EPS_EPS)                               \
    RTFACT__SSE_DECLARE_CONST(C_EPS_EPS_EPS_0)                                 \
    RTFACT__SSE_DECLARE_CONST(C_MAX_MAX_MAX_MAX)                               \
    RTFACT__SSE_DECLARE_CONST(C_MIN_MIN_MIN_MIN)                               \

#define RTFACT_FLOAT4_INIT_CONSTS                                              \
                                                                               \
    RTFACT_FLOAT4_INIT_CONST(C_1_0_0_0, 1.f, 0.f, 0.f, 0.f)                    \
    RTFACT_FLOAT4_INIT_CONST(C_0_1_0_0, 0.f, 1.f, 0.f, 0.f)                    \
    RTFACT_FLOAT4_INIT_CONST(C_0_0_1_0, 0.f, 0.f, 1.f, 0.f)                    \
    RTFACT_FLOAT4_INIT_CONST(C_0_0_0_1, 0.f, 0.f, 0.f, 1.f)                    \
    RTFACT_FLOAT4_INIT_CONST(C_0_1_0_1, 0.f, 1.f, 0.f, 1.f)                    \
    RTFACT_FLOAT4_INIT_CONST(C_1_1_1_0, 1.f, 1.f, 1.f, 0.f)                    \
    RTFACT_FLOAT4_INIT_CONST(C_1_1_1_1, 1.f, 1.f, 1.f, 1.f)                    \
    RTFACT_FLOAT4_INIT_CONST(C_2_2_2_2, 2.f, 2.f, 2.f, 2.f)                    \
    RTFACT_FLOAT4_INIT_CONST(C_3_3_3_3, 3.f, 3.f, 3.f, 3.f)                    \
    RTFACT_FLOAT4_INIT_CONST(C_4_4_4_4, 4.f, 4.f, 4.f, 4.f)                    \
    RTFACT_FLOAT4_INIT_CONST(C_0_1_2_3, 0.f, 1.f, 2.f, 3.f)                    \
    RTFACT_FLOAT4_INIT_CONST(C_0_1_1_0, 0.f, 1.f, 1.f, 0.f)                    \
    RTFACT_FLOAT4_INIT_CONST(C_0_0_1_1, 0.f, 0.f, 1.f, 1.f)                    \
    RTFACT_FLOAT4_INIT_CONST(C_0_2_2_0, 0.f, 2.f, 2.f, 0.f)                    \
    RTFACT_FLOAT4_INIT_CONST(C_0_0_2_2, 0.f, 0.f, 2.f, 2.f)                    \
    RTFACT_FLOAT4_INIT_CONST(C_0_4_4_0, 0.f, 4.f, 4.f, 0.f)                    \
    RTFACT_FLOAT4_INIT_CONST(C_0_0_4_4, 0.f, 0.f, 4.f, 4.f)                    \
    RTFACT_FLOAT4_INIT_CONST(C__0_5__0_5__0_5__0_5, 0.5f, 0.5f, 0.5f, 0.5f)    \
    RTFACT_FLOAT4_INIT_CONST(C__1_5__1_5__1_5__1_5, 1.5f, 1.5f, 1.5f, 1.5f)    \
    RTFACT_FLOAT4_INIT_CONST(C__2_5__2_5__2_5__2_5, 2.5f, 2.5f, 2.5f, 2.5f)    \
    RTFACT_FLOAT4_INIT_CONST(C__3_5__3_5__3_5__3_5, 3.5f, 3.5f, 3.5f, 3.5f)    \
    RTFACT_FLOAT4_INIT_CONST(C__0_5__1_5__2_5__3_5, 0.5f, 1.5f, 2.5f, 3.5f)    \
    RTFACT_FLOAT4_INIT_CONST(C__0_5__1_5__0_5__1_5, 0.5f, 1.5f, 0.5f, 1.5f)    \
    RTFACT_FLOAT4_INIT_CONST(C__0_5__0_5__1_5__1_5, 0.5f, 0.5f, 1.5f, 1.5f)    \
                                                                               \
    RTFACT_FLOAT4_INIT_CONST(C_ABS_MASK, Detail::_mf, Detail::_mf, Detail::_mf, Detail::_mf)\
    RTFACT_FLOAT4_INIT_CONST(C_EPS_EPS_EPS_EPS, RTFACT_EPSILON, RTFACT_EPSILON, RTFACT_EPSILON, RTFACT_EPSILON)\
    RTFACT_FLOAT4_INIT_CONST(C_EPS_EPS_EPS_0,   RTFACT_EPSILON, RTFACT_EPSILON, RTFACT_EPSILON, 0.f)\
    RTFACT_FLOAT4_INIT_CONST(C_MAX_MAX_MAX_MAX, RTFACT_FLOAT_MAX, RTFACT_FLOAT_MAX, RTFACT_FLOAT_MAX, RTFACT_FLOAT_MAX)\
    RTFACT_FLOAT4_INIT_CONST(C_MIN_MIN_MIN_MIN, RTFACT_FLOAT_MIN, RTFACT_FLOAT_MIN, RTFACT_FLOAT_MIN, RTFACT_FLOAT_MIN)\

#define RTFACT_SSE_0f _mm_setzero_ps()
#define RTFACT_SSE_0  _mm_setzero_si128()

#pragma endregion // Constant declaration and initialization macros

#pragma region Utility functions definitions

#define RTFACT__DAZ_BIT                  6
#define RTFACT__FTZ_BIT                  15
#define RTFACT__DENORMAL_EXCEPTION_MASK  8
#define RTFACT__UNDERFLOW_EXCEPTION_MASK 11

namespace Detail {

RTFACT_INLINE void set_mxcsr_on(int bit_num)
{
    #if defined(RTFACT_OS_WIN) && !defined(RTFACT_ARCH_64_BIT)

        __m128 state[32];
        __int32 x;
        __asm fxsave   state
        memcpy( (void*)&x, (char*)state+24, 4);
        x |= (1 << bit_num);
        __asm ldmxcsr   x

    #else

        #pragma message("Implement a FTZ/DAZ code for non-Windows systems")

    #endif
}

RTFACT_INLINE bool enableFastSSEDenormals()
{
    Detail::set_mxcsr_on(RTFACT__DAZ_BIT);
    Detail::set_mxcsr_on(RTFACT__DENORMAL_EXCEPTION_MASK);
    Detail::set_mxcsr_on(RTFACT__FTZ_BIT);
    Detail::set_mxcsr_on(RTFACT__UNDERFLOW_EXCEPTION_MASK);

    return true;
}

} // namespace Detail

#if !defined(RTFACT_COMPILER_INTEL) &&                                         \
    defined(RTFACT_COMPILER_MSVC) && (RTFACT_COMPILER_MSVC_VERSION < 1500)

    namespace Detail
    {
        union __m128ConvStruct
        {
            __m128 fdt;
            __m128i idt;
        };
    }

    RTFACT_FORCE_INLINE __m128 _mm_castsi128_ps(__m128i aData)
    {
        Detail::__m128ConvStruct t;
        t.idt = aData;
        return t.fdt;

        //return *(__m128 *) &aData;
    }

    RTFACT_FORCE_INLINE __m128i _mm_castps_si128(__m128 aData)
    {
        Detail::__m128ConvStruct t;
        t.fdt = aData;
        return t.idt;

        //return *(__m128i*) &aData;
    }

#endif

#pragma endregion // Utility functions definitions

#pragma region Operator definition macros

#define RTFACT__VEC4_BINOP_ARITH(aOP, aFunc)                                   \
    RTFACT_SSE_INLINE const t_This operator aOP(                               \
        const t_This& aVec) const                                              \
    {                                                                          \
        return getInstance(aFunc(data, aVec.data));                            \
    }                                                                          \
                                                                               \
    RTFACT_SSE_INLINE t_This& operator aOP##=(                                 \
        const t_This& aVec)                                                    \
    {                                                                          \
        data = aFunc(data, aVec.data);                                         \
        return *this;                                                          \
    }

#define RTFACT__INT4_BINOP_FLOAT_ARITH(aOP, aFunc)                             \
    RTFACT_SSE_INLINE const t_This operator aOP(                               \
        const t_This& aVec) const                                              \
    {                                                                          \
        return getInstance(_mm_cvtps_epi32(aFunc(_mm_cvtepi32_ps(data),        \
                                                 _mm_cvtepi32_ps(aVec.data))));\
    }                                                                          \
                                                                               \
    RTFACT_SSE_INLINE t_This& operator aOP##=(                                 \
        const t_This& aVec)                                                    \
    {                                                                          \
        data = _mm_cvtps_epi32(aFunc(_mm_cvtepi32_ps(data),                    \
                                     _mm_cvtepi32_ps(aVec.data)));             \
        return *this;                                                          \
    }

#define RTFACT__VEC4_UNOP(aOP, aFunc, aConstant)                               \
    RTFACT_SSE_INLINE const t_This operator aOP() const                        \
    {                                                                          \
        return getInstance(aFunc(aConstant, data));                            \
    }

#define RTFACT__INT4_BINOP_BIT_FLOAT4__DECLARE(aOP)                            \
    const float4 operator aOP(                                                 \
        const float4& aVec) const;

#define RTFACT__INT4_BINOP_BIT_FLOAT4__DEFINE(aOP, aFunc)                      \
    RTFACT_SSE_INLINE const float4 int4::operator aOP(                         \
        const float4& aVec) const                                              \
    {                                                                          \
        return float4::getInstance(aFunc(_mm_castsi128_ps(data), aVec.data));  \
    }

#define RTFACT_FLOAT4_BINOP_BIT_INT4(aOP, aFunc)                               \
    RTFACT_SSE_INLINE const float4 operator aOP(                               \
        const int4& aVec) const                                                \
    {                                                                          \
        return getInstance(aFunc(data, _mm_castsi128_ps(aVec.data)));          \
    }                                                                          \
                                                                               \
    RTFACT_SSE_INLINE float4& operator aOP##=(                                 \
        const int4& aVec)                                                      \
    {                                                                          \
        data = aFunc(data, _mm_castsi128_ps(aVec.data));                       \
        return *this;                                                          \
    }                                                                          \


#define RTFACT__FLOAT4_BINOP_CMP(aOP, aFunc)                                   \
    RTFACT_SSE_INLINE const int4 operator aOP(                                 \
        const float4& aVec) const                                              \
    {                                                                          \
        return int4::getInstance(_mm_castps_si128(aFunc(data, aVec.data)));    \
    }

#define RTFACT__INT4_BINOP_CMP(aOP, aFunc)                                     \
    RTFACT_SSE_INLINE const int4 operator aOP(                                 \
        const int4& aVec) const                                                \
    {                                                                          \
        return getInstance(aFunc(data, aVec.data));                            \
    }

#define RTFACT__INT4_BINOP_CMP_EQ(aOP, aFunc)                                  \
    RTFACT_SSE_INLINE const int4 operator aOP(                                 \
        const int4& aVec) const                                                \
    {                                                                          \
        return getInstance(_mm_xor_si128(C_T_T_T_T.data,                       \
                                         aFunc(data, aVec.data)));             \
    }

#pragma endregion // Operator definition macros

#pragma region Constructor, data, and SSE specific operations definition macros

#define RTFACT__VEC4_GEN_DATA_AND_CONSTRUCTORS(                                \
    aClassName, aSingleSetFunc, aSet4Func, aArrLoadFunc)                       \
    union                                                                      \
    {                                                                          \
        Data data;                                                             \
        /*struct { t_Scalar x, y, z, w; };*/                                   \
    };                                                                         \
                                                                               \
    static RTFACT_SSE_INLINE const t_This getInstance(                         \
        const t_Scalar aX,                                                     \
        const t_Scalar aY,                                                     \
        const t_Scalar aZ,                                                     \
        const t_Scalar aW)                                                     \
    {                                                                          \
        t_This ret;                                                            \
        ret.data = aSet4Func(aW, aZ, aY, aX);                                  \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    static RTFACT_SSE_INLINE const t_This getInstance(                         \
        const t_This& aData,                                                   \
        const t_Scalar aW)                                                     \
    {                                                                          \
        t_This ret;                                                            \
        ret.data = aSet4Func(aW, aData[2], aData[1], aData[0]);                \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    static RTFACT_SSE_INLINE const t_This getInstance(                         \
        const Data &aData)                                                     \
    {                                                                          \
        t_This ret;                                                            \
        ret.data = aData;                                                      \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    static RTFACT_SSE_INLINE const t_This getInstance(                         \
        const t_Scalar aValue)                                                 \
    {                                                                          \
        t_This ret;                                                            \
        ret.data = aSingleSetFunc(aValue);                                     \
        return ret;                                                            \
    }

#define RTFACT__VEC4_GEN_SHUFFLE_AND_ACCESSORS_BASE                            \
    RTFACT_SSE_INLINE const t_Scalar& operator[](                              \
        const uint aIndex) const                                               \
    {                                                                          \
        RTFACT_SSE_ASSERT(aIndex < 4);                                         \
        return *(((const t_Scalar*) &data) + aIndex);                          \
    }                                                                          \
                                                                               \
    RTFACT_SSE_INLINE t_Scalar& operator[](                                    \
        const uint aIndex)                                                     \
    {                                                                          \
        RTFACT_SSE_ASSERT(aIndex < 4);                                         \
        return *(((t_Scalar*) &data) + aIndex);                                \
    }                                                                          \
                                                                               \
    template<                                                                  \
        uint taComp>                                                           \
    RTFACT_SSE_INLINE const t_Scalar at() const                                \
    {                                                                          \
        RTFACT_SSE_STATIC_ASSERT(taComp < 4);                                  \
        return *(((const t_Scalar*) &data) + taComp);                          \
    }                                                                          \
                                                                               \
    template<                                                                  \
        uint taComp>                                                           \
    RTFACT_SSE_INLINE t_Scalar& at()                                           \
    {                                                                          \
        RTFACT_SSE_STATIC_ASSERT(taComp < 4);                                  \
        return *(((t_Scalar*) &data) + taComp);                                \
    }

#pragma endregion // Constructor, data, and SSE specific operations definition macros

#pragma region int4 class definition

class float4;

class RTFACT_ALIGN_SIMD int4
{
    typedef int4    t_This;
    typedef int     t_Scalar;

public:

    typedef __m128i Data;

    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

    RTFACT__VEC4_GEN_DATA_AND_CONSTRUCTORS(
        int4, _mm_cvtsi32_si128, _mm_set_epi32, _mm_load_si128)

    #pragma region Operators

    RTFACT__VEC4_BINOP_ARITH(+, _mm_add_epi32)
    RTFACT__VEC4_BINOP_ARITH(-, _mm_sub_epi32)
    RTFACT__VEC4_BINOP_ARITH(&, _mm_and_si128)
    RTFACT__VEC4_BINOP_ARITH(|, _mm_or_si128)
    RTFACT__VEC4_BINOP_ARITH(^, _mm_xor_si128)
    RTFACT__INT4_BINOP_FLOAT_ARITH(*, _mm_mul_ps)
    RTFACT__INT4_BINOP_FLOAT_ARITH(/, _mm_div_ps)

    RTFACT__VEC4_BINOP_ARITH(<<, _mm_sll_epi32)
    RTFACT__VEC4_BINOP_ARITH(>>, _mm_sra_epi32)

    RTFACT__INT4_BINOP_CMP   (< , _mm_cmplt_epi32)
    RTFACT__INT4_BINOP_CMP_EQ(<=, _mm_cmpgt_epi32)
    RTFACT__INT4_BINOP_CMP   (> , _mm_cmpgt_epi32)
    RTFACT__INT4_BINOP_CMP_EQ(>=, _mm_cmplt_epi32)
    RTFACT__INT4_BINOP_CMP   (==, _mm_cmpeq_epi32)
    RTFACT__INT4_BINOP_CMP_EQ(!=, _mm_cmpeq_epi32)

    RTFACT__VEC4_UNOP(-, _mm_sub_epi32, _mm_setzero_si128())
    RTFACT__VEC4_UNOP(~, _mm_xor_si128, C_T_T_T_T.data)

    RTFACT__INT4_BINOP_BIT_FLOAT4__DECLARE(&)
    RTFACT__INT4_BINOP_BIT_FLOAT4__DECLARE(|)
    RTFACT__INT4_BINOP_BIT_FLOAT4__DECLARE(^)

    RTFACT_SSE_INLINE const t_This operator<<(
        const int aData) const
    {
        return getInstance(_mm_slli_epi32(data, aData));
    }

    RTFACT_SSE_INLINE const t_This operator>>(
        const int aData) const
    {
        return getInstance(_mm_srai_epi32(data, aData));
    }

    #pragma endregion

    #pragma region Utility methods

    RTFACT__VEC4_GEN_SHUFFLE_AND_ACCESSORS_BASE

    template<
        uint taI1,
        uint taI2,
        uint taI3,
        uint taI4>
    RTFACT_SSE_INLINE const t_This shuffle() const
    {
        return getInstance(
            _mm_shuffle_epi32(data,_MM_SHUFFLE(taI4, taI3, taI2, taI1)));
    }

    // Note: only the .x component is valid!
    RTFACT_SSE_INLINE const t_This addHoriz() const
    {
        t_This t1 = *this + shuffle<2, 3, 0, 0>();
        return t1 + t1.shuffle<1, 1, 1, 1>();
    }

    RTFACT_SSE_INLINE const t_This andNot(
        const t_This& aMask) const
    {
        return getInstance(_mm_andnot_si128(aMask.data, data));
    }

    const t_This andNot(
        const float4& aMask) const;

    const int4 blend(
        const int4& aData1,
        const int4& aData2) const;

    const float4 blend(
        const float4& aData1,
        const float4& aData2) const;

    RTFACT_SSE_INLINE const uint getI32Mask() const
    {
        return _mm_movemask_ps(_mm_castsi128_ps(data));
    }

    static RTFACT_SSE_INLINE const t_This replicate(
        const int aValue)
    {
        return getInstance(_mm_set1_epi32(aValue));
    }

    RTFACT_SSE_INLINE const float4& asFloat4() const;
    RTFACT_SSE_INLINE float4& asFloat4();
    RTFACT_SSE_INLINE const float4 toFloat4() const;

    #pragma endregion

    #pragma region Constants

    RTFACT__INT4_DECLARE_CONSTS

    static const uint C_0_T_T_T_I32_MASK = 7;
    static const uint C_T_T_T_0_I32_MASK = 14;
    static const uint C_T_T_T_T_I32_MASK = 15;
    static const uint C_0_0_0_0_I32_MASK = 0;

    static RTFACT_SSE_INLINE const int4 C_0_0_0_0()
    {
        return getInstance(_mm_setzero_si128());
    }

    #pragma endregion
};

template<>
RTFACT_SSE_INLINE const int int4::at<0>() const
{
    return _mm_cvtsi128_si32(data);
}

#pragma endregion // int4 class definition

#pragma region float4 class definition

class RTFACT_ALIGN_SIMD float4
{
    typedef float4 t_This;
    typedef float  t_Scalar;

public:

    typedef __m128 Data;

    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

    RTFACT__VEC4_GEN_DATA_AND_CONSTRUCTORS(
        float4, _mm_set_ss, _mm_set_ps, _mm_load_ps)

    #pragma region Operators

    RTFACT__VEC4_BINOP_ARITH(+, _mm_add_ps)
    RTFACT__VEC4_BINOP_ARITH(-, _mm_sub_ps)
    RTFACT__VEC4_BINOP_ARITH(/, _mm_div_ps)
    RTFACT__VEC4_BINOP_ARITH(*, _mm_mul_ps)
    RTFACT__VEC4_BINOP_ARITH(&, _mm_and_ps)
    RTFACT__VEC4_BINOP_ARITH(|, _mm_or_ps)
    RTFACT__VEC4_BINOP_ARITH(^, _mm_xor_ps)

    RTFACT__FLOAT4_BINOP_CMP(< , _mm_cmplt_ps)
    RTFACT__FLOAT4_BINOP_CMP(<=, _mm_cmple_ps)
    RTFACT__FLOAT4_BINOP_CMP(> , _mm_cmpgt_ps)
    RTFACT__FLOAT4_BINOP_CMP(>=, _mm_cmpge_ps)
    RTFACT__FLOAT4_BINOP_CMP(==, _mm_cmpeq_ps)
    RTFACT__FLOAT4_BINOP_CMP(!=, _mm_cmpneq_ps)

    RTFACT__VEC4_UNOP(-, _mm_sub_ps, _mm_setzero_ps())
    RTFACT__VEC4_UNOP(~, _mm_xor_ps, C_1_1_1_1.data)

    RTFACT_FLOAT4_BINOP_BIT_INT4(&, _mm_and_ps)
    RTFACT_FLOAT4_BINOP_BIT_INT4(|, _mm_or_ps)
    RTFACT_FLOAT4_BINOP_BIT_INT4(^, _mm_xor_ps)

    #pragma endregion

    #pragma region Utility methods

    RTFACT__VEC4_GEN_SHUFFLE_AND_ACCESSORS_BASE

    template<
        uint taI1,
        uint taI2,
        uint taI3,
        uint taI4>
    RTFACT_SSE_INLINE const t_This shuffle() const
    {
        return getInstance(
            _mm_shuffle_ps(data, data, _MM_SHUFFLE(taI4, taI3, taI2, taI1)));
    }

    // Note: only the .x component is valid!
    RTFACT_SSE_INLINE const float4 addHoriz() const
    {
        float4 t1 = *this + shuffle<2, 3, 0, 0>();
        return t1 + t1.shuffle<1, 1, 1, 1>();
    }

    RTFACT_SSE_INLINE const float4 absOld() const
    {
        return getInstance(_mm_max_ps(_mm_sub_ps(RTFACT_SSE_0f, data),
                                                 data));
    }

    RTFACT_SSE_INLINE const float4 abs() const
    {
        return getInstance(_mm_and_ps(C_ABS_MASK.data, data));
    }

    RTFACT_SSE_INLINE const float4 sqrt() const
    {
        return getInstance(_mm_sqrt_ps(data));
    }

    RTFACT_SSE_INLINE const float4 reciprocal() const
    {
        #ifdef RTFACT_SSE_FAST_RECIPROCAL

            return getInstance(_mm_rcp_ps(data));

        #else

            // this does not handle zeros, infinites, etc.
            /*const Data recip = _mm_rcp_ps(data);
            return getInstance(_mm_sub_ps(
                _mm_add_ps(recip, recip),
                _mm_mul_ps(_mm_mul_ps(recip, recip), data)));*/

            const __m128 recip = _mm_rcp_ps(data);
            const __m128 temp = _mm_mul_ps(_mm_mul_ps(recip, recip), data);

            return getInstance(
                _mm_sub_ps(
                    _mm_add_ps(recip, recip),
                    _mm_and_ps(
                        _mm_cmpord_ps(temp, recip),
                        temp)));

        #endif
    }

    RTFACT_SSE_INLINE const float4 recipSqrt() const
    {
        #ifdef RTFACT_SSE_FAST_RECIP_SQRT

            return getInstance(_mm_rsqrt_ps(data));

        #else

            __m128 nr = _mm_rsqrt_ps(data);
            __m128 muls = _mm_mul_ps(_mm_mul_ps(data, nr), nr);

            return getInstance(_mm_mul_ps(_mm_mul_ps(
                C__0_5__0_5__0_5__0_5.data, nr),
                _mm_sub_ps(C_3_3_3_3.data, muls)));

        #endif
    }

    RTFACT_SSE_INLINE const float4 andNot(
        const float4& aMask) const
    {
        return getInstance(_mm_andnot_ps(aMask.data, data));
    }

    RTFACT_SSE_INLINE const float4 andNot(
        const int4& aMask) const
    {
        return getInstance(_mm_andnot_ps(_mm_castsi128_ps(aMask.data), data));
    }

    static RTFACT_SSE_INLINE float4 replicate(
        const float aValue)
    {
        #if defined(RTFACT_COMPILER_MSVC) && !defined(RTFACT_COMPILER_INTEL)

            return getInstance(_mm_set_ps1(aValue));

        #else

            #ifndef RTFACT_COMPILER_INTEL

                #pragma message "Please check the fastest way for replication"

            #endif

            const Data temp = _mm_set_ss(aValue);
            return getInstance(_mm_shuffle_ps(temp, temp, 0));

        #endif
    }

    RTFACT_SSE_INLINE const float max3() const;

    RTFACT_SSE_INLINE const float min3() const;

    RTFACT_SSE_INLINE const float max4() const;

    RTFACT_SSE_INLINE const float min4() const;

    static RTFACT_SSE_INLINE const float4 max(
        const float4& aData1,
        const float4& aData2)
    {
        return getInstance(_mm_max_ps(aData1.data, aData2.data));
    }

    static RTFACT_SSE_INLINE const float4 min(
        const float4& aData1,
        const float4& aData2)
    {
        return getInstance(_mm_min_ps(aData1.data, aData2.data));
    }

    static RTFACT_SSE_INLINE const float dot4(
        const float4& aData1,
        const float4& aData2);

    static RTFACT_SSE_INLINE float dot3(
        const float4& aData1,
        const float4& aData2);

    static RTFACT_SSE_INLINE const float4 cross(
        const float4& aData1,
        const float4& aData2)
    {
        return aData1.shuffle<1, 2, 0, 3>() * aData2.shuffle<2, 0, 1, 3>() -
               aData1.shuffle<2, 0, 1, 3>() * aData2.shuffle<1, 2, 0, 3>();
    }

    RTFACT_SSE_INLINE const float length3() const
    {
        return Math::sqrt(dot3(*this, *this));
    }

    RTFACT_SSE_INLINE const float length3Sqr() const
    {
        return dot3(*this, *this);
    }

    RTFACT_SSE_INLINE const float length4() const
    {
        return Math::sqrt(dot4(*this, *this));
    }

    RTFACT_SSE_INLINE const float length4Sqr() const
    {
        return dot4(*this, *this);
    }

    RTFACT_SSE_INLINE const float4 normalize3() const
    {
        return *this / getInstance(length3()).shuffle<0, 0, 0, 0>();
    }

    RTFACT_SSE_INLINE const float4 normalize4() const
    {
        return *this / replicate(length4());
    }

    RTFACT_SSE_INLINE const uint getI32Mask() const
    {
        return _mm_movemask_ps(data);
    }

    RTFACT_SSE_INLINE const int4& asInt4() const
    {
        //return int4::getInstance(_mm_castps_si128(data));
        return *(int4*)this;
    }

    RTFACT_SSE_INLINE int4& asInt4()
    {
        //return int4::getInstance(_mm_castps_si128(data));
        return *(int4*)this;
    }

    RTFACT_SSE_INLINE const float4 floor() const
    {
        return getInstance(
            Math::floor((*this)[0]),
            Math::floor((*this)[1]),
            Math::floor((*this)[2]),
            Math::floor((*this)[3]));
    }

    RTFACT_SSE_INLINE const int4 floorToInt() const
    {
        return int4::getInstance(
            Math::floorToInt((*this)[0]),
            Math::floorToInt((*this)[1]),
            Math::floorToInt((*this)[2]),
            Math::floorToInt((*this)[3]));
    }

    RTFACT_SSE_INLINE const float4 ceil() const
    {
        return getInstance(
            Math::ceil((*this)[0]),
            Math::ceil((*this)[1]),
            Math::ceil((*this)[2]),
            Math::ceil((*this)[3]));
    }

    RTFACT_SSE_INLINE const int4 ceilToInt() const
    {
        return int4::getInstance(
            Math::ceilToInt((*this)[0]),
            Math::ceilToInt((*this)[1]),
            Math::ceilToInt((*this)[2]),
            Math::ceilToInt((*this)[3]));
    }

    RTFACT_SSE_INLINE const float4 round() const
    {
        return float4::getInstance(_mm_cvtepi32_ps(_mm_cvtps_epi32(data)));
    }

    RTFACT_SSE_INLINE const int4 roundToInt() const
    {
        return int4::getInstance(_mm_cvtps_epi32(data));
    }

    RTFACT_SSE_INLINE const float4 truncate() const
    {
        return getInstance(_mm_cvtepi32_ps(_mm_cvttps_epi32(data)));
    }

    RTFACT_SSE_INLINE const int4 truncateToInt() const
    {
        return int4::getInstance(_mm_cvttps_epi32(data));
    }

    RTFACT_SSE_INLINE const float4 fraction() const
    {
        return float4::getInstance(
            Math::fraction((*this)[0]),
            Math::fraction((*this)[1]),
            Math::fraction((*this)[2]),
            Math::fraction((*this)[3]));
    }

    // 0 - first vector, 1 - second vector
    template<
        uint taI1,
        uint taI2,
        uint taI3,
        uint taI4>
    RTFACT_SSE_INLINE const float4 interleave(
        const float4& aOther) const
    {
        static int4 maskVect = int4::getInstance(taI1 - 1, taI2 - 1, taI3 - 1, taI4 - 1);
        return *this & maskVect | aOther.andNot(maskVect);
    }

    #pragma endregion

    #pragma region Constants

    RTFACT__FLOAT4_DECLARE_CONSTS

    static RTFACT_SSE_INLINE const float4 C_0_0_0_0()
    {
        return getInstance(_mm_setzero_ps());
    }

    #pragma endregion
};

template<>
RTFACT_SSE_INLINE const float float4::at<0>() const
{
    float ret;
    _mm_store_ss(&ret, data);
    return ret;
}

RTFACT_SSE_INLINE const float float4::max3() const
{
    const Data d0 = _mm_shuffle_ps(data, data, _MM_SHUFFLE(2, 2, 1, 0));
    const Data d = _mm_max_ps(data, _mm_movehl_ps(d0, d0));

    return getInstance(_mm_max_ss(
        d,
        _mm_shuffle_ps(d, d, _MM_SHUFFLE(1, 1, 1, 1)))).at<0>();
}

RTFACT_SSE_INLINE const float float4::min3() const
{
    const Data d0 = _mm_shuffle_ps(data, data, _MM_SHUFFLE(2, 2, 1, 0));
    const Data d = _mm_min_ps(data, _mm_movehl_ps(d0, d0));

    return getInstance(_mm_min_ss(
        d,
        _mm_shuffle_ps(d, d, _MM_SHUFFLE(1, 1, 1, 1)))).at<0>();
}

RTFACT_SSE_INLINE const float float4::max4() const
{
    const Data d = _mm_max_ps(data, _mm_movehl_ps(data, data));

    return  getInstance(_mm_max_ss(
        d,
        _mm_shuffle_ps(d, d, _MM_SHUFFLE(1, 1, 1, 1)))).at<0>();
}

RTFACT_SSE_INLINE const float float4::min4() const
{
    const Data d = _mm_min_ps(data, _mm_movehl_ps(data, data));

    return  getInstance(_mm_min_ss(
        d,
        _mm_shuffle_ps(d, d, _MM_SHUFFLE(1, 1, 1, 1)))).at<0>();
}

template<>
RTFACT_SSE_INLINE const float4 float4::shuffle<2, 3, 2, 3>() const
{
    return getInstance(_mm_movehl_ps(data, data));
}

template<>
RTFACT_SSE_INLINE const float4 float4::shuffle<0, 1, 0, 1>() const
{
    return getInstance(_mm_movelh_ps(data, data));
}

RTFACT_SSE_INLINE const float float4::dot4(
    const float4& aData1,
    const float4& aData2)
{
    //return (aData1 * aData2).addHoriz().at<0>();
    const float4 a = (aData1 * aData2).addHoriz();
    return a.at<0>();
}

RTFACT_SSE_INLINE float float4::dot3(
    const float4& aData1,
    const float4& aData2)
{
    return ((aData1 * aData2) & int4::C_T_T_T_0).addHoriz().at<0>();
}

#pragma endregion // float4 class definition

#pragma region float4-related int4 function definitions

RTFACT__INT4_BINOP_BIT_FLOAT4__DEFINE(&, _mm_and_ps)
RTFACT__INT4_BINOP_BIT_FLOAT4__DEFINE(|, _mm_or_ps)
RTFACT__INT4_BINOP_BIT_FLOAT4__DEFINE(^, _mm_xor_ps)

RTFACT_SSE_INLINE const float4& int4::asFloat4() const
{
    //return float4::getInstance(_mm_castsi128_ps(data));
    return *(float4*)this;
}

RTFACT_SSE_INLINE float4& int4::asFloat4()
{
    return *(float4*)this;
}

RTFACT_SSE_INLINE const float4 int4::toFloat4() const
{
    return float4::getInstance(_mm_cvtepi32_ps(data));
}

RTFACT_SSE_INLINE const int4 int4::andNot(
    const float4& aMask) const
{
    return getInstance(_mm_andnot_si128(_mm_castps_si128(aMask.data), data));
}

RTFACT_SSE_INLINE const int4 int4::blend(
    const int4& aData1,
    const int4& aData2) const
{
    #ifdef RTFACT_HAVE_SSE41

        return getInstance(_mm_blendv_epi8(aData2.data, aData1.data, data));

    #else

        return getInstance(
            _mm_or_si128(
                _mm_and_si128(data, aData1.data),
                _mm_andnot_si128(data, aData2.data)));

    #endif
}

RTFACT_SSE_INLINE const float4 int4::blend(
    const float4& aData1,
    const float4& aData2) const
{
    #ifdef RTFACT_HAVE_SSE41

    return float4::getInstance(
            _mm_blendv_ps(aData2.data, aData1.data, _mm_castsi128_ps(data)));

    #else

        return float4::getInstance(
            _mm_or_ps(
                _mm_and_ps(_mm_castsi128_ps(data), aData1.data),
                _mm_andnot_ps(_mm_castsi128_ps(data), aData2.data)));

    #endif
}

#pragma endregion // float4-related int4 function definitions

#pragma region Undefine the helper macros

#undef RTFACT__VEC4_GEN_DATA_AND_CONSTRUCTORS
#undef RTFACT__VEC4_GEN_SHUFFLE_AND_ACCESSORS_BASE

#undef RTFACT__VEC4_BINOP_ARITH
#undef RTFACT__INT4_BINOP_FLOAT_ARITH
#undef RTFACT__VEC4_UNOP
#undef RTFACT__INT4_BINOP_BIT_FLOAT4__DECLARE
#undef RTFACT__INT4_BINOP_BIT_FLOAT4__DEFINE
#undef RTFACT_FLOAT4_BINOP_BIT_INT4
#undef RTFACT__FLOAT4_BINOP_CMP
#undef RTFACT__INT4_BINOP_CMP
#undef RTFACT__INT4_BINOP_CMP_EQ

#undef RTFACT__SSE_DECLARE_CONST
#undef RTFACT__INT4_DECLARE_CONSTS
#undef RTFACT__FLOAT4_DECLARE_CONSTS

#undef RTFACT_SSE_INLINE
#undef RTFACT_SSE_ASSERT
#undef RTFACT_SSE_STATIC_ASSERT

#undef RTFACT__DAZ_BIT
#undef RTFACT__FTZ_BIT
#undef RTFACT__DENORMAL_EXCEPTION_MASK
#undef RTFACT__UNDERFLOW_EXCEPTION_MASK

#pragma endregion // Undefine the helper macros

RTFACT_NAMESPACE_END

#endif // RTFACT__SSEVEC4_HPP
