/* 
 *  Vec3f1.hpp
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
 *  Created on: 2008-11-03 01:12:24 +0100
 *  Author(s): Iliyan Georgiev, Georgiev Iliyan
 */
 
#ifndef RTFACT__SINGLEVEC3F_HPP
#define RTFACT__SINGLEVEC3F_HPP

#include <string>
#include <stdio.h>

#pragma region Operator overloading macros

#define RTFACT__VEC3F_1_ARITH_OP(aOperator)                                    \
    RTFACT_VEC3F_INLINE const t_This operator aOperator(                       \
        const t_This& aVec) const                                              \
    {                                                                          \
        return t_This(data aOperator aVec.data);                               \
    }                                                                          \
                                                                               \
    RTFACT_VEC3F_INLINE t_This& operator aOperator##=(                         \
        const t_This& aVec)                                                    \
    {                                                                          \
        data aOperator##= aVec.data;                                           \
        return *this;                                                          \
    }

#define RTFACT__VEC3F_1_ARITH_OP_VEC3F_GENERIC_4__DECLARE(aOperator)           \
    template<                                                                  \
        uint taSize>                                                           \
    RTFACT_VEC3F_INLINE const Vec3f<taSize> operator aOperator(                \
        const Vec3f<taSize>& aVec) const;                                      \
    RTFACT_VEC3F_INLINE const Vec3f<4> operator aOperator(                     \
        const Vec3f<4>& aVec) const;

#define RTFACT__VEC3F_1_ARITH_OP_VEC3F_GENERIC__DEFINE(aOperator)              \
    template<                                                                  \
        uint taSize>                                                           \
    RTFACT_VEC3F_INLINE const Vec3f<taSize> Vec3f<1>::operator aOperator(      \
        const Vec3f<taSize>& aVec) const                                       \
    {                                                                          \
        Vec3f<taSize> ret;                                                     \
        const typename Vec3f<4>::FloatPacket tempX =                           \
            Vec3f<4>::FloatPacket::replicate(xf);                              \
        const typename Vec3f<4>::FloatPacket tempY =                           \
            Vec3f<4>::FloatPacket::replicate(yf);                              \
        const typename Vec3f<4>::FloatPacket tempZ =                           \
            Vec3f<4>::FloatPacket::replicate(zf);                              \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)                            \
        {                                                                      \
            ret(i).x = tempX aOperator aVec(i).x;                              \
            ret(i).y = tempY aOperator aVec(i).y;                              \
            ret(i).z = tempZ aOperator aVec(i).z;                              \
        }                                                                      \
        return ret;                                                            \
    }

#define RTFACT__VEC3F_1_ARITH_OP_VEC3F_4__DEFINE(aOperator)                    \
    RTFACT_VEC3F_INLINE const Vec3f<4> Vec3f<1>::operator aOperator(           \
        const Vec3f<4>& aVec) const                                            \
    {                                                                          \
        Vec3f<4> ret;                                                          \
        ret.x = x aOperator aVec.x;                                            \
        ret.y = y aOperator aVec.y;                                            \
        ret.z = z aOperator aVec.z;                                            \
        return ret;                                                            \
    }

#define RTFACT__VEC3F_1_UNARY_ARITH_OP(aOperator)                              \
    RTFACT_VEC3F_INLINE const t_This operator aOperator() const                \
    {                                                                          \
        return t_This( aOperator data);                                        \
    }

#define RTFACT__VEC3F_1_COMPARISON_OP(aOperator)                               \
    RTFACT_VEC3F_INLINE const Mask<1> operator aOperator(                      \
        const t_This& aVec) const                                              \
    {                                                                          \
        return Mask<1>(                                                        \
            ((data aOperator aVec.data).getI32Mask() &                         \
                Mask<4>::Data::C_0_T_T_T_I32_MASK) ==                          \
            Mask<4>::Data::C_0_T_T_T_I32_MASK);                                \
    }

#define RTFACT__VEC3F_1_COMPARISON_NE_OP                                       \
    RTFACT_VEC3F_INLINE const Mask<1> operator!=(                              \
        const t_This& aVec) const                                              \
    {                                                                          \
        return Mask<1>((data == aVec.data).getI32Mask() !=                     \
                             Mask<4>::Data::C_T_T_T_T_I32_MASK);               \
    }

