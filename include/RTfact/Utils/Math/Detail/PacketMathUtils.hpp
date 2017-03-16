/* 
 *  PacketMathUtils.hpp
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
 
 #ifndef RTFACT__PACKETMATHUTILS_HPP
#define RTFACT__PACKETMATHUTILS_HPP

#include <RTfact/Config/Common.hpp>

#include <RTfact/Utils/Math/MathUtils.hpp>

RTFACT_NAMESPACE_BEGIN

namespace Math {

#pragma region Binary function definition macros

#pragma region RTFACT__BINARY_FUNCTION_PACKET_PACKET macro definition

#define RTFACT__BINARY_FUNCTION_PACKET_PACKET(aFunc, aBaseFunc, aScalarType)   \
    template<                                                                  \
        uint taSize>                                                           \
    RTFACT_MATH_INLINE Packet<taSize, aScalarType> aFunc(                      \
        const Packet<taSize, aScalarType>& aPacket1,                           \
        const Packet<taSize, aScalarType>& aPacket2)                           \
    {                                                                          \
        Packet<taSize, aScalarType> ret;                                       \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)                            \
        {                                                                      \
            ret(i)[0] = aBaseFunc(aPacket1(i)[0], aPacket2(i)[0]);             \
            ret(i)[1] = aBaseFunc(aPacket1(i)[1], aPacket2(i)[1]);             \
            ret(i)[2] = aBaseFunc(aPacket1(i)[2], aPacket2(i)[2]);             \
            ret(i)[3] = aBaseFunc(aPacket1(i)[3], aPacket2(i)[3]);             \
        }                                                                      \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    template<>                                                                 \
    RTFACT_MATH_INLINE Packet<4, aScalarType> aFunc<4>(                        \
        const Packet<4, aScalarType>& aPacket1,                                \
        const Packet<4, aScalarType>& aPacket2)                                \
    {                                                                          \
        return Packet<4, aScalarType>(Packet<4, aScalarType>::Container(       \
            aBaseFunc(aPacket1[0], aPacket2[0]),                               \
            aBaseFunc(aPacket1[1], aPacket2[1]),                               \
            aBaseFunc(aPacket1[2], aPacket2[2]),                               \
            aBaseFunc(aPacket1[3], aPacket2[3])));                             \
    }                                                                          \
                                                                               \
    template<>                                                                 \
    RTFACT_MATH_INLINE Packet<1, aScalarType> aFunc<1>(                        \
        const Packet<1, aScalarType>& aPacket1,                                \
        const Packet<1, aScalarType>& aPacket2)                                \
    {                                                                          \
        Packet<1, aScalarType> ret;                                            \
        ret.data = aBaseFunc(aPacket1.data, aPacket2.data);                    \
        return ret;                                                            \
    }

#pragma endregion

#pragma region RTFACT__BINARY_FUNCTION_PACKET_SCALAR macro definition

#define RTFACT__BINARY_FUNCTION_PACKET_SCALAR(aFunc, aBaseFunc, aScalarType)   \
    template<                                                                  \
        uint taSize>                                                           \
    RTFACT_MATH_INLINE Packet<taSize, aScalarType> aFunc(                      \
        const Packet<taSize, aScalarType>& aPacket,                            \
        const aScalarType aValue)                                              \
    {                                                                          \
        Packet<taSize, aScalarType> ret;                                       \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)                            \
        {                                                                      \
            ret(i)[0] = aBaseFunc(aPacket(i)[0], aValue);                      \
            ret(i)[1] = aBaseFunc(aPacket(i)[1], aValue);                      \
            ret(i)[2] = aBaseFunc(aPacket(i)[2], aValue);                      \
            ret(i)[3] = aBaseFunc(aPacket(i)[3], aValue);                      \
        }                                                                      \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    template<                                                                  \
        uint taSize>                                                           \
    RTFACT_MATH_INLINE Packet<taSize, aScalarType> aFunc(                      \
        const aScalarType aValue,                                              \
        const Packet<taSize, aScalarType>& aPacket)                            \
    {                                                                          \
        Packet<taSize, aScalarType> ret;                                       \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)                            \
        {                                                                      \
            ret(i)[0] = aBaseFunc(aValue, aPacket(i)[0]);                      \
            ret(i)[1] = aBaseFunc(aValue, aPacket(i)[1]);                      \
            ret(i)[2] = aBaseFunc(aValue, aPacket(i)[2]);                      \
            ret(i)[3] = aBaseFunc(aValue, aPacket(i)[3]);                      \
        }                                                                      \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    template<>                                                                 \
    RTFACT_MATH_INLINE Packet<4, aScalarType> aFunc<4>(                        \
        const Packet<4, aScalarType>& aPacket,                                 \
        const aScalarType aValue)                                              \
    {                                                                          \
        return Packet<4, aScalarType>(Packet<4, aScalarType>::Container(       \
            aBaseFunc(aPacket[0], aValue),                                     \
            aBaseFunc(aPacket[1], aValue),                                     \
            aBaseFunc(aPacket[2], aValue),                                     \
            aBaseFunc(aPacket[3], aValue)));                                   \
    }                                                                          \
                                                                               \
    template<>                                                                 \
    RTFACT_MATH_INLINE Packet<4, aScalarType> aFunc<4>(                        \
        const aScalarType aValue,                                              \
        const Packet<4, aScalarType>& aPacket)                                 \
    {                                                                          \
        return Packet<4, aScalarType>(Packet<4, aScalarType>::Container(       \
            aBaseFunc(aValue, aPacket[0]),                                     \
            aBaseFunc(aValue, aPacket[1]),                                     \
            aBaseFunc(aValue, aPacket[2]),                                     \
            aBaseFunc(aValue, aPacket[3])));                                   \
    }                                                                          \
                                                                               \
    template<>                                                                 \
    RTFACT_MATH_INLINE Packet<1, aScalarType> aFunc<1>(                        \
        const Packet<1, aScalarType>& aPacket,                                 \
        const aScalarType aValue)                                              \
    {                                                                          \
        Packet<1, aScalarType> ret;                                            \
        ret.data = aBaseFunc(aPacket.data, aValue);                            \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    template<>                                                                 \
    RTFACT_MATH_INLINE Packet<1, aScalarType> aFunc<1>(                        \
        const aScalarType aValue,                                              \
        const Packet<1, aScalarType>& aPacket)                                 \
    {                                                                          \
        Packet<1, aScalarType> ret;                                            \
        ret.data = aBaseFunc(aValue, aPacket.data);                            \
        return ret;                                                            \
    }

#pragma endregion

#pragma region RTFACT__BINARY_FUNCTION_SSE_PACKET_SSE_PACKET macro definition

#define RTFACT__BINARY_FUNCTION_SSE_PACKET_SSE_PACKET(aFunc, aBaseFunc, aScalarType)\
    template<                                                                  \
        uint taSize>                                                           \
    RTFACT_MATH_INLINE Packet<taSize, aScalarType> aFunc(                      \
        const Packet<taSize, aScalarType>& aPacket1,                           \
        const Packet<taSize, aScalarType>& aPacket2)                           \
    {                                                                          \
        Packet<taSize, aScalarType> ret;                                       \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)                            \
        {                                                                      \
            ret(i).data = Packet<taSize, aScalarType>::Data::aBaseFunc(        \
                aPacket1(i).data, aPacket2(i).data);                           \
        }                                                                      \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    template<>                                                                 \
    RTFACT_MATH_INLINE Packet<4, aScalarType> aFunc<4>(                        \
        const Packet<4, aScalarType>& aPacket1,                                \
        const Packet<4, aScalarType>& aPacket2)                                \
    {                                                                          \
        return Packet<4, aScalarType>(Packet<4, aScalarType>::Data::aBaseFunc( \
            aPacket1.data, aPacket2.data));                                    \
    }

#pragma endregion

#pragma region RTFACT__BINARY_FUNCTION_PACKET_1_PACKET_1 macro definition

#define RTFACT__BINARY_FUNCTION_PACKET_1_PACKET_1(aFunc, aBaseFunc, aScalarType)\
    template<>                                                                 \
    RTFACT_MATH_INLINE Packet<1, aScalarType> aFunc<1>(                        \
        const Packet<1, aScalarType>& aPacket1,                                \
        const Packet<1, aScalarType>& aPacket2)                                \
    {                                                                          \
        Packet<1, aScalarType> ret;                                            \
        ret.data = aBaseFunc(aPacket1.data, aPacket2.data);                    \
        return ret;                                                            \
    }

#pragma endregion

#pragma endregion

#pragma region Unary packet function definition macro

#pragma region RTFACT__UNARY_FUNCTION_PACKET macro definition

#define RTFACT__UNARY_FUNCTION_PACKET(aFunc, aBaseFunc, aScalarType)           \
    template<                                                                  \
        uint taSize>                                                           \
    RTFACT_MATH_INLINE Packet<taSize, aScalarType> aFunc(                      \
        const Packet<taSize, aScalarType>& aPacket)                            \
    {                                                                          \
        Packet<taSize, aScalarType> ret;                                       \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)                            \
        {                                                                      \
            ret(i)[0] = aBaseFunc(aPacket(i)[0]);                              \
            ret(i)[1] = aBaseFunc(aPacket(i)[1]);                              \
            ret(i)[2] = aBaseFunc(aPacket(i)[2]);                              \
            ret(i)[3] = aBaseFunc(aPacket(i)[3]);                              \
        }                                                                      \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    template<>                                                                 \
    RTFACT_MATH_INLINE Packet<4, aScalarType> aFunc<4>(                        \
        const Packet<4, aScalarType>& aPacket)                                 \
    {                                                                          \
        return Packet<4, aScalarType>(                                         \
            Packet<4, aScalarType>::Container(aBaseFunc(aPacket[0]),           \
                                              aBaseFunc(aPacket[1]),           \
                                              aBaseFunc(aPacket[2]),           \
                                              aBaseFunc(aPacket[3])));         \
    }                                                                          \
                                                                               \
    RTFACT_MATH_INLINE Packet<1, aScalarType> aFunc(                           \
        const Packet<1, aScalarType> aPacket)                                  \
    {                                                                          \
        Packet<1, aScalarType> ret;                                            \
        ret.data = aBaseFunc(aPacket.data);                                    \
        return ret;                                                            \
    }

#pragma endregion

#pragma region RTFACT__UNARY_FUNCTION_SSE_PACKET macro definition

#define RTFACT__UNARY_FUNCTION_SSE_PACKET(aFunc, aBaseFunc, aScalarType)       \
    template<                                                                  \
        uint taSize>                                                           \
    RTFACT_MATH_INLINE Packet<taSize, aScalarType> aFunc(                      \
        const Packet<taSize, aScalarType>& aPacket)                            \
    {                                                                          \
        Packet<taSize, aScalarType> ret;                                       \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)                            \
        {                                                                      \
            ret(i).data = aPacket(i).data.aBaseFunc();                         \
        }                                                                      \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    template<>                                                                 \
    RTFACT_MATH_INLINE Packet<4, aScalarType> aFunc<4>(                        \
        const Packet<4, aScalarType>& aPacket)                                 \
    {                                                                          \
        return Packet<4, aScalarType>(aPacket.data.aBaseFunc());               \
    }

#pragma endregion

#pragma region RTFACT__UNARY_FUNCTION_PACKET_1 macro definition

#define RTFACT__UNARY_FUNCTION_PACKET_1(aFunc, aBaseFunc, aScalarType)    \
    template<>                                                                 \
    RTFACT_MATH_INLINE Packet<1, aScalarType> aFunc<1>(                        \
        const Packet<1, aScalarType>& aPacket)                                 \
    {                                                                          \
        Packet<1, aScalarType> ret;                                            \
        ret.data = aBaseFunc(aPacket.data);                                    \
        return ret;                                                            \
    }

#pragma endregion

#pragma region RTFACT__UNARY_FUNCTION_SSE_PACKET_TO_OTHER macro definition

#define RTFACT__UNARY_FUNCTION_SSE_PACKET_TO_OTHER(aFunc, aBaseFunc, aInType, aOutType)\
    template<                                                                  \
        uint taSize>                                                           \
    RTFACT_MATH_INLINE Packet<taSize, aOutType> aFunc(                         \
        const Packet<taSize, aInType>& aPacket)                                \
    {                                                                          \
        Packet<taSize, aOutType> ret;                                          \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)                            \
        {                                                                      \
            ret(i).data = aPacket(i).data.aBaseFunc();                         \
        }                                                                      \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    template<>                                                                 \
    RTFACT_MATH_INLINE Packet<4, aOutType> aFunc<4>(                           \
        const Packet<4, aInType>& aPacket)                                     \
    {                                                                          \
        return Packet<4, aOutType>(aPacket.data.aBaseFunc());                  \
    }

#pragma endregion

#pragma region RTFACT__UNARY_FUNCTION_PACKET_1_TO_OTHER macro definition

#define RTFACT__UNARY_FUNCTION_PACKET_1_TO_OTHER(aFunc, aBaseFunc, aInType, aOutType)\
    template<>                                                                 \
    RTFACT_MATH_INLINE Packet<1, aOutType> aFunc<1>(                           \
        const Packet<1, aInType>& aPacket)                                     \
    {                                                                          \
        Packet<1, aOutType> ret;                                               \
        ret.data = aBaseFunc(aPacket.data);                                    \
        return ret;                                                            \
    }

#pragma endregion

#pragma endregion

RTFACT__BINARY_FUNCTION_PACKET_PACKET(pow, ::pow, float)
RTFACT__BINARY_FUNCTION_PACKET_SCALAR(pow, ::pow, float)

RTFACT__UNARY_FUNCTION_PACKET(cos, ::cos, float)
RTFACT__UNARY_FUNCTION_PACKET(acos, ::acos, float)
RTFACT__UNARY_FUNCTION_PACKET(sin, ::sin, float)
RTFACT__UNARY_FUNCTION_PACKET(asin, ::asin, float)
RTFACT__UNARY_FUNCTION_PACKET(tan, ::tan, float)
RTFACT__UNARY_FUNCTION_PACKET(atan, ::atan, float)

RTFACT__UNARY_FUNCTION_SSE_PACKET(abs, abs, float)
RTFACT__UNARY_FUNCTION_PACKET_1  (abs, ::fabsf, float)

RTFACT__UNARY_FUNCTION_SSE_PACKET(sqrt, sqrt, float)
RTFACT__UNARY_FUNCTION_PACKET_1  (sqrt, ::sqrt, float)

RTFACT__UNARY_FUNCTION_SSE_PACKET(reciprocal, reciprocal, float)
RTFACT__UNARY_FUNCTION_PACKET_1  (reciprocal, reciprocal, float)

RTFACT__UNARY_FUNCTION_SSE_PACKET(recipSqrt, recipSqrt, float)
RTFACT__UNARY_FUNCTION_PACKET_1  (recipSqrt, recipSqrt, float)

RTFACT__UNARY_FUNCTION_SSE_PACKET_TO_OTHER(floor, floor, float, float)
RTFACT__UNARY_FUNCTION_PACKET_1_TO_OTHER  (floor, floor, float, float)

RTFACT__UNARY_FUNCTION_SSE_PACKET_TO_OTHER(floorToInt, floorToInt, float, int)
RTFACT__UNARY_FUNCTION_PACKET_1_TO_OTHER  (floorToInt, floorToInt, float, int)

RTFACT__UNARY_FUNCTION_SSE_PACKET_TO_OTHER(ceil, ceil, float, float)
RTFACT__UNARY_FUNCTION_PACKET_1_TO_OTHER  (ceil, ceil, float, float)

RTFACT__UNARY_FUNCTION_SSE_PACKET_TO_OTHER(ceilToInt, ceilToInt, float, int)
RTFACT__UNARY_FUNCTION_PACKET_1_TO_OTHER  (ceilToInt, ceilToInt, float, int)

RTFACT__UNARY_FUNCTION_SSE_PACKET_TO_OTHER(round, round, float, float)
RTFACT__UNARY_FUNCTION_PACKET_1_TO_OTHER  (round, round, float, float)

RTFACT__UNARY_FUNCTION_SSE_PACKET_TO_OTHER(roundToInt, roundToInt, float, int)
RTFACT__UNARY_FUNCTION_PACKET_1_TO_OTHER  (roundToInt, roundToInt, float, int)

RTFACT__UNARY_FUNCTION_SSE_PACKET_TO_OTHER(truncate, truncate, float, float)
RTFACT__UNARY_FUNCTION_PACKET_1_TO_OTHER  (truncate, truncate, float, float)

RTFACT__UNARY_FUNCTION_SSE_PACKET_TO_OTHER(truncateToInt, truncateToInt, float, int)
RTFACT__UNARY_FUNCTION_PACKET_1_TO_OTHER  (truncateToInt, truncateToInt, float, int)

RTFACT__UNARY_FUNCTION_SSE_PACKET_TO_OTHER(fraction, fraction, float, float)
RTFACT__UNARY_FUNCTION_PACKET_1_TO_OTHER  (fraction, fraction, float, float)

RTFACT__UNARY_FUNCTION_SSE_PACKET_TO_OTHER(toFloat, toFloat4, int, float)
RTFACT__UNARY_FUNCTION_PACKET_1_TO_OTHER  (toFloat, toFloat, int, float)

RTFACT__BINARY_FUNCTION_SSE_PACKET_SSE_PACKET(min, min, float)
RTFACT__BINARY_FUNCTION_SSE_PACKET_SSE_PACKET(max, max, float)
RTFACT__BINARY_FUNCTION_PACKET_1_PACKET_1(min, min, float)
RTFACT__BINARY_FUNCTION_PACKET_1_PACKET_1(max, max, float)

template<                                                                  
    uint taSize,
    class tScalar>                                                           
RTFACT_MATH_INLINE Packet<taSize, tScalar> atan2(                      
    const Packet<taSize, tScalar>& aPacket,
    const Packet<taSize, tScalar>& aOther)                            
{                                                                          
    Packet<taSize, tScalar> ret;                                       
    RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)                            
    {                    
        for (int k=0; k < Packet<taSize, tScalar>::SIZE; ++k)
        {
            ret(i)[k] = atan2f(aPacket(i)[k], aOther(i)[k]);
            ret(i)[k] = ret(i)[k] < 0.0f ? ret(i)[k] + RTFACT_2_PI : ret(i)[k];
        }
    }                                                                      
    return ret;                                                            
} 

template<
    uint taSize,
    class tScalar>
RTFACT_MATH_INLINE const Packet<taSize, tScalar> sqr(
    const Packet<taSize, tScalar>& aPacket)
{
    return aPacket * aPacket;
}

template<
    uint taSize>
RTFACT_MATH_INLINE const Packet<taSize, float> clamp(
    const Packet<taSize, float>& aValue,
    const Packet<taSize, float>& aMin,
    const Packet<taSize, float>& aMax)
{
    return min(max(aValue, aMin), aMax);
}

template<
    uint taSize>
RTFACT_MATH_INLINE const Packet<taSize, float> smoothStep(
    const Packet<taSize, float>& aValue,
    const Packet<taSize, float>& aMin,
    const Packet<taSize, float>& aMax)
{
    typedef Packet<taSize, float>        t_Packet;
    typedef typename t_Packet::Container t_PacketC;

    t_Packet ret;

    RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)
    {
        const t_PacketC v = clamp(
            (aValue(i) - aMin(i)) * reciprocal(aMax(i) - aMin(i)),
            t_PacketC::C_0(),
            t_PacketC::C_1);

        ret(i) = v * v * (t_PacketC::C_3 - 
                          t_PacketC::C_2 * v);
    }

    return ret;
}

RTFACT_MATH_INLINE const FPacketC interpolate(
    const FPacketC& aPosition, 
    const FPacketC& aValue1,
    const FPacketC& aValue2)
{
    return (1.f - aPosition) * aValue1 + aPosition * aValue2;
}

template<
    uint taSize>
RTFACT_MATH_INLINE const Packet<taSize, float> interpolate(
    const Packet<taSize, float>& aPosition, 
    const Packet<taSize, float>& aValue1,
    const Packet<taSize, float>& aValue2)
{
    return (1.f - aPosition) * aValue1 + aPosition * aValue2;
}


} // namespace Math

RTFACT_NAMESPACE_END

#pragma region Undefine the helper macros

#undef RTFACT__BINARY_FUNCTION_PACKET_PACKET
#undef RTFACT__BINARY_FUNCTION_PACKET_SCALAR
#undef RTFACT__BINARY_FUNCTION_SSE_PACKET_SSE_PACKET
#undef RTFACT__BINARY_FUNCTION_PACKET_1_PACKET_1

#undef RTFACT__UNARY_FUNCTION_PACKET
#undef RTFACT__UNARY_FUNCTION_SSE_PACKET
#undef RTFACT__UNARY_FUNCTION_PACKET_1

#pragma endregion

#endif // RTFACT__PACKETMATHUTILS_HPP
