/*
 *  MathUtils.hpp
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

#ifndef RTFACT__MATH_UTILS_HPP
#define RTFACT__MATH_UTILS_HPP

#include <RTfact/Config/Common.hpp>

#include <cmath>
#include <time.h>

#define RTFACT__INV_MAX_RAND        (1.0f/(RAND_MAX))
#define RTFACT__INV_MAX_RAND_PLUS_1 (1.0f/(RAND_MAX+1.0f))

#define RTFACT__ROUND_EPSILON       0.5f

#if defined(RTFACT_COMPILER_MSVC) && !defined(RTFACT_ARCH_64_BIT) && !defined(RTFACT_PRECISE_INT)
#    define RTFACT__FAST_INT
#endif

RTFACT_NAMESPACE_BEGIN
namespace Math {

#pragma region Standard math functions

#define RTFACT__COMMON_MATH_FLOAT_UNARY_FUNCTION(aFuncName, aCommonFuncName)   \
    RTFACT_MATH_INLINE float aFuncName(                                        \
        const float aValue)                                                    \
    {                                                                          \
        return aCommonFuncName(aValue);                                        \
    }

#define RTFACT__COMMON_MATH_FLOAT_BINARY_FUNCTION(aFuncName, aCommonFuncName)  \
    RTFACT_MATH_INLINE float aFuncName(                                        \
        const float aValue1,                                                   \
        const float aValue2)                                                   \
    {                                                                          \
        return aCommonFuncName(aValue1, aValue2);                              \
    }

RTFACT__COMMON_MATH_FLOAT_UNARY_FUNCTION(sin,  ::sinf)
RTFACT__COMMON_MATH_FLOAT_UNARY_FUNCTION(cos,  ::cosf)
RTFACT__COMMON_MATH_FLOAT_UNARY_FUNCTION(asin, ::asinf)
RTFACT__COMMON_MATH_FLOAT_UNARY_FUNCTION(acos, ::acosf)
RTFACT__COMMON_MATH_FLOAT_UNARY_FUNCTION(tan,  ::tanf)
RTFACT__COMMON_MATH_FLOAT_UNARY_FUNCTION(atan, ::atanf)
RTFACT__COMMON_MATH_FLOAT_UNARY_FUNCTION(abs,  ::fabsf)
RTFACT__COMMON_MATH_FLOAT_UNARY_FUNCTION(sqrt, ::sqrtf)

RTFACT__COMMON_MATH_FLOAT_BINARY_FUNCTION(pow, ::powf)
RTFACT__COMMON_MATH_FLOAT_BINARY_FUNCTION(atan2, ::atan2f)

#pragma endregion // Standard math functions

#pragma region Utility functions

RTFACT_MATH_INLINE float reciprocal(
    const float aValue)
{
    return 1.f / aValue;
}

RTFACT_MATH_INLINE float sqr(
    const float aValue)
{
    return aValue * aValue;
}

RTFACT_MATH_INLINE uint sqr(
    const int aValue)
{
    return aValue * aValue;
}

RTFACT_MATH_INLINE float exp(
    const float aValue)
{
    return ::expf(aValue);
}

RTFACT_MATH_INLINE float recipSqrt(
    float aValue)
{
    float xhalf = 0.5f * aValue;
    int i = *(int*) &aValue;

    i = 0x5f3759df - (i >> 1);
    aValue = *(float*) &i;
    aValue = aValue * (1.5f - xhalf * aValue * aValue);

    return aValue;
}

RTFACT_MATH_INLINE float radians(
    const float aDegrees)
{
    return aDegrees * RTFACT_PI_OVER_180;
}

RTFACT_MATH_INLINE float degrees(
    const float aRadians)
{
    return aRadians * RTFACT_180_OVER_PI;
}

RTFACT_MATH_INLINE bool isPowerOf2(
    const int aValue)
{
    return (aValue & (aValue - 1)) == 0;
}

RTFACT_MATH_INLINE bool isPowerOf4(
    const int aValue)
{
    return ((aValue & (aValue - 1)) == 0) && (aValue & 0x55555555);
}

RTFACT_MATH_INLINE float min(
    const float aValue1,
    const float aValue2)
{
    return (aValue2 < aValue1) ? aValue2 : aValue1;
}

RTFACT_MATH_INLINE int min(
    const int aValue1,
    const int aValue2)
{
    return (aValue2 < aValue1) ? aValue2 : aValue1;
}

RTFACT_MATH_INLINE uint min(
    const uint aValue1,
    const uint aValue2)
{
    return (aValue2 < aValue1) ? aValue2 : aValue1;
}

RTFACT_MATH_INLINE float max(
    const float aValue1,
    const float aValue2)
{
    return (aValue2 > aValue1) ? aValue2 : aValue1;
}

RTFACT_MATH_INLINE int max(
    const int aValue1,
    const int aValue2)
{
    return (aValue2 > aValue1) ? aValue2 : aValue1;
}

RTFACT_MATH_INLINE uint max(
    const uint aValue1,
    const uint aValue2)
{
    return (aValue2 > aValue1) ? aValue2 : aValue1;
}

RTFACT_MATH_INLINE float interpolate(
    const float aPosition,
    const float aValue1,
    const float aValue2)
{
    return (1.f - aPosition) * aValue1 + aPosition * aValue2;
}

RTFACT_MATH_INLINE float interpolate(
    const float aPositionX,
    const float aPositionY,
    const float aValue00,
    const float aValue10,
    const float aValue01,
    const float aValue11)
{
    const float oppWeightX = 1.f - aPositionX;
    const float oppWeightY = 1.f - aPositionY;

    return
        oppWeightX * oppWeightY * aValue00 +
        oppWeightX * aPositionY * aValue01 +
        aPositionX * oppWeightY * aValue10 +
        aPositionX * aPositionY * aValue11;
}

template<
    class tValue>
RTFACT_MATH_INLINE tValue clamp(
    const tValue aValue,
    const tValue aLow,
    const tValue aHigh)
{
    return (aValue < aLow) ? aLow : ((aValue > aHigh) ? aHigh : aValue);
}

RTFACT_MATH_INLINE int mod(
    const int aValue1,
    const int aValue2)
{
    const int tempVal = (aValue1 / aValue2) * aValue2;

    return aValue1 - tempVal + ((tempVal > aValue1) ? aValue2 : 0);
}

RTFACT_MATH_INLINE float log(
    const float aValue)
{
    return logf(aValue);
}

RTFACT_MATH_INLINE float log10(
    const float aValue)
{
    return log10f(aValue);
}

RTFACT_MATH_INLINE float log2(
    const float aValue)
{
    return logf(aValue) * RTFACT_INV_LOG2;
}

RTFACT_MATH_INLINE int log2Int(
    const float aValue)
{
    return ((*(int *) &aValue) >> 23) - 127;
}

template<
    class tType>
RTFACT_MATH_INLINE bool isInfinity(
    tType aValue)
{
    return (aValue == std::numeric_limits<tType>::infinity());
}

template<
    class tType>
RTFACT_MATH_INLINE bool isNaN(
    tType aValue)
{
    return (aValue == std::numeric_limits<tType>::quiet_NaN());
}

RTFACT_MATH_INLINE float sign(
const float aValue)
{
    return (aValue < 0.f) ? -1.f : 1.f;
}

RTFACT_MATH_INLINE int signBit(
    const float aValue)
{
    return 2147483648u & *reinterpret_cast<const int*>(&aValue);
}

#pragma endregion // Utility functions

#pragma region Conversion and rounding

namespace Detail {

#ifdef RTFACT__FAST_INT

    static RTFACT_MATH_INLINE void _roundToInt(
        int *oResult,
        const float aValue)
    {
        __asm  fld  aValue
        __asm  mov  edx, oResult
        __asm  FRNDINT
        __asm  fistp dword ptr [edx];
    }

#endif

    template<
        uint taNumber>
    struct NumberOfSignificantBits;

    template<>
    struct NumberOfSignificantBits<0>
    {
        enum { VALUE = 0 };
    };

    template<
        uint taNumber>
    struct NumberOfSignificantBits
    {
        enum { VALUE = 1 + NumberOfSignificantBits<(taNumber >> 1)>::VALUE };
    };

    template<
        uint taNumber>
    struct FloatConvertShift
    {
        enum
        {
            SHIFT_LEFT = NumberOfSignificantBits<taNumber>::VALUE < 23,
            SHIFT_AMOUNT = SHIFT_LEFT ?
            23 - NumberOfSignificantBits<taNumber>::VALUE :
            NumberOfSignificantBits<taNumber>::VALUE - 23
        };
    };

} // namespace Detail

RTFACT_MATH_INLINE float toFloat(
    const int aValue)
{
    return static_cast<float>(aValue);
}

template<
    uint taMaxValue>
RTFACT_MATH_INLINE float toFloat01(
    const uint aValue)
{
    union { int i; float f; };

    if(Detail::FloatConvertShift<taMaxValue>::SHIFT_LEFT)
    {
        i = 0x3F800000 | (aValue << Detail::FloatConvertShift<taMaxValue>::SHIFT_AMOUNT);
    }
    else
    {
        i = 0x3F800000 | (aValue >> Detail::FloatConvertShift<taMaxValue>::SHIFT_AMOUNT);
    }

    return f - 1.0f;
}

RTFACT_MATH_INLINE int roundUpToPowerOf2(
    int aValue)
{
    aValue--;

    aValue |= aValue >> 1;
    aValue |= aValue >> 2;
    aValue |= aValue >> 4;
    aValue |= aValue >> 8;
    aValue |= aValue >> 16;

    return aValue + 1;
}

RTFACT_MATH_INLINE float fraction(
    const float aValue1)
{
    return aValue1 - floor(aValue1);
}

RTFACT_MATH_INLINE void fraction(
    const float aValue,
    float& oIntegerPart,
    float& oFractionalPart)
{
    oIntegerPart = floor(aValue);
    oFractionalPart = aValue - oIntegerPart;
}

RTFACT_MATH_INLINE float fraction(
    const float aValue,
    float& oIntegerPart)
{
    float fracPart;
    fraction(aValue, oIntegerPart, fracPart);
    return fracPart;
}

RTFACT_MATH_INLINE float round(
    const float aValue)
{
    int intRep = *(int*) &aValue;
    int exp = ((intRep >> 23) & 0xFF) - 0x7F; // exponent in normal int

    if( exp < -1)
    {
        return 0;
    }

    int man = (intRep & 0x7FFFFF) | 0x800000; // mantisa (plus hidden 1)

    int mask = ~(0x7FFFFF >> (exp + 1)); // mask to get rid of all but the first decimal (binar)
    int add  = 0x800000 >> (exp+1);

    int manStrip = man & mask;
    int manRound = manStrip + add; // 1.0 + 0.5 = 1.5, 1.5 + 0.5 = 2.0
    int manStrip2 = manRound & (mask << 1); // get rid of the last digit;

    int fi2 = (0xFF800000 & intRep) + (manStrip2 >> 1); // patch new mantisa to old exp&sign
    int fi3 = (0xFF800000 & fi2) | (manStrip2 & 0x7FFFFF);

    return *(float*) &fi3;
}

RTFACT_MATH_INLINE int roundToInt(
    const float aValue)
{
   /* #ifdef RTFACT__FAST_INT

        int ret;
        _roundToInt(&ret, aValue);

        return ret;

    #else*/

        int fi = *(int*) &aValue;
        int exp = ((fi >> 23)&0xFF)-0x7F; // exponent in normal int

        if(exp < -1)
        {
            return 0;
        }

        int man = (fi & 0x7FFFFF) | 0x800000; // mantisa (plus hidden 1)

        int man1dec = man >> (22 - exp); // mask to get rid of all but the first decimal (binar)
        int manRound = man1dec + 1;
        int manRes = manRound >> 1;

        if((fi & 0x80000000) != 0)
        {
            return -manRes;
        }

        return manRes;

        //return static_cast<int>(aValue + 0.5f);

    //#endif
}