#define RTFACT__VEC3F_1_COMPARISON_OP_VEC3F_GENERIC_4__DECLARE(aOperator)      \
    template<                                                                  \
        uint taSize>                                                           \
    RTFACT_VEC3F_INLINE const Mask<taSize> operator aOperator(                 \
        const Vec3f<taSize>& aVec) const;                                      \
    RTFACT_VEC3F_INLINE const Mask<4> operator aOperator (                     \
        const Vec3f<4>& aVec) const;

#define RTFACT__VEC3F_1_COMPARISON_OP_VEC3F_GENERIC__DEFINE(aOperator)         \
    template<                                                                  \
        uint taSize>                                                           \
    RTFACT_VEC3F_INLINE const Mask<taSize> Vec3f<1>::operator aOperator(       \
        const Vec3f<taSize>& aVec) const                                       \
    {                                                                          \
        Mask<taSize> ret;                                                      \
        const typename Vec3f<4>::FloatPacket tempX =                           \
            Vec3f<4>::FloatPacket::replicate(xf);                              \
        const typename Vec3f<4>::FloatPacket tempY =                           \
            Vec3f<4>::FloatPacket::replicate(yf);                              \
        const typename Vec3f<4>::FloatPacket tempZ =                           \
            Vec3f<4>::FloatPacket::replicate(zf);                              \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)                            \
        {                                                                      \
            ret(i) =    (tempX aOperator aVec(i).x) &                          \
                        (tempY aOperator aVec(i).y) &                          \
                        (tempZ aOperator aVec(i).z);                           \
        }                                                                      \
        return ret;                                                            \
    }

#define RTFACT__VEC3F_1_COMPARISON_OP_VEC3F_4__DEFINE(aOperator)               \
    RTFACT_VEC3F_INLINE const Mask<4> Vec3f<1>::operator aOperator(            \
        const Vec3f<4>& aVec) const                                            \
    {                                                                          \
        return (x aOperator aVec.x) &                                          \
               (y aOperator aVec.y) &                                          \
               (z aOperator aVec.z);                                           \
    }

#define RTFACT__VEC3F_1_ARITH_OP_SCALAR(aOperator)                             \
    RTFACT_VEC3F_INLINE const t_This operator aOperator(                       \
        const float aValue) const                                              \
    {                                                                          \
        return t_This(data aOperator ContainerData::replicate(aValue));        \
    }                                                                          \
                                                                               \
    RTFACT_VEC3F_INLINE t_This& operator aOperator##=(                         \
        const float aValue)                                                    \
    {                                                                          \
        data aOperator##= ContainerData::replicate(aValue);                    \
        return *this;                                                          \
    }

#define RTFACT__VEC3F_1_ARITH_OP_PACKET(aOperator)                             \
    RTFACT_VEC3F_INLINE const t_This operator aOperator(                       \
        const FloatPacket& aPacket) const                                      \
    {                                                                          \
        return                                                                 \
            t_This(data aOperator ContainerData::replicate(aPacket));          \
    }                                                                          \
                                                                               \
    RTFACT_VEC3F_INLINE t_This& operator aOperator##=(                         \
        const FloatPacket& aPacket)                                            \
    {                                                                          \
        data aOperator##= ContainerData::replicate(aPacket);                   \
        return *this;                                                          \
    }

#define RTFACT__VEC3F_1_ARITH_OP_GENERAL_PACKET_4__DECLARE(aOperator)          \
    template<                                                                  \
        uint taSize>                                                           \
    RTFACT_VEC3F_INLINE const Vec3f<taSize> operator aOperator(                \
        const Packet<taSize, float>& aPacket) const;

