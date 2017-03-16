/*
 *  SamplingUtils.hpp
 *
 *  RTfact - Real-Time Ray Tracing Library
 *
 *  Copyright (C) 2008  Saarland University
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

#ifndef RTFACT__SAMPLINGUTILS_HPP
#define RTFACT__SAMPLINGUTILS_HPP

#include <RTfact/Config/Common.hpp>

#include <cstdlib>

#include <RTfact/Utils/Math/HaltonFaure.hpp>
#include <RTfact/Utils/Math/MathUtils.hpp>
#include <RTfact/Utils/Packets/Vec2f.hpp>
#include <RTfact/Utils/Packets/Vec3f.hpp>

#define RTFACT__MTWISTER_RAND_N   624
#define RTAFCT__M                397
#define RTAFCT__MATRIX_A         0x9908b0df
#define RTAFCT__UPPER_MASK       0x80000000
#define RTAFCT__LOWER_MASK       0x7fffffff
#define RTAFCT__TEMPERING_MASK_B 0x9d2c5680
#define RTAFCT__TEMPERING_MASK_C 0xefc60000
#define RTAFCT__TEMPERING_SHIFT_U(x) (x >> 11)
#define RTAFCT__TEMPERING_SHIFT_S(x) (x << 7)
#define RTAFCT__TEMPERING_SHIFT_T(x) (x << 15)
#define RTAFCT__TEMPERING_SHIFT_L(x) (x >> 18)

RTFACT_NAMESPACE_BEGIN

namespace Math {

namespace Detail {

    static uint MTWISTER_DATA[RTFACT__MTWISTER_RAND_N];
    static uint MTWISTER_INDEX;

} // namespace Detail

#pragma region Multiple importance sampling utilities

RTFACT_MATH_INLINE float balanceHeuristic(
    const float aPrimaryPdf,
    const float aSecondaryPdf,
    const uint aPrimaryPdfSamples = 1,
    const uint aSecondaryPdfSamples = 1)
{
    const float term1 = aPrimaryPdf * aPrimaryPdfSamples;
    const float term2 = aSecondaryPdf * aSecondaryPdfSamples;

    return term1 / (term1 + term2);
}

RTFACT_MATH_INLINE float powerHeuristic(
    const float aPrimaryPdf,
    const float aSecondaryPdf,
    const uint aPrimaryPdfSamples = 1,
    const uint aSecondaryPdfSamples = 1)
{
    // hard-coded power (2)
    const float term1 = aPrimaryPdf * aPrimaryPdfSamples;
    const float term2 = aSecondaryPdf * aSecondaryPdfSamples;
    const float term12 = term1 * term1;
    const float term22 = term2 * term2;

    return term12 / (term12 + term22);
}

#pragma endregion // Multiple importance sampling utilities

#pragma region Random number generation

RTFACT_MATH_INLINE void seedStd(
    const uint aSeed = static_cast<uint>(time(RTFACT_NULL)))
{
    srand(aSeed);
}

RTFACT_MATH_INLINE uint uniformIntStd()
{
    RTFACT_MATH_ASSERT(RTFACT_RAND_MAX == RAND_MAX);

    return rand();
}

RTFACT_MATH_INLINE void seedMTwister(
    const uint aSeed = static_cast<uint>(time(RTFACT_NULL)))
{
    Detail::MTWISTER_DATA[0] = aSeed & 0xFFFFFFFF;

    for(Detail::MTWISTER_INDEX = 1;
        Detail::MTWISTER_INDEX < RTFACT__MTWISTER_RAND_N;
        ++Detail::MTWISTER_INDEX)
    {
        Detail::MTWISTER_DATA[Detail::MTWISTER_INDEX] =
            (69069 * Detail::MTWISTER_DATA[Detail::MTWISTER_INDEX - 1]) & 0xFFFFFFFF;
    }

    uint temp = 373737;

    for(Detail::MTWISTER_INDEX = 1;
        Detail::MTWISTER_INDEX < RTFACT__MTWISTER_RAND_N;
        ++Detail::MTWISTER_INDEX)
    {
        Detail::MTWISTER_DATA[Detail::MTWISTER_INDEX] ^= temp;
        temp = temp * 5531 + 81547;
        temp ^= (temp >> 9) ^ (temp << 19);
    }
}

RTFACT_MATH_INLINE uint uniformIntMTwister()
{
    static const uint mag01[2] = {0x0, RTAFCT__MATRIX_A};

    if(Detail::MTWISTER_INDEX >= RTFACT__MTWISTER_RAND_N)
    {
        uint i;

        for(i = 0; i < RTFACT__MTWISTER_RAND_N - RTAFCT__M; ++i)
        {
            const uint temp =
                (Detail::MTWISTER_DATA[i] & RTAFCT__UPPER_MASK) |
                (Detail::MTWISTER_DATA[i + 1] & RTAFCT__LOWER_MASK);

            Detail::MTWISTER_DATA[i] =
                Detail::MTWISTER_DATA[i + RTAFCT__M] ^ (temp >> 1) ^ mag01[temp & 0x1];
        }

        for(;i < RTFACT__MTWISTER_RAND_N - 1; ++i)
        {
            const uint temp =
                (Detail::MTWISTER_DATA[i] & RTAFCT__UPPER_MASK) |
                (Detail::MTWISTER_DATA[i + 1] & RTAFCT__LOWER_MASK);

            Detail::MTWISTER_DATA[i] =
                Detail::MTWISTER_DATA[int(i) + (RTAFCT__M - RTFACT__MTWISTER_RAND_N)] ^
                (temp >> 1) ^ mag01[temp & 0x1];
        }

        const uint y =
            (Detail::MTWISTER_DATA[RTFACT__MTWISTER_RAND_N - 1] & RTAFCT__UPPER_MASK) |
            (Detail::MTWISTER_DATA[0] & RTAFCT__LOWER_MASK);

        Detail::MTWISTER_DATA[RTFACT__MTWISTER_RAND_N - 1] =
            Detail::MTWISTER_DATA[RTAFCT__M - 1] ^ (y >> 1) ^ mag01[y & 0x1];

        Detail::MTWISTER_INDEX = 0;
    }

    uint ret = Detail::MTWISTER_DATA[Detail::MTWISTER_INDEX++];

    ret ^= RTAFCT__TEMPERING_SHIFT_U(ret);
    ret ^= RTAFCT__TEMPERING_SHIFT_S(ret) & RTAFCT__TEMPERING_MASK_B;
    ret ^= RTAFCT__TEMPERING_SHIFT_T(ret) & RTAFCT__TEMPERING_MASK_C;
    ret ^= RTAFCT__TEMPERING_SHIFT_L(ret);

    return ret;
}

RTFACT_INLINE void seedRandom(
    const uint aSeed = static_cast<uint>(time(RTFACT_NULL)))
{
    #ifdef RTFACT_RAND_USE_MTWISTER

        seedMTwister(aSeed);

    #else

        seedStd(aSeed);

    #endif
}

/* Generates a uniform random integer in [0, RTFACT_RAND_MAX) */
RTFACT_MATH_INLINE uint uniformInt()
{
    #ifdef RTFACT_RAND_USE_MTWISTER

        return uniformIntMTwister();

    #else

        return uniformIntStd();

    #endif
}