RTFACT_MATH_INLINE float floor(
    const float aValue)
{
    return floorf(aValue);
}

RTFACT_MATH_INLINE int floorToInt(
    const float aValue)
{
    #ifdef RTFACT__FAST_INT

        return roundToInt(aValue - RTFACT__ROUND_EPSILON);

    #else

        return static_cast<int>(floorf(aValue));

    #endif
}

RTFACT_MATH_INLINE float ceil(
    const float aValue1)
{
    return ceilf(aValue1);
}

RTFACT_MATH_INLINE int ceilToInt(
    const float aValue)
{
    #ifdef RTFACT__FAST_INT

        return roundToInt(aValue + RTFACT__ROUND_EPSILON);

    #else

        return static_cast<int>(ceilf(aValue));

    #endif
}

RTFACT_MATH_INLINE float truncate(
    const float aValue)
{
    int fi = *(int*) &aValue;

    int exp = ((fi >> 23) & 0xFF) - 0x7F; // exponent in normal int

    if(exp < 0)
    {
        return 0;
    }

    int man = fi & 0x7FFFFF; // mantisa (plus hidden 1)
    int mask = ~(0x7FFFFF >> exp); // mask to get rid of all but the first decimal (binar)
    int manStrip = man & mask;
    int fi2 = (0xFF800000 & fi) | (manStrip); // patch new mantisa to old exp&sign

    return *(float*) &fi2;
}