#define RTFACT__VEC3F_1_ARITH_OP_GENERAL_PACKET_4__DEFINE(aOperator)           \
    template<                                                                  \
        uint taSize>                                                           \
    RTFACT_VEC3F_INLINE const Vec3f<taSize> Vec3f<1>::operator aOperator(      \
        const Packet<taSize, float>& aPacket) const                            \
    {                                                                          \
        typedef Vec3f<4>::FloatPacket t_Packet;                                \
        Vec3f<taSize> ret;                                                     \
        const t_Packet tempX = t_Packet::replicate(x);                         \
        const t_Packet tempY = t_Packet::replicate(y);                         \
        const t_Packet tempZ = t_Packet::replicate(z);                         \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)                            \
        {                                                                      \
            ret(i).x = tempX aOperator aPacket(i);                             \
            ret(i).y = tempY aOperator aPacket(i);                             \
            ret(i).z = tempZ aOperator aPacket(i);                             \
        }                                                                      \
        return ret;                                                            \
    }                                                                          \
                                                                               \
    template<>                                                                 \
    RTFACT_VEC3F_INLINE const Vec3f<4> Vec3f<1>::operator aOperator<4>(        \
        const Packet<4, float>& aPacket) const                                 \
    {                                                                          \
        Vec3f<4> ret;                                                          \
        ret.x = x aOperator aPacket;                                           \
        ret.y = y aOperator aPacket;                                           \
        ret.z = z aOperator aPacket;                                           \
        return ret;                                                            \
    }

#define RTFACT__SCALAR_ARITH_OP_VEC3F_1(aOperator)                             \
    RTFACT_VEC3F_INLINE const Vec3f<1> operator aOperator(                     \
        const float aValue,                                                    \
        const Vec3f<1>& aVec)                                                  \
    {                                                                          \
        return Vec3f<1>(Vec3f<1>::ContainerData::replicate(aValue) aOperator   \
                        aVec.data);                                            \
    }

#pragma endregion

RTFACT_NAMESPACE_BEGIN

template<>
class RTFACT_ALIGN_SIMD Vec3f<1>
{
public:
    
    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

private:

    typedef Vec3f<1> t_This;

public:

    enum
    {
        SIZE = 1,
        SIZE_SQRT = 1,
        CONTAINER_COUNT = 1
    };

    typedef Packet<1, float> FloatPacket;
    typedef Packet<1, int>   IntPacket;
    typedef float            Data;
    typedef t_This           Container;
    typedef float4           ContainerData;

    union
    {
        struct { ContainerData data; };
        struct { FloatPacket   x,  y,  z,  _dummy; };
        struct { float         xf, yf, zf, __dummy; };
    };

    #pragma region Constants

    RTFACT_VEC3F_DECLARE_CONSTS

    static RTFACT_VEC3F_INLINE const t_This C_0_0_0()
    {
        return t_This(ContainerData::C_0_0_0_0());
    }

    #pragma endregion

    #pragma region Constructors

    Vec3f()
    {}

    RTFACT_VEC3F_INLINE explicit Vec3f(
        const ContainerData& aData
    ) :
        data(aData)
    {}

    RTFACT_VEC3F_INLINE Vec3f(
        const float* aData
    ) :
        data(ContainerData::getInstance(aData[0], aData[1], aData[2], 0.f))
    {}

    RTFACT_VEC3F_INLINE Vec3f(
        const float aX,
        const float aY,
        const float aZ
    ) :
        data(ContainerData::getInstance(aX, aY, aZ, 0.f))
    {}

    RTFACT_VEC3F_INLINE Vec3f(
        const FloatPacket& aX,
        const FloatPacket& aY,
        const FloatPacket& aZ
    ) :
        data(ContainerData::getInstance(aX.data, aY.data, aZ.data, 0.f))
    {}

    #pragma endregion

    #pragma region Accessors