/* Generates a uniform random integer packet in [0, RTFACT_RAND_MAX) */
RTFACT_MATH_INLINE const IPacketC uniformIntC()
{
    return IPacketC(uniformInt(), uniformInt(), uniformInt(), uniformInt());
}

/* Generates a uniform random float in [0, 1) */
RTFACT_MATH_INLINE float uniformFloat()
{
    return RTFACT_RAND_NORMALIZATION * uniformInt();
}

/* Generates a uniform random float in [0, 1) */
template<uint taSize>
RTFACT_MATH_INLINE const Packet<taSize, float> uniformFloat()
{
    typedef Packet<taSize, float>        t_Packet;
    typedef typename t_Packet::Container t_PacketC;
    t_Packet rndPacket;
    RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
    {
        for (uint k=0; k < t_PacketC::SIZE; ++k)
        {
            rndPacket(i)[k] = uniformFloat();
        }
    }
    return rndPacket;
}

/* Generates a uniform random float pair in [0, 1) */
RTFACT_MATH_INLINE const Vec2fC uniformFloat2DC()
{
    return Vec2fC(uniformFloat<Vec2fC::SIZE>(), uniformFloat<Vec2fC::SIZE>());
}

template<
    uint taSize>
RTFACT_MATH_INLINE const Vec2f<taSize> uniformFloat2D()
{
    Vec2f<taSize> res;
    RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
    {
        res(i) = uniformFloat2DC();
    }
    return res;
}

/* Generates a uniform random float pair in [0, 1) */
template<>
RTFACT_MATH_INLINE const Vec2f1 uniformFloat2D<1>()
{
    return Vec2f1(uniformFloat(), uniformFloat());
}

#pragma endregion // Random number generation

#pragma region Halton number generation

RTFACT_MATH_INLINE void genFaurePermutation(
    uint aBase,
    uint* oPermutation)
{
    uint i;

    if(aBase == 2)
    {
        oPermutation[0] = 0;
        oPermutation[1] = 1;
    }
    else if((aBase & 1) != 0)
    {
        aBase--;

        genFaurePermutation(aBase, oPermutation);

        for(i = 0; i < aBase; i++)
        {
            if(oPermutation[i] >= aBase / 2)
            {
                oPermutation[i]++;
            }
        }

        for(i = aBase; i > aBase / 2; i--)
        {
            oPermutation[i] = oPermutation[i - 1];
        }

        oPermutation[aBase / 2] = aBase / 2;
    }
    else
    {
        aBase /= 2;

        genFaurePermutation(aBase, oPermutation);

        for (i = 0; i < aBase; i++)
        {
            oPermutation[i] *= 2;
            oPermutation[i + aBase] = oPermutation[i] + 1;
        }
    }
}

RTFACT_MATH_INLINE float haltonBase(
    uint aSeed,
    const uint aBase)
{
    float ret = 0.f;
    const float invBase = 1.f / aBase;
    float invBi = invBase;

    while(aSeed > 0)
    {
        ret += (aSeed % aBase) * invBi;
        aSeed /= aBase;
        invBi *= invBase;
    }

    return ret;
}