RTFACT_MATH_INLINE int truncateToInt(
    const float aValue)
{
    int fi = *(int*) &aValue;
    int exp = ((fi >> 23) & 0xFF) - 0x7F; // exponent in normal int

    if(exp < 0)
    {
        return 0;
    }

    int man = (fi & 0x7FFFFF) | 0x800000; // mantisa (plus hidden 1)
    int res = man >> (23 - exp);

    if((fi & 0x80000000) != 0)
    {
        return -res;
    }

    return res;
}

#pragma endregion // Conversion and rounding

RTFACT_MATH_INLINE float smoothStep(
    const float aValue,
    const float aMin,
    const float aMax)
{
    const float v = clamp((aValue - aMin) / (aMax - aMin), 0.f, 1.f);

    return v * v * (3.f - 2.f * v);
}

} // namespace Math
RTFACT_NAMESPACE_END

#pragma region Undefine helper macros

#undef RTFACT__COMMON_MATH_FLOAT_UNARY_FUNCTION
#undef RTFACT__COMMON_MATH_FLOAT_BINARY_FUNCTION

#undef RTFACT__ROUND_EPSILON

#undef RTFACT__INV_MAX_RAND
#undef RTFACT__INV_MAX_RAND_PLUS_1

#ifdef RTFACT__FAST_INT
#    undef RTFACT__FAST_INT
#endif

#pragma endregion // Undefine helper macros

#endif // RTFACT__MATH_UTILS_HPP