    #define RTFACT__GEN_COMPONENT_ACCESSORS(aComponent, aComponentCapital)     \
        RTFACT_VEC3F_INLINE const FloatPacket& get##aComponentCapital() const  \
        {                                                                      \
            return aComponent;                                                 \
        }                                                                      \
                                                                               \
        RTFACT_VEC3F_INLINE float get##aComponentCapital##f() const            \
        {                                                                      \
            return aComponent##f;                                              \
        }                                                                      \
                                                                               \
        RTFACT_VEC3F_INLINE void set##aComponentCapital(                       \
            const FloatPacket& aPacket)                                        \
        {                                                                      \
            aComponent = aPacket;                                              \
        }                                                                      \
                                                                               \
        RTFACT_VEC3F_INLINE void set##aComponentCapital##f(                    \
            const float aValue)                                                \
        {                                                                      \
            aComponent##f = aValue;                                            \
        }                                                                      \
                                                                               \
        RTFACT_VEC3F_INLINE const float aComponent##At(                        \
            uint aIndex) const                                                 \
        {                                                                      \
            RTFACT_VEC3F_ASSERT(aIndex == 0);                                  \
                                                                               \
            return aComponent##f;                                              \
        }                                                                      \
                                                                               \
        RTFACT_VEC3F_INLINE float& aComponent##At(                             \
            uint aIndex)                                                       \
        {                                                                      \
            RTFACT_VEC3F_ASSERT(aIndex == 0);                                  \
                                                                               \
            return aComponent##f;                                              \
        }

    RTFACT__GEN_COMPONENT_ACCESSORS(x, X)
    RTFACT__GEN_COMPONENT_ACCESSORS(y, Y)
    RTFACT__GEN_COMPONENT_ACCESSORS(z, Z)

    #undef RTFACT__GEN_COMPONENT_ACCESSORS

    RTFACT_VEC3F_INLINE const float& get(
        const uint aDimension,
        const uint aIndex) const
    {
        RTFACT_VEC3F_ASSERT(aDimension < 3);
        RTFACT_VEC3F_ASSERT(aIndex == 0);

        return *(&xf + aDimension);
    }

    RTFACT_VEC3F_INLINE float& get(
        const uint aDimension,
        const uint aIndex)
    {
        RTFACT_VEC3F_ASSERT(aDimension < 3);
        RTFACT_VEC3F_ASSERT(aIndex == 0);

        return *(&xf + aDimension);
    }

    RTFACT_VEC3F_INLINE const FloatPacket& get(
        const uint aIndex) const
    {
        RTFACT_VEC3F_ASSERT(aIndex < 3);

        return *(&x + aIndex);
    }

    RTFACT_VEC3F_INLINE FloatPacket& get(
        const uint aIndex)
    {
        RTFACT_VEC3F_ASSERT(aIndex < 3);

        return *(&x + aIndex);
    }

    RTFACT_VEC3F_INLINE void set(
        const uint aIndex,
        const FloatPacket& aPacket)
    {
        RTFACT_VEC3F_ASSERT(aIndex < 3);

        *(&x + aIndex) = aPacket;
    }

    RTFACT_VEC3F_INLINE const Vec3f<1>& operator[](
        const uint aIndex) const
    {
        RTFACT_VEC3F_ASSERT(aIndex == 0);

        return *this;
    }

    RTFACT_VEC3F_INLINE Vec3f<1>& operator[](
        const uint aIndex)
    {
        RTFACT_VEC3F_ASSERT(aIndex == 0);

        return *this;
    }

    RTFACT_VEC3F_INLINE const Container& operator()(
        const uint aIndex) const
    {
        RTFACT_VEC3F_ASSERT(aIndex == 0);

        return *this;
    }

    RTFACT_VEC3F_INLINE Container& operator()(
        const uint aIndex)
    {
        RTFACT_VEC3F_ASSERT(aIndex == 0);

        return *this;
    }

    #pragma endregion

    #pragma region Utility methods

    #pragma region Min/max

    RTFACT_VEC3F_INLINE const IntPacket maxDim() const
    {
        IntPacket ret;

        ret.data = ((xf > yf) ? ((xf > zf)? 0 : 2) : ((yf > zf) ? 1 : 2));
        
        return ret;
    }

    RTFACT_VEC3F_INLINE const FloatPacket min() const
    {
        FloatPacket ret;

        ret.data = data.min3();

        return ret;
    }

    RTFACT_VEC3F_INLINE const FloatPacket max() const
    {
        FloatPacket ret;

        ret.data = data.max3();

        return ret;
    }

    static RTFACT_VEC3F_INLINE const t_This min(
        const t_This& aVec1,
        const t_This& aVec2)
    {
        return t_This(ContainerData::min(aVec1.data, aVec2.data));
    }

    static RTFACT_VEC3F_INLINE const t_This max(
        const t_This& aVec1,
        const t_This& aVec2)
    {
        return t_This(ContainerData::max(aVec1.data, aVec2.data));
    }

    RTFACT_VEC3F_INLINE void setMin(
        const t_This& aVec)
    { 
        data = ContainerData::min(data, aVec.data);
    }

    RTFACT_VEC3F_INLINE void setMax(
        const t_This& aVec)
    { 
        data = ContainerData::max(data, aVec.data);
    }

    #pragma endregion

    #pragma region Miscellaneous

    RTFACT_VEC3F_INLINE const FloatPacket length() const
    {
        FloatPacket ret;

        ret.data = data.length3();

        return ret;
    }

    RTFACT_VEC3F_INLINE const FloatPacket lengthSqr() const
    {
        FloatPacket ret;

        ret.data = data.length3Sqr();

        return ret;
    }

    RTFACT_VEC3F_INLINE const t_This normalize() const
    {
        return t_This(data.normalize3());
    }

    RTFACT_VEC3F_INLINE void setNormalized()
    {
        data = data.normalize3();
    }

    RTFACT_VEC3F_INLINE const FloatPacket average() const
    {
        FloatPacket ret;

        ret.data = data.addHoriz()[0] * 0.33333334f;

        return ret;
    }

    RTFACT_VEC3F_INLINE const FloatPacket luminance() const
    {
        return dot(*this, C_LUMINANCE_FACTORS);
    }

    RTFACT_VEC3F_INLINE const t_This reciprocal() const
    {
        return t_This(data.reciprocal());
    }

    RTFACT_VEC3F_INLINE const FloatPacket addVertical() const
    {
        FloatPacket ret;

        ret.data = data.addHoriz().at<0>();

        return ret;
    }

    RTFACT_VEC3F_INLINE const t_This& addHoriz() const
    {
        return *this;
    }

    template<
        class tType>
    RTFACT_VEC3F_INLINE tType getExtraValue() const;

    template<
        class tType>
    RTFACT_VEC3F_INLINE void setExtraValue(
        const tType aValue);

    #pragma endregion

    #pragma region Dot product, product, and cross product

    static RTFACT_VEC3F_INLINE const FloatPacket dot(
        const t_This& aVec1,
        const t_This& aVec2)
    {
        FloatPacket ret;

        ret.data = float4::dot3(aVec1.data, aVec2.data);

        return ret;
    }

    static RTFACT_VEC3F_INLINE t_This product(
        const t_This& aVec1,
        const t_This& aVec2)
    {
        return t_This(aVec1.data * aVec2.data);
    }

    static RTFACT_VEC3F_INLINE t_This cross(
        const t_This& aVec1,
        const t_This& aVec2)
    {
        return t_This(float4::cross(aVec1.data, aVec2.data));
    }

    #pragma endregion

    #pragma endregion

    #pragma region Replicate methods

    static RTFACT_VEC3F_INLINE const t_This replicate(
        const float aX,
        const float aY,
        const float aZ)
    {
        return t_This(aX, aY, aZ);
    }

    static RTFACT_VEC3F_INLINE const t_This& replicate(
        const t_This& aVec)
    {
        return aVec;
    }

    #pragma endregion

    #pragma region Operators

    RTFACT__VEC3F_1_ARITH_OP(+)
    RTFACT__VEC3F_1_ARITH_OP(-)
    RTFACT__VEC3F_1_ARITH_OP(*)
    RTFACT__VEC3F_1_ARITH_OP(/)

    RTFACT__VEC3F_1_ARITH_OP_VEC3F_GENERIC_4__DECLARE(+)
    RTFACT__VEC3F_1_ARITH_OP_VEC3F_GENERIC_4__DECLARE(-)
    RTFACT__VEC3F_1_ARITH_OP_VEC3F_GENERIC_4__DECLARE(*)
    RTFACT__VEC3F_1_ARITH_OP_VEC3F_GENERIC_4__DECLARE(/)

    RTFACT__VEC3F_1_UNARY_ARITH_OP(-)

    RTFACT__VEC3F_1_COMPARISON_OP(>)
    RTFACT__VEC3F_1_COMPARISON_OP(>=)
    RTFACT__VEC3F_1_COMPARISON_OP(<)
    RTFACT__VEC3F_1_COMPARISON_OP(<=)
    RTFACT__VEC3F_1_COMPARISON_OP(==)

    RTFACT__VEC3F_1_COMPARISON_NE_OP

    RTFACT__VEC3F_1_COMPARISON_OP_VEC3F_GENERIC_4__DECLARE(>)
    RTFACT__VEC3F_1_COMPARISON_OP_VEC3F_GENERIC_4__DECLARE(>=)
    RTFACT__VEC3F_1_COMPARISON_OP_VEC3F_GENERIC_4__DECLARE(<)
    RTFACT__VEC3F_1_COMPARISON_OP_VEC3F_GENERIC_4__DECLARE(<=)
    RTFACT__VEC3F_1_COMPARISON_OP_VEC3F_GENERIC_4__DECLARE(==)
    RTFACT__VEC3F_1_COMPARISON_OP_VEC3F_GENERIC_4__DECLARE(!=)

    RTFACT__VEC3F_1_ARITH_OP_SCALAR(*)
    RTFACT__VEC3F_1_ARITH_OP_SCALAR(/)

    RTFACT__VEC3F_1_ARITH_OP_PACKET(*)
    RTFACT__VEC3F_1_ARITH_OP_PACKET(/)

    RTFACT__VEC3F_1_ARITH_OP_GENERAL_PACKET_4__DECLARE(*)
    RTFACT__VEC3F_1_ARITH_OP_GENERAL_PACKET_4__DECLARE(/)

    #pragma endregion

    #pragma region String parsing

    // temp hack
    static bool parseString(
        const std::string& aString,
        t_This& oParsedObject)
    {
        if(sscanf(aString.c_str(), "%f,%f,%f",
                  &oParsedObject.xf, &oParsedObject.yf, &oParsedObject.zf) == 3)
        {
            oParsedObject.__dummy = 0.f;

            return true;
        }
        else
        {
            return false;
        }
    }

    #pragma endregion
};