RTFACT_MATH_INLINE float nextHaltonBase(
    float aPrevious,
    const uint aBase)
{
    float h, hh, r = 0.9999999f - aPrevious;
    const float invBase = 1.f / aBase;

    if(invBase < r)
    {
        aPrevious += invBase;
    }
    else
    {
        h = invBase;

        do
        {
            hh = h;
            h *= invBase;
        }
        while(h >= r);

        aPrevious += hh + h - 1.f;
    }

    return aPrevious;
}

RTFACT_MATH_INLINE float haltonFaureBase(
    uint aSeed,
    const uint aBase,
    const uint* aPermutation)
{
    float ret = 0.f;
    const float invBase = 1.f / aBase;
    float invBi = invBase;

    while(aSeed > 0)
    {
        ret += aPermutation[aSeed % aBase] * invBi;
        aSeed /= aBase;
        invBi *= invBase;
    }

    return ret;
}

RTFACT_MATH_INLINE float halton(
    uint aSeed,
    const uint aBaseIndex)
{
    return haltonBase(aSeed, getPrime(aBaseIndex));
}

RTFACT_MATH_INLINE float nextHalton(
    float aPrevious,
    const uint aBaseIndex)
{
    return nextHaltonBase(aPrevious, getPrime(aBaseIndex));
}

RTFACT_MATH_INLINE float haltonFaure(
    uint aSeed,
    const uint aBaseIndex)
{
    return haltonFaureBase(
        aSeed, getPrime(aBaseIndex), getFaurePermutation(aBaseIndex));
}

RTFACT_MATH_INLINE const Vec2f1 halton2DBase(
    uint aSeed,
    const uint aBaseX,
    const uint aBaseY)
{
    return Vec2f1(haltonBase(aSeed, aBaseX), haltonBase(aSeed, aBaseY));
}

RTFACT_MATH_INLINE const Vec2f1 nextHalton2DBase(
    const float aX,
    const float aY,
    const uint aBaseX,
    const uint aBaseY)
{
    return Vec2f1(nextHaltonBase(aX, aBaseX), nextHaltonBase(aY, aBaseY));
}

RTFACT_MATH_INLINE const Vec2f1 haltonFaure2DBase(
    uint aSeed,
    const uint aBaseX,
    const uint aBaseY,
    const uint* aPermutationX,
    const uint* aPermutationY)
{
    return Vec2f1(
        haltonFaureBase(aSeed, aBaseX, aPermutationX),
        haltonFaureBase(aSeed, aBaseY, aPermutationY));
}

RTFACT_MATH_INLINE const Vec2f1 halton2D(
    int aSeed,
    const int aBaseIndexX,
    const int aBaseIndexY)
{
    return halton2DBase(aSeed, getPrime(aBaseIndexX), getPrime(aBaseIndexY));
}

RTFACT_MATH_INLINE const Vec2f1 nextHalton2D(
    const float aX,
    const float aY,
    const uint aBaseIndexX,
    const uint aBaseIndexY)
{
    return nextHalton2DBase(aX, aY, getPrime(aBaseIndexX), getPrime(aBaseIndexY));
}

RTFACT_MATH_INLINE const Vec2f1 haltonFaure2D(
    int aSeed,
    const int aBaseIndexX,
    const int aBaseIndexY)
{
    return haltonFaure2DBase(
        aSeed, getPrime(aBaseIndexX), getPrime(aBaseIndexY),
        getFaurePermutation(aBaseIndexX), getFaurePermutation(aBaseIndexY));
}

#pragma endregion // Halton number generation

#pragma region OneD distributions

RTFACT_MATH_INLINE float sampleExponential(
    const float aSample,
    const float aExponent,
    const float aMax = RTFACT_INFINITY)
{
    RTFACT_MATH_ASSERT(aMax > 0.f);
    RTFACT_MATH_ASSERT(aExponent > 0.f);

    if(aMax == RTFACT_INFINITY)
    {
        return -Math::log(aSample) / aExponent;
    }
    else
    {
        const float normalization = 1.f - Math::exp(-aExponent * aMax);

        return -Math::log(1.f - aSample * normalization) / aExponent;
    }
}

RTFACT_MATH_INLINE float exponentialPDF(
    const float aValue,
    const float aExponent,
    const float aMax = RTFACT_INFINITY)
{
    RTFACT_MATH_ASSERT(aMax > 0.f);

    const float unnormalized = Math::exp(-aExponent * aValue) * aExponent;

    if(aMax == RTFACT_INFINITY)
    {
        return unnormalized;
    }
    else
    {
        const float normalization = 1.f - Math::exp(-aExponent * aMax);

        return  unnormalized / normalization;
    }
}

#pragma endregion // 1D distributions

#pragma region Sphere/hemisphere/triangle sampling

RTFACT_MATH_INLINE const Vec2f1 uniformTriangle(
    const float aX,
    const float aY)
{
    const float term = Math::sqrt(aX);

    return Vec2f1(1.f - term, aY * term);
}

