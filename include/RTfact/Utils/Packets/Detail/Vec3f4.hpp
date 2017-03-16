/*
 *  Vec3f4.hpp
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

 #ifndef RTFACT__QUADVEC3F_HPP
#define RTFACT__QUADVEC3F_HPP

RTFACT_NAMESPACE_BEGIN

#pragma region Operator overloading macros

#define RTFACT__VEC3F_4_ARITH_OP(aOperator)                                    \
    RTFACT_VEC3F_INLINE const t_This operator aOperator(                       \
        const t_This& aVec) const                                              \
    {                                                                          \
        return t_This(x aOperator aVec.x,                                      \
                      y aOperator aVec.y,                                      \
                      z aOperator aVec.z);                                     \
    }                                                                          \
                                                                               \
    RTFACT_VEC3F_INLINE const t_This operator aOperator(                       \
        const Vec3f<1>& aVec) const                                            \
    {                                                                          \
        return t_This(x aOperator aVec.x,                                      \
                      y aOperator aVec.y,                                      \
                      z aOperator aVec.z);                                     \
    }                                                                          \
                                                                               \
    RTFACT_VEC3F_INLINE t_This& operator aOperator##=(                         \
        const t_This& aVec)                                                    \
    {                                                                          \
        x aOperator##= aVec.x;                                                 \
        y aOperator##= aVec.y;                                                 \
        z aOperator##= aVec.z;                                                 \
        return *this;                                                          \
    }                                                                          \
                                                                               \
    RTFACT_VEC3F_INLINE t_This& operator aOperator##=(                         \
        const Vec3f<1>& aVec)                                                  \
    {                                                                          \
        x aOperator##= aVec.x;                                                 \
        y aOperator##= aVec.y;                                                 \
        z aOperator##= aVec.z;                                                 \
        return *this;                                                          \
    }

#define RTFACT__VEC3F_4_ARITH_OP_VEC3F_GENERIC__DECLARE(aOperator)             \
    template<                                                                  \
        uint taSize>                                                           \
    RTFACT_VEC3F_INLINE const Vec3f<taSize> operator aOperator(                \
    const Vec3f<taSize>& aVec) const;

#define RTFACT__VEC3F_4_ARITH_OP_VEC3F_GENERIC__DEFINE(aOperator)              \
    template<                                                                  \
        uint taSize>                                                           \
    RTFACT_VEC3F_INLINE const Vec3f<taSize> Vec3f<4>::operator aOperator(      \
        const Vec3f<taSize>& aVec) const                                       \
    {                                                                          \
        Vec3f<taSize> ret;                                                     \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)                            \
        {                                                                      \
            ret(i).x = x aOperator aVec(i).x;                                  \
            ret(i).y = y aOperator aVec(i).y;                                  \
            ret(i).z = z aOperator aVec(i).z;                                  \
        }                                                                      \
        return ret;                                                            \
    }

#define RTFACT__VEC3F_4_UNARY_ARITH_OP(aOperator)                              \
    RTFACT_VEC3F_INLINE const t_This operator aOperator() const                \
    {                                                                          \
        return t_This(aOperator x,                                             \
                      aOperator y,                                             \
                      aOperator z);                                            \
    }

#define RTFACT__VEC3F_4_COMPARISON_OP(aOperator)                               \
    RTFACT_VEC3F_INLINE const Mask<4> operator aOperator(                      \
        const t_This& aVec) const                                              \
    {                                                                          \
        return (x aOperator aVec.x) &                                          \
               (y aOperator aVec.y) &                                          \
               (z aOperator aVec.z);                                           \
    }                                                                          \
                                                                               \
    RTFACT_VEC3F_INLINE const Mask<4> operator aOperator(                      \
        const Vec3f<1>& aVec) const                                            \
    {                                                                          \
        return (x aOperator aVec.x) &                                          \
               (y aOperator aVec.y) &                                          \
               (z aOperator aVec.z);                                           \
    }

#define RTFACT__VEC3F_4_COMPARISON_NE_OP                                       \
    RTFACT_VEC3F_INLINE const Mask<4> operator!=(                              \
        const t_This& aVec) const                                              \
    {                                                                          \
        return (x != aVec.x) | (y != aVec.y) | (z != aVec.z);                  \
    }

#define RTFACT__VEC3F_4_COMPARISON_OP_VEC3F_GENERIC__DECLARE(aOperator)        \
    template<                                                                  \
        uint taSize>                                                           \
    RTFACT_VEC3F_INLINE const Mask<taSize> operator aOperator(                 \
        const Vec3f<taSize>& aVec) const;

#define RTFACT__VEC3F_4_COMPARISON_OP_VEC3F_GENERIC__DEFINE(aOperator)         \
    template<                                                                  \
        uint taSize>                                                           \
    RTFACT_VEC3F_INLINE const Mask<taSize> Vec3f<4>::operator aOperator(       \
        const Vec3f<taSize>& aVec) const                                       \
    {                                                                          \
        Mask<taSize> ret;                                                      \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)                            \
        {                                                                      \
            ret(i) = operator aOperator(aVec(i));                              \
        }                                                                      \
        return ret;                                                            \
    }

#define RTFACT__VEC3F_4_ARITH_OP_SCALAR(aOperator)                             \
    RTFACT_VEC3F_INLINE const t_This operator aOperator(                       \
        const float aValue) const                                              \
    {                                                                          \
        const FloatPacket tempPacket = FloatPacket::replicate(aValue);         \
        return t_This(x aOperator tempPacket,                                  \
                      y aOperator tempPacket,                                  \
                      z aOperator tempPacket);                                 \
    }                                                                          \
                                                                               \
    RTFACT_VEC3F_INLINE t_This& operator aOperator##=(                         \
        const float aValue)                                                    \
    {                                                                          \
        const FloatPacket tempPacket = FloatPacket::replicate(aValue);         \
        x aOperator##= tempPacket;                                             \
        y aOperator##= tempPacket;                                             \
        z aOperator##= tempPacket;                                             \
        return *this;                                                          \
}

#define RTFACT__VEC3F_4_ARITH_OP_PACKET(aOperator)                             \
    RTFACT_VEC3F_INLINE const t_This operator aOperator(                       \
        const FloatPacket& aPacket) const                                      \
    {                                                                          \
        return t_This(x aOperator aPacket,                                     \
                      y aOperator aPacket,                                     \
                      z aOperator aPacket);                                    \
    }                                                                          \
                                                                               \
    RTFACT_VEC3F_INLINE const t_This operator aOperator(                       \
        const Packet<1, float>& aPacket) const                                 \
    {                                                                          \
        const FloatPacket tempPacket = FloatPacket::replicate(aPacket);        \
        return t_This(x aOperator tempPacket,                                  \
                      y aOperator tempPacket,                                  \
                      z aOperator tempPacket);                                 \
    }                                                                          \
                                                                               \
    RTFACT_VEC3F_INLINE t_This& operator aOperator##=(                         \
        const FloatPacket& aPacket)                                            \
    {                                                                          \
        x aOperator##= aPacket;                                                \
        y aOperator##= aPacket;                                                \
        z aOperator##= aPacket;                                                \
        return *this;                                                          \
    }                                                                          \
                                                                               \
    RTFACT_VEC3F_INLINE t_This& operator aOperator##=(                         \
        const Packet<1, float>& aPacket)                                       \
    {                                                                          \
        const FloatPacket tempPacket = FloatPacket::replicate(aPacket);        \
        x aOperator##= tempPacket;                                             \
        y aOperator##= tempPacket;                                             \
        z aOperator##= tempPacket;                                             \
        return *this;                                                          \
    }

#define RTFACT__VEC3F_4_ARITH_OP_PACKET_GENERIC__DECLARE(aOperator)            \
    template<                                                                  \
        uint taSize>                                                           \
    RTFACT_VEC3F_INLINE const Vec3f<taSize> operator aOperator(                \
        const Packet<taSize, float>& aPacket) const;

#define RTFACT__VEC3F_4_ARITH_OP_PACKET_GENERIC__DEFINE(aOperator)             \
    template<                                                                  \
        uint taSize>                                                           \
    RTFACT_VEC3F_INLINE const Vec3f<taSize> Vec3f<4>::operator aOperator(      \
        const Packet<taSize, float>& aPacket) const                            \
    {                                                                          \
        Vec3f<taSize> ret;                                                     \
        RTFACT_PACKET_FOR_ALL_CONTAINERS(i, taSize)                            \
        {                                                                      \
            ret(i).x = x aOperator aPacket(i);                                 \
            ret(i).y = y aOperator aPacket(i);                                 \
            ret(i).z = z aOperator aPacket(i);                                 \
        }                                                                      \
        return ret;                                                            \
    }

#define RTFACT__SCALAR_ARITH_OP_VEC3F_4(aOperator)                             \
    RTFACT_VEC3F_INLINE const Vec3f<4> operator aOperator(                     \
        const float aValue,                                                    \
        const Vec3f<4>& aVec)                                                  \
    {                                                                          \
        const Vec3f<4>::FloatPacket tempPacket =                               \
            Vec3f<4>::FloatPacket::replicate(aValue);                          \
        return Vec3f<4>(tempPacket aOperator aVec.x,                           \
                        tempPacket aOperator aVec.y,                           \
                        tempPacket aOperator aVec.z);                          \
    }

#pragma endregion

template<>
class Vec3f<4>
{
public:

    RTFACT_SIMD_ALIGNED_NEW_OPERATORS

private:

    typedef Vec3f<4> t_This;

public:

    enum
    {
        SIZE = 4,
        SIZE_SQRT = 2,
        CONTAINER_COUNT = 1
    };

    typedef Packet<4, float> FloatPacket;
    typedef Packet<4, int>   IntPacket;
    typedef float4           Data;
    typedef t_This           Container;

    FloatPacket x;
    FloatPacket y;
    FloatPacket z;

#pragma region Constants

    RTFACT_VEC3F_DECLARE_CONSTS

    static RTFACT_VEC3F_INLINE const t_This C_0_0_0()
    {
        t_This ret;

        ret.x = FloatPacket::C_0();
        ret.y = ret.x;
        ret.z = ret.x;

        return ret;
    }

#pragma endregion

#pragma region Constructors

    Vec3f()
    {}

    RTFACT_VEC3F_INLINE Vec3f(
        const FloatPacket& aX,
        const FloatPacket& aY,
        const FloatPacket& aZ
    ) :
        x(aX),
        y(aY),
        z(aZ)
    {}

private:

    RTFACT_VEC3F_INLINE Vec3f(
        const FloatPacket::Data& aX,
        const FloatPacket::Data& aY,
        const FloatPacket::Data& aZ
    ) :
        x(aX),
        y(aY),
        z(aZ)
    {}

public:

    #pragma endregion

    #pragma region Accessors

    #define RTFACT__GEN_COMPONENT_ACCESSORS(aComponent, aComponentCapital)     \
        RTFACT_VEC3F_INLINE const FloatPacket& get##aComponentCapital() const  \
        {                                                                      \
            return aComponent;                                                 \
        }                                                                      \
                                                                               \
        RTFACT_VEC3F_INLINE void set##aComponentCapital(                       \
            const FloatPacket& aPacket)                                        \
        {                                                                      \
            aComponent = aPacket;                                              \
        }                                                                      \
                                                                               \
        RTFACT_VEC3F_INLINE const float aComponent##At(                        \
            uint aIndex) const                                                 \
        {                                                                      \
            RTFACT_VEC3F_ASSERT(aIndex < 4);                                   \
                                                                               \
            return aComponent[aIndex];                                         \
        }                                                                      \
                                                                               \
        RTFACT_VEC3F_INLINE float& aComponent##At(                             \
            uint aIndex)                                                       \
        {                                                                      \
            RTFACT_VEC3F_ASSERT(aIndex < 4);                                   \
                                                                               \
            return aComponent[aIndex];                                         \
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
        RTFACT_VEC3F_ASSERT(aIndex < SIZE);

        return (*(&x + aDimension))[aIndex];
    }

    RTFACT_VEC3F_INLINE float& get(
        const uint aDimension,
        const uint aIndex)
    {
        RTFACT_VEC3F_ASSERT(aDimension < 3);
        RTFACT_VEC3F_ASSERT(aIndex < SIZE);

        return (*(&x + aDimension))[aIndex];
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

    RTFACT_VEC3F_INLINE const Vec3f<1> operator[](
        const uint aIndex) const
    {
        RTFACT_VEC3F_ASSERT(aIndex < 4);

        return Vec3f<1>(x[aIndex], y[aIndex], z[aIndex]);
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
        return (x > y).iif((x > z).iif(IntPacket::C_0(), IntPacket::C_2),
                           (y > z).iif(IntPacket::C_1, IntPacket::C_2));
    }

    RTFACT_VEC3F_INLINE const FloatPacket min() const
    {
        return Math::min(Math::min(x, y), z);
    }

    RTFACT_VEC3F_INLINE const FloatPacket max() const
    {
        return Math::max(Math::max(x, y), z);
    }

    static RTFACT_VEC3F_INLINE const t_This min(
        const t_This& aVec1,
        const t_This& aVec2)
    {
        return t_This(Math::min(aVec1.x, aVec2.x),
                      Math::min(aVec1.y, aVec2.y),
                      Math::min(aVec1.z, aVec2.z));
    }

    static RTFACT_VEC3F_INLINE const t_This min(
        const Vec3f<1>& aVec1,
        const t_This& aVec2)
    {
        return t_This(Math::min(aVec2.x, FloatPacket::replicate(aVec1.x)),
                      Math::min(aVec2.y, FloatPacket::replicate(aVec1.y)),
                      Math::min(aVec2.z, FloatPacket::replicate(aVec1.z)));
    }

    static RTFACT_VEC3F_INLINE const t_This min(
        const t_This& aVec1,
        const Vec3f<1>& aVec2)
    {
        return min(aVec2, aVec1);
    }

    static RTFACT_VEC3F_INLINE const t_This max(
        const t_This& aVec1,
        const t_This& aVec2)
    {
        return t_This(Math::max(aVec1.x, aVec2.x),
                      Math::max(aVec1.y, aVec2.y),
                      Math::max(aVec1.z, aVec2.z));
    }

    static RTFACT_VEC3F_INLINE const t_This max(
        const Vec3f<1>& aVec1,
        const t_This& aVec2)
    {
        return t_This(Math::max(aVec2.x, FloatPacket::replicate(aVec1.x)),
                      Math::max(aVec2.y, FloatPacket::replicate(aVec1.y)),
                      Math::max(aVec2.z, FloatPacket::replicate(aVec1.z)));
    }

    static RTFACT_VEC3F_INLINE const t_This max(
        const t_This& aVec1,
        const Vec3f<1>& aVec2)
    {
        return max(aVec2, aVec1);
    }

    RTFACT_VEC3F_INLINE void setMin(
        const t_This& aVec)
    {
        x = Math::min(x, aVec.x);
        y = Math::min(y, aVec.y);
        z = Math::min(z, aVec.z);
    }

    RTFACT_VEC3F_INLINE void setMin(
        const Vec3f<1>& aVec)
    {
        x = Math::min(x, FloatPacket::replicate(aVec.x));
        y = Math::min(y, FloatPacket::replicate(aVec.y));
        z = Math::min(z, FloatPacket::replicate(aVec.z));
    }

    RTFACT_VEC3F_INLINE void setMax(
        const t_This& aVec)
    {
        x = Math::max(x, aVec.x);
        y = Math::max(y, aVec.y);
        z = Math::max(z, aVec.z);
    }

    RTFACT_VEC3F_INLINE void setMax(
        const Vec3f<1>& aVec)
    {
        x = Math::max(x, FloatPacket::replicate(aVec.x));
        y = Math::max(y, FloatPacket::replicate(aVec.y));
        z = Math::max(z, FloatPacket::replicate(aVec.z));
    }

    #pragma endregion

    #pragma region Miscellaneous

    RTFACT_VEC3F_INLINE const FloatPacket length() const
    {
        return Math::sqrt(x * x + y * y + z * z);
    }

    RTFACT_VEC3F_INLINE const FloatPacket lengthSqr() const
    {
        return x * x + y * y + z * z;
    }

    RTFACT_VEC3F_INLINE const t_This normalize() const
    {
        FloatPacket norm = Math::recipSqrt(x * x + y * y + z * z);

        return t_This(x * norm, y * norm, z * norm);
    }

    RTFACT_VEC3F_INLINE void setNormalized()
    {
        FloatPacket norm = Math::recipSqrt(x * x + y * y + z * z);

        x *= norm;
        y *= norm;
        z *= norm;
    }

    RTFACT_VEC3F_INLINE const FloatPacket average() const
    {
        return (x + y + z) * FloatPacket::C_ONE_THIRD;
    }

    RTFACT_VEC3F_INLINE const FloatPacket luminance() const
    {
        return dot(*this, C_LUMINANCE_FACTORS);
    }

    RTFACT_VEC3F_INLINE const t_This reciprocal() const
    {
        return t_This(Math::reciprocal(x),
                      Math::reciprocal(y),
                      Math::reciprocal(z));
    }

    RTFACT_VEC3F_INLINE const FloatPacket addVertical() const
    {
        return x + y + z;
    }

    RTFACT_VEC3F_INLINE const Vec3f<1> addHoriz() const
    {
        return Vec3f<1>(x.addHoriz(), y.addHoriz(), z.addHoriz());
    }

    template<
        uint ta0,
        uint ta1,
        uint ta2,
        uint ta3>
    RTFACT_VEC3F_INLINE const t_This shuffle() const
    {
        return t_This(x.shuffle<ta0, ta1, ta2, ta3>(),
                      y.shuffle<ta0, ta1, ta2, ta3>(),
                      z.shuffle<ta0, ta1, ta2, ta3>());
    }

    #pragma endregion

    #pragma region Dot product

    #define RTFACT__DOT(aVec1, aVec2)                                          \
        aVec1.x * aVec2.x + aVec1.y * aVec2.y + aVec1.z * aVec2.z

    static RTFACT_VEC3F_INLINE const FloatPacket dot(
        const t_This& aVec1,
        const t_This& aVec2)
    {
        return RTFACT__DOT(aVec1, aVec2);
    }

    static RTFACT_VEC3F_INLINE const FloatPacket dot(
        const Vec3f<1>& aVec1,
        const t_This& aVec2)
    {
        return RTFACT__DOT(replicate(aVec1), aVec2);
    }

    static RTFACT_VEC3F_INLINE const FloatPacket dot(
        const t_This& aVec1,
        const Vec3f<1>& aVec2)
    {
        return RTFACT__DOT(aVec1, replicate(aVec2));
    }

    #undef RTFACT__DOT

    #pragma endregion

    #pragma region Product

    #define RTFACT__PRODUCT(aVec1, aVec2)                                      \
        aVec1.x * aVec2.x, aVec1.y * aVec2.y, aVec1.z * aVec2.z

    static RTFACT_VEC3F_INLINE t_This product(
        const t_This& aVec1,
        const t_This& aVec2)
    {
        return t_This(RTFACT__PRODUCT(aVec1, aVec2));
    }

    static RTFACT_VEC3F_INLINE t_This product(
        const Vec3f<1>& aVec1,
        const t_This& aVec2)
    {
        return t_This(RTFACT__PRODUCT(replicate(aVec1), aVec2));
    }

    static RTFACT_VEC3F_INLINE t_This product(
        const t_This& aVec1,
        const Vec3f<1>& aVec2)
    {
        return t_This(RTFACT__PRODUCT(aVec1, replicate(aVec2)));
    }

    #undef RTFACT__PRODUCT

    #pragma endregion

    #pragma region Cross product

    #define RTFACT__CROSS(aVec1, aVec2)                                        \
        aVec1.y * aVec2.z - aVec1.z * aVec2.y,                                 \
        aVec1.z * aVec2.x - aVec1.x * aVec2.z,                                 \
        aVec1.x * aVec2.y - aVec1.y * aVec2.x

    static RTFACT_VEC3F_INLINE t_This cross(
        const t_This& aVec1,
        const t_This& aVec2)
    {
        return t_This(RTFACT__CROSS(aVec1, aVec2));
    }

    static RTFACT_VEC3F_INLINE t_This cross(
        const Vec3f<1>& aVec1,
        const t_This& aVec2)
    {
        return t_This(RTFACT__CROSS(replicate(aVec1), aVec2));
    }

    static RTFACT_VEC3F_INLINE t_This cross(
        const t_This& aVec1,
        const Vec3f<1>& aVec2)
    {
        return t_This(RTFACT__CROSS(aVec1, replicate(aVec2)));
    }

    #undef RTFACT__CROSS

    #pragma endregion

    #pragma endregion

    #pragma region Replicate methods

    static RTFACT_VEC3F_INLINE const t_This replicate(
        const float aX,
        const float aY,
        const float aZ)
    {
        return t_This(float4::replicate(aX),
                      float4::replicate(aY),
                      float4::replicate(aZ));
    }

    static RTFACT_VEC3F_INLINE const t_This replicate(
        const Vec3f<1>& aVec)
    {
        #if defined(RTFACT_COMPILER_MSVC) && !defined(RTFACT_COMPILER_INTEL)

            return t_This(aVec.data.shuffle<0, 0, 0, 0>(),
                          aVec.data.shuffle<1, 1, 1, 1>(),
                          aVec.data.shuffle<2, 2, 2, 2>());

        #else

            #ifndef RTFACT_COMPILER_INTEL

                #pragma message "Please check the fastest way for replication"

            #endif

            return t_This(float4::replicate(aVec.xf),
                          float4::replicate(aVec.yf),
                          float4::replicate(aVec.zf));

        #endif
    }

    static RTFACT_VEC3F_INLINE const t_This& replicate(
        const t_This& aVec)
    {
        return aVec;
    }

    #pragma endregion

    #pragma region Operators

    RTFACT__VEC3F_4_ARITH_OP(+)
    RTFACT__VEC3F_4_ARITH_OP(-)
    RTFACT__VEC3F_4_ARITH_OP(*)
    RTFACT__VEC3F_4_ARITH_OP(/)

    RTFACT__VEC3F_4_ARITH_OP_VEC3F_GENERIC__DECLARE(+)
    RTFACT__VEC3F_4_ARITH_OP_VEC3F_GENERIC__DECLARE(-)
    RTFACT__VEC3F_4_ARITH_OP_VEC3F_GENERIC__DECLARE(*)
    RTFACT__VEC3F_4_ARITH_OP_VEC3F_GENERIC__DECLARE(/)

    RTFACT__VEC3F_4_UNARY_ARITH_OP(-)

    RTFACT__VEC3F_4_COMPARISON_OP(>)
    RTFACT__VEC3F_4_COMPARISON_OP(>=)
    RTFACT__VEC3F_4_COMPARISON_OP(<)
    RTFACT__VEC3F_4_COMPARISON_OP(<=)
    RTFACT__VEC3F_4_COMPARISON_OP(==)

    RTFACT__VEC3F_4_COMPARISON_NE_OP

    RTFACT__VEC3F_4_COMPARISON_OP_VEC3F_GENERIC__DECLARE(>)
    RTFACT__VEC3F_4_COMPARISON_OP_VEC3F_GENERIC__DECLARE(>=)
    RTFACT__VEC3F_4_COMPARISON_OP_VEC3F_GENERIC__DECLARE(<)
    RTFACT__VEC3F_4_COMPARISON_OP_VEC3F_GENERIC__DECLARE(<=)
    RTFACT__VEC3F_4_COMPARISON_OP_VEC3F_GENERIC__DECLARE(==)
    RTFACT__VEC3F_4_COMPARISON_OP_VEC3F_GENERIC__DECLARE(!=)

    RTFACT__VEC3F_4_ARITH_OP_SCALAR(*)
    RTFACT__VEC3F_4_ARITH_OP_SCALAR(/)

    RTFACT__VEC3F_4_ARITH_OP_PACKET(*)
    RTFACT__VEC3F_4_ARITH_OP_PACKET(/)

    RTFACT__VEC3F_4_ARITH_OP_PACKET_GENERIC__DECLARE(*)
    RTFACT__VEC3F_4_ARITH_OP_PACKET_GENERIC__DECLARE(/)

    #pragma endregion
};

#pragma region Operator definitions

RTFACT__SCALAR_ARITH_OP_VEC3F_4(*)
RTFACT__SCALAR_ARITH_OP_VEC3F_4(/)

#pragma endregion

#pragma region Undefine the helper macros

#undef RTFACT__VEC3F_4_ARITH_OP
#undef RTFACT__VEC3F_4_ARITH_OP_VEC3F_GENERIC__DECLARE
#undef RTFACT__VEC3F_4_UNARY_ARITH_OP
#undef RTFACT__VEC3F_4_COMPARISON_OP
#undef RTFACT__VEC3F_4_COMPARISON_OP_VEC3F_GENERIC__DECLARE
#undef RTFACT__VEC3F_4_ARITH_OP_SCALAR
#undef RTFACT__VEC3F_4_ARITH_OP_PACKET
#undef RTFACT__VEC3F_4_ARITH_OP_PACKET_GENERIC__DECLARE
#undef RTFACT__SCALAR_ARITH_OP_VEC3F_4

#pragma endregion

RTFACT_NAMESPACE_END

#endif // RTFACT__QUADVEC3F_HPP