template<>
RTFACT_VEC3F_INLINE float Vec3f<1>::getExtraValue<float>() const
{
    return __dummy;
}

template<>
RTFACT_VEC3F_INLINE Packet<1, float>
Vec3f<1>::getExtraValue<Packet<1, float> >() const
{
    return _dummy;
}

template<>
RTFACT_VEC3F_INLINE int Vec3f<1>::getExtraValue<int>() const
{
    return *reinterpret_cast<const int*>(&__dummy) & 0x3FFFFFFF;
}

template<>
RTFACT_VEC3F_INLINE uint Vec3f<1>::getExtraValue<uint>() const
{
    return *reinterpret_cast<const uint*>(&__dummy) & 0x3FFFFFFF;
}

template<>
RTFACT_VEC3F_INLINE void Vec3f<1>::setExtraValue<float>(
    const float aValue)
{
    __dummy = aValue;
}

template<>
RTFACT_VEC3F_INLINE void Vec3f<1>::setExtraValue<Packet<1, float> >(
    const Packet<1, float> aValue)
{
    _dummy = aValue;
}

template<>
RTFACT_VEC3F_INLINE void Vec3f<1>::setExtraValue<int>(
    const int aValue)
{
    RTFACT_VEC3F_ASSERT((aValue & 0xC0000000) == 0);

    *reinterpret_cast<int*>(&__dummy) = aValue | 0xC0000000;
}