RTFACT_MATH_INLINE const Vec3f1 uniformSphere(
    const Vec2f1& aSamples,
    float* oPDF = RTFACT_NULL)
{
    if(oPDF)
    {
        *oPDF = RTFACT_INV_4_PI;
    }

    const float term = 2.f * Math::sqrt(aSamples.y - aSamples.y * aSamples.y);

    return Vec3f1(
        Math::cos(RTFACT_2_PI * aSamples.x) * term,
        Math::sin(RTFACT_2_PI * aSamples.x) * term,
        1.f - 2.f * aSamples.y);
}

RTFACT_MATH_INLINE const Vec3fC uniformSphere(
    const Vec2fC& aSamples,
    FPacketC* oPDF = RTFACT_NULL)
{
    if(oPDF)
    {
        *oPDF = FPacketC::replicate(RTFACT_INV_4_PI);
    }

    const FPacketC term = 2.f * Math::sqrt(aSamples.y - aSamples.y * aSamples.y);

    return Vec3fC(
        Math::cos(RTFACT_2_PI * aSamples.x) * term,
        Math::sin(RTFACT_2_PI * aSamples.x) * term,
        1.f - 2.f * aSamples.y);
}

template<
    uint taSize>
RTFACT_MATH_INLINE const Vec3f<taSize> uniformSphere(
    const Vec2f<taSize>& aSamples,
    Packet<taSize, float>* oPDF = RTFACT_NULL)
{
    typedef Packet<taSize, float>   tPacket;
    typedef Vec3f<taSize>           tVec3f;

    if(oPDF)
    {
        *oPDF = tPacket::replicate(RTFACT_INV_4_PI);
    }

    const tPacket term = tPacket::replicate(2.f) * Math::sqrt(aSamples.y - aSamples.y * aSamples.y);

    return tVec3f(
        Math::cos(RTFACT_2_PI * aSamples.x) * term,
        Math::sin(RTFACT_2_PI * aSamples.x) * term,
        1.f - 2.f * aSamples.y);
}

RTFACT_MATH_INLINE float uniformSpherePDF()
{
    return RTFACT_INV_4_PI;
}

template<
    uint taSize>
RTFACT_MATH_INLINE Packet<taSize, float> uniformSpherePDF()
{
    typedef Packet<taSize, float>   tPacket;

    return tPacket::replicate(RTFACT_INV_4_PI);
}


RTFACT_MATH_INLINE const Vec3f1 uniformHemisphere(
    const Vec2f1& aSamples,
    float* oPDF = RTFACT_NULL)
{
    if(oPDF)
    {
        *oPDF = RTFACT_INV_2_PI;
    }

    const float term1 = RTFACT_2_PI * aSamples.x;
    const float term2 = Math::sqrt(1.f - aSamples.y * aSamples.y);

    return Vec3f1(
        Math::cos(term1) * term2,
        Math::sin(term1) * term2,
        aSamples.y);
}

RTFACT_MATH_INLINE float uniformHemispherePDF(
    const Vec3f1& aNormal,
    const Vec3f1& aDirection)
{
    const float cosTheta = Math::dot(aNormal, aDirection);

    return cosTheta > 0.f ? RTFACT_INV_2_PI : 0.f;
}

RTFACT_MATH_INLINE const Vec3fC uniformHemisphere(
    const Vec2fC& aSamples,
    FPacketC* oPDF = RTFACT_NULL)
{
    if(oPDF)
    {
        *oPDF = FPacketC::replicate(RTFACT_INV_2_PI);
    }

    const FPacketC term1 = RTFACT_2_PI * aSamples.x;
    const FPacketC term2 = Math::sqrt(1.f - aSamples.y * aSamples.y);

    return Vec3fC(
        Math::cos(term1) * term2,
        Math::sin(term1) * term2,
        aSamples.y);
}

template <
    uint taSize>
RTFACT_MATH_INLINE const Vec3f<taSize> uniformHemisphere(
    const Vec2f<taSize>& aSamples,
    Packet<taSize, float>* oPDF = RTFACT_NULL)
{
    typedef Packet<taSize, float>   tFPacket;
    typedef Vec3f<taSize>           tVec3f;

    if(oPDF)
    {
        *oPDF = tFPacket::replicate(RTFACT_INV_2_PI);
    }

    const tFPacket term1 = RTFACT_2_PI * aSamples.x;
    const tFPacket term2 = Math::sqrt(1.f - aSamples.y * aSamples.y);

    return tVec3f(
        Math::cos(term1) * term2,
        Math::sin(term1) * term2,
        aSamples.y);
}

RTFACT_MATH_INLINE const Vec3f1 cosineHemisphere(
    const Vec2f1& aSamples,
    float* oPDF = RTFACT_NULL)
{
    const float term1 = RTFACT_2_PI * aSamples.x;
    const float term2 = Math::sqrt(1.f - aSamples.y);

    const Vec3f1 ret(
        Math::cos(term1) * term2,
        Math::sin(term1) * term2,
        Math::sqrt(aSamples.y));

    if(oPDF)
    {
        *oPDF = ret.z * RTFACT_INV_PI;
    }

    return ret;
}

RTFACT_MATH_INLINE float cosineHemispherePDF(
    const Vec3f1& aNormal,
    const Vec3f1& aDirection)
{
    return Math::zeroClampDot(aNormal, aDirection) * RTFACT_INV_PI;
}

template <
    uint taSize>
RTFACT_MATH_INLINE const Vec3f<taSize> cosineHemisphere(
    const Vec2f<taSize>& aSamples,
    Packet<taSize, float>* oPDF = RTFACT_NULL)
{
    typedef Vec3f<taSize>           tVec3f;
    typedef Packet<taSize, float>   tFPacket;

    const tFPacket term1 = aSamples.x * tFPacket::replicate(RTFACT_2_PI);
    const tFPacket term2 = Math::sqrt(tFPacket::C_1 - aSamples.y);

    const tVec3f ret(
        Math::cos(term1) * term2,
        Math::sin(term1) * term2,
        Math::sqrt(aSamples.y));

    if(oPDF)
    {
        *oPDF = ret.z * RTFACT_INV_PI;
    }

    return ret;
}

RTFACT_MATH_INLINE const Vec3fC cosineHemisphere(
    const Vec2fC& aSamples,
    FPacketC* oPDF = RTFACT_NULL)
{
    const FPacketC term1 = RTFACT_2_PI * aSamples.x;
    const FPacketC term2 = Math::sqrt(1.f - aSamples.y);

    const Vec3fC ret(
        Math::cos(term1) * term2,
        Math::sin(term1) * term2,
        Math::sqrt(aSamples.y));

    if(oPDF)
    {
        *oPDF = ret.z * RTFACT_INV_PI;
    }

    return ret;
}

RTFACT_MATH_INLINE const Vec2f1 cosineHemisphere(
    const Vec3f1& aVector)
{
    Vec2f1 ret;

    ret.y = aVector.z * aVector.z;

    // avoid division by 0
    if(ret.y == 1.f)
    {
        ret.x = 0.f;
    }
    else
    {
        //const float temp1 = Math::atan2(aVector.y, aVector.x) * Math::sign(aVector.y);
        const float temp1 =
            (Math::acos(aVector.x / Math::sqrt(1.f - ret.y))) * Math::sign(aVector.y);
        ret.x = ((temp1 < 0.f) ? temp1 + RTFACT_2_PI : temp1) * RTFACT_INV_2_PI;
    }

    return ret;
}

RTFACT_MATH_INLINE const Vec3f1 powerCosineHemisphere(
    const Vec2f1& aSamples,
    const float aPower,
    float* oPDF = RTFACT_NULL)
{
    const float term1 = RTFACT_2_PI * aSamples.x;
    const float term2 = Math::pow(aSamples.y, Math::reciprocal(aPower + 1.f));
    const float term3 = Math::sqrt(1.f - term2 * term2);

    if(oPDF)
    {
        *oPDF = (aPower + 1.f) * Math::pow(term2, aPower) * RTFACT_INV_2_PI;
    }

    return Vec3f1(
        Math::cos(term1) * term3,
        Math::sin(term1) * term3,
        term2);
}

RTFACT_MATH_INLINE float powerCosineHemispherePDF(
    const Vec3f1& aNormal,
    const Vec3f1& aDirection,
    const float aPower)
{
    const float cosTheta = Math::dot(aNormal, aDirection);

    return (aPower + 1.f) * Math::pow(cosTheta, aPower) * RTFACT_INV_2_PI;
}

RTFACT_MATH_INLINE const Vec3fC powerCosineHemisphere(
    const Vec2fC& aSamples,
    const float aPower,
    FPacketC* oPDF = RTFACT_NULL)
{
    const FPacketC term1 = RTFACT_2_PI * aSamples.x;
    const FPacketC term2 = Math::pow(aSamples.y, Math::reciprocal(aPower + 1.f));
    const FPacketC term3 = Math::sqrt(1.f - term2 * term2);

    if(oPDF)
    {
        *oPDF =
            ((aPower + 1.f) * RTFACT_INV_2_PI) * Math::pow(term2, aPower);
    }

    return Vec3fC(
        Math::cos(term1) * term3,
        Math::sin(term1) * term3,
        term2);
}

#pragma endregion // Sphere/hemisphere sampling

#pragma region Cone sampling

RTFACT_MATH_INLINE const Vec3f1 uniformCone(
    const Vec2f1& aSamples,
    const float aCosSpan,
    float* oPDF = RTFACT_NULL)
{
    if(oPDF)
    {
        *oPDF = Math::reciprocal(RTFACT_2_PI * (1.f - aCosSpan));
    }

    const float cosTheta = Math::interpolate(aSamples.x, aCosSpan, 1.f);
    const float sinTheta = Math::sqrt(1.f - cosTheta * cosTheta);
    const float phi = aSamples.y * RTFACT_2_PI;

    return Vec3f1(
        Math::cos(phi) * sinTheta,
        Math::sin(phi) * sinTheta,
        cosTheta);
}