template<>
RTFACT_VEC3F_INLINE void Vec3f<1>::setExtraValue<uint>(
    const uint aValue)
{
    RTFACT_VEC3F_ASSERT((aValue & 0xC0000000) == 0);

    *reinterpret_cast<uint*>(&__dummy) = aValue | 0xC0000000;
}

#pragma region Operator definitions

RTFACT__SCALAR_ARITH_OP_VEC3F_1(*)
RTFACT__SCALAR_ARITH_OP_VEC3F_1(/)

#pragma endregion

typedef Vec3f<1> Vec3f1;

RTFACT_NAMESPACE_END

#pragma region Undefine the helper macros

#undef RTFACT__VEC3F_1_ARITH_OP
#undef RTFACT__VEC3F_1_ARITH_OP_VEC3F_GENERIC_4__DECLARE
#undef RTFACT__VEC3F_1_UNARY_ARITH_OP
#undef RTFACT__VEC3F_1_COMPARISON_OP
#undef RTFACT__VEC3F_1_COMPARISON_OP_VEC3F_GENERIC_4__DECLARE
#undef RTFACT__VEC3F_1_ARITH_OP_SCALAR
#undef RTFACT__VEC3F_1_ARITH_OP_PACKET
#undef RTFACT__VEC3F_1_ARITH_OP_GENERAL_PACKET_4__DECLARE
#undef RTFACT__SCALAR_ARITH_OP_VEC3F_1

#pragma endregion

#endif // RTFACT__SINGLEVEC3F_HPP