RTFACT_MATH_INLINE const Vec3f1 uniformCone(
    const Vec2f1& aSamples,
    const float aCosSpan,
    Packet<1, float>* oPDF = RTFACT_NULL)
{
    if(oPDF)
    {
        *oPDF = Math::reciprocal(RTFACT_2_PI * (1.f - aCosSpan));
    }

    const float cosTheta = Math::interpolate(aSamples.x, aCosSpan, 1.f);
    const float sinTheta = Math::sqrt(1.f - cosTheta * cosTheta);
    const float phi = aSamples.y * RTFACT_2_PI;

    return Vec3f1(
        Math::cos(phi) * sinTheta,
        Math::sin(phi) * sinTheta,
        cosTheta);
}

RTFACT_MATH_INLINE float uniformConePDF(
    const float aCosSpan)
{
    return Math::reciprocal(RTFACT_2_PI * (1.f - aCosSpan));
}

RTFACT_MATH_INLINE const Vec3fC uniformCone(
    const Vec2fC& aSamples,
    const FPacketC& aCosSpan,
    FPacketC* oPDF = RTFACT_NULL)
{
    if(oPDF)
    {
        *oPDF = Math::reciprocal(RTFACT_2_PI * (1.f - aCosSpan));
    }

    const FPacketC cosTheta = Math::interpolate(aSamples.x, aCosSpan, FPacketC::C_1);
    const FPacketC sinTheta = Math::sqrt(1.f - cosTheta * cosTheta);
    const FPacketC phi = aSamples.y * RTFACT_2_PI;

    return Vec3fC(
        Math::cos(phi) * sinTheta,
        Math::sin(phi) * sinTheta,
        cosTheta);
}

#pragma endregion // Cone sampling

#pragma region Disk sampling

RTFACT_MATH_INLINE const Vec2f1 uniformDisk(
    const Vec2f1& aSamples,
    float* oPDF = RTFACT_NULL)
{
    if(oPDF)
    {
        RTFACT_MATH_ASSERT(oPDF);

        *oPDF = RTFACT_INV_PI;
    }

    const float r = Math::sqrt(aSamples.x);
    const float theta = RTFACT_2_PI * aSamples.x;

    return Vec2f1(
        r * Math::cos(theta),
        r * Math::sin(theta));
}

RTFACT_MATH_INLINE float uniformDiskPDF()
{
    return RTFACT_INV_PI;
}

RTFACT_MATH_INLINE const Vec2fC uniformDisk(
    const Vec2fC& aSamples,
    FPacketC* oPDF = RTFACT_NULL)
{
    if(oPDF)
    {
        RTFACT_MATH_ASSERT(oPDF);

        *oPDF = FPacketC::replicate(RTFACT_INV_PI);
    }

    const FPacketC r = Math::sqrt(aSamples.x);
    const FPacketC theta = RTFACT_2_PI * aSamples.x;

    return Vec2fC(
        r * Math::cos(theta),
        r * Math::sin(theta));
}

RTFACT_MATH_INLINE const Vec2f1 concentricDisk(
    const Vec2f1& aSamples,
    float* oPDF = RTFACT_NULL)
{
    if(oPDF)
    {
        *oPDF = RTFACT_INV_PI;
    }

    const float sx = 2.f * aSamples.x - 1.f;
    const float sy = 2.f * aSamples.y - 1.f;

    if(sx == 0 && sy == 0)
    {
        return Vec2f1(0.f, 0.f);
    }

    const bool c1 = sx >= -sy;
    const bool c2 = sx > sy;
    const bool c1o2 = c1 | c2;
    const bool c1a2 = c1 & c2;
    const bool c1a2an1o2 = c1a2 | !c1o2;

    float r, theta;

    if(sx >= -sy)
    {
        if(sx > sy)
        {
            r = sx;
            theta = (sy > 0.0) ? sy / r : sy / r + 8.f;
        }
        else
        {
            r = sy;
            theta = 2.0f - sx / r;
        }
    }
    else
    {
        if(sx <= sy)
        {
            r = -sx;
            theta = 4.0f - sy/r;
        }
        else
        {
            r = -sy;
            theta = 6.0f + sx/r;
        }
    }

    theta *= RTFACT_PI_OVER_4;

    return Vec2f1(
        r * Math::cos(theta),
        r * Math::sin(theta));
}

RTFACT_MATH_INLINE const Vec2fC concentricDisk(
    const Vec2fC& aSamples,
    FPacketC* oPDF = RTFACT_NULL)
{
    Vec2fC ret;

    // HACK: inefficient
    for(uint i = 0; i < RTFACT_SIMD_WIDTH; ++i)
    {
        Vec2f1 pos;

        if(oPDF)
        {
            pos = Math::concentricDisk(aSamples[i], &(*oPDF)[i]);
        }
        else
        {
            pos = Math::concentricDisk(aSamples[i]);
        }

        ret.set(i, pos);
    }

    return ret;
}

RTFACT_MATH_INLINE const Vec2f1 concentricDisk(
    const Vec2f1& aSamples,
    Packet<1, float>* oPDF = RTFACT_NULL)
{
    if(oPDF)
    {
        *oPDF = RTFACT_INV_PI;
    }

    const float sx = 2.f * aSamples.x - 1.f;
    const float sy = 2.f * aSamples.y - 1.f;

    if(sx == 0 && sy == 0)
    {
        return Vec2f1(0.f, 0.f);
    }

    const bool c1 = sx >= -sy;
    const bool c2 = sx > sy;
    const bool c1o2 = c1 | c2;
    const bool c1a2 = c1 & c2;
    const bool c1a2an1o2 = c1a2 | !c1o2;

    float r, theta;

    if(sx >= -sy)
    {
        if(sx > sy)
        {
            r = sx;
            theta = (sy > 0.0) ? sy / r : sy / r + 8.f;
        }
        else
        {
            r = sy;
            theta = 2.0f - sx / r;
        }
    }
    else
    {
        if(sx <= sy)
        {
            r = -sx;
            theta = 4.0f - sy/r;
        }
        else
        {
            r = -sy;
            theta = 6.0f + sx/r;
        }
    }

    theta *= RTFACT_PI_OVER_4;

    return Vec2f1(
        r * Math::cos(theta),
        r * Math::sin(theta));
}

#pragma endregion // Disk sampling

#pragma region Local frames and transformations

RTFACT_MATH_INLINE const Vec3f1 getBinormal(
    const Vec3f1& aNormal)
{
    if(Math::abs(aNormal.x) > Math::abs(aNormal.y))
    {
        const float invLen =
            Math::recipSqrt(aNormal.x * aNormal.x + aNormal.z * aNormal.z);

        return Vec3f1(-aNormal.z * invLen, 0.f, aNormal.x * invLen);
    }
    else
    {
        const float invLen =
            Math::recipSqrt(aNormal.y * aNormal.y + aNormal.z * aNormal.z);

        return Vec3f1(0.f, aNormal.z * invLen, -aNormal.y * invLen);
    }
}

RTFACT_MATH_INLINE void getFrame(
    const Vec3f1& aNormal,
    Vec3f1& oBinormal,
    Vec3f1& oTangent)
{
    oBinormal = getBinormal(aNormal);
    oTangent = Math::cross(aNormal, oBinormal);
}

RTFACT_MATH_INLINE const Vec3fC getBinormal(
    const Vec3fC& aNormal)
{
    const MaskC mask = (Math::abs(aNormal.x) > Math::abs(aNormal.y));
    const FPacketC minusY = -aNormal.y;
    const FPacketC x = mask.iif(aNormal.x, minusY);

    const FPacketC invLen = Math::reciprocal(x * x + aNormal.z * aNormal.z);

    const FPacketC zTimesInvLen = aNormal.z * invLen;
    const FPacketC zero = FPacketC::C_0();

    return Vec3fC(
        mask.iif(-zTimesInvLen, zero),
        mask.iif(zero, zTimesInvLen),
        x * invLen);
}

RTFACT_MATH_INLINE void getFrame(
    const Vec3fC& aNormal,
    Vec3fC& oBinormal,
    Vec3fC& oTangent)
{
    oBinormal = getBinormal(aNormal);
    oTangent = Math::cross(aNormal, oBinormal);
}

RTFACT_MATH_INLINE const Vec3f1 transformToLocal(
    const Vec3f1& aVector,
    const Vec3f1& aNormal,
    const Vec3f1& aBinormal,
    const Vec3f1& aTangent)
{
    return Vec3f1(
        Math::dot(aVector, aBinormal),
        Math::dot(aVector, aTangent),
        Math::dot(aVector, aNormal));
}

RTFACT_MATH_INLINE const Vec3fC transformToLocal(
    const Vec3fC& aVector,
    const Vec3f1& aNormal,
    const Vec3f1& aBinormal,
    const Vec3f1& aTangent)
{
    return Vec3fC(
        Math::dot(aVector, aBinormal),
        Math::dot(aVector, aTangent),
        Math::dot(aVector, aNormal));
}

RTFACT_MATH_INLINE const Vec3fC transformToLocal(
    const Vec3fC& aVector,
    const Vec3fC& aNormal,
    const Vec3fC& aBinormal,
    const Vec3fC& aTangent)
{
    return Vec3fC(
        Math::dot(aVector, aBinormal),
        Math::dot(aVector, aTangent),
        Math::dot(aVector, aNormal));
}

RTFACT_MATH_INLINE const Vec3f1 transformToLocal(
    const Vec3f1& aVector,
    const Vec3f1& aNormal)
{
    Vec3f1 binormal, tangent;
    Math::getFrame(aNormal, binormal, tangent);

    return transformToLocal(aVector, aNormal, binormal, tangent);
}

RTFACT_MATH_INLINE const Vec3fC transformToLocal(
    const Vec3fC& aVector,
    const Vec3f1& aNormal)
{
    Vec3f1 binormal, tangent;
    Math::getFrame(aNormal, binormal, tangent);

    return transformToLocal(aVector, aNormal, binormal, tangent);
}

RTFACT_MATH_INLINE const Vec3fC transformToLocal(
    const Vec3fC& aVector,
    const Vec3fC& aNormal)
{
    Vec3fC binormal, tangent;
    Math::getFrame(aNormal, binormal, tangent);

    return transformToLocal(aVector, aNormal, binormal, tangent);
}

RTFACT_MATH_INLINE const Vec3f1 transformToWorld(
    const Vec3f1& aVector,
    const Vec3f1& aNormal,
    const Vec3f1& aBinormal,
    const Vec3f1& aTangent)
{
    return
        aVector.x * aBinormal +
        aVector.y * aTangent +
        aVector.z * aNormal;
}

RTFACT_MATH_INLINE const Vec3fC transformToWorld(
    const Vec3fC& aVector,
    const Vec3f1& aNormal,
    const Vec3f1& aBinormal,
    const Vec3f1& aTangent)
{
    return
        aVector.x * aBinormal +
        aVector.y * aTangent +
        aVector.z * aNormal;
}

RTFACT_MATH_INLINE const Vec3fC transformToWorld(
    const Vec3fC& aVector,
    const Vec3fC& aNormal,
    const Vec3fC& aBinormal,
    const Vec3fC& aTangent)
{
    return
        aVector.x * aBinormal +
        aVector.y * aTangent +
        aVector.z * aNormal;
}

RTFACT_MATH_INLINE const Vec3f1 transformToWorld(
    const Vec3f1& aVector,
    const Vec3f1& aNormal)
{
    Vec3f1 binormal, tangent;
    Math::getFrame(aNormal, binormal, tangent);

    return transformToWorld(aVector, aNormal, binormal, tangent);
}

RTFACT_MATH_INLINE const Vec3fC transformToWorld(
    const Vec3fC& aVector,
    const Vec3f1& aNormal)
{
    Vec3f1 binormal, tangent;
    Math::getFrame(aNormal, binormal, tangent);

    return transformToWorld(aVector, aNormal, binormal, tangent);
}

RTFACT_MATH_INLINE const Vec3fC transformToWorld(
    const Vec3fC& aVector,
    const Vec3fC& aNormal)
{
    Vec3fC binormal, tangent;
    Math::getFrame(aNormal, binormal, tangent);

    return transformToWorld(aVector, aNormal, binormal, tangent);
}

#pragma endregion // Local frames and transformations

#pragma region Spherical coordinaces

RTFACT_MATH_INLINE float sphericalPhi(
    const Vec3f1& aDirection)
{
    return Math::acos(Math::clamp(aDirection.z, FPacket1::replicate(-1.f), FPacket1::replicate(1.f)));
}

RTFACT_MATH_INLINE float sphericalTheta(
    const Vec3f1& aDirection)
{
    const float p = Math::atan2(aDirection.y, aDirection.x);

    return (p < 0.f) ? p + RTFACT_2_PI : p;
}

RTFACT_MATH_INLINE const Vec3f1 sphericalDir(
    const float aSinTheta,
    const float aCosTheta,
    const float aPhi,
    const Vec3f1& aX,
    const Vec3f1& aY,
    const Vec3f1& aZ)
{
    return aSinTheta * cos(aPhi) * aX + aSinTheta * sin(aPhi) * aY + aCosTheta * aZ;
}

RTFACT_MATH_INLINE const Vec3f1 sphericalDir(
    const float aSinTheta,
    const float aCosTheta,
    const float aPhi,
    const Vec3f1& aZ)
{
    Vec3f1 v1, v2;

    Math::getFrame(aZ, v1, v2);

    return sphericalDir(aSinTheta, aCosTheta, aPhi, v1, v2, aZ);
}

RTFACT_MATH_INLINE const Vec3f1 sphericalDir(
    const float aSinTheta,
    const float aCosTheta,
    const float aPhi)
{
    return Vec3f1(aSinTheta * cos(aPhi), aSinTheta * sin(aPhi), aCosTheta);
}

#pragma endregion // Spherical coordinates

#pragma region Variance computation

RTFACT_MATH_INLINE float estimatorVariance(
    const float aSampleSum,
    const float aSampleSum2,
    const float aSampleCount)
{
    const float invSampleCount = reciprocal(aSampleCount);

    return
        (invSampleCount * aSampleSum2 - sqr(invSampleCount * aSampleSum)) /
        (aSampleCount - 1.f);
}

RTFACT_MATH_INLINE float signalVariance(
    const float aSampleSum,
    const float aSampleSum2,
    const float aSampleCount)
{
    const float invSampleCount = reciprocal(aSampleCount);

    return
        (aSampleSum2 - invSampleCount * sqr(aSampleSum)) /
        (aSampleCount - 1.f);
}

#pragma endregion // Variance computation

} // namespace Math

RTFACT_NAMESPACE_END

#undef RTFACT__MTWISTER_RAND_N
#undef RTAFCT__M
#undef RTAFCT__MATRIX_A
#undef RTAFCT__UPPER_MASK
#undef RTAFCT__LOWER_MASK
#undef RTAFCT__TEMPERING_MASK_B
#undef RTAFCT__TEMPERING_MASK_C
#undef RTAFCT__TEMPERING_SHIFT_U
#undef RTAFCT__TEMPERING_SHIFT_S
#undef RTAFCT__TEMPERING_SHIFT_T
#undef RTAFCT__TEMPERING_SHIFT_L

#endif // RTFACT__SAMPLINGUTILS_HPP
